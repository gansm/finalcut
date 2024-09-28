/***********************************************************************
* foptiattr.h - Sets video attributes in optimized order               *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2016-2023 Markus Gans                                      *
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
#include <array>
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
    struct TermEnvTextStyle
    {
      const char* on;
      const char* off;
    };

    struct TermEnvColorStyle
    {
      const char* a_foreground;
      const char* a_background;
      const char* foreground;
      const char* background;
      const char* color_pair;
      const char* orig_pair;
      const char* orig_colors;
      int         max_color;
      int         attr_without_color;
      bool        ansi_default_color;
    };

    struct TermEnv
    {
      TermEnvTextStyle  t_set_bold;
      TermEnvTextStyle  t_set_dim;
      TermEnvTextStyle  t_set_italics;
      TermEnvTextStyle  t_set_underline;
      TermEnvTextStyle  t_set_blink;
      TermEnvTextStyle  t_set_reverse;
      TermEnvTextStyle  t_set_standout;
      TermEnvTextStyle  t_set_secure;
      TermEnvTextStyle  t_set_protected;
      TermEnvTextStyle  t_set_crossed_out;
      TermEnvTextStyle  t_set_dbl_underline;
      TermEnvTextStyle  t_set_attributes;
      TermEnvTextStyle  t_set_alt_charset;
      TermEnvTextStyle  t_set_pc_charset;
      TermEnvColorStyle t_set_color;
    };

    // Constructor
    FOptiAttr();

    // Accessors
    auto        getClassName() const -> FString;
    static auto getInstance() -> FOptiAttr&;

    // Mutators
    void        setTermEnvironment (const TermEnv&);
    void        setMaxColor (const int&) noexcept;
    void        setNoColorVideo (int) noexcept;
    void        setDefaultColorSupport() noexcept;
    void        unsetDefaultColorSupport() noexcept;
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
    void        set_orig_colors (const char[]);

    // Inquiry
    static auto isNormal (const FChar&) -> bool;

    // Methods
    void        initialize();
    static auto vga2ansi (FColor) -> FColor;
    auto        changeAttribute (FChar&, FChar&) -> std::string;

  private:
    struct Capability
    {
      const char* cap{};
      bool  caused_reset{};
    };

    struct TextStyle
    {
      Capability on{};
      Capability off{};
    };

    struct ColorStyle
    {
      Capability   a_foreground{};
      Capability   a_background{};
      Capability   foreground{};
      Capability   background{};
      Capability   color_pair{};
      Capability   orig_pair{};
      Capability   orig_colors{};
      int          attr_without_color{0};
      int          max_color{1};
      bool         monochron{true};
      bool         ansi_default_color{false};
    };

    struct AttributeChanges
    {
      FChar on{};
      FChar off{};
    };

    // Using-declarations
    using SetFunctionCall = std::function<bool(FOptiAttr*, FChar&)>;

    struct AttributeHandlerEntry
    {
      FAttribute mask;
      SetFunctionCall function;
    };

    using AttributeHandlers = std::array<AttributeHandlerEntry, 13>;
    using NoColorVideoHandler = std::function<void(FOptiAttr*, FChar&)>;
    using NoColorVideoHandlerTable = std::array<NoColorVideoHandler, 18>;

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
    void        setAttributes ( const FAttribute&
                              , const AttributeHandlers&, FChar& );

    // Inquiries
    static auto hasColor (const FChar&) -> bool;
    static auto hasAttribute (const FChar&) -> bool;
    static auto hasNoAttribute (const FChar&) -> bool;
    auto        isItalicsUsed (const FChar&, const FChar&) const -> bool;
    auto        isCrossedOutUsed (const FChar&, const FChar&) const -> bool;
    auto        isDoubleUnderlineUsed (const FChar&, const FChar&) const -> bool;
    auto        isPCcharsetUsed (const FChar&, const FChar&) const -> bool;
    auto        isPCcharsetUsable (FChar&, const FChar&) -> bool;
    auto        hasColorChanged (const FChar&, const FChar&) const -> bool;

    // Methods
    void        resetColor (FChar&) const;
    void        prevent_no_color_video_attributes (FChar&, bool = false);
    void        deactivateAttributes (FChar&, FChar&);
    void        changeAttributeSGR (FChar&, FChar&);
    void        changeAttributeSeparately (FChar&, FChar&);
    void        change_color (FChar&, FChar&);
    void        normalizeColor (FColor&) const noexcept;
    void        handleDefaultColors (FChar&, FChar&, FColor&, FColor&);
    void        change_to_default_color (FChar&, FChar&, FColor&, FColor&);
    void        setDefaultForeground (FChar&);
    void        setDefaultBackground (FChar&);
    void        change_current_color (const FChar&, const FColor, const FColor);
    void        resetAttribute (FChar&) const;
    void        reset (FChar&) const;
    auto        caused_reset_attributes (const char[], uChar = all_tests) const -> bool;
    void        init_reset_attribute (Capability&, uChar = all_tests) const;
    auto        fake_reverse_color_change (const FChar& term) const -> bool;
    auto        hasCharsetEquivalence() const -> bool;
    static auto getNoColorVideoHandlerTable() -> const NoColorVideoHandlerTable&;
    static auto getAttributeOnHandlers() -> const AttributeHandlers&;
    static auto getAttributeOffHandlers() -> const AttributeHandlers&;
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
    auto        append_sequence (CharT) -> bool;
    template <typename CharT
            , enable_if_char_array_t<CharT> = nullptr>
    auto        append_sequence (CharT) -> bool;
    auto        append_sequence (const std::string&) -> bool;

    // Data members
    TextStyle        F_bold{};
    TextStyle        F_dim{};
    TextStyle        F_italics{};
    TextStyle        F_underline{};
    TextStyle        F_blink{};
    TextStyle        F_reverse{};
    TextStyle        F_standout{};
    TextStyle        F_secure{};
    TextStyle        F_protected{};
    TextStyle        F_crossed_out{};
    TextStyle        F_dbl_underline{};
    TextStyle        F_attributes{};
    TextStyle        F_alt_charset{};
    TextStyle        F_pc_charset{};
    ColorStyle       F_color{};

    AttributeChanges changes{};
    std::string      attr_buf{};
    SGRoptimizer     sgr_optimizer{attr_buf};
    bool             alt_equal_pc_charset{false};
    bool             fake_reverse{false};
};


// FOptiAttr inline functions
//----------------------------------------------------------------------
inline auto FOptiAttr::getClassName() const -> FString
{ return "FOptiAttr"; }

//----------------------------------------------------------------------
inline void FOptiAttr::setMaxColor (const int& c) noexcept
{ F_color.max_color = c; }

//----------------------------------------------------------------------
inline void FOptiAttr::setNoColorVideo (int attr) noexcept
{ F_color.attr_without_color = attr; }

//----------------------------------------------------------------------
inline void FOptiAttr::setDefaultColorSupport() noexcept
{ F_color.ansi_default_color = true; }

//----------------------------------------------------------------------
inline void FOptiAttr::unsetDefaultColorSupport() noexcept
{ F_color.ansi_default_color = false; }

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
