#----------------------------------------------------------------------
# Makefile.am  -  FINAL CUT library
#----------------------------------------------------------------------

AM_CPPFLAGS = -I$(top_srcdir)/final -Wall -Werror -DCOMPILE_FINAL_CUT -std=c++11
AUTOMAKE_OPTIONS = subdir-objects

SUBDIRS = . font

lib_LTLIBRARIES = libfinal.la

libfinal_la_SOURCES = \
	dialog/fdialog.cpp \
	dialog/fmessagebox.cpp \
	dialog/ffiledialog.cpp \
	input/fmouse.cpp \
	input/fkeyboard.cpp \
	input/fkey_map.cpp \
	menu/fmenu.cpp \
	menu/fdialoglistmenu.cpp \
	menu/fmenubar.cpp \
	menu/fmenuitem.cpp \
	menu/fradiomenuitem.cpp \
	menu/fcheckmenuitem.cpp \
	menu/fmenulist.cpp \
	output/foutput.cpp \
	output/fcolorpalette.cpp \
	output/tty/ftermoutput.cpp \
	output/tty/fcharmap.cpp \
	output/tty/ftermcap.cpp \
	output/tty/ftermcapquirks.cpp \
	output/tty/ftermxterminal.cpp \
	output/tty/ftermfreebsd.cpp \
	output/tty/ftermopenbsd.cpp \
	output/tty/ftermlinux.cpp \
	output/tty/ftermdebugdata.cpp \
	output/tty/ftermdetection.cpp \
	output/tty/ftermios.cpp \
	output/tty/fterm.cpp \
	output/tty/fterm_functions.cpp \
	output/tty/sgr_optimizer.cpp \
	output/tty/foptiattr.cpp \
	output/tty/foptimove.cpp \
	util/fstring.cpp \
	util/fstringstream.cpp \
	util/fpoint.cpp \
	util/fsize.cpp \
	util/frect.cpp \
	util/fcallback.cpp \
	util/fdata.cpp \
	util/flog.cpp \
	util/flogger.cpp \
	util/fsystemimpl.cpp \
	util/fsystem.cpp \
	vterm/fvterm.cpp \
	vterm/fvtermbuffer.cpp \
	widget/fscrollbar.cpp \
	widget/fprogressbar.cpp \
	widget/flineedit.cpp \
	widget/fbutton.cpp \
	widget/fbuttongroup.cpp \
	widget/ftogglebutton.cpp \
	widget/fradiobutton.cpp \
	widget/fcheckbox.cpp \
	widget/fswitch.cpp \
	widget/flabel.cpp \
	widget/flistbox.cpp \
	widget/flistview.cpp \
	widget/fscrollview.cpp \
	widget/fwindow.cpp \
	widget/ftooltip.cpp \
	widget/fbusyindicator.cpp \
	widget/fspinbox.cpp \
	widget/fcombobox.cpp \
	widget/fstatusbar.cpp \
	widget/ftextview.cpp \
	fstartoptions.cpp \
	fevent.cpp \
	fapplication.cpp \
	fwidgetcolors.cpp \
	fwidget.cpp \
	fwidget_functions.cpp \
	fobject.cpp

libfinal_la_LDFLAGS = -no-undefined -version-info @SO_VERSION@

finalcutincludedir = $(includedir)/final
finalcutdialogincludedir = $(includedir)/final/dialog
finalcutinputincludedir = $(includedir)/final/input
finalcutmenuincludedir = $(includedir)/final/menu
finalcutoutputincludedir = $(includedir)/final/output
finalcutoutputttyincludedir = $(includedir)/final/output/tty
finalcututilincludedir = $(includedir)/final/util
finalcutvtermincludedir = $(includedir)/final/vterm
finalcutwidgetincludedir = $(includedir)/final/widget

finalcutdialoginclude_HEADERS = \
	dialog/fdialog.h \
	dialog/ffiledialog.h \
	dialog/fmessagebox.h

finalcutinputinclude_HEADERS = \
	input/fkey_map.h \
	input/fmouse.h \
	input/fkeyboard.h

