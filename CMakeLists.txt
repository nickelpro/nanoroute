cmake_minimum_required(VERSION 3.25)

project(nanoroute LANGUAGES CXX VERSION 2.0.5)

option(NANOROUTE_USE_IPO "Use interprocedural optimization if supported" ON)
option(NANOROUTE_STRIP "Run system strip on compiled module" ON)
option(NANOROUTE_GEN_IWYU_MAPPINGS "Generate Python mappings for IWYU" OFF)

if(NANOROUTE_GEN_IWYU_MAPPINGS)
  include(cmake/GetIWYUMapGen.cmake)
endif()

find_package(Python3 3.13 REQUIRED COMPONENTS Development.Module)
Python3_add_library(nanoroute MODULE)
target_compile_features(nanoroute PRIVATE cxx_std_23)
set_target_properties(nanoroute PROPERTIES CXX_VISIBILITY_PRESET hidden)

if(NANOROUTE_USE_IPO AND NOT(CMAKE_BUILD_TYPE STREQUAL Debug))
  include(CheckIPOSupported)
  check_ipo_supported(RESULT ipo OUTPUT ipo_out)

  if(ipo)
    set_property(TARGET nanoroute PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

    if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
      target_compile_options(nanoroute PRIVATE /Gy /Gw)
      target_link_options(nanoroute PRIVATE /OPT:ICF=3)
    endif()

  else()
    message(WARNING "IPO is not supported: ${ipo_out}")
  endif()
endif()

if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
  target_compile_options(nanoroute PRIVATE
    -Wall -Wextra -Wno-missing-field-initializers -Wno-cast-function-type
  )

  if(NOT APPLE)
    target_link_options(nanoroute PRIVATE -Wl,--exclude-libs,ALL)
  endif()
elseif(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
  target_compile_options(nanoroute PRIVATE /W3)
endif()

if(NANOROUTE_STRIP AND DEFINED CMAKE_STRIP AND NOT(CMAKE_BUILD_TYPE STREQUAL Debug))
  if(APPLE)
    add_custom_command(TARGET nanoroute POST_BUILD
      COMMAND ${CMAKE_STRIP} -x $<TARGET_FILE:nanoroute>
    )
  else()
    add_custom_command(TARGET nanoroute POST_BUILD
      COMMAND ${CMAKE_STRIP} $<TARGET_FILE:nanoroute>
    )
  endif()
endif()

add_subdirectory(src)

if(DEFINED PY_BUILD_CMAKE_VERSION)
  set(NANOROUTE_PLATLIB ${PY_BUILD_CMAKE_PROJECT_NAME}-${PY_BUILD_CMAKE_PROJECT_VERSION}.data/platlib)
else()
  set(NANOROUTE_PLATLIB platlib)
endif()

install(
  TARGETS nanoroute
  LIBRARY DESTINATION ${NANOROUTE_PLATLIB}
)

install(
  FILES src/nanoroute.pyi
  DESTINATION ${NANOROUTE_PLATLIB}
)
