#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
##########################################################################

AC_PREREQ(2.53)
AC_INIT([Money Manager Ex], [0.9.3.0], [madhan@sourceforge.net])

##########################################################################

AC_CONFIG_SRCDIR([src/mmex.cpp])
#AC_CONFIG_MACRO_DIR([m4])
#AC_CONFIG_AUX_DIR([aux])

##########################################################################

# Standard Checks

AC_CANONICAL_SYSTEM

##########################################################################

# Checks for programs

AC_PROG_CXX
AC_PROG_CC
AC_PROG_CXXCPP
AC_PROG_CPP
#AC_PROG_LIBTOOL

##########################################################################

# Checks for compilation model

AC_SYS_LARGEFILE

##########################################################################

# Add wxConfig Options to Configure

AM_OPTIONS_WXCONFIG
AM_OPTIONS_WXPRESETS

AM_WXPRESETS_FULL([2.8.0], [base, core, html, xml, net, aui, adv])

##########################################################################

AM_INIT_AUTOMAKE

##########################################################################

# Process standard bakefile
AC_BAKEFILE([m4_include(autoconf_inc.m4)])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

##########################################################################

AM_WXPRESETS_MSG_BEGIN
AM_WXPRESETS_MSG
AM_WXPRESETS_MSG_END

##########################################################################
