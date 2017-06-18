#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([LivreNFE], [0.1], [pggllrd@gmail.com])
AM_INIT_AUTOMAKE([livrenfe], [2.6])
AC_OUTPUT([Makefile src/Makefile])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_SRCDIR(src)
AM_SILENT_RULES([yes])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([sqlite3], [main])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h sqlite3.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([mkdir strdup])

LT_INIT

AC_OUTPUT
