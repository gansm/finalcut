/***********************************************************************
* fterm.h - Base class for terminal detection and control              *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2012-2018 Markus Gans                                      *
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

/*  Base class
 *  ══════════
 *
 * ▕▔▔▔▔▔▔▔▏1       1▕▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FTerm ▏-┬- - - -▕ FTermios ▏
 * ▕▁▁▁▁▁▁▁▏ :       ▕▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FTermcap ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FTermcapQuirks ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FTermDetection ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FTermXTerminal ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕   FTermLinux   ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕  FTermFreeBSD  ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕  FTermOpenBSD  ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FColorPalette ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FOptiAttr ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FOptiMove ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FKeyboard ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      *▕▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FString ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      *▕▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FPoint ▏
 *           :       ▕▁▁▁▁▁▁▁▁▏
 *           :
 *           :      *▕▔▔▔▔▔▔▔▏
 *           └- - - -▕ FRect ▏
 *                   ▕▁▁▁▁▁▁▁▏
 */

#ifndef FTERM_H
#define FTERM_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fconfig.h"

#include <sys/ioctl.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <langinfo.h>

#if defined(__sun) && defined(__SVR4)
  #include <termio.h>
  typedef struct termio SGTTY;
  typedef struct termios SGTTYS;

  #ifdef _LP64
    typedef unsigned int chtype;
  #else
    typedef unsigned long chtype;
  #endif

  #include <term.h>  // termcap
#else
  #include <term.h>  // termcap
#endif

#ifdef F_HAVE_LIBGPM
  #undef buttons  // from term.h
#endif

#if F_HAVE_GETTTYNAM && F_HAVE_TTYENT_H
  #include <ttyent.h>
#endif

#include <clocale>
#include <cmath>
#include <csignal>
#include <map>
#include <queue>
#include <string>

#include "final/fc.h"
#include "final/fcolorpalette.h"
#include "final/fkeyboard.h"
#include "final/fmouse.h"
#include "final/fobject.h"
#include "final/foptiattr.h"
#include "final/foptimove.h"
#include "final/fpoint.h"
#include "final/frect.h"
#include "final/fstring.h"
#include "final/ftermcap.h"
#include "final/ftermcapquirks.h"
#include "final/ftermdetection.h"

#if defined(__linux__)
  #include "final/ftermlinux.h"
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__)
  #include "final/ftermfreebsd.h"
#endif

#if defined(__NetBSD__) || defined(__OpenBSD__)
  #include "final/ftermopenbsd.h"
#endif

#include "final/ftermios.h"
#include "final/ftermxterminal.h"


