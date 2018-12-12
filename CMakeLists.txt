cmake_minimum_required(VERSION 3.12)
project(nsrllookup)
find_package(Boost 1.60.0 REQUIRED COMPONENTS program_options)
include_directories($Boost_INCLUDE_DIRS)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED OFF)
set(Boost_USE_STATIC_RUNTIME ON)

set(CMAKE_CXX_STANDARD 14)

add_compile_definitions(PACKAGE_VERSION="1.4")
add_compile_definitions(PACKAGE_URL="http://rjhansen.github.io/nsrlsvr")
add_compile_definitions(PACKAGE_BUGREPORT="rjh@sixdemonbag.org")

add_executable(nsrllookup
        src/common.cc
        src/common.hpp
        src/nsrllookup.cc
        src/parse_options.cc
        src/query_server.cc
        src/unix.hpp
        src/win32.hpp)
set_property(TARGET nsrllookup PROPERTY CXX_STANDARD 14)
set_property(TARGET nsrllookup PROPERTY CXX_STANDARD_REQUIRED true)
target_link_libraries(nsrllookup ${Boost_PROGRAM_OPTIONS_LIBRARY})
if (WIN32)
    target_link_libraries(nsrllookup wsock32 ws2_32)
endif()

if(MSVC)
    target_compile_options(nsrllookup PRIVATE /W4 /WX)
else(MSVC)
    target_compile_options(nsrllookup PRIVATE -Wall -Wextra -pedantic -Werror)
endif(MSVC)

install(TARGETS nsrllookup RUNTIME DESTINATION bin)
