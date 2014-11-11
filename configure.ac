#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([yahtsee], [0.1], [c0der78@gmail.com])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE

LT_INIT

# Checks for programs.
: ${CXXFLAGS=""}

# Checks for programs.
AC_PROG_CXX([clang++ g++])
AC_PROG_CC

if test "x$CXX" = "xclang++"; then
  AC_SUBST([AM_CPPFLAGS], ["-std=c++11 -stdlib=libc++ -Wall -Werror"])
else
  AC_SUBST([AM_CPPFLAGS], ["-std=c++11 -Wall -Werror"])
fi

# Checks for libraries.
AC_CHECK_LIB([caca], [caca_create_display])

if test x"$ac_cv_lib_caca_caca_create_display" != x"yes" ; then
	AC_MSG_ERROR([libcaca is required])
fi

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h caca.h])

# Checks for typedefs, structures, and compiler characteristics.
#AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

AC_CHECK_PROG(GIT_CHECK,git,yes)
if test x"$GIT_CHECK" != x"yes" ; then
    AC_MSG_ERROR([Required program 'git' not found.])
else
    git submodule update --init --recursive
fi

AC_CONFIG_SUBDIRS([src/arg3dice])

# Checks for library functions.
AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile])

AC_OUTPUT
