/***********************************************************************
* foptiattr.cpp - Sets video attributes in optimized order             *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2016-2026 Markus Gans                                      *
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

#include <strings.h>  // need for ffs()

#include <array>
#include <cstring>
#include <functional>
#include <memory>
#include <utility>

#include "final/fc.h"
#include "final/fstartoptions.h"
#include "final/output/tty/foptiattr.h"
#include "final/output/tty/ftermcap.h"

namespace finalcut
{

namespace internal
{

template<typename T>
constexpr auto createMask (T setter) noexcept -> uInt32
{
  FCharAttribute mask{};
  setter(mask);
  return FCharAttribute_to_uInt32(mask);
}

constexpr void setBoldMask (FCharAttribute& attr) noexcept
{
  attr.bold = true;
}

constexpr void setDimMask (FCharAttribute& attr) noexcept
{
  attr.dim = true;
}

constexpr void setItalicMask (FCharAttribute& attr) noexcept
{
  attr.italic = true;
}

constexpr void setUnderlineMask (FCharAttribute& attr) noexcept
{
  attr.underline = true;
}

constexpr void setBlinkMask (FCharAttribute& attr) noexcept
{
  attr.blink = true;
}

constexpr void setReverseMask (FCharAttribute& attr) noexcept
{
  attr.reverse = true;
}

constexpr void setStandoutMask (FCharAttribute& attr) noexcept
{
  attr.standout = true;
}

constexpr void setInvisibleMask (FCharAttribute& attr) noexcept
{
  attr.invisible = true;
}

constexpr void setProtectMask (FCharAttribute& attr) noexcept
{
  attr.protect = true;
}

constexpr void setCrossedOutMask (FCharAttribute& attr) noexcept
{
  attr.crossed_out = true;
}

constexpr void setDblUnderlineMask (FCharAttribute& attr) noexcept
{
  attr.dbl_underline = true;
}

constexpr void setAltCharsetMask (FCharAttribute& attr) noexcept
{
  attr.alt_charset = true;
}

constexpr void setPcCharsetMask (FCharAttribute& attr) noexcept
{
  attr.pc_charset = true;
}

constexpr void setFakeReverseMask (FCharAttribute& attr) noexcept
{
  attr.reverse = true;
  attr.standout = true;
}

constexpr void setAttributeMask (FCharAttribute& attr) noexcept
{
  attr.bold = true;
  attr.dim = true;
  attr.italic = true;
  attr.underline = true;
  attr.blink = true;
  attr.reverse = true;
  attr.standout = true;
  attr.invisible = true;
  attr.protect = true;
  attr.crossed_out = true;
  attr.dbl_underline = true;
  attr.alt_charset = true;
  attr.pc_charset = true;
}

constexpr void setResetMask (FCharAttribute& attr) noexcept
{
  // Set bits that must not be reset
  attr.transparent = true;
  attr.color_overlay = true;
  attr.inherit_background = true;
  attr.no_changes = true;
  attr.printed = true;
}

template<typename T>
constexpr auto createColorPair (T setter) noexcept -> uInt32
{
  FCellColor fcellcolor{};
  setter(fcellcolor);
  return FCellColor_to_uInt32(fcellcolor);
}

constexpr void setDefaultColorPair (FCellColor& fcellcolor) noexcept
{
  fcellcolor.pair = {FColor::Default, FColor::Default};
}

struct var
{
  static constexpr auto bold_mask                = createMask(setBoldMask);
  static constexpr auto dim_mask                 = createMask(setDimMask);
  static constexpr auto italic_mask              = createMask(setItalicMask);
  static constexpr auto underline_mask           = createMask(setUnderlineMask);
  static constexpr auto blink_mask               = createMask(setBlinkMask);
  static constexpr auto reverse_mask             = createMask(setReverseMask);
  static constexpr auto standout_mask            = createMask(setStandoutMask);
  static constexpr auto invisible_mask           = createMask(setInvisibleMask);
  static constexpr auto protect_mask             = createMask(setProtectMask);
  static constexpr auto crossed_out_mask         = createMask(setCrossedOutMask);
  static constexpr auto dbl_underline_mask       = createMask(setDblUnderlineMask);
  static constexpr auto alt_charset_mask         = createMask(setAltCharsetMask);
  static constexpr auto pc_charset_mask          = createMask(setPcCharsetMask);
  static constexpr auto fake_reverse_mask        = createMask(setFakeReverseMask);
  static constexpr auto attribute_mask           = createMask(setAttributeMask);
  static constexpr auto bold_reset_mask          = ~bold_mask;
  static constexpr auto dim_reset_mask           = ~dim_mask;
  static constexpr auto italic_reset_mask        = ~italic_mask;
  static constexpr auto underline_reset_mask     = ~underline_mask;
  static constexpr auto blink_reset_mask         = ~blink_mask;
  static constexpr auto reverse_reset_mask       = ~reverse_mask;
  static constexpr auto standout_reset_mask      = ~standout_mask;
  static constexpr auto invisible_reset_mask     = ~invisible_mask;
  static constexpr auto protect_reset_mask       = ~protect_mask;
  static constexpr auto crossed_out_reset_mask   = ~crossed_out_mask;
  static constexpr auto dbl_underline_reset_mask = ~dbl_underline_mask;
  static constexpr auto alt_charset_reset_mask   = ~alt_charset_mask;
  static constexpr auto pc_charset_reset_mask    = ~pc_charset_mask;
  static constexpr auto reset_mask               = createMask(setResetMask);
  static constexpr auto default_color_pair       = createColorPair(setDefaultColorPair);
  static constexpr char sgr_39[]                 = {CSI "39m"};
  static constexpr char sgr_39_49[]              = {CSI "39;49m"};
};

constexpr uInt32 var::bold_mask;
constexpr uInt32 var::dim_mask;
constexpr uInt32 var::italic_mask;
constexpr uInt32 var::underline_mask;
constexpr uInt32 var::blink_mask;
constexpr uInt32 var::reverse_mask;
constexpr uInt32 var::standout_mask;
constexpr uInt32 var::invisible_mask;
constexpr uInt32 var::protect_mask;
constexpr uInt32 var::crossed_out_mask;
constexpr uInt32 var::dbl_underline_mask;
constexpr uInt32 var::alt_charset_mask;
constexpr uInt32 var::pc_charset_mask;
constexpr uInt32 var::fake_reverse_mask;
constexpr uInt32 var::attribute_mask;
constexpr uInt32 var::bold_reset_mask;
constexpr uInt32 var::dim_reset_mask;
constexpr uInt32 var::italic_reset_mask;
constexpr uInt32 var::underline_reset_mask;
constexpr uInt32 var::blink_reset_mask;
constexpr uInt32 var::reverse_reset_mask;
constexpr uInt32 var::standout_reset_mask;
constexpr uInt32 var::invisible_reset_mask;
constexpr uInt32 var::protect_reset_mask;
constexpr uInt32 var::crossed_out_reset_mask;
constexpr uInt32 var::dbl_underline_reset_mask;
constexpr uInt32 var::alt_charset_reset_mask;
constexpr uInt32 var::pc_charset_reset_mask;
constexpr uInt32 var::reset_mask;
constexpr uInt32 var::default_color_pair;
constexpr char   var::sgr_39[];
constexpr char   var::sgr_39_49[];

}  // namespace internal

// Function prototypes
auto has_foreground_changes (const FChar&, FColor, bool) noexcept -> bool;
auto has_background_changes (const FChar&, FColor, bool) noexcept -> bool;


//----------------------------------------------------------------------
// class FOptiAttr
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FOptiAttr::FOptiAttr()
{
  attr_buf.reserve(SGRoptimizer::ATTR_BUF_SIZE);
}


// public methods of FOptiAttr
//----------------------------------------------------------------------
auto FOptiAttr::getInstance() -> FOptiAttr&
{
  static const auto& opti_attr = std::make_unique<FOptiAttr>();
  return *opti_attr;
}

//----------------------------------------------------------------------
void FOptiAttr::setTermEnvironment (const TermEnv& term_env)
{
  // Set all required termcap values at once
  // and initialize the FOptiAttr environment

  set_enter_bold_mode (term_env.t_set_bold.on);
  set_exit_bold_mode (term_env.t_set_bold.off);
  set_enter_dim_mode (term_env.t_set_dim.on);
  set_exit_dim_mode (term_env.t_set_dim.off);
  set_enter_italics_mode (term_env.t_set_italics.on);
  set_exit_italics_mode (term_env.t_set_italics.off);
  set_enter_underline_mode (term_env.t_set_underline.on);
  set_exit_underline_mode (term_env.t_set_underline.off);
  set_enter_blink_mode (term_env.t_set_blink.on);
  set_exit_blink_mode (term_env.t_set_blink.off);
  set_enter_reverse_mode (term_env.t_set_reverse.on);
  set_exit_reverse_mode (term_env.t_set_reverse.off);
  set_enter_standout_mode (term_env.t_set_standout.on);
  set_exit_standout_mode (term_env.t_set_standout.off);
  set_enter_secure_mode (term_env.t_set_secure.on);
  set_exit_secure_mode (term_env.t_set_secure.off);
  set_enter_protected_mode (term_env.t_set_protected.on);
  set_exit_protected_mode (term_env.t_set_protected.off);
  set_enter_crossed_out_mode (term_env.t_set_crossed_out.on);
  set_exit_crossed_out_mode (term_env.t_set_crossed_out.off);
  set_enter_dbl_underline_mode (term_env.t_set_dbl_underline.on);
  set_exit_dbl_underline_mode (term_env.t_set_dbl_underline.off);
  set_set_attributes (term_env.t_set_attributes.on);
  set_exit_attribute_mode (term_env.t_set_attributes.off);
  set_enter_alt_charset_mode (term_env.t_set_alt_charset.on);
  set_exit_alt_charset_mode (term_env.t_set_alt_charset.off);
  set_enter_pc_charset_mode (term_env.t_set_pc_charset.on);
  set_exit_pc_charset_mode (term_env.t_set_pc_charset.off);
  set_a_foreground_color (term_env.t_set_color.a_foreground);
  set_a_background_color (term_env.t_set_color.a_background);
  set_foreground_color (term_env.t_set_color.foreground);
  set_background_color (term_env.t_set_color.background);
  set_term_color_pair (term_env.t_set_color.color_pair);
  set_orig_pair (term_env.t_set_color.orig_pair);
  set_orig_colors (term_env.t_set_color.orig_colors);

  F_color.max_color = term_env.t_set_color.max_color;
  F_color.attr_without_color = term_env.t_set_color.attr_without_color;
  F_color.ansi_default_color = term_env.t_set_color.ansi_default_color;

  initialize();
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_bold_mode (const char cap[]) noexcept
{
  set_mode_on (F_bold, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_bold_mode (const char cap[]) noexcept
{
  set_mode_off (F_bold, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_dim_mode (const char cap[]) noexcept
{
  set_mode_on (F_dim, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_dim_mode (const char cap[]) noexcept
{
  set_mode_off (F_dim, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_italics_mode (const char cap[]) noexcept
{
  set_mode_on (F_italics, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_italics_mode (const char cap[]) noexcept
{
  set_mode_off (F_italics, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_underline_mode (const char cap[]) noexcept
{
  set_mode_on (F_underline, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_underline_mode (const char cap[]) noexcept
{
  set_mode_off (F_underline, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_blink_mode (const char cap[]) noexcept
{
  set_mode_on (F_blink, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_blink_mode (const char cap[]) noexcept
{
  set_mode_off (F_blink, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_reverse_mode (const char cap[]) noexcept
{
  set_mode_on (F_reverse, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_reverse_mode (const char cap[]) noexcept
{
  set_mode_off (F_reverse, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_secure_mode (const char cap[]) noexcept
{
  set_mode_on (F_secure, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_secure_mode (const char cap[]) noexcept
{
  set_mode_off (F_secure, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_protected_mode (const char cap[]) noexcept
{
  set_mode_on (F_protected, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_protected_mode (const char cap[]) noexcept
{
  set_mode_off (F_protected, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_crossed_out_mode (const char cap[]) noexcept
{
  set_mode_on (F_crossed_out, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_crossed_out_mode (const char cap[]) noexcept
{
  set_mode_off (F_crossed_out, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_dbl_underline_mode (const char cap[]) noexcept
{
  set_mode_on (F_dbl_underline, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_dbl_underline_mode (const char cap[]) noexcept
{
  set_mode_off (F_dbl_underline, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_standout_mode (const char cap[]) noexcept
{
  set_mode_on (F_standout, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_standout_mode (const char cap[]) noexcept
{
  set_mode_off (F_standout, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_set_attributes (const char cap[]) noexcept
{
  set_mode_on (F_attributes, cap, true);
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_attribute_mode (const char cap[]) noexcept
{
  set_mode_off (F_attributes, cap, true);
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_alt_charset_mode (const char cap[]) noexcept
{
  set_mode_on (F_alt_charset, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_alt_charset_mode (const char cap[]) noexcept
{
  set_mode_off (F_alt_charset, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_pc_charset_mode (const char cap[]) noexcept
{
  set_mode_on (F_pc_charset, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_pc_charset_mode (const char cap[]) noexcept
{
  set_mode_off (F_pc_charset, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_a_foreground_color (const char cap[]) noexcept
{
  set_mode (F_color.a_foreground, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_a_background_color (const char cap[]) noexcept
{
  set_mode (F_color.a_background, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_foreground_color (const char cap[]) noexcept
{
  set_mode (F_color.foreground, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_background_color (const char cap[]) noexcept
{
  set_mode (F_color.background, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_term_color_pair (const char cap[]) noexcept
{
  set_mode (F_color.color_pair, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_orig_pair (const char cap[]) noexcept
{
  set_mode (F_color.orig_pair, cap, false);
}

//----------------------------------------------------------------------
void FOptiAttr::set_orig_colors (const char cap[]) noexcept
{
  set_mode (F_color.orig_colors, cap, false);
}

//----------------------------------------------------------------------
auto FOptiAttr::isNormal (const FChar& ch) noexcept -> bool
{
  return ! hasAttribute(ch) && ! hasColor(ch);
}

//----------------------------------------------------------------------
void FOptiAttr::initialize()
{
  F_color.monochron = F_color.max_color < 8;
  init_reset_attribute (F_bold.off);
  init_reset_attribute (F_dim.off);
  init_reset_attribute (F_italics.off);
  init_reset_attribute (F_blink.off);
  init_reset_attribute (F_underline.off, all_tests & ~same_like_ue);
  init_reset_attribute (F_reverse.off);
  init_reset_attribute (F_secure.off);
  init_reset_attribute (F_protected.off);
  init_reset_attribute (F_crossed_out.off);
  init_reset_attribute (F_dbl_underline.off);
  init_reset_attribute (F_standout.off, all_tests & ~same_like_se);
  alt_equal_pc_charset = hasCharsetEquivalence();
}

//----------------------------------------------------------------------
auto FOptiAttr::vga2ansi (FColor color) noexcept -> FColor
{
  //   VGA   |  ANSI
  // i R G B | i B G R
  //---------+---------
  // 0 0 0 0 | 0 0 0 0    i = intensity bit
  // 0 0 0 1 | 0 1 0 0    R = red
  // 0 0 1 0 | 0 0 1 0    G = green
  // 0 0 1 1 | 0 1 1 0    B = blue
  // 0 1 0 0 | 0 0 0 1
  // 0 1 0 1 | 0 1 0 1
  // 0 1 1 0 | 0 0 1 1
  // 0 1 1 1 | 0 1 1 1
  // 1 0 0 0 | 1 0 0 0
  // 1 0 0 1 | 1 1 0 0
  // 1 0 1 0 | 1 0 1 0
  // 1 0 1 1 | 1 1 1 0
  // 1 1 0 0 | 1 0 0 1
  // 1 1 0 1 | 1 1 0 1
  // 1 1 1 0 | 1 0 1 1
  // 1 1 1 1 | 1 1 1 1

  if ( color == FColor::Default )
    color = FColor::Black;
  else if ( color < 16 )
  {
    static constexpr std::array<FColor, 16> lookup_table
    {{
      FColor(0), FColor(4),  FColor(2),  FColor(6),
      FColor(1), FColor(5),  FColor(3),  FColor(7),
      FColor(8), FColor(12), FColor(10), FColor(14),
      FColor(9), FColor(13), FColor(11), FColor(15)
    }};

    color = lookup_table[uInt16(color)];
  }

  return color;
}

//----------------------------------------------------------------------
auto FOptiAttr::changeAttribute (FChar& term, FChar& next) -> std::string
{
  const bool next_has_color = hasColor(next);
  fake_reverse = false;
  attr_buf.clear();
  prevent_no_color_video_attributes (term, next_has_color);
  prevent_no_color_video_attributes (next);
  detectSwitchOn (term, next);
  detectSwitchOff (term, next);

  // Simulate invisible characters
  if ( ! F_secure.on.cap && next.attr.data & internal::var::invisible_mask )
    next.encoded_char.unicode_data[0] = ' ';

  // Look for no changes
  if ( ! (switchOn() || switchOff() || hasColorChanged(term, next)) )
    return {};

  if ( hasNoAttribute(next) )
  {
    deactivateAttributes (term, next);
  }
  else if ( F_attributes.on.cap
         && (! (term.attr.data & internal::var::pc_charset_mask) || alt_equal_pc_charset) )
  {
    changeAttributeSGR (term, next);
  }
  else
  {
    changeAttributeSeparately (term, next);
  }

  static const auto& start_options = FStartOptions::getInstance();

  if ( start_options.sgr_optimizer )
    sgr_optimizer.optimize();

  return attr_buf;
}


// private methods of FOptiAttr
//----------------------------------------------------------------------
inline void FOptiAttr::set_mode ( Capability& capability
                                , const char cap[]
                                , bool caused_reset ) const noexcept
{
  if ( cap )
  {
    capability.cap = cap;
    capability.caused_reset = caused_reset;
  }
}

//----------------------------------------------------------------------
inline void FOptiAttr::set_mode_on ( TextStyle& style
                                   , const char cap[]
                                   , bool caused_reset ) const noexcept
{
  set_mode (style.on, cap, caused_reset);
}

//----------------------------------------------------------------------
inline void FOptiAttr::set_mode_off ( TextStyle& style
                                    , const char cap[]
                                    , bool caused_reset ) const noexcept
{
  set_mode (style.off, cap, caused_reset);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermBold (FChar& term) noexcept -> bool
{
  term.attr.data |= internal::var::bold_mask;
  return append_sequence(F_bold.on.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermBold (FChar& term) noexcept -> bool
{
  // Back to normal intensity (turns off bold + dim)

  if ( F_bold.off.caused_reset )
    reset(term);
  else
  {
    term.attr.data &= internal::var::bold_reset_mask;
    term.attr.data &= internal::var::dim_reset_mask;
  }

  return append_sequence(F_bold.off.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermDim (FChar& term) noexcept -> bool
{
  term.attr.data |= internal::var::dim_mask;
  return append_sequence(F_dim.on.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermDim (FChar& term) noexcept -> bool
{
  // Back to normal intensity (turns off bold + dim)

  if ( F_dim.off.caused_reset )
    reset(term);
  else
  {
    term.attr.data &= internal::var::bold_reset_mask;
    term.attr.data &= internal::var::dim_reset_mask;
  }

  return append_sequence(F_dim.off.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermItalic (FChar& term) noexcept -> bool
{
  term.attr.data |= internal::var::italic_mask;
  return append_sequence(F_italics.on.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermItalic (FChar& term) noexcept -> bool
{
  if ( F_italics.off.caused_reset )
    reset(term);
  else
    term.attr.data &= internal::var::italic_reset_mask;

  return append_sequence(F_italics.off.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermUnderline (FChar& term) noexcept -> bool
{
  term.attr.data |= internal::var::underline_mask;
  return append_sequence(F_underline.on.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermUnderline (FChar& term) noexcept -> bool
{
  // Turns off every underlining

  if ( F_underline.off.caused_reset )
    reset(term);
  else
  {
    term.attr.data &= internal::var::underline_reset_mask;
    term.attr.data &= internal::var::dbl_underline_reset_mask;
  }

  return append_sequence(F_underline.off.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermBlink (FChar& term) noexcept -> bool
{
  term.attr.data |= internal::var::blink_mask;
  return append_sequence(F_blink.on.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermBlink (FChar& term) noexcept -> bool
{
  if ( F_blink.off.caused_reset )
    reset(term);
  else
    term.attr.data &= internal::var::blink_reset_mask;

  return append_sequence(F_blink.off.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermReverse (FChar& term) noexcept -> bool
{
  term.attr.data |= internal::var::reverse_mask;
  return ( ! fake_reverse && append_sequence(F_reverse.on.cap) );
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermReverse (FChar& term) noexcept -> bool
{
  if ( F_reverse.off.caused_reset )
    reset(term);
  else
    term.attr.data &= internal::var::reverse_reset_mask;

  return ( ! fake_reverse && append_sequence(F_reverse.off.cap) );
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermStandout (FChar& term) noexcept -> bool
{
  term.attr.data |= internal::var::standout_mask;
  return ( ! fake_reverse && append_sequence(F_standout.on.cap) );
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermStandout (FChar& term) noexcept -> bool
{
  if ( F_standout.off.caused_reset )
    reset(term);
  else
    term.attr.data &= internal::var::standout_reset_mask;

  return ( ! fake_reverse && append_sequence(F_standout.off.cap) );
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermInvisible (FChar& term) noexcept -> bool
{
  term.attr.data |= internal::var::invisible_mask;
  return append_sequence(F_secure.on.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermInvisible (FChar& term) noexcept -> bool
{
  if ( F_secure.off.caused_reset )
    reset(term);
  else
    term.attr.data &= internal::var::invisible_reset_mask;

  return append_sequence(F_secure.off.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermProtected (FChar& term) noexcept -> bool
{
  term.attr.data |= internal::var::protect_mask;
  return append_sequence(F_protected.on.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermProtected (FChar& term) noexcept -> bool
{
  if ( F_protected.off.caused_reset )
    reset(term);
  else
    term.attr.data &= internal::var::protect_reset_mask;

  return append_sequence(F_protected.off.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermCrossedOut (FChar& term) noexcept -> bool
{
  term.attr.data |= internal::var::crossed_out_mask;
  return append_sequence(F_crossed_out.on.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermCrossedOut (FChar& term) noexcept -> bool
{
  if ( F_crossed_out.off.caused_reset )
    reset(term);
  else
    term.attr.data &= internal::var::crossed_out_reset_mask;

  return append_sequence(F_crossed_out.off.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermDoubleUnderline (FChar& term) noexcept -> bool
{
  term.attr.data |= internal::var::dbl_underline_mask;
  return append_sequence(F_dbl_underline.on.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermDoubleUnderline (FChar& term) noexcept -> bool
{
  // Turns off every underlining

  if ( F_dbl_underline.off.caused_reset )
    reset(term);
  else
  {
    term.attr.data &= internal::var::underline_reset_mask;
    term.attr.data &= internal::var::dbl_underline_reset_mask;
  }

  return append_sequence(F_dbl_underline.off.cap);
}

//----------------------------------------------------------------------
auto FOptiAttr::setTermAttributes (FChar& term, const TCapAttributes& attr) -> bool
{
  if ( F_attributes.on.cap )
  {
    const auto sgr = FTermcap::encodeParameter ( F_attributes.on.cap
                                               , attr.p1 && ! fake_reverse
                                               , attr.p2
                                               , attr.p3 && ! fake_reverse
                                               , attr.p4
                                               , attr.p5
                                               , attr.p6
                                               , attr.p7
                                               , attr.p8
                                               , attr.p9 );
    append_sequence (sgr);
    resetColor(term);
    term.attr.bit.standout      = attr.p1;
    term.attr.bit.underline     = attr.p2;
    term.attr.bit.reverse       = attr.p3;
    term.attr.bit.blink         = attr.p4;
    term.attr.bit.dim           = attr.p5;
    term.attr.bit.bold          = attr.p6;
    term.attr.bit.invisible     = attr.p7;
    term.attr.bit.protect       = attr.p8;
    term.attr.bit.alt_charset   = attr.p9;
    term.attr.bit.pc_charset    = false;
    term.attr.bit.italic        = false;
    term.attr.bit.crossed_out   = false;
    term.attr.bit.dbl_underline = false;
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermAttributes (FChar& term) noexcept -> bool
{
  reset(term);
  return append_sequence(F_attributes.off.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermAltCharset (FChar& term) noexcept -> bool
{
  term.attr.data |= internal::var::alt_charset_mask;

  if ( alt_equal_pc_charset
    && term.attr.data & internal::var::pc_charset_mask )
    return false;

  return append_sequence(F_alt_charset.on.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermAltCharset (FChar& term) noexcept -> bool
{
  term.attr.data &= internal::var::alt_charset_reset_mask;

  if ( alt_equal_pc_charset
    && term.attr.data & internal::var::pc_charset_mask )
    return false;

  return append_sequence(F_alt_charset.off.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermPCcharset (FChar& term) noexcept -> bool
{
  term.attr.data |= internal::var::pc_charset_mask;

  if ( alt_equal_pc_charset
    && term.attr.data & internal::var::alt_charset_mask )
    return false;

  return append_sequence(F_pc_charset.on.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermPCcharset (FChar& term) noexcept -> bool
{
  term.attr.data &= internal::var::pc_charset_reset_mask;

  if ( alt_equal_pc_charset
    && term.attr.data & internal::var::alt_charset_mask )
    return false;

  return append_sequence(F_pc_charset.off.cap);
}

//----------------------------------------------------------------------
auto FOptiAttr::setTermDefaultColor (FChar& term) -> bool
{
  term.color.data = internal::var::default_color_pair;

  if ( append_sequence(F_color.orig_pair.cap)
    || append_sequence(F_color.orig_colors.cap) )
    return true;

  if ( F_color.ansi_default_color )
  {
    append_sequence (internal::var::sgr_39_49);
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
void FOptiAttr::setAttributesOn (FChar& term)
{
  static const auto& attribute_on_handlers = getAttributeOnHandlers();
  setAttributes (changes.on.attr, attribute_on_handlers, term);
}

//----------------------------------------------------------------------
void FOptiAttr::setAttributesOff (FChar& term)
{
  static const auto& attribute_off_handlers = getAttributeOffHandlers();
  setAttributes (changes.off.attr, attribute_off_handlers, term);
}

//----------------------------------------------------------------------
void FOptiAttr::setAttributes ( FAttribute attribute
                              , const AttributeHandlers& attribute_handlers
                              , FChar& term )
{
  if ( ! attribute.data )
    return;

  for (const auto& handler : attribute_handlers)
  {
    const auto& mask = handler.mask;

    for (std::size_t b{0}; b < 2; b++)
    {
      if ( ! (attribute.byte[b] & mask.byte[b]) )
        continue;

      handler.function(this, term);  // Call function
      attribute.byte[b] ^= mask.byte[b];  // Clear found bit
    }

    if ( ! attribute.data )
      break;
  }
}

//----------------------------------------------------------------------
auto FOptiAttr::hasColor (const FChar& attr) noexcept -> bool
{
  return ( attr.color.pair.fg != FColor::Default
        || attr.color.pair.bg != FColor::Default );
}

//----------------------------------------------------------------------
auto FOptiAttr::hasAttribute (const FChar& attr) noexcept -> bool
{
  return attr.attr.data & internal::var::attribute_mask;
}

//----------------------------------------------------------------------
auto FOptiAttr::hasNoAttribute (const FChar& attr) noexcept -> bool
{
  return ! hasAttribute(attr);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::isItalicsUsed ( const FChar& term
                                     , const FChar& next ) const noexcept -> bool
{
  return ! (term.attr.data & internal::var::italic_mask)
      && next.attr.data & internal::var::italic_mask;
}

//----------------------------------------------------------------------
inline auto FOptiAttr::isCrossedOutUsed ( const FChar& term
                                        , const FChar& next ) const noexcept -> bool
{
  return ! (term.attr.data & internal::var::crossed_out_mask)
      && (next.attr.data & internal::var::crossed_out_mask);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::isDoubleUnderlineUsed ( const FChar& term
                                             , const FChar& next ) const noexcept -> bool
{
  return ! (term.attr.data & internal::var::dbl_underline_mask)
       && (next.attr.data & internal::var::dbl_underline_mask);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::isPCcharsetUsed ( const FChar& term
                                       , const FChar& next ) const noexcept -> bool
{
  return ! (term.attr.data & internal::var::pc_charset_mask)
      && next.attr.data & internal::var::pc_charset_mask;
}

//----------------------------------------------------------------------
inline auto FOptiAttr::isPCcharsetUsable ( FChar& term
                                         , const FChar& next ) noexcept -> bool
{
  if ( alt_equal_pc_charset
    && F_pc_charset.on.cap
    && next.attr.data & internal::var::alt_charset_mask )
  {
    term.attr.bit.pc_charset = next.attr.bit.pc_charset;
    changes.off.attr.data &= internal::var::pc_charset_reset_mask;
    return false;
  }

  return true;
}

//----------------------------------------------------------------------
inline auto FOptiAttr::hasColorChanged ( const FChar& term
                                       , const FChar& next ) const noexcept -> bool
{
  const auto& fake_reverse_mask = internal::var::fake_reverse_mask;
  return term.color.data != next.color.data
      || ( fake_reverse
        && ( (changes.on.attr.data & fake_reverse_mask)
          || (changes.off.attr.data & fake_reverse_mask) ) );
}

//----------------------------------------------------------------------
inline void FOptiAttr::resetColor (FChar& attr) const noexcept
{
  attr.color.data = internal::var::default_color_pair;
}

//----------------------------------------------------------------------
inline void FOptiAttr::prevent_no_color_video_attributes ( FChar& attr
                                                         , bool next_has_color )
{
  // Ignore video attributes which can not combined with a color

  if ( ! (hasColor(attr) || next_has_color) || F_color.attr_without_color <= 0 )
    return;

  static const auto& no_color_video_handlers = getNoColorVideoHandlerTable();
  auto set_bits = uInt(F_color.attr_without_color);

  while ( set_bits )
  {
    const uInt bit = set_bits & (~set_bits + 1);  // Get rightmost set bit
    set_bits &= ~bit;  // Clear rightmost set bit

    if ( ! bit )
      continue;

    const auto mode_index = unsigned(ffs(int(bit)));
    const auto& handler = no_color_video_handlers[mode_index];

    if ( handler )
      handler(this, attr);
  }
}

//----------------------------------------------------------------------
inline void FOptiAttr::deactivateAttributes (FChar& term, FChar& next)
{
  if ( hasAttribute(term) )
  {
    if ( F_attributes.off.cap )
    {
      if ( changes.off.attr.data & internal::var::alt_charset_mask )  // Required for rxvt terminals
        unsetTermAltCharset(term);

      unsetTermAttributes(term);

      if ( changes.off.attr.data & internal::var::pc_charset_mask )
        unsetTermPCcharset(term);
    }
    else
      setAttributesOff(term);
  }

  if ( hasColorChanged(term, next) )
    change_color (term, next);
}

//----------------------------------------------------------------------
inline void FOptiAttr::changeAttributeSGR (FChar& term, FChar& next)
{
  if ( switchOn() || switchOff() )
    setTermAttributes ( term
                      , { next.attr.bit.standout
                        , next.attr.bit.underline
                        , next.attr.bit.reverse
                        , next.attr.bit.blink
                        , next.attr.bit.dim
                        , next.attr.bit.bold
                        , next.attr.bit.invisible
                        , next.attr.bit.protect
                        , next.attr.bit.alt_charset } );

  const auto pc_charset_usable = isPCcharsetUsable(term, next);

  if ( changes.off.attr.data & internal::var::pc_charset_mask )
    unsetTermPCcharset(term);

  if ( isItalicsUsed(term, next) )
    setTermItalic(term);

  if ( isCrossedOutUsed(term, next) )
    setTermCrossedOut(term);

  if ( isDoubleUnderlineUsed(term, next) )
    setTermDoubleUnderline(term);

  if ( isPCcharsetUsed(term, next) && pc_charset_usable )
    setTermPCcharset(term);

  if ( hasColorChanged(term, next) )
    change_color(term, next);
}

//----------------------------------------------------------------------
inline void FOptiAttr::changeAttributeSeparately (FChar& term, FChar& next)
{
  setAttributesOff(term);

  if ( hasColorChanged(term, next) )
    change_color (term, next);

  detectSwitchOn (term, next);  // After reset all attributes
  setAttributesOn(term);
}

//----------------------------------------------------------------------
void FOptiAttr::change_color (FChar& term, FChar& next)
{
  if ( F_color.monochron )
  {
    next.color.data = internal::var::default_color_pair;
    return;
  }

  normalizeColor (next.color.pair.fg);
  normalizeColor (next.color.pair.bg);
  auto fg = next.color.pair.fg;
  auto bg = next.color.pair.bg;
  handleDefaultColors (term, next, fg, bg);

  if ( fake_reverse )
  {
    if ( fg == FColor::Default && bg == FColor::Default )
      return;

    if ( next.attr.data & internal::var::fake_reverse_mask )
    {
      std::swap (fg, bg);
      handleDefaultColors (term, next, fg, bg);
    }
  }

  change_current_color (term, fg, bg);
  term.color.data = next.color.data;
}

//----------------------------------------------------------------------
inline void FOptiAttr::normalizeColor (FColor& color) const noexcept
{
  if ( color != FColor::Default )
    color %= uInt16(F_color.max_color);
}

//----------------------------------------------------------------------
inline void FOptiAttr::handleDefaultColors ( FChar& term, FChar& next
                                           , FColor& fg, FColor& bg )
{
  if ( fg == FColor::Default || bg == FColor::Default )
    change_to_default_color(term, next, fg, bg);
}

//----------------------------------------------------------------------
inline void FOptiAttr::change_to_default_color ( FChar& term, FChar& next
                                               , FColor& fg, FColor& bg )
{
  if ( F_color.ansi_default_color )
  {
    const auto set_default_fg = fg == FColor::Default && term.color.pair.fg != FColor::Default;
    const auto set_default_bg = bg == FColor::Default && term.color.pair.bg != FColor::Default;

    if ( set_default_fg && set_default_bg )
      setTermDefaultColor(term);
    else if ( set_default_fg )
      setDefaultForeground(term);
    else if ( set_default_bg )
      setDefaultBackground(term);
  }
  else if ( ! setTermDefaultColor(term) )
  {
    // Fallback to gray on black
    fg = next.color.pair.fg = FColor::LightGray;
    bg = next.color.pair.bg = FColor::Black;
  }
}

//----------------------------------------------------------------------
inline void FOptiAttr::setDefaultForeground (FChar& term)
{
  append_sequence (internal::var::sgr_39);
  term.color.pair.fg = FColor::Default;
}

//----------------------------------------------------------------------
inline void FOptiAttr::setDefaultBackground (FChar& term)
{
  const char* sgr_49;
  const auto& op = F_color.orig_pair.cap;

  if ( op && std::memcmp (op, CSI "39;49;25m", 11) == 0 )
    sgr_49 = CSI "49;25m";
  else
    sgr_49 = CSI "49m";

  append_sequence (sgr_49);
  term.color.pair.bg = FColor::Default;
}

//----------------------------------------------------------------------
inline void FOptiAttr::change_current_color ( const FChar& term
                                            , const FColor fg, const FColor bg )
{
  const auto& AF = F_color.a_foreground.cap;
  const auto& AB = F_color.a_background.cap;
  const auto& Sf = F_color.foreground.cap;
  const auto& Sb = F_color.background.cap;
  const auto& sp = F_color.color_pair.cap;
  const bool frev = fake_reverse_color_change(term);
  static constexpr auto ANSI = 0;
  static constexpr auto VGA = 1;

  auto apply_color_change = [this, &term, fg, bg, frev] ( const char* fg_cap
                                                        , const char* bg_cap
                                                        , int cm ) noexcept
  {
    if ( ! fg_cap || ! bg_cap )
      return false;

    if ( has_foreground_changes(term, fg, frev) )
    {
      const auto fg_value = ( cm == VGA ) ? uInt16(vga2ansi(fg)) : uInt16(fg);
      append_sequence(FTermcap::encodeParameter(fg_cap, fg_value));
    }

    if ( has_background_changes(term, bg, frev) )
    {
      const auto bg_value = ( cm == VGA ) ? uInt16(vga2ansi(bg)) : uInt16(bg);
      append_sequence(FTermcap::encodeParameter(bg_cap, bg_value));
    }

    return true;
  };

  if ( ! apply_color_change(AF, AB, VGA)
    && ! apply_color_change(Sf, Sb, ANSI)
    && sp )
  {
    const auto fg_value = uInt16(vga2ansi(fg));
    const auto bg_value = uInt16(vga2ansi(bg));
    append_sequence (FTermcap::encodeParameter(sp, fg_value, bg_value));
  }
}

//----------------------------------------------------------------------
inline void FOptiAttr::resetAttribute (FChar& attr) const noexcept
{
  attr.attr.data &= internal::var::reset_mask;
}

//----------------------------------------------------------------------
inline void FOptiAttr::reset (FChar& attr) const noexcept
{
  resetAttribute(attr);
  resetColor(attr);
}

//----------------------------------------------------------------------
auto FOptiAttr::caused_reset_attributes (const char cap[], uChar test) const noexcept -> bool
{
  // test if "cap" reset all attributes

  if ( ! cap )
    return false;

  const auto& ue = F_underline.off.cap;
  const auto& se = F_standout.off.cap;
  const auto& me = F_attributes.off.cap;

  return ( (test & test_ansi_reset) && std::memcmp (cap, CSI "m", 3) == 0 )
      || ( (test & test_adm3_reset) && std::memcmp (cap, ESC "G0", 3) == 0 )
      || ( (test & same_like_ue) && ue && std::strcmp (cap, ue) == 0
                                 && std::memcmp (cap, CSI "24m", 5) != 0 )
      || ( (test & same_like_se) && se && std::strcmp (cap, se) == 0
                                 && std::memcmp (cap, CSI "27m", 5) != 0 )
      || ( (test & same_like_me) && me && std::strcmp (cap, me) == 0 );
}

//----------------------------------------------------------------------
void FOptiAttr::init_reset_attribute (Capability& off, uChar test) const noexcept
{
  if ( caused_reset_attributes(off.cap, test) )
    off.caused_reset = true;
}

//----------------------------------------------------------------------
inline auto FOptiAttr::fake_reverse_color_change (const FChar& term) const noexcept -> bool
{
  const auto& fake_reverse_mask = internal::var::fake_reverse_mask;
  return fake_reverse
      && ( (changes.off.attr.data & fake_reverse_mask)
        || (term.attr.data & fake_reverse_mask) );
}

//----------------------------------------------------------------------
inline auto FOptiAttr::hasCharsetEquivalence() const noexcept -> bool
{
  // Detect if alt charset and pc charset are the same sequences

  const auto& alt_on  = F_alt_charset.on.cap;
  const auto& alt_off = F_alt_charset.off.cap;
  const auto& pc_on   = F_pc_charset.on.cap;
  const auto& pc_off  = F_pc_charset.off.cap;

  return ( alt_on && pc_on && std::strcmp (alt_on, pc_on) == 0 )
      || ( alt_off && pc_off && std::strcmp (alt_off, pc_off) == 0 );
}

//----------------------------------------------------------------------
auto FOptiAttr::getNoColorVideoHandlerTable() -> const NoColorVideoHandlerTable&
{
  static const auto& no_color_video_handlers = std::make_unique<NoColorVideoHandlerTable>
  (
    NoColorVideoHandlerTable
    {{
      nullptr,                                                                      // No bit set (0)
      [] (const FOptiAttr*, FChar& fchar) { fchar.attr.bit.standout = false; },     // Standout mode (1)
      [] (const FOptiAttr*, FChar& fchar) { fchar.attr.bit.underline = false; },    // Underline mode (2)
      [] (FOptiAttr* obj, const FChar&)   { obj->fake_reverse = true; },            // Reverse mode (4)
      [] (const FOptiAttr*, FChar& fchar) { fchar.attr.bit.blink = false; },        // Blink mode (8)
      [] (const FOptiAttr*, FChar& fchar) { fchar.attr.bit.dim = false; },          // Dim mode (16)
      [] (const FOptiAttr*, FChar& fchar) { fchar.attr.bit.bold = false; },         // Bold mode (32)
      [] (const FOptiAttr*, FChar& fchar) { fchar.attr.bit.invisible = false; },    // Invisible mode (64)
      [] (const FOptiAttr*, FChar& fchar) { fchar.attr.bit.protect = false; },      // Protected mode (128)
      [] (const FOptiAttr*, FChar& fchar) { fchar.attr.bit.alt_charset = false; },  // Alt_charset mode (256)
      nullptr,                                                                      // Horizontal mode (512)
      nullptr,                                                                      // Left mode (1024)
      nullptr,                                                                      // Low mode (2048)
      nullptr,                                                                      // Right mode (4096)
      nullptr,                                                                      // Top mode (8192)
      nullptr,                                                                      // Vertical mode (1638)
      [] (const FOptiAttr*, FChar& fchar) { fchar.attr.bit.italic = false; },       // Italic mode (32768)
      nullptr                                                                       // No mode (65536)
    }}
  );

  return *no_color_video_handlers;
}

//----------------------------------------------------------------------
auto FOptiAttr::getAttributeOnHandlers() -> const AttributeHandlers&
{
  static const auto& attribute_on_handlers = std::make_unique<AttributeHandlers>
  (
    AttributeHandlers
    {{
      { {{0x00, 0x08, 0x00, 0x00}}, [] (FOptiAttr* obj, FChar& fchar) { return obj->setTermAltCharset(fchar); } },
      { {{0x00, 0x10, 0x00, 0x00}}, [] (FOptiAttr* obj, FChar& fchar) { return obj->setTermPCcharset(fchar); } },
      { {{0x01, 0x00, 0x00, 0x00}}, [] (FOptiAttr* obj, FChar& fchar) { return obj->setTermBold(fchar); } },
      { {{0x02, 0x00, 0x00, 0x00}}, [] (FOptiAttr* obj, FChar& fchar) { return obj->setTermDim(fchar); } },
      { {{0x04, 0x00, 0x00, 0x00}}, [] (FOptiAttr* obj, FChar& fchar) { return obj->setTermItalic(fchar); } },
      { {{0x08, 0x00, 0x00, 0x00}}, [] (FOptiAttr* obj, FChar& fchar) { return obj->setTermUnderline(fchar); } },
      { {{0x10, 0x00, 0x00, 0x00}}, [] (FOptiAttr* obj, FChar& fchar) { return obj->setTermBlink(fchar); } },
      { {{0x20, 0x00, 0x00, 0x00}}, [] (FOptiAttr* obj, FChar& fchar) { return obj->setTermReverse(fchar); } },
      { {{0x40, 0x00, 0x00, 0x00}}, [] (FOptiAttr* obj, FChar& fchar) { return obj->setTermStandout(fchar); } },
      { {{0x80, 0x00, 0x00, 0x00}}, [] (FOptiAttr* obj, FChar& fchar) { return obj->setTermInvisible(fchar); } },
      { {{0x00, 0x01, 0x00, 0x00}}, [] (FOptiAttr* obj, FChar& fchar) { return obj->setTermProtected(fchar); } },
      { {{0x00, 0x02, 0x00, 0x00}}, [] (FOptiAttr* obj, FChar& fchar) { return obj->setTermCrossedOut(fchar); } },
      { {{0x00, 0x04, 0x00, 0x00}}, [] (FOptiAttr* obj, FChar& fchar) { return obj->setTermDoubleUnderline(fchar); } }
    }}
  );

  return *attribute_on_handlers;
}

//----------------------------------------------------------------------
auto FOptiAttr::getAttributeOffHandlers() -> const AttributeHandlers&
{
  static const auto& attribute_off_handlers = std::make_unique<AttributeHandlers>
  (
    AttributeHandlers
    {{
      { {{0x00, 0x10, 0x00, 0x00}}, [] (FOptiAttr* obj, FChar& fchar) { return obj->unsetTermPCcharset(fchar); } },
      { {{0x00, 0x08, 0x00, 0x00}}, [] (FOptiAttr* obj, FChar& fchar) { return obj->unsetTermAltCharset(fchar); } },
      { {{0x01, 0x00, 0x00, 0x00}}, [] (FOptiAttr* obj, FChar& fchar) { return obj->unsetTermBold(fchar); } },
      { {{0x02, 0x00, 0x00, 0x00}}, [] (FOptiAttr* obj, FChar& fchar) { return obj->unsetTermDim(fchar); } },
      { {{0x04, 0x00, 0x00, 0x00}}, [] (FOptiAttr* obj, FChar& fchar) { return obj->unsetTermItalic(fchar); } },
      { {{0x08, 0x00, 0x00, 0x00}}, [] (FOptiAttr* obj, FChar& fchar) { return obj->unsetTermUnderline(fchar); } },
      { {{0x10, 0x00, 0x00, 0x00}}, [] (FOptiAttr* obj, FChar& fchar) { return obj->unsetTermBlink(fchar); } },
      { {{0x20, 0x00, 0x00, 0x00}}, [] (FOptiAttr* obj, FChar& fchar) { return obj->unsetTermReverse(fchar); } },
      { {{0x40, 0x00, 0x00, 0x00}}, [] (FOptiAttr* obj, FChar& fchar) { return obj->unsetTermStandout(fchar); } },
      { {{0x80, 0x00, 0x00, 0x00}}, [] (FOptiAttr* obj, FChar& fchar) { return obj->unsetTermInvisible(fchar); } },
      { {{0x00, 0x01, 0x00, 0x00}}, [] (FOptiAttr* obj, FChar& fchar) { return obj->unsetTermProtected(fchar); } },
      { {{0x00, 0x02, 0x00, 0x00}}, [] (FOptiAttr* obj, FChar& fchar) { return obj->unsetTermCrossedOut(fchar); } },
      { {{0x00, 0x04, 0x00, 0x00}}, [] (FOptiAttr* obj, FChar& fchar) { return obj->unsetTermDoubleUnderline(fchar); } }
    }}
  );

  return *attribute_off_handlers;
}

//----------------------------------------------------------------------
inline void FOptiAttr::detectSwitchOn (const FChar& term, const FChar& next) noexcept
{
  // Detect switched on attributes on transition from "term" to "next"
  // and store the result in "on"

  const auto& mask = internal::var::attribute_mask;
  changes.on.attr.data = ~(term.attr.data) & next.attr.data & mask;
}

//----------------------------------------------------------------------
inline void FOptiAttr::detectSwitchOff (const FChar& term, const FChar& next) noexcept
{
  // Detect switched off attributes on transition from "term" to "next"
  // and store the result in "on"

  const auto& mask = internal::var::attribute_mask;
  changes.off.attr.data = term.attr.data & ~(next.attr.data) & mask;
}

//----------------------------------------------------------------------
inline auto FOptiAttr::switchOn() const noexcept -> bool
{
  return hasAttribute(changes.on);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::switchOff() const noexcept -> bool
{
  return hasAttribute(changes.off);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::append_sequence (const std::string& seq) noexcept -> bool
{
  if ( seq.empty() )
    return false;

  attr_buf.append(seq);
  return true;
}


// non-member functions
//----------------------------------------------------------------------
inline auto has_foreground_changes (const FChar& term, FColor fg, bool frev) noexcept -> bool
{
  return term.color.pair.fg != fg || frev;
}

//----------------------------------------------------------------------
inline auto has_background_changes (const FChar& term, FColor bg, bool frev) noexcept -> bool
{
  return term.color.pair.bg != bg || frev;
}

}  // namespace finalcut
