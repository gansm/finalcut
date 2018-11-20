#----------------------------------------------------------------------
# Makefile.am  -  The Final Cut library
#----------------------------------------------------------------------

AM_CPPFLAGS = -Iinclude -Wall -Werror -DCOMPILE_FINAL_CUT

SUBDIRS = . 

lib_LTLIBRARIES = libfinal.la

libfinal_la_SOURCES = \
	fstring.cpp \
	fpoint.cpp \
	frect.cpp \
	fscrollbar.cpp \
	fprogressbar.cpp \
	flineedit.cpp \
	fbutton.cpp \
	fbuttongroup.cpp \
	ftogglebutton.cpp \
	fradiobutton.cpp \
	fcheckbox.cpp \
	fswitch.cpp \
	flabel.cpp \
	flistbox.cpp \
	flistview.cpp \
	fmenu.cpp \
	fmouse.cpp \
	fkeyboard.cpp \
	fdialoglistmenu.cpp \
	fmenubar.cpp \
	fmenuitem.cpp \
	fradiomenuitem.cpp \
	fcheckmenuitem.cpp \
	fmenulist.cpp \
	fdialog.cpp \
	fscrollview.cpp \
	fwindow.cpp \
	fmessagebox.cpp \
	ftooltip.cpp \
	ffiledialog.cpp \
	fkey_map.cpp \
	ftextview.cpp \
	fstatusbar.cpp \
	ftermcap.cpp \
	ftermcapquirks.cpp \
	ftermxterminal.cpp \
	ftermfreebsd.cpp \
	ftermopenbsd.cpp \
	ftermlinux.cpp \
	ftermdetection.cpp \
	ftermios.cpp \
	fterm.cpp \
	fvterm.cpp \
	fevent.cpp \
	foptiattr.cpp \
	foptimove.cpp \
	ftermbuffer.cpp \
	fapplication.cpp \
	fcolorpalette.cpp \
	fwidgetcolors.cpp \
	fwidget.cpp \
	fobject.cpp

libfinal_la_LDFLAGS = -version-info @SO_VERSION@

finalcutincludedir = $(includedir)/final

finalcutinclude_HEADERS = \
	include/final/fapplication.h \
	include/final/fbutton.h \
	include/final/fbuttongroup.h \
	include/final/fcharmap.h \
	include/final/fcheckbox.h \
	include/final/fconfig.h \
	include/final/fswitch.h \
	include/final/fdialog.h \
	include/final/fc.h \
	include/final/ftypes.h \
	include/final/emptyfstring.h \
	include/final/fevent.h \
	include/final/ffiledialog.h \
	include/final/final.h \
	include/final/fkey_map.h \
	include/final/flabel.h \
	include/final/flineedit.h \
	include/final/flistbox.h \
	include/final/flistview.h \
	include/final/fmenu.h \
	include/final/fmouse.h \
	include/final/fkeyboard.h \
	include/final/fdialoglistmenu.h \
	include/final/fmenubar.h \
	include/final/fmenuitem.h \
	include/final/fradiomenuitem.h \
	include/final/fcheckmenuitem.h \
	include/final/fmenulist.h \
	include/final/fmessagebox.h \
	include/final/ftooltip.h \
	include/final/fobject.h \
	include/final/fpoint.h \
	include/final/foptiattr.h \
	include/final/foptimove.h \
	include/final/ftermbuffer.h \
	include/final/fprogressbar.h \
	include/final/fradiobutton.h \
	include/final/frect.h \
	include/final/fscrollbar.h \
	include/final/fscrollview.h \
	include/final/fstatusbar.h \
	include/final/fstring.h \
	include/final/ftermcap.h \
	include/final/ftermcapquirks.h \
	include/final/ftermxterminal.h \
	include/final/ftermfreebsd.h \
	include/final/ftermopenbsd.h \
	include/final/ftermlinux.h \
	include/final/ftermdebugdata.h \
	include/final/ftermdetection.h \
	include/final/ftermios.h \
	include/final/fterm.h \
	include/final/ftermdata.h \
	include/final/fvterm.h \
	include/final/ftextview.h \
	include/final/ftogglebutton.h \
	include/final/fcolorpalette.h \
	include/final/fwidgetcolors.h \
	include/final/fwidget.h \
	include/final/fwindow.h

clean-local:
	-find . \( -name "*.gcda" -o -name "*.gcno" -o -name "*.gcov" \) -delete

#uninstall:
#	rm -R -f $(includedir)/final
