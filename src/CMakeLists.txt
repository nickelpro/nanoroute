target_sources(nanoroute
  PRIVATE
    ModNanoroute.cpp
    HTTPPerfectHash.cpp
    HTTPRouter.cpp
    PyRouter.cpp

  PRIVATE
  FILE_SET HEADERS
  BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
  FILES
    http_method.def
    HTTPPerfectHash.hpp
    HTTPRouter.hpp
    ModNanoroute.hpp
    PyRouter.hpp
    Util.hpp
)