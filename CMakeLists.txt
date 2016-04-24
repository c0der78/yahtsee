
cmake_minimum_required (VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)

# add options for testing
option(CODE_COVERAGE "Enable code coverage testing." OFF)
option(MEMORY_CHECK "Enable testing for memory leaks." OFF)

# define project name
project (yahtsee VERSION 0.1.0)

# set path to custom modules
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(PkgConfig)
find_package(CURL)

pkg_search_module(CACA REQUIRED caca)
pkg_search_module(ARG3LOG REQUIRED arg3log)
pkg_search_module(ARG3STR REQUIRED arg3str)
pkg_search_module(ARG3JSON REQUIRED arg3json)
pkg_search_module(ARG3DICE REQUIRED arg3dice)
pkg_search_module(ARG3NET REQUIRED arg3net)
pkg_search_module(ARG3LOG REQUIRED arg3log)
pkg_search_module(JSONC REQUIRED json-c json json0)

find_path(ARCHIVE_INCLUDE_DIR archive.h HINTS "/usr/local/opt/libarchive/include")
find_library(ARCHIVE_LIBRARY archive HINTS "/usr/local/opt/libarchive/lib")

find_path(MINIUPNPC_INCLUDE_DIR miniupnpc.h PATH_SUFFIXES miniupnpc)
find_library(MINIUPNPC_LIBRARY miniupnpc)

# add directories
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(resources)

# Setup testing
enable_testing()

add_test(${PROJECT_NAME}-test ${PROJECT_BINARY_DIR}/tests/${PROJECT_NAME}-test)

