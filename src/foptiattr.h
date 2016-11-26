// File: foptiattr.h
// Provides: class FOptiAttr
//
//  Standalone class
//  ════════════════
//
// ▕▔▔▔▔▔▔▔▔▔▔▔▏
// ▕ FOptiAttr ▏
// ▕▁▁▁▁▁▁▁▁▁▁▁▏

#ifndef _FOPTIATTR_H
#define _FOPTIATTR_H


#include <algorithm>  // need for std::swap
#include <assert.h>
#include <term.h>     // need for tparm

// ascii sequences
#define ESC  "\033"   // Escape
#define CSI  ESC "["  // Control sequence introducer (7-bit)

typedef unsigned char uChar;


//----------------------------------------------------------------------
// class FOptiAttr
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FOptiAttr
{
 public:
   // Typedef
   typedef struct
   {
     int   code;      // character code
     short fg_color;  // foreground color
     short bg_color;  // background color
     uChar bold          : 1;  // bold
     uChar dim           : 1;  // dim
     uChar italic        : 1;  // italic
     uChar underline     : 1;  // underline
     uChar blink         : 1;  // blink
     uChar reverse       : 1;  // reverse
     uChar standout      : 1;  // standout
     uChar invisible     : 1;  // invisible
     uChar protect       : 1;  // protect mode
     uChar crossed_out   : 1;  // crossed out
     uChar dbl_underline : 1;  // double underline
     uChar alt_charset   : 1;  // alternate character set (vt100)
     uChar pc_charset    : 1;  // pc character set (CP437)
     uChar transparent   : 1;  // transparent
     uChar trans_shadow  : 1;  // transparent shadow
     uChar inherit_bg    : 1;  // inherit background
   } char_data;

   // Constructor
   explicit FOptiAttr();

   // Destructor
  ~FOptiAttr();

   // Friend operator functions
   friend bool operator == (const char_data&, const char_data&);
   friend bool operator != (const char_data&, const char_data&);

   // Mutators
   void  setMaxColor (int&);
   void  setNoColorVideo (int);
   void  setDefaultColorSupport();
   void  setCygwinTerminal();
   void  set_enter_bold_mode (char*&);
   void  set_exit_bold_mode (char*&);
   void  set_enter_dim_mode (char*&);
   void  set_exit_dim_mode (char*&);
   void  set_enter_italics_mode (char*&);
   void  set_exit_italics_mode (char*&);
   void  set_enter_underline_mode (char*&);
   void  set_exit_underline_mode (char*&);
   void  set_enter_blink_mode (char*&);
   void  set_exit_blink_mode (char*&);
   void  set_enter_reverse_mode (char*&);
   void  set_exit_reverse_mode (char*&);
   void  set_enter_secure_mode (char*&);
   void  set_exit_secure_mode (char*&);
   void  set_enter_protected_mode (char*&);
   void  set_exit_protected_mode (char*&);
   void  set_enter_crossed_out_mode (char*&);
   void  set_exit_crossed_out_mode (char*&);
   void  set_enter_dbl_underline_mode (char*&);
   void  set_exit_dbl_underline_mode (char*&);
   void  set_enter_standout_mode (char*&);
   void  set_exit_standout_mode (char*&);
   void  set_set_attributes (char*&);
   void  set_exit_attribute_mode (char*&);
   void  set_enter_alt_charset_mode (char*&);
   void  set_exit_alt_charset_mode (char*&);
   void  set_enter_pc_charset_mode (char*&);
   void  set_exit_pc_charset_mode (char*&);
   void  set_a_foreground_color (char*&);
   void  set_a_background_color (char*&);
   void  set_foreground_color (char*&);
   void  set_background_color (char*&);
   void  set_term_color_pair (char*&);
   void  set_orig_pair (char*&);
   void  set_orig_orig_colors (char*&);

   // Inquiry
   static bool  isNormal (char_data*&);

   // Methods
   void         init();
   static short vga2ansi (register short);
   char*        changeAttribute (char_data*&, char_data*&);

 private:
   // Typedefs and Enumerations
   typedef unsigned char uChar;

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

   enum default_color
   {
     Default   = -1,
     Black     = 0,
     LightGray = 7
   };

   // Disable copy constructor
   FOptiAttr (const FOptiAttr&);

   // Disable assignment operator (=)
   FOptiAttr& operator = (const FOptiAttr&);

   // Mutators
   bool  setTermBold (char_data*&);
   bool  unsetTermBold (char_data*&);
   bool  setTermDim (char_data*&);
   bool  unsetTermDim (char_data*&);
   bool  setTermItalic (char_data*&);
   bool  unsetTermItalic (char_data*&);
   bool  setTermUnderline (char_data*&);
   bool  unsetTermUnderline (char_data*&);
   bool  setTermBlink (char_data*&);
   bool  unsetTermBlink (char_data*&);
   bool  setTermReverse (char_data*&);
   bool  unsetTermReverse (char_data*&);
   bool  setTermStandout (char_data*&);
   bool  unsetTermStandout (char_data*&);
   bool  setTermInvisible (char_data*&);
   bool  unsetTermInvisible (char_data*&);
   bool  setTermProtected (char_data*&);
   bool  unsetTermProtected (char_data*&);
   bool  setTermCrossedOut (char_data*&);
   bool  unsetTermCrossedOut (char_data*&);
   bool  setTermDoubleUnderline (char_data*&);
   bool  unsetTermDoubleUnderline (char_data*&);
   bool  setTermAttributes ( char_data*&
                           , bool, bool, bool
                           , bool, bool, bool
                           , bool, bool, bool );
   bool  unsetTermAttributes (char_data*&);
   bool  setTermAltCharset (char_data*&);
   bool  unsetTermAltCharset (char_data*&);
   bool  setTermPCcharset (char_data*&);
   bool  unsetTermPCcharset (char_data*&);
   bool  setTermDefaultColor (char_data*&);

   // Inquiries
   static bool  hasColor (char_data*&);
   static bool  hasAttribute (char_data*&);
   static bool  hasNoAttribute (char_data*&);

   // Methods
   bool  colorChange (char_data*&, char_data*&);
   void  resetColor (char_data*&);
   void  prevent_no_color_video_attributes (char_data*&);
   void  change_color (char_data*&, char_data*&);
   void  resetAttribute (char_data*&);
   void  reset (char_data*&);
   bool  caused_reset_attributes (char*&, uChar = all_tests);
   void  detectSwitchOn (char_data*&, char_data*&);
   void  detectSwitchOff (char_data*&, char_data*&);
   bool  switchOn();
   bool  switchOff();
   bool  append_sequence (char*&);
   bool  replace_sequence (char*&);

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

   char_data  on;
   char_data  off;

   int        max_color;
   int        attr_without_color;
   bool       ansi_default_color;
   bool       monochron;
   bool       fake_reverse;
   bool       cygwin_terminal;
   char       attr_buf[8192];
   char*      attr_ptr;
};
#pragma pack(pop)


