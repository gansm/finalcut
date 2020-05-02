/***********************************************************************
* ftermcap.h - Provides access to terminal capabilities                *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2016-2020 Markus Gans                                      *
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
 * ▕▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FTermcap ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FTERMCAP_H
#define FTERMCAP_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#if defined(__sun) && defined(__SVR4)
  #include <termio.h>
  typedef struct termio SGTTY;
  typedef struct termios SGTTYS;

  #ifdef _LP64
    typedef unsigned int chtype;
  #else
    typedef unsigned long chtype;
  #endif  // _LP64

  #include <term.h>  // termcap
#else
  #include <term.h>  // termcap
#endif  // defined(__sun) && defined(__SVR4)

#ifdef F_HAVE_LIBGPM
  #undef buttons  // from term.h
#endif

#include <string>
#include <vector>

// FTermcap string macro
#define TCAP(...)  FTermcap::strings[__VA_ARGS__].string

namespace finalcut
{

// class forward declaration
class FSystem;
class FTermData;
class FTermDetection;

//----------------------------------------------------------------------
// class FTermcap
//----------------------------------------------------------------------

class FTermcap final
{
  public:
    // Using-declaration
    using fn_putc = int (*)(int);

    // Typedef
    typedef struct
    {
      const char* string;
      char  tname[alignof(char*)];
    }
    tcap_map;

    // Constructors
    FTermcap() = default;

    // Destructor
    ~FTermcap() = default;

    // Accessors
    const FString        getClassName() const;
    template<typename CharT>
    static bool          getFlag (const CharT&);
    template<typename CharT>
    static int           getNumber (const CharT&);
    template<typename CharT>
    static char*         getString (const CharT&);
    template<typename CharT>
    static char*         encodeMotionParameter (const CharT&, int, int);
    template<typename CharT, typename... Args>
    static char*         encodeParameter (const CharT&, Args&&...);
    template<typename CharT>
    static int           paddingPrint (const CharT&, int, fn_putc);

    // Methods
    static void init();

    // Data members
    static bool          background_color_erase;
    static bool          can_change_color_palette;
    static bool          automatic_left_margin;
    static bool          automatic_right_margin;
    static bool          eat_nl_glitch;
    static bool          ansi_default_color;
    static bool          osc_support;
    static bool          no_utf8_acs_chars;
    static int           max_color;
    static int           tabstop;
    static int           attr_without_color;
    static tcap_map      strings[];

  private:
    // Methods
    static void          termcap();
    static void          termcapError (int);
    static void          termcapVariables();
    static void          termcapBoleans();
    static void          termcapNumerics();
    static void          termcapStrings();
    static void          termcapKeys();
    static int           _tputs (const char*, int, fn_putc);

    // Data member
    static FSystem*        fsystem;
    static FTermData*      fterm_data;
    static FTermDetection* term_detection;
    static char            string_buf[2048];
};


// FTermcap inline functions
//----------------------------------------------------------------------
inline const FString FTermcap::getClassName() const
{ return "FTermcap"; }

//----------------------------------------------------------------------
template<typename CharT>
bool FTermcap::getFlag (const CharT& cap)
{
  return tgetflag(C_STR(cap));
}

//----------------------------------------------------------------------
template<typename CharT>
int FTermcap::getNumber (const CharT& cap)
{
  return tgetnum(C_STR(cap));
}

//----------------------------------------------------------------------
template<typename CharT>
char* FTermcap::getString (const CharT& cap)
{
  return tgetstr(C_STR(cap), reinterpret_cast<char**>(&string_buf));
}

//----------------------------------------------------------------------
template<typename CharT>
char* FTermcap::encodeMotionParameter (const CharT& cap, int col, int row)
{
  return tgoto(C_STR(cap), col, row);
}

//----------------------------------------------------------------------
template<typename CharT, typename... Args>
inline char* FTermcap::encodeParameter (const CharT& cap, Args&&... args)
{
  return tparm (C_STR(cap), std::forward<Args>(args)...);
}

//----------------------------------------------------------------------
template<typename CharT>
int FTermcap::paddingPrint (const CharT& str, int affcnt, fn_putc putc)
{
  return _tputs (C_STR(str), affcnt, putc);
}

}  // namespace finalcut


#endif  // FTERMCAP_H
