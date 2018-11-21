/***********************************************************************
* foptiattr.cpp - Sets video attributes in optimized order             *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2016-2018 Markus Gans                                      *
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

#include <cstring>

#include "final/foptiattr.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FOptiAttr
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FOptiAttr::FOptiAttr()
  : F_enter_bold_mode()
  , F_exit_bold_mode()
  , F_enter_dim_mode()
  , F_exit_dim_mode()
  , F_enter_italics_mode()
  , F_exit_italics_mode()
  , F_enter_underline_mode()
  , F_exit_underline_mode()
  , F_enter_blink_mode()
  , F_exit_blink_mode()
  , F_enter_reverse_mode()
  , F_exit_reverse_mode()
  , F_enter_standout_mode()
  , F_exit_standout_mode()
  , F_enter_secure_mode()
  , F_exit_secure_mode()
  , F_enter_protected_mode()
  , F_exit_protected_mode()
  , F_enter_crossed_out_mode()
  , F_exit_crossed_out_mode()
  , F_enter_dbl_underline_mode()
  , F_exit_dbl_underline_mode()
  , F_set_attributes()
  , F_exit_attribute_mode()
  , F_enter_alt_charset_mode()
  , F_exit_alt_charset_mode()
  , F_enter_pc_charset_mode()
  , F_exit_pc_charset_mode()
  , F_set_a_foreground()
  , F_set_a_background()
  , F_set_foreground()
  , F_set_background()
  , F_set_color_pair()
  , F_orig_pair()
  , F_orig_colors()
  , on()
  , off()
  , reset_byte_mask()
  , max_color(1)
  , attr_without_color(0)
  , ansi_default_color(false)
  , alt_equal_pc_charset(false)
  , monochron(true)
  , fake_reverse(false)
  , attr_ptr(attr_buf)
{
  attr_buf[0] = '\0';
  // Set to 0 to reset
  reset_byte_mask.attr.byte[0] = 0;
  reset_byte_mask.attr.byte[1] = 0;
  reset_byte_mask.attr.byte[2] = 0;
  // Set bits that must not be reset
  reset_byte_mask.attr.bit.transparent  = true;
  reset_byte_mask.attr.bit.trans_shadow = true;
  reset_byte_mask.attr.bit.inherit_bg   = true;
  reset_byte_mask.attr.bit.no_changes   = true;
  reset_byte_mask.attr.bit.printed      = true;
}

//----------------------------------------------------------------------
FOptiAttr::~FOptiAttr()  // destructor
{ }


// public methods of FOptiAttr
//----------------------------------------------------------------------
void FOptiAttr::setTermEnvironment (termEnv& term_env)
{
  // Set all required termcap values at once
  // and initialize the FOptiAttr environment

  ansi_default_color = term_env.ansi_default_color;
  max_color = term_env.max_color;
  attr_without_color = term_env.attr_without_color;

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

  initialize();
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_bold_mode (char cap[])
{
  if ( cap )
  {
    F_enter_bold_mode.cap = cap;
    F_enter_bold_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_bold_mode (char cap[])
{
  if ( cap )
  {
    F_exit_bold_mode.cap = cap;
    F_exit_bold_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_dim_mode (char cap[])
{
  if ( cap )
  {
    F_enter_dim_mode.cap = cap;
    F_enter_dim_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_dim_mode (char cap[])
{
  if ( cap )
  {
    F_exit_dim_mode.cap = cap;
    F_exit_dim_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_italics_mode (char cap[])
{
  if ( cap )
  {
    F_enter_italics_mode.cap = cap;
    F_enter_italics_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_italics_mode (char cap[])
{
  if ( cap )
  {
    F_exit_italics_mode.cap = cap;
    F_exit_italics_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_underline_mode (char cap[])
{
  if ( cap )
  {
    F_enter_underline_mode.cap = cap;
    F_enter_underline_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_underline_mode (char cap[])
{
  if ( cap )
  {
    F_exit_underline_mode.cap = cap;
    F_exit_underline_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_blink_mode (char cap[])
{
  if ( cap )
  {
    F_enter_blink_mode.cap = cap;
    F_enter_blink_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_blink_mode (char cap[])
{
  if ( cap )
  {
    F_exit_blink_mode.cap = cap;
    F_exit_blink_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_reverse_mode (char cap[])
{
  if ( cap )
  {
    F_enter_reverse_mode.cap = cap;
    F_enter_reverse_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_reverse_mode (char cap[])
{
  if ( cap )
  {
    F_exit_reverse_mode.cap = cap;
    F_exit_reverse_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_secure_mode (char cap[])
{
  if ( cap )
  {
    F_enter_secure_mode.cap = cap;
    F_enter_secure_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_secure_mode (char cap[])
{
  if ( cap )
  {
    F_exit_secure_mode.cap = cap;
    F_exit_secure_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_protected_mode (char cap[])
{
  if ( cap )
  {
    F_enter_protected_mode.cap = cap;
    F_enter_protected_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_protected_mode (char cap[])
{
  if ( cap )
  {
    F_exit_protected_mode.cap = cap;
    F_exit_protected_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_crossed_out_mode (char cap[])
{
  if ( cap )
  {
    F_enter_crossed_out_mode.cap = cap;
    F_enter_crossed_out_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_crossed_out_mode (char cap[])
{
  if ( cap )
  {
    F_exit_crossed_out_mode.cap = cap;
    F_exit_crossed_out_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_dbl_underline_mode (char cap[])
{
  if ( cap )
  {
    F_enter_dbl_underline_mode.cap = cap;
    F_enter_dbl_underline_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_dbl_underline_mode (char cap[])
{
  if ( cap )
  {
    F_exit_dbl_underline_mode.cap = cap;
    F_exit_dbl_underline_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_standout_mode (char cap[])
{
  if ( cap )
  {
    F_enter_standout_mode.cap = cap;
    F_enter_standout_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_standout_mode (char cap[])
{
  if ( cap )
  {
    F_exit_standout_mode.cap = cap;
    F_exit_standout_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_set_attributes (char cap[])
{
  if ( cap )
  {
    F_set_attributes.cap = cap;
    F_set_attributes.caused_reset = true;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_attribute_mode (char cap[])
{
  if ( cap )
  {
    F_exit_attribute_mode.cap = cap;
    F_exit_attribute_mode.caused_reset = true;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_alt_charset_mode (char cap[])
{
  if ( cap )
  {
    F_enter_alt_charset_mode.cap = cap;
    F_enter_alt_charset_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_alt_charset_mode (char cap[])
{
  if ( cap )
  {
    F_exit_alt_charset_mode.cap = cap;
    F_exit_alt_charset_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_pc_charset_mode (char cap[])
{
  if ( cap )
  {
    F_enter_pc_charset_mode.cap = cap;
    F_enter_pc_charset_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_pc_charset_mode (char cap[])
{
  if ( cap )
  {
    F_exit_pc_charset_mode.cap = cap;
    F_exit_pc_charset_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_a_foreground_color (char cap[])
{
  if ( cap )
  {
    F_set_a_foreground.cap = cap;
    F_set_a_foreground.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_a_background_color (char cap[])
{
  if ( cap )
  {
    F_set_a_background.cap = cap;
    F_set_a_background.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_foreground_color (char cap[])
{
  if ( cap )
  {
    F_set_foreground.cap = cap;
    F_set_foreground.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_background_color (char cap[])
{
  if ( cap )
  {
    F_set_background.cap = cap;
    F_set_background.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_term_color_pair (char cap[])
{
  if ( cap )
  {
    F_set_color_pair.cap = cap;
    F_set_color_pair.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_orig_pair (char cap[])
{
  if ( cap )
  {
    F_orig_pair.cap = cap;
    F_orig_pair.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_orig_orig_colors (char cap[])
{
  if ( cap )
  {
    F_orig_colors.cap = cap;
    F_orig_colors.caused_reset = false;
  }
}

//----------------------------------------------------------------------
bool FOptiAttr::isNormal (charData*& ch)
{
  return hasNoAttribute(ch) && ! hasColor(ch);
}

//----------------------------------------------------------------------
void FOptiAttr::initialize()
{
  if ( max_color < 8 )
    monochron = true;
  else
    monochron = false;

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
FColor FOptiAttr::vga2ansi (FColor color)
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

  if ( color == fc::Default )
    color = 0;
  else if ( color < 16 )
  {
    static const FColor lookup_table[] =
    {
      0,  4,  2,  6,  1,  5,  3,  7,
      8, 12, 10, 14,  9, 13, 11, 15
    };

    color = lookup_table[color];
  }

  return color;
}

//----------------------------------------------------------------------
char* FOptiAttr::changeAttribute (charData*& term, charData*& next)
{
  const bool next_has_color = hasColor(next);
  fake_reverse = false;
  attr_buf[0] = '\0';

  if ( ! (term && next) )
    return attr_buf;

  prevent_no_color_video_attributes (term, next_has_color);
  prevent_no_color_video_attributes (next);
  detectSwitchOn (term, next);
  detectSwitchOff (term, next);

  // Simulate invisible characters
  if ( ! F_enter_secure_mode.cap && next->attr.bit.invisible )
    next->code = ' ';

  // Look for no changes
  if ( ! (switchOn() || switchOff() || hasColorChanged(term, next)) )
    return 0;

  if ( hasNoAttribute(next) )
  {
    deactivateAttributes (term, next);
  }
  else if ( F_set_attributes.cap
         && (! term->attr.bit.pc_charset || alt_equal_pc_charset) )
  {
    changeAttributeSGR (term, next);
  }
  else
  {
    changeAttributeSeparately (term, next);
  }

  return attr_buf;
}


// private methods of FOptiAttr
//----------------------------------------------------------------------
inline bool FOptiAttr::setTermBold (charData*& term)
{
  if ( ! term )
    return false;

  term->attr.bit.bold = true;

  if ( append_sequence(F_enter_bold_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermBold (charData*& term)
{
  // Back to normal intensity (turns off bold + dim)

  if ( ! term )
    return false;

  if ( F_exit_bold_mode.caused_reset )
    reset(term);
  else
  {
    term->attr.bit.bold = false;
    term->attr.bit.dim = false;
  }

  if ( append_sequence(F_exit_bold_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermDim (charData*& term)
{
  if ( ! term )
    return false;

  term->attr.bit.dim = true;

  if ( append_sequence(F_enter_dim_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermDim (charData*& term)
{
  // Back to normal intensity (turns off bold + dim)

  if ( ! term )
    return false;

  if ( F_exit_dim_mode.caused_reset )
    reset(term);
  else
  {
    term->attr.bit.bold = false;
    term->attr.bit.dim = false;
  }

  if ( append_sequence(F_exit_dim_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermItalic (charData*& term)
{
  if ( ! term )
    return false;

  term->attr.bit.italic = true;

  if ( append_sequence(F_enter_italics_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermItalic (charData*& term)
{
  if ( ! term )
    return false;

  if ( F_exit_italics_mode.caused_reset )
    reset(term);
  else
    term->attr.bit.italic = false;

  if ( append_sequence(F_exit_italics_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermUnderline (charData*& term)
{
  if ( ! term )
    return false;

  term->attr.bit.underline = true;

  if ( append_sequence(F_enter_underline_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermUnderline (charData*& term)
{
  // Turns off every underlining

  if ( ! term )
    return false;

  if ( F_exit_underline_mode.caused_reset )
    reset(term);
  else
  {
    term->attr.bit.underline = false;
    term->attr.bit.dbl_underline = false;
  }

  if ( append_sequence(F_exit_underline_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermBlink (charData*& term)
{
  if ( ! term )
    return false;

  term->attr.bit.blink = true;

  if ( append_sequence(F_enter_blink_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermBlink (charData*& term)
{
  if ( ! term )
    return false;

  if ( F_exit_blink_mode.caused_reset )
    reset(term);
  else
    term->attr.bit.blink = false;

  if ( append_sequence(F_exit_blink_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermReverse (charData*& term)
{
  if ( ! term )
    return false;

  term->attr.bit.reverse = true;

  if ( ! fake_reverse && append_sequence(F_enter_reverse_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermReverse (charData*& term)
{
  if ( ! term )
    return false;

  if ( F_exit_reverse_mode.caused_reset )
    reset(term);
  else
    term->attr.bit.reverse = false;

  if ( ! fake_reverse && append_sequence(F_exit_reverse_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermStandout (charData*& term)
{
  if ( ! term )
    return false;

  term->attr.bit.standout = true;

  if ( ! fake_reverse && append_sequence(F_enter_standout_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermStandout (charData*& term)
{
  if ( ! term )
    return false;

  if ( F_exit_standout_mode.caused_reset )
    reset(term);
  else
    term->attr.bit.standout = false;

  if ( ! fake_reverse && append_sequence(F_exit_standout_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermInvisible (charData*& term)
{
  if ( ! term )
    return false;

  term->attr.bit.invisible = true;

  if ( append_sequence(F_enter_secure_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermInvisible (charData*& term)
{
  if ( ! term )
    return false;

  if ( F_exit_secure_mode.caused_reset )
    reset(term);
  else
    term->attr.bit.invisible = false;

  if ( append_sequence(F_exit_secure_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermProtected (charData*& term)
{
  if ( ! term )
    return false;

  term->attr.bit.protect = true;

  if ( append_sequence(F_enter_protected_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermProtected (charData*& term)
{
  if ( ! term )
    return false;

  if ( F_exit_protected_mode.caused_reset )
    reset(term);
  else
    term->attr.bit.protect = false;

  if ( append_sequence(F_exit_protected_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermCrossedOut (charData*& term)
{
  if ( ! term )
    return false;

  term->attr.bit.crossed_out = true;

  if ( append_sequence(F_enter_crossed_out_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermCrossedOut (charData*& term)
{
  if ( ! term )
    return false;

  if ( F_exit_crossed_out_mode.caused_reset )
    reset(term);
  else
    term->attr.bit.crossed_out = false;

  if ( append_sequence(F_exit_crossed_out_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermDoubleUnderline (charData*& term)
{
  if ( ! term )
    return false;

  term->attr.bit.dbl_underline = true;

  if ( append_sequence(F_enter_dbl_underline_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermDoubleUnderline (charData*& term)
{
  // Turns off every underlining

  if ( ! term )
    return false;

  if ( F_exit_dbl_underline_mode.caused_reset )
    reset(term);
  else
  {
    term->attr.bit.underline = false;
    term->attr.bit.dbl_underline = false;
  }

  if ( append_sequence(F_exit_dbl_underline_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
bool FOptiAttr::setTermAttributes ( charData*& term
                                  , bool p1, bool p2, bool p3
                                  , bool p4, bool p5, bool p6
                                  , bool p7, bool p8, bool p9 )
{
  if ( term && F_set_attributes.cap )
  {
    char* sgr = tparm ( F_set_attributes.cap
                      , p1 && ! fake_reverse
                      , p2
                      , p3 && ! fake_reverse
                      , p4
                      , p5
                      , p6
                      , p7
                      , p8
                      , p9 );
    append_sequence (sgr);
    resetColor(term);
    term->attr.bit.standout      = p1;
    term->attr.bit.underline     = p2;
    term->attr.bit.reverse       = p3;
    term->attr.bit.blink         = p4;
    term->attr.bit.dim           = p5;
    term->attr.bit.bold          = p6;
    term->attr.bit.invisible     = p7;
    term->attr.bit.protect       = p8;
    term->attr.bit.alt_charset   = p9;
    term->attr.bit.pc_charset    = false;
    term->attr.bit.italic        = false;
    term->attr.bit.crossed_out   = false;
    term->attr.bit.dbl_underline = false;

    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermAttributes (charData*& term)
{
  if ( ! term )
    return false;

  reset(term);

  if ( append_sequence(F_exit_attribute_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermAltCharset (charData*& term)
{
  if ( ! term )
    return false;

  term->attr.bit.alt_charset = true;

  if ( alt_equal_pc_charset && term->attr.bit.pc_charset )
    return false;

  if ( append_sequence(F_enter_alt_charset_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermAltCharset (charData*& term)
{
  if ( ! term )
    return false;

  term->attr.bit.alt_charset = false;

  if ( alt_equal_pc_charset && term->attr.bit.pc_charset )
    return false;

  if ( append_sequence(F_exit_alt_charset_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermPCcharset (charData*& term)
{
  if ( ! term )
    return false;

  term->attr.bit.pc_charset = true;

  if ( alt_equal_pc_charset && term->attr.bit.alt_charset )
    return false;

  if ( append_sequence(F_enter_pc_charset_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermPCcharset (charData*& term)
{
  if ( ! term )
    return false;

  term->attr.bit.pc_charset = false;

  if ( alt_equal_pc_charset && term->attr.bit.alt_charset )
    return false;

  if ( append_sequence(F_exit_pc_charset_mode.cap) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
bool FOptiAttr::setTermDefaultColor (charData*& term)
{
  if ( ! term )
    return false;

  term->fg_color = fc::Default;
  term->bg_color = fc::Default;

  if ( append_sequence(F_orig_pair.cap) )
    return true;
  else if ( append_sequence(F_orig_colors.cap) )
    return true;
  else if ( ansi_default_color )
  {
    char sgr_39_49[] = CSI "39;49m";
    append_sequence (sgr_39_49);
    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
void FOptiAttr::setAttributesOn (charData*& term)
{
  if ( on.attr.bit.alt_charset )
    setTermAltCharset(term);

  if ( on.attr.bit.pc_charset )
    setTermPCcharset(term);

  if ( on.attr.bit.bold )
    setTermBold(term);

  if ( on.attr.bit.dim )
    setTermDim(term);

  if ( on.attr.bit.italic )
    setTermItalic(term);

  if ( on.attr.bit.underline )
    setTermUnderline(term);

  if ( on.attr.bit.blink )
    setTermBlink(term);

  if ( on.attr.bit.reverse )
    setTermReverse(term);

  if ( on.attr.bit.standout )
    setTermStandout(term);

  if ( on.attr.bit.invisible )
    setTermInvisible(term);

  if ( on.attr.bit.protect )
    setTermProtected(term);

  if ( on.attr.bit.crossed_out )
    setTermCrossedOut(term);

  if ( on.attr.bit.dbl_underline )
    setTermDoubleUnderline(term);
}

//----------------------------------------------------------------------
void FOptiAttr::setAttributesOff (charData*& term)
{
  if ( off.attr.bit.pc_charset )
    unsetTermPCcharset(term);

  if ( off.attr.bit.alt_charset )
    unsetTermAltCharset(term);

  if ( off.attr.bit.bold )
    unsetTermBold(term);

  if ( off.attr.bit.dim )
    unsetTermDim(term);

  if ( off.attr.bit.italic )
    unsetTermItalic(term);

  if ( off.attr.bit.underline )
    unsetTermUnderline(term);

  if ( off.attr.bit.blink )
    unsetTermBlink(term);

  if ( off.attr.bit.reverse )
    unsetTermReverse(term);

  if ( off.attr.bit.standout )
    unsetTermStandout(term);

  if ( off.attr.bit.invisible )
    unsetTermInvisible(term);

  if ( off.attr.bit.protect )
    unsetTermProtected(term);

  if ( off.attr.bit.crossed_out )
    unsetTermCrossedOut(term);

  if ( off.attr.bit.dbl_underline )
    unsetTermDoubleUnderline(term);
}

//----------------------------------------------------------------------
bool FOptiAttr::hasColor (charData*& attr)
{
  if ( attr
    && attr->fg_color == fc::Default
    && attr->bg_color == fc::Default )
    return false;
  else
    return true;
}

//----------------------------------------------------------------------
bool FOptiAttr::hasAttribute (charData*& attr)
{
  if ( attr )
  {
    return attr->attr.bit.bold          == 1
        || attr->attr.bit.dim           == 1
        || attr->attr.bit.italic        == 1
        || attr->attr.bit.underline     == 1
        || attr->attr.bit.blink         == 1
        || attr->attr.bit.reverse       == 1
        || attr->attr.bit.standout      == 1
        || attr->attr.bit.invisible     == 1
        || attr->attr.bit.protect       == 1
        || attr->attr.bit.crossed_out   == 1
        || attr->attr.bit.dbl_underline == 1
        || attr->attr.bit.alt_charset   == 1
        || attr->attr.bit.pc_charset    == 1;
  }

  return false;
}

//----------------------------------------------------------------------
bool FOptiAttr::hasNoAttribute (charData*& attr)
{
  return ! hasAttribute(attr);
}

//----------------------------------------------------------------------
inline bool FOptiAttr::hasColorChanged (charData*& term, charData*& next)
{
  if ( term && next )
  {
    bool frev = ( on.attr.bit.reverse
               || on.attr.bit.standout
               || off.attr.bit.reverse
               || off.attr.bit.standout ) && fake_reverse;
    return bool ( frev
               || term->fg_color != next->fg_color
               || term->bg_color != next->bg_color );
  }

  return false;
}

//----------------------------------------------------------------------
inline void FOptiAttr::resetColor (charData*& attr)
{
  if ( attr )
  {
    attr->fg_color = fc::Default;
    attr->bg_color = fc::Default;
  }
}

//----------------------------------------------------------------------
inline void FOptiAttr::prevent_no_color_video_attributes ( charData*& attr
                                                         , bool next_has_color )
{
  // Ignore attributes which can not combined with a color

  if ( ! attr
    || ! (hasColor(attr) || next_has_color)
    || attr_without_color <= 0 )
    return;

  for (int bit = 1; bit < no_mode; bit <<= 1)
  {
    switch ( bit & attr_without_color )
    {
      case standout_mode:
        attr->attr.bit.standout = false;
        break;

      case underline_mode:
        attr->attr.bit.underline = false;
        break;

      case reverse_mode:
        fake_reverse = true;
        break;

      case blink_mode:
        attr->attr.bit.blink = false;
        break;

      case dim_mode:
        attr->attr.bit.dim = false;
        break;

      case bold_mode:
        attr->attr.bit.bold = false;
        break;

      case invisible_mode:
        attr->attr.bit.invisible = false;
        break;

      case protected_mode:
        attr->attr.bit.protect = false;
        break;

      case alt_charset_mode:
        attr->attr.bit.alt_charset = false;
        break;

      case italic_mode:
        attr->attr.bit.italic = false;
        break;

      default:
        break;
    }
  }
}

//----------------------------------------------------------------------
inline void FOptiAttr::deactivateAttributes ( charData*& term
                                            , charData*& next )
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
inline void FOptiAttr::changeAttributeSGR ( charData*& term
                                          , charData*& next )
{
  bool pc_charset_usable = true;

  if ( ! (term && next) )
    return;

  if ( switchOn() || switchOff() )
    setTermAttributes ( term
                      , next->attr.bit.standout
                      , next->attr.bit.underline
                      , next->attr.bit.reverse
                      , next->attr.bit.blink
                      , next->attr.bit.dim
                      , next->attr.bit.bold
                      , next->attr.bit.invisible
                      , next->attr.bit.protect
                      , next->attr.bit.alt_charset );

  if ( alt_equal_pc_charset
    && F_enter_pc_charset_mode.cap
    && next->attr.bit.alt_charset )
  {
    term->attr.bit.pc_charset = next->attr.bit.pc_charset;
    off.attr.bit.pc_charset = false;
    pc_charset_usable = false;
  }

  if ( off.attr.bit.pc_charset )
    unsetTermPCcharset(term);

  if ( ! term->attr.bit.italic && next->attr.bit.italic )
    setTermItalic(term);

  if ( ! term->attr.bit.crossed_out && next->attr.bit.crossed_out )
    setTermCrossedOut(term);

  if ( ! term->attr.bit.dbl_underline && next->attr.bit.dbl_underline )
    setTermDoubleUnderline(term);

  if ( ! term->attr.bit.pc_charset && next->attr.bit.pc_charset
    && pc_charset_usable )
    setTermPCcharset(term);

  if ( hasColorChanged(term, next) )
    change_color(term, next);
}

//----------------------------------------------------------------------
inline void FOptiAttr::changeAttributeSeparately ( charData*& term
                                                 , charData*& next )
{
  setAttributesOff(term);

  if ( hasColorChanged(term, next) )
    change_color (term, next);

  detectSwitchOn (term, next);  // After reset all attributes
  setAttributesOn(term);
}

//----------------------------------------------------------------------
void FOptiAttr::change_color (charData*& term, charData*& next)
{
  FColor fg, bg;

  if ( ! (term && next) )
    return;

  if ( monochron )
  {
    next->fg_color = fc::Default;
    next->bg_color = fc::Default;
    return;
  }

  if ( next->fg_color != fc::Default )
    next->fg_color %= max_color;

  if ( next->bg_color != fc::Default )
    next->bg_color %= max_color;

  fg = next->fg_color;
  bg = next->bg_color;

  if ( fg == fc::Default || bg == fc::Default )
    change_to_default_color (term, next, fg, bg);

  if ( fake_reverse && fg == fc::Default && bg == fc::Default )
    return;

  if ( fake_reverse
    && (next->attr.bit.reverse || next->attr.bit.standout) )
  {
    std::swap (fg, bg);

    if ( fg == fc::Default || bg == fc::Default )
      setTermDefaultColor(term);
  }

  change_current_color (term, fg, bg);

  term->fg_color = next->fg_color;
  term->bg_color = next->bg_color;
}

//----------------------------------------------------------------------
inline void FOptiAttr::change_to_default_color ( charData*& term
                                               , charData*& next
                                               , FColor& fg, FColor& bg )
{
  if ( ansi_default_color )
  {
    if ( fg == fc::Default && term->fg_color != fc::Default
      && bg == fc::Default && term->bg_color != fc::Default )
    {
      setTermDefaultColor(term);
    }
    else if ( fg == fc::Default && term->fg_color != fc::Default )
    {
      char sgr_39[] = CSI "39m";
      append_sequence (sgr_39);
      term->fg_color = fc::Default;
    }
    else if ( bg == fc::Default && term->bg_color != fc::Default )
    {
      char* sgr_49;
      char* op = F_orig_pair.cap;

      if ( op && std::strncmp (op, CSI "39;49;25m", 11) == 0 )
        sgr_49 = C_STR(CSI "49;25m");
      else
        sgr_49 = C_STR(CSI "49m");

      append_sequence (sgr_49);
      term->bg_color = fc::Default;
    }
  }
  else if ( ! setTermDefaultColor(term) )
  {
    // Fallback to gray on black
    fg = next->fg_color = fc::LightGray;
    bg = next->bg_color = fc::Black;
  }
}

//----------------------------------------------------------------------
inline void FOptiAttr::change_current_color ( charData*& term
                                            , FColor fg, FColor bg )
{
  char* color_str;
  char* AF = F_set_a_foreground.cap;
  char* AB = F_set_a_background.cap;
  char* Sf = F_set_foreground.cap;
  char* Sb = F_set_background.cap;
  char* sp = F_set_color_pair.cap;
  bool frev = ( off.attr.bit.reverse
             || off.attr.bit.standout
             || term->attr.bit.reverse
             || term->attr.bit.standout ) && fake_reverse;

  if ( AF && AB )
  {
    FColor ansi_fg = vga2ansi(fg);
    FColor ansi_bg = vga2ansi(bg);

    if ( (term->fg_color != fg || frev)
      && (color_str = tparm(AF, ansi_fg, 0, 0, 0, 0, 0, 0, 0, 0)) )
      append_sequence (color_str);

    if ( (term->bg_color != bg || frev)
      && (color_str = tparm(AB, ansi_bg, 0, 0, 0, 0, 0, 0, 0, 0)) )
      append_sequence (color_str);
  }
  else if ( Sf && Sb )
  {
    if ( (term->fg_color != fg || frev)
      && (color_str = tparm(Sf, fg, 0, 0, 0, 0, 0, 0, 0, 0)) )
      append_sequence (color_str);

    if ( (term->bg_color != bg || frev)
      && (color_str = tparm(Sb, bg, 0, 0, 0, 0, 0, 0, 0, 0)) )
      append_sequence (color_str);
  }
  else if ( sp )
  {
    fg = vga2ansi(fg);
    bg = vga2ansi(bg);

    if ( (color_str = tparm(sp, fg, bg, 0, 0, 0, 0, 0, 0, 0)) )
      append_sequence (color_str);
  }
}

//----------------------------------------------------------------------
inline void FOptiAttr::resetAttribute (charData*& attr)
{
  if ( attr )
  {
    attr->attr.byte[0]  = 0;
    attr->attr.byte[1] &= reset_byte_mask.attr.byte[1];
  }
}

//----------------------------------------------------------------------
inline void FOptiAttr::reset (charData*& attr)
{
  if ( attr )
  {
    resetAttribute(attr);
    resetColor(attr);
  }
}

//----------------------------------------------------------------------
bool FOptiAttr::caused_reset_attributes (char cap[], uChar test)
{
  // test if "cap" reset all attributes
  char* ue = F_exit_underline_mode.cap;
  char* se = F_exit_standout_mode.cap;
  char* me = F_exit_attribute_mode.cap;

  if ( cap )
  {
    if ( (test & test_ansi_reset) && std::strncmp (cap, CSI "m", 3) == 0 )
      return true;

    if ( (test & test_adm3_reset) && std::strncmp (cap, ESC "G0", 3) == 0 )
      return true;

    if ( (test & same_like_ue) && ue && std::strcmp (cap, ue) == 0
       && std::strncmp (cap, CSI "24m", 5) != 0)
      return true;

    if ( (test & same_like_se) && se && std::strcmp (cap, se) == 0
       && std::strncmp (cap, CSI "27m", 5) != 0 )
      return true;

    if ( (test & same_like_me) && me && std::strcmp (cap, me) == 0 )
      return true;
  }

  return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::hasCharsetEquivalence()
{
  // Detect if alt charset and pc charset are the same sequences

  char* alt_on  = F_enter_alt_charset_mode.cap;
  char* alt_off = F_enter_pc_charset_mode.cap;
  char* pc_on   = F_enter_pc_charset_mode.cap;
  char* pc_off  = F_exit_pc_charset_mode.cap;

  if ( alt_on && pc_on && std::strcmp (alt_on, pc_on) == 0 )
    return true;

  if ( alt_off && pc_off && std::strcmp (alt_off, pc_off) == 0 )
    return true;

  return false;
}

//----------------------------------------------------------------------
inline void FOptiAttr::detectSwitchOn (charData*& term, charData*& next)
{
  if ( ! (term && next) )
    return;

  on.attr.bit.bold          = ! term->attr.bit.bold          && next->attr.bit.bold;
  on.attr.bit.dim           = ! term->attr.bit.dim           && next->attr.bit.dim;
  on.attr.bit.italic        = ! term->attr.bit.italic        && next->attr.bit.italic;
  on.attr.bit.underline     = ! term->attr.bit.underline     && next->attr.bit.underline;
  on.attr.bit.blink         = ! term->attr.bit.blink         && next->attr.bit.blink;
  on.attr.bit.reverse       = ! term->attr.bit.reverse       && next->attr.bit.reverse;
  on.attr.bit.standout      = ! term->attr.bit.standout      && next->attr.bit.standout;
  on.attr.bit.invisible     = ! term->attr.bit.invisible     && next->attr.bit.invisible;
  on.attr.bit.protect       = ! term->attr.bit.protect       && next->attr.bit.protect;
  on.attr.bit.crossed_out   = ! term->attr.bit.crossed_out   && next->attr.bit.crossed_out;
  on.attr.bit.dbl_underline = ! term->attr.bit.dbl_underline && next->attr.bit.dbl_underline;
  on.attr.bit.alt_charset   = ! term->attr.bit.alt_charset   && next->attr.bit.alt_charset;
  on.attr.bit.pc_charset    = ! term->attr.bit.pc_charset    && next->attr.bit.pc_charset;
}

//----------------------------------------------------------------------
inline void FOptiAttr::detectSwitchOff (charData*& term, charData*& next)
{
  if ( ! (term && next) )
    return;

  off.attr.bit.bold          = term->attr.bit.bold          && ! next->attr.bit.bold;
  off.attr.bit.dim           = term->attr.bit.dim           && ! next->attr.bit.dim;
  off.attr.bit.italic        = term->attr.bit.italic        && ! next->attr.bit.italic;
  off.attr.bit.underline     = term->attr.bit.underline     && ! next->attr.bit.underline;
  off.attr.bit.blink         = term->attr.bit.blink         && ! next->attr.bit.blink;
  off.attr.bit.reverse       = term->attr.bit.reverse       && ! next->attr.bit.reverse;
  off.attr.bit.standout      = term->attr.bit.standout      && ! next->attr.bit.standout;
  off.attr.bit.invisible     = term->attr.bit.invisible     && ! next->attr.bit.invisible;
  off.attr.bit.protect       = term->attr.bit.protect       && ! next->attr.bit.protect;
  off.attr.bit.crossed_out   = term->attr.bit.crossed_out   && ! next->attr.bit.crossed_out;
  off.attr.bit.dbl_underline = term->attr.bit.dbl_underline && ! next->attr.bit.dbl_underline;
  off.attr.bit.alt_charset   = term->attr.bit.alt_charset   && ! next->attr.bit.alt_charset;
  off.attr.bit.pc_charset    = term->attr.bit.pc_charset    && ! next->attr.bit.pc_charset;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::switchOn()
{
  charData* on_ptr = &on;
  return hasAttribute(on_ptr);
}

//----------------------------------------------------------------------
inline bool FOptiAttr::switchOff()
{
  charData* off_ptr = &off;
  return hasAttribute(off_ptr);
}

//----------------------------------------------------------------------
inline bool FOptiAttr::append_sequence (char seq[])
{
  if ( seq )
  {
    std::strncat (attr_ptr, seq, sizeof(attr_buf) - std::strlen(attr_ptr));
    attr_buf[sizeof(attr_buf) - 1] = '\0';
    return true;
  }
  else
    return false;
}

}  // namespace finalcut
