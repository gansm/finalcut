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
#include <thread>
#include <utility>
#include <vector>

#include "final/ftypes.h"
#include "final/util/fstring.h"

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
    // Enumeration
    enum class Status
    {
      Error = -1,
      OK = 0
    };

    struct TCapMap
    {
      const char* string;
      char  tname[alignof(char*)];
    };

    // Using-declaration
    using TCapMapType = std::array<TCapMap, 85>;
    using PutCharFunc = int (*)(int);

    // Constructors
    FTermcap() = default;

    // Accessors
    FString              getClassName() const;
    static bool          getFlag (const std::string&);
    static int           getNumber (const std::string&);
    static char*         getString (const std::string&);
    static std::string   encodeMotionParameter (const std::string&, int, int);
    template <typename... Args>
    static std::string   encodeParameter (const std::string&, Args&&...);
    static Status        paddingPrint (const std::string&, int);

    // Inquiry
    static bool          isInitialized();

    // Mutator
    template<typename PutChar>
    static void          setPutCharFunction (const PutChar&);
    static void          setDefaultPutcharFunction();
    static void          setBaudrate (int);

    // Methods
    static void          init();

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
    static bool          no_padding_char;
    static bool          xon_xoff_flow_control;
    static int           max_color;
    static int           tabstop;
    static int           padding_baudrate;
    static int           attr_without_color;
    static TCapMapType   strings;

  private:
    // Using-declaration
    using string_iterator = std::string::const_iterator;

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
                                      , const std::array<int, 9>& );
    static void          delayOutput (int);
    static void          decimalPoint (string_iterator&, int&);
    static void          readDigits (string_iterator&, int&);
    static void          asteriskSlash (string_iterator&, int&, int, bool&);
    // Data member
    static bool          initialized;
    static int           baudrate;
    static char          PC;
    static char          string_buf[BUF_SIZE];
    static PutCharFunc   outc;
};

// FTermcap inline functions
//----------------------------------------------------------------------
inline FString FTermcap::getClassName() const
{ return "FTermcap"; }

//----------------------------------------------------------------------
template <typename... Args>
std::string FTermcap::encodeParameter (const std::string& cap, Args&&... args)
{
  std::array<int, 9> attr{{ 0, 0, 0, 0, 0, 0, 0, 0, 0 }};
  attr = {{static_cast<int>(args)...}};
  return encodeParams(cap, attr);
}

//----------------------------------------------------------------------
inline bool FTermcap::isInitialized()
{
  return initialized && outc;
}

//----------------------------------------------------------------------
template<typename PutChar>
inline void FTermcap::setPutCharFunction (const PutChar& put_char)
{ outc = put_char; }

//----------------------------------------------------------------------
inline void FTermcap::setBaudrate (int baud)
{
  baudrate = baud;
}

//----------------------------------------------------------------------
inline void FTermcap::delayOutput (int ms)
{
  if ( no_padding_char )
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
  }
  else
  {
    static constexpr int baudbyte = 9;  // = 7 bit + 1 parity + 1 stop

    for ( int pad_char_count = (ms * baudrate) / (baudbyte * 1000);
          pad_char_count > 0;
          pad_char_count-- )
    {
      outc(int(PC));
    }

    std::fflush(stdout);
  }
}

}  // namespace finalcut


#endif  // FTERMCAP_H
