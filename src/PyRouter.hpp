#ifndef NANOROUTE_PYROUTER_HPP
#define NANOROUTE_PYROUTER_HPP

#include <vector>

#include <Python.h>

#include "HTTPRouter.hpp"
#include "ModNanoroute.hpp"

namespace nanoroute {

extern PyType_Spec router_spec;

struct PyRouter : PyObject {

  static PyObject* get(PyRouter* self, PyObject* const* args, Py_ssize_t nargs);
  static PyObject* post(PyRouter* self, PyObject* const* args,
      Py_ssize_t nargs);
  static PyObject* put(PyRouter* self, PyObject* const* args, Py_ssize_t nargs);
  static PyObject* delete_(PyRouter* self, PyObject* const* args,
      Py_ssize_t nargs);
  static PyObject* route(PyRouter* self, PyObject* const* args,
      Py_ssize_t nargs);

  static PyObject* lookup(PyRouter* self, PyObject* const* args,
      Py_ssize_t nargs);
  static PyObject* wsgi_app(PyRouter* self, PyObject* const* args,
      Py_ssize_t nargs);

  static PyObject* register_route(PyObject* self, PyObject* const* args,
      Py_ssize_t nargs);

  static PyRouter* new_(PyTypeObject* subtype, PyObject* /*args*/,
      PyObject* /*kwargs*/);
  static int init(PyRouter* self, PyObject* /*args*/, PyObject* /*kwargs*/);

  static int traverse(PyRouter* self, visitproc visit, void* arg);
  static int clear(PyRouter* self);

  static void dealloc(PyRouter* self);

private:
  PyObject* route_(HTTPMethod meth, PyObject* route_def);
  PyObject* route_(std::vector<HTTPMethod> meth, PyObject* route_def);

  HTTPRouter httprouter_;
  PyObject* mod_;
  NanorouteState* state_;
};

} // namespace nanoroute

#endif // NANOROUTE_PYROUTER_HPP
