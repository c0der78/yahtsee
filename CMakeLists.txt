
cmake_minimum_required (VERSION 3.2)
set(CMAKE_CXX_STANDARD 14)

# add options for testing
option(ENABLE_COVERAGE "Enable code coverage testing." OFF)
option(ENABLE_MEMCHECK "Enable testing for memory leaks." OFF)

# define project name
project (yahtsee VERSION 0.1.0)

# set path to custom modules
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(PkgConfig)

pkg_search_module(CACA REQUIRED caca)
pkg_search_module(RJLOG REQUIRED rj_log)
pkg_search_module(RJSTR REQUIRED rj_string)
pkg_search_module(RJDICE REQUIRED rj_dice)
pkg_search_module(RJNET REQUIRED rj_net)
pkg_search_module(RJHTTP REQUIRED rj_net_http)
pkg_search_module(RJASYNC REQUIRED rj_net_async)
pkg_search_module(URIPARSER REQUIRED liburiparser)

find_package(Curses REQUIRED)

find_package(Prep)

if (PREP_FOUND)
    prep_init()
endif()

find_path(ARCHIVE_INCLUDE_DIR archive.h HINTS "/usr/local/opt/libarchive/include")
find_library(ARCHIVE_LIBRARY archive HINTS "/usr/local/opt/libarchive/lib")

find_path(MINIUPNPC_INCLUDE_DIR miniupnpc.h PATH_SUFFIXES miniupnpc)
find_library(MINIUPNPC_LIBRARY miniupnpc)

# add directories
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(resources)

if (PREP_FOUND)
    add_dependencies(${PROJECT_NAME} ${PREP_INIT_DEPENDENCIES})
endif()

# Setup testing
enable_testing()

add_test(${PROJECT_NAME}_test ${PROJECT_BINARY_DIR}/tests/${PROJECT_NAME}_test)
