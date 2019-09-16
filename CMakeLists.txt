cmake_minimum_required(VERSION 3.10)
project(nsrllookup)

find_package(Boost 1.65.0 REQUIRED COMPONENTS program_options system)
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_executable(nsrllookup
        src/common.cc src/nsrllookup.cc
        src/parse_options.cc src/query_server.cc)
add_definitions(-DPACKAGE_VERSION="1.4.1"
	-DPACKAGE_URL="http://rjhansen.github.io/nsrlsvr"
	-DPACKAGE_BUGREPORT="rob@hansen.engineering")
target_include_directories(nsrllookup PRIVATE ${Boost_INCLUDE_DIRS})

if (WIN32)
  add_definitions(-DBOOST_ALL_NO_LIB -D_WIN32_WINNT=0x0601 -DBOOST_ALL_DYN_LINK)
  set(Boost_USE_STATIC_LIBS ON)
  set(Boost_ALL_DYN_LINK ON)
  target_link_libraries(nsrllookup wsock32 ws2_32 Boost::program_options Threads::Threads)
endif()

if (UNIX)
  include(GNUInstallDirs)
  set(MAN_NAMES man1/nsrllookup.1)
  add_custom_target(man ALL DEPENDS ${MAN_NAMES})
  target_link_libraries(nsrllookup Boost::program_options Threads::Threads)
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/man1/nsrllookup.1 DESTINATION
    ${CMAKE_INSTALL_MANDIR}/man1/)
endif()

install(TARGETS nsrllookup RUNTIME DESTINATION bin)
