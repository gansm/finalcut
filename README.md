![FINAL CUT](logo/svg/finalcut-logo.svg)
============================================

# Library for creating terminal applications with text-based widgets
The FINAL CUT is a C++ class library and widget toolkit with full mouse support for creating a [text-based user interface](https://en.wikipedia.org/wiki/Text-based_user_interface). The library supports the programmer to develop an application for the text console. It allows the simultaneous handling of multiple text windows on the screen.

The structure of the Qt framework was originally the inspiration for the C++ class design of FINAL CUT. It provides common controls like dialog boxes, push buttons, check boxes, radio buttons, input lines, list boxes, status bars and so on.

## Building and code analysis
*Latest release:*<br />
&#160;&#160;&#160;&#160;&#160;[![Latest Release](https://img.shields.io/github/release/gansm/finalcut.svg)](https://github.com/gansm/finalcut/releases) <br />
*License:*<br />
&#160;&#160;&#160;&#160;&#160;[![license](https://img.shields.io/github/license/gansm/finalcut.svg?colorA=#333)](COPYING) <br />
*Travis CI:*<br />
&#160;&#160;&#160;&#160;&#160;[![Build Status](https://travis-ci.org/gansm/finalcut.svg?branch=master)](https://travis-ci.org/gansm/finalcut) <br />
*Coverity Scan:*<br />
&#160;&#160;&#160;&#160;&#160;[![Coverity Scan Status](https://img.shields.io/coverity/scan/6508.svg)](https://scan.coverity.com/projects/6508) <br />
*LGTM:*<br />
&#160;&#160;&#160;&#160;&#160;[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/gansm/finalcut.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/gansm/finalcut/context:cpp) <br />
*Class Reference:*<br />
&#160;&#160;&#160;&#160;&#160;[![documented](https://codedocs.xyz/gansm/finalcut.svg)](https://codedocs.xyz/gansm/finalcut/hierarchy.html)

## Installation
```bash
> git clone git://github.com/gansm/finalcut.git
> cd finalcut
> autoreconf --install --force
> ./configure --prefix=/usr
> make
> su -c "make install"
```

## Supported platforms
* Linux
* FreeBSD
* NetBSD
* OpenBSD
* macOS
* Cygwin
* Solaris

## First steps
Read here [how to use the library](doc/first-steps.md#first-steps-with-the-final-cut-widget-toolkit)

## Some screenshots

The FFileDialog widget with incremental file name search:

![FFileDialog](doc/fileopen-dialog.png)


The Final Cut FProgressbar widget:

![FProgressbar](doc/progress-bar.png)


Scrollable text in the FTextView widget:

![FTextView](doc/textview.png)


The Mandelbrot set example:

![Mandelbrot set](doc/Mandelbrot.png)


newfont
-------
A [graphical text font](fonts/) for X11 and the Linux console.

![ui example in newfont mode](doc/newfont1.png)


Newfont drive symbols:

![drive symbols](doc/newfont2.png)


The calculator example in newfont mode:

![calculator](doc/calculator.png)


Virtual terminal
----------------
It uses a virtual terminal to print the character via an update method on the screen.
The virtual windows are an overlying layer to realizing window movements.
The update method transmits only the changes to the virtual terminal or the screen.

<pre style="line-height: 1 !important;">
 print(...)
printf(...)
  │
  │           ╔═════════════════════════[ vterm ]═════════════════════════╗
  │           ║createVTerm()                                              ║
  │           ║                                 ┌ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ┐ ║
  │           ║                                                           ║
  │           ║                                 │ restoreVTerm(x,y,w,h) │ ║
  │           ║                                                           ║
  │           ║                                 └ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ┘ ║
  │           ║                                                           ║
  │   ┌───────╨─────[ vwin ]─────────────┐                                ║
  │   │createArea(area)                  │                                ║
  │   │                                  │                                ║
  │   │                                  │                                ║
  └───┼─────────────►     ──────► updateVTerm(area) ────►                 ║
      │                                  │                                ║
      │                           putArea(x,y,area)                       ║
      │                         ────────────────────►                     ║
      │                           getArea(x,y,area)                       ║
      │                        ◄────────────────────                      ║
      │                                  │                                ║
      │                                  │                                ║
      │                  resizeArea(area)│                                ║
      └───────╥──────────────────────────┘                                ║
              ║                                                           ║
              ║                                                           ║
              ║                                                           ║
              ║   │                                          resizeVTerm()║
              ╚═══▼═══════════════════════════════════════════════════════╝
                  │
                  │    putVTerm()
                  └──────────────────► updateTerminalLine(y)
                    updateTerminal()             │
                                                 ▼
                                         ┌───────────────┐
                                         │ output_buffer │
                                         └───────────────┘
                                                 │
                                                 │ flush_out()
                                                 │     +
                                                 │ Fputchar(char)
                                                 │
                                                 ▼
                                         ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
                                         ▌               ▐
                                         ▌    screen     ▐
                                         ▌ ───────────── ▐
                                         ▌ real terminal ▐
                                         ▌               ▐
                                         ▀▀▀▀▀▀▀███▀▀▀▀▀▀▀
                                                ███
                                             ▀▀▀▀▀▀▀▀▀
</pre>


Class digramm
-------------

<pre style="line-height: 1 !important;">
              1┌──────────────┐
   ┌-----------┤ FTermFreeBSD │
   :           └──────────────┘
   :          1┌──────────────┐
   ┌-----------┤ FTermOpenBSD │
   :           └──────────────┘
   :          1┌────────────────┐       ┌───────────┐
   ┌-----------┤ FTermDetection │  ┌────┤ FKeyEvent │
   :           └────────────────┘  │    └───────────┘
   :          1┌────────────────┐  │    ┌─────────────┐
   ┌-----------┤ FTermcapQuirks │  ├────┤ FMouseEvent │
   :           └────────────────┘  │    └─────────────┘
   :          1┌────────────────┐  │    ┌─────────────┐
   ┌-----------┤ FTermXTerminal │  ├────┤ FWheelEvent │
   :           └────────────────┘  │    └─────────────┘
   :          1┌──────────┐        │    ┌─────────────┐
   ┌-----------┤ FTermcap │        ├────┤ FFocusEvent │
   :           └──────────┘        │    └─────────────┘
   :          1┌──────────┐        │    ┌─────────────┐
   ┌-----------┤ FTermios │        ├────┤ FAccelEvent │
   :           └──────────┘        │    └─────────────┘
   :          1┌───────────────┐   │    ┌──────────────┐
   ┌-----------┤ FColorPalette │   ├────┤ FResizeEvent │
   :           └───────────────┘   │    └──────────────┘
   :          1┌───────────┐       │    ┌────────────┐
   ┌-----------┤ FOptiMove │       ├────┤ FShowEvent │
   :           └───────────┘       │    └────────────┘
   :          1┌───────────┐       │    ┌────────────┐
   ┌-----------┤ FOptiAttr │       ├────┤ FHideEvent │
   :           └───────────┘       │    └────────────┘
   :          1┌───────────┐       │    ┌─────────────┐
   ┌-----------┤ FKeyboard │       ├────┤ FCloseEvent │
   :           └───────────┘       │    └─────────────┘
   :          1┌───────────────┐   │    ┌─────────────┐
   ┌-----------┤ FMouseControl │   ├────┤ FTimerEvent │
   :           └───────────────┘   │    └─────────────┘
   :          1┌─────────┐         │
   ┌-----------┤ FSystem │         │
   :           └─────────┘         │
   :          *┌─────────┐         │
   :  ┌--------┤ FString │         │       ┌──────────────┐
   :  :        └─────────┘         │  ┌────┤ FApplication │
   :  :       *┌────────┐          │  │    └──────────────┘
   :  ┌--------┤ FPoint │          │  │    ┌─────────┐
   :  :        └────────┘          │  ├────┤ FButton │
   :  :       *┌───────┐           │  │    └─────────┘
   :  ┌--------┤ FRect │           │  │    ┌────────┐
   :  :        └───────┘           │  ├────┤ FLabel │
   :  :       *┌───────┐           │  │    └────────┘
   :  ┌--------┤ FSize │           │  │    ┌───────────┐
   :  :        └───────┘           │  ├────┤ FLineEdit │
   :1 :1                           │  │    └───────────┘
 ┌─┴──┴──┐                         │  │    ┌──────────────┐      ┌──────────────┐
 │ FTerm │                         │  ├────┤ FButtonGroup │   ┌──┤ FRadioButton │
 └───┬───┘         ┌────────┐      │  │    └──────────────┘   │  └──────────────┘
     :1            │ FEvent │◄─────┘  │    ┌───────────────┐  │  ┌───────────┐
 ┌───┴────┐        └────┬───┘         ├────┤ FToggleButton │◄─┼──┤ FCheckBox │
 │ FVTerm │◄──┐         :1            │    └───────────────┘  │  └───────────┘
 └────────┘   │    ┌────┴────┐        │    ┌──────────────┐   │  ┌─────────┐
              ├────┤ FWidget │◄───────┼────┤ FProgressbar │   └──┤ FSwitch │
┌─────────┐   │    └─────────┘        │    └──────────────┘      └─────────┘
│ FObject │◄──┘                       │    ┌────────────┐
└─────────┘                           ├────┤ FScrollbar │
                                      │    └────────────┘
                                      │    ┌───────────┐
                                      ├────┤ FTextView │
                                      │    └───────────┘
                                      │    ┌──────────┐1     *┌──────────────┐
                                      ├────┤ FListBox ├-------┤ FListBoxItem │
                                      │    └──────────┘       └──────────────┘
 ┌─────────────┐1                     │   1┌───────────┐1    *┌───────────────┐
 │ FTermBuffer ├----------------------├────┤ FListView ├------┤ FListViewItem │
 └─────────────┘                      │    └───────────┘      └───────────────┘
                                      │    ┌─────────────┐
                                      ├────┤ FScrollView │
                                      │    └─────────────┘
                                      │    ┌────────────┐1   *┌────────────┐
                                      │ ┌──┤ FStatusBar ├-----┤ FStatusKey │
                                      │ │  └────────────┘     └────────────┘
                                      │ │
                                      │ ▼                       ┌─────────────┐
                                  ┌───┴─┴───┐  ┌─────────┐   ┌──┤ FFileDialog │
                                  │ FWindow │◄─┤ FDialog │◄──┤  └─────────────┘
                                  └──┬──┬───┘  └─────────┘   │  ┌─────────────┐
                                     ▲  ▲                    └──┤ FMessageBox │
                                     │  │                       └─────────────┘
                                     │  │      ┌──────────┐
                                     │  └──────┤ FToolTip │
                                     │         └──────────┘
                                     └───────────────┐          ┌──────────┐
                                                     │      ┌───┤ FMenuBar │
                                    ┌───────────┐    └──────┤   └──────────┘
                                    │ FMenuList │◄──────────┤   ┌───────┐
                                    └────┬──────┘           └───┤ FMenu │◄──┐
                                         :1                     └───────┘   │
                                         :            ┌─────────────────┐   │
                                         :            │ FDialogListMenu ├───┘
                                         :            └─────────────────┘
                                         └--------------------------------┐
                                         :*          ┌────────────────┐*  :
                                    ┌────┴─────┐  ┌──┤ FCheckMenuItem ├---┘
                                    │FMenuItem │◄─┤  └────────────────┘   :
                                    └──────────┘  │  ┌────────────────┐*  :
                                                  └──┤ FRadioMenuItem ├---┘
                                                     └────────────────┘
</pre>

License
-------
GNU Lesser General Public License Version 3

Frequently Asked Questions
--------------------------
If you have any problems, please read the
[FAQ](doc/faq.md#frequently-asked-questions)
before you give up.

Please send bug reports to
--------------------------
https://github.com/gansm/finalcut/issues

