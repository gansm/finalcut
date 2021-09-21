#!/bin/bash

git mv src/include/final/* src
rmdir  src/include/final
rmdir  src/include
git mv src final

mkdir final/dialog
mkdir final/input
mkdir final/menu
mkdir final/output
mkdir final/output/tty
mkdir final/vterm
mkdir final/util
mkdir final/widget

git mv fonts final/font

git mv final/fmessagebox.cpp final/dialog/fmessagebox.cpp
git mv final/fmessagebox.h final/dialog/fmessagebox.h
git mv final/ffiledialog.cpp final/dialog/ffiledialog.cpp
git mv final/ffiledialog.h final/dialog/ffiledialog.h
git mv final/fdialog.cpp final/dialog/fdialog.cpp
git mv final/fdialog.h final/dialog/fdialog.h

git mv final/fmouse.cpp final/input/fmouse.cpp
git mv final/fmouse.h final/input/fmouse.h
git mv final/fkeyboard.cpp final/input/fkeyboard.cpp
git mv final/fkeyboard.h final/input/fkeyboard.h
git mv final/fkey_map.cpp final/input/fkey_map.cpp
git mv final/fkey_map.h final/input/fkey_map.h

git mv final/fmenu.cpp final/menu/fmenu.cpp
git mv final/fmenu.h final/menu/fmenu.h
git mv final/fmenuitem.cpp final/menu/fmenuitem.cpp
git mv final/fmenuitem.h final/menu/fmenuitem.h
git mv final/fmenubar.cpp final/menu/fmenubar.cpp
git mv final/fmenubar.h final/menu/fmenubar.h
git mv final/fmenulist.cpp final/menu/fmenulist.cpp
git mv final/fmenulist.h final/menu/fmenulist.h
git mv final/fradiomenuitem.cpp final/menu/fradiomenuitem.cpp
git mv final/fradiomenuitem.h final/menu/fradiomenuitem.h
git mv final/fcheckmenuitem.cpp final/menu/fcheckmenuitem.cpp
git mv final/fcheckmenuitem.h final/menu/fcheckmenuitem.h
git mv final/fdialoglistmenu.cpp final/menu/fdialoglistmenu.cpp
git mv final/fdialoglistmenu.h final/menu/fdialoglistmenu.h

git mv final/foutput.cpp final/output/foutput.cpp
git mv final/foutput.h final/output/foutput.h
git mv final/fcolorpalette.cpp final/output/fcolorpalette.cpp
git mv final/fcolorpalette.h final/output/fcolorpalette.h

git mv final/fcharmap.cpp final/output/tty/fcharmap.cpp
git mv final/fcharmap.h final/output/tty/fcharmap.h
git mv final/foptimove.cpp final/output/tty/foptimove.cpp
git mv final/foptimove.h final/output/tty/foptimove.h
git mv final/foptiattr.cpp final/output/tty/foptiattr.cpp
git mv final/foptiattr.h final/output/tty/foptiattr.h
git mv final/fterm.cpp final/output/tty/fterm.cpp
git mv final/fterm.h final/output/tty/fterm.h
git mv final/ftermdata.h final/output/tty/ftermdata.h
git mv final/ftermcap.cpp final/output/tty/ftermcap.cpp
git mv final/ftermcap.h final/output/tty/ftermcap.h
git mv final/ftermcapquirks.cpp final/output/tty/ftermcapquirks.cpp
git mv final/ftermcapquirks.h final/output/tty/ftermcapquirks.h
git mv final/ftermlinux.cpp final/output/tty/ftermlinux.cpp
git mv final/ftermlinux.h final/output/tty/ftermlinux.h
git mv final/fterm_functions.cpp final/output/tty/fterm_functions.cpp
git mv final/fterm_functions.h final/output/tty/fterm_functions.h
git mv final/ftermdetection.cpp final/output/tty/ftermdetection.cpp
git mv final/ftermdetection.h final/output/tty/ftermdetection.h
git mv final/ftermoutput.cpp final/output/tty/ftermoutput.cpp
git mv final/ftermoutput.h final/output/tty/ftermoutput.h
git mv final/ftermopenbsd.cpp final/output/tty/ftermopenbsd.cpp
git mv final/ftermopenbsd.h final/output/tty/ftermopenbsd.h
git mv final/ftermxterminal.cpp final/output/tty/ftermxterminal.cpp
git mv final/ftermxterminal.h final/output/tty/ftermxterminal.h
git mv final/ftermfreebsd.cpp final/output/tty/ftermfreebsd.cpp
git mv final/ftermfreebsd.h final/output/tty/ftermfreebsd.h
git mv final/ftermdebugdata.cpp final/output/tty/ftermdebugdata.cpp
git mv final/ftermdebugdata.h final/output/tty/ftermdebugdata.h
git mv final/ftermios.cpp final/output/tty/ftermios.cpp
git mv final/ftermios.h final/output/tty/ftermios.h
git mv final/sgr_optimizer.cpp final/output/tty/sgr_optimizer.cpp
git mv final/sgr_optimizer.h final/output/tty/sgr_optimizer.h

git mv final/fcolorpair.h final/vterm/fcolorpair.h
git mv final/fstyle.h final/vterm/fstyle.h
git mv final/ftermbuffer.cpp final/vterm/ftermbuffer.cpp
git mv final/ftermbuffer.h final/vterm/ftermbuffer.h
git mv final/fvterm.h final/vterm/fvterm.h
git mv final/fvterm.cpp final/vterm/fvterm.cpp

git mv final/fdata.cpp final/util/fdata.cpp
git mv final/fdata.h final/util/fdata.h
git mv final/fcallback.cpp final/util/fcallback.cpp
git mv final/fcallback.h final/util/fcallback.h
git mv final/flog.cpp final/util/flog.cpp
git mv final/flog.h final/util/flog.h
git mv final/flogger.cpp final/util/flogger.cpp
git mv final/flogger.h final/util/flogger.h
git mv final/fstring.cpp final/util/fstring.cpp
git mv final/fstring.h final/util/fstring.h
git mv final/fstringstream.cpp final/util/fstringstream.cpp
git mv final/fstringstream.h final/util/fstringstream.h
git mv final/emptyfstring.h final/util/emptyfstring.h
git mv final/fsystem.cpp final/util/fsystem.cpp
git mv final/fsystem.h final/util/fsystem.h
git mv final/fsystemimpl.cpp final/util/fsystemimpl.cpp
git mv final/fsystemimpl.h final/util/fsystemimpl.h
git mv final/fsize.cpp final/util/fsize.cpp
git mv final/fsize.h final/util/fsize.h
git mv final/fpoint.cpp final/util/fpoint.cpp
git mv final/fpoint.h final/util/fpoint.h
git mv final/frect.cpp final/util/frect.cpp
git mv final/frect.h final/util/frect.h

git mv final/fcombobox.h final/widget/fcombobox.h
git mv final/fcombobox.cpp final/widget/fcombobox.cpp
git mv final/fspinbox.cpp final/widget/fspinbox.cpp
git mv final/fspinbox.h final/widget/fspinbox.h
git mv final/fbusyindicator.cpp final/widget/fbusyindicator.cpp
git mv final/fbusyindicator.h final/widget/fbusyindicator.h
git mv final/flabel.cpp final/widget/flabel.cpp
git mv final/flabel.h final/widget/flabel.h
git mv final/fbutton.cpp final/widget/fbutton.cpp
git mv final/fbutton.h final/widget/fbutton.h
git mv final/fbuttongroup.cpp final/widget/fbuttongroup.cpp
git mv final/fbuttongroup.h final/widget/fbuttongroup.h
git mv final/fcheckbox.cpp final/widget/fcheckbox.cpp
git mv final/fcheckbox.h final/widget/fcheckbox.h
git mv final/fradiobutton.cpp final/widget/fradiobutton.cpp
git mv final/fradiobutton.h final/widget/fradiobutton.h
git mv final/fswitch.cpp final/widget/fswitch.cpp
git mv final/fswitch.h final/widget/fswitch.h
git mv final/ftogglebutton.h final/widget/ftogglebutton.h
git mv final/ftogglebutton.cpp final/widget/ftogglebutton.cpp
git mv final/flineedit.cpp final/widget/flineedit.cpp
git mv final/flineedit.h final/widget/flineedit.h
git mv final/flistbox.cpp final/widget/flistbox.cpp
git mv final/flistbox.h final/widget/flistbox.h
git mv final/flistview.cpp final/widget/flistview.cpp
git mv final/flistview.h final/widget/flistview.h
git mv final/fprogressbar.cpp final/widget/fprogressbar.cpp
git mv final/fprogressbar.h final/widget/fprogressbar.h
git mv final/fstatusbar.cpp final/widget/fstatusbar.cpp
git mv final/fstatusbar.h final/widget/fstatusbar.h
git mv final/fscrollbar.cpp final/widget/fscrollbar.cpp
git mv final/fscrollbar.h final/widget/fscrollbar.h
git mv final/ftextview.h final/widget/ftextview.h
git mv final/ftextview.cpp final/widget/ftextview.cpp
git mv final/fscrollview.cpp final/widget/fscrollview.cpp
git mv final/fscrollview.h final/widget/fscrollview.h
git mv final/ftooltip.cpp final/widget/ftooltip.cpp
git mv final/ftooltip.h final/widget/ftooltip.h
git mv final/fwindow.cpp final/widget/fwindow.cpp
git mv final/fwindow.h final/widget/fwindow.h
