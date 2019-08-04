/***********************************************************************
* final.h - Include all in the Final Cut required functions, types     *
*           and macros                                                 *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2014-2019 Markus Gans                                      *
*                                                                      *
* The Final Cut is free software; you can redistribute it and/or       *
* modify it under the terms of the GNU Lesser General Public License   *
* as published by the Free Software Foundation; either version 3 of    *
* the License, or (at your option) any later version.                  *
*                                                                      *
* The Final Cut is distributed in the hope that it will be useful,     *
* but WITHOUT ANY WARRANTY; without even the implied warranty of       *
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

#include <final/emptyfstring.h>
#include <final/fkey_map.h>
#include <final/fapplication.h>
#include <final/fbuttongroup.h>
#include <final/fbutton.h>
#include <final/fc.h>
#include <final/fcolorpair.h>
#include <final/fcharmap.h>
#include <final/fcheckbox.h>
#include <final/fcheckmenuitem.h>
#include <final/fdialog.h>
#include <final/fdialoglistmenu.h>
#include <final/fevent.h>
#include <final/ffiledialog.h>
#include <final/fkeyboard.h>
#include <final/flabel.h>
#include <final/flineedit.h>
#include <final/flistbox.h>
#include <final/flistview.h>
#include <final/fmenubar.h>
#include <final/fmenu.h>
#include <final/fmenuitem.h>
#include <final/fmessagebox.h>
#include <final/fmouse.h>
#include <final/foptiattr.h>
#include <final/foptimove.h>
#include <final/fpoint.h>
#include <final/fprogressbar.h>
#include <final/fradiobutton.h>
#include <final/fradiomenuitem.h>
#include <final/frect.h>
#include <final/fscrollbar.h>
#include <final/fstatusbar.h>
#include <final/fstring.h>
#include <final/fswitch.h>
#include <final/fsystem.h>
#include <final/fterm.h>
#include <final/ftermios.h>
#include <final/ftermcap.h>
#include <final/ftermcapquirks.h>
#include <final/ftermdata.h>
#include <final/ftermdebugdata.h>
#include <final/ftermdetection.h>
#include <final/ftextview.h>
#include <final/ftooltip.h>
#include <final/ftypes.h>
#include <final/fwidget.h>

#if defined(UNIT_TEST)
  #include <final/ftermlinux.h>
  #include <final/ftermfreebsd.h>
  #include <final/ftermopenbsd.h>
#elif defined(__linux__)
  #include <final/ftermlinux.h>
#elif defined(__FreeBSD__) || defined(__DragonFly__)
  #include <final/ftermfreebsd.h>
#elif defined(__NetBSD__) || defined(__OpenBSD__)
  #include <final/ftermopenbsd.h>
#endif

#undef USE_FINAL_H

#endif  // FINAL_H
