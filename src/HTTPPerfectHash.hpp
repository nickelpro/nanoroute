#ifndef NANOROUTE_PERFECTHASH_HPP
#define NANOROUTE_PERFECTHASH_HPP

#include <cstddef>

#include "Util.hpp"

namespace nanoroute {

struct PerfectHashResult {
  const char* name;
  HTTPMethod meth;
};

class HTTPPerfectHash {
private:
  static inline unsigned int hash(const char* str, std::size_t len);

public:
  static const struct PerfectHashResult* in_word_set(const char* str,
      std::size_t len);
};

} // namespace nanoroute

#endif // NANOROUTE_PERFECTHASH_HPP
