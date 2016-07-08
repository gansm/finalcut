// File: foptiattr.cpp
// Provides: class FOptiAttr

#include "foptiattr.h"
#include "string.h"

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
  , max_color(1)
  , attr_without_color(0)
  , ansi_default_color(false)
  , monochron(true)
  , fake_reverse(false)
  , cygwin_terminal(false)
  , attr_ptr(attr_buf)
  , on()
  , off()
{
  attr_buf[0] = '\0';
}

//----------------------------------------------------------------------
FOptiAttr::~FOptiAttr() // destructor
{ }


// private methods of FApplication
//----------------------------------------------------------------------
inline bool FOptiAttr::hasColor (char_data*& attr)
{
  if ( attr && attr->fg_color < 0 && attr->bg_color < 0 )
    return false;
  else
    return true;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::colorChange (char_data*& term, char_data*& next)
{
  return bool (  fake_reverse
              || term->fg_color != next->fg_color
              || term->bg_color != next->bg_color );
}

//----------------------------------------------------------------------
inline void FOptiAttr::resetColor (char_data*& attr)
{
  attr->fg_color = Default;
  attr->bg_color = Default;
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

  if ( hasColor(attr) && attr_without_color > 0 )
  {
    for (int bit=1; bit < no_mode; bit <<= 1)
    {
      switch ( bit & attr_without_color )
      {
        case standout_mode:
          if ( attr->standout )
            attr->standout = false;
          break;

        case underline_mode:
          if ( attr->underline )
            attr->underline = false;
          break;

        case reverse_mode:
          if ( attr->reverse )
          {
            attr->reverse = false;

            if ( attr->fg_color != attr->bg_color )
              fake_reverse = true;
          }
          break;

        case blink_mode:
          if ( attr->blink )
            attr->blink = false;
          break;

        case dim_mode:
          if ( attr->dim )
            attr->dim = false;
          break;

        case bold_mode:
          if ( attr->bold )
            attr->bold = false;
          break;

        case invisible_mode:
          if ( attr->invisible )
            attr->invisible = false;
          break;

        case protected_mode:
          if ( attr->protect )
            attr->protect = false;
          break;

        case alt_charset_mode:
          if ( attr->alt_charset )
            attr->alt_charset = false;
          break;

        case italic_mode:
          if ( attr->italic )
            attr->italic = false;
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
  short fg = next->fg_color;
  short bg = next->bg_color;

  if ( monochron )
    return;

  if ( fg == Default || bg == Default )
  {
    if ( ansi_default_color )
    {
      if (  fg == Default && term->fg_color != Default
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

        if ( op && strncmp (op, CSI "39;49;25m", 11) == 0 )
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

    if ( cygwin_terminal )
    {
      // reset blink and bold mode from colors > 7
      char* rst = const_cast<char*>(CSI "m");
      append_sequence (rst);
      reset(term);

      if ( ansi_fg != Default )
      {
        color_str = tparm(AF, ansi_fg);

        if ( color_str )
          append_sequence (color_str);
      }

      if ( ansi_bg != Default )
      {
        color_str = tparm(AB, ansi_bg);
        if ( color_str )
          append_sequence (color_str);
      }
    }
    else
    {
      if ( term->fg_color != fg && (color_str = tparm(AF, ansi_fg)) )
        append_sequence (color_str);

      if ( term->bg_color != bg && (color_str = tparm(AB, ansi_bg)) )
        append_sequence (color_str);
    }
  }
  else if ( Sf && Sb )
  {
    if ( term->fg_color != fg && (color_str = tparm(Sf, fg)) )
      append_sequence (color_str);

    if ( term->bg_color != bg && (color_str = tparm(Sb, bg)) )
      append_sequence (color_str);
  }
  else if ( sp )
  {
    fg = vga2ansi(fg);
    bg = vga2ansi(bg);

    if ( (color_str = tparm(sp, fg, bg)) )
      append_sequence (color_str);
  }

  term->fg_color = next->fg_color;
  term->bg_color = next->bg_color;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::hasAttribute (char_data*& attr)
{
  return attr->bold          == true
      || attr->dim           == true
      || attr->italic        == true
      || attr->underline     == true
      || attr->blink         == true
      || attr->reverse       == true
      || attr->standout      == true
      || attr->invisible     == true
      || attr->protect       == true
      || attr->crossed_out   == true
      || attr->dbl_underline == true
      || attr->alt_charset   == true
      || attr->pc_charset    == true;
}

//----------------------------------------------------------------------
inline void FOptiAttr::resetAttribute (char_data*& attr)
{
  attr->bold          = \
  attr->dim           = \
  attr->italic        = \
  attr->underline     = \
  attr->blink         = \
  attr->reverse       = \
  attr->standout      = \
  attr->invisible     = \
  attr->protect       = \
  attr->crossed_out   = \
  attr->dbl_underline = \
  attr->alt_charset   = \
  attr->pc_charset    = false;
}

//----------------------------------------------------------------------
inline void FOptiAttr::reset (char_data*& attr)
{
  resetAttribute(attr);
  resetColor(attr);
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
    if ( (test & test_ansi_reset) && strncmp (cap, CSI "m", 3) == 0 )
      return true;

    if ( (test & test_adm3_reset) && strncmp (cap, ESC "G0", 3) == 0 )
      return true;

    if ( (test & same_like_ue) && ue && strcmp (cap, ue) == 0 )
      return true;

    if ( (test & same_like_se) && se && strcmp (cap, se) == 0 )
      return true;

    if ( (test & same_like_me) && me && strcmp (cap, me) == 0 )
      return true;
  }

  return false;
}

//----------------------------------------------------------------------
inline void FOptiAttr::detectSwitchOn (char_data*& term, char_data*& next)
{
  on.bold          = ! term->bold          && next->bold;
  on.dim           = ! term->dim           && next->dim;
  on.italic        = ! term->italic        && next->italic;
  on.underline     = ! term->underline     && next->underline;
  on.blink         = ! term->blink         && next->blink;
  on.reverse       = ! term->reverse       && next->reverse;
  on.standout      = ! term->standout      && next->standout;
  on.invisible     = ! term->invisible     && next->invisible;
  on.protect       = ! term->protect       && next->protect;
  on.crossed_out   = ! term->crossed_out   && next->crossed_out;
  on.dbl_underline = ! term->dbl_underline && next->dbl_underline;
  on.alt_charset   = ! term->alt_charset   && next->alt_charset;
  on.pc_charset    = ! term->pc_charset    && next->pc_charset;
}

//----------------------------------------------------------------------
inline void FOptiAttr::detectSwitchOff (char_data*& term, char_data*& next)
{
  off.bold          = term->bold          && ! next->bold;
  off.dim           = term->dim           && ! next->dim;
  off.italic        = term->italic        && ! next->italic;
  off.underline     = term->underline     && ! next->underline;
  off.blink         = term->blink         && ! next->blink;
  off.reverse       = term->reverse       && ! next->reverse;
  off.standout      = term->standout      && ! next->standout;
  off.invisible     = term->invisible     && ! next->invisible;
  off.protect       = term->protect       && ! next->protect;
  off.crossed_out   = term->crossed_out   && ! next->crossed_out;
  off.dbl_underline = term->dbl_underline && ! next->dbl_underline;
  off.alt_charset   = term->alt_charset   && ! next->alt_charset;
  off.pc_charset    = term->pc_charset    && ! next->pc_charset;
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
    strncat (attr_ptr, seq, sizeof(attr_buf) - strlen(attr_ptr) - 1 );
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
    strncpy (attr_ptr, seq, sizeof(attr_buf) - 1);
    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermBold (char_data*& term)
{
  if ( append_sequence(F_enter_bold_mode.cap) )
    return (term->bold = true);
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
      term->bold = false;
      term->dim = false;
    }

    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermDim (char_data*& term)
{
  if ( append_sequence(F_enter_dim_mode.cap) )
    return (term->dim = true);
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermDim (char_data*& term)
{
  // Back to normal intensity (turns off bold + dim)
  if ( append_sequence(F_exit_dim_mode.cap) )
  {
    if ( F_exit_dim_mode.caused_reset )
      reset(term);
    else
    {
      term->bold = false;
      term->dim = false;
    }

    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermItalic (char_data*& term)
{
  if ( append_sequence(F_enter_italics_mode.cap) )
    return (term->italic = true);
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermItalic (char_data*& term)
{
  if ( append_sequence(F_exit_italics_mode.cap) )
  {
    if ( F_exit_italics_mode.caused_reset )
      reset(term);
    else
      term->italic = false;

    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermUnderline (char_data*& term)
{
  if ( append_sequence(F_enter_underline_mode.cap) )
    return (term->underline = true);
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermUnderline (char_data*& term)
{
  // Turns off every underlining
  if ( append_sequence(F_exit_underline_mode.cap) )
  {
    if ( F_exit_underline_mode.caused_reset )
      reset(term);
    else
    {
      term->underline = false;
      term->dbl_underline = false;
    }

    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermBlink (char_data*& term)
{
  if ( append_sequence(F_enter_blink_mode.cap) )
    return (term->blink = true);
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermBlink (char_data*& term)
{
  if ( append_sequence(F_exit_blink_mode.cap) )
  {
    if ( F_exit_blink_mode.caused_reset )
      reset(term);
    else
      term->blink = false;

    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermReverse (char_data*& term)
{
  if ( append_sequence(F_enter_reverse_mode.cap) )
    return (term->reverse = true);
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermReverse (char_data*& term)
{
  if ( append_sequence(F_exit_reverse_mode.cap) )
  {
    if ( F_exit_reverse_mode.caused_reset )
      reset(term);
    else
      term->reverse = false;

    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermStandout (char_data*& term)
{
  if ( append_sequence(F_enter_standout_mode.cap) )
    return (term->standout = true);
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermStandout (char_data*& term)
{
  if ( append_sequence(F_exit_standout_mode.cap) )
  {
    if ( F_exit_standout_mode.caused_reset )
      reset(term);
    else
      term->standout = false;

    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermInvisible (char_data*& term)
{
  if ( append_sequence(F_enter_secure_mode.cap) )
    return (term->invisible = true);
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermInvisible (char_data*& term)
{
  if ( append_sequence(F_exit_secure_mode.cap) )
  {
    if ( F_exit_secure_mode.caused_reset )
      reset(term);
    else
      term->invisible = false;

    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermProtected (char_data*& term)
{
  if ( append_sequence(F_enter_protected_mode.cap) )
    return (term->protect = true);
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermProtected (char_data*& term)
{
  if ( append_sequence(F_exit_protected_mode.cap) )
  {
    if ( F_exit_protected_mode.caused_reset )
      reset(term);
    else
      term->protect = false;

    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermCrossedOut (char_data*& term)
{
  if ( append_sequence(F_enter_crossed_out_mode.cap) )
    return (term->crossed_out = true);
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermCrossedOut (char_data*& term)
{
  if ( append_sequence(F_exit_crossed_out_mode.cap) )
  {
    if ( F_exit_crossed_out_mode.caused_reset )
      reset(term);
    else
      term->crossed_out = false;

    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermDoubleUnderline (char_data*& term)
{
  if ( append_sequence(F_enter_dbl_underline_mode.cap) )
    return (term->dbl_underline = true);
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermDoubleUnderline (char_data*& term)
{
  // Turns off every underlining
  if ( append_sequence(F_exit_dbl_underline_mode.cap) )
  {
    if ( F_exit_dbl_underline_mode.caused_reset )
      reset(term);
    else
    {
      term->underline = false;
      term->dbl_underline = false;
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
  if ( F_set_attributes.cap )
  {
    char* sgr = tparm ( F_set_attributes.cap
                      , p1, p2, p3, p4, p5, p6, p7, p8, p9 );

    append_sequence (sgr);

    resetColor(term);
    term->standout      = p1;
    term->underline     = p2;
    term->reverse       = p3;
    term->blink         = p4;
    term->dim           = p5;
    term->bold          = p6;
    term->invisible     = p7;
    term->protect       = p8;
    term->alt_charset   = p9;
    term->pc_charset    = false;
    term->italic        = false;
    term->crossed_out   = false;
    term->dbl_underline = false;

    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermAttributes (char_data*& term)
{
  if ( replace_sequence(F_exit_attribute_mode.cap) )
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
  if ( append_sequence(F_enter_alt_charset_mode.cap) )
  {
    term->alt_charset = true;
    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermAltCharset (char_data*& term)
{
  if ( append_sequence(F_exit_alt_charset_mode.cap) )
  {
    term->alt_charset = false;
    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::setTermPCcharset (char_data*& term)
{
  if ( append_sequence(F_enter_pc_charset_mode.cap) )
  {
    term->pc_charset = true;
    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FOptiAttr::unsetTermPCcharset (char_data*& term)
{
  if ( append_sequence(F_exit_pc_charset_mode.cap) )
  {
    term->pc_charset = false;
    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
bool FOptiAttr::setTermDefaultColor (char_data*& term)
{
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


// public methods of FOptiAttr
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
char* FOptiAttr::change_attribute (char_data*& term, char_data*& next)
{
  fake_reverse = false;
  attr_buf[0] = '\0';

  prevent_no_color_video_attributes (next);

  detectSwitchOn (term, next);
  detectSwitchOff (term, next);

  // look for no changes
  if ( ! ( switchOn() || switchOff() || colorChange(term, next) ) )
    return 0;

  if ( hasNoAttribute(next) )
  {
    if ( off.pc_charset )
      unsetTermPCcharset(term);

    if ( off.alt_charset )
      unsetTermAltCharset(term);

    if ( hasAttribute(term) )
    {
      if ( F_exit_attribute_mode.cap )
        unsetTermAttributes(term);
      else
      {
        if ( off.bold )
          unsetTermBold(term);

        if ( off.dim )
          unsetTermDim(term);

        if ( off.italic )
          unsetTermItalic(term);

        if ( off.underline )
          unsetTermUnderline(term);

        if ( off.blink )
          unsetTermBlink(term);

        if ( off.reverse )
          unsetTermReverse(term);

        if ( off.standout )
          unsetTermStandout(term);

        if ( off.invisible )
          unsetTermInvisible(term);

        if ( off.protect )
          unsetTermProtected(term);

        if ( off.crossed_out )
          unsetTermCrossedOut(term);

        if ( off.dbl_underline )
          unsetTermDoubleUnderline(term);
      }
    }

    if ( colorChange(term, next) )
      change_color (term, next);
  }
  else if ( F_set_attributes.cap )
  {
    if ( off.pc_charset )
      unsetTermPCcharset(term);

    if ( switchOn() || switchOff() )
      setTermAttributes ( term
                        , next->standout
                        , next->underline
                        , next->reverse
                        , next->blink
                        , next->dim
                        , next->bold
                        , next->invisible
                        , next->protect
                        , next->alt_charset );

    if ( next->italic )
      setTermItalic(term);

    if ( next->crossed_out )
      setTermCrossedOut(term);

    if ( next->dbl_underline )
      setTermDoubleUnderline(term);

    if ( next->pc_charset )
      setTermPCcharset(term);

    if ( colorChange(term, next) )
    {
      change_color (term, next);

      if ( cygwin_terminal )
      {
        if ( next->bold )
          setTermBold(term);

        if ( next->reverse )
          setTermReverse(term);

        if ( next->standout )
         setTermStandout(term);
      }
    }
  }
  else
  {
    if ( off.pc_charset )
      unsetTermPCcharset(term);

    if ( off.alt_charset )
      unsetTermAltCharset(term);

    if ( off.bold )
      unsetTermBold(term);

    if ( off.dim )
      unsetTermDim(term);

    if ( off.italic )
      unsetTermItalic(term);

    if ( off.underline )
      unsetTermUnderline(term);

    if ( off.blink )
      unsetTermBlink(term);

    if ( off.reverse )
      unsetTermReverse(term);

    if ( off.standout )
      unsetTermStandout(term);

    if ( off.invisible )
      unsetTermInvisible(term);

    if ( off.protect )
      unsetTermProtected(term);

    if ( off.crossed_out )
      unsetTermCrossedOut(term);

    if ( off.dbl_underline )
      unsetTermDoubleUnderline(term);

    detectSwitchOff (term, next);

    if ( switchOff() )
      unsetTermAttributes(term);

    if ( colorChange(term, next) )
      change_color (term, next);

    detectSwitchOn (term, next);

    if ( on.pc_charset )
      setTermPCcharset(term);

    if ( on.alt_charset )
      setTermAltCharset(term);

    if ( on.bold )
      setTermBold(term);

    if ( on.dim )
      setTermDim(term);

    if ( on.italic )
      setTermItalic(term);

    if ( on.underline )
      setTermUnderline(term);

    if ( on.blink )
      setTermBlink(term);

    if ( on.reverse )
      setTermReverse(term);

    if ( on.standout )
      setTermStandout(term);

    if ( on.invisible )
      setTermInvisible(term);

    if ( on.protect )
      setTermProtected(term);

    if ( on.crossed_out )
      setTermCrossedOut(term);

    if ( on.dbl_underline )
      setTermDoubleUnderline(term);
  }

  if ( fake_reverse )
    term->reverse = true;

  return attr_buf;
}
