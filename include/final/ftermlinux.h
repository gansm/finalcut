/***********************************************************************
* ftermlinux.h - Contains the Linux terminal functions                 *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2018 Markus Gans                                           *
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

/*  Standalone class
 *  ════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FTermLinux ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FTERMLINUX_H
#define FTERMLINUX_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#if defined(__linux__)
  #include <linux/fb.h>        // Linux framebuffer console

  #if defined(__x86_64__) || defined(__i386) || defined(__arm__)
    #include <sys/io.h>        // <asm/io.h> is deprecated
  #endif

  #include <sys/kd.h>
#endif

#include <sys/ioctl.h>
#include <unistd.h>

#include <cstdio>  // need for sprintf
#include <cstring>

#include "final/fc.h"
#include "final/fcharmap.h"
#include "final/ftermdetection.h"
#include "final/ftypes.h"


//----------------------------------------------------------------------
// class FTermLinux
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FTermLinux
{
  public:
    // Constructors
    FTermLinux();

    // Destructor
    ~FTermLinux();

    // Accessors
    static fc::linuxConsoleCursorStyle getCursorStyle();
    static int           getFramebufferBpp();

    // Mutators
    static void          setFTermDetection (FTermDetection*);
    static char*         setCursorStyle (fc::linuxConsoleCursorStyle, bool);
    static bool          setPalette (short, int, int, int);
    static void          setUTF8 (bool);

    // Inquiries
    static bool          isLinuxConsole();
    static bool          hasShadowCharacter();
    static bool          hasHalfBlockCharacter();
    static bool          isVGAFontUsed();
    static bool          isNewFontUsed();

    // Methods
    static void          init();
    static void          initCharMap (uInt[][fc::NUM_OF_ENCODINGS]);
    static void          finish();
    static bool          loadVGAFont();
    static bool          loadNewFont();
    static bool          loadOldFont (uInt[][fc::NUM_OF_ENCODINGS]);
    static bool          saveColorMap();
    static bool          resetColorMap();
    static void          setBeep (int, int);
    static void          resetBeep();
    static char*         restoreCursorStyle();
    static int           modifierKeyCorrection (const int&);

  private:
    // Typedef
    static struct modifier_key  // bit field
    {
      uChar shift  : 1;  // 0..1
      uChar alt_gr : 1;  // 0..1
      uChar ctrl   : 1;  // 0..1
      uChar alt    : 1;  // 0..1
      uChar        : 4;  // padding bits
    } mod_key;

    typedef struct
    {
      uChar red;
      uChar green;
      uChar blue;
    } rgb;

    typedef struct
    {
      rgb color[16];
    } ColorMap;

    // Disable copy constructor
    FTermLinux (const FTermLinux&);

    // Disable assignment operator (=)
    FTermLinux& operator = (const FTermLinux&);

    // Accessors
    static int           getFramebuffer_bpp();
    static bool          getScreenFont();
    static bool          getUnicodeMap ();
    static modifier_key& getModifierKey();

    // Mutators
    static int           setScreenFont ( uChar[], uInt, uInt, uInt
                                       , bool = false );
    static int           setUnicodeMap (struct unimapdesc*);

    // Methods
#if defined(__x86_64__) || defined(__i386) || defined(__arm__)
    static uInt16        getInputStatusRegisterOne();
    static uChar         readAttributeController (uChar);
    static void          writeAttributeController (uChar, uChar);
    static uChar         getAttributeMode();
    static void          setAttributeMode (uChar);
    static int           setBlinkAsIntensity (bool);
    static bool          setVGAPalette (short, int, int, int);
    static bool          saveVGAPalette();
    static bool          resetVGAPalette();
#endif
    static int           shiftKeyCorrection (const int&);
    static int           ctrlKeyCorrection (const int&);
    static int           altKeyCorrection (const int&);
    static int           shiftCtrlKeyCorrection (const int&);
    static int           shiftAltKeyCorrection (const int&);
    static int           ctrlAltKeyCorrection (const int&);
    static int           shiftCtrlAltKeyCorrection (const int&);

    // Data Members
#if defined(__linux__)
    static bool                   VGAFont;
    static bool                   NewFont;
    static bool                   shadow_character;
    static bool                   half_block_character;
    static bool                   has_saved_palette;
    static FTermDetection*        term_detection;
    static fc::linuxConsoleCursorStyle
                                  linux_console_cursor_style;
    static console_font_op        screen_font;
    static unimapdesc             screen_unicode_map;
    static ColorMap               saved_color_map;
    static ColorMap               cmap;
#if DEBUG
    static int                    framebuffer_bpp;
#endif
#endif  // defined(__linux__)
};
#pragma pack(pop)

// FTermLinux inline functions
//----------------------------------------------------------------------
#if defined(__linux__)
inline int FTermLinux::getFramebufferBpp()
{ return framebuffer_bpp; }

//----------------------------------------------------------------------
inline void FTermLinux::setFTermDetection (FTermDetection* td)
{ term_detection = td; }

//----------------------------------------------------------------------
inline bool FTermLinux::hasShadowCharacter()
{ return shadow_character; }

//----------------------------------------------------------------------
inline bool FTermLinux::hasHalfBlockCharacter()
{ return half_block_character; }

//----------------------------------------------------------------------
inline bool FTermLinux::isVGAFontUsed()
{ return VGAFont; }

//----------------------------------------------------------------------
inline bool FTermLinux::isNewFontUsed()
{ return NewFont; }
#endif  // defined(__linux__)

#endif  // FTERMLINUX_H