//----------------------------------------------------------------------
// class FTerm
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FTerm
{
  public:
    // Typedefs
    typedef FOptiAttr::charData  charData;

    // Constructor
    explicit FTerm (bool = false);

    // Destructor
    virtual ~FTerm();

    // Accessors
    virtual const char*   getClassName() const;
    static FKeyboard*     getKeyboard();
    static FMouseControl* getMouseControl();
    static int            getLineNumber();
    static int            getColumnNumber();
    static const FString  getKeyName (int);

    static int            getTTYFileDescriptor();
    static char*          getTermType();
    static char*          getTermFileName();
    static int            getTabstop();
    static int            getMaxColor();

#if DEBUG
    static const FString& getAnswerbackString();
    static const FString& getSecDAString();
    static const char*    getTermType_256color();
    static const char*    getTermType_Answerback();
    static const char*    getTermType_SecDA();
    static int            getFramebufferBpp();
#endif

    // Inquiries
    static bool           isCursorHidden();
    static bool           isNormal (charData*&);
    static bool           isRaw();
    static bool           hasPCcharset();
    static bool           hasUTF8();
    static bool           hasVT100();
    static bool           hasASCII();
    static bool           isMonochron();
    static bool           isXTerminal();
    static bool           isAnsiTerminal();
    static bool           isRxvtTerminal();
    static bool           isUrxvtTerminal();
    static bool           isMltermTerminal();
    static bool           isPuttyTerminal();
    static bool           isKdeTerminal();
    static bool           isGnomeTerminal();
    static bool           isKtermTerminal();
    static bool           isTeraTerm();
    static bool           isSunTerminal();
    static bool           isCygwinTerminal();
    static bool           isMinttyTerm();
    static bool           isLinuxTerm();
    static bool           isFreeBSDTerm();
    static bool           isNetBSDTerm();
    static bool           isOpenBSDTerm();
    static bool           isScreenTerm();
    static bool           isTmuxTerm();
    static bool           isNewFont();
    static bool           isUTF8();

    // Mutators
    static void           setTermType (char[]);
    static void           setInsertCursor (bool on);
    static void           setInsertCursor();
    static void           unsetInsertCursor();
    static bool           setCursorOptimisation (bool);
    static void           redefineDefaultColors (bool);
    static void           setDblclickInterval (const long);
    static void           disableAltScreen();
    static bool           setUTF8 (bool);
    static bool           setUTF8();
    static bool           unsetUTF8();

    // Methods
    static bool           setVGAFont();
    static bool           setNewFont();
    static bool           setOldFont();
    static int            openConsole();
    static int            closeConsole();
    static char*          moveCursor (int, int, int, int);
    static char*          cursorsVisibility (bool);
    static void           printMoveDurations();
    static char*          enableCursor();
    static char*          disableCursor();
    static void           detectTermSize();
    static void           setTermSize (int, int);
    static void           setTermTitle(const FString&);
    static void           setKDECursor (fc::kdeKonsoleCursorShape);
    static void           saveColorMap();
    static void           resetColorMap();
    static void           setPalette (short, int, int, int);
    static void           setBeep (int, int);
    static void           resetBeep();
    static void           beep();

    static void           setEncoding (fc::encoding);
    static fc::encoding   getEncoding();
    static std::string    getEncodingString();
    static bool           charEncodable (uInt);
    static uInt           charEncode (uInt);
    static uInt           charEncode (uInt, fc::encoding);

    static bool           scrollTermForward();
    static bool           scrollTermReverse();

    // function pointer -> static function
    static int            (*Fputchar)(int);

    static void           putstringf (const char[], ...)
#if defined(__clang__)
      __attribute__((__format__ (__printf__, 1, 2)))
#elif defined(__GNUC__)
      __attribute__ ((format (printf, 1, 2)))
#endif
                          ;
    static void           putstring (const char[], int = 1);

#if defined(__sun) && defined(__SVR4)
    static int            putchar_ASCII (register char);
#endif

    static int            putchar_ASCII (register int);
    static int            putchar_UTF8  (register int);

#if DEBUG
    static int            framebuffer_bpp;
#endif

  protected:
    // Inquiries
    static bool           hasChangedTermSize();
    static bool           hasShadowCharacter();
    static bool           hasHalfBlockCharacter();
    static bool           hasAlternateScreen();

    // Accessors
    FOptiMove*            getFOptiMove();
    static uInt           getEraseCharsLength();
    static uInt           getRepeatCharLength();
    static uInt           getClrBolLength();
    static uInt           getClrEolLength();
    static uInt           getCursorAddressLength();

    // Methods
    static void           initScreenSettings();
    static char*          changeAttribute ( charData*&
                                          , charData*& );
    static void           changeTermSizeFinished();
    static void           exitWithMessage (std::string)
    #if defined(__clang__) || defined(__GNUC__)
      __attribute__((noreturn))
    #endif
                          ;
    // Data Members
    static struct initializationValues
    {
      public:
        void setDefault()
        {
          cursor_optimisation = true;
          mouse_support = true;
          terminal_detection = true;
          color_change = true;
          vgafont = false;
          newfont = false;
          encoding = fc::UNKNOWN;

        #if defined(__FreeBSD__) || defined(__DragonFly__)
          meta_sends_escape = true;
          change_cursorstyle = true;
        #endif

        #if defined(__NetBSD__) || defined(__OpenBSD__)
          meta_sends_escape = true;
        #endif
        }

        uInt8 cursor_optimisation : 1;
        uInt8 mouse_support       : 1;
        uInt8 terminal_detection  : 1;
        uInt8 color_change        : 1;
        uInt8 vgafont             : 1;
        uInt8 newfont             : 1;
        uInt8                     : 2;  // padding bits
        fc::encoding encoding;

      #if defined(__FreeBSD__) || defined(__DragonFly__)
        uInt8 meta_sends_escape  : 1;
        uInt8 change_cursorstyle : 1;
        uInt8                    : 6;  // padding bits
      #endif

      #if defined(__NetBSD__) || defined(__OpenBSD__)
        uInt8 meta_sends_escape  : 1;
        uInt8                    : 7;  // padding bits
      #endif
    } init_values;

  private:
    // Typedefs
    typedef FTermcap::tcap_map termcap_map;

    // Disable copy constructor
    FTerm (const FTerm&);
    // Disable assignment operator (=)
    FTerm& operator = (const FTerm&);

    // Methods
    static void           init_global_values();
    static void           oscPrefix();
    static void           oscPostfix();
    static void           init_alt_charset();
    static void           init_pc_charset();
    static void           init_cygwin_charmap();
    static void           init_teraterm_charmap();
    static void           init_fixed_max_color();
    static void           init_keyboard();
    static void           init_termcap();
    static void           init_termcap_error (int);
    static void           init_termcap_variables(char*&);
    static void           init_termcap_booleans();
    static void           init_termcap_numerics();
    static void           init_termcap_strings (char*&);
    static void           init_termcap_keys (char*&);
    static void           init_OptiMove();
    static void           init_OptiAttr();
    static void           init_font();
    static void           init_locale();
    static void           init_encoding();
    static void           init_encoding_set();
    static void           init_term_encoding();
    static void           init_individual_term_encoding();
    static bool           init_force_vt100_encoding();
    static void           init_utf8_without_alt_charset();
    static void           init_tab_quirks();
    static void           init_captureFontAndTitle();
    static void           redefineColorPalette();
    static void           restoreColorPalette();
    static void           setInsertCursorStyle();
    static void           setOverwriteCursorStyle();
    static void           enableMouse();
    static void           disableMouse();
    static void           useAlternateScreenBuffer();
    static void           useNormalScreenBuffer();
    void                  allocationValues();
    void                  deallocationValues();
    void                  init();
    void                  initOSspecifics();
    void                  finish();
    void                  finishOSspecifics1();
    void                  finish_encoding();
    static uInt           cp437_to_unicode (uChar);
    static void           setSignalHandler();
    static void           resetSignalHandler();
    static void           signal_handler (int);

    // Data Members
    static std::map <uChar,uChar>* vt100_alt_char;
    static std::map <std::string,fc::encoding>* encoding_set;
    static FTermcap::tcap_map* tcap;
    static fc::encoding   term_encoding;

    static bool           shadow_character;
    static bool           half_block_character;
    static bool           cursor_optimisation;
    static bool           hidden_cursor;
    static bool           use_alternate_screen;
    static bool           pc_charset_console;
    static bool           utf8_state;
    static bool           utf8_console;
    static bool           utf8_linux_terminal;
    static bool           force_vt100;
    static bool           vt100_console;
    static bool           ascii_console;
    static bool           NewFont;
    static bool           VGAFont;
    static bool           monochron;
    static char           termtype[256];
    static char           termfilename[256];
    static char           exit_message[8192];
    static char*          locale_name;
    static char*          locale_xterm;
    static FRect*         term;  // current terminal geometry

    static int            fd_tty;
    static uInt           baudrate;
    static bool           resize_term;

    static fc::linuxConsoleCursorStyle \
                          linux_console_cursor_style;
    static struct console_font_op \
                          screen_font;
    static struct unimapdesc \
                          screen_unicode_map;

    static FOptiMove*      opti_move;
    static FOptiAttr*      opti_attr;
    static FTermDetection* term_detection;
    static FTermXTerminal* xterm;
    static FKeyboard*      keyboard;

#if defined(__linux__)
    #undef linux
    static FTermLinux*     linux;
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__)
    static FTermFreeBSD*   freebsd;
