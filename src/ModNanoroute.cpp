#include "ModNanoroute.hpp"

#include <array>

#include <Python.h>

#include "PyRouter.hpp"

namespace nanoroute {
namespace {

int nrmod_traverse(PyObject* mod, visitproc visit, void* arg) {
  auto state {static_cast<NanorouteState*>(PyModule_GetState(mod))};
  Py_VISIT(state->RouterType);
  return 0;
}

int nrmod_clear(PyObject* mod) {
  auto state {static_cast<NanorouteState*>(PyModule_GetState(mod))};
  Py_CLEAR(state->RouterType);
  Py_XDECREF(state->req_meth_string);
  Py_XDECREF(state->path_info_string);
  Py_XDECREF(state->wsgi_key_string);
  return 0;
}

void nrmod_free(void* mod) {
  nrmod_clear(static_cast<PyObject*>(mod));
}

int nrmod_exec(PyObject* mod) {
  auto state {static_cast<NanorouteState*>(PyModule_GetState(mod))};

  state->RouterType = reinterpret_cast<PyTypeObject*>(
      PyType_FromModuleAndSpec(mod, &router_spec, nullptr));
  if(!state->RouterType)
    return -1;
  if(PyModule_AddType(mod, state->RouterType) < 0)
    return -1;

  state->req_meth_string = PyUnicode_InternFromString("REQUEST_METHOD");
  if(!state->req_meth_string)
    return -1;

  state->path_info_string = PyUnicode_InternFromString("PATH_INFO");
  if(!state->path_info_string)
    return -1;

  state->wsgi_key_string = PyUnicode_InternFromString("nanoroute.params");
  if(!state->wsgi_key_string)
    return -1;

  return 0;
}

#define slot(s, v)                                                             \
  PyModuleDef_Slot {                                                           \
    s, reinterpret_cast<void*>(v)                                              \
  }

std::array nrmod_slots {
    slot(Py_mod_exec, nrmod_exec),
    slot(Py_mod_multiple_interpreters, Py_MOD_PER_INTERPRETER_GIL_SUPPORTED),
    slot(Py_mod_gil, Py_MOD_GIL_NOT_USED),
    PyModuleDef_Slot {},
};
#undef slot

} // namespace

PyModuleDef nanoroute_mod {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "nanoroute",
    .m_doc = "Small, fast, HTTP URL router",
    .m_size = sizeof(NanorouteState),
    .m_slots = nrmod_slots.data(),
    .m_traverse = nrmod_traverse,
    .m_clear = nrmod_clear,
    .m_free = nrmod_free,
};

} // namespace nanoroute

PyMODINIT_FUNC PyInit_nanoroute() {
  return PyModuleDef_Init(&nanoroute::nanoroute_mod);
}
