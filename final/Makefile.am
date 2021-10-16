#----------------------------------------------------------------------
# Makefile.am  -  FINAL CUT library
#----------------------------------------------------------------------

LIBS = @FINAL_LIBS@

AM_CPPFLAGS = -I$(top_srcdir)/final -Wall -Werror -DCOMPILE_FINAL_CUT -std=c++11
AUTOMAKE_OPTIONS = subdir-objects

SUBDIRS = . font

lib_LTLIBRARIES = libfinal.la

libfinal_la_SOURCES = \
	dialog/fdialog.cpp \
	dialog/ffiledialog.cpp \
	dialog/fmessagebox.cpp \
	input/fkeyboard.cpp \
	input/fkey_map.cpp \
	input/fmouse.cpp \
	menu/fcheckmenuitem.cpp \
	menu/fdialoglistmenu.cpp \
	menu/fmenubar.cpp \
	menu/fmenu.cpp \
	menu/fmenuitem.cpp \
	menu/fmenulist.cpp \
	menu/fradiomenuitem.cpp \
	output/fcolorpalette.cpp \
	output/foutput.cpp \
	output/tty/fcharmap.cpp \
	output/tty/foptiattr.cpp \
	output/tty/foptimove.cpp \
	output/tty/ftermcap.cpp \
	output/tty/ftermcapquirks.cpp \
	output/tty/fterm.cpp \
	output/tty/ftermdebugdata.cpp \
	output/tty/ftermdetection.cpp \
	output/tty/ftermfreebsd.cpp \
	output/tty/fterm_functions.cpp \
	output/tty/ftermios.cpp \
	output/tty/ftermlinux.cpp \
	output/tty/ftermopenbsd.cpp \
	output/tty/ftermoutput.cpp \
	output/tty/ftermxterminal.cpp \
	output/tty/sgr_optimizer.cpp \
	util/fcallback.cpp \
	util/fdata.cpp \
	util/flog.cpp \
	util/flogger.cpp \
	util/fpoint.cpp \
	util/frect.cpp \
	util/fsize.cpp \
	util/fstring.cpp \
	util/fstringstream.cpp \
	util/fsystem.cpp \
	util/fsystemimpl.cpp \
	vterm/fvtermattribute.cpp \
	vterm/fvtermbuffer.cpp \
	vterm/fvterm.cpp \
	widget/fbusyindicator.cpp \
	widget/fbutton.cpp \
	widget/fbuttongroup.cpp \
	widget/fcheckbox.cpp \
	widget/fcombobox.cpp \
	widget/flabel.cpp \
	widget/flineedit.cpp \
	widget/flistbox.cpp \
	widget/flistview.cpp \
	widget/fprogressbar.cpp \
	widget/fradiobutton.cpp \
	widget/fscrollbar.cpp \
	widget/fscrollview.cpp \
	widget/fspinbox.cpp \
	widget/fstatusbar.cpp \
	widget/fswitch.cpp \
	widget/ftextview.cpp \
	widget/ftogglebutton.cpp \
	widget/ftooltip.cpp \
	widget/fwindow.cpp \
	fapplication.cpp \
	fevent.cpp \
	fobject.cpp \
	fstartoptions.cpp \
	fwidgetcolors.cpp \
	fwidget.cpp \
	fwidget_functions.cpp

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
	input/fkeyboard.h \
	input/fkey_map.h \
	input/fmouse.h

finalcutmenuinclude_HEADERS = \
	menu/fcheckmenuitem.h \
	menu/fdialoglistmenu.h \
	menu/fmenubar.h \
	menu/fmenu.h \
	menu/fmenuitem.h \
	menu/fmenulist.h \
	menu/fradiomenuitem.h

finalcutoutputinclude_HEADERS = \
	output/fcolorpalette.h \
	output/foutput.h

finalcutoutputttyinclude_HEADERS = \
	output/tty/fcharmap.h \
	output/tty/foptiattr.h \
	output/tty/foptimove.h \
	output/tty/ftermcap.h \
	output/tty/ftermcapquirks.h \
	output/tty/ftermdata.h \
	output/tty/ftermdebugdata.h \
	output/tty/ftermdetection.h \
	output/tty/ftermfreebsd.h \
	output/tty/fterm_functions.h \
	output/tty/fterm.h \
	output/tty/ftermios.h \
	output/tty/ftermlinux.h \
	output/tty/ftermopenbsd.h \
	output/tty/ftermoutput.h \
	output/tty/ftermxterminal.h \
	output/tty/sgr_optimizer.h

finalcututilinclude_HEADERS = \
	util/emptyfstring.h \
	util/fcallback.h \
	util/fdata.h \
	util/flogger.h \
	util/flog.h \
	util/fpoint.h \
	util/frect.h \
	util/fsize.h \
	util/fstring.h \
	util/fstringstream.h \
	util/fsystem.h \
	util/fsystemimpl.h

finalcutvterminclude_HEADERS = \
	vterm/fcolorpair.h \
	vterm/fstyle.h \
	vterm/fvtermattribute.h \
	vterm/fvtermbuffer.h \
	vterm/fvterm.h

finalcutwidgetinclude_HEADERS = \
	widget/fbusyindicator.h \
	widget/fbuttongroup.h \
	widget/fbutton.h \
	widget/fcheckbox.h \
	widget/fcombobox.h \
	widget/flabel.h \
	widget/flineedit.h \
	widget/flistbox.h \
	widget/flistview.h \
	widget/fprogressbar.h \
	widget/fradiobutton.h \
	widget/fscrollbar.h \
	widget/fscrollview.h \
	widget/fspinbox.h \
	widget/fstatusbar.h \
	widget/fswitch.h \
	widget/ftextview.h \
	widget/ftogglebutton.h \
	widget/ftooltip.h \
	widget/fwindow.h

finalcutinclude_HEADERS = \
	fapplication.h \
	fc.h \
	fconfig.h \
	fevent.h \
	final.h \
	fobject.h \
	fstartoptions.h \
	ftypes.h \
	fwidgetcolors.h \
	fwidget_functions.h \
	fwidget.h

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

