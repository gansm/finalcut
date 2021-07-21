![FINAL CUT](logo/svg/finalcut-logo.svg)

# Library for creating terminal applications with text-based widgets

FINAL CUT is a C++ class library and widget toolkit with full [mouse](doc/mouse-control.md#title-bar-actions-on-mouse-clicks) support for creating a [text-based user interface](https://en.wikipedia.org/wiki/Text-based_user_interface). The library supports the programmer to develop an application for the text console. It allows the simultaneous handling of multiple text windows on the screen.

The structure of the Qt framework was originally the inspiration for the C++ class design of FINAL CUT. It provides common controls like dialog boxes, push buttons, check boxes, radio buttons, input lines, list boxes, status bars and so on.

## Building and code analysis

|                    | Badge |
|-------------------:|:------|
| *Latest release*   | [![Latest Release](https://img.shields.io/github/release/gansm/finalcut.svg)](https://github.com/gansm/finalcut/releases) |
| *License*          | [![license](https://img.shields.io/github/license/gansm/finalcut.svg?colorA=#333)](COPYING) |
| *Class Reference*  | [![documented](https://codedocs.xyz/gansm/finalcut.svg)](https://codedocs.xyz/gansm/finalcut/hierarchy.html) |
| *ARM build*        | [![ARM build](https://github.com/gansm/finalcut/actions/workflows/arm.yml/badge.svg)](https://github.com/gansm/finalcut/actions/workflows/arm.yml) |
| *FreeBSD build*    | [![FreeBSD build](https://github.com/gansm/finalcut/actions/workflows/freebsd.yml/badge.svg)](https://github.com/gansm/finalcut/actions/workflows/freebsd.yml) |
| *OpenBSD build*    | [![OpenBSD build](https://github.com/gansm/finalcut/actions/workflows/openbsd.yml/badge.svg)](https://github.com/gansm/finalcut/actions/workflows/openbsd.yml) |
| *macOS build*      | [![macOS build](https://github.com/gansm/finalcut/actions/workflows/macos.yml/badge.svg)](https://github.com/gansm/finalcut/actions/workflows/macos.yml) |
| *Cygwin build*     | [![Cygwin build](https://github.com/gansm/finalcut/actions/workflows/cygwin.yml/badge.svg)](https://github.com/gansm/finalcut/actions/workflows/cygwin.yml) |
| *Solaris build*    | [![Solaris build](https://github.com/gansm/finalcut/actions/workflows/solaris.yml/badge.svg)](https://github.com/gansm/finalcut/actions/workflows/solaris.yml) |
| *CodeQL analysis*  | [![CodeQL analysis](https://github.com/gansm/finalcut/actions/workflows/codeql-analysis.yml/badge.svg)](https://github.com/gansm/finalcut/actions/workflows/codeql-analysis.yml) |
| *LGTM*             | [![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/gansm/finalcut.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/gansm/finalcut/context:cpp) |
| *Coverity Scan*    | [![Coverity Scan Status](https://img.shields.io/coverity/scan/6508.svg)](https://scan.coverity.com/projects/6508 ) |
| *SonarCloud*       | [![Quality gate](https://sonarcloud.io/api/project_badges/measure?project=gansm_finalcut&metric=alert_status)](https://sonarcloud.io/dashboard?id=gansm_finalcut) |
| *CodeFactor*       | [![CodeFactor](https://www.codefactor.io/repository/github/gansm/finalcut/badge)](https://www.codefactor.io/repository/github/gansm/finalcut) |

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

See the [first steps](doc/first-steps.md#first-steps-with-the-final-cut-widget-toolkit) documentation for information on how to use the library.

## Some screenshots

The FFileDialog widget with incremental file name search:

![FFileDialog](doc/fileopen-dialog.png)

The FINAL CUT FProgressbar widget:

![FProgressbar](doc/progress-bar.png)

Scrollable text in the FTextView widget:

![FTextView](doc/textview.png)

The Mandelbrot set example:

![Mandelbrot set](doc/Mandelbrot.png)

## newfont

A [graphical text font](fonts/) for X11 and the Linux console.

![ui example in newfont mode](doc/newfont1.png)

Newfont drive symbols:

![drive symbols](doc/newfont2.png)

The calculator example in newfont mode:

![calculator](doc/calculator.png)

## Benchmark

Here you can find a test for [measuring the character speed](doc/benchmark.md#benchmark) in the terminal.

## Virtual terminal

FINAL CUT uses a virtual terminal to print character via an update method on the screen. It provides (as an overlying layer) virtual windows for the realization of window movements. The update methods only transfer differences to the virtual terminal or physical screen.

<pre style="line-height: 1 !important;">
 print(...)
printf(...)
  │
  │          ╔════════════════════════[ vterm ]════════════════════════╗
  │          ║createVTerm()                                            ║
  │          ║                               ┌ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ┐ ║
  │          ║                                                         ║
  │          ║                               │ restoreVTerm(x,y,w,h) │ ║
  │  ┌───────╨────[ vwin ]────────────┐                                ║
  │  │createArea(area)                │      └ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ┘ ║
  │  │                                │                                ║
  │  │                                │                                ║
  └──┼────────►                  putArea(area) ────►                   ║
     │                                │                                ║
     │                         putArea(x,y,area) ────►                 ║
     │                                │                                ║
     │                   ◄──── getArea(x,y,area)                       ║
     │                                │                                ║
     │                                │                                ║
     │                                │                                ║
     │                resizeArea(area)│                                ║
     └───────╥────────────────────────┘                                ║
             ║                                                         ║
             ║       │                                    resizeVTerm()║
             ╚═══════▼═════════════════════════════════════════════════╝
                     │
                     │   putVTerm()
                     └──────────────────► updateTerminalLine(y)
                       updateTerminal()            │
                                                   ▼
                                           ┌───────────────┐
                                           │ output_buffer │
                                           └───────────────┘
                                                   │
                                                   │ flushOutputBuffer()
                                                   │ and putchar(char)
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

## Class digramm

<pre style="line-height: 1 !important;">
              1┌────────────┐
   ┌-----------┤ FTermLinux │
   :           └────────────┘
   :          1┌──────────────┐
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
   :          1┌─────────┐         │    ┌────────────┐1    1┌───────┐
   ┌-----------┤ FSystem │         ├────┤ FUserEvent ├------┤ FData │
   :           └─────────┘         │    └────────────┘      └───────┘
   :          *┌─────────┐         │       ┌──────┐   ┌─────────┐
   :  ┌--------┤ FString │         │       │ FLog │◄──┤ FLogger │
   :  :        └─────────┘         │       └──┬───┘   └─────────┘
   :  :       *┌───────────────┐   │          :1
   :  ┌--------┤ FStringStream │   │       ┌──┴───────────┐
   :  :        └───────────────┘   │  ┌────┤ FApplication │
   :  :       *┌────────┐          │  │    └──────────────┘
   :  ┌--------┤ FPoint │          │  │    ┌────────┐
   :  :        └────────┘          │  ├────┤ FLabel │
   :  :       *┌───────┐           │  │    └────────┘
   :  ┌--------┤ FRect │           │  │    ┌───────────┐
   :  :        └───────┘           │  ├────┤ FLineEdit │
   :  :       *┌───────┐           │  │    └───────────┘
   :  ┌--------┤ FSize │           │  │    ┌──────────┐
   :  :        └───────┘           │  ├────┤ FSpinBox │
   :1 :1                           │  │    └──────────┘
 ┌─┴──┴──┐                         │  │    ┌─────────┐
 │ FTerm │                         │  ├────┤ FButton │
 └───┬───┘         ┌────────┐      │  │    └─────────┘
     :1            │ FEvent │◄─────┘  │    ┌──────────────┐      ┌──────────────┐
 ┌───┴────┐        └────┬───┘         ├────┤ FButtonGroup │   ┌──┤ FRadioButton │
 │ FVTerm │◄──┐         :1            │    └──────────────┘   │  └──────────────┘
 └────────┘   │    ┌────┴────┐        │    ┌───────────────┐  │  ┌───────────┐
              ├────┤ FWidget │◄───────┼────┤ FToggleButton │◄─┼──┤ FCheckBox │
┌─────────┐   │    └────┬────┘        │    └───────────────┘  │  └───────────┘
│ FObject │◄──┘         :1            │    ┌──────────────┐   │  ┌─────────┐
└─────────┘      ┌──────┴────────┐    ├────┤ FProgressbar │   └──┤ FSwitch │
                 │ FWidgetColors │    │    └──────────────┘      └─────────┘
                 └───────────────┘    │    ┌────────────┐
                                      ├────┤ FScrollbar │
                                      │    └────────────┘
                                      │    ┌───────────┐
                                      ├────┤ FTextView │
                                      │    └───────────┘
                                      │    ┌───────────┐1    1┌──────────────────┐
                                      ├────┤ FComboBox ├------┤ FDropDownListBox │
                                      │    └───────────┘      └──────────────────┘
 ┌─────────────┐1                     │    ┌──────────┐1     *┌──────────────┐1
 │ FTermBuffer ├----------------------├────┤ FListBox ├-------┤ FListBoxItem ├--┐
 └─────────────┘                      │    └──────────┘       └──────────────┘  :
                                      │   1┌───────────┐1    *┌───────────────┐ :
                                      ├────┤ FListView ├------┤ FListViewItem │ :
                                      │    └───────────┘      └────────┬──────┘ :
                                      │    ┌─────────────┐             :1       :
                                      ├────┤ FScrollView │         ┌───┴───┐1   :
                                      │    └─────────────┘         │ FData ├----┘
                                      │                            └───────┘
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
                                     │  │      ┌──────────┐  ┌────────────────┐
                                     │  └──────┤ FToolTip │◄─┤ FBusyIndicator │
                                     │         └──────────┘  └────────────────┘
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

## Frequently Asked Questions

For general questions about FINAL CUT, likely the answer is already included in the [FAQ](doc/faq.md#frequently-asked-questions).

## Please send bug reports to

https://github.com/gansm/finalcut/issues

## License

GNU Lesser General Public License Version 3 <a href="https://www.gnu.org/licenses/lgpl-3.0-standalone.html"><img width="200" align="right" src="https://camo.githubusercontent.com/726b87cc2ebaf8c40716842ff509c5f874381c8e/68747470733a2f2f75706c6f61642e77696b696d656469612e6f72672f77696b6970656469612f636f6d6d6f6e732f332f33622f4c47504c76335f4c6f676f2e737667" alt="LGPLv3" data-canonical-src="https://upload.wikimedia.org/wikipedia/commons/3/3b/LGPLv3_Logo.svg"></a>
