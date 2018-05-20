![The Final Cut](logo/png/finalcut-logo.png)

### Building and code analysis
*Travis CI:*<br />
&#160;&#160;&#160;&#160;&#160;[![Build Status](https://travis-ci.org/gansm/finalcut.svg?branch=master)](https://travis-ci.org/gansm/finalcut) <br />
*Coverity Scan:*<br />
&#160;&#160;&#160;&#160;&#160;[![Coverity Scan Status](https://scan.coverity.com/projects/6508/badge.svg)](https://scan.coverity.com/projects/6508) <br />
*Class Reference:*<br />
&#160;&#160;&#160;&#160;&#160;[![documented](https://codedocs.xyz/gansm/finalcut.svg)](https://codedocs.xyz/gansm/finalcut/hierarchy.html)

### Installation
```bash
> git clone git://github.com/gansm/finalcut.git
> cd finalcut
> ./configure --prefix=/usr
> make
> su -c "make install"
```

### Supported platforms
* Linux
* FreeBSD
* NetBSD
* OpenBSD
* macOS
* Cygwin
* Solaris

The Final Cut
=============
The Final Cut is a C++ class library and widget toolkit with full mouse support for creating a [text-based user interface](https://en.wikipedia.org/wiki/Text-based_user_interface). The library supports the programmer to develop an application for the text console. It allows the simultaneous handling of multiple text windows on the screen.
The C++ class design was inspired by the Qt framework. It provides common controls like dialog boxes, push buttons, check boxes, radio buttons, input lines, list boxes, status bars and so on.

![](doc/fileopen-dialog.png)

![](doc/progress-bar.png)

![](doc/textview.png)

![](doc/Mandelbrot.png)


newfont
-------
A [graphical text font](fonts/) for X11 and the Linux console.

![](doc/newfont1.png)

![](doc/newfont2.png)

![](doc/calculator.png)


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
                                                ┌───────────┐
                                           ┌────┤ FKeyEvent │
                                           │    └───────────┘
                                           │    ┌─────────────┐
                                           ├────┤ FMouseEvent │
                                           │    └─────────────┘
              1┌────────────────┐          │    ┌─────────────┐
   ┌-----------┤ FTermDetection │          ├────┤ FWheelEvent │
   :           └────────────────┘          │    └─────────────┘
   :          1┌────────────────┐          │    ┌─────────────┐
   ┌-----------┤ FTermcapQuirks │          ├────┤ FFocusEvent │
   :           └────────────────┘          │    └─────────────┘
   :          1┌──────────┐                │    ┌─────────────┐
   ┌-----------┤ FTermcap │    ┌────────┐  ├────┤ FAccelEvent │
   :           └──────────┘    │ FEvent │◄─┤    └─────────────┘
   :          1┌──────────┐    └───┬────┘  │    ┌──────────────┐
   ┌-----------┤ FTermios │        :1      ├────┤ FResizeEvent │
   :           └──────────┘        :       │    └──────────────┘
   :          1┌────────────────┐  :       │    ┌────────────┐
   ┌-----------┤ FTermXTerminal │  :       ├────┤ FShowEvent │
   :           └────────────────┘  :       │    └────────────┘
   :          1┌───────────────┐   :       │    ┌────────────┐
   ┌-----------┤ FColorPalette │   :       ├────┤ FHideEvent │
   :           └───────────────┘   :       │    └────────────┘
   :          1┌───────────┐       :       │    ┌─────────────┐
   ┌-----------┤ FOptiMove │       :       ├────┤ FCloseEvent │
   :           └───────────┘       :       │    └─────────────┘
   :          1┌───────────┐       :       │    ┌─────────────┐
   ┌-----------┤ FOptiAttr │       :       └────┤ FTimerEvent │
   :           └───────────┘       :            └─────────────┘
   :          1┌───────────────┐   :
   ┌-----------┤ FMouseControl │   :            ┌──────────────┐
   :           └───────────────┘   :       ┌────┤ FApplication │
   :          *┌─────────┐         :       │    └──────────────┘
   :  ┌--------┤ FString │         :       │    ┌─────────┐
   :  :        └─────────┘         :       ├────┤ FButton │
   :  :       *┌────────┐          :       │    └─────────┘
   :  ┌--------┤ FPoint │          :       │    ┌────────┐
   :  :        └────────┘          :       ├────┤ FLabel │
   :  :       *┌───────┐           :       │    └────────┘
   :  ┌--------┤ FRect │           :       │    ┌───────────┐
   :  :        └───────┘           :       ├────┤ FLineEdit │
   :1 :1                           :       │    └───────────┘
 ┌─┴──┴──┐    ┌────────┐           :       │    ┌──────────────┐      ┌──────────────┐
 │ FTerm │◄───┤ FVTerm │◄──┐       :1      ├────┤ FButtonGroup │   ┌──┤ FRadioButton │
 └───────┘    └────────┘   │  ┌────┴────┐  │    └──────────────┘   │  └──────────────┘
                           ├──┤ FWidget │◄─┤    ┌───────────────┐  │  ┌───────────┐
             ┌─────────┐   │  └─────────┘  ├────┤ FToggleButton │◄─┼──┤ FCheckBox │
             │ FObject │◄──┘               │    └───────────────┘  │  └───────────┘
             └─────────┘                   │    ┌──────────────┐   │  ┌─────────┐
                                           ├────┤ FProgressbar │   └──┤ FSwitch │
                                           │    └──────────────┘      └─────────┘
                                           │    ┌────────────┐
                                           ├────┤ FScrollbar │
                                           │    └────────────┘
                                           │    ┌───────────┐
                                           ├────┤ FTextView │
                                           │    └───────────┘
                                           │    ┌──────────┐1     *┌──────────────┐
                                           ├────┤ FListBox ├-------┤ FListBoxItem │
                                           │    └──────────┘       └──────────────┘
 ┌─────────────┐1                          │   1┌───────────┐1    *┌───────────────┐
 │ FTermBuffer ├---------------------------├────┤ FListView ├------┤ FListViewItem │
 └─────────────┘                           │    └───────────┘      └───────────────┘
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
                                             1:                      └───────┘   │
                                              :            ┌─────────────────┐   │
                                              :            │ FDialogListMenu ├───┘
                                              :            └─────────────────┘
                                              └--------------------------------┐
                                              :*          ┌────────────────┐*  :
                                        ┌─────┴─────┐  ┌──┤ FCheckMenuItem ├---┘
                                        │ FMenuItem │◄─┤  └────────────────┘   :
                                        └───────────┘  │  ┌────────────────┐*  :
                                                       └──┤ FRadioMenuItem ├---┘
                                                          └────────────────┘
</pre>

License
-------
GNU Lesser General Public License Version 3 

Please send bug reports to
--------------------------
https://github.com/gansm/finalcut/issues

