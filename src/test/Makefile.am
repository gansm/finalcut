#----------------------------------------------------------------------
# Makefile.am  -  The Final Cut unit tests programs
#----------------------------------------------------------------------

if CPPUNIT_TEST
AM_LDFLAGS = -L$(top_builddir)/src/.libs -lfinal $(CPPUNIT_LIBS)
AM_CPPFLAGS = -I$(top_srcdir)/include -Wall -Werror

noinst_PROGRAMS = \
	fstring_test \
	fpoint_test \
	frect_test

fstring_test_SOURCES = fstring-test.cpp
fpoint_test_SOURCES = fpoint-test.cpp
frect_test_SOURCES = frect-test.cpp

TESTS = fstring_test \
	fpoint_test \
	frect_test

check_PROGRAMS = $(TESTS)

test: check
endif

clean-local:
	-find . \( -name "*.gcda" -o -name "*.gcno" -o -name "*.gcov" \) -delete
