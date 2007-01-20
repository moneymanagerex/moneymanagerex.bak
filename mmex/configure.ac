AC_INIT([Money Manager Ex], [0.8.0.3], [madhan@sourceforge.net])

AC_PREREQ(2.53)

dnl AC_INIT(aclocal.m4)

dnl Standard Checks
AC_CANONICAL_SYSTEM

dnl Checks for programs
AC_PROG_CXX
AC_PROG_CXXCPP

dnl Add sqlit3 Options to Configure
AX_LIB_SQLITE3(3.3.5)

dnl Add wxConfig Options to Configure
AM_OPTIONS_WXCONFIG
AM_OPTIONS_WXPRESETS

AM_WXPRESETS_FULL([2.8.0], [base, core, html, net, aui, adv])

dnl Process standard bakefile
AC_BAKEFILE([m4_include(autoconf_inc.m4)])

AC_OUTPUT(Makefile)

AM_WXPRESETS_MSG_BEGIN
AM_WXPRESETS_MSG
AM_WXPRESETS_MSG_END
