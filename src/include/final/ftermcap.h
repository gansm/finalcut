/***********************************************************************
* ftermcap.h - Provides access to terminal capabilities                *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2016-2021 Markus Gans                                      *
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
 * ▕▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FTermcap ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FTERMCAP_H
#define FTERMCAP_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <array>
#include <string>
#include <utility>
#include <vector>

// FTermcap string macro
#define TCAP(...)  FTermcap::strings[int(Termcap::__VA_ARGS__)].string

namespace finalcut
{

//----------------------------------------------------------------------
// class FTermcap
//----------------------------------------------------------------------

class FTermcap final
{
  public:
    struct TCapMap
    {
      const char* string;
      char  tname[alignof(char*)];
    };

    // Using-declaration
    using fn_putc = int (*)(int);
    using TCapMapType = std::array<TCapMap, 83>;

    // Constructors
    FTermcap() = default;

    // Destructor
    ~FTermcap() = default;

    // Accessors
    FString              getClassName() const;
    static bool          getFlag (const std::string&);
    static int           getNumber (const std::string&);
    static char*         getString (const std::string&);
    static std::string   encodeMotionParameter (const std::string&, int, int);
    template <typename... Args>
    static std::string   encodeParameter (const std::string&, Args&&...);
    static int           paddingPrint (const std::string&, int, fn_putc);

    // Inquiry
    static bool          isInitialized();

    // Methods
    static void init();

    // Data members
    static bool          background_color_erase;
    static bool          can_change_color_palette;
    static bool          automatic_left_margin;
    static bool          automatic_right_margin;
    static bool          eat_nl_glitch;
    static bool          has_ansi_escape_sequences;
    static bool          ansi_default_color;
    static bool          osc_support;
    static bool          no_utf8_acs_chars;
    static int           max_color;
    static int           tabstop;
    static int           attr_without_color;
    static TCapMapType   strings;

  private:
    // Constant
    static constexpr std::size_t BUF_SIZE{2048};

    // Methods
    static void          termcap();
    static void          termcapError (int);
    static void          termcapVariables();
    static void          termcapBoleans();
    static void          termcapNumerics();
    static void          termcapStrings();
    static void          termcapKeys();
    static std::string   encodeParams ( const std::string&
                                      , const std::vector<int>& );
    static int           _tputs (const char*, int, fn_putc);

    // Data member
    static char          string_buf[BUF_SIZE];
    static bool          initialized;
};

// FTermcap inline functions
//----------------------------------------------------------------------
inline FString FTermcap::getClassName() const
{ return "FTermcap"; }

//----------------------------------------------------------------------
template <typename... Args>
std::string FTermcap::encodeParameter (const std::string& cap, Args&&... args)
{
  return encodeParams(cap, {static_cast<int>(args)...});
}

//----------------------------------------------------------------------
inline int FTermcap::paddingPrint (const std::string& str, int affcnt, fn_putc putc)
{
  return _tputs (C_STR(str.data()), affcnt, putc);
}

//----------------------------------------------------------------------
inline bool FTermcap::isInitialized()
{
  return initialized;
}

}  // namespace finalcut


#endif  // FTERMCAP_H
