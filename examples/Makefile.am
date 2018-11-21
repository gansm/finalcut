#----------------------------------------------------------------------
# Makefile.am  -  The Final Cut example programs
#----------------------------------------------------------------------

if ! CPPUNIT_TEST

AM_LDFLAGS = -L$(top_builddir)/src/.libs -lfinal
AM_CPPFLAGS = -I$(top_srcdir)/src/include -Wall -Werror

noinst_PROGRAMS = \
	hello \
	dialog \
	input-dialog \
	choice \
	listbox \
	listview \
	checklist \
	treeview \
	opti-move \
	termcap \
	string-operations \
	mandelbrot \
	calculator \
	watch \
	term-attributes \
	transparent \
	keyboard \
	mouse \
	timer \
	scrollview \
	windows \
	menu \
	ui

hello_SOURCES = hello.cpp
dialog_SOURCES = dialog.cpp
input_dialog_SOURCES = input-dialog.cpp
choice_SOURCES = choice.cpp
listbox_SOURCES = listbox.cpp
listview_SOURCES = listview.cpp
checklist_SOURCES = checklist.cpp
treeview_SOURCES = treeview.cpp
opti_move_SOURCES = opti-move.cpp
string_operations_SOURCES = string-operations.cpp
mandelbrot_SOURCES = mandelbrot.cpp
calculator_SOURCES = calculator.cpp
watch_SOURCES = watch.cpp
term_attributes_SOURCES = term-attributes.cpp
termcap_SOURCES = termcap.cpp
transparent_SOURCES = transparent.cpp
keyboard_SOURCES = keyboard.cpp
mouse_SOURCES = mouse.cpp
timer_SOURCES = timer.cpp
scrollview_SOURCES = scrollview.cpp
windows_SOURCES = windows.cpp
menu_SOURCES = menu.cpp
ui_SOURCES = ui.cpp

endif

clean-local:
	-find . \( -name "*.gcda" -o -name "*.gcno" -o -name "*.gcov" \) -delete
