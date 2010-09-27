#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.65)
AC_INIT([nwc-toolkit], [0.0.0], [syata@acm.org])
AC_CONFIG_SRCDIR([include/nwc-toolkit/string.h])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_INSTALL

AC_LANG_CPLUSPLUS
AC_C_INLINE

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDBOOL
AC_HEADER_STDC

AC_CHECK_HEADER([unicode/unistr.h], , [AC_MSG_ERROR([\
The NWC Toolkit requires the ICU C++ library.

ICU - International Components for Unicode.
Project URL: http://site.icu-project.org/])])

AC_CHECK_HEADER([iconv.h], , [AC_MSG_ERROR([\
The NWC Toolkit requires libiconv.

Project URL: http://www.gnu.org/software/libiconv/])])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_LIB([iconv], [iconv], , [AC_MSG_ERROR([\
The NWC Toolkit requires libiconv.

Project URL: http://www.gnu.org/software/libiconv/])])

AC_CONFIG_FILES([Makefile lib/Makefile tools/Makefile tests/Makefile])
AC_OUTPUT
