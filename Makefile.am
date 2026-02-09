#----------------------------------------------------------------------
# Makefile.am  -  FINAL CUT terminal programming library
#----------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
	.codacy.yml \
	.codedocs \
	.editorconfig \
	.github/ISSUE_TEMPLATE/bug_report.md \
	.github/ISSUE_TEMPLATE/feature_request.md \
	.github/scripts/get_code_cov.sh \
	.github/workflows/alpine.yml \
	.github/workflows/arm.yml \
	.github/workflows/codeql-analysis.yml \
	.github/workflows/coverage.yml \
	.github/workflows/coverity-scan.yml \
	.github/workflows/cygwin.yml \
	.github/workflows/freebsd.yml \
	.github/workflows/macos.yml \
	.github/workflows/netbsd.yml \
	.github/workflows/openbsd.yml \
	.github/workflows/sigrid-publish.yml \
	.github/workflows/sigrid-pullrequest.yml \
	.github/workflows/solaris.yml \
	.github/workflows/sonarcloud.yml \
	.gitignore \
	CODE_OF_CONDUCT.md \
	Contributing.md \
	LICENSE \
	SECURITY.md \
	autogen.sh \
	build.sh \
	codecov.yml \
	finalcut.pc \
	misc/icon/16x16/finalcut.png \
	misc/icon/24x24/finalcut.png \
	misc/icon/256x256/finalcut.png \
	misc/icon/32x32/finalcut.png \
	misc/icon/48x48/finalcut.png \
	misc/icon/scalable/finalcut.svg \
	misc/logo/png/finalcut-logo.png \
	misc/logo/svg/finalcut-logo.svg \
	scripts/cppcheck.sh \
	scripts/cpuprofile.sh \
	scripts/disassemble.sh \
	scripts/kcachegrind.sh \
	scripts/library_calls.sh \
	scripts/perf-call-graph.sh \
	scripts/profile.sh \
	scripts/remove_eol_spaces.sh \
	scripts/show_graph_font.sh \
	scripts/showconsolefont.sh \
	scripts/syscall_statistic.sh \
	scripts/valgrind.sh \
	scripts/xterm-latin1.sh \
	sigrid.yaml \
	sonar-project.properties

if INSTALL_CMAKE_MODULES
  cmakemodulesdir = @CMAKEMODULESDIR@
  dist_cmakemodules_DATA = Findfinalcut.cmake
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = finalcut.pc

CLEANFILES = finalcut.pc

SUBDIRS = final doc examples test

docdir = ${datadir}/doc/${PACKAGE}
doc_DATA = AUTHORS LICENSE ChangeLog

test: check

clean-local:
	-rm -f *~

uninstall-hook:
	if test -d ${docdir}; then rmdir ${docdir}; fi

