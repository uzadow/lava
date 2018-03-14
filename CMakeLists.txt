project(lava)
cmake_minimum_required(VERSION 3.5)
find_package(PkgConfig REQUIRED)

set(warnings_cxx "-Wall -Woverloaded-virtual -Wswitch -Wuninitialized -Wempty-body -Wno-unused-local-typedefs")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -fPIC -ffast-math -std=c++11 ${warnings_cxx}")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

pkg_search_module(GDK_PIXBUF REQUIRED gdk-pixbuf-2.0)

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release" CACHE STRING
       "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
       FORCE)
endif(NOT CMAKE_BUILD_TYPE)

add_library(base
    Test.cpp TestSuite.cpp Exception.cpp)

link_libraries(base)
add_executable(testbase testbase.cpp)