// FOptiAttr inline functions
//----------------------------------------------------------------------
inline bool operator == ( const FOptiAttr::char_data& lhs,
                          const FOptiAttr::char_data& rhs )
{
  return lhs.code          == rhs.code
      && lhs.fg_color      == rhs.fg_color
      && lhs.bg_color      == rhs.bg_color
      && lhs.bold          == rhs.bold
      && lhs.dim           == rhs.dim
      && lhs.italic        == rhs.italic
      && lhs.underline     == rhs.underline
      && lhs.blink         == rhs.blink
      && lhs.reverse       == rhs.reverse
      && lhs.standout      == rhs.standout
      && lhs.invisible     == rhs.invisible
      && lhs.protect       == rhs.protect
      && lhs.crossed_out   == rhs.crossed_out
      && lhs.dbl_underline == rhs.dbl_underline
      && lhs.alt_charset   == rhs.alt_charset
      && lhs.pc_charset    == rhs.pc_charset
      && lhs.transparent   == rhs.transparent
      && lhs.trans_shadow  == rhs.trans_shadow
      && lhs.inherit_bg    == rhs.inherit_bg;
}

//----------------------------------------------------------------------
inline bool operator != ( const FOptiAttr::char_data& lhs,
                          const FOptiAttr::char_data& rhs )
{ return ! ( lhs == rhs ); }

//----------------------------------------------------------------------
inline void FOptiAttr::setMaxColor (int& c)
{ max_color = c; }

//----------------------------------------------------------------------
inline void FOptiAttr::setNoColorVideo (int attr)
{ attr_without_color = attr; }

//----------------------------------------------------------------------
inline void FOptiAttr::setDefaultColorSupport()
{ ansi_default_color = true; }

//----------------------------------------------------------------------
inline void FOptiAttr::setCygwinTerminal()
{ cygwin_terminal = true; }

#endif  // _FOPTIATTR_H
