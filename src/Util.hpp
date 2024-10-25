#ifndef NANOROUTE_UTIL_HPP
#define NANOROUTE_UTIL_HPP

#include <cstdint>

namespace nanoroute {

enum struct HTTPMethod : std::int8_t {
  INVALID = -1,
#define HTTP_METHOD(code, name, sym) sym = code,
#include "http_method.def"
  MAX,
};

} // namespace nanoroute

#endif // NANOROUTE_UTIL_HPP
