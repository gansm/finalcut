
Frequently Asked Questions
==========================


What is FINAL CUT?
------------------

FINAL CUT is a [C++](https://en.wikipedia.org/wiki/C%2B%2B) class library 
and widget toolkit featuring full mouse support for creating text-based user
interfaces (TUI). It is built on the Termcap library and includes its own
cursor optimization and window management.


Why did you develop FINAL CUT?
------------------------------

Most Unix [TUIs](https://en.wikipedia.org/wiki/Text-based_user_interface) 
are either difficult to use or limited in functionality. I developed 
FINAL CUT to solve these issues. My goal was to make it more visually 
impressive than its competitors.


Where does the name FINAL CUT come from?
----------------------------------------

It dates back to a text-based file manager I developed for
[DOS](https://en.wikipedia.org/wiki/DOS) between 1991 and 1996. Built
with [Borland Pascal](https://en.wikipedia.org/wiki/Turbo_Pascal),
"The Final Cut" was designed to outshine any other interface available
at the time—it even included the calculator example still found in the
library today.

When I started learning 
[object-oriented programming](https://en.wikipedia.org/wiki/Object-oriented_programming)
with
[C++](https://en.wikipedia.org/wiki/C%2B%2B) in the late 90s, I realized 
that my original code had some design flaws. This inspired me to rewrite
the project from scratch as an open-source application. After switching
from [DOS](https://en.wikipedia.org/wiki/DOS) to
[Linux](https://en.wikipedia.org/wiki/Linux), I started developing core
widgets, such as FDialog, FButton, and FString, for the
[Linux console](https://en.wikipedia.org/wiki/Linux_console). After
taking a long break starting in 2000, I returned to the project in 2012
when I needed a polished terminal interface for a data structure project.
FINAL CUT eventually evolved into the full widget toolkit it is today.


Which platforms are supported?
------------------------------

Supported platforms include Linux, FreeBSD, NetBSD, OpenBSD, macOS, 
Cygwin (on MS Windows), GNU Hurd, and Oracle Solaris.


What do I need to write my own programs with FINAL CUT?
-------------------------------------------------------

A C++ compiler is required (e.g., GCC or Clang).


What do I need to build this library?
-------------------------------------

You need three things:

1. A C++ compiler (GCC or Clang)

   > *For Debian-based distributions 
   > the following packages are required:*
   >
   > * g++<br />*or*
   > * clang

2. The GNU build tools

   > *For Debian-based distributions 
   > the following packages are required:*
   >
   > * autoconf
   > * autoconf-archive
   > * automake
   > * autotools-dev
   > * libtool
   > * make
   > * pkg-config

3. Development packages for the following libraries:

    * C and C++ standard library
    * Termcap library *(usually part of the curses package)*
    * General Purpose Mouse (GPM) library *(optional, for
        mouse support on the Linux console)*<br /><br />

    > *For Debian-based distributions 
    > the following packages are required:*
    > 
    > * libglib2.0-dev
    > * libgpm-dev
    > * libncurses-dev


How do I compile FINAL CUT without GPM support?
-----------------------------------------------

```bash
  ./configure --without-gpm
```


Which mouse types are supported?
--------------------------------
* Standard xterm mouse tracking (limited to 223 rows/columns)
* Xterm SGR mouse tracking
* URXVT mouse tracking
* General Purpose Mouse (gpm) on the Linux console


What do I need to compile the unit tests?
-----------------------------------------

You require the unit testing framework CppUnit.


How can I fix display problems?
-------------------------------

* Ensure the TERM environment variable is set to the correct terminal 
  name.

* Run `msgcat --color=test` to verify that your terminal supports colors.

* If characters appear misaligned, try disabling cursor optimization
  using the --no-optimized-cursor command line argument.

* If terminal detection fails, you can bypass it with --no-terminal-detection.

* If redefining the color palette causes issues, disable color mapping
  with --no-color-change.


How can I redraw the terminal?
------------------------------

Press Ctrl-L to clear and redraw the entire terminal screen. This is useful
if a background process has corrupted the display.


How to disable mouse tracking and the non-scrollable alternate screen buffer in the GNU Debugger (GDB)
------------------------------------------------------------------------------------------------------

If you are using an xterm-compatible terminal, you can disable mouse
tracking and re-enable the scroll buffer by resetting the terminal settings.
Run the following command within the GNU Debugger:

```
(gdb) printf "%s\n", "\033c\033]104\007"
```
