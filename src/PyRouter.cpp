#include "PyRouter.hpp"

// https://github.com/include-what-you-use/include-what-you-use/issues/1616
// IWYU pragma: no_include <iterator>

#include <array>
#include <cstddef>
#include <new>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <Python.h>

#define Py_BUILD_CORE
#include <internal/pycore_modsupport.h>

#include "HTTPPerfectHash.hpp"
#include "HTTPRouter.hpp"
#include "ModNanoroute.hpp"
#include "Util.hpp"

namespace nanoroute {
namespace {

struct meth__ : PyMethodDef {
  meth__(const char* n, auto f)
      : PyMethodDef {n, reinterpret_cast<PyCFunction>(f), METH_FASTCALL} {};
  meth__() : PyMethodDef {} {};
};

std::array router_methods {
    meth__("get", PyRouter::get),
    meth__("post", PyRouter::post),
    meth__("put", PyRouter::put),
    meth__("delete", PyRouter::delete_),
    meth__("route", PyRouter::route),
    meth__("lookup", PyRouter::lookup),
    meth__("wsgi_app", PyRouter::wsgi_app),
    meth__(),
};

struct slot__ : PyType_Slot {
  slot__(int s, auto v) : PyType_Slot {s, reinterpret_cast<void*>(v)} {};
  slot__() : PyType_Slot {} {};
};

std::array router_slots {
    slot__(Py_tp_dealloc, PyRouter::dealloc),
    slot__(Py_tp_traverse, PyRouter::traverse),
    slot__(Py_tp_clear, PyRouter::clear),
    slot__(Py_tp_methods, router_methods.data()),
    slot__(Py_tp_init, PyRouter::init),
    slot__(Py_tp_new, PyRouter::new_),
    slot__(),
};

std::array cbclosure_slots {
    slot__(Py_tp_dealloc, PyCBClosure::dealloc),
    slot__(Py_tp_traverse, PyCBClosure::traverse),
    slot__(Py_tp_clear, PyCBClosure::clear),
    slot__(),
};

} // namespace

PyType_Spec router_spec {
    .name = "nanoroute.router",
    .basicsize = sizeof(PyRouter),
    .flags = (Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_IMMUTABLETYPE),
    .slots = router_slots.data(),
};

PyType_Spec cbclosure_spec {
    .name = "nanoroute.cbclosure",
    .basicsize = sizeof(PyCBClosure),
    .flags = (Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_IMMUTABLETYPE),
    .slots = cbclosure_slots.data(),
};

namespace {
template <std::size_t N> void type_error(const char (&str)[N], PyObject* obj) {
  PyObject* name {PyType_GetName(Py_TYPE(obj))};
  PyErr_Format(PyExc_TypeError, str, name);
  Py_DECREF(name);
}
} // namespace

PyCBClosure* PyCBClosure::internal_new(PyTypeObject* subtype,
    PyRouter* pyrouter, PyObject* route, std::vector<HTTPMethod> meths) {

  auto ptr {new(subtype->tp_alloc(subtype, sizeof(PyCBClosure)))
          PyCBClosure(pyrouter, route, std::move(meths))};

  if(!ptr)
    return ptr;

  Py_INCREF(pyrouter);
  Py_INCREF(route);
  return ptr;
}

int PyCBClosure::traverse(PyCBClosure* self, visitproc visit, void* arg) {
  Py_VISIT(self->pyrouter);
  return 0;
}

int PyCBClosure::clear(PyCBClosure* self) {
  Py_CLEAR(self->pyrouter);
  return 0;
}

void PyCBClosure::dealloc(PyCBClosure* self) {
  PyObject_GC_UnTrack(self);
  clear(self);
  auto tp {Py_TYPE(self)};
  self->~PyCBClosure();
  tp->tp_free(self);
  Py_DECREF(tp);
}

PyObject* PyRouter::register_route(PyCBClosure* self, PyObject* const* args,
    Py_ssize_t nargs) {
  PyObject* pyo;
  if(!_PyArg_ParseStack(args, nargs, "O:register_route", &pyo))
    return nullptr;

  Py_ssize_t sz;
  const char* c {PyUnicode_AsUTF8AndSize(self->route, &sz)};
  if(!c)
    return nullptr;
  std::string_view route {c, static_cast<std::size_t>(sz)};

  for(auto meth : self->meths) {
    Py_INCREF(pyo);
    try {
      self->pyrouter->httprouter_.reg_route(meth, route, pyo);
    } catch(const std::exception& e) {
      Py_DECREF(pyo);
      PyErr_SetString(PyExc_TypeError, e.what());
      return nullptr;
    }
  }

  Py_INCREF(pyo);
  return pyo;
}

PyObject* PyRouter::get(PyRouter* self, PyObject* const* args,
    Py_ssize_t nargs) {
  PyObject* route_def;
  if(!_PyArg_ParseStack(args, nargs, "O!:get", &PyUnicode_Type, &route_def))
    return nullptr;

  return self->route_(HTTPMethod::Get, route_def);
}

PyObject* PyRouter::post(PyRouter* self, PyObject* const* args,
    Py_ssize_t nargs) {
  PyObject* route_def;
  if(!_PyArg_ParseStack(args, nargs, "O!:post", &PyUnicode_Type, &route_def))
    return nullptr;

  return self->route_(HTTPMethod::Post, route_def);
}

PyObject* PyRouter::put(PyRouter* self, PyObject* const* args,
    Py_ssize_t nargs) {
  PyObject* route_def;
  if(!_PyArg_ParseStack(args, nargs, "O!:put", &PyUnicode_Type, &route_def))
    return nullptr;

  return self->route_(HTTPMethod::Put, route_def);
}

PyObject* PyRouter::delete_(PyRouter* self, PyObject* const* args,
    Py_ssize_t nargs) {
  PyObject* route_def;
  if(!_PyArg_ParseStack(args, nargs, "O!:delete", &PyUnicode_Type, &route_def))
    return nullptr;

  return self->route_(HTTPMethod::Delete, route_def);
}

PyObject* PyRouter::route(PyRouter* self, PyObject* const* args,
    Py_ssize_t nargs) {
  PyObject* meths;
  PyObject* route_def;
  if(!_PyArg_ParseStack(args, nargs, "OO!:route", &meths, &PyUnicode_Type,
         &route_def))
    return nullptr;

  if(PyUnicode_Check(meths)) {
    Py_ssize_t sz;
    const char* c {PyUnicode_AsUTF8AndSize(meths, &sz)};
    if(!c)
      return nullptr;
    auto hash {HTTPPerfectHash::in_word_set(c, static_cast<std::size_t>(sz))};
    if(hash == nullptr) {
      PyErr_Format(PyExc_TypeError, "%U is not a valid HTTP method", meths);
      return nullptr;
    }
    return self->route_(hash->meth, route_def);
  }

  std::vector<HTTPMethod> vec;
  auto iter {PyObject_GetIter(meths)};
  if(!iter)
    return nullptr;

  for(PyObject* next; (next = PyIter_Next(iter)); Py_DECREF(next)) {
    if(!PyUnicode_Check(next)) {
      type_error(
          "route() argument 1 must be str or iterator[str], not iterator[%U]",
          next);
      Py_DECREF(next);
      return nullptr;
    }

    Py_ssize_t sz;
    const char* c {PyUnicode_AsUTF8AndSize(next, &sz)};
    if(!c) {
      Py_DECREF(next);
      return nullptr;
    }

    auto hash {HTTPPerfectHash::in_word_set(c, static_cast<std::size_t>(sz))};
    if(hash == nullptr) {
      PyErr_Format(PyExc_TypeError, "%U is not a valid HTTP method", next);
      Py_DECREF(next);
      return nullptr;
    }

    vec.push_back(hash->meth);
  }

  return self->route_(std::move(vec), route_def);
}

namespace {
PyObject* make_cap_dict(
    std::pair<std::vector<std::string>*, std::vector<std::string_view>*>&
        caps) {
  auto& [keys, vals] = caps;
  auto dict {_PyDict_NewPresized(keys->size())};
  if(!dict)
    return nullptr;

  std::size_t i {0};
  for(auto& key : *keys) {
    if(key.empty()) {
      ++i;
      continue;
    }
    auto& val {(*vals)[i++]};

    PyObject* k {PyUnicode_FromStringAndSize(key.data(), key.size())};
    if(!k) {
      Py_DECREF(dict);
      return nullptr;
    }
    PyObject* v {PyUnicode_FromStringAndSize(val.data(), val.size())};
    if(!v) {
      Py_DECREF(k);
      Py_DECREF(dict);
      return nullptr;
    }
    int err {PyDict_SetItem(dict, k, v)};
    Py_DECREF(k);
    Py_DECREF(v);
    if(err) {
      Py_DECREF(dict);
      return nullptr;
    }
  }

  return dict;
}
} // namespace

PyObject* PyRouter::lookup(PyRouter* self, PyObject* const* args,
    Py_ssize_t nargs) {
  PyObject* pymeth;
  PyObject* pyroute;

  if(!_PyArg_ParseStack(args, nargs, "O!O!:lookup", &PyUnicode_Type, &pymeth,
         &PyUnicode_Type, &pyroute))
    return nullptr;

  Py_ssize_t sz;
  const char* c {PyUnicode_AsUTF8AndSize(pymeth, &sz)};
  auto hash {HTTPPerfectHash::in_word_set(c, static_cast<std::size_t>(sz))};
  if(hash == nullptr) {
    PyErr_Format(PyExc_TypeError, "%U is not a valid HTTP method", pymeth);
    return nullptr;
  }

  c = {PyUnicode_AsUTF8AndSize(pyroute, &sz)};
  if(!c)
    return nullptr;

  auto route_result {self->httprouter_.get_route(hash->meth,
      {c, static_cast<std::size_t>(sz)})};

  if(!route_result) {
    PyErr_SetString(PyExc_LookupError, "No such route");
    return nullptr;
  }

  auto dict {make_cap_dict(route_result->second)};
  params_q.push(route_result->second.second);
  if(!dict)
    return nullptr;

  auto tuple {PyTuple_Pack(2, route_result->first, dict)};
  Py_DECREF(dict);
  return tuple;
}

PyObject* PyRouter::wsgi_app(PyRouter* self, PyObject* const* args,
    Py_ssize_t nargs) {
  PyObject* ws_env;
  PyObject* start_response;

  if(!_PyArg_ParseStack(args, nargs, "O!O:wsgi_app", &PyDict_Type, &ws_env,
         &start_response))
    return nullptr;

  if(!PyCallable_Check(start_response)) {
    type_error("wsgi_app() argument 2 must be callable, not %U",
        start_response);
    return nullptr;
  }

  PyObject* meth {
      PyDict_GetItemWithError(ws_env, self->state_->req_meth_string)};
  if(!meth) {
    if(!PyErr_Occurred())
      PyErr_SetString(PyExc_TypeError,
          "environ does not contain 'REQUEST_METHOD'");
    return nullptr;
  }

  Py_ssize_t sz;
  const char* c {PyUnicode_AsUTF8AndSize(meth, &sz)};
  if(!c)
    return nullptr;
  auto hash {HTTPPerfectHash::in_word_set(c, static_cast<std::size_t>(sz))};
  if(hash == nullptr) {
    PyErr_Format(PyExc_TypeError, "%U is not a valid HTTP method", meth);
    return nullptr;
  }

  PyObject* path {
      PyDict_GetItemWithError(ws_env, self->state_->path_info_string)};
  if(!path) {
    if(!PyErr_Occurred())
      PyErr_SetString(PyExc_TypeError, "environ does not contain 'PATH_INFO'");
    return nullptr;
  }

  c = {PyUnicode_AsUTF8AndSize(path, &sz)};
  if(!c)
    return nullptr;

  auto route_result {self->httprouter_.get_route(hash->meth,
      {c, static_cast<std::size_t>(sz)})};

  if(!route_result) {
    PyErr_SetString(PyExc_LookupError, "No such route");
    return nullptr;
  }

  auto dict {make_cap_dict(route_result->second)};
  params_q.push(route_result->second.second);
  if(!dict)
    return nullptr;

  int err {PyDict_SetItem(ws_env, self->state_->wsgi_key_string, dict)};
  Py_DECREF(dict);
  if(err)
    return nullptr;

  return PyObject_CallFunctionObjArgs(route_result->first, ws_env,
      start_response, nullptr);
}

namespace {

PyMethodDef register_route_def {
    .ml_name = "nanoroute.register_route",
    .ml_meth = (PyCFunction) PyRouter::register_route,
    .ml_flags = METH_FASTCALL,
};

} // namespace

PyObject* PyRouter::route_(HTTPMethod meth, PyObject* route_def) {
  auto closure {PyCBClosure::internal_new(state_->CBClosureType, this,
      route_def, {meth})};
  if(!closure)
    return nullptr;

  auto f {PyCFunction_New(&register_route_def, closure)};
  if(!f) {
    Py_DECREF(closure);
    return nullptr;
  }

  return f;
}

PyObject* PyRouter::route_(std::vector<HTTPMethod> meth, PyObject* route_def) {
  auto closure {PyCBClosure::internal_new(state_->CBClosureType, this,
      route_def, std::move(meth))};
  if(!closure)
    return nullptr;

  auto f {PyCFunction_New(&register_route_def, closure)};
  if(!f) {
    Py_DECREF(closure);
    return nullptr;
  }

  return f;
}

PyRouter* PyRouter::new_(PyTypeObject* subtype, PyObject* /*args*/,
    PyObject* /*kwargs*/) {
  auto mod {PyType_GetModuleByDef(subtype, &nanoroute_mod)};
  if(!mod)
    return nullptr;

  auto pyrtr {new(subtype->tp_alloc(subtype, sizeof(PyRouter))) PyRouter};
  if(!pyrtr)
    return nullptr;

  Py_INCREF(mod);
  pyrtr->mod_ = mod;
  pyrtr->state_ = static_cast<NanorouteState*>(PyModule_GetState(mod));

  return pyrtr;
}

int PyRouter::init(PyRouter* self, PyObject* /*args*/, PyObject* /*kwargs*/) {
  self->httprouter_.traverse([&](PyObject* app) { Py_CLEAR(app); });
  self->httprouter_.clear();
  return 0;
}

int PyRouter::traverse(PyRouter* self, visitproc visit, void* arg) {
  Py_VISIT(Py_TYPE(self));
  Py_VISIT(self->mod_);
  return self->httprouter_
      .traverse(
          [&](PyObject* app) {
            Py_VISIT(app);
            return 0;
          },
          [](int v) { return v; })
      .value_or(0);
}

int PyRouter::clear(PyRouter* self) {
  Py_CLEAR(self->mod_);
  self->httprouter_.traverse([&](PyObject* app) { Py_CLEAR(app); });
  return 0;
}

void PyRouter::dealloc(PyRouter* self) {
  PyObject_GC_UnTrack(self);
  clear(self);
  auto tp {Py_TYPE(self)};
  self->~PyRouter();
  tp->tp_free(self);
  Py_DECREF(tp);
}

} // namespace nanoroute
