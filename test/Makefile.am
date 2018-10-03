#----------------------------------------------------------------------
# Makefile.am  -  The Final Cut unit tests programs
#----------------------------------------------------------------------

if CPPUNIT_TEST
AM_LDFLAGS = -L$(top_builddir)/src/.libs -lfinal $(CPPUNIT_LIBS)
AM_CPPFLAGS = -I$(top_srcdir)/src/include -Wall -Werror

noinst_PROGRAMS = \
	fobject_test \
	fmouse_test \
	fkeyboard_test \
	ftermdata_test \
	ftermdetection_test \
	ftermcapquirks_test \
	foptimove_test \
	foptiattr_test \
	fstring_test \
	fpoint_test \
	frect_test

fobject_test_SOURCES = fobject-test.cpp
fmouse_test_SOURCES = fmouse-test.cpp
fkeyboard_test_SOURCES = fkeyboard-test.cpp
ftermdata_test_SOURCES = ftermdata-test.cpp
ftermdetection_test_SOURCES = ftermdetection-test.cpp
ftermcapquirks_test_SOURCES = ftermcapquirks-test.cpp
foptimove_test_SOURCES = foptimove-test.cpp
foptiattr_test_SOURCES = foptiattr-test.cpp
fstring_test_SOURCES = fstring-test.cpp
fpoint_test_SOURCES = fpoint-test.cpp
frect_test_SOURCES = frect-test.cpp

TESTS = fobject_test \
	fmouse_test \
	fkeyboard_test \
	ftermdata_test \
	ftermdetection_test \
	ftermcapquirks_test \
	foptimove_test \
	foptiattr_test \
	fstring_test \
	fpoint_test \
	frect_test

check_PROGRAMS = $(TESTS)

test: check
endif

clean-local:
	-find . \( -name "*.gcda" -o -name "*.gcno" -o -name "*.gcov" \) -delete
