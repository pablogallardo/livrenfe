#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([LivreNFE], [0.1], [pggllrd@gmail.com])
AM_INIT_AUTOMAKE([livrenfe], [2.6])
AC_CONFIG_FILES([Makefile src/Makefile include/Makefile src/libnfe/Makefile include/libnfe/Makefile src/icons/Makefile])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_SRCDIR(src)
AM_SILENT_RULES([yes])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([sqlite3], [main])

m4_ifdef([GTK_DOC_CHECK], [
GTK_DOC_CHECK([1.14], [--flavour no-tmpl]),
AC_CONFIG_FILES([docs/reference/libnfe/Makefile])
],[
AM_CONDITIONAL([ENABLE_GTK_DOC], false)
])

AC_CONFIG_MACRO_DIR(m4)

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h sqlite3.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([mkdir strdup])

LT_INIT
AM_ENABLE_SHARED
AC_ENABLE_SHARED
LT_OUTPUT

AC_OUTPUT
