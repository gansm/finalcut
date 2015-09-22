// fterm.h
// class FTerm

#ifndef _FTERM_H
#define _FTERM_H

#include <fconfig.h>

#ifdef F_HAVE_LIBGPM
  #include <gpm.h>
#endif

#include <linux/fb.h> // Linux framebuffer console

#include <sys/io.h>
#include <sys/ioctl.h>
#include <sys/kd.h>
#include <sys/stat.h> // <asm/io.h> is deprecated, use <sys/io.h> instead

#include <fcntl.h>
#include <langinfo.h>
#include <term.h> // termcap
#include <termios.h>
#include <unistd.h>

#include <cmath>
#include <csignal>
#include <map>
#include <queue>

#include "fenum.h"
#include "fobject.h"
#include "foptimove.h"
#include "fpoint.h"
#include "frect.h"
#include "fstring.h"


#ifdef F_HAVE_LIBGPM
  #undef buttons  // from term.h
#endif

// VGA I/O-ports
#define VideoIOBase ( (inb(0x3CC) & 0x01) ? 0x3D0 : 0x3B0 )
#define AttrC_Index  0x3C0  // Attribute controller index
#define AttrC_DataW  0x3C0  // Attribute controller dataW
#define AttrC_DataR  0x3C1  // Attribute controller dataR
#define AttrC_DataSwitch (VideoIOBase+0x0A) // Attribute controller data switch

// parseKeyString return value
#define NEED_MORE_DATA  -1

class FWidget;

