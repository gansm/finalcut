#----------------------------------------------------------------------
# Makefile.am  -  The Final Cut unit tests programs
#----------------------------------------------------------------------

AM_LDFLAGS = -L$(top_builddir)/src/.libs -lfinal $(CPPUNIT_LIBS)
AM_CPPFLAGS = -I$(top_srcdir)/include -Wall -Werror

noinst_PROGRAMS = \
	fstring_test

fstring_test_SOURCES = fstring-test.cpp

if CPPUNIT_TEST
TESTS = fstring_test
endif

check_PROGRAMS = $(TESTS)

test: check

clean-local:
	-find . \( -name "*.gcda" -o -name "*.gcno" -o -name "*.gcov" \) -delete
