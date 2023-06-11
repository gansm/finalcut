#----------------------------------------------------------------------
# configure.ac  -  FINAL CUT library
#----------------------------------------------------------------------

# Process this file with autoconf to produce a configure script.

AC_INIT([finalcut], [0.9.0])
AC_CONFIG_HEADERS([config.h])
AX_PREFIX_CONFIG_H([final/fconfig.h], [F])
AC_CONFIG_SRCDIR([final/fobject.cpp])
AC_USE_SYSTEM_EXTENSIONS
AM_INIT_AUTOMAKE
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
AC_PROG_CC

if test -z $CXXFLAGS
then
  CXXFLAGS="-O3"
fi

AC_PROG_CXX

# Checks for header files.
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
		 vector )

# Checks for library functions.
AC_CHECK_FUNCS( \
		getuid \
		geteuid \
		getttynam \
		select \
		sigaction \
		strdup \
		strstr \
		vsnprintf \
		wcwidth )

# Checks for 'tgetent'
AC_SEARCH_LIBS([tgetent], [terminfo mytinfo termlib termcap tinfo ncurses curses])
# Checks for 'tparm'
AC_SEARCH_LIBS([tparm], [terminfo mytinfo termlib termcap tinfo ncurses curses])
# Checks for 'timer_create'
AC_SEARCH_LIBS([timer_create], [rt])
# Checks for 'timer_create'
AC_SEARCH_LIBS([timer_settime], [rt])

AC_SUBST([FINAL_LIBS])
AC_SUBST([TERMCAP_LIB])
TERMCAP_LIB="$LIBS"
FINAL_LIBS="$LIBS"
LIBS=""

# Checks for libtool
AC_ENABLE_SHARED
AC_ENABLE_STATIC
AC_LANG([C++])
LT_INIT([dlopen])
LT_LANG([C++])
LT_OUTPUT

### This defines the version number of the installed .so files
### Update this value for every release!  (A:B:C will map to foo.so.(A-C).C.B)
### using libtool's versioning system.
AC_SUBST(SO_VERSION, ["9:0:9"])

AC_SUBST([LIBTOOL_DEPS])

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_FILES([Makefile
                 final/Makefile
                 final/font/Makefile
                 doc/Makefile
                 examples/Makefile
                 test/Makefile
                 finalcut.pc])

# Check for C++14 support
m4_ifdef([AX_CXX_COMPILE_STDCXX_14],
         [AX_CXX_COMPILE_STDCXX_14([noext],[mandatory])],
         [AC_MSG_ERROR([Macro AX_CXX_COMPILE_STDCXX_14 not found. Please install GNU autoconf-archive])])

# use GPM (General Purpose Mouse)
AC_ARG_WITH([gpm],
            [AS_HELP_STRING([--without-gpm], [Disable GPM mouse support])],
            [],
            [with_gpm=yes])
if test "x$with_gpm" != "xno"
then
  AC_CHECK_LIB([gpm],
               [Gpm_Open],
               [AC_DEFINE([HAVE_LIBGPM], 1, [Define to 1 if GPM mouse is enabled])
                FINAL_LIBS="$FINAL_LIBS -lgpm"])
fi

# profiling
AC_ARG_WITH([profiler],
            [AS_HELP_STRING([--with-profiler], [build extra google profiler binaries])],
            [],
            [with_profiler=no])
if test "x$with_profiler" != "xno"
then
  AC_CHECK_LIB([profiler],
               [ProfilerFlush],
               [FINAL_LIBS="$FINAL_LIBS -lprofiler"])
fi

# unit test
AC_ARG_WITH([unit-test],
            [AS_HELP_STRING([--with-unit-test], [build unit tests])],
            [],
            [with_unit_test=no])
if test "x$with_unit_test" != "xno"
then
  AC_MSG_NOTICE(enabled cppunit test)
  PKG_CHECK_MODULES(CPPUNIT,
                    [cppunit > 1.12.0])
  AM_CONDITIONAL(CPPUNIT_TEST, [test "1" = "1"])
else
  AM_CONDITIONAL(CPPUNIT_TEST, [test "1" = "0"])
fi

# code coverage
AC_ARG_WITH([gcov],
            [AS_HELP_STRING([--with-gcov], [build for code coverage testing])],
            [],
            [with_gcov=no])
if test "x$with_gcov" != "xno"
then
  AC_CHECK_LIB([gcov],
               [main],
               [FINAL_LIBS="$FINAL_LIBS -lgcov"])
  AM_CXXFLAGS="-fprofile-arcs -ftest-coverage $AM_CXXFLAGS"
  AC_SUBST([AM_CXXFLAGS])
fi

AC_OUTPUT
