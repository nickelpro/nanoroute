#ifndef NANOROUTE_UTIL_HPP
#define NANOROUTE_UTIL_HPP

#include <string_view>

#include <HTTPRouter.hpp>


namespace nanoroute {

HTTPMethod str2meth(std::string_view str);

}

#endif // NANOROUTE_UTIL_HPP
