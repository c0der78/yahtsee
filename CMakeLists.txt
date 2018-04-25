
cmake_minimum_required (VERSION 3.2)
set(CMAKE_CXX_STANDARD 17)

# add options for testing
option(ENABLE_COVERAGE "Enable code coverage testing." OFF)
option(ENABLE_MEMCHECK "Enable testing for memory leaks." OFF)

# define project name
project (yahtsee VERSION 0.1.0)

# set path to custom modules
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(Prep)

if (PREP_FOUND)
    prep_init()
endif()

find_package(PkgConfig)

pkg_search_module(LIBLOG REQUIRED coda_log)
pkg_search_module(LIBSTR REQUIRED coda_string)
pkg_search_module(LIBDICE REQUIRED coda_dice)
pkg_search_module(LIBNET REQUIRED coda_net)
pkg_search_module(LIBNETHTTP REQUIRED coda_net_http)
pkg_search_module(LIBNETASYNC REQUIRED coda_net_async)
pkg_search_module(LIBURIPARSER REQUIRED liburiparser)

set(CURSES_NEED_NCURSES TRUE)

find_package(Curses REQUIRED)

find_library(CURSES_MENU_LIBRARY menu)

if (NOT CURSES_MENU_LIBRARY)
    message(FATAL "Curses menu extension required")
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
