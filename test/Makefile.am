#----------------------------------------------------------------------
# Makefile.am  -  FINAL CUT unit tests programs
#----------------------------------------------------------------------

if CPPUNIT_TEST
AM_LDFLAGS = -L$(top_builddir)/final/.libs -lfinal $(CPPUNIT_LIBS)
AM_CPPFLAGS = -I$(top_srcdir)/final -Wall -Werror -std=c++11

noinst_PROGRAMS = \
	fobject_test \
	fcallback_test \
	fdata_test \
	fmouse_test \
	fkeyboard_test \
	fterm_functions_test \
	ftermcap_test \
	ftermdata_test \
	fvtermbuffer_test \
	ftermdetection_test \
	ftermcapquirks_test \
	ftermlinux_test \
	ftermopenbsd_test \
	ftermfreebsd_test \
	foptimove_test \
	foptiattr_test \
	fcolorpair_test \
	fstyle_test \
	fstring_test \
	fstringstream_test \
	flogger_test \
	fsize_test \
	fpoint_test \
	frect_test

fobject_test_SOURCES = fobject-test.cpp
fcallback_test_SOURCES = fcallback-test.cpp
fdata_test_SOURCES = fdata-test.cpp
fmouse_test_SOURCES = fmouse-test.cpp
fkeyboard_test_SOURCES = fkeyboard-test.cpp
fterm_functions_test_SOURCES = fterm_functions-test.cpp
ftermcap_test_SOURCES = ftermcap-test.cpp
ftermdata_test_SOURCES = ftermdata-test.cpp
fvtermbuffer_test_SOURCES = fvtermbuffer-test.cpp
ftermdetection_test_SOURCES = ftermdetection-test.cpp
ftermcapquirks_test_SOURCES = ftermcapquirks-test.cpp
ftermlinux_test_SOURCES = ftermlinux-test.cpp
ftermopenbsd_test_SOURCES = ftermopenbsd-test.cpp
ftermfreebsd_test_SOURCES = ftermfreebsd-test.cpp
foptimove_test_SOURCES = foptimove-test.cpp
foptiattr_test_SOURCES = foptiattr-test.cpp
fcolorpair_test_SOURCES = fcolorpair-test.cpp
fstyle_test_SOURCES = fstyle-test.cpp
fstring_test_SOURCES = fstring-test.cpp
fstringstream_test_SOURCES = fstringstream-test.cpp
flogger_test_SOURCES = flogger-test.cpp
fsize_test_SOURCES = fsize-test.cpp
fpoint_test_SOURCES = fpoint-test.cpp
frect_test_SOURCES = frect-test.cpp

TESTS = fobject_test \
	fcallback_test \
	fdata_test \
	fmouse_test \
	fkeyboard_test \
	fterm_functions_test \
	ftermcap_test \
	ftermdata_test \
	fvtermbuffer_test \
	ftermdetection_test \
	ftermcapquirks_test \
	ftermlinux_test \
	ftermopenbsd_test \
	ftermfreebsd_test \
	foptimove_test \
	foptiattr_test \
	fcolorpair_test \
	fstyle_test \
	fstring_test \
	fstringstream_test \
	flogger_test \
	fsize_test \
	fpoint_test \
	frect_test

check_PROGRAMS = $(TESTS)

test: check
endif

clean-local:
	-find . \( -name "*.gcda" -o -name "*.gcno" -o -name "*.gcov" \) -delete
	-rm -rf .deps
