#----------------------------------------------------------------------
# Makefile.am  -  The Final Cut example programs
#----------------------------------------------------------------------

AM_LDFLAGS = -L$(top_builddir)/src/.libs -lfinal
AM_CPPFLAGS = -Wall -Werror -I$(top_srcdir)/src

noinst_PROGRAMS = \
	hello \
	dialog \
	input-dialog \
	mandelbrot \
	calculator \
	fstring \
	watch \
	timer \
	ui

hello_SOURCES = hello.cpp
dialog_SOURCES = dialog.cpp
input_dialog_SOURCES = input-dialog.cpp
mandelbrot_SOURCES = mandelbrot.cpp
calculator_SOURCES = calculator.cpp
fstring_SOURCES = fstring.cpp
watch_SOURCES = watch.cpp
timer_SOURCES = timer.cpp
ui_SOURCES = ui.cpp

