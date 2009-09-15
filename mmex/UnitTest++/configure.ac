#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([UnitTest++], [1.4])
AC_CONFIG_SRCDIR([src/UnitTest++.h])
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([${PACKAGE_NAME}], [${PACKAGE_VERSION}], [${PACKAGE_BUGREPORT}])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
AC_CHECK_FUNCS([gettimeofday strstr])

AC_CONFIG_FILES([Makefile src/Makefile src/tests/Makefile])
AC_OUTPUT
