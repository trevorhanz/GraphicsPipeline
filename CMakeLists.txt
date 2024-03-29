cmake_policy(SET CMP0048 NEW)

project(GraphicsPipeline VERSION 0.1)
cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Perfer GLVND by default when available
if(POLICY CMP0072)
  cmake_policy(SET CMP0072 NEW)
endif(POLICY CMP0072)

# Uses <PackageName>_ROOT variables
if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif(POLICY CMP0074)

option(GP_BUILD_QT5 "Build Qt5 support." OFF)
option(GP_BUILD_TEST "Build unit tests." OFF)
option(GP_BUILD_EXAMPLES "BUILD example programs." OFF)

add_subdirectory(src)
add_subdirectory(docs)

if(GP_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif(GP_BUILD_EXAMPLES)

if(GP_BUILD_TEST)
  enable_testing()
  add_subdirectory(tests)
endif(GP_BUILD_TEST)
