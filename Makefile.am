#----------------------------------------------------------------------
# Makefile.am  -  FINAL CUT terminal programming library
#----------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = finalcut.pc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = finalcut.pc

CLEANFILES = finalcut.pc

SUBDIRS = final doc examples test

docdir = ${datadir}/doc/${PACKAGE}
doc_DATA = AUTHORS COPYING COPYING.LESSER ChangeLog

test: check

clean-local:
	-rm -f *~

uninstall-hook:
	if test -d ${docdir}; then rmdir ${docdir}; fi