//----------------------------------------------------------------------
// class FTerm
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FTerm
{
 private:
   static std::queue<int>* output_buffer;
   static std::map <uChar,uChar>* vt100_alt_char;
   static std::map <std::string,fc::encoding>* encoding_set;

   static bool    term_bold;
   static bool    term_reverse;
   static bool    term_underline;
   static bool    hiddenCursor;
   static bool    mouse_support;
   static bool    vt100_state;
   static bool    ignore_vt100_state;
   static bool    raw_mode;
   static bool    non_blocking_stdin;
   static bool    gpm_mouse_enabled;
   static bool    pc_charset_state;
   static bool    pc_charset_console;
   static bool    utf8_input;
   static bool    utf8_state;
   static bool    utf8_console;
   static bool    utf8_linux_terminal;
   static bool    force_vt100;
   static bool    vt100_console;
   static bool    ascii_console;
   static bool    color256;
   static bool    monochron;
   static bool    xterm;
   static bool    rxvt_terminal;
   static bool    urxvt_terminal;
   static bool    mlterm_terminal;
   static bool    putty_terminal;
   static bool    kde_konsole;
   static bool    gnome_terminal;
   static bool    kterm_terminal;
   static bool    tera_terminal;
   static bool    cygwin_terminal;
   static bool    mintty_terminal;
   static bool    linux_terminal;
   static bool    screen_terminal;
   static bool    tmux_terminal;
   static bool    terminal_updates;
   static bool    vterm_updates;
   static bool    background_color_erase;
   static bool    automatic_left_margin;
   static bool    automatic_right_margin;
   static bool    eat_nl_glitch;
   static char*   termtype;
   static char*   term_name;
   static char*   locale_name;
   static char*   locale_xterm;
   static uChar   x11_button_state;
   static FRect*  term;
   static FPoint* mouse;
   static FPoint* cursor;
   static int     stdin_status_flags;
   static int     stdin_no;
   static int     stdout_no;
   static int     max_color;
   static int     fd_tty;
   static uInt    baudrate;
   static int     fg_color;
   static int     bg_color;
   static int     fg_term_color;
   static int     bg_term_color;
   static int     x_term_pos;
   static int     y_term_pos;
   static bool    resize_term;

   static struct termios term_init;

   static fc::console_cursor_style consoleCursorStyle;
   static struct console_font_op screenFont;
   static struct unimapdesc      screenUnicodeMap;
   static FOptiMove* opti;
   static const FString* xterm_font;
   static const FString* xterm_title;
   static const FString* AnswerBack;
   static const FString* Sec_DA;

   typedef struct
   {
     uChar red;
     uChar green;
     uChar blue;
   } dacreg;

   struct
   {
     dacreg d[16];
   } map;

 protected:
   static bool bold;
   static bool reverse;
   static bool underline;
   static bool NewFont;
   static bool VGAFont;
   static uInt tabstop;
   static fc::encoding Encoding;

   typedef struct
   {
     uInt xmin;
     uInt xmax;
   } line_changes;

   typedef struct
   {
     int   code;           // character code
     uChar fg_color;       // foreground color
     uChar bg_color;       // background color
     uChar bold      : 1;  // bold
     uChar reverse   : 1;  // reverse
     uChar underline : 1;  // underline
     uChar           : 5;  // padding bits
   } char_data;

   typedef struct
   {
     int width;
     int height;
     int right_shadow;
     int bottom_shadow;
     FWidget* widget;
     line_changes* changes;
     char_data* text;
     bool visible;
   } term_area;

   static term_area* vterm;      // virtual terminal
   static term_area* vdesktop;   // virtual desktop
   static term_area* vmenubar;   // virtual menubar
   static term_area* vstatusbar; // virtual statusbar
   static term_area* last_area;  // last used area
   term_area* vwin;              // virtual window

 private:
   FTerm (const FTerm&);
   FTerm& operator = (const FTerm&);

   static void  outb_Attribute_Controller (int, int);
   static int   inb_Attribute_Controller (int);
   static int   getFramebuffer_bpp();
   static int   openConsole();
   static int   closeConsole();
   static int   isConsole();
   static int   getScreenFont();
   static int   setScreenFont (uChar*, uInt, uInt, uInt, bool direct=false);
   static int   setUnicodeMap (struct unimapdesc*);
   static int   getUnicodeMap ();
   static int   setLightBackgroundColors (bool);
   static uInt  getBaudRate (const struct termios*);
   static void  init_termcaps();
   static void  init_vt100altChar();
   static void  init_consoleCharMap();
   void         init();
   void         finish();
   static uInt  charEncode (uInt);
   static uInt  cp437_to_unicode (uChar);
   static void  signal_handler (int);
   friend class FWidget;
   friend class FApplication;

 protected:
   void         createArea (FTerm::term_area*&); // reference to pointer
   void         resizeArea (FTerm::term_area*);
   void         restoreVTerm (const FRect&);
   void         restoreVTerm (int, int, int, int);
   bool         isCovered (const FPoint&, FTerm::term_area*) const;
   bool         isCovered (int, int, FTerm::term_area*) const;
   void         updateVTerm (FTerm::term_area*);
   void         setUpdateVTerm (bool);
   void         getArea (const FPoint&, FTerm::term_area*);
   void         getArea (int, int, FTerm::term_area*);
   void         getArea (const FRect&, FTerm::term_area*);
   void         getArea (int, int, int, int, FTerm::term_area*);
   void         putArea (const FPoint&, FTerm::term_area*);
   void         putArea (int, int, FTerm::term_area*);
   char_data    getCoveredCharacter (const FPoint&, FTerm*);
   char_data    getCoveredCharacter (int, int, FTerm*);

 public:
   FTerm ();  // constructor
   virtual ~FTerm();  // destructor

   virtual const char* getClassName() const;
   FTerm::term_area* getVWin() const;
   static bool    isKeyTimeout (timeval*, register long);
   static int     parseKeyString (char*, int, timeval*);
   int            getLineNumber();
   int            getColumnNumber();
   static FString getKeyName(int);

   static char*   getTermType();
   static char*   getTermName();
   static bool    hasPCcharset();
   static bool    hasUTF8();
   static bool    hasVT100();
   static bool    hasASCII();
   static bool    isMonochron();
   static bool    isCygwinTerminal();
   static bool    isUrxvtTerminal();
   static bool    setVGAFont();
   static bool    setNewFont();
   static bool    isNewFont();
   static bool    setOldFont();
   static void    setConsoleCursor(fc::console_cursor_style);
   static void    getTermSize();
   static void    setTermSize (int, int);
   void           createVTerm();
   static void    resizeVTerm();
   static void    putVTerm();
   static void    updateTerminal();
   static void    setKDECursor(fc::kde_konsole_CursorShape);
   static FString getXTermFont();
   static FString getXTermTitle();
   static void    setXTermCursorStyle (fc::xterm_cursor_style);
   static void    setXTermTitle (const FString&);
   static void    setXTermForeground (const FString&);
   static void    setXTermBackground (const FString&);
   static void    setXTermCursor (const FString&);
   static void    setXTermMouseForeground (const FString&);
   static void    setXTermMouseBackground (const FString&);
   static void    setXTermHighlightBackground (const FString&);
   static void    resetXTermForeground();
   static void    resetXTermBackground();
   static void    resetXTermCursor();
   static void    resetXTermMouseForeground();
   static void    resetXTermMouseBackground();
   static void    saveColorMap();
   static void    resetColorMap();
   static void    setPalette (int, int, int, int);
   static int     vga2ansi (int);
   void           setColor (int, int);
   static void    setTermColor (int, int);
   static int     getMaxColor();
   static void    xtermMouse (bool);
   static void    enableXTermMouse();
   static void    disableXTermMouse();

#ifdef F_HAVE_LIBGPM
   static bool    gpmMouse (bool);
   static bool    enableGpmMouse();
   static bool    disableGpmMouse();
#endif  // F_HAVE_LIBGPM

   static void    setTermXY (register int, register int);
   static void    setBeep(int, int);
   static void    resetBeep();
   static void    beep();

   static bool    setTermBold(bool);
   static bool    setTermReverse(bool);
   static bool    setTermUnderline(bool);

   static bool    hideCursor(bool);
   static bool    hideCursor();
   static bool    showCursor();
   static bool    isHiddenCursor();
   bool           isCursorInside();

   static void         setEncoding(std::string);
   static std::string  getEncoding();

   static bool    setPCcharset(bool);
   static bool    setPCcharset();
   static bool    unsetPCcharset();
   static bool    isPCcharset();

   static bool    setNonBlockingInput(bool);
   static bool    setNonBlockingInput();
   static bool    unsetNonBlockingInput();

   static bool    setVT100altChar(bool);
   static bool    setVT100altChar();
   static bool    unsetVT100altChar();
   static bool    isVT100altChar();

   static bool    setUTF8(bool);
   static bool    setUTF8();
   static bool    unsetUTF8();
   static bool    isUTF8();
   static bool    isUTF8_linux_terminal();

   static bool    setRawMode(bool);
   static bool    setRawMode();
   static bool    unsetRawMode();
   static bool    setCookedMode();
   static bool    isRaw();
   static FString getAnswerbackMsg();
   static FString getSecDA();

   int            printf (const wchar_t*, ...);
   int            printf (const char*, ...)
   #if defined(__clang__)
     __attribute__((__format__ (__printf__, 2, 3)))
   #elif defined(__GNUC__)
     __attribute__ ((format (printf, 2, 3)))
   #endif
                  ;
   int            print (const std::wstring&);
   int            print (FTerm::term_area*, const std::wstring&);
   int            print (const wchar_t*);
   int            print (FTerm::term_area*, const wchar_t*);
   int            print (const char*);
   int            print (FTerm::term_area*, const char*);
   int            print (const std::string&);
   int            print (FTerm::term_area*, const std::string&);
   int            print (FString&);
   int            print (FTerm::term_area*, FString&);
   int            print (int);
   int            print (FTerm::term_area*, int);
   static void    appendCharacter (char_data*&);
   static void    appendAttributes (char_data*&);
   static int     appendLowerRight (char_data*&);
   static int     appendOutputBuffer (int);
   static void    flush_out();

   static int     (*Fputchar)(int); // function pointer -> static function
   static int     putchar_ASCII (register int);
   static int     putchar_VT100 (register int);
   static int     putchar_PC    (register int);
   static int     putchar_UTF8  (register int);
   static int     UTF8decode (char*);
};

