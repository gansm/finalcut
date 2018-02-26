#----------------------------------------------------------------------
# Makefile.am  -  The Final Cut unit tests programs
#----------------------------------------------------------------------

AM_LDFLAGS = -L$(top_builddir)/src/.libs -lfinal $(CPPUNIT_LIBS)
AM_CPPFLAGS = -I$(top_srcdir)/include -Wall -Werror

if CPPUNIT_TEST
noinst_PROGRAMS = \
	fstring_test

fstring_test_SOURCES = fstring-test.cpp

TESTS = fstring_test

check_PROGRAMS = $(TESTS)
endif

test: check

clean-local:
	-find . \( -name "*.gcda" -o -name "*.gcno" -o -name "*.gcov" \) -delete
