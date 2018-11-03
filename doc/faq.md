
Frequently Asked Questions
==========================


What is FINAL CUT?
------------------

The Final Cut is a C++ class library and a widget toolkit with full mouse 
support for creating a text-based user interface. It's based on the Termcap
library and has its own cursor optimization and window management.


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
