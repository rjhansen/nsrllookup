AC_PREREQ(2.61)
AC_INIT([NSRL Lookup], [1.2], [Robert J. Hansen <rjh@secret-alchemy.com>], [nsrllookup], [http://rjhansen.github.com/nsrllookup])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([src/nsrllookup.cc])
AC_CONFIG_HEADER([config.h])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_PROG_CXX
AC_CHECK_HEADERS([netdb.h netinet/in.h sys/socket.h])
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_HEADER_STDC
AC_FUNC_SELECT_ARGTYPES
AC_CHECK_FUNCS([gethostbyname memset select socket poll])
AC_CONFIG_FILES([Makefile
                 man/Makefile
                 src/Makefile])
AC_OUTPUT
