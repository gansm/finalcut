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

#include <strings.h>  // need for fss()

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

// Function prototypes
bool has_foreground_changes (const FChar&, const FColor, bool);
bool has_background_changes (const FChar&, const FColor, bool);


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
void FOptiAttr::set_enter_bold_mode (const char cap[])
{
  if ( cap )
  {
    F_bold.on.cap = cap;
    F_bold.on.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_bold_mode (const char cap[])
{
  if ( cap )
  {
    F_bold.off.cap = cap;
    F_bold.off.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_dim_mode (const char cap[])
{
  if ( cap )
  {
    F_dim.on.cap = cap;
    F_dim.on.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_dim_mode (const char cap[])
{
  if ( cap )
  {
    F_dim.off.cap = cap;
    F_dim.off.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_italics_mode (const char cap[])
{
  if ( cap )
  {
    F_italics.on.cap = cap;
    F_italics.on.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_italics_mode (const char cap[])
{
  if ( cap )
  {
    F_italics.off.cap = cap;
    F_italics.off.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_underline_mode (const char cap[])
{
  if ( cap )
  {
    F_underline.on.cap = cap;
    F_underline.on.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_underline_mode (const char cap[])
{
  if ( cap )
  {
    F_underline.off.cap = cap;
    F_underline.off.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_blink_mode (const char cap[])
{
  if ( cap )
  {
    F_blink.on.cap = cap;
    F_blink.on.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_blink_mode (const char cap[])
{
  if ( cap )
  {
    F_blink.off.cap = cap;
    F_blink.off.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_reverse_mode (const char cap[])
{
  if ( cap )
  {
    F_reverse.on.cap = cap;
    F_reverse.on.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_reverse_mode (const char cap[])
{
  if ( cap )
  {
    F_reverse.off.cap = cap;
    F_reverse.off.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_secure_mode (const char cap[])
{
  if ( cap )
  {
    F_secure.on.cap = cap;
    F_secure.on.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_secure_mode (const char cap[])
{
  if ( cap )
  {
    F_secure.off.cap = cap;
    F_secure.off.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_protected_mode (const char cap[])
{
  if ( cap )
  {
    F_protected.on.cap = cap;
    F_protected.on.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_protected_mode (const char cap[])
{
  if ( cap )
  {
    F_protected.off.cap = cap;
    F_protected.off.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_crossed_out_mode (const char cap[])
{
  if ( cap )
  {
    F_crossed_out.on.cap = cap;
    F_crossed_out.on.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_crossed_out_mode (const char cap[])
{
  if ( cap )
  {
    F_crossed_out.off.cap = cap;
    F_crossed_out.off.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_dbl_underline_mode (const char cap[])
{
  if ( cap )
  {
    F_dbl_underline.on.cap = cap;
    F_dbl_underline.on.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_dbl_underline_mode (const char cap[])
{
  if ( cap )
  {
    F_dbl_underline.off.cap = cap;
    F_dbl_underline.off.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_standout_mode (const char cap[])
{
  if ( cap )
  {
    F_standout.on.cap = cap;
    F_standout.on.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_standout_mode (const char cap[])
{
  if ( cap )
  {
    F_standout.off.cap = cap;
    F_standout.off.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_set_attributes (const char cap[])
{
  if ( cap )
  {
    F_attributes.on.cap = cap;
    F_attributes.on.caused_reset = true;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_attribute_mode (const char cap[])
{
  if ( cap )
  {
    F_attributes.off.cap = cap;
    F_attributes.off.caused_reset = true;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_alt_charset_mode (const char cap[])
{
  if ( cap )
  {
    F_alt_charset.on.cap = cap;
    F_alt_charset.on.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_alt_charset_mode (const char cap[])
{
  if ( cap )
  {
    F_alt_charset.off.cap = cap;
    F_alt_charset.off.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_pc_charset_mode (const char cap[])
{
  if ( cap )
  {
    F_pc_charset.on.cap = cap;
    F_pc_charset.on.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_pc_charset_mode (const char cap[])
{
  if ( cap )
  {
    F_pc_charset.off.cap = cap;
    F_pc_charset.off.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_a_foreground_color (const char cap[])
{
  if ( cap )
  {
    F_color.a_foreground.cap = cap;
    F_color.a_foreground.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_a_background_color (const char cap[])
{
  if ( cap )
  {
    F_color.a_background.cap = cap;
    F_color.a_background.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_foreground_color (const char cap[])
{
  if ( cap )
  {
    F_color.foreground.cap = cap;
    F_color.foreground.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_background_color (const char cap[])
{
  if ( cap )
  {
    F_color.background.cap = cap;
    F_color.background.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_term_color_pair (const char cap[])
{
  if ( cap )
  {
    F_color.color_pair.cap = cap;
    F_color.color_pair.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_orig_pair (const char cap[])
{
  if ( cap )
  {
    F_color.orig_pair.cap = cap;
    F_color.orig_pair.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_orig_colors (const char cap[])
{
  if ( cap )
  {
    F_color.orig_colors.cap = cap;
    F_color.orig_colors.caused_reset = false;
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
  if ( ! F_secure.on.cap && next.attr.bit.invisible )
    next.encoded_char[0] = ' ';

  // Look for no changes
  if ( ! (switchOn() || switchOff() || hasColorChanged(term, next)) )
    return {};

  if ( hasNoAttribute(next) )
  {
    deactivateAttributes (term, next);
  }
  else if ( F_attributes.on.cap
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
  return append_sequence(F_bold.on.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermBold (FChar& term) -> bool
{
  // Back to normal intensity (turns off bold + dim)

  if ( F_bold.off.caused_reset )
    reset(term);
  else
  {
    term.attr.bit.bold = false;
    term.attr.bit.dim = false;
  }

  return append_sequence(F_bold.off.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermDim (FChar& term) -> bool
{
  term.attr.bit.dim = true;
  return append_sequence(F_dim.on.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermDim (FChar& term) -> bool
{
  // Back to normal intensity (turns off bold + dim)

  if ( F_dim.off.caused_reset )
    reset(term);
  else
  {
    term.attr.bit.bold = false;
    term.attr.bit.dim = false;
  }

  return append_sequence(F_dim.off.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermItalic (FChar& term) -> bool
{
  term.attr.bit.italic = true;
  return append_sequence(F_italics.on.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermItalic (FChar& term) -> bool
{
  if ( F_italics.off.caused_reset )
    reset(term);
  else
    term.attr.bit.italic = false;

  return append_sequence(F_italics.off.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermUnderline (FChar& term) -> bool
{
  term.attr.bit.underline = true;
  return append_sequence(F_underline.on.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermUnderline (FChar& term) -> bool
{
  // Turns off every underlining

  if ( F_underline.off.caused_reset )
    reset(term);
  else
  {
    term.attr.bit.underline = false;
    term.attr.bit.dbl_underline = false;
  }

  return append_sequence(F_underline.off.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermBlink (FChar& term) -> bool
{
  term.attr.bit.blink = true;
  return append_sequence(F_blink.on.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermBlink (FChar& term) -> bool
{
  if ( F_blink.off.caused_reset )
    reset(term);
  else
    term.attr.bit.blink = false;

  return append_sequence(F_blink.off.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermReverse (FChar& term) -> bool
{
  term.attr.bit.reverse = true;
  return ( ! fake_reverse && append_sequence(F_reverse.on.cap) );
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermReverse (FChar& term) -> bool
{
  if ( F_reverse.off.caused_reset )
    reset(term);
  else
    term.attr.bit.reverse = false;

  return ( ! fake_reverse && append_sequence(F_reverse.off.cap) );
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermStandout (FChar& term) -> bool
{
  term.attr.bit.standout = true;

  return ( ! fake_reverse && append_sequence(F_standout.on.cap) );
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermStandout (FChar& term) -> bool
{
  if ( F_standout.off.caused_reset )
    reset(term);
  else
    term.attr.bit.standout = false;

  return ( ! fake_reverse && append_sequence(F_standout.off.cap) );
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermInvisible (FChar& term) -> bool
{
  term.attr.bit.invisible = true;
  return append_sequence(F_secure.on.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermInvisible (FChar& term) -> bool
{
  if ( F_secure.off.caused_reset )
    reset(term);
  else
    term.attr.bit.invisible = false;

  return append_sequence(F_secure.off.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermProtected (FChar& term) -> bool
{
  term.attr.bit.protect = true;
  return append_sequence(F_protected.on.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermProtected (FChar& term) -> bool
{
  if ( F_protected.off.caused_reset )
    reset(term);
  else
    term.attr.bit.protect = false;

  return append_sequence(F_protected.off.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermCrossedOut (FChar& term) -> bool
{
  term.attr.bit.crossed_out = true;
  return append_sequence(F_crossed_out.on.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermCrossedOut (FChar& term) -> bool
{
  if ( F_crossed_out.off.caused_reset )
    reset(term);
  else
    term.attr.bit.crossed_out = false;

  return append_sequence(F_crossed_out.off.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermDoubleUnderline (FChar& term) -> bool
{
  term.attr.bit.dbl_underline = true;
  return append_sequence(F_dbl_underline.on.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermDoubleUnderline (FChar& term) -> bool
{
  // Turns off every underlining

  if ( F_dbl_underline.off.caused_reset )
    reset(term);
  else
  {
    term.attr.bit.underline = false;
    term.attr.bit.dbl_underline = false;
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
inline auto FOptiAttr::unsetTermAttributes (FChar& term) -> bool
{
  reset(term);
  return append_sequence(F_attributes.off.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermAltCharset (FChar& term) -> bool
{
  term.attr.bit.alt_charset = true;

  if ( alt_equal_pc_charset && term.attr.bit.pc_charset )
    return false;

  return append_sequence(F_alt_charset.on.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermAltCharset (FChar& term) -> bool
{
  term.attr.bit.alt_charset = false;

  if ( alt_equal_pc_charset && term.attr.bit.pc_charset )
    return false;

  return append_sequence(F_alt_charset.off.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::setTermPCcharset (FChar& term) -> bool
{
  term.attr.bit.pc_charset = true;

  if ( alt_equal_pc_charset && term.attr.bit.alt_charset )
    return false;

  return append_sequence(F_pc_charset.on.cap);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::unsetTermPCcharset (FChar& term) -> bool
{
  term.attr.bit.pc_charset = false;

  if ( alt_equal_pc_charset && term.attr.bit.alt_charset )
    return false;

  return append_sequence(F_pc_charset.off.cap);
}

//----------------------------------------------------------------------
auto FOptiAttr::setTermDefaultColor (FChar& term) -> bool
{
  term.fg_color = FColor::Default;
  term.bg_color = FColor::Default;

  if ( append_sequence(F_color.orig_pair.cap)
    || append_sequence(F_color.orig_colors.cap) )
    return true;

  if ( F_color.ansi_default_color )
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
  setAttributes (changes.on.attr, attribute_on_handlers, term);
}

//----------------------------------------------------------------------
void FOptiAttr::setAttributesOff (FChar& term)
{
  static const auto& attribute_off_handlers = getAttributeOffHandlers();
  setAttributes (changes.off.attr, attribute_off_handlers, term);
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
inline auto FOptiAttr::isItalicsUsed ( const FChar& term
                                     , const FChar& next ) const -> bool
{
  return ! term.attr.bit.italic && next.attr.bit.italic;
}

//----------------------------------------------------------------------
inline auto FOptiAttr::isCrossedOutUsed ( const FChar& term
                                        , const FChar& next ) const -> bool
{
  return ! term.attr.bit.crossed_out && next.attr.bit.crossed_out;
}

//----------------------------------------------------------------------
inline auto FOptiAttr::isDoubleUnderlineUsed ( const FChar& term
                                             , const FChar& next ) const -> bool
{
  return ! term.attr.bit.dbl_underline && next.attr.bit.dbl_underline;
}

//----------------------------------------------------------------------
inline auto FOptiAttr::isPCcharsetUsed ( const FChar& term
                                       , const FChar& next ) const -> bool
{
  return ! term.attr.bit.pc_charset && next.attr.bit.pc_charset;
}

//----------------------------------------------------------------------
inline auto FOptiAttr::isPCcharsetUsable ( FChar& term
                                         , const FChar& next ) -> bool
{
  if ( alt_equal_pc_charset
    && F_pc_charset.on.cap
    && next.attr.bit.alt_charset )
  {
    term.attr.bit.pc_charset = next.attr.bit.pc_charset;
    changes.off.attr.bit.pc_charset = false;
    return false;
  }

  return true;
}

//----------------------------------------------------------------------
inline auto FOptiAttr::hasColorChanged ( const FChar& term
                                       , const FChar& next ) const -> bool
{
  const auto& b0_reverse_mask = internal::var::b0_reverse_mask;
  const bool frev ( ( (changes.on.attr.byte[0] & b0_reverse_mask)
                   || (changes.off.attr.byte[0] & b0_reverse_mask) ) && fake_reverse );
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
  // Ignore video attributes which can not combined with a color

  if ( ! (hasColor(attr) || next_has_color) || F_color.attr_without_color <= 0 )
    return;

  static const auto& no_color_video_handlers = getNoColorVideoHandlerTable();
  auto set_bits = uInt(F_color.attr_without_color);

  while ( set_bits )
  {
    uInt bit = set_bits & (~set_bits + 1);  // Get rightmost set bit
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
      if ( changes.off.attr.bit.alt_charset )  // Required for rxvt terminals
        unsetTermAltCharset(term);

      unsetTermAttributes(term);

      if ( changes.off.attr.bit.pc_charset )
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

  auto pc_charset_usable = isPCcharsetUsable(term, next);

  if ( changes.off.attr.bit.pc_charset )
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
    next.fg_color = FColor::Default;
    next.bg_color = FColor::Default;
    return;
  }

  normalizeColor (next.fg_color);
  normalizeColor (next.bg_color);
  FColor fg = next.fg_color;
  FColor bg = next.bg_color;
  handleDefaultColors (term, next, fg, bg);

  if ( fake_reverse && fg == FColor::Default && bg == FColor::Default )
    return;

  if ( fake_reverse && (next.attr.bit.reverse || next.attr.bit.standout) )
  {
    std::swap (fg, bg);
    handleDefaultColors (term, next, fg, bg);
  }

  change_current_color (term, fg, bg);
  term.fg_color = next.fg_color;
  term.bg_color = next.bg_color;
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
    auto set_default_fg = fg == FColor::Default && term.fg_color != FColor::Default;
    auto set_default_bg = bg == FColor::Default && term.bg_color != FColor::Default;

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
    fg = next.fg_color = FColor::LightGray;
    bg = next.bg_color = FColor::Black;
  }
}

//----------------------------------------------------------------------
inline void FOptiAttr::setDefaultForeground (FChar& term)
{
  std::string sgr_39{CSI "39m"};
  append_sequence (sgr_39);
  term.fg_color = FColor::Default;
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
  term.bg_color = FColor::Default;
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
  constexpr auto ANSI = 0;
  constexpr auto VGA = 1;

  auto apply_color_change = [this, &term, &fg, &bg, &frev] ( const char* fg_cap
                                                           , const char* bg_cap
                                                           , int cm )
  {
    if ( ! fg_cap || ! bg_cap )
      return false;

    const auto fg_value = ( cm == VGA ) ? uInt16(vga2ansi(fg)) : uInt16(fg);
    const auto bg_value = ( cm == VGA ) ? uInt16(vga2ansi(bg)) : uInt16(bg);

    if ( has_foreground_changes(term, fg, frev) )
      append_sequence(FTermcap::encodeParameter(fg_cap, fg_value));

    if ( has_background_changes(term, bg, frev) )
      append_sequence(FTermcap::encodeParameter(bg_cap, bg_value));

    return true;
  };

  const auto& apply_AF_AB = apply_color_change(AF, AB, VGA);
  const auto& apply_Sf_Sb = apply_color_change(Sf, Sb, ANSI);

  if ( apply_AF_AB || apply_Sf_Sb || ! sp )
    return;

  const auto fg_value = uInt16(vga2ansi(fg));
  const auto bg_value = uInt16(vga2ansi(bg));
  append_sequence (FTermcap::encodeParameter(sp, fg_value, bg_value));
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
void FOptiAttr::init_reset_attribute (Capability& off, uChar test) const
{
  if ( caused_reset_attributes(off.cap, test) )
    off.caused_reset = true;
}

//----------------------------------------------------------------------
inline auto FOptiAttr::fake_reverse_color_change (const FChar& term) const -> bool
{
  const auto& b0_reverse_mask = internal::var::b0_reverse_mask;
  return ( (changes.off.attr.byte[0] & b0_reverse_mask)
        || (term.attr.byte[0] & b0_reverse_mask) ) && fake_reverse;
}

//----------------------------------------------------------------------
inline auto FOptiAttr::hasCharsetEquivalence() const -> bool
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
auto FOptiAttr::getByte0ReverseMask() -> uInt8
{
  FCharAttribute mask{};
  mask.reverse = true;
  mask.standout = true;
  return getFAttributeByte(mask, 0);
}

//----------------------------------------------------------------------
auto FOptiAttr::getByte1Mask() -> uInt8
{
  FCharAttribute mask{};
  mask.protect = true;
  mask.crossed_out = true;
  mask.dbl_underline = true;
  mask.alt_charset = true;
  mask.pc_charset = true;
  return getFAttributeByte(mask, 1);
}

//----------------------------------------------------------------------
auto FOptiAttr::getByte1ResetMask() -> uInt8
{
  // Set bits that must not be reset
  FCharAttribute mask{};
  mask.transparent = true;
  mask.color_overlay = true;
  mask.inherit_background = true;
  return getFAttributeByte(mask, 1);
}

//----------------------------------------------------------------------
auto FOptiAttr::getByte2ResetMask() -> uInt8
{
  // Set bits that must not be reset
  FCharAttribute mask{};
  mask.no_changes = true;
  mask.printed = true;
  return getFAttributeByte(mask, 2);
}

//----------------------------------------------------------------------
inline void FOptiAttr::detectSwitchOn (const FChar& term, const FChar& next)
{
  // Detect switched on attributes on transition from "term" to "next"
  // and store the result in "on"

  const auto& mask = internal::var::b1_mask;
  changes.on.attr.byte[0] = ~(term.attr.byte[0]) & next.attr.byte[0];
  changes.on.attr.byte[1] = ~(term.attr.byte[1]) & next.attr.byte[1] & mask;
}

//----------------------------------------------------------------------
inline void FOptiAttr::detectSwitchOff (const FChar& term, const FChar& next)
{
  // Detect switched off attributes on transition from "term" to "next"
  // and store the result in "on"

  const auto& mask = internal::var::b1_mask;
  changes.off.attr.byte[0] = term.attr.byte[0] & ~(next.attr.byte[0]);
  changes.off.attr.byte[1] = term.attr.byte[1] & ~(next.attr.byte[1]) & mask;
}

//----------------------------------------------------------------------
inline auto FOptiAttr::switchOn() const -> bool
{
  return hasAttribute(changes.on);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::switchOff() const -> bool
{
  return hasAttribute(changes.off);
}

//----------------------------------------------------------------------
inline auto FOptiAttr::append_sequence (const std::string& seq) -> bool
{
  if ( seq.empty() )
    return false;

  attr_buf.append(seq);
  return true;
}


// non-member functions
//----------------------------------------------------------------------
inline bool has_foreground_changes (const FChar& term, const FColor fg, bool frev)
{
  return term.fg_color != fg || frev;
}

//----------------------------------------------------------------------
inline bool has_background_changes (const FChar& term, const FColor bg, bool frev)
{
  return term.bg_color != bg || frev;
}

}  // namespace finalcut