finalcutmenuinclude_HEADERS = \
	menu/fmenu.h \
	menu/fdialoglistmenu.h \
	menu/fmenubar.h \
	menu/fmenuitem.h \
	menu/fradiomenuitem.h \
	menu/fcheckmenuitem.h \
	menu/fmenulist.h

finalcutoutputinclude_HEADERS = \
	output/foutput.h \
	output/fcolorpalette.h

finalcutoutputttyinclude_HEADERS = \
	output/tty/fcharmap.h \
	output/tty/sgr_optimizer.h \
	output/tty/foptiattr.h \
	output/tty/foptimove.h \
	output/tty/ftermoutput.h \
	output/tty/ftermcap.h \
	output/tty/ftermcapquirks.h \
	output/tty/ftermxterminal.h \
	output/tty/ftermfreebsd.h \
	output/tty/ftermopenbsd.h \
	output/tty/ftermlinux.h \
	output/tty/ftermdebugdata.h \
	output/tty/ftermdetection.h \
	output/tty/ftermios.h \
	output/tty/fterm.h \
	output/tty/fterm_functions.h \
	output/tty/ftermdata.h

finalcututilinclude_HEADERS = \
	util/fcallback.h \
	util/fdata.h \
	util/emptyfstring.h \
	util/flog.h \
	util/flogger.h \
	util/fpoint.h \
	util/fsize.h \
	util/frect.h \
	util/fstring.h \
	util/fstringstream.h \
	util/fsystem.h \
	util/fsystemimpl.h

finalcutvterminclude_HEADERS = \
	vterm/fcolorpair.h \
	vterm/fstyle.h \
	vterm/fvtermbuffer.h \
	vterm/fvterm.h

finalcutwidgetinclude_HEADERS = \
	widget/fbutton.h \
	widget/fbuttongroup.h \
	widget/fcheckbox.h \
	widget/fswitch.h \
	widget/flabel.h \
	widget/flineedit.h \
	widget/flistbox.h \
	widget/flistview.h \
	widget/ftooltip.h \
	widget/fbusyindicator.h \
	widget/fprogressbar.h \
	widget/fradiobutton.h \
	widget/fscrollbar.h \
	widget/fscrollview.h \
	widget/fspinbox.h \
	widget/fcombobox.h \
	widget/fstatusbar.h \
	widget/ftextview.h \
	widget/ftogglebutton.h \
	widget/fwindow.h

finalcutinclude_HEADERS = \
	fapplication.h \
	fconfig.h \
	fc.h \
	ftypes.h \
	fevent.h \
	final.h \
	fobject.h \
	fstartoptions.h \
	fwidgetcolors.h \
	fwidget.h \
	fwidget_functions.h

clean-local:
	-find . \( -name "*.gcda" -o -name "*.gcno" -o -name "*.gcov" \) -delete
	-rm -rf .deps dialog/.deps input/.deps menu/.deps output/.deps output/tty/.deps util/.deps vterm/.deps widget/.deps

uninstall-hook:
	if test -d ${finalcutdialogincludedir}; then rmdir ${finalcutdialogincludedir}; fi
	if test -d ${finalcutinputincludedir}; then rmdir ${finalcutinputincludedir}; fi
	if test -d ${finalcutmenuincludedir}; then rmdir ${finalcutmenuincludedir}; fi
	if test -d ${finalcutoutputttyincludedir}; then rmdir ${finalcutoutputttyincludedir}; fi
	if test -d ${finalcutoutputincludedir}; then rmdir ${finalcutoutputincludedir}; fi
	if test -d ${finalcututilincludedir}; then rmdir ${finalcututilincludedir}; fi
	if test -d ${finalcutvtermincludedir}; then rmdir ${finalcutvtermincludedir}; fi
	if test -d ${finalcutwidgetincludedir}; then rmdir ${finalcutwidgetincludedir}; fi
	if test -d ${finalcutincludedir}; then rmdir ${finalcutincludedir}; fi

