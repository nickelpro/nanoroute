#include <string_view>

#include <absl/container/flat_hash_map.h>

#include <HTTPRouter.hpp>
using std::operator""sv;

namespace nanoroute {

namespace {
#define HTTP_METHOD(code, name, sym) {#name##sv, static_cast<HTTPMethod>(code)},
static const absl::flat_hash_map<std::string_view, HTTPMethod> methmap {
#include "http_method.def"
};
} // namespace

HTTPMethod str2meth(std::string_view str) {
  if(auto it = methmap.find(str); it != methmap.end())
    return it->second;
  return HTTPMethod::INVALID;
}
} // namespace nanoroute
