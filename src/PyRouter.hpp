#ifndef NANOROUTE_PYROUTER_HPP
#define NANOROUTE_PYROUTER_HPP

#include <cstdint>
#include <utility>
#include <vector>

#include <Python.h>

#include "HTTPRouter.hpp"

namespace nanoroute {
enum struct HTTPMethod : std::int8_t;
struct NanorouteState;
} // namespace nanoroute

namespace nanoroute {

extern PyType_Spec router_spec;
extern PyType_Spec cbclosure_spec;

struct PyCBClosure;

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

  static PyObject* register_route(PyCBClosure* self, PyObject* const* args,
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

struct PyCBClosure : PyObject {
  static PyCBClosure* internal_new(PyTypeObject* subtype, PyRouter* pyrouter,
      PyObject* route, std::vector<HTTPMethod> meths);

private:
  PyCBClosure(PyRouter* pyrouter, PyObject* route,
      std::vector<HTTPMethod> meths)
      : pyrouter {pyrouter}, route {route}, meths {std::move(meths)} {}

public:
  static int traverse(PyCBClosure* self, visitproc visit, void* arg);
  static int clear(PyCBClosure* self);

  static void dealloc(PyCBClosure* self);

  PyRouter* pyrouter;
  PyObject* route;
  std::vector<HTTPMethod> meths;
};

} // namespace nanoroute

#endif // NANOROUTE_PYROUTER_HPP