#endif

#if defined(__NetBSD__) || defined(__OpenBSD__)
    static FTermOpenBSD*   openbsd;
#endif

    static FMouseControl*  mouse;
    static const FString*  save_xterm_font;
    static const FString*  save_xterm_title;
};

#pragma pack(pop)

// FTerm inline functions
//----------------------------------------------------------------------
inline const char* FTerm::getClassName() const
{ return "FTerm"; }

//----------------------------------------------------------------------
inline FKeyboard* FTerm::getKeyboard()
{ return ( keyboard ) ? keyboard : 0; }

//----------------------------------------------------------------------
inline FMouseControl* FTerm::getMouseControl()
{ return ( mouse ) ? mouse : 0; }

//----------------------------------------------------------------------
inline int FTerm::getTTYFileDescriptor()
{ return fd_tty; }

//----------------------------------------------------------------------
inline char* FTerm::getTermType()
{ return termtype; }

//----------------------------------------------------------------------
inline char* FTerm::getTermFileName()
{ return termfilename; }

//----------------------------------------------------------------------
inline int FTerm::getTabstop()
{ return FTermcap::tabstop; }

//----------------------------------------------------------------------
inline int FTerm::getMaxColor()
{ return FTermcap::max_color; }

#if DEBUG
//----------------------------------------------------------------------
inline const FString& FTerm::getAnswerbackString()
{ return term_detection->getAnswerbackString(); }

//----------------------------------------------------------------------
inline const FString& FTerm::getSecDAString()
{ return term_detection->getSecDAString(); }

//----------------------------------------------------------------------
inline const char* FTerm::getTermType_256color()
{ return term_detection->getTermType_256color(); }

//----------------------------------------------------------------------
inline const char* FTerm::getTermType_Answerback()
{ return term_detection->getTermType_Answerback(); }

//----------------------------------------------------------------------
inline const char* FTerm::getTermType_SecDA()
{ return term_detection->getTermType_SecDA(); }

//----------------------------------------------------------------------
inline int FTerm::getFramebufferBpp()
{ return framebuffer_bpp; }
#endif

//----------------------------------------------------------------------
inline bool FTerm::isCursorHidden()
{ return hidden_cursor; }

