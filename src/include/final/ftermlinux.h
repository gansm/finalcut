/***********************************************************************
* ftermlinux.h - Contains the Linux terminal functions                 *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2018-2019 Markus Gans                                      *
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
  #include <linux/fb.h>  // Linux framebuffer console

  #if defined(__arm__) && defined(__GLIBC__) && defined(__GLIBC_PREREQ)
    // ISA sysctl support on arm processors only up to glibc-2.29
    #if !__GLIBC_PREREQ(2,30)
      #define ARM_ISA_SYSCTL
    #endif
  #endif

  #if defined(__x86_64__) || defined(__i386) || defined(ARM_ISA_SYSCTL)
    #include <sys/io.h>
  #endif  // defined(__x86_64__) || defined(__i386) || defined(ARM_ISA_SYSCTL)

  #include <sys/kd.h>
#endif  // defined(__linux__)

#include <sys/ioctl.h>
#include <unistd.h>

#include <cstdio>  // need for sprintf
#include <cstring>
#include <vector>

#include "final/ftermdata.h"

namespace finalcut
{

// class forward declaration
class FSystem;
class FTermData;
class FTermDetection;

//----------------------------------------------------------------------
// class FTermLinux
//----------------------------------------------------------------------

class FTermLinux final
{
  public:
    // Typedef
    typedef fc::linuxConsoleCursorStyle  CursorStyle;

    // Constructors
    FTermLinux() = default;

    // Disable copy constructor
    FTermLinux (const FTermLinux&) = delete;

    // Destructor
    virtual ~FTermLinux();

    // Disable assignment operator (=)
    FTermLinux& operator = (const FTermLinux&) = delete;

    // Accessors
    const FString        getClassName() const;
    fc::linuxConsoleCursorStyle getCursorStyle();
    char*                getCursorStyleString();
    int                  getFramebufferBpp();

    // Mutators
    bool                 setCursorStyle (CursorStyle);
    bool                 setPalette (FColor, int, int, int);
    void                 setUTF8 (bool);

    // Inquiries
    bool                 isLinuxConsole();
    bool                 isVGAFontUsed();
    bool                 isNewFontUsed();

    // Methods
    void                 init();
    void                 initCharMap();
    void                 finish();
    bool                 loadVGAFont();
    bool                 loadNewFont();
    bool                 loadOldFont();
    bool                 saveColorMap();
    bool                 resetColorMap();
    void                 setBeep (int, int);
    void                 resetBeep();

    FKey                 modifierKeyCorrection (const FKey&);

  private:
    // Typedef
    struct modifier_key  // bit field
    {
      uChar shift  : 1;  // 0..1
      uChar alt_gr : 1;  // 0..1
      uChar ctrl   : 1;  // 0..1
      uChar alt    : 1;  // 0..1
      uChar        : 4;  // padding bits
    } mod_key{};

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

    // Accessors
    int                  getFramebuffer_bpp();
    bool                 getScreenFont();
    bool                 getUnicodeMap ();
    modifier_key&        getModifierKey();

    // Mutators
    int                  setScreenFont ( uChar[], uInt, uInt, uInt
                                       , bool = false );
    int                  setUnicodeMap (struct unimapdesc*);
    void                 setLinuxCursorStyle (fc::linuxConsoleCursorStyle);

    // Methods
#if defined(__x86_64__) || defined(__i386) || defined(ARM_ISA_SYSCTL)
    uInt16               getInputStatusRegisterOne();
    uChar                readAttributeController (uChar);
    void                 writeAttributeController (uChar, uChar);
    uChar                getAttributeMode();
    void                 setAttributeMode (uChar);
    int                  setBlinkAsIntensity (bool);
    void                 getVGAPalette();
    void                 setVGADefaultPalette();
    bool                 setVGAPalette (FColor, int, int, int);
    bool                 saveVGAPalette();
    bool                 resetVGAPalette();
#endif  // defined(__x86_64__) || defined(__i386) || defined(ARM_ISA_SYSCTL)
    FKey                 shiftKeyCorrection (const FKey&);
    FKey                 ctrlKeyCorrection (const FKey&);
    FKey                 altKeyCorrection (const FKey&);
    FKey                 shiftCtrlKeyCorrection (const FKey&);
    FKey                 shiftAltKeyCorrection (const FKey&);
    FKey                 ctrlAltKeyCorrection (const FKey&);
    FKey                 shiftCtrlAltKeyCorrection (const FKey&);
    sInt16               getFontPos (wchar_t ucs);
    void                 initSpecialCharacter();
    void                 characterFallback (wchar_t, std::vector<wchar_t>);

    // Data members
#if defined(__linux__)
    bool                 vga_font{};
    bool                 new_font{};
    bool                 has_saved_palette{};
    FTermData*           fterm_data{nullptr};
    FSystem*             fsystem{nullptr};
    FTermDetection*      term_detection{nullptr};
    CursorStyle          linux_console_cursor_style{};
    console_font_op      screen_font{};
    unimapdesc           screen_unicode_map{};
    ColorMap             saved_color_map{};
    ColorMap             cmap{};
    int                  framebuffer_bpp{-1};
#endif  // defined(__linux__)
};


// FTermLinux inline functions
//----------------------------------------------------------------------
inline const FString FTermLinux::getClassName() const
{ return "FTermLinux"; }

//----------------------------------------------------------------------
#if defined(__linux__)
inline int FTermLinux::getFramebufferBpp()
{ return framebuffer_bpp; }

//----------------------------------------------------------------------
inline bool FTermLinux::isVGAFontUsed()
{ return vga_font; }

//----------------------------------------------------------------------
inline bool FTermLinux::isNewFontUsed()
{ return new_font; }
#endif  // defined(__linux__)

}  // namespace finalcut

#endif  // FTERMLINUX_H
