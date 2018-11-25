#----------------------------------------------------------------------
# configure.ac  -  the Final Cut library
#----------------------------------------------------------------------

# Process this file with autoconf to produce a configure script.

AC_INIT([finalcut], [0.5.0])
AC_CONFIG_HEADER([config.h])
AX_PREFIX_CONFIG_H([src/include/final/fconfig.h], [F])
AC_CONFIG_SRCDIR([src/fobject.cpp])
AC_USE_SYSTEM_EXTENSIONS
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_PROG_CXX

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS( \
		 linux/fb.h \
		 sys/io.h \
		 sys/kd.h \
		 sys/stat.h \
		 sys/time.h \
		 fcntl.h \
		 langinfo.h \
		 term.h \
		 termios.h \
		 ttyent.h \
		 unistd.h \
		 cmath \
		 csignal \
		 cstdlib \
                 list \
		 map \
		 queue \
		 vector)

# Checks for library functions.
AC_CHECK_FUNCS( \
		getuid \
		geteuid \
		getttynam \
		select \
		strdup \
		strstr \
		vsnprintf )

# Checks for 'tgetent'
AC_SEARCH_LIBS([tgetent], [termcap tinfo curses ncurses])
# Checks for 'tparm'
AC_SEARCH_LIBS([tparm], [termcap tinfo curses ncurses])

# Checks for libtool
AC_ENABLE_SHARED
AC_ENABLE_STATIC
LT_INIT([dlopen])
LT_LANG([C++])
LT_OUTPUT

### This defines the version number of the installed .so files
### Update this value for every release!  (A:B:C will map to foo.so.(A-C).C.B)
### using libtool's versioning system.
AC_SUBST(SO_VERSION, ["5:0:5"])

AC_SUBST([LIBTOOL_DEPS])

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 fonts/Makefile
                 doc/Makefile
                 examples/Makefile
                 test/Makefile
                 finalcut.spec
                 finalcut.pc])

# use GPM (General Purpose Mouse)
AC_ARG_WITH([gpm],
            [AS_HELP_STRING([--without-gpm], [Disable GPM mouse support])],
            [with_gpm=no],
            [with_gpm=yes])
if test "x$with_gpm" = "xyes"
then
  AC_CHECK_LIB([gpm],
               [main],
               [AC_DEFINE([HAVE_LIBGPM], 1, [Define to 1 if GPM mouse is enabled])
                LIBS="$LIBS -lgpm"])
fi

# profiling
AC_ARG_WITH([profiler],
            [AS_HELP_STRING([--with-profiler], [build extra google profiler binaries])],
 	    [with_profiler=yes],
            [with_profiler=no])
if test "x$with_profiler" = "xyes"
then
  AC_CHECK_LIB([profiler],
               [ProfilerFlush],
               [LIBS="$LIBS -lprofiler"])
fi

# unit test
AC_ARG_WITH([unit-test],
            [AS_HELP_STRING([--with-unit-test], [build unit tests])],
 	    [with_unit_test=yes],
            [with_unit_test=no])
if test "x$with_unit_test" = "xyes"
then
  AC_MSG_NOTICE(enabled cppunit test)
  AM_PATH_CPPUNIT([1.12.0],
                  [],
                  [AC_MSG_ERROR([*** CppUnit not found! ***])])
  AM_CONDITIONAL(CPPUNIT_TEST, [test "1" = "1"])
else
  AM_CONDITIONAL(CPPUNIT_TEST, [test "1" = "0"])
fi


# code coverage
AC_ARG_WITH([gcov],
            [AS_HELP_STRING([--with-gcov], [build for code coverage testing])],
 	    [with_gcov=yes],
            [with_gcov=no])
if test "x$with_gcov" = "xyes"
then
  AC_CHECK_LIB([gcov],
               [main],
               [LIBS="$LIBS -lgcov"])
  AM_CXXFLAGS="-fprofile-arcs -ftest-coverage $AM_CXXFLAGS"
  AC_SUBST([AM_CXXFLAGS])
fi

AC_OUTPUT
