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
	fmenubar.cpp \
	fmenuitem.cpp \
	fmenulist.cpp \
	fdialog.cpp \
	fwindow.cpp \
	fmessagebox.cpp \
	ffiledialog.cpp \
	ftextview.cpp \
	fstatusbar.cpp \
	fterm.cpp \
	fevent.cpp \
	foptimove.cpp \
	fapp.cpp \
	fwidget.cpp \
	fobject.cpp

libfinal_la_LDFLAGS = -version-info @SO_VERSION@

finalcutincludedir = $(includedir)/final

finalcutinclude_HEADERS = \
	fapp.h \
	fbutton.h \
	fbuttongroup.h \
	fcharmap.h \
	fcheckbox.h \
	fswitch.h \
	fdialog.h \
	fenum.h \
	fevent.h \
	ffiledialog.h \
	final.h \
	fkey_map.h \
	flabel.h \
	flineedit.h \
	flistbox.h \
	fmenubar.h \
	fmenuitem.h \
	fmenulist.h \
	fmessagebox.h \
	fobject.h \
	foptimove.h \
	fpoint.h \
	fprogressbar.h \
	fradiobutton.h \
	frect.h \
	fscrollbar.h \
	fstatusbar.h \
	fstring.h \
	ftcap_map.h \
	fterm.h \
	ftextview.h \
	ftogglebutton.h \
	fwidget.h \
	fwindow.h

#uninstall:
#	rm -R -f $(includedir)/final

