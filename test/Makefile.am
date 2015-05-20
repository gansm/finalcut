#----------------------------------------------------------------------
# Makefile.am  -  The Final Cut example programs
#----------------------------------------------------------------------

AM_LDFLAGS = -L$(top_builddir)/src/.libs -lfinal
AM_CPPFLAGS = -Wall -Werror -I$(top_srcdir)/src

noinst_PROGRAMS = \
	hello \
	dialog \
	fstring \
	timer \
	ui 

hello_SOURCES = hello.cpp
dialog_SOURCES = dialog.cpp
fstring_SOURCES = fstring.cpp
timer_SOURCES = timer.cpp
ui_SOURCES = ui.cpp

