/***********************************************************************
* foptiattr.cpp - Sets video attributes in optimized order             *
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

struct var
{
  static uInt8 b0_reverse_mask;
  static uInt8 b1_mask;
  static uInt8 b1_reset_mask;
  static uInt8 b2_reset_mask;
};

uInt8 var::b0_reverse_mask{};
uInt8 var::b1_mask{};
uInt8 var::b1_reset_mask{};
uInt8 var::b2_reset_mask{};

}  // namespace internal


//----------------------------------------------------------------------
// class FOptiAttr
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FOptiAttr::FOptiAttr()
{
  attr_buf.reserve(SGRoptimizer::ATTR_BUF_SIZE);
  internal::var::b0_reverse_mask = getByte0ReverseMask();
  internal::var::b1_mask = getByte1Mask();
  internal::var::b1_reset_mask = getByte1ResetMask();
  internal::var::b2_reset_mask = getByte2ResetMask();
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

  set_enter_bold_mode (term_env.t_enter_bold_mode);
  set_exit_bold_mode (term_env.t_exit_bold_mode);
  set_enter_dim_mode (term_env.t_enter_dim_mode);
  set_exit_dim_mode (term_env.t_exit_dim_mode);
  set_enter_italics_mode (term_env.t_enter_italics_mode);
  set_exit_italics_mode (term_env.t_exit_italics_mode);
  set_enter_underline_mode (term_env.t_enter_underline_mode);
  set_exit_underline_mode (term_env.t_exit_underline_mode);
  set_enter_blink_mode (term_env.t_enter_blink_mode);
  set_exit_blink_mode (term_env.t_exit_blink_mode);
  set_enter_reverse_mode (term_env.t_enter_reverse_mode);
  set_exit_reverse_mode (term_env.t_exit_reverse_mode);
  set_enter_standout_mode (term_env.t_enter_standout_mode);
  set_exit_standout_mode (term_env.t_exit_standout_mode);
  set_enter_secure_mode (term_env.t_enter_secure_mode);
  set_exit_secure_mode (term_env.t_exit_secure_mode);
  set_enter_protected_mode (term_env.t_enter_protected_mode);
  set_exit_protected_mode (term_env.t_exit_protected_mode);
  set_enter_crossed_out_mode (term_env.t_enter_crossed_out_mode);
  set_exit_crossed_out_mode (term_env.t_exit_crossed_out_mode);
  set_enter_dbl_underline_mode (term_env.t_enter_dbl_underline_mode);
  set_exit_dbl_underline_mode (term_env.t_exit_dbl_underline_mode);
  set_set_attributes (term_env.t_set_attributes);
  set_exit_attribute_mode (term_env.t_exit_attribute_mode);
  set_enter_alt_charset_mode (term_env.t_enter_alt_charset_mode);
  set_exit_alt_charset_mode (term_env.t_exit_alt_charset_mode);
  set_enter_pc_charset_mode (term_env.t_enter_pc_charset_mode);
  set_exit_pc_charset_mode (term_env.t_exit_pc_charset_mode);
  set_a_foreground_color (term_env.t_set_a_foreground);
  set_a_background_color (term_env.t_set_a_background);
  set_foreground_color (term_env.t_set_foreground);
  set_background_color (term_env.t_set_background);
  set_term_color_pair (term_env.t_set_color_pair);
  set_orig_pair (term_env.t_orig_pair);
  set_orig_orig_colors (term_env.t_orig_colors);

  max_color = term_env.max_color;
  attr_without_color = term_env.attr_without_color;
  ansi_default_color = term_env.ansi_default_color;

  initialize();
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_bold_mode (const char cap[])
{
  if ( cap )
  {
    F_enter_bold_mode.cap = cap;
    F_enter_bold_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_bold_mode (const char cap[])
{
  if ( cap )
  {
    F_exit_bold_mode.cap = cap;
    F_exit_bold_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_dim_mode (const char cap[])
{
  if ( cap )
  {
    F_enter_dim_mode.cap = cap;
    F_enter_dim_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_dim_mode (const char cap[])
{
  if ( cap )
  {
    F_exit_dim_mode.cap = cap;
    F_exit_dim_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_italics_mode (const char cap[])
{
  if ( cap )
  {
    F_enter_italics_mode.cap = cap;
    F_enter_italics_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_italics_mode (const char cap[])
{
  if ( cap )
  {
    F_exit_italics_mode.cap = cap;
    F_exit_italics_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_underline_mode (const char cap[])
{
  if ( cap )
  {
    F_enter_underline_mode.cap = cap;
    F_enter_underline_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_underline_mode (const char cap[])
{
  if ( cap )
  {
    F_exit_underline_mode.cap = cap;
    F_exit_underline_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_blink_mode (const char cap[])
{
  if ( cap )
  {
    F_enter_blink_mode.cap = cap;
    F_enter_blink_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_blink_mode (const char cap[])
{
  if ( cap )
  {
    F_exit_blink_mode.cap = cap;
    F_exit_blink_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_reverse_mode (const char cap[])
{
  if ( cap )
  {
    F_enter_reverse_mode.cap = cap;
    F_enter_reverse_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_reverse_mode (const char cap[])
{
  if ( cap )
  {
    F_exit_reverse_mode.cap = cap;
    F_exit_reverse_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_secure_mode (const char cap[])
{
  if ( cap )
  {
    F_enter_secure_mode.cap = cap;
    F_enter_secure_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_secure_mode (const char cap[])
{
  if ( cap )
  {
    F_exit_secure_mode.cap = cap;
    F_exit_secure_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_protected_mode (const char cap[])
{
  if ( cap )
  {
    F_enter_protected_mode.cap = cap;
    F_enter_protected_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_protected_mode (const char cap[])
{
  if ( cap )
  {
    F_exit_protected_mode.cap = cap;
    F_exit_protected_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_crossed_out_mode (const char cap[])
{
  if ( cap )
  {
    F_enter_crossed_out_mode.cap = cap;
    F_enter_crossed_out_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_crossed_out_mode (const char cap[])
{
  if ( cap )
  {
    F_exit_crossed_out_mode.cap = cap;
    F_exit_crossed_out_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_dbl_underline_mode (const char cap[])
{
  if ( cap )
  {
    F_enter_dbl_underline_mode.cap = cap;
    F_enter_dbl_underline_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_dbl_underline_mode (const char cap[])
{
  if ( cap )
  {
    F_exit_dbl_underline_mode.cap = cap;
    F_exit_dbl_underline_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_standout_mode (const char cap[])
{
  if ( cap )
  {
    F_enter_standout_mode.cap = cap;
    F_enter_standout_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_standout_mode (const char cap[])
{
  if ( cap )
  {
    F_exit_standout_mode.cap = cap;
    F_exit_standout_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_set_attributes (const char cap[])
{
  if ( cap )
  {
    F_set_attributes.cap = cap;
    F_set_attributes.caused_reset = true;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_attribute_mode (const char cap[])
{
  if ( cap )
  {
    F_exit_attribute_mode.cap = cap;
    F_exit_attribute_mode.caused_reset = true;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_alt_charset_mode (const char cap[])
{
  if ( cap )
  {
    F_enter_alt_charset_mode.cap = cap;
    F_enter_alt_charset_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_alt_charset_mode (const char cap[])
{
  if ( cap )
  {
    F_exit_alt_charset_mode.cap = cap;
    F_exit_alt_charset_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_pc_charset_mode (const char cap[])
{
  if ( cap )
  {
    F_enter_pc_charset_mode.cap = cap;
    F_enter_pc_charset_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_pc_charset_mode (const char cap[])
{
  if ( cap )
  {
    F_exit_pc_charset_mode.cap = cap;
    F_exit_pc_charset_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_a_foreground_color (const char cap[])
{
  if ( cap )
  {
    F_set_a_foreground.cap = cap;
    F_set_a_foreground.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_a_background_color (const char cap[])
{
  if ( cap )
  {
    F_set_a_background.cap = cap;
    F_set_a_background.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_foreground_color (const char cap[])
{
  if ( cap )
  {
    F_set_foreground.cap = cap;
    F_set_foreground.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_background_color (const char cap[])
{
  if ( cap )
  {
    F_set_background.cap = cap;
    F_set_background.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_term_color_pair (const char cap[])
{
  if ( cap )
  {
    F_set_color_pair.cap = cap;
    F_set_color_pair.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_orig_pair (const char cap[])
{
  if ( cap )
  {
    F_orig_pair.cap = cap;
    F_orig_pair.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_orig_orig_colors (const char cap[])
{
  if ( cap )
  {
    F_orig_colors.cap = cap;
    F_orig_colors.caused_reset = false;
  }
}

//----------------------------------------------------------------------
auto FOptiAttr::isNormal (const FChar& ch) -> bool
{
  return hasNoAttribute(ch) && ! hasColor(ch);
}

//----------------------------------------------------------------------
void FOptiAttr::initialize()
{
  monochron = max_color < 8;

  if ( caused_reset_attributes(F_exit_bold_mode.cap) )
    F_exit_bold_mode.caused_reset = true;

  if ( caused_reset_attributes(F_exit_dim_mode.cap) )
    F_exit_dim_mode.caused_reset = true;

  if ( caused_reset_attributes(F_exit_italics_mode.cap) )
    F_exit_italics_mode.caused_reset = true;

  if ( caused_reset_attributes(F_exit_blink_mode.cap) )
    F_exit_blink_mode.caused_reset = true;

  if ( caused_reset_attributes ( F_exit_underline_mode.cap
                               , all_tests & ~same_like_ue) )
    F_exit_underline_mode.caused_reset = true;

  if ( caused_reset_attributes(F_exit_reverse_mode.cap) )
    F_exit_reverse_mode.caused_reset = true;

  if ( caused_reset_attributes(F_exit_secure_mode.cap) )
    F_exit_secure_mode.caused_reset = true;

  if ( caused_reset_attributes(F_exit_protected_mode.cap) )
    F_exit_protected_mode.caused_reset = true;

  if ( caused_reset_attributes(F_exit_crossed_out_mode.cap) )
    F_exit_crossed_out_mode.caused_reset = true;

  if ( caused_reset_attributes(F_exit_dbl_underline_mode.cap) )
    F_exit_dbl_underline_mode.caused_reset = true;

  if ( caused_reset_attributes ( F_exit_standout_mode.cap
                               , all_tests & ~same_like_se) )
    F_exit_standout_mode.caused_reset = true;

  if ( hasCharsetEquivalence() )
    alt_equal_pc_charset = true;
}

//----------------------------------------------------------------------
auto FOptiAttr::vga2ansi (FColor color) -> FColor
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
  if ( ! F_enter_secure_mode.cap && next.attr.bit.invisible )
    next.encoded_char[0] = ' ';

  // Look for no changes
  if ( ! (switchOn() || switchOff() || hasColorChanged(term, next)) )
    return {};

  if ( hasNoAttribute(next) )
  {
    deactivateAttributes (term, next);
  }
  else if ( F_set_attributes.cap
         && (! term.attr.bit.pc_charset || alt_equal_pc_charset) )
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
inline auto FOptiAttr::setTermBold (FChar& term) -> bool
{
  term.attr.bit.bold = true;
  return append_sequence(F_enter_bold_mode.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermBold (FChar& term) -> bool
{
  // Back to normal intensity (turns off bold + dim)

  if ( F_exit_bold_mode.caused_reset )
    reset(term);
  else
  {
    term.attr.bit.bold = false;
    term.attr.bit.dim = false;
  }

  return append_sequence(F_exit_bold_mode.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermDim (FChar& term) -> bool
{
  term.attr.bit.dim = true;
  return append_sequence(F_enter_dim_mode.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermDim (FChar& term) -> bool
{
  // Back to normal intensity (turns off bold + dim)

  if ( F_exit_dim_mode.caused_reset )
    reset(term);
  else
  {
    term.attr.bit.bold = false;
    term.attr.bit.dim = false;
  }

  return append_sequence(F_exit_dim_mode.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermItalic (FChar& term) -> bool
{
  term.attr.bit.italic = true;
  return append_sequence(F_enter_italics_mode.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermItalic (FChar& term) -> bool
{
  if ( F_exit_italics_mode.caused_reset )
    reset(term);
  else
    term.attr.bit.italic = false;

  return append_sequence(F_exit_italics_mode.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermUnderline (FChar& term) -> bool
{
  term.attr.bit.underline = true;
  return append_sequence(F_enter_underline_mode.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermUnderline (FChar& term) -> bool
{
  // Turns off every underlining

  if ( F_exit_underline_mode.caused_reset )
    reset(term);
  else
  {
    term.attr.bit.underline = false;
    term.attr.bit.dbl_underline = false;
  }

  return append_sequence(F_exit_underline_mode.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermBlink (FChar& term) -> bool
{
  term.attr.bit.blink = true;
  return append_sequence(F_enter_blink_mode.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermBlink (FChar& term) -> bool
{
  if ( F_exit_blink_mode.caused_reset )
    reset(term);
  else
    term.attr.bit.blink = false;

  return append_sequence(F_exit_blink_mode.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermReverse (FChar& term) -> bool
{
  term.attr.bit.reverse = true;
  return ( ! fake_reverse && append_sequence(F_enter_reverse_mode.cap) );
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermReverse (FChar& term) -> bool
{
  if ( F_exit_reverse_mode.caused_reset )
    reset(term);
  else
    term.attr.bit.reverse = false;

  return ( ! fake_reverse && append_sequence(F_exit_reverse_mode.cap) );
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermStandout (FChar& term) -> bool
{
  term.attr.bit.standout = true;

  return ( ! fake_reverse && append_sequence(F_enter_standout_mode.cap) );
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermStandout (FChar& term) -> bool
{
  if ( F_exit_standout_mode.caused_reset )
    reset(term);
  else
    term.attr.bit.standout = false;

  return ( ! fake_reverse && append_sequence(F_exit_standout_mode.cap) );
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermInvisible (FChar& term) -> bool
{
  term.attr.bit.invisible = true;
  return append_sequence(F_enter_secure_mode.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermInvisible (FChar& term) -> bool
{
  if ( F_exit_secure_mode.caused_reset )
    reset(term);
  else
    term.attr.bit.invisible = false;

  return append_sequence(F_exit_secure_mode.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermProtected (FChar& term) -> bool
{
  term.attr.bit.protect = true;
  return append_sequence(F_enter_protected_mode.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermProtected (FChar& term) -> bool
{
  if ( F_exit_protected_mode.caused_reset )
    reset(term);
  else
    term.attr.bit.protect = false;

  return append_sequence(F_exit_protected_mode.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermCrossedOut (FChar& term) -> bool
{
  term.attr.bit.crossed_out = true;
  return append_sequence(F_enter_crossed_out_mode.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermCrossedOut (FChar& term) -> bool
{
  if ( F_exit_crossed_out_mode.caused_reset )
    reset(term);
  else
    term.attr.bit.crossed_out = false;

  return append_sequence(F_exit_crossed_out_mode.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermDoubleUnderline (FChar& term) -> bool
{
  term.attr.bit.dbl_underline = true;
  return append_sequence(F_enter_dbl_underline_mode.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermDoubleUnderline (FChar& term) -> bool
{
  // Turns off every underlining

  if ( F_exit_dbl_underline_mode.caused_reset )
    reset(term);
  else
  {
    term.attr.bit.underline = false;
    term.attr.bit.dbl_underline = false;
  }

  return append_sequence(F_exit_dbl_underline_mode.cap);
}

//----------------------------------------------------------------------
auto FOptiAttr::setTermAttributes (FChar& term, const TCapAttributes& attr) -> bool
{
  if ( F_set_attributes.cap )
  {
    const auto sgr = FTermcap::encodeParameter ( F_set_attributes.cap
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
inline auto FOptiAttr::unsetTermAttributes (FChar& term) -> bool
{
  reset(term);
  return append_sequence(F_exit_attribute_mode.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermAltCharset (FChar& term) -> bool
{
  term.attr.bit.alt_charset = true;

  if ( alt_equal_pc_charset && term.attr.bit.pc_charset )
    return false;

  return append_sequence(F_enter_alt_charset_mode.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermAltCharset (FChar& term) -> bool
{
  term.attr.bit.alt_charset = false;

  if ( alt_equal_pc_charset && term.attr.bit.pc_charset )
    return false;

  return append_sequence(F_exit_alt_charset_mode.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermPCcharset (FChar& term) -> bool
{
  term.attr.bit.pc_charset = true;

  if ( alt_equal_pc_charset && term.attr.bit.alt_charset )
    return false;

  return append_sequence(F_enter_pc_charset_mode.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermPCcharset (FChar& term) -> bool
{
  term.attr.bit.pc_charset = false;

  if ( alt_equal_pc_charset && term.attr.bit.alt_charset )
    return false;

  return append_sequence(F_exit_pc_charset_mode.cap);
}

//----------------------------------------------------------------------
auto FOptiAttr::setTermDefaultColor (FChar& term) -> bool
{
  term.fg_color = FColor::Default;
  term.bg_color = FColor::Default;

  if ( append_sequence(F_orig_pair.cap)
    || append_sequence(F_orig_colors.cap) )
    return true;

  if ( ansi_default_color )
  {
    std::string sgr_39_49{CSI "39;49m"};
    append_sequence (sgr_39_49);
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
void FOptiAttr::setAttributesOn (FChar& term)
{
  static const auto& attribute_on_handlers = getAttributeOnHandlers();
  setAttributes (on.attr, attribute_on_handlers, term);
}

//----------------------------------------------------------------------
void FOptiAttr::setAttributesOff (FChar& term)
{
  static const auto& attribute_off_handlers = getAttributeOffHandlers();
  setAttributes (off.attr, attribute_off_handlers, term);
}

//----------------------------------------------------------------------
void FOptiAttr::setAttributes ( const FAttribute& attribute
                              , const AttributeHandlers& attribute_handlers
                              , FChar& term )
{
  FAttribute attr{attribute};

  if ( ! attr.word )
    return;

  for (const auto& handler : attribute_handlers)
  {
    auto& mask = handler.mask;

    for (std::size_t b{0}; b < 2; b++)
    {
      if ( ! (attr.byte[b] & mask.byte[b]) )
        continue;

      handler.function(this, term);  // Call function
      attr.byte[b] ^= mask.byte[b];  // Clear found bit
    }

    if ( ! attr.word )
      break;
  }
}

//----------------------------------------------------------------------
auto FOptiAttr::hasColor (const FChar& attr) -> bool
{
  return ( attr.fg_color != FColor::Default
        || attr.bg_color != FColor::Default );
}

//----------------------------------------------------------------------
auto FOptiAttr::hasAttribute (const FChar& attr) -> bool
{
  return attr.attr.byte[0]
      || (attr.attr.byte[1] & internal::var::b1_mask);
}

//----------------------------------------------------------------------
auto FOptiAttr::hasNoAttribute (const FChar& attr) -> bool
{
  return ! hasAttribute(attr);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::hasColorChanged ( const FChar& term
                                       , const FChar& next ) const -> bool
{
  const auto& b0_reverse_mask = internal::var::b0_reverse_mask;
  const bool frev ( ( (on.attr.byte[0] & b0_reverse_mask)
                   || (off.attr.byte[0] & b0_reverse_mask) ) && fake_reverse );
  return frev
      || term.fg_color != next.fg_color
      || term.bg_color != next.bg_color;
}

//----------------------------------------------------------------------
inline void FOptiAttr::resetColor (FChar& attr) const
{
  attr.fg_color = FColor::Default;
  attr.bg_color = FColor::Default;
}

//----------------------------------------------------------------------
inline void FOptiAttr::prevent_no_color_video_attributes ( FChar& attr
                                                         , bool next_has_color )
{
  // Ignore attributes which can not combined with a color

  if ( ! (hasColor(attr) || next_has_color) || attr_without_color <= 0 )
    return;

  auto set_bits = uInt(attr_without_color);

  while ( set_bits )
  {
    uInt bit = set_bits & (~set_bits + 1);  // Get rightmost set bit
    set_bits &= ~bit;  // Clear rightmost set bit

    switch ( bit )
    {
      case standout_mode:
        attr.attr.bit.standout = false;
        break;

      case underline_mode:
        attr.attr.bit.underline = false;
        break;

      case reverse_mode:
        fake_reverse = true;
        break;

      case blink_mode:
        attr.attr.bit.blink = false;
        break;

      case dim_mode:
        attr.attr.bit.dim = false;
        break;

      case bold_mode:
        attr.attr.bit.bold = false;
        break;

      case invisible_mode:
        attr.attr.bit.invisible = false;
        break;

      case protected_mode:
        attr.attr.bit.protect = false;
        break;

      case alt_charset_mode:
        attr.attr.bit.alt_charset = false;
        break;

      case italic_mode:
        attr.attr.bit.italic = false;
        break;

      default:
        break;  // Unknown attribute mode
    }
  }
}

//----------------------------------------------------------------------
inline void FOptiAttr::deactivateAttributes (FChar& term, FChar& next)
{
  if ( hasAttribute(term) )
  {
    if ( F_exit_attribute_mode.cap )
    {
      if ( off.attr.bit.alt_charset )  // Required for rxvt terminals
        unsetTermAltCharset(term);

      unsetTermAttributes(term);

      if ( off.attr.bit.pc_charset )
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
  bool pc_charset_usable{true};

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

  if ( alt_equal_pc_charset
    && F_enter_pc_charset_mode.cap
    && next.attr.bit.alt_charset )
  {
    term.attr.bit.pc_charset = next.attr.bit.pc_charset;
    off.attr.bit.pc_charset = false;
    pc_charset_usable = false;
  }

  if ( off.attr.bit.pc_charset )
    unsetTermPCcharset(term);

  if ( ! term.attr.bit.italic && next.attr.bit.italic )
    setTermItalic(term);

  if ( ! term.attr.bit.crossed_out && next.attr.bit.crossed_out )
    setTermCrossedOut(term);

  if ( ! term.attr.bit.dbl_underline && next.attr.bit.dbl_underline )
    setTermDoubleUnderline(term);

  if ( ! term.attr.bit.pc_charset && next.attr.bit.pc_charset
    && pc_charset_usable )
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
  if ( monochron )
  {
    next.fg_color = FColor::Default;
    next.bg_color = FColor::Default;
    return;
  }

  if ( next.fg_color != FColor::Default )
    next.fg_color %= uInt16(max_color);

  if ( next.bg_color != FColor::Default )
    next.bg_color %= uInt16(max_color);

  FColor fg = next.fg_color;
  FColor bg = next.bg_color;

  if ( fg == FColor::Default || bg == FColor::Default )
    change_to_default_color (term, next, fg, bg);

  if ( fake_reverse && fg == FColor::Default && bg == FColor::Default )
    return;

  if ( fake_reverse
    && (next.attr.bit.reverse || next.attr.bit.standout) )
  {
    std::swap (fg, bg);

    if ( fg == FColor::Default || bg == FColor::Default )
      setTermDefaultColor(term);
  }

  change_current_color (term, fg, bg);

  term.fg_color = next.fg_color;
  term.bg_color = next.bg_color;
}

//----------------------------------------------------------------------
inline void FOptiAttr::change_to_default_color ( FChar& term, FChar& next
                                               , FColor& fg, FColor& bg )
{
  if ( ansi_default_color )
  {
    if ( fg == FColor::Default && term.fg_color != FColor::Default
      && bg == FColor::Default && term.bg_color != FColor::Default )
    {
      setTermDefaultColor(term);
    }
    else if ( fg == FColor::Default && term.fg_color != FColor::Default )
    {
      std::string sgr_39{CSI "39m"};
      append_sequence (sgr_39);
      term.fg_color = FColor::Default;
    }
    else if ( bg == FColor::Default && term.bg_color != FColor::Default )
    {
      const char* sgr_49;
      const auto& op = F_orig_pair.cap;

      if ( op && std::memcmp (op, CSI "39;49;25m", 11) == 0 )
        sgr_49 = CSI "49;25m";
      else
        sgr_49 = CSI "49m";

      append_sequence (sgr_49);
      term.bg_color = FColor::Default;
    }
  }
  else if ( ! setTermDefaultColor(term) )
  {
    // Fallback to gray on black
    fg = next.fg_color = FColor::LightGray;
    bg = next.bg_color = FColor::Black;
  }
}

//----------------------------------------------------------------------
inline void FOptiAttr::change_current_color ( const FChar& term
                                            , FColor fg, FColor bg )
{
  const auto& AF = F_set_a_foreground.cap;
  const auto& AB = F_set_a_background.cap;
  const auto& Sf = F_set_foreground.cap;
  const auto& Sb = F_set_background.cap;
  const auto& sp = F_set_color_pair.cap;
  const auto& b0_reverse_mask = internal::var::b0_reverse_mask;
  const bool frev ( ( (off.attr.byte[0] & b0_reverse_mask)
                   || (term.attr.byte[0] & b0_reverse_mask) ) && fake_reverse );

  if ( AF && AB )
  {
    const auto& ansi_fg = vga2ansi(fg);
    const auto& ansi_bg = vga2ansi(bg);

    if ( term.fg_color != fg || frev )
    {
      const auto& color_str = FTermcap::encodeParameter(AF, uInt16(ansi_fg));
      append_sequence (color_str);
    }

    if ( term.bg_color != bg || frev )
    {
      const auto& color_str = FTermcap::encodeParameter(AB, uInt16(ansi_bg));
      append_sequence (color_str);
    }
  }
  else if ( Sf && Sb )
  {
    if ( term.fg_color != fg || frev )
    {
      const auto& color_str = FTermcap::encodeParameter(Sf, uInt16(fg));
      append_sequence (color_str);
    }

    if ( term.bg_color != bg || frev )
    {
      const auto& color_str = FTermcap::encodeParameter(Sb, uInt16(bg));
      append_sequence (color_str);
    }
  }
  else if ( sp )
  {
    fg = vga2ansi(fg);
    bg = vga2ansi(bg);
    const auto& color_str = FTermcap::encodeParameter(sp, uInt16(fg), uInt16(bg));
    append_sequence (color_str);
  }
}

//----------------------------------------------------------------------
inline void FOptiAttr::resetAttribute (FChar& attr) const
{
  attr.attr.byte[0]  = 0;
  attr.attr.byte[1] &= internal::var::b1_reset_mask;
  attr.attr.byte[2] &= internal::var::b2_reset_mask;
}

//----------------------------------------------------------------------
inline void FOptiAttr::reset (FChar& attr) const
{
  resetAttribute(attr);
  resetColor(attr);
}

//----------------------------------------------------------------------
auto FOptiAttr::caused_reset_attributes (const char cap[], uChar test) const -> bool
{
  // test if "cap" reset all attributes

  if ( cap )
  {
    const auto& ue = F_exit_underline_mode.cap;
    const auto& se = F_exit_standout_mode.cap;
    const auto& me = F_exit_attribute_mode.cap;

    if ( (test & test_ansi_reset) && std::memcmp (cap, CSI "m", 3) == 0 )
      return true;

    if ( (test & test_adm3_reset) && std::memcmp (cap, ESC "G0", 3) == 0 )
      return true;

    if ( (test & same_like_ue) && ue && std::strcmp (cap, ue) == 0
       && std::memcmp (cap, CSI "24m", 5) != 0)
      return true;

    if ( (test & same_like_se) && se && std::strcmp (cap, se) == 0
       && std::memcmp (cap, CSI "27m", 5) != 0 )
      return true;

    if ( (test & same_like_me) && me && std::strcmp (cap, me) == 0 )
      return true;
  }

  return false;
}

//----------------------------------------------------------------------
inline auto FOptiAttr::hasCharsetEquivalence() const -> bool
{
  // Detect if alt charset and pc charset are the same sequences

  const auto& alt_on  = F_enter_alt_charset_mode.cap;
  const auto& alt_off = F_enter_pc_charset_mode.cap;
  const auto& pc_on   = F_enter_pc_charset_mode.cap;
  const auto& pc_off  = F_exit_pc_charset_mode.cap;

  if ( alt_on && pc_on && std::strcmp (alt_on, pc_on) == 0 )
    return true;

  if ( alt_off && pc_off && std::strcmp (alt_off, pc_off) == 0 )
    return true;

  return false;
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
auto FOptiAttr::getByte0ReverseMask() -> uInt8
{
  FAttribute mask{};
  mask.bit.reverse = true;
  mask.bit.standout = true;
  return mask.byte[0];
}

//----------------------------------------------------------------------
auto FOptiAttr::getByte1Mask() -> uInt8
{
  FAttribute mask{};
  mask.bit.protect = true;
  mask.bit.crossed_out = true;
  mask.bit.dbl_underline = true;
  mask.bit.alt_charset = true;
  mask.bit.pc_charset = true;
  return mask.byte[1];
}

//----------------------------------------------------------------------
auto FOptiAttr::getByte1ResetMask() -> uInt8
{
  // Set bits that must not be reset
  FAttribute mask{};
  mask.bit.transparent = true;
  mask.bit.color_overlay = true;
  mask.bit.inherit_background = true;
  return mask.byte[1];
}

//----------------------------------------------------------------------
auto FOptiAttr::getByte2ResetMask() -> uInt8
{
  // Set bits that must not be reset
  FAttribute mask{};
  mask.bit.no_changes = true;
  mask.bit.printed = true;
  return mask.byte[2];
}

//----------------------------------------------------------------------
inline void FOptiAttr::detectSwitchOn (const FChar& term, const FChar& next)
{
  // Detect switched on attributes on transition from "term" to "next"
  // and store the result in "on"

  const auto& mask = internal::var::b1_mask;
  on.attr.byte[0] = ~(term.attr.byte[0]) & next.attr.byte[0];
  on.attr.byte[1] = ~(term.attr.byte[1]) & next.attr.byte[1] & mask;
}

//----------------------------------------------------------------------
inline void FOptiAttr::detectSwitchOff (const FChar& term, const FChar& next)
{
  // Detect switched off attributes on transition from "term" to "next"
  // and store the result in "on"

  const auto& mask = internal::var::b1_mask;
  off.attr.byte[0] = term.attr.byte[0] & ~(next.attr.byte[0]);
  off.attr.byte[1] = term.attr.byte[1] & ~(next.attr.byte[1]) & mask;
}

//----------------------------------------------------------------------
inline auto FOptiAttr::switchOn() const -> bool
{
  return hasAttribute(on);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::switchOff() const -> bool
{
  return hasAttribute(off);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::append_sequence (const std::string& seq) -> bool
{
  if ( seq.empty() )
    return false;

  attr_buf.append(seq);
  return true;
}

}  // namespace finalcut
