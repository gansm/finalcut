/***********************************************************************
* ftermfreebsd.h - Contains the FreeBSD terminal functions             *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2020 Markus Gans                                      *
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
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FTermFreeBSD ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FTERMFREEBSD_H
#define FTERMFREEBSD_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fc.h"

#if defined(UNIT_TEST)
  #define CONS_CURSORTYPE 0x80046307
  #define GIO_KEYMAP      0x20006b06
  #define PIO_KEYMAP      0x20006b07
  #define META            0x84  // Meta key
  #define NUM_KEYS        256   // Number of keys in table
  #define NUM_STATES      8     // States per key

  struct keyent_t
  {
    int map[NUM_STATES];
    int spcl;
    int flgs;
  };

  struct keymap_t
  {
    int n_keys;
    struct keyent_t key[NUM_KEYS];
  };
#elif defined(__FreeBSD__) || defined(__DragonFly__)
  #undef mouse_info  // consio.h
  #undef buttons     // consio.h

  #include <sys/consio.h>
  #include <sys/kbio.h>
#endif

namespace finalcut
{

// class forward declaration
class FSystem;
class FTermData;

//----------------------------------------------------------------------
// class FTermFreeBSD
//----------------------------------------------------------------------

class FTermFreeBSD final
{
  public:
    // Typedef
    typedef fc::freebsdConsoleCursorStyle  CursorStyle;

    // Constructors
    FTermFreeBSD() = default;

    // Disable copy constructor
    FTermFreeBSD (const FTermFreeBSD&) = delete;

    // Destructor
    ~FTermFreeBSD() = default;

    // Disable copy assignment operator (=)
    FTermFreeBSD& operator = (const FTermFreeBSD&) = delete;

    // Accessors
    const FString      getClassName() const;
    static CursorStyle getCursorStyle();

    // Inquiry
    static bool        isFreeBSDConsole();

    // Mutators
    static bool        setCursorStyle (CursorStyle);
    static void        enableChangeCursorStyle();
    static void        disableChangeCursorStyle();
    static void        enableMetaSendsEscape();
    static void        disableMetaSendsEscape();
    static void        setBeep (int, int);
    static void        resetBeep();

    // Methods
    static void        init();
    static void        initCharMap();
    static void        finish();

  private:
    // Methods
    static void        warnNotInitialized();
    static bool        saveFreeBSDAltKey();
    static bool        setFreeBSDAltKey (uInt);
    static bool        setFreeBSDAlt2Meta();
    static bool        resetFreeBSDAlt2Meta();
    static bool        setFreeBSDCursorStyle (CursorStyle);
    static bool        isInitialized();

    // Data members
    static uInt        bsd_alt_keymap;
    static CursorStyle cursor_style;
    static bool        change_cursorstyle;
    static bool        meta_sends_escape;
    static FSystem*    fsystem;
    static FTermData*  fterm_data;
};


// FTermFreeBSD inline functions
//----------------------------------------------------------------------
inline const FString FTermFreeBSD::getClassName() const
{ return "FTermFreeBSD"; }

//----------------------------------------------------------------------
#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
inline void FTermFreeBSD::enableChangeCursorStyle()
{ change_cursorstyle = true; }

//----------------------------------------------------------------------
inline void FTermFreeBSD::disableChangeCursorStyle()
{ change_cursorstyle = false; }

//----------------------------------------------------------------------
inline void FTermFreeBSD::enableMetaSendsEscape()
{ meta_sends_escape = true; }

//----------------------------------------------------------------------
inline void FTermFreeBSD::disableMetaSendsEscape()
{ meta_sends_escape = false; }

//----------------------------------------------------------------------
inline bool FTermFreeBSD::isInitialized()
{ return bool(fsystem && fterm_data); }
#endif  // defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)

}  // namespace finalcut

#endif  // FTERMFREEBSD_H
