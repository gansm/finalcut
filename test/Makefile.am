#----------------------------------------------------------------------
# Makefile.am  -  FINAL CUT unit tests programs
#----------------------------------------------------------------------

LIBS = -lfinal $(CPPUNIT_LIBS)

EXTRA_DIST = \
	Makefile.clang \
	Makefile.gcc \
	conemu.h \
	fvterm_check.h \
	main-test.inc \
	test \
	test.sh

if CPPUNIT_TEST

AM_LDFLAGS = -L$(top_builddir)/final/.libs
AM_CPPFLAGS = -I$(top_srcdir)/final -Wall -Werror -std=c++14

noinst_PROGRAMS = \
	char_ringbuffer_test \
	eventloop_monitor_test \
	fcallback_test \
	fcolorpair_test \
	fdata_test \
	fevent_test \
	fkeyboard_test \
	flistview_test \
	flogger_test \
	fmouse_test \
	fobject_test \
	foptiattr_test \
	foptimove_test \
	fpoint_test \
	frect_test \
	fsize_test \
	fstring_test \
	fstringstream_test \
	fstyle_test \
	fterm_functions_test \
	ftermcap_test \
	ftermcapquirks_test \
	ftermdata_test \
	ftermdetection_test \
	ftermfreebsd_test \
	ftermlinux_test \
	ftermopenbsd_test \
	ftimer_test \
	fvterm_test \
	fvtermattribute_test \
	fvtermbuffer_test \
	fwidget_test

char_ringbuffer_test_SOURCES = char_ringbuffer-test.cpp
eventloop_monitor_test_SOURCES = eventloop-monitor-test.cpp
fcallback_test_SOURCES = fcallback-test.cpp
fcolorpair_test_SOURCES = fcolorpair-test.cpp
fdata_test_SOURCES = fdata-test.cpp
fevent_test_SOURCES = fevent-test.cpp
fkeyboard_test_SOURCES = fkeyboard-test.cpp
flistview_test_SOURCES = flistview-test.cpp
flogger_test_SOURCES = flogger-test.cpp
fmouse_test_SOURCES = fmouse-test.cpp
fobject_test_SOURCES = fobject-test.cpp
foptiattr_test_SOURCES = foptiattr-test.cpp
foptimove_test_SOURCES = foptimove-test.cpp
fpoint_test_SOURCES = fpoint-test.cpp
frect_test_SOURCES = frect-test.cpp
fsize_test_SOURCES = fsize-test.cpp
fstring_test_SOURCES = fstring-test.cpp
fstringstream_test_SOURCES = fstringstream-test.cpp
fstyle_test_SOURCES = fstyle-test.cpp
fterm_functions_test_SOURCES = fterm_functions-test.cpp
ftermcap_test_SOURCES = ftermcap-test.cpp
ftermcapquirks_test_SOURCES = ftermcapquirks-test.cpp
ftermdata_test_SOURCES = ftermdata-test.cpp
ftermdetection_test_SOURCES = ftermdetection-test.cpp
ftermfreebsd_test_LDADD = @TERMCAP_LIB@
ftermfreebsd_test_SOURCES = ftermfreebsd-test.cpp
ftermlinux_test_LDADD = @TERMCAP_LIB@
ftermlinux_test_SOURCES = ftermlinux-test.cpp
ftermopenbsd_test_LDADD = @TERMCAP_LIB@
ftermopenbsd_test_SOURCES = ftermopenbsd-test.cpp
ftimer_test_SOURCES = ftimer-test.cpp
fvterm_test_SOURCES = fvterm-test.cpp
fvtermattribute_test_SOURCES = fvtermattribute-test.cpp
fvtermbuffer_test_SOURCES = fvtermbuffer-test.cpp
fwidget_test_SOURCES = fwidget-test.cpp

TESTS = \
	char_ringbuffer_test \
	eventloop_monitor_test \
	fcallback_test \
	fcolorpair_test \
	fdata_test \
	fevent_test \
	fkeyboard_test \
	flistview_test \
	flogger_test \
	fmouse_test \
	fobject_test \
	foptiattr_test \
	foptimove_test \
	fpoint_test \
	frect_test \
	fsize_test \
	fstring_test \
	fstringstream_test \
	fstyle_test \
	fterm_functions_test \
	ftermcap_test \
	ftermcapquirks_test \
	ftermdata_test \
	ftermdetection_test \
	ftermfreebsd_test \
	ftermlinux_test \
	ftermopenbsd_test \
	ftimer_test \
	fvterm_test \
	fvtermattribute_test \
	fvtermbuffer_test \
	fwidget_test

check_PROGRAMS = $(TESTS)

test: check
endif

clean-local:
	-find . \( -name "*.gcda" -o -name "*.gcno" -o -name "*.gcov" \) -delete
	-rm -rf .deps