//----------------------------------------------------------------------
inline bool FTerm::hasPCcharset()
{ return pc_charset_console; }

//----------------------------------------------------------------------
inline bool FTerm::hasUTF8()
{ return utf8_console; }

//----------------------------------------------------------------------
inline bool FTerm::hasVT100()
{ return vt100_console; }

//----------------------------------------------------------------------
inline bool FTerm::hasASCII()
{ return ascii_console; }

//----------------------------------------------------------------------
inline bool FTerm::isMonochron()
{ return monochron; }

//----------------------------------------------------------------------
inline bool FTerm::isXTerminal()
{ return term_detection->isXTerminal(); }

//----------------------------------------------------------------------
inline bool FTerm::isAnsiTerminal()
{ return term_detection->isAnsiTerminal(); }

//----------------------------------------------------------------------
inline bool FTerm::isRxvtTerminal()
{ return term_detection->isRxvtTerminal(); }

//----------------------------------------------------------------------
inline bool FTerm::isUrxvtTerminal()
{ return term_detection->isUrxvtTerminal(); }

//----------------------------------------------------------------------
inline bool FTerm::isMltermTerminal()
{ return term_detection->isMltermTerminal(); }

//----------------------------------------------------------------------
inline bool FTerm::isPuttyTerminal()
{ return term_detection->isPuttyTerminal(); }

//----------------------------------------------------------------------
inline bool FTerm::isKdeTerminal()
{ return term_detection->isKdeTerminal(); }

//----------------------------------------------------------------------
inline bool FTerm::isGnomeTerminal()
{ return term_detection->isGnomeTerminal(); }

//----------------------------------------------------------------------
inline bool FTerm::isKtermTerminal()
{ return term_detection->isKtermTerminal(); }

//----------------------------------------------------------------------
inline bool FTerm::isTeraTerm()
{ return term_detection->isTeraTerm(); }

//----------------------------------------------------------------------
inline bool FTerm::isSunTerminal()
{ return term_detection->isSunTerminal(); }

//----------------------------------------------------------------------
inline bool FTerm::isCygwinTerminal()
{ return term_detection->isCygwinTerminal(); }

//----------------------------------------------------------------------
inline bool FTerm::isMinttyTerm()
{ return term_detection->isMinttyTerm(); }

//----------------------------------------------------------------------
inline bool FTerm::isLinuxTerm()
{ return term_detection->isLinuxTerm(); }

//----------------------------------------------------------------------
inline bool FTerm::isFreeBSDTerm()
{ return term_detection->isFreeBSDTerm(); }

//----------------------------------------------------------------------
inline bool FTerm::isNetBSDTerm()
{ return term_detection->isNetBSDTerm(); }

//----------------------------------------------------------------------
inline bool FTerm::isOpenBSDTerm()
{ return term_detection->isOpenBSDTerm(); }

//----------------------------------------------------------------------
inline bool FTerm::isScreenTerm()
{ return term_detection->isScreenTerm(); }

//----------------------------------------------------------------------
inline bool FTerm::isTmuxTerm()
{ return term_detection->isTmuxTerm(); }

//----------------------------------------------------------------------
inline bool FTerm::isNewFont()
{ return NewFont; }

//----------------------------------------------------------------------
inline bool FTerm::isUTF8()
{ return utf8_state; }

//----------------------------------------------------------------------
inline void FTerm::setInsertCursor()
{ setInsertCursor(true); }

//----------------------------------------------------------------------
inline void FTerm::unsetInsertCursor()
{ setInsertCursor(false); }

//----------------------------------------------------------------------
inline bool FTerm::setCursorOptimisation (bool on)
{ return cursor_optimisation = ( on ) ? true : false; }

//----------------------------------------------------------------------
inline void FTerm::disableAltScreen()
{ use_alternate_screen = false; }

//----------------------------------------------------------------------
inline bool FTerm::setUTF8()
{ return setUTF8(true); }

//----------------------------------------------------------------------
inline bool FTerm::unsetUTF8()
{ return setUTF8(false); }

//----------------------------------------------------------------------
inline bool FTerm::hasChangedTermSize()
{ return resize_term; }

//----------------------------------------------------------------------
inline bool FTerm::hasShadowCharacter()
{ return shadow_character; }

//----------------------------------------------------------------------
inline bool FTerm::hasHalfBlockCharacter()
{ return half_block_character; }

//----------------------------------------------------------------------
inline bool FTerm::hasAlternateScreen()
{ return use_alternate_screen; }

//----------------------------------------------------------------------
inline FOptiMove* FTerm::getFOptiMove()
{ return opti_move; }

//----------------------------------------------------------------------
inline void FTerm::changeTermSizeFinished()
{ resize_term = false; }


#endif  // FTERM_H
