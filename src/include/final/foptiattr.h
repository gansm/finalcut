/***********************************************************************
* foptiattr.h - Sets video attributes in optimized order               *
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

/*  Standalone class
 *  ════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FOptiAttr ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FOPTIATTR_H
#define FOPTIATTR_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

// Typecast to c-string
#define C_STR const_cast<char*>

#include <assert.h>

#if defined(__sun) && defined(__SVR4)
  #include <termio.h>
  typedef struct termio SGTTY;
  typedef struct termios SGTTYS;

  #ifdef _LP64
    typedef unsigned int chtype;
  #else
    typedef unsigned long chtype;
  #endif

  #include <term.h>   // need for tparm
#else
  #include <term.h>   // need for tparm
#endif

#include <algorithm>  // need for std::swap

#include "final/fc.h"
#include "final/ftypes.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FOptiAttr
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FOptiAttr
{
  public:
    // Typedefs
    typedef struct
    {
      int    code;      // character code
      FColor fg_color;  // foreground color
      FColor bg_color;  // background color

      union attribute
      {
        struct
        {
          // Attribute byte #0
          uInt8 bold          : 1;  // bold
          uInt8 dim           : 1;  // dim
          uInt8 italic        : 1;  // italic
          uInt8 underline     : 1;  // underline
          uInt8 blink         : 1;  // blink
          uInt8 reverse       : 1;  // reverse
          uInt8 standout      : 1;  // standout
          uInt8 invisible     : 1;  // invisible
          // Attribute byte #1
          uInt8 protect       : 1;  // protect mode
          uInt8 crossed_out   : 1;  // crossed out
          uInt8 dbl_underline : 1;  // double underline
          uInt8 alt_charset   : 1;  // alternate character set (vt100)
          uInt8 pc_charset    : 1;  // pc character set (CP437)
          uInt8 transparent   : 1;  // transparent
          uInt8 trans_shadow  : 1;  // transparent shadow
          uInt8 inherit_bg    : 1;  // inherit background
          // Attribute byte #2
          uInt8 no_changes    : 1;  // no changes required
          uInt8 printed       : 1;  // is printed to VTerm
          uInt8               : 6;  // padding bits
        } bit;

        uInt8 byte[3];
      } attr;
    } charData;

    typedef struct
    {
      bool  ansi_default_color;
      int   max_color;
      int   attr_without_color;
      char* t_enter_bold_mode;
      char* t_exit_bold_mode;
      char* t_enter_dim_mode;
      char* t_exit_dim_mode;
      char* t_enter_italics_mode;
      char* t_exit_italics_mode;
      char* t_enter_underline_mode;
      char* t_exit_underline_mode;
      char* t_enter_blink_mode;
      char* t_exit_blink_mode;
      char* t_enter_reverse_mode;
      char* t_exit_reverse_mode;
      char* t_enter_standout_mode;
      char* t_exit_standout_mode;
      char* t_enter_secure_mode;
      char* t_exit_secure_mode;
      char* t_enter_protected_mode;
      char* t_exit_protected_mode;
      char* t_enter_crossed_out_mode;
      char* t_exit_crossed_out_mode;
      char* t_enter_dbl_underline_mode;
      char* t_exit_dbl_underline_mode;
      char* t_set_attributes;
      char* t_exit_attribute_mode;
      char* t_enter_alt_charset_mode;
      char* t_exit_alt_charset_mode;
      char* t_enter_pc_charset_mode;
      char* t_exit_pc_charset_mode;
      char* t_set_a_foreground;
      char* t_set_a_background;
      char* t_set_foreground;
      char* t_set_background;
      char* t_set_color_pair;
      char* t_orig_pair;
      char* t_orig_colors;
    } termEnv;

    // Constructor
    FOptiAttr();

    // Destructor
    virtual ~FOptiAttr();

    // Friend operator functions
    friend bool operator == (const charData&, const charData&);
    friend bool operator != (const charData&, const charData&);

    // Accessors
    const char* getClassName() const;

    // Mutators
    void  setTermEnvironment (termEnv&);
    void  setMaxColor (const int&);
    void  setNoColorVideo (int);
    void  setDefaultColorSupport();
    void  unsetDefaultColorSupport();
    void  set_enter_bold_mode (char[]);
    void  set_exit_bold_mode (char[]);
    void  set_enter_dim_mode (char[]);
    void  set_exit_dim_mode (char[]);
    void  set_enter_italics_mode (char[]);
    void  set_exit_italics_mode (char[]);
    void  set_enter_underline_mode (char[]);
    void  set_exit_underline_mode (char[]);
    void  set_enter_blink_mode (char[]);
    void  set_exit_blink_mode (char[]);
    void  set_enter_reverse_mode (char[]);
    void  set_exit_reverse_mode (char[]);
    void  set_enter_secure_mode (char[]);
    void  set_exit_secure_mode (char[]);
    void  set_enter_protected_mode (char[]);
    void  set_exit_protected_mode (char[]);
    void  set_enter_crossed_out_mode (char[]);
    void  set_exit_crossed_out_mode (char[]);
    void  set_enter_dbl_underline_mode (char[]);
    void  set_exit_dbl_underline_mode (char[]);
    void  set_enter_standout_mode (char[]);
    void  set_exit_standout_mode (char[]);
    void  set_set_attributes (char[]);
    void  set_exit_attribute_mode (char[]);
    void  set_enter_alt_charset_mode (char[]);
    void  set_exit_alt_charset_mode (char[]);
    void  set_enter_pc_charset_mode (char[]);
    void  set_exit_pc_charset_mode (char[]);
    void  set_a_foreground_color (char[]);
    void  set_a_background_color (char[]);
    void  set_foreground_color (char[]);
    void  set_background_color (char[]);
    void  set_term_color_pair (char[]);
    void  set_orig_pair (char[]);
    void  set_orig_orig_colors (char[]);

    // Inquiry
    static bool   isNormal (charData*&);

    // Methods
    void          initialize();
    static FColor vga2ansi (FColor);
    char*         changeAttribute (charData*&, charData*&);

  private:
    // Typedefs and Enumerations
    typedef struct
    {
      char* cap;
      bool  caused_reset;
    } capability;

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

    // Disable copy constructor
    FOptiAttr (const FOptiAttr&);

    // Disable assignment operator (=)
    FOptiAttr& operator = (const FOptiAttr&);

    // Mutators
    bool  setTermBold (charData*&);
    bool  unsetTermBold (charData*&);
    bool  setTermDim (charData*&);
    bool  unsetTermDim (charData*&);
    bool  setTermItalic (charData*&);
    bool  unsetTermItalic (charData*&);
    bool  setTermUnderline (charData*&);
    bool  unsetTermUnderline (charData*&);
    bool  setTermBlink (charData*&);
    bool  unsetTermBlink (charData*&);
    bool  setTermReverse (charData*&);
    bool  unsetTermReverse (charData*&);
    bool  setTermStandout (charData*&);
    bool  unsetTermStandout (charData*&);
    bool  setTermInvisible (charData*&);
    bool  unsetTermInvisible (charData*&);
    bool  setTermProtected (charData*&);
    bool  unsetTermProtected (charData*&);
    bool  setTermCrossedOut (charData*&);
    bool  unsetTermCrossedOut (charData*&);
    bool  setTermDoubleUnderline (charData*&);
    bool  unsetTermDoubleUnderline (charData*&);
    bool  setTermAttributes ( charData*&
                            , bool, bool, bool
                            , bool, bool, bool
                            , bool, bool, bool );
    bool  unsetTermAttributes (charData*&);
    bool  setTermAltCharset (charData*&);
    bool  unsetTermAltCharset (charData*&);
    bool  setTermPCcharset (charData*&);
    bool  unsetTermPCcharset (charData*&);
    bool  setTermDefaultColor (charData*&);
    void  setAttributesOn (charData*&);
    void  setAttributesOff (charData*&);

    // Inquiries
    static bool  hasColor (charData*&);
    static bool  hasAttribute (charData*&);
    static bool  hasNoAttribute (charData*&);

    // Methods
    bool  hasColorChanged (charData*&, charData*&);
    void  resetColor (charData*&);
    void  prevent_no_color_video_attributes (charData*&, bool = false);
    void  deactivateAttributes (charData*&, charData*&);
    void  changeAttributeSGR (charData*&, charData*&);
    void  changeAttributeSeparately (charData*&, charData*&);
    void  change_color (charData*&, charData*&);
    void  change_to_default_color (charData*&, charData*&, FColor&, FColor&);
    void  change_current_color (charData*&, FColor, FColor);
    void  resetAttribute (charData*&);
    void  reset (charData*&);
    bool  caused_reset_attributes (char[], uChar = all_tests);
    bool  hasCharsetEquivalence();
    void  detectSwitchOn (charData*&, charData*&);
    void  detectSwitchOff (charData*&, charData*&);
    bool  switchOn();
    bool  switchOff();
    bool  append_sequence (char[]);

    // Data Members
    capability F_enter_bold_mode;
    capability F_exit_bold_mode;
    capability F_enter_dim_mode;
    capability F_exit_dim_mode;
    capability F_enter_italics_mode;
    capability F_exit_italics_mode;
    capability F_enter_underline_mode;
    capability F_exit_underline_mode;
    capability F_enter_blink_mode;
    capability F_exit_blink_mode;
    capability F_enter_reverse_mode;
    capability F_exit_reverse_mode;
    capability F_enter_standout_mode;
    capability F_exit_standout_mode;
    capability F_enter_secure_mode;
    capability F_exit_secure_mode;
    capability F_enter_protected_mode;
    capability F_exit_protected_mode;
    capability F_enter_crossed_out_mode;
    capability F_exit_crossed_out_mode;
    capability F_enter_dbl_underline_mode;
    capability F_exit_dbl_underline_mode;
    capability F_set_attributes;
    capability F_exit_attribute_mode;
    capability F_enter_alt_charset_mode;
    capability F_exit_alt_charset_mode;
    capability F_enter_pc_charset_mode;
    capability F_exit_pc_charset_mode;
    capability F_set_a_foreground;
    capability F_set_a_background;
    capability F_set_foreground;
    capability F_set_background;
    capability F_set_color_pair;
    capability F_orig_pair;
    capability F_orig_colors;

    charData   on;
    charData   off;
    charData   reset_byte_mask;

    int        max_color;
    int        attr_without_color;
    bool       ansi_default_color;
    bool       alt_equal_pc_charset;
    bool       monochron;
    bool       fake_reverse;
    char       attr_buf[8192];
    char*      attr_ptr;
};
#pragma pack(pop)


// FOptiAttr inline functions
//----------------------------------------------------------------------
inline bool operator == ( const FOptiAttr::charData& lhs,
                          const FOptiAttr::charData& rhs )
{
  return lhs.code         == rhs.code
      && lhs.fg_color     == rhs.fg_color
      && lhs.bg_color     == rhs.bg_color
      && lhs.attr.byte[0] == rhs.attr.byte[0]
      && lhs.attr.byte[1] == rhs.attr.byte[1];
}

//----------------------------------------------------------------------
inline bool operator != ( const FOptiAttr::charData& lhs,
                          const FOptiAttr::charData& rhs )
{ return ! ( lhs == rhs ); }

//----------------------------------------------------------------------
inline const char* FOptiAttr::getClassName() const
{ return "FOptiAttr"; }

//----------------------------------------------------------------------
inline void FOptiAttr::setMaxColor (const int& c)
{ max_color = c; }

//----------------------------------------------------------------------
inline void FOptiAttr::setNoColorVideo (int attr)
{ attr_without_color = attr; }

//----------------------------------------------------------------------
inline void FOptiAttr::setDefaultColorSupport()
{ ansi_default_color = true; }

//----------------------------------------------------------------------
inline void FOptiAttr::unsetDefaultColorSupport()
{ ansi_default_color = false; }

}  // namespace finalcut

#endif  // FOPTIATTR_H
