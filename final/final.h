/***********************************************************************
* final.h - Include all in FINAL CUT required functions,               *
*           types and macros                                           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2023 Markus Gans                                      *
*                                                                      *
* FINAL CUT is free software; you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as       *
* published by the Free Software Foundation; either version 3 of       *
* the License, or (at your option) any later version.                  *
*                                                                      *
* FINAL CUT is distributed in the hope that it will be useful, but     *
* WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU Lesser General Public License for more details.                  *
*                                                                      *
* You should have received a copy of the GNU Lesser General Public     *
* License along with this program.  If not, see                        *
* <http://www.gnu.org/licenses/>.                                      *
***********************************************************************/

#ifndef FINAL_H
#define FINAL_H

#define USE_FINAL_H

#include <final/fapplication.h>
#include <final/fc.h>
#include <final/fevent.h>
#include <final/fobject.h>
#include <final/fstartoptions.h>
#include <final/ftimer.h>
#include <final/ftypes.h>
#include <final/fwidgetcolors.h>
#include <final/fwidget_flags.h>
#include <final/fwidget.h>
#include <final/dialog/fdialog.h>
#include <final/dialog/ffiledialog.h>
#include <final/dialog/fmessagebox.h>
#include <final/eventloop/backend_monitor.h>
#include <final/eventloop/eventloop.h>
#include <final/eventloop/io_monitor.h>
#include <final/eventloop/monitor.h>
#include <final/eventloop/signal_monitor.h>
#include <final/eventloop/timer_monitor.h>
#include <final/input/fkeyboard.h>
#include <final/input/fkey_map.h>
#include <final/input/fmouse.h>
#include <final/menu/fcheckmenuitem.h>
#include <final/menu/fdialoglistmenu.h>
#include <final/menu/fmenubar.h>
#include <final/menu/fmenu.h>
#include <final/menu/fmenuitem.h>
#include <final/menu/fradiomenuitem.h>
#include <final/output/fcolorpalette.h>
#include <final/output/foutput.h>
#include <final/output/tty/fcharmap.h>
#include <final/output/tty/foptiattr.h>
#include <final/output/tty/foptimove.h>
#include <final/output/tty/ftermcap.h>
#include <final/output/tty/ftermcapquirks.h>
#include <final/output/tty/ftermdata.h>
#include <final/output/tty/ftermdebugdata.h>
#include <final/output/tty/ftermdetection.h>
#include <final/output/tty/fterm.h>
#include <final/output/tty/ftermios.h>
#include <final/output/tty/ftermoutput.h>
#include <final/output/tty/ftermxterminal.h>
#include <final/output/tty/sgr_optimizer.h>
#include <final/util/char_ringbuffer.h>
#include <final/util/emptyfstring.h>
#include <final/util/fdata.h>
#include <final/util/flogger.h>
#include <final/util/flog.h>
#include <final/util/fpoint.h>
#include <final/util/frect.h>
#include <final/util/fsize.h>
#include <final/util/fstring.h>
#include <final/util/fsystem.h>
#include <final/vterm/fcolorpair.h>
#include <final/vterm/fstyle.h>
#include <final/vterm/fvtermbuffer.h>
#include <final/vterm/fvterm.h>
#include <final/widget/fbusyindicator.h>
#include <final/widget/fbuttongroup.h>
#include <final/widget/fbutton.h>
#include <final/widget/fcheckbox.h>
#include <final/widget/fcombobox.h>
#include <final/widget/flabel.h>
#include <final/widget/flineedit.h>
#include <final/widget/flistbox.h>
#include <final/widget/flistview.h>
#include <final/widget/fprogressbar.h>
#include <final/widget/fradiobutton.h>
#include <final/widget/fscrollbar.h>
#include <final/widget/fscrollview.h>
#include <final/widget/fspinbox.h>
#include <final/widget/fstatusbar.h>
#include <final/widget/fswitch.h>
#include <final/widget/ftextview.h>
#include <final/widget/ftogglebutton.h>
#include <final/widget/ftooltip.h>
#include <final/widget/fwindow.h>

#if defined(UNIT_TEST)
  #include <final/output/tty/ftermlinux.h>
  #include <final/output/tty/ftermfreebsd.h>
  #include <final/output/tty/ftermopenbsd.h>
#elif defined(__linux__)
  #include <final/output/tty/ftermlinux.h>
#elif defined(__FreeBSD__) || defined(__DragonFly__)
  #include <final/output/tty/ftermfreebsd.h>
#elif defined(__NetBSD__) || defined(__OpenBSD__)
  #include <final/output/tty/ftermopenbsd.h>
#endif

#undef USE_FINAL_H

#endif  // FINAL_H
