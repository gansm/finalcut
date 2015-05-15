The Final Cut
=============
The Final Cut is a programming library and widget toolkit that creates a text-based user interface with full mouse support. It supports the controlled handling of multiple simultaneous windows.  
The C++ class design is inspired by the Qt framework. It offers common user interface elements like dialog windows, push buttons, check boxes, radio buttons, input lines, list boxes, status bars and so on.

![](https://github.com/gansm/finalcut/blob/master/doc/fileopen-dialog.png)  

![](https://github.com/gansm/finalcut/blob/master/doc/progress-bar.png)  

![](https://github.com/gansm/finalcut/blob/master/doc/textview.png)  


newfont
-------
A new text font for X11 and the Linux console.
![](https://github.com/gansm/finalcut/blob/master/doc/newfont1.png)  

![](https://github.com/gansm/finalcut/blob/master/doc/newfont2.png)


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
                     │1     ├────┤ FResizeEvent │
                     │      │    └──────────────┘
                     │      │    ┌────────────┐
                     │      ├────┤ FShowEvent │
                     │      │    └────────────┘
                     │      │    ┌────────────┐
                     │      ├────┤ FHideEvent │
                     │      │    └────────────┘
                     │      │    ┌─────────────┐
                     │      ├────┤ FCloseEvent │
                     │      │    └─────────────┘
                     │      │    ┌─────────────┐
                     │      └────┤ FTimerEvent │
                     │           └─────────────┘
                     │
                     │           ┌──────────────┐
                     │      ┌────┤ FApplication │
                     │      │    └──────────────┘
                     │      │    ┌─────────┐1
                     │      ├────┤ FButton ├-----------------------------.
                     │      │    └─────────┘                             │
                     │      │    ┌────────┐1                             │
                     │      ├────┤ FLabel ├------------------------------┤
                     │      │    └────────┘                              │
                     │      │    ┌───────────┐1                          │
                     │      ├────┤ FLineEdit ├---------------------------┤
                     │      │    └───────────┘         ┌──────────────┐1 │
                     │      │    ┌───────────────┐  ┌──┤ FRadioButton ├--┤
                     │1     ├────┤ FToggleButton │◄─┤  └──────────────┘  │
 ┌─────────┐   ┌─────┴───┐  │    └───────────────┘  │  ┌───────────┐1    │
 │ FObject │◄─┬┤ FWidget │◄─┤    ┌──────────────┐   └──┤ FCheckBox ├-----┤
 └─────────┘  │└───┬─┬───┘  ├────┤ FButtonGroup │      └───────────┘     │
              │    │1│1     │    └──────────────┘                        │
   ┌───────┐  │    │ │      │    ┌──────────────┐                        │
   │ FTerm │◄─┘    │ │      ├────┤ FProgressbar │                        │  *┌─────────┐
   └──┬─┬──┘       │ │      │    └──────────────┘                        ├---┤ FString │
      │1│1         │ │      │    ┌────────────┐                          │   └─────────┘
      │ └----------┤ │      ├────┤ FScrollbar │                          │
      │*           │ │      │    └────────────┘                          │
 ┌────┴──────┐     │ │      │    ┌───────────┐1                          │
 │ FOptiMove │     │ │      ├────┤ FTextView ├---------------------------┤
 └───────────┘     │ │      │    └───────────┘                           │
                   │ │      │    ┌──────────┐1     *┌──────────────┐1    │
                   │ │      ├────┤ FListBox ├-------┤ FListBoxItem ├-----┤
                   │ │      │    └──────────┘       └──────────────┘     │
                   │ │      │    ┌────────────┐1   *┌────────────┐1      │
                   │ │      ├────┤ FStatusBar ├-----┤ FStatusKey ├-------┤
                   │ │      │    └────┬───────┘     └────────────┘       │
                   │ │      │        1└----------------------------------┤
                   │ │      │                         ┌─────────────┐1   │
                   │ │  ┌───┴─────┐  ┌─────────┐   ┌──┤ FFileDialog ├----┤
                   │ │  │ FWindow │◄─┤ FDialog │◄──┤  └─────────────┘    │
                   │ │  └─────────┘  └────┬────┘   │  ┌─────────────┐1   │
                   │ │                    │        └──┤ FMessageBox ├----┤
                   │ │                    │           └─────────────┘    │
                   │ │                   1└------------------------------┤
                   │ │                                                   │
                   │ └---------------------------------------------------'
                   │  *┌────────┐
                   ├---┤ FPoint │
                   │   └────────┘
                   │  *┌───────┐
                   └---┤ FRect │
                       └───────┘
</pre>
