#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([freesweep], [1.1.0], [rwestlun@gmail.com])
AM_INIT_AUTOMAKE
AM_SILENT_RULES
AC_CONFIG_SRCDIR([main.c])

# Checks for programs.
PKG_PROG_PKG_CONFIG([0.25])
AC_PROG_CC

# Checks for libraries.
PKG_CHECK_MODULES([CURSES], [ncursesw >= 5],
  [AC_DEFINE(HAVE_NCURSESW_H)],
  [PKG_CHECK_MODULES([CURSES], [ncurses >= 5], [AC_DEFINE(HAVE_CURSES_H)],
    [AC_MSG_ERROR([ncurses library not found])])])

# Package configure sometimes only links to ncurses instead of ncursesw. This
# fixes that.
AC_CHECK_LIB(ncursesw, main,
  [CURSES_LIBS=$(echo $CURSES_LIBS | sed 's/-lncurses/-lncursesw/')
   CURSES_LIBS=$(echo $CURSES_LIBS | sed 's/-lncursesww/-lncursesw/')])

# Checks for header files.
AC_CHECK_HEADERS([sys/file.h unistd.h getopt.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UID_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([alarm getcwd memset mkdir setlocale strdup strncasecmp
                fileno flock lockf getopt getopt_long])

AC_CONFIG_FILES([Makefile
                 sweeprc])
AC_OUTPUT
