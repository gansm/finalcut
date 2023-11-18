#----------------------------------------------------------------------
# Makefile.am  -  FINAL CUT library
#----------------------------------------------------------------------

EXTRA_DIST = \
	8x16graph.bdf \
	8x16graph.pcf.gz \
	9x16graph_patch.bdf \
	bdfmerge.sh \
	bdf2code.sh \
	bdf2pcf.sh \
	fonts.alias \
	fonts.dir \
	map-newfont \
	map-vga \
	Makefile.am \
	newfont_8x16.h \
	newfont_9x16.h \
	newfont.h \
	unicodemap.h \
	vgafont.h

docdir = ${datadir}/doc/${PACKAGE}
doc_DATA = \
	newfont.txt

fontdir = ${datadir}/fonts/X11/misc/${PACKAGE}
font_DATA = \
	8x16graph.pcf.gz \
	fonts.alias \
	fonts.dir

uninstall-hook:
	if test -d ${fontdir}; then rm -f ${fontdir}/.* 2>/dev/null ; rmdir ${fontdir}; fi

