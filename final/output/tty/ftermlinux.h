/***********************************************************************
* ftermlinux.h - Contains the Linux terminal functions                 *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2024 Markus Gans                                      *
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
#else
  // struct forward declaration
  struct console_font_op;
  struct unimapdesc;
#endif  // defined(__linux__)

#include <sys/ioctl.h>
#include <unistd.h>

#include <bitset>
#include <cstdio>  // need for sprintf
#include <cstring>
#include <unordered_map>
#include <utility>
#include <vector>

#include "final/output/tty/ftermdata.h"
#include "final/util/fstring.h"

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

    // Disable move constructor
    FTermLinux (FTermLinux&&) noexcept = delete;

    // Destructor
    ~FTermLinux();

    // Disable copy assignment operator (=)
    auto operator = (const FTermLinux&) -> FTermLinux& = delete;

    // Disable move assignment operator (=)
    auto operator = (FTermLinux&&) noexcept -> FTermLinux& = delete;

    // Accessors
    auto  getClassName() const -> FString;
    static auto getInstance() -> FTermLinux&;
    auto  getCursorStyle() const -> CursorStyle;
    auto  getCursorStyleString() -> char*;
    auto  getFramebufferBpp() const noexcept -> int;

    // Mutators
    auto  setCursorStyle (CursorStyle) -> bool;
    auto  setPalette (FColor, int, int, int) -> bool;
    void  setUTF8 (bool = true) const;

    // Inquiries
    static auto isLinuxConsole() -> bool;
    auto isVGAFontUsed() const noexcept -> bool;
    auto isNewFontUsed() const noexcept -> bool;

    // Methods
    void  init();
    void  initCharMap() const;
    void  finish() const;
    auto  loadVGAFont() -> bool;
    auto  loadNewFont() -> bool;
    auto  loadOldFont() -> bool;
    auto  saveColorMap() -> bool;
    auto  resetColorMap() -> bool;
    void  setBeep (int, int) const;
    void  resetBeep() const;
    auto  modifierKeyCorrection (const FKey&) -> FKey;

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
      auto operator () (const ModifierKey& m_key) const noexcept -> std::size_t
      {
        uChar m{};
        std::memcpy(&m, &m_key, sizeof(uChar));
        return std::hash<uChar>{}(m);
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
      auto operator () (const Pair& pair) const noexcept -> std::size_t
      {
        size_t seed = 0;
        const auto hash1 = ModifierKeyHash{}(pair.modifier);
        const auto hash2 = EnumHash<FKey>{}(pair.key);
        seed ^= hash1 + 0x9e3779b9 + (seed << 6u) + (seed >> 2u);
        seed ^= hash2 + 0x9e3779b9 + (seed << 6u) + (seed >> 2u);
        return seed;
      }
    };

    struct PairEqual
    {
      auto operator () (const Pair& lhs, const Pair& rhs) const noexcept -> bool
      {
        return std::memcmp(&lhs.modifier, &rhs.modifier, sizeof(ModifierKey)) == 0
            && lhs.key == rhs.key;
      }
    };

    // Using-declaration
    using KeyMap = std::unordered_map<Pair, FKey, PairHash, PairEqual>;
    using FontData = std::vector<uChar>;
    using UnicodeEntries = std::vector<struct unipair>;

    // Accessors
    auto  getFramebuffer_bpp() const -> int;
    auto  getScreenFont() -> bool;
    auto  getUnicodeMap () -> bool;
    auto  getModifierKey() & -> ModifierKey&;

    // Inquiries
    auto  isLinuxTerm() const -> bool;

    // Mutators
    auto  setScreenFont ( const uChar[], uInt, uInt, uInt
                        , bool = false ) -> int;
    auto  setUnicodeMap (struct unimapdesc*) const -> int;
    void  setLinuxCursorStyle (LinuxConsoleCursorStyle) const;

    // Methods
#if defined(ISA_SYSCTL_SUPPORT)
    auto  getInputStatusRegisterOne() const -> uInt16;
    auto  readAttributeController (uChar) const -> uChar;
    void  writeAttributeController (uChar, uChar) const;
    auto  getAttributeMode() const -> uChar;
    void  setAttributeMode (uChar) const;
    auto  setBlinkAsIntensity (bool = true) const -> int;
    auto  has9BitCharacters() const -> bool;
    void  getVGAPalette();
    void  setVGADefaultPalette();
    auto  setVGAPalette (FColor, int, int, int) -> bool;
    auto  saveVGAPalette() -> bool;
    auto  resetVGAPalette() -> bool;
#endif  // defined(ISA_SYSCTL_SUPPORT)
    void  initKeyMap();
    void  keyCorrection();
    void  shiftKeyCorrection();
    void  ctrlKeyCorrection();
    void  altKeyCorrection();
    void  shiftCtrlKeyCorrection();
    void  shiftAltKeyCorrection();
    void  ctrlAltKeyCorrection();
    void  shiftCtrlAltKeyCorrection();
    void  initSpecialCharacter() const;
    auto  getFontPos (wchar_t ucs) const -> sInt16;
    void  deleteFontData (console_font_op&);
    void  deleteUnicodeMapEntries (unimapdesc&);
    void  characterFallback (wchar_t, const std::vector<wchar_t>&) const;

    // Data members
#if defined(__linux__)
    bool             vga_font{};
    bool             new_font{};
    bool             has_saved_palette{};
    int              framebuffer_bpp{-1};
    CursorStyle      linux_console_cursor_style{};
    console_font_op  screen_font{};
    unimapdesc       screen_unicode_map{};
    FontData         font_data{};
    FontData         screen_font_data{};
    UnicodeEntries   unicode_entries{};
    ColorMap         saved_color_map{};
    ColorMap         cmap{};
    KeyMap           key_map{};
    ModifierKey      mod_key{};
#endif  // defined(__linux__)
};


// FTermLinux inline functions
//----------------------------------------------------------------------
inline auto FTermLinux::getClassName() const -> FString
{ return "FTermLinux"; }

//----------------------------------------------------------------------
#if defined(__linux__)
inline auto FTermLinux::getFramebufferBpp() const noexcept -> int
{ return framebuffer_bpp; }

//----------------------------------------------------------------------
inline auto FTermLinux::isVGAFontUsed() const noexcept -> bool
{ return vga_font; }

//----------------------------------------------------------------------
inline auto FTermLinux::isNewFontUsed() const noexcept -> bool
{ return new_font; }
#endif  // defined(__linux__)

}  // namespace finalcut

#endif  // FTERMLINUX_H