#pragma pack(pop)
//----------------------------------------------------------------------
inline const char* FTerm::getClassName() const
{ return "FTerm"; }

//----------------------------------------------------------------------
inline int FTerm::getLineNumber()
{
  if ( term->getHeight() == 0 )
    getTermSize();
  return term->getHeight();
}

//----------------------------------------------------------------------
inline int FTerm::getColumnNumber()
{
  if ( term->getWidth() == 0 )
    getTermSize();
  return term->getWidth();
}

//----------------------------------------------------------------------
inline char* FTerm::getTermType()
{ return termtype; }

//----------------------------------------------------------------------
inline char* FTerm::getTermName()
{ return term_name; }

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
inline bool FTerm::isNewFont()
{ return NewFont; }

//----------------------------------------------------------------------
inline bool FTerm::isHiddenCursor()
{ return hiddenCursor; }

//----------------------------------------------------------------------
inline bool FTerm::isMonochron()
{ return monochron; }

//----------------------------------------------------------------------
inline bool FTerm::isCygwinTerminal()
{ return cygwin_terminal; }

//----------------------------------------------------------------------
inline bool FTerm::isUrxvtTerminal()
{ return urxvt_terminal; }

//----------------------------------------------------------------------
inline bool FTerm::isRaw()
{ return raw_mode; }

