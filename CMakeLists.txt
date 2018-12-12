cmake_minimum_required(VERSION 3.12)
project(nsrllookup)
find_package(Boost 1.60.0 REQUIRED COMPONENTS program_options system)
find_package(Threads REQUIRED)
set(CMAKE_CXX_STANDARD 14)

add_executable(nsrllookup)
target_sources(nsrllookup PRIVATE
        src/common.cc src/nsrllookup.cc
        src/parse_options.cc src/query_server.cc)
add_compile_definitions(PACKAGE_VERSION="1.4")
add_compile_definitions(PACKAGE_URL="http://rjhansen.github.io/nsrlsvr")
add_compile_definitions(PACKAGE_BUGREPORT="rob@hansen.engineering")
include_directories(${Boost_INCLUDE_DIRS})
set_property(TARGET nsrllookup PROPERTY CXX_STANDARD 14)
set_property(TARGET nsrllookup PROPERTY CXX_STANDARD_REQUIRED true)
target_link_libraries(nsrllookup ${Boost_LIBRARIES} Threads::Threads)

if (WIN32)
    target_link_libraries(nsrllookup wsock32 ws2_32)
endif()

install(TARGETS nsrllookup RUNTIME DESTINATION bin)
