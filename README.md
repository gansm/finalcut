![The Final Cut](logo/png/finalcut-logo.png)

### Building and code analysis
*Travis CI:*<br />
&#160;&#160;&#160;&#160;&#160;[![Build Status](https://travis-ci.org/gansm/finalcut.svg?branch=master)](https://travis-ci.org/gansm/finalcut) <br />
*Coverity Scan:*<br />
&#160;&#160;&#160;&#160;&#160;[![Coverity Scan Status](https://scan.coverity.com/projects/6508/badge.svg)](https://scan.coverity.com/projects/6508)

### Installation
```
> git clone git://github.com/gansm/finalcut.git
> cd finalcut
> ./configure --prefix=/usr
> make
> su -c "make install"
```

The Final Cut
=============
The Final Cut is a class library and widget toolkit with full mouse support for creating a text-based user interface. The library supports the programmer to develop an application for the text console. It allows the simultaneous handling of multiple windows on the screen.  
The C++ class design was inspired by the Qt framework. It provides common controls like dialog windows, push buttons, check boxes, radio buttons, input lines, list boxes, status bars and so on.

![](doc/fileopen-dialog.png)  

![](doc/progress-bar.png)  

![](doc/textview.png)  

![](doc/Mandelbrot.png)  


newfont
-------
A new text font for X11 and the Linux console.
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
                  │    putVTerm()      ┌───────────────┐
                  └───────────────────►│ output_buffer │
                    updateTerminal()   └───────┬───────┘
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
                                           │    ┌─────────────┐
                                           ├────┤ FWheelEvent │
                                           │    └─────────────┘
                                           │    ┌─────────────┐
                                           ├────┤ FFocusEvent │
                                           │    └─────────────┘
                                           │    ┌─────────────┐
                               ┌────────┐  ├────┤ FAccelEvent │
                               │ FEvent │◄─┤    └─────────────┘
                               └────┬───┘  │    ┌──────────────┐
                                    :1     ├────┤ FResizeEvent │
                                    :      │    └──────────────┘
                                    :      │    ┌────────────┐
                                    :      ├────┤ FShowEvent │
                                    :      │    └────────────┘
                                    :      │    ┌────────────┐
                                    :      ├────┤ FHideEvent │
                                    :      │    └────────────┘
                                    :      │    ┌─────────────┐
                                    :      ├────┤ FCloseEvent │
                                    :      │    └─────────────┘
                                    :      │    ┌─────────────┐
                                    :      └────┤ FTimerEvent │
                                    :           └─────────────┘
                                    :
                                    :           ┌──────────────┐
                                    :      ┌────┤ FApplication │
                                    :      │    └──────────────┘
                                    :      │    ┌─────────┐1
                                    :      ├────┤ FButton ├-----------------------------┐
                                    :      │    └─────────┘                             :
                                    :      │    ┌────────┐1                             :
                                    :      ├────┤ FLabel ├------------------------------┐
                                    :      │    └────────┘                              :
                                    :      │    ┌───────────┐1                          :
                                    :      ├────┤ FLineEdit ├---------------------------┐
                                    :      │    └───────────┘                           :
                                    :      │    ┌──────────────┐      ┌──────────────┐1 :
                                    :1     ├────┤ FButtonGroup │   ┌──┤ FRadioButton ├--┐
 ┌─────────┐    ┌────────┐    ┌─────┴───┐  │    └──────────────┘   │  └──────────────┘  :
 │ FObject │◄─┬─┤ FVTerm │◄───┤ FWidget │◄─┤    ┌───────────────┐  │  ┌───────────┐1    :
 └─────────┘  │ └────┬───┘    └───┬─┬───┘  ├────┤ FToggleButton │◄─┼──┤ FCheckBox ├-----┐
              │      :1           :1:1     │    └───────────────┘  │  └───────────┘     :
   ┌───────┐  │      :            : :      │    ┌──────────────┐   │  ┌─────────┐1      :
   │ FTerm │◄─┘      └------------┐ :      ├────┤ FProgressbar │   └──┤ FSwitch ├-------┐
   └─┬───┬─┘                      : :      │    └──────────────┘      └─────────┘       :
     :1  :1                       : :      │    ┌────────────┐                          :  *┌─────────┐
     :   └------------------------┐ :      ├────┤ FScrollbar │                          ├---┤ FString │
     :                            : :      │    └────────────┘                          :   └─────────┘
     :  1┌───────────┐            : :      │    ┌───────────┐1                          :
     └---┤ FOptiAttr │            : :      ├────┤ FTextView ├---------------------------┘
     :   └───────────┘            : :      │    └───────────┘                           :
     :  1┌───────────┐            : :      │    ┌──────────┐1     *┌──────────────┐1    :
     └---┤ FOptiMove │            : :      ├────┤ FListBox ├-------┤ FListBoxItem ├-----┘
         └───────────┘            : :      │    └──────────┘       └──────────────┘     :
                                  : :      │    ┌────────────┐1   *┌────────────┐1      :
                                  : :      │ ┌──┤ FStatusBar ├-----┤ FStatusKey ├-------┘
                                  : :      │ │  └────┬───────┘     └────────────┘       :
                                  : :      │ │      1└----------------------------------┘
                                  : :      │ ▼                       ┌─────────────┐1   :
                                  : :  ┌───┴─┴───┐  ┌─────────┐   ┌──┤ FFileDialog ├----┘
                                  : :  │ FWindow │◄─┤ FDialog │◄──┤  └─────────────┘    :
                                  : :  └──┬──┬───┘  └────┬────┘   │  ┌─────────────┐1   :
                                  : :     ▲  ▲          1:        └──┤ FMessageBox ├----┘
                                  : :     │  │           :           └─────────────┘    :
                                  : :     │  │           └------------------------------┘
                                  : :     │  │      ┌──────────┐                        :
                                  : :     │  └──────┤ FToolTip ├------------------------┘
                                  : :     │         └──────────┘                        :
                                  : :     └───────────────┐          ┌──────────┐       :
                                  : :                     │      ┌───┤ FMenuBar │       :
                                  : :    ┌───────────┐    └──────┤   └──────────┘       :
                                  : :    │ FMenuList │◄──────────┤   ┌───────┐          :
                                  : :    └────┬──────┘           └───┤ FMenu │◄──┐      :
                                  : :        1:                      └───────┘   │      :
                                  : :         :            ┌─────────────────┐   │      :
                                  : :         :            │ FDialogListMenu ├───┘      :
                                  : :         :            └─────────────────┘          :
                                  : :         :                    ┌────────────────┐*  :
                                  : :         : *┌───────────┐  ┌──┤ FCheckMenuItem ├-┐ :
                                  : :         ├--┤ FMenuItem │◄─┤  └────────────────┘ : :
                                  : :         :  └───────────┘  │  ┌────────────────┐*: :
                                  : :         :                 └──┤ FRadioMenuItem ├-┤ :
                                  : :         :                    └────────────────┘ : :
                                  : :         └---------------------------------------┘ :
                                  : └---------------------------------------------------┘
                                  :  *┌────────┐
                                  └---┤ FPoint │
                                  :   └────────┘
                                  :  *┌───────┐
                                  └---┤ FRect │
                                      └───────┘
</pre>
