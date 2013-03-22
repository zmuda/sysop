#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(pamiec, 1.0, m.zmuda@aol.com)
AC_CONFIG_SRCDIR([test.c])
AC_CONFIG_HEADERS([config.h])
AM_PROG_CC_C_O
AM_INIT_AUTOMAKE
LT_INIT

# Checks for programs.
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC

AC_ARG_WITH(
	allocator, 
	AC_HELP_STRING([--with-allocator], [Okresl sposob alokacji; small = w najmniejszym wolnym obszarze; big = w najwiekszym; first = w pierwszym na liscie(default)]), 
	[ if test "x$with_allocator" = "xsmall" ; then
         AC_DEFINE([SMALL_STRATEGY], [], [Okresl sposob allokacji])
      else
	if test "x$with_allocator" = "xbig" ; then
	AC_DEFINE([BIG_STRATEGY], [], [Okresl sposob allokacji])
	else	
	if test "x$with_allocator" = "xfirst" ; then
	AC_DEFINE([FIRST_STRATEGY], [], [Okresl sposob allokacji])
	fi	
	fi
      fi ]
)

AC_ARG_WITH(
	block-size, 
	AC_HELP_STRING([--enable-block-size], [Set choosen block size in bytes]), 
	[ if test "x$with_block_size" = "" ; then
         AC_MSG_RESULT([No])
         AC_DEFINE_UNQUOTED([BLOCK_SIZE], [1], [Defines block size in bytes.])
      else
         AC_DEFINE_UNQUOTED([BLOCK_SIZE], [$with_block_size], [Defines block size in bytes.])
         AC_MSG_RESULT([Yes])
      fi ],	
    [ AC_DEFINE_UNQUOTED([BLOCK_SIZE], [1], [Defines block size in bytes.])
	AC_MSG_RESULT([No]) ]
)

AC_CONFIG_FILES([Makefile
                 src/Makefile
		 src/makefile])
AC_OUTPUT