//----------------------------------------------------------------------
inline int FTerm::getMaxColor()
{ return max_color; }

//----------------------------------------------------------------------
inline void FTerm::enableXTermMouse()
{ xtermMouse(true); }

//----------------------------------------------------------------------
inline void FTerm::disableXTermMouse()
{ xtermMouse(false); }


#ifdef F_HAVE_LIBGPM
//----------------------------------------------------------------------
inline bool FTerm::enableGpmMouse()
{ return gpmMouse(true); }

//----------------------------------------------------------------------
inline bool FTerm::disableGpmMouse()
{ return gpmMouse(false); }

#endif  // F_HAVE_LIBGPM

//----------------------------------------------------------------------
inline bool FTerm::hideCursor()
{ return hideCursor(true); }

//----------------------------------------------------------------------
inline bool FTerm::showCursor()
{ return hideCursor(false); }

//----------------------------------------------------------------------
inline bool FTerm::setPCcharset()
{ return setPCcharset(true); }

//----------------------------------------------------------------------
inline bool FTerm::unsetPCcharset()
{ return setPCcharset(false); }

//----------------------------------------------------------------------
inline bool FTerm::isPCcharset()
{ return pc_charset_state; }

//----------------------------------------------------------------------
inline bool FTerm::setNonBlockingInput()
{ return setNonBlockingInput(true); }

//----------------------------------------------------------------------
inline bool FTerm::unsetNonBlockingInput()
{ return setNonBlockingInput(false); }

//----------------------------------------------------------------------
inline bool FTerm::setVT100altChar()
{ return setVT100altChar(true); }

//----------------------------------------------------------------------
inline bool FTerm::unsetVT100altChar()
{ return setVT100altChar(false); }

//----------------------------------------------------------------------
inline bool FTerm::isVT100altChar()
{ return vt100_state; }

//----------------------------------------------------------------------
inline bool FTerm::setUTF8()
{ return setUTF8(true); }

//----------------------------------------------------------------------
inline bool FTerm::unsetUTF8()
{ return setUTF8(false); }

//----------------------------------------------------------------------
inline bool FTerm::isUTF8()
{ return utf8_state; }

//----------------------------------------------------------------------
inline bool FTerm::isUTF8_linux_terminal()
{ return utf8_linux_terminal; }

//----------------------------------------------------------------------
inline bool FTerm::setRawMode()
{ return setRawMode(true); }

//----------------------------------------------------------------------
inline bool FTerm::unsetRawMode()
{ return setRawMode(false); }

//----------------------------------------------------------------------
inline bool FTerm::setCookedMode()
{ return setRawMode(false); }


#endif  // _FTERM_H
