
Frequently Asked Questions
==========================


What is FINAL CUT?
------------------

The Final Cut is a [C++](https://en.wikipedia.org/wiki/C%2B%2B) class library 
and a widget toolkit with full mouse support for creating a text-based user 
interface. It's based on the Termcap library and has its own cursor 
optimization and window management.


Why did you develop FINAL CUT?
------------------------------

Most Unix [TUIs](https://en.wikipedia.org/wiki/Text-based_user_interface) 
are difficult to use or limited in their functionality. I developed 
FINAL CUT to fix these problems. The main goals in FINAL CUT design were 
to create a library that is easy to use, powerful, visually appealing and 
easily expandable.


Where does the name FINAL CUT come from?
----------------------------------------
From 1991 to 1996 I developed with 
[Borland Pascal](https://en.wikipedia.org/wiki/Turbo_Pascal) 
under [DOS](https://en.wikipedia.org/wiki/DOS) a text-based file manager 
called “The Final Cut”, with the intention to be more visually appealing 
than all other products. The calculator example is, among other things an 
extract from this program.

As I got to learn 
[object-oriented programming](https://en.wikipedia.org/wiki/Object-oriented_programming) (OOP) 
with [C++](https://en.wikipedia.org/wiki/C%2B%2B) at the end of 
the 90s, I noticed significant design weaknesses in my program code. 
I decided to re-implement the code cleanly in C++. This time I wanted to 
avoid old mistakes and do everything right and open source. In the meanwhile, 
I switched from [DOS](https://en.wikipedia.org/wiki/DOS) to 
[Linux](https://en.wikipedia.org/wiki/Linux), so I started the implementing 
of the first widgets (FDialog, FButton, FLineEdit, FScrollbar) and the 
string class FString for the 
[Linux console](https://en.wikipedia.org/wiki/Linux_console). 
Unfortunately, the project lost my attention in 2000.

In 2012, I needed an attractive terminal presentation for a textual data 
structure. So I have started to develop FINAL CUT to a widget library.


Are Windows and DOS supported by FINAL CUT?
-------------------------------------------

You need an operating system environment that implements a POSIX system 
call API. This can be realized in Windows with the Cygwin project.


Is my platform supported?
-------------------------

The supported platforms are Linux, FreeBSD, NetBSD, OpenBSD, macOS, 
Cygwin on Microsoft Windows and Oracle Solaris


What do I need to write my own programs with FINAL CUT?
-------------------------------------------------------

You need a C++ compiler like GCC (g++) or Clang (clang++).


What do I need to build this library?
-------------------------------------

You need three things:

1. C++ compiler like GCC (g++) or Clang (clang++).

2. The GNU building tools

    *For example, in a Debian-based distribution it would be 
    the following packages:*

    * autotools-dev
    * automake
    * autoconf
    * autoconf-archive
    * libtool

3. Development packages for following libraries:

    * C standard library
    * C++ standard library
    * Termcap library *(mostly part of the curses library)*
    * General Purpose Mouse library *(optional, only if you need 
      mouse support on the Linux console)*

    *For example, in a Debian-based distribution it would be the following 
    packages:*

    * libglib2.0-dev
    * libstdc++-6-dev
    * libtinfo-dev
    * libncurses5-dev
    * libgpm-dev


How to compile FINAL CUT without gpm support?
---------------------------------------------

```bash
  ./configure --without-gpm
```


What do I need to compile the unit tests?
-----------------------------------------

You need the unit testing framework CppUnit.


How can I fix display problems?
-------------------------------

* Make sure that the environment variable TERM has the right 
  terminal name.

* Use the command "`msgcat --color=test`" to test whether the terminal 
  displays colors correctly in the terminal.

* If characters are not displayed in the right place on the screen, 
  it may help to disable cursor optimization for your program with 
  the parameter *--no-optimized-cursor*.

* If terminal detection did not work, it is useful to turn off the terminal 
  detection with the parameter *--no-terminal-detection*.

* If the color palette redefinition causes problems, you can switch off 
  the color mapping with the parameter *--no-color-change*.
