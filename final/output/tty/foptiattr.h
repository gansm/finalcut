/***********************************************************************
* foptiattr.h - Sets video attributes in optimized order               *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2016-2022 Markus Gans                                      *
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
 *   ▕▔▔▔▔▔▔▔▔▔▔▔▏1     1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *   ▕ FOptiAttr ▏- - - -▕ SGRoptimizer ▏
 *   ▕▁▁▁▁▁▁▁▁▁▁▁▏       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FOPTIATTR_H
#define FOPTIATTR_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <cassert>
#include <algorithm>  // need for std::swap
#include <string>

#include "final/ftypes.h"
#include "final/output/tty/sgr_optimizer.h"
#include "final/util/fstring.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FOptiAttr
//----------------------------------------------------------------------

class FOptiAttr final
{
  public:
    struct TermEnv
    {
      const char* t_enter_bold_mode;
      const char* t_exit_bold_mode;
      const char* t_enter_dim_mode;
      const char* t_exit_dim_mode;
      const char* t_enter_italics_mode;
      const char* t_exit_italics_mode;
      const char* t_enter_underline_mode;
      const char* t_exit_underline_mode;
      const char* t_enter_blink_mode;
      const char* t_exit_blink_mode;
      const char* t_enter_reverse_mode;
      const char* t_exit_reverse_mode;
      const char* t_enter_standout_mode;
      const char* t_exit_standout_mode;
      const char* t_enter_secure_mode;
      const char* t_exit_secure_mode;
      const char* t_enter_protected_mode;
      const char* t_exit_protected_mode;
      const char* t_enter_crossed_out_mode;
      const char* t_exit_crossed_out_mode;
      const char* t_enter_dbl_underline_mode;
      const char* t_exit_dbl_underline_mode;
      const char* t_set_attributes;
      const char* t_exit_attribute_mode;
      const char* t_enter_alt_charset_mode;
      const char* t_exit_alt_charset_mode;
      const char* t_enter_pc_charset_mode;
      const char* t_exit_pc_charset_mode;
      const char* t_set_a_foreground;
      const char* t_set_a_background;
      const char* t_set_foreground;
      const char* t_set_background;
      const char* t_set_color_pair;
      const char* t_orig_pair;
      const char* t_orig_colors;
      int   max_color;
      int   attr_without_color;
      bool  ansi_default_color;
    };

    // Constructor
    FOptiAttr();

    // Accessors
    auto        getClassName() const -> FString;
    static auto getInstance() -> FOptiAttr&;

    // Mutators
    void        setTermEnvironment (const TermEnv&);
    inline void setMaxColor (const int&) noexcept;
    inline void setNoColorVideo (int) noexcept;
    inline void setDefaultColorSupport() noexcept;
    inline void unsetDefaultColorSupport() noexcept;
    void        set_enter_bold_mode (const char[]);
    void        set_exit_bold_mode (const char[]);
    void        set_enter_dim_mode (const char[]);
    void        set_exit_dim_mode (const char[]);
    void        set_enter_italics_mode (const char[]);
    void        set_exit_italics_mode (const char[]);
    void        set_enter_underline_mode (const char[]);
    void        set_exit_underline_mode (const char[]);
    void        set_enter_blink_mode (const char[]);
    void        set_exit_blink_mode (const char[]);
    void        set_enter_reverse_mode (const char[]);
    void        set_exit_reverse_mode (const char[]);
    void        set_enter_secure_mode (const char[]);
    void        set_exit_secure_mode (const char[]);
    void        set_enter_protected_mode (const char[]);
    void        set_exit_protected_mode (const char[]);
    void        set_enter_crossed_out_mode (const char[]);
    void        set_exit_crossed_out_mode (const char[]);
    void        set_enter_dbl_underline_mode (const char[]);
    void        set_exit_dbl_underline_mode (const char[]);
    void        set_enter_standout_mode (const char[]);
    void        set_exit_standout_mode (const char[]);
    void        set_set_attributes (const char[]);
    void        set_exit_attribute_mode (const char[]);
    void        set_enter_alt_charset_mode (const char[]);
    void        set_exit_alt_charset_mode (const char[]);
    void        set_enter_pc_charset_mode (const char[]);
    void        set_exit_pc_charset_mode (const char[]);
    void        set_a_foreground_color (const char[]);
    void        set_a_background_color (const char[]);
    void        set_foreground_color (const char[]);
    void        set_background_color (const char[]);
    void        set_term_color_pair (const char[]);
    void        set_orig_pair (const char[]);
    void        set_orig_orig_colors (const char[]);

    // Inquiry
    static auto isNormal (const FChar&) -> bool;

    // Methods
    void        initialize();
    static auto vga2ansi (FColor) -> FColor;
    auto        changeAttribute (FChar&, FChar&) -> std::string;

  private:
    struct Capability
    {
      const char* cap;
      bool  caused_reset;
    };

    // Enumerations
    enum init_reset_tests
    {
      no_test         = 0x00,
      test_ansi_reset = 0x01,  // ANSI X3.64 terminal
      test_adm3_reset = 0x02,  // Lear Siegler ADM-3 terminal
      same_like_ue    = 0x04,
      same_like_se    = 0x08,
      same_like_me    = 0x10,
      all_tests       = 0x1f
    };

    enum attr_modes
    {
      standout_mode    = 1,
      underline_mode   = 2,
      reverse_mode     = 4,
      blink_mode       = 8,
      dim_mode         = 16,
      bold_mode        = 32,
      invisible_mode   = 64,
      protected_mode   = 128,
      alt_charset_mode = 256,
      horizontal_mode  = 512,
      left_mode        = 1024,
      low_mode         = 2048,
      right_mode       = 4096,
      top_mode         = 8192,
      vertical_mode    = 16384,
      italic_mode      = 32768,
      no_mode          = 65536
    };

    // Mutators
    auto        setTermBold (FChar&) -> bool;
    auto        unsetTermBold (FChar&) -> bool;
    auto        setTermDim (FChar&) -> bool;
    auto        unsetTermDim (FChar&) -> bool;
    auto        setTermItalic (FChar&) -> bool;
    auto        unsetTermItalic (FChar&) -> bool;
    auto        setTermUnderline (FChar&) -> bool;
    auto        unsetTermUnderline (FChar&) -> bool;
    auto        setTermBlink (FChar&) -> bool;
    auto        unsetTermBlink (FChar&) -> bool;
    auto        setTermReverse (FChar&) -> bool;
    auto        unsetTermReverse (FChar&) -> bool;
    auto        setTermStandout (FChar&) -> bool;
    auto        unsetTermStandout (FChar&) -> bool;
    auto        setTermInvisible (FChar&) -> bool;
    auto        unsetTermInvisible (FChar&) -> bool;
    auto        setTermProtected (FChar&) -> bool;
    auto        unsetTermProtected (FChar&) -> bool;
    auto        setTermCrossedOut (FChar&) -> bool;
    auto        unsetTermCrossedOut (FChar&) -> bool;
    auto        setTermDoubleUnderline (FChar&) -> bool;
    auto        unsetTermDoubleUnderline (FChar&) -> bool;
    auto        setTermAttributes (FChar&, const TCapAttributes&) -> bool;
    auto        unsetTermAttributes (FChar&) -> bool;
    auto        setTermAltCharset (FChar&) -> bool;
    auto        unsetTermAltCharset (FChar&) -> bool;
    auto        setTermPCcharset (FChar&) -> bool;
    auto        unsetTermPCcharset (FChar&) -> bool;
    auto        setTermDefaultColor (FChar&) -> bool;
    void        setAttributesOn (FChar&);
    void        setAttributesOff (FChar&);

    // Inquiries
    static auto hasColor (const FChar&) -> bool;
    static auto hasAttribute (const FChar&) -> bool;
    static auto hasNoAttribute (const FChar&) -> bool;

    // Methods
    auto        hasColorChanged (const FChar&, const FChar&) const -> bool;
    void        resetColor (FChar&) const;
    void        prevent_no_color_video_attributes (FChar&, bool = false);
    void        deactivateAttributes (FChar&, FChar&);
    void        changeAttributeSGR (FChar&, FChar&);
    void        changeAttributeSeparately (FChar&, FChar&);
    void        change_color (FChar&, FChar&);
    void        change_to_default_color (FChar&, FChar&, FColor&, FColor&);
    void        change_current_color (const FChar&, FColor, FColor);
    void        resetAttribute (FChar&) const;
    void        reset (FChar&) const;
    auto        caused_reset_attributes (const char[], uChar = all_tests) const -> bool;
    auto        hasCharsetEquivalence() const -> bool;
    static auto getByte0ReverseMask() -> uInt8;
    static auto getByte1Mask() -> uInt8;
    static auto getByte1ResetMask() -> uInt8;
    static auto getByte2ResetMask() -> uInt8;
    void        detectSwitchOn (const FChar&, const FChar&);
    void        detectSwitchOff (const FChar&, const FChar&);
    auto        switchOn() const -> bool;
    auto        switchOff() const -> bool;
    template <typename CharT
            , enable_if_char_ptr_t<CharT> = nullptr>
    inline auto append_sequence (CharT) -> bool;
    template <typename CharT
            , enable_if_char_array_t<CharT> = nullptr>
    inline auto append_sequence (CharT) -> bool;
    auto        append_sequence (const std::string&) -> bool;

    // Data members
    Capability   F_enter_bold_mode{};
    Capability   F_exit_bold_mode{};
    Capability   F_enter_dim_mode{};
    Capability   F_exit_dim_mode{};
    Capability   F_enter_italics_mode{};
    Capability   F_exit_italics_mode{};
    Capability   F_enter_underline_mode{};
    Capability   F_exit_underline_mode{};
    Capability   F_enter_blink_mode{};
    Capability   F_exit_blink_mode{};
    Capability   F_enter_reverse_mode{};
    Capability   F_exit_reverse_mode{};
    Capability   F_enter_standout_mode{};
    Capability   F_exit_standout_mode{};
    Capability   F_enter_secure_mode{};
    Capability   F_exit_secure_mode{};
    Capability   F_enter_protected_mode{};
    Capability   F_exit_protected_mode{};
    Capability   F_enter_crossed_out_mode{};
    Capability   F_exit_crossed_out_mode{};
    Capability   F_enter_dbl_underline_mode{};
    Capability   F_exit_dbl_underline_mode{};
    Capability   F_set_attributes{};
    Capability   F_exit_attribute_mode{};
    Capability   F_enter_alt_charset_mode{};
    Capability   F_exit_alt_charset_mode{};
    Capability   F_enter_pc_charset_mode{};
    Capability   F_exit_pc_charset_mode{};
    Capability   F_set_a_foreground{};
    Capability   F_set_a_background{};
    Capability   F_set_foreground{};
    Capability   F_set_background{};
    Capability   F_set_color_pair{};
    Capability   F_orig_pair{};
    Capability   F_orig_colors{};

    FChar        on{};
    FChar        off{};
    std::string  attr_buf{};
    SGRoptimizer sgr_optimizer{attr_buf};
    int          max_color{1};
    int          attr_without_color{0};
    bool         ansi_default_color{false};
    bool         alt_equal_pc_charset{false};
    bool         monochron{true};
    bool         fake_reverse{false};
};


// FOptiAttr inline functions
//----------------------------------------------------------------------
inline auto FOptiAttr::getClassName() const -> FString
{ return "FOptiAttr"; }

//----------------------------------------------------------------------
inline void FOptiAttr::setMaxColor (const int& c) noexcept
{ max_color = c; }

//----------------------------------------------------------------------
inline void FOptiAttr::setNoColorVideo (int attr) noexcept
{ attr_without_color = attr; }

//----------------------------------------------------------------------
inline void FOptiAttr::setDefaultColorSupport() noexcept
{ ansi_default_color = true; }

//----------------------------------------------------------------------
inline void FOptiAttr::unsetDefaultColorSupport() noexcept
{ ansi_default_color = false; }

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_char_ptr_t<CharT>>
inline auto FOptiAttr::append_sequence (CharT seq) -> bool
{
  // for char* and const char*
  return seq
       ? append_sequence(std::string(seq))
       : false;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_char_array_t<CharT>>
inline auto FOptiAttr::append_sequence (CharT seq) -> bool
{
  // for char[] and const char[]
  return append_sequence(std::string(seq));
}

}  // namespace finalcut

#endif  // FOPTIATTR_H
