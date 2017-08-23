#----------------------------------------------------------------------
# Makefile.am  -  The Final Cut library
#----------------------------------------------------------------------

SUBDIRS = fonts

AM_CPPFLAGS = -Wall -Werror

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
	ftextview.cpp \
	fstatusbar.cpp \
	fterm.cpp \
	fvterm.cpp \
	fevent.cpp \
	foptiattr.cpp \
	foptimove.cpp \
	ftermbuffer.cpp \
	fapplication.cpp \
	fwidget.cpp \
	fobject.cpp

libfinal_la_LDFLAGS = -version-info @SO_VERSION@

finalcutincludedir = $(includedir)/final

finalcutinclude_HEADERS = \
	fapplication.h \
	fbutton.h \
	fbuttongroup.h \
	fcharmap.h \
	fcheckbox.h \
	fconfig.h \
	fswitch.h \
	fdialog.h \
	fc.h \
	fevent.h \
	ffiledialog.h \
	final.h \
	fkey_map.h \
	flabel.h \
	flineedit.h \
	flistbox.h \
	flistview.h \
	fmenu.h \
	fdialoglistmenu.h \
	fmenubar.h \
	fmenuitem.h \
	fradiomenuitem.h \
	fcheckmenuitem.h \
	fmenulist.h \
	fmessagebox.h \
	ftooltip.h \
	fobject.h \
	fpoint.h \
	foptiattr.h \
	foptimove.h \
	ftermbuffer.h \
	fprogressbar.h \
	fradiobutton.h \
	frect.h \
	fscrollbar.h \
	fscrollview.h \
	fstatusbar.h \
	fstring.h \
	ftcap_map.h \
	ftermcap.h \
	fterm.h \
	fvterm.h \
	ftextview.h \
	ftogglebutton.h \
	fwidget.h \
	fwindow.h

#uninstall:
#	rm -R -f $(includedir)/final

