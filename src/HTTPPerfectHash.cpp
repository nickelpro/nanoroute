/* C++ code produced by gperf version 3.1 */
/* Command-line: gperf http_method.gperf  */
/* Computed positions: -k'1,3-4' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gperf@gnu.org>."
#endif

#line 1 "http_method.gperf"

#include "Util.hpp"

namespace nanoroute {
#line 16 "http_method.gperf"
struct PerfectHashResult {const char* name; HTTPMethod meth;};
#include <string.h>
enum
  {
    TOTAL_KEYWORDS = 47,
    MIN_WORD_LENGTH = 3,
    MAX_WORD_LENGTH = 13,
    MIN_HASH_VALUE = 3,
    MAX_HASH_VALUE = 88
  };

/* maximum key range = 86, duplicates = 0 */

class HTTPPerfectHash
{
private:
  static inline unsigned int hash (const char *str, size_t len);
public:
  static const struct PerfectHashResult *in_word_set (const char *str, size_t len);
};

inline unsigned int
HTTPPerfectHash::hash (const char *str, size_t len)
{
  static const unsigned char asso_values[] =
    {
      89, 89, 89, 89, 89, 89, 89, 89, 89, 89,
      89, 89, 89, 89, 89, 89, 89, 89, 89, 89,
      89, 89, 89, 89, 89, 89, 89, 89, 89, 89,
      89, 89, 89, 89, 89, 89, 89, 89, 89, 89,
      89, 89, 89, 89, 89, 89, 89, 89, 89, 89,
      89, 89, 89, 89, 89, 89, 89, 89, 89, 89,
      89, 89, 89, 89, 89,  5, 20, 20,  5,  0,
      30, 50, 56,  5,  5, 89, 15,  5, 35,  0,
       0, 55, 10,  0,  0,  0, 40, 89, 89, 89,
       0, 89, 89, 89, 89, 89, 25, 89, 89, 89,
      89, 89, 89, 89, 89, 89, 89, 89, 89, 89,
      89, 89, 89, 89, 89, 89, 89, 89, 89, 89,
      89, 89, 89, 89, 89, 89, 89, 89, 89
    };
  unsigned int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[static_cast<unsigned char>(str[3]+1)];
      /*FALLTHROUGH*/
      case 3:
        hval += asso_values[static_cast<unsigned char>(str[2])];
      /*FALLTHROUGH*/
      case 2:
      case 1:
        hval += asso_values[static_cast<unsigned char>(str[0])];
        break;
    }
  return hval;
}

static const unsigned char lengthtable[] =
  {
     0,  0,  0,  3,  4,  5,  6,  0,  3,  4,  5,  6,  7,  8,
     0,  5,  6,  0,  8,  0,  0,  6,  0,  3,  4, 10,  6,  0,
     8,  9,  5,  6,  0,  8,  0,  5,  6,  0, 13,  0,  0,  6,
     0,  8,  0,  5,  6,  0,  8,  0,  0, 11,  0,  3,  4, 10,
     6,  0,  0,  4,  5,  0,  7,  8,  9,  4,  0,  0,  0,  4,
     0,  5,  0,  0,  0,  0,  5,  0,  0,  4,  0,  0,  0,  0,
     0,  0,  0,  0, 13
  };

