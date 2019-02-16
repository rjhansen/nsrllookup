cmake_minimum_required(VERSION 3.10)
project(nsrllookup)
include(GNUInstallDirs)
find_package(Boost 1.65.0 REQUIRED COMPONENTS program_options system)
find_package(Threads REQUIRED)
set(CMAKE_CXX_STANDARD 14)

add_executable(nsrllookup
        src/common.cc src/nsrllookup.cc
        src/parse_options.cc src/query_server.cc)
add_definitions(-DPACKAGE_VERSION="1.4.1"
	-DPACKAGE_URL="http://rjhansen.github.io/nsrlsvr"
	-DPACKAGE_BUGREPORT="rob@hansen.engineering")
include_directories(${Boost_INCLUDE_DIRS})
set_property(TARGET nsrllookup PROPERTY CXX_STANDARD 14)
set_property(TARGET nsrllookup PROPERTY CXX_STANDARD_REQUIRED true)
target_link_libraries(nsrllookup ${Boost_LIBRARIES} Threads::Threads)

if (WIN32)
    target_link_libraries(nsrllookup wsock32 ws2_32)
endif()

if (UNIX)
  set(MAN_NAMES man1/nsrllookup.1)
  add_custom_target(man ALL DEPENDS ${MAN_NAMES})
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/man1/nsrllookup.1 DESTINATION
    ${CMAKE_INSTALL_MANDIR}/man1/)
endif()

install(TARGETS nsrllookup RUNTIME DESTINATION bin)
