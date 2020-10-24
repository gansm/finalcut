/***********************************************************************
* foptiattr.h - Sets video attributes in optimized order               *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2016-2020 Markus Gans                                      *
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

#include <assert.h>
#include <algorithm>  // need for std::swap

#include "final/fstring.h"
#include "final/sgr_optimizer.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FOptiAttr
//----------------------------------------------------------------------

class FOptiAttr final
{
  public:
    // Typedef
    typedef struct
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
    } TermEnv;

    // Constructor
    FOptiAttr();

    // Disable copy constructor
    FOptiAttr (const FOptiAttr&) = delete;

    // Destructor
    ~FOptiAttr();

    // Disable copy assignment operator (=)
    FOptiAttr& operator = (const FOptiAttr&) = delete;

    // Accessors
    FString       getClassName() const;

    // Mutators
    void          setTermEnvironment (const TermEnv&);
    void          setMaxColor (const int&);
    void          setNoColorVideo (int);
    void          setDefaultColorSupport();
    void          unsetDefaultColorSupport();
    void          set_enter_bold_mode (const char[]);
    void          set_exit_bold_mode (const char[]);
    void          set_enter_dim_mode (const char[]);
    void          set_exit_dim_mode (const char[]);
    void          set_enter_italics_mode (const char[]);
    void          set_exit_italics_mode (const char[]);
    void          set_enter_underline_mode (const char[]);
    void          set_exit_underline_mode (const char[]);
    void          set_enter_blink_mode (const char[]);
    void          set_exit_blink_mode (const char[]);
    void          set_enter_reverse_mode (const char[]);
    void          set_exit_reverse_mode (const char[]);
    void          set_enter_secure_mode (const char[]);
    void          set_exit_secure_mode (const char[]);
    void          set_enter_protected_mode (const char[]);
    void          set_exit_protected_mode (const char[]);
    void          set_enter_crossed_out_mode (const char[]);
    void          set_exit_crossed_out_mode (const char[]);
    void          set_enter_dbl_underline_mode (const char[]);
    void          set_exit_dbl_underline_mode (const char[]);
    void          set_enter_standout_mode (const char[]);
    void          set_exit_standout_mode (const char[]);
    void          set_set_attributes (const char[]);
    void          set_exit_attribute_mode (const char[]);
    void          set_enter_alt_charset_mode (const char[]);
    void          set_exit_alt_charset_mode (const char[]);
    void          set_enter_pc_charset_mode (const char[]);
    void          set_exit_pc_charset_mode (const char[]);
    void          set_a_foreground_color (const char[]);
    void          set_a_background_color (const char[]);
    void          set_foreground_color (const char[]);
    void          set_background_color (const char[]);
    void          set_term_color_pair (const char[]);
    void          set_orig_pair (const char[]);
    void          set_orig_orig_colors (const char[]);

    // Inquiry
    static bool   isNormal (const FChar&);

    // Methods
    void          initialize();
    static FColor vga2ansi (FColor);
    const char*   changeAttribute (FChar&, FChar&);

  private:
    // Typedefs and Enumerations
    typedef SGRoptimizer::AttributeBuffer AttributeBuffer;

    typedef struct
    {
      const char* cap;
      bool  caused_reset;
    } Capability;

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
    bool          setTermBold (FChar&);
    bool          unsetTermBold (FChar&);
    bool          setTermDim (FChar&);
    bool          unsetTermDim (FChar&);
    bool          setTermItalic (FChar&);
    bool          unsetTermItalic (FChar&);
    bool          setTermUnderline (FChar&);
    bool          unsetTermUnderline (FChar&);
    bool          setTermBlink (FChar&);
    bool          unsetTermBlink (FChar&);
    bool          setTermReverse (FChar&);
    bool          unsetTermReverse (FChar&);
    bool          setTermStandout (FChar&);
    bool          unsetTermStandout (FChar&);
    bool          setTermInvisible (FChar&);
    bool          unsetTermInvisible (FChar&);
    bool          setTermProtected (FChar&);
    bool          unsetTermProtected (FChar&);
    bool          setTermCrossedOut (FChar&);
    bool          unsetTermCrossedOut (FChar&);
    bool          setTermDoubleUnderline (FChar&);
    bool          unsetTermDoubleUnderline (FChar&);
    bool          setTermAttributes ( FChar&
                                    , bool, bool, bool
                                    , bool, bool, bool
                                    , bool, bool, bool );
    bool          unsetTermAttributes (FChar&);
    bool          setTermAltCharset (FChar&);
    bool          unsetTermAltCharset (FChar&);
    bool          setTermPCcharset (FChar&);
    bool          unsetTermPCcharset (FChar&);
    bool          setTermDefaultColor (FChar&);
    void          setAttributesOn (FChar&);
    void          setAttributesOff (FChar&);

    // Inquiries
    static bool   hasColor (const FChar&);
    static bool   hasAttribute (const FChar&);
    static bool   hasNoAttribute (const FChar&);

    // Methods
    bool          hasColorChanged (const FChar&, const FChar&) const;
    void          resetColor (FChar&) const;
    void          prevent_no_color_video_attributes (FChar&, bool = false);
    void          deactivateAttributes (FChar&, FChar&);
    void          changeAttributeSGR (FChar&, FChar&);
    void          changeAttributeSeparately (FChar&, FChar&);
    void          change_color (FChar&, FChar&);
    void          change_to_default_color (FChar&, FChar&, FColor&, FColor&);
    void          change_current_color (const FChar&, FColor, FColor);
    void          resetAttribute (FChar&) const;
    void          reset (FChar&) const;
    bool          caused_reset_attributes (const char[], uChar = all_tests) const;
    bool          hasCharsetEquivalence() const;
    void          detectSwitchOn (const FChar&, const FChar&);
    void          detectSwitchOff (const FChar&, const FChar&);
    bool          switchOn() const;
    bool          switchOff() const;
    bool          append_sequence (const char[]);

    // Data members
    Capability      F_enter_bold_mode{};
    Capability      F_exit_bold_mode{};
    Capability      F_enter_dim_mode{};
    Capability      F_exit_dim_mode{};
    Capability      F_enter_italics_mode{};
    Capability      F_exit_italics_mode{};
    Capability      F_enter_underline_mode{};
    Capability      F_exit_underline_mode{};
    Capability      F_enter_blink_mode{};
    Capability      F_exit_blink_mode{};
    Capability      F_enter_reverse_mode{};
    Capability      F_exit_reverse_mode{};
    Capability      F_enter_standout_mode{};
    Capability      F_exit_standout_mode{};
    Capability      F_enter_secure_mode{};
    Capability      F_exit_secure_mode{};
    Capability      F_enter_protected_mode{};
    Capability      F_exit_protected_mode{};
    Capability      F_enter_crossed_out_mode{};
    Capability      F_exit_crossed_out_mode{};
    Capability      F_enter_dbl_underline_mode{};
    Capability      F_exit_dbl_underline_mode{};
    Capability      F_set_attributes{};
    Capability      F_exit_attribute_mode{};
    Capability      F_enter_alt_charset_mode{};
    Capability      F_exit_alt_charset_mode{};
    Capability      F_enter_pc_charset_mode{};
    Capability      F_exit_pc_charset_mode{};
    Capability      F_set_a_foreground{};
    Capability      F_set_a_background{};
    Capability      F_set_foreground{};
    Capability      F_set_background{};
    Capability      F_set_color_pair{};
    Capability      F_orig_pair{};
    Capability      F_orig_colors{};

    FChar           on{};
    FChar           off{};
    FChar           reset_byte_mask{};

    SGRoptimizer    sgr_optimizer{attr_buf};
    AttributeBuffer attr_buf{};

    int             max_color{1};
    int             attr_without_color{0};
    bool            ansi_default_color{false};
    bool            alt_equal_pc_charset{false};
    bool            monochron{true};
    bool            fake_reverse{false};
};


// FOptiAttr inline functions
//----------------------------------------------------------------------
inline FString FOptiAttr::getClassName() const
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