static const struct PerfectHashResult wordlist[] =
  {
    {""}, {""}, {""},
#line 22 "http_method.gperf"
    {"PUT",           HTTPMethod::Put},
#line 21 "http_method.gperf"
    {"POST",          HTTPMethod::Post},
#line 57 "http_method.gperf"
    {"PAUSE",         HTTPMethod::Pause},
#line 51 "http_method.gperf"
    {"SOURCE",        HTTPMethod::Source},
    {""},
#line 52 "http_method.gperf"
    {"PRI",           HTTPMethod::Pri},
#line 56 "http_method.gperf"
    {"PLAY",          HTTPMethod::Play},
#line 46 "http_method.gperf"
    {"PATCH",         HTTPMethod::Patch},
#line 32 "http_method.gperf"
    {"SEARCH",        HTTPMethod::Search},
#line 24 "http_method.gperf"
    {"OPTIONS",       HTTPMethod::Options},
#line 58 "http_method.gperf"
    {"TEARDOWN",      HTTPMethod::Teardown},
    {""},
#line 25 "http_method.gperf"
    {"TRACE",         HTTPMethod::Trace},
#line 38 "http_method.gperf"
    {"REPORT",        HTTPMethod::Report},
    {""},
#line 53 "http_method.gperf"
    {"DESCRIBE",      HTTPMethod::Describe},
    {""}, {""},
#line 33 "http_method.gperf"
    {"UNLOCK",        HTTPMethod::Unlock},
    {""},
#line 37 "http_method.gperf"
    {"ACL",           HTTPMethod::ACL},
#line 26 "http_method.gperf"
    {"COPY",          HTTPMethod::Copy},
#line 39 "http_method.gperf"
    {"MKACTIVITY",    HTTPMethod::MkActivity},
#line 50 "http_method.gperf"
    {"UNLINK",        HTTPMethod::Unlink},
    {""},
#line 61 "http_method.gperf"
    {"REDIRECT",      HTTPMethod::Redirect},
#line 44 "http_method.gperf"
    {"SUBSCRIBE",     HTTPMethod::Subscribe},
#line 28 "http_method.gperf"
    {"MKCOL",         HTTPMethod::MkCol},
#line 36 "http_method.gperf"
    {"UNBIND",        HTTPMethod::Unbind},
    {""},
#line 40 "http_method.gperf"
    {"CHECKOUT",      HTTPMethod::Checkout},
    {""},
#line 63 "http_method.gperf"
    {"FLUSH",         HTTPMethod::Flush},
#line 62 "http_method.gperf"
    {"RECORD",        HTTPMethod::Record},
    {""},
#line 60 "http_method.gperf"
    {"SET_PARAMETER", HTTPMethod::Set_Parameter},
    {""}, {""},
#line 35 "http_method.gperf"
    {"REBIND",        HTTPMethod::Rebind},
    {""},
#line 42 "http_method.gperf"
    {"M-SEARCH",      HTTPMethod::MSearch},
    {""},
#line 55 "http_method.gperf"
    {"SETUP",         HTTPMethod::Setup},
#line 43 "http_method.gperf"
    {"NOTIFY",        HTTPMethod::Notify},
    {""},
#line 54 "http_method.gperf"
    {"ANNOUNCE",      HTTPMethod::Announce},
    {""}, {""},
#line 45 "http_method.gperf"
    {"UNSUBSCRIBE",   HTTPMethod::Unsubscribe},
    {""},
#line 19 "http_method.gperf"
    {"GET",           HTTPMethod::Get},
#line 27 "http_method.gperf"
    {"LOCK",          HTTPMethod::Lock},
#line 48 "http_method.gperf"
    {"MKCALENDAR",    HTTPMethod::MkCalendar},
#line 18 "http_method.gperf"
    {"DELETE",        HTTPMethod::Delete},
    {""}, {""},
#line 34 "http_method.gperf"
    {"BIND",          HTTPMethod::Bind},
#line 64 "http_method.gperf"
    {"QUERY",         HTTPMethod::Query},
    {""},
#line 23 "http_method.gperf"
    {"CONNECT",       HTTPMethod::Connect},
#line 30 "http_method.gperf"
    {"PROPFIND",      HTTPMethod::PropFind},
#line 31 "http_method.gperf"
    {"PROPPATCH",     HTTPMethod::PropPatch},
#line 20 "http_method.gperf"
    {"HEAD",          HTTPMethod::Head},
    {""}, {""}, {""},
#line 49 "http_method.gperf"
    {"LINK",          HTTPMethod::Link},
    {""},
#line 47 "http_method.gperf"
    {"PURGE",         HTTPMethod::Purge},
    {""}, {""}, {""}, {""},
#line 41 "http_method.gperf"
    {"MERGE",         HTTPMethod::Merge},
    {""}, {""},
#line 29 "http_method.gperf"
    {"MOVE",          HTTPMethod::Move},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 59 "http_method.gperf"
    {"GET_PARAMETER", HTTPMethod::Get_Parameter}
  };

const struct PerfectHashResult *
HTTPPerfectHash::in_word_set (const char *str, size_t len)
{
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      unsigned int key = hash (str, len);

      if (key <= MAX_HASH_VALUE)
        if (len == lengthtable[key])
          {
            const char *s = wordlist[key].name;

            if (*str == *s && !memcmp (str + 1, s + 1, len - 1))
              return &wordlist[key];
          }
    }
  return 0;
}
#line 65 "http_method.gperf"

}
