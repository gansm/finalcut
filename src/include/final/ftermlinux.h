/***********************************************************************
* ftermlinux.h - Contains the Linux terminal functions                 *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2020 Markus Gans                                      *
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
    #define ISA_SYSCTL_SUPPORT
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
    ~FTermLinux();

    // Disable copy assignment operator (=)
    FTermLinux& operator = (const FTermLinux&) = delete;

    // Accessors
    const FString        getClassName() const;
    fc::linuxConsoleCursorStyle getCursorStyle() const;
    char*                getCursorStyleString();
    int                  getFramebufferBpp() const;

    // Mutators
    bool                 setCursorStyle (CursorStyle);
    bool                 setPalette (FColor, int, int, int);
    void                 setUTF8 (bool) const;

    // Inquiries
    bool                 isLinuxConsole();
    bool                 isVGAFontUsed() const;
    bool                 isNewFontUsed() const;

    // Methods
    void                 init();
    void                 initCharMap();
    void                 finish();
    bool                 loadVGAFont();
    bool                 loadNewFont();
    bool                 loadOldFont();
    bool                 saveColorMap();
    bool                 resetColorMap();
    void                 setBeep (int, int) const;
    void                 resetBeep() const;

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
    };

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
    int                  setScreenFont ( const uChar[], uInt, uInt, uInt
                                       , bool = false );
    int                  setUnicodeMap (struct unimapdesc*);
    void                 setLinuxCursorStyle (fc::linuxConsoleCursorStyle) const;

    // Methods
#if defined(ISA_SYSCTL_SUPPORT)
    uInt16               getInputStatusRegisterOne();
    uChar                readAttributeController (uChar);
    void                 writeAttributeController (uChar, uChar);
    uChar                getAttributeMode();
    void                 setAttributeMode (uChar);
    int                  setBlinkAsIntensity (bool);
    bool                 has9BitCharacters();
    void                 getVGAPalette();
    void                 setVGADefaultPalette();
    bool                 setVGAPalette (FColor, int, int, int);
    bool                 saveVGAPalette();
    bool                 resetVGAPalette();
#endif  // defined(ISA_SYSCTL_SUPPORT)
    FKey                 shiftKeyCorrection (const FKey&) const;
    FKey                 ctrlKeyCorrection (const FKey&) const;
    FKey                 altKeyCorrection (const FKey&) const;
    FKey                 shiftCtrlKeyCorrection (const FKey&) const;
    FKey                 shiftAltKeyCorrection (const FKey&) const;
    FKey                 ctrlAltKeyCorrection (const FKey&) const;
    FKey                 shiftCtrlAltKeyCorrection (const FKey&) const;
    sInt16               getFontPos (wchar_t ucs) const;
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
    modifier_key         mod_key{};
#endif  // defined(__linux__)
};


// FTermLinux inline functions
//----------------------------------------------------------------------
inline const FString FTermLinux::getClassName() const
{ return "FTermLinux"; }

//----------------------------------------------------------------------
#if defined(__linux__)
inline int FTermLinux::getFramebufferBpp() const
{ return framebuffer_bpp; }

//----------------------------------------------------------------------
inline bool FTermLinux::isVGAFontUsed() const
{ return vga_font; }

//----------------------------------------------------------------------
inline bool FTermLinux::isNewFontUsed() const
{ return new_font; }
#endif  // defined(__linux__)

}  // namespace finalcut

#endif  // FTERMLINUX_H
