#----------------------------------------------------------------------
# Makefile.am  -  FINAL CUT unit tests programs
#----------------------------------------------------------------------

LIBS = -lfinal $(CPPUNIT_LIBS)

if CPPUNIT_TEST

AM_LDFLAGS = -L$(top_builddir)/final/.libs
AM_CPPFLAGS = -I$(top_srcdir)/final -Wall -Werror -std=c++11

noinst_PROGRAMS = \
	fcallback_test \
	fcolorpair_test \
	fdata_test \
	fkeyboard_test \
	flogger_test \
	fmouse_test \
	fobject_test \
	foptiattr_test \
	foptimove_test \
	fpoint_test \
	frect_test \
	fsize_test \
	fstringstream_test \
	fstring_test \
	fstyle_test \
	ftermcapquirks_test \
	ftermcap_test \
	ftermdata_test \
	ftermdetection_test \
	ftermfreebsd_test \
	fterm_functions_test \
	ftermlinux_test \
	ftermopenbsd_test \
	fvtermbuffer_test

fcallback_test_SOURCES = fcallback-test.cpp
fcolorpair_test_SOURCES = fcolorpair-test.cpp
fdata_test_SOURCES = fdata-test.cpp
fkeyboard_test_SOURCES = fkeyboard-test.cpp
flogger_test_SOURCES = flogger-test.cpp
fmouse_test_SOURCES = fmouse-test.cpp
fobject_test_SOURCES = fobject-test.cpp
foptiattr_test_SOURCES = foptiattr-test.cpp
foptimove_test_SOURCES = foptimove-test.cpp
fpoint_test_SOURCES = fpoint-test.cpp
frect_test_SOURCES = frect-test.cpp
fsize_test_SOURCES = fsize-test.cpp
fstringstream_test_SOURCES = fstringstream-test.cpp
fstring_test_SOURCES = fstring-test.cpp
fstyle_test_SOURCES = fstyle-test.cpp
ftermcapquirks_test_SOURCES = ftermcapquirks-test.cpp
ftermcap_test_SOURCES = ftermcap-test.cpp
ftermdata_test_SOURCES = ftermdata-test.cpp
ftermdetection_test_SOURCES = ftermdetection-test.cpp
ftermfreebsd_test_SOURCES = ftermfreebsd-test.cpp
ftermfreebsd_test_LDADD = @TERMCAP_LIB@
fterm_functions_test_SOURCES = fterm_functions-test.cpp
ftermlinux_test_SOURCES = ftermlinux-test.cpp
ftermlinux_test_LDADD = @TERMCAP_LIB@
ftermopenbsd_test_SOURCES = ftermopenbsd-test.cpp
ftermopenbsd_test_LDADD = @TERMCAP_LIB@
fvtermbuffer_test_SOURCES = fvtermbuffer-test.cpp

TESTS = \
	fcallback_test \
	fcolorpair_test \
	fdata_test \
	fkeyboard_test \
	flogger_test \
	fmouse_test \
	fobject_test \
	foptiattr_test \
	foptimove_test \
	fpoint_test \
	frect_test \
	fsize_test \
	fstringstream_test \
	fstring_test \
	fstyle_test \
	ftermcapquirks_test \
	ftermcap_test \
	ftermdata_test \
	ftermdetection_test \
	ftermfreebsd_test \
	fterm_functions_test \
	ftermlinux_test \
	ftermopenbsd_test \
	fvtermbuffer_test

check_PROGRAMS = $(TESTS)

test: check
endif

clean-local:
	-find . \( -name "*.gcda" -o -name "*.gcno" -o -name "*.gcov" \) -delete
	-rm -rf .deps
