target_sources(nanoroute
  PRIVATE
    ModNanoroute.cpp
    PyRouter.cpp
    Util.cpp

  PRIVATE
  FILE_SET HEADERS
  BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
  FILES
    http_method.def
    HTTPRouter.hpp
    ModNanoroute.hpp
    PyRouter.hpp
    Util.hpp
)
