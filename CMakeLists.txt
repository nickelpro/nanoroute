cmake_minimum_required(VERSION 3.25)

project(nanoroute LANGUAGES CXX VERSION 2.0.0)

option(NANOROUTE_USE_IPO "Use interprocedural optimization if supported" ON)
option(NANOROUTE_STRIP "Run system strip on compiled module" ON)

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

install(
  TARGETS nanoroute
  LIBRARY DESTINATION ${PY_BUILD_CMAKE_PACKAGE_NAME}-${PY_BUILD_CMAKE_PACKAGE_VERSION}.data/platlib
)

install(
  FILES src/nanoroute.pyi
  DESTINATION ${PY_BUILD_CMAKE_PACKAGE_NAME}-${PY_BUILD_CMAKE_PACKAGE_VERSION}.data/purelib
)
