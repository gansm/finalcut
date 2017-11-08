/***********************************************************************
* foptiattr.cpp - Sets video attributes in optimized order             *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2016-2017 Markus Gans                                      *
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
  , max_color(1)
  , attr_without_color(0)
  , ansi_default_color(false)
  , monochron(true)
  , fake_reverse(false)
  , cygwin_terminal(false)
  , attr_ptr(attr_buf)
{
  attr_buf[0] = '\0';
}

//----------------------------------------------------------------------
FOptiAttr::~FOptiAttr()  // destructor
{ }


// public methods of FOptiAttr
//----------------------------------------------------------------------
void FOptiAttr::set_enter_bold_mode (char*& cap)
{
  if ( cap )
  {
    F_enter_bold_mode.cap = cap;
    F_enter_bold_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_bold_mode (char*& cap)
{
  if ( cap )
  {
    F_exit_bold_mode.cap = cap;
    F_exit_bold_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_dim_mode (char*& cap)
{
  if ( cap )
  {
    F_enter_dim_mode.cap = cap;
    F_enter_dim_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_dim_mode (char*& cap)
{
  if ( cap )
  {
    F_exit_dim_mode.cap = cap;
    F_exit_dim_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_italics_mode (char*& cap)
{
  if ( cap )
  {
    F_enter_italics_mode.cap = cap;
    F_enter_italics_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_italics_mode (char*& cap)
{
  if ( cap )
  {
    F_exit_italics_mode.cap = cap;
    F_exit_italics_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_underline_mode (char*& cap)
{
  if ( cap )
  {
    F_enter_underline_mode.cap = cap;
    F_enter_underline_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_underline_mode (char*& cap)
{
  if ( cap )
  {
    F_exit_underline_mode.cap = cap;
    F_exit_underline_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_blink_mode (char*& cap)
{
  if ( cap )
  {
    F_enter_blink_mode.cap = cap;
    F_enter_blink_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_blink_mode (char*& cap)
{
  if ( cap )
  {
    F_exit_blink_mode.cap = cap;
    F_exit_blink_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_reverse_mode (char*& cap)
{
  if ( cap )
  {
    F_enter_reverse_mode.cap = cap;
    F_enter_reverse_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_reverse_mode (char*& cap)
{
  if ( cap )
  {
    F_exit_reverse_mode.cap = cap;
    F_exit_reverse_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_secure_mode (char*& cap)
{
  if ( cap )
  {
    F_enter_secure_mode.cap = cap;
    F_enter_secure_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_secure_mode (char*& cap)
{
  if ( cap )
  {
    F_exit_secure_mode.cap = cap;
    F_exit_secure_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_protected_mode (char*& cap)
{
  if ( cap )
  {
    F_enter_protected_mode.cap = cap;
    F_enter_protected_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_protected_mode (char*& cap)
{
  if ( cap )
  {
    F_exit_protected_mode.cap = cap;
    F_exit_protected_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_crossed_out_mode (char*& cap)
{
  if ( cap )
  {
    F_enter_crossed_out_mode.cap = cap;
    F_enter_crossed_out_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_crossed_out_mode (char*& cap)
{
  if ( cap )
  {
    F_exit_crossed_out_mode.cap = cap;
    F_exit_crossed_out_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_dbl_underline_mode (char*& cap)
{
  if ( cap )
  {
    F_enter_dbl_underline_mode.cap = cap;
    F_enter_dbl_underline_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_dbl_underline_mode (char*& cap)
{
  if ( cap )
  {
    F_exit_dbl_underline_mode.cap = cap;
    F_exit_dbl_underline_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_standout_mode (char*& cap)
{
  if ( cap )
  {
    F_enter_standout_mode.cap = cap;
    F_enter_standout_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_standout_mode (char*& cap)
{
  if ( cap )
  {
    F_exit_standout_mode.cap = cap;
    F_exit_standout_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_set_attributes (char*& cap)
{
  if ( cap )
  {
    F_set_attributes.cap = cap;
    F_set_attributes.caused_reset = true;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_attribute_mode (char*& cap)
{
  if ( cap )
  {
    F_exit_attribute_mode.cap = cap;
    F_exit_attribute_mode.caused_reset = true;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_alt_charset_mode (char*& cap)
{
  if ( cap )
  {
    F_enter_alt_charset_mode.cap = cap;
    F_enter_alt_charset_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_alt_charset_mode (char*& cap)
{
  if ( cap )
  {
    F_exit_alt_charset_mode.cap = cap;
    F_exit_alt_charset_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_enter_pc_charset_mode (char*& cap)
{
  if ( cap )
  {
    F_enter_pc_charset_mode.cap = cap;
    F_enter_pc_charset_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_exit_pc_charset_mode (char*& cap)
{
  if ( cap )
  {
    F_exit_pc_charset_mode.cap = cap;
    F_exit_pc_charset_mode.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_a_foreground_color (char*& cap)
{
  if ( cap )
  {
    F_set_a_foreground.cap = cap;
    F_set_a_foreground.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_a_background_color (char*& cap)
{
  if ( cap )
  {
    F_set_a_background.cap = cap;
    F_set_a_background.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_foreground_color (char*& cap)
{
  if ( cap )
  {
    F_set_foreground.cap = cap;
    F_set_foreground.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_background_color (char*& cap)
{
  if ( cap )
  {
    F_set_background.cap = cap;
    F_set_background.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_term_color_pair (char*& cap)
{
  if ( cap )
  {
    F_set_color_pair.cap = cap;
    F_set_color_pair.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_orig_pair (char*& cap)
{
  if ( cap )
  {
    F_orig_pair.cap = cap;
    F_orig_pair.caused_reset = false;
  }
}

//----------------------------------------------------------------------
void FOptiAttr::set_orig_orig_colors (char*& cap)
{
  if ( cap )
  {
    F_orig_colors.cap = cap;
    F_orig_colors.caused_reset = false;
  }
}

//----------------------------------------------------------------------
bool FOptiAttr::isNormal (char_data*& ch)
{
  return hasNoAttribute(ch) && ! hasColor(ch);
}

//----------------------------------------------------------------------
void FOptiAttr::init()
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
}

//----------------------------------------------------------------------
short FOptiAttr::vga2ansi (register short color)
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

  if ( color >= 0 && color < 16 )
  {
    static const short lookup_table[] =
    {
      0,  4,  2,  6,  1,  5,  3,  7,
      8, 12, 10, 14,  9, 13, 11, 15
    };

    color = lookup_table[color];
  }

  return color;
}

//----------------------------------------------------------------------
char* FOptiAttr::changeAttribute (char_data*& term, char_data*& next)
{
  fake_reverse = false;
  attr_buf[0] = '\0';

  if ( ! (term && next) )
    return attr_buf;

  prevent_no_color_video_attributes (next);
  detectSwitchOn (term, next);
  detectSwitchOff (term, next);

  // look for no changes
  if ( ! ( switchOn() || switchOff() || colorChange(term, next) ) )
    return 0;

  // Simulate invisible characters
  if ( ! F_enter_secure_mode.cap && next->attr.bit.invisible )
    next->code = ' ';

  if ( cygwin_terminal && (term->fg_color > 7 || term->bg_color > 7) )
  {
    // reset blink and bold mode from colors > 7
    char* rst = const_cast<char*>(CSI "m");
    append_sequence (rst);
    reset(term);
  }

  if ( hasNoAttribute(next) )
  {
    if ( hasAttribute(term) )
    {
      if ( F_exit_attribute_mode.cap )
      {
        unsetTermAttributes(term);

        if ( off.attr.bit.pc_charset )
          unsetTermPCcharset(term);

        if ( off.attr.bit.alt_charset )
          unsetTermAltCharset(term);
      }
      else
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
    }

    if ( colorChange(term, next) )
      change_color (term, next);
  }
  else if ( F_set_attributes.cap && ! term->attr.bit.pc_charset )
  {
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

    if ( off.attr.bit.pc_charset )
      unsetTermPCcharset(term);

    if ( next->attr.bit.italic )
      setTermItalic(term);

    if ( next->attr.bit.crossed_out )
      setTermCrossedOut(term);

    if ( next->attr.bit.dbl_underline )
      setTermDoubleUnderline(term);

    if ( next->attr.bit.pc_charset )
      setTermPCcharset(term);

    if ( colorChange(term, next) )
    {
      change_color(term, next);

      if ( cygwin_terminal )
      {
        if ( next->attr.bit.bold )
          setTermBold(term);

        if ( next->attr.bit.reverse )
          setTermReverse(term);

        if ( next->attr.bit.standout )
          setTermStandout(term);
      }
    }
  }
  else
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

    detectSwitchOff (term, next);

    if ( switchOff() )
      unsetTermAttributes(term);

    if ( colorChange(term, next) )
      change_color (term, next);

    detectSwitchOn (term, next);

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

  if ( term && fake_reverse )
    term->attr.bit.reverse = true;

  return attr_buf;
}


// private methods of FOptiAttr
//----------------------------------------------------------------------
inline bool FOptiAttr::setTermBold (char_data*& term)
{
  if ( term && append_sequence(F_enter_bold_mode.cap) )
    return (term->attr.bit.bold = true );
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermBold (char_data*& term)
{
  // Back to normal intensity (turns off bold + dim)
  if ( append_sequence(F_exit_bold_mode.cap) )
  {
    if ( F_exit_bold_mode.caused_reset )
      reset(term);
    else
    {
      term->attr.bit.bold = false;
      term->attr.bit.dim = false;
    }

    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermDim (char_data*& term)
{
  if ( term && append_sequence(F_enter_dim_mode.cap) )
    return (term->attr.bit.dim = true);
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermDim (char_data*& term)
{
  // Back to normal intensity (turns off bold + dim)
  if ( term && append_sequence(F_exit_dim_mode.cap) )
  {
    if ( F_exit_dim_mode.caused_reset )
      reset(term);
    else
    {
      term->attr.bit.bold = false;
      term->attr.bit.dim = false;
    }

    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermItalic (char_data*& term)
{
  if ( term && append_sequence(F_enter_italics_mode.cap) )
    return (term->attr.bit.italic = true);
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermItalic (char_data*& term)
{
  if ( term && append_sequence(F_exit_italics_mode.cap) )
  {
    if ( F_exit_italics_mode.caused_reset )
      reset(term);
    else
      term->attr.bit.italic = false;

    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermUnderline (char_data*& term)
{
  if ( term && append_sequence(F_enter_underline_mode.cap) )
    return (term->attr.bit.underline = true);
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermUnderline (char_data*& term)
{
  // Turns off every underlining
  if ( term && append_sequence(F_exit_underline_mode.cap) )
  {
    if ( F_exit_underline_mode.caused_reset )
      reset(term);
    else
    {
      term->attr.bit.underline = false;
      term->attr.bit.dbl_underline = false;
    }

    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermBlink (char_data*& term)
{
  if ( term && append_sequence(F_enter_blink_mode.cap) )
    return (term->attr.bit.blink = true);
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermBlink (char_data*& term)
{
  if ( term && append_sequence(F_exit_blink_mode.cap) )
  {
    if ( F_exit_blink_mode.caused_reset )
      reset(term);
    else
      term->attr.bit.blink = false;

    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermReverse (char_data*& term)
{
  if ( term && append_sequence(F_enter_reverse_mode.cap) )
    return (term->attr.bit.reverse = true);
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermReverse (char_data*& term)
{
  if ( term && append_sequence(F_exit_reverse_mode.cap) )
  {
    if ( F_exit_reverse_mode.caused_reset )
      reset(term);
    else
      term->attr.bit.reverse = false;

    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermStandout (char_data*& term)
{
  if ( term && append_sequence(F_enter_standout_mode.cap) )
    return (term->attr.bit.standout = true);
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermStandout (char_data*& term)
{
  if ( term && append_sequence(F_exit_standout_mode.cap) )
  {
    if ( F_exit_standout_mode.caused_reset )
      reset(term);
    else
      term->attr.bit.standout = false;

    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermInvisible (char_data*& term)
{
  if ( term && append_sequence(F_enter_secure_mode.cap) )
    return (term->attr.bit.invisible = true);
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermInvisible (char_data*& term)
{
  if ( term && append_sequence(F_exit_secure_mode.cap) )
  {
    if ( F_exit_secure_mode.caused_reset )
      reset(term);
    else
      term->attr.bit.invisible = false;

    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermProtected (char_data*& term)
{
  if ( term && append_sequence(F_enter_protected_mode.cap) )
    return (term->attr.bit.protect = true);
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermProtected (char_data*& term)
{
  if ( term && append_sequence(F_exit_protected_mode.cap) )
  {
    if ( F_exit_protected_mode.caused_reset )
      reset(term);
    else
      term->attr.bit.protect = false;

    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermCrossedOut (char_data*& term)
{
  if ( term && append_sequence(F_enter_crossed_out_mode.cap) )
    return (term->attr.bit.crossed_out = true);
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermCrossedOut (char_data*& term)
{
  if ( term && append_sequence(F_exit_crossed_out_mode.cap) )
  {
    if ( F_exit_crossed_out_mode.caused_reset )
      reset(term);
    else
      term->attr.bit.crossed_out = false;

    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermDoubleUnderline (char_data*& term)
{
  if ( term && append_sequence(F_enter_dbl_underline_mode.cap) )
    return (term->attr.bit.dbl_underline = true);
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermDoubleUnderline (char_data*& term)
{
  // Turns off every underlining
  if ( term && append_sequence(F_exit_dbl_underline_mode.cap) )
  {
    if ( F_exit_dbl_underline_mode.caused_reset )
      reset(term);
    else
    {
      term->attr.bit.underline = false;
      term->attr.bit.dbl_underline = false;
    }

    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
bool FOptiAttr::setTermAttributes ( char_data*& term
                                  , bool p1, bool p2, bool p3
                                  , bool p4, bool p5, bool p6
                                  , bool p7, bool p8, bool p9 )
{
  if ( term && F_set_attributes.cap )
  {
    char* sgr = tparm ( F_set_attributes.cap
                      , p1, p2, p3, p4, p5, p6, p7, p8, p9 );

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
inline bool FOptiAttr::unsetTermAttributes (char_data*& term)
{
  if ( term && replace_sequence(F_exit_attribute_mode.cap) )
  {
    reset(term);
    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermAltCharset (char_data*& term)
{
  if ( term && append_sequence(F_enter_alt_charset_mode.cap) )
  {
    term->attr.bit.alt_charset = true;
    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermAltCharset (char_data*& term)
{
  if ( term && append_sequence(F_exit_alt_charset_mode.cap) )
  {
    term->attr.bit.alt_charset = false;
    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermPCcharset (char_data*& term)
{
  if ( term && append_sequence(F_enter_pc_charset_mode.cap) )
  {
    term->attr.bit.pc_charset = true;
    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermPCcharset (char_data*& term)
{
  if ( term && append_sequence(F_exit_pc_charset_mode.cap) )
  {
    term->attr.bit.pc_charset = false;
    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
bool FOptiAttr::setTermDefaultColor (char_data*& term)
{
  if ( ! term )
    return false;

  if ( append_sequence(F_orig_pair.cap) )
  {
    term->fg_color = Default;
    term->bg_color = Default;
    return true;
  }
  else if ( append_sequence(F_orig_colors.cap) )
  {
    term->fg_color = Default;
    term->bg_color = Default;
    return true;
  }
  else if ( ansi_default_color )
  {
    char* sgr_39_49 = const_cast<char*>(CSI "39;49m");
    append_sequence (sgr_39_49);
    term->fg_color = Default;
    term->bg_color = Default;
    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
bool FOptiAttr::hasColor (char_data*& attr)
{
  if ( attr && attr->fg_color < 0 && attr->bg_color < 0 )
    return false;
  else
    return true;
}

//----------------------------------------------------------------------
bool FOptiAttr::hasAttribute (char_data*& attr)
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
bool FOptiAttr::hasNoAttribute (char_data*& attr)
{ return ! hasAttribute(attr); }

//----------------------------------------------------------------------
inline bool FOptiAttr::colorChange (char_data*& term, char_data*& next)
{
  if ( term && next )
  {
    return bool ( fake_reverse
                 || term->fg_color != next->fg_color
                 || term->bg_color != next->bg_color );
  }

  return false;
}

//----------------------------------------------------------------------
inline void FOptiAttr::resetColor (char_data*& attr)
{
  if ( attr )
  {
    attr->fg_color = Default;
    attr->bg_color = Default;
  }
}

//----------------------------------------------------------------------
inline void FOptiAttr::prevent_no_color_video_attributes (char_data*& attr)
{
  // ignore attributes which can not combined with a color

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

  if ( ! attr )
    return;

  if ( hasColor(attr) && attr_without_color > 0 )
  {
    for (int bit = 1; bit < no_mode; bit <<= 1)
    {
      switch ( bit & attr_without_color )
      {
        case standout_mode:
          if ( attr->attr.bit.standout )
            attr->attr.bit.standout = false;
          break;

        case underline_mode:
          if ( attr->attr.bit.underline )
            attr->attr.bit.underline = false;
          break;

        case reverse_mode:
          if ( attr->attr.bit.reverse )
          {
            attr->attr.bit.reverse = false;

            if ( attr->fg_color != attr->bg_color )
              fake_reverse = true;
          }
          break;

        case blink_mode:
          if ( attr->attr.bit.blink )
            attr->attr.bit.blink = false;
          break;

        case dim_mode:
          if ( attr->attr.bit.dim )
            attr->attr.bit.dim = false;
          break;

        case bold_mode:
          if ( attr->attr.bit.bold )
            attr->attr.bit.bold = false;
          break;

        case invisible_mode:
          if ( attr->attr.bit.invisible )
            attr->attr.bit.invisible = false;
          break;

        case protected_mode:
          if ( attr->attr.bit.protect )
            attr->attr.bit.protect = false;
          break;

        case alt_charset_mode:
          if ( attr->attr.bit.alt_charset )
            attr->attr.bit.alt_charset = false;
          break;

        case italic_mode:
          if ( attr->attr.bit.italic )
            attr->attr.bit.italic = false;
          break;

        default:
          break;
      }
    }
  }
}

//----------------------------------------------------------------------
void FOptiAttr::change_color (char_data*& term, char_data*& next)
{
  char* color_str;
  char* AF = F_set_a_foreground.cap;
  char* AB = F_set_a_background.cap;
  char* Sf = F_set_foreground.cap;
  char* Sb = F_set_background.cap;
  char* sp = F_set_color_pair.cap;
  short fg, bg;

  if ( monochron || ! (term && next) )
    return;

  fg = next->fg_color;
  bg = next->bg_color;

  if ( fg == Default || bg == Default )
  {
    if ( ansi_default_color )
    {
      if ( fg == Default && term->fg_color != Default
          && bg == Default && term->bg_color != Default )
      {
        setTermDefaultColor(term);
      }
      else if ( fg == Default && term->fg_color != Default )
      {
        char* sgr_39 = const_cast<char*>(CSI "39m");
        append_sequence (sgr_39);
        term->fg_color = Default;
      }
      else if ( bg == Default && term->bg_color != Default )
      {
        char* sgr_49;
        char* op = F_orig_pair.cap;

        if ( op && std::strncmp (op, CSI "39;49;25m", 11) == 0 )
          sgr_49 = const_cast<char*>(CSI "49;25m");
        else
          sgr_49 = const_cast<char*>(CSI "49m");

        append_sequence (sgr_49);
        term->bg_color = Default;
      }
    }
    else if ( ! setTermDefaultColor(term) )
    {
      // fallback to gray on black
      fg = next->fg_color = LightGray;
      bg = next->bg_color = Black;
    }
  }

  if ( ! fake_reverse && fg < 0 && bg < 0 )
    return;

  if ( fake_reverse )
  {
    std::swap (fg, bg);

    if ( fg == Default || bg == Default )
      setTermDefaultColor(term);
  }

  if ( AF && AB )
  {
    short ansi_fg = vga2ansi(fg);
    short ansi_bg = vga2ansi(bg);

    if ( term->fg_color != fg
        && (color_str = tparm(AF, ansi_fg, 0, 0, 0, 0, 0, 0, 0, 0)) )
      append_sequence (color_str);

    if ( term->bg_color != bg
        && (color_str = tparm(AB, ansi_bg, 0, 0, 0, 0, 0, 0, 0, 0)) )
      append_sequence (color_str);
  }
  else if ( Sf && Sb )
  {
    if ( term->fg_color != fg
        && (color_str = tparm(Sf, fg, 0, 0, 0, 0, 0, 0, 0, 0)) )
      append_sequence (color_str);

    if ( term->bg_color != bg
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

  term->fg_color = next->fg_color;
  term->bg_color = next->bg_color;
}

//----------------------------------------------------------------------
inline void FOptiAttr::resetAttribute (char_data*& attr)
{
  if ( attr )
  {
    attr->attr.bit.bold          = \
    attr->attr.bit.dim           = \
    attr->attr.bit.italic        = \
    attr->attr.bit.underline     = \
    attr->attr.bit.blink         = \
    attr->attr.bit.reverse       = \
    attr->attr.bit.standout      = \
    attr->attr.bit.invisible     = \
    attr->attr.bit.protect       = \
    attr->attr.bit.crossed_out   = \
    attr->attr.bit.dbl_underline = \
    attr->attr.bit.alt_charset   = \
    attr->attr.bit.pc_charset    = 0;
  }
}

//----------------------------------------------------------------------
inline void FOptiAttr::reset (char_data*& attr)
{
  if ( attr )
  {
    resetAttribute(attr);
    resetColor(attr);
  }
}

//----------------------------------------------------------------------
bool FOptiAttr::caused_reset_attributes (char*& cap, uChar test)
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

    if ( (test & same_like_ue) && ue && std::strcmp (cap, ue) == 0 )
      return true;

    if ( (test & same_like_se) && se && std::strcmp (cap, se) == 0 )
      return true;

    if ( (test & same_like_me) && me && std::strcmp (cap, me) == 0 )
      return true;
  }

  return false;
}

//----------------------------------------------------------------------
inline void FOptiAttr::detectSwitchOn (char_data*& term, char_data*& next)
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
inline void FOptiAttr::detectSwitchOff (char_data*& term, char_data*& next)
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
  char_data* on_ptr = &on;
  return hasAttribute(on_ptr);
}

//----------------------------------------------------------------------
inline bool FOptiAttr::switchOff()
{
  char_data* off_ptr = &off;
  return hasAttribute(off_ptr);
}

//----------------------------------------------------------------------
inline bool FOptiAttr::append_sequence (char*& seq)
{
  if ( seq )
  {
    std::strncat (attr_ptr, seq, sizeof(attr_buf) - std::strlen(attr_ptr) - 1 );
    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::replace_sequence (char*& seq)
{
  if ( seq )
  {
    std::strncpy (attr_ptr, seq, sizeof(attr_buf) - 1);
    return true;
  }
  else
    return false;
}
