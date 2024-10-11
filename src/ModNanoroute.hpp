#ifndef NANOROUTE_MODNANOROUTE_HPP
#define NANOROUTE_MODNANOROUTE_HPP

#include <Python.h>

namespace nanoroute {

struct NanorouteState {
  PyTypeObject* RouterType;
  PyObject* req_meth_string;
  PyObject* path_info_string;
  PyObject* wsgi_key_string;
};

extern PyModuleDef nanoroute_mod;

} // namespace nanoroute

#endif // NANOROUTE_MOD_NANOROUTE
