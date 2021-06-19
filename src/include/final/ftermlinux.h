/***********************************************************************
* ftermlinux.h - Contains the Linux terminal functions                 *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2021 Markus Gans                                      *
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

#include <bitset>
#include <cstdio>  // need for sprintf
#include <cstring>
#include <unordered_map>
#include <utility>
#include <vector>

#include "final/ftermdata.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FTermLinux
//----------------------------------------------------------------------

class FTermLinux final
{
  public:
    // Using-declaration
    using CursorStyle = LinuxConsoleCursorStyle;

    // Constructors
    FTermLinux() = default;

    // Disable copy constructor
    FTermLinux (const FTermLinux&) = delete;

    // Destructor
    ~FTermLinux();

    // Disable copy assignment operator (=)
    FTermLinux& operator = (const FTermLinux&) = delete;

    // Accessors
    FString              getClassName() const;
    static auto          getInstance() -> FTermLinux&;
    CursorStyle          getCursorStyle() const;
    char*                getCursorStyleString();
    int                  getFramebufferBpp() const;

    // Mutators
    bool                 setCursorStyle (CursorStyle);
    bool                 setPalette (FColor, int, int, int);
    void                 setUTF8 (bool = true) const;

    // Inquiries
    static bool          isLinuxConsole();
    bool                 isVGAFontUsed() const;
    bool                 isNewFontUsed() const;

    // Methods
    void                 init();
    void                 initCharMap() const;
    void                 finish() const;
    bool                 loadVGAFont();
    bool                 loadNewFont();
    bool                 loadOldFont();
    bool                 saveColorMap();
    bool                 resetColorMap();
    void                 setBeep (int, int) const;
    void                 resetBeep() const;

    FKey                 modifierKeyCorrection (const FKey&);

  private:
    struct ModifierKey  // bit field
    {
      uChar shift  : 1;  // 0..1
      uChar alt_gr : 1;  // 0..1
      uChar ctrl   : 1;  // 0..1
      uChar alt    : 1;  // 0..1
      uChar        : 4;  // padding bits
    };

    struct ModifierKeyHash
    {
      std::size_t operator () (const ModifierKey& m_key) const
      {
        uChar m{};
        std::memcpy(&m, &m_key, sizeof(uChar));
        return std::hash<uChar>()(m);
      }
    };

    struct RGB
    {
      uChar red;
      uChar green;
      uChar blue;
    };

    struct ColorMap
    {
      std::array<RGB, 16> color;
    };

    struct Pair
    {
      ModifierKey modifier;
      FKey key;
    };

    struct PairHash
    {
      std::size_t operator () (const Pair& pair) const
      {
        size_t seed = 0;
        const auto hash1 = ModifierKeyHash()(pair.modifier);
        const auto hash2 = FKeyHash()(pair.key);
        seed ^= hash1 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= hash2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
      }
    };

    struct PairEqual
    {
      bool operator () (const Pair& lhs, const Pair& rhs) const
      {
        return std::memcmp(&lhs.modifier, &rhs.modifier, sizeof(ModifierKey)) == 0
            && lhs.key == rhs.key;
      }
    };

    // Using-declaration
    using KeyMap = std::unordered_map<Pair, FKey, PairHash, PairEqual>;

    // Accessors
    int                  getFramebuffer_bpp() const;
    bool                 getScreenFont();
    bool                 getUnicodeMap ();
    ModifierKey&         getModifierKey();

    // Mutators
    int                  setScreenFont ( const uChar[], uInt, uInt, uInt
                                       , bool = false );
    int                  setUnicodeMap (struct unimapdesc*) const;
    void                 setLinuxCursorStyle (LinuxConsoleCursorStyle) const;

    // Methods
#if defined(ISA_SYSCTL_SUPPORT)
    uInt16               getInputStatusRegisterOne() const;
    uChar                readAttributeController (uChar) const;
    void                 writeAttributeController (uChar, uChar) const;
    uChar                getAttributeMode() const;
    void                 setAttributeMode (uChar) const;
    int                  setBlinkAsIntensity (bool = true) const;
    bool                 has9BitCharacters() const;
    void                 getVGAPalette();
    void                 setVGADefaultPalette();
    bool                 setVGAPalette (FColor, int, int, int);
    bool                 saveVGAPalette();
    bool                 resetVGAPalette();
#endif  // defined(ISA_SYSCTL_SUPPORT)
    void                 initKeyMap();
    void                 keyCorrection();
    void                 shiftKeyCorrection();
    void                 ctrlKeyCorrection();
    void                 altKeyCorrection();
    void                 shiftCtrlKeyCorrection();
    void                 shiftAltKeyCorrection();
    void                 ctrlAltKeyCorrection();
    void                 shiftCtrlAltKeyCorrection();
    void                 initSpecialCharacter() const;
    sInt16               getFontPos (wchar_t ucs) const;
    void                 characterFallback (wchar_t, const std::vector<wchar_t>&) const;

    // Data members
#if defined(__linux__)
    bool                 vga_font{};
    bool                 new_font{};
    bool                 has_saved_palette{};
    int                  framebuffer_bpp{-1};
    CursorStyle          linux_console_cursor_style{};
    console_font_op      screen_font{};
    unimapdesc           screen_unicode_map{};
    ColorMap             saved_color_map{};
    ColorMap             cmap{};
    KeyMap               key_map{};
    ModifierKey          mod_key{};
#endif  // defined(__linux__)
};


// FTermLinux inline functions
//----------------------------------------------------------------------
inline FString FTermLinux::getClassName() const
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
