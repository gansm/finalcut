/***********************************************************************
* ftermdetection.h - Detection of the terminal type                    *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2018 Markus Gans                                           *
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
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FTermDetection ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FTERMDETECTION_H
#define FTERMDETECTION_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "final/fc.h"
#include "final/fconfig.h"
#include "final/ftermdata.h"
#include "final/ftermios.h"
#include "final/ftypes.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FTermDetection
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FTermDetection
{
  public:
    // Typedefs
    typedef struct
    {
      // byte #0
      uInt8 ansi           : 1;
      uInt8 xterm          : 1;
      uInt8 rxvt           : 1;
      uInt8 urxvt          : 1;
      uInt8 mlterm         : 1;
      uInt8 putty          : 1;
      uInt8 kde_konsole    : 1;
      uInt8 gnome_terminal : 1;
      // byte #1
      uInt8 kterm          : 1;
      uInt8 tera_term      : 1;
      uInt8 cygwin         : 1;
      uInt8 mintty         : 1;
      uInt8 linux_con      : 1;
      uInt8 freebsd_con    : 1;
      uInt8 netbsd_con     : 1;
      uInt8 openbsd_con    : 1;
      // byte #2
      uInt8 sun_con        : 1;
      uInt8 screen         : 1;
      uInt8 tmux           : 1;
      uInt8                : 5;  // padding bits
    } terminalType;

    // Constructors
    FTermDetection();

    // Destructor
    virtual ~FTermDetection();

    // Accessor
    const char*           getClassName() const;
    static char*          getTermType();
    static int            getGnomeTerminalID();
    terminalType&         getTermTypeStruct();

#if DEBUG
    static const FString& getAnswerbackString();
    static const FString& getSecDAString();
    static const char*    getTermType_256color();
    static const char*    getTermType_Answerback();
    static const char*    getTermType_SecDA();
#endif

    // Inquiries
    static bool           isAnsiTerminal();
    static bool           isXTerminal();
    static bool           isRxvtTerminal();
    static bool           isUrxvtTerminal();
    static bool           isMltermTerminal();
    static bool           isPuttyTerminal();
    static bool           isKdeTerminal();
    static bool           isGnomeTerminal();
    static bool           isKtermTerminal();
    static bool           isTeraTerm();
    static bool           isCygwinTerminal();
    static bool           isMinttyTerm();
    static bool           isLinuxTerm();
    static bool           isFreeBSDTerm();
    static bool           isNetBSDTerm();
    static bool           isOpenBSDTerm();
    static bool           isSunTerminal();
    static bool           isScreenTerm();
    static bool           isTmuxTerm();
    static bool           canDisplay256Colors();
    static bool           hasTerminalDetection();
    static bool           hasSetCursorStyleSupport();

    // Mutators
    static void           setAnsiTerminal (bool);
    static void           setXTerminal (bool);
    static void           setRxvtTerminal (bool);
    static void           setUrxvtTerminal (bool);
    static void           setMltermTerminal (bool);
    static void           setPuttyTerminal (bool);
    static void           setKdeTerminal (bool);
    static void           setGnomeTerminal (bool);
    static void           setKtermTerminal (bool);
    static void           setTeraTerm (bool);
    static void           setCygwinTerminal (bool);
    static void           setMinttyTerm (bool);
    static void           setLinuxTerm (bool);
    static void           setFreeBSDTerm (bool);
    static void           setNetBSDTerm (bool);
    static void           setOpenBSDTerm (bool);
    static void           setSunTerminal (bool);
    static void           setScreenTerm (bool);
    static void           setTmuxTerm (bool);
    static void           setTerminalDetection (bool);
    static void           setFTermData (FTermData*);
    static void           setTtyTypeFileName (char[]);

    // Methods
    static void           detect();

    // Data Members
#if DEBUG
    static char           termtype_256color[256];
    static char           termtype_Answerback[256];
    static char           termtype_SecDA[256];
#endif

  private:
    // Methods
    static void           getSystemTermType();
    static bool           getTTYtype();
#if F_HAVE_GETTTYNAM
    static bool           getTTYSFileEntry();
#endif
    static void           termtypeAnalysis();
    static void           detectTerminal();
    static char*          init_256colorTerminal();
    static bool           get256colorEnvString();
    static char*          termtype_256color_quirks();
    static char*          determineMaxColor (char[]);
    static const FString  getXTermColorName (int);
    static char*          parseAnswerbackMsg (char[]);
    static const FString  getAnswerbackMsg();
    static char*          parseSecDA (char[]);
    static int            str2int (const FString&);
    static const FString  getSecDA();
    static char*          secDA_Analysis (char[]);
    static char*          secDA_Analysis_0 (char[]);
    static char*          secDA_Analysis_1 (char[]);
    static char*          secDA_Analysis_24 (char[]);
    static char*          secDA_Analysis_32 (char[]);
    static char*          secDA_Analysis_65 (char[]);
    static char*          secDA_Analysis_67 (char[]);
    static char*          secDA_Analysis_77 (char[]);
    static char*          secDA_Analysis_82 (char[]);
    static char*          secDA_Analysis_83 (char[]);
    static char*          secDA_Analysis_84 (char[]);
    static char*          secDA_Analysis_85 (char[]);
    static char*          secDA_Analysis_vte (char[]);

    // Data Members
    static char           termtype[256];
    static char           ttytypename[256];
    static bool           decscusr_support;
    static bool           terminal_detection;
    static bool           color256;
    static int            gnome_terminal_id;
    static const FString* answer_back;
    static const FString* sec_da;
    static FTermData*     fterm_data;
    static terminalType   terminal_type;

    static struct colorEnv
    {
      void setDefault()
      {
        string1 = 0;
        string2 = 0;
        string3 = 0;
        string4 = 0;
        string5 = 0;
        string6 = 0;
        string7 = 0;
      }

      char* string1;
      char* string2;
      char* string3;
      char* string4;
      char* string5;
      char* string6;
      char* string7;
    } color_env;

    static struct secondaryDA
    {
      void setDefault()
      {
        terminal_id_type = -1;
        terminal_id_version = -1;
        terminal_id_hardware = -1;
      }

      int terminal_id_type;
      int terminal_id_version;
      int terminal_id_hardware;
    } secondary_da;
};
#pragma pack(pop)

// FTermDetection inline functions
//----------------------------------------------------------------------
inline const char* FTermDetection::getClassName() const
{ return "FTermDetection"; }

//----------------------------------------------------------------------
inline char* FTermDetection::getTermType()
{ return termtype; }

//----------------------------------------------------------------------
inline int FTermDetection::getGnomeTerminalID()
{ return gnome_terminal_id; }

//----------------------------------------------------------------------
inline FTermDetection::terminalType& FTermDetection::getTermTypeStruct()
{ return terminal_type; }

#if DEBUG
//----------------------------------------------------------------------
inline const FString& FTermDetection::getAnswerbackString()
{ return ( answer_back ) ? *answer_back : fc::emptyFString::get(); }

//----------------------------------------------------------------------
inline const FString& FTermDetection::getSecDAString()
{ return ( sec_da ) ? *sec_da : fc::emptyFString::get(); }

//----------------------------------------------------------------------
inline const char* FTermDetection::getTermType_256color()
{ return termtype_256color; }

//----------------------------------------------------------------------
inline const char* FTermDetection::getTermType_Answerback()
{ return termtype_Answerback; }

//----------------------------------------------------------------------
inline const char* FTermDetection::getTermType_SecDA()
{ return termtype_SecDA; }
#endif

//----------------------------------------------------------------------
inline bool FTermDetection::canDisplay256Colors()
{ return color256; }

//----------------------------------------------------------------------
inline bool FTermDetection::hasSetCursorStyleSupport()
{ return decscusr_support; }

//----------------------------------------------------------------------
inline bool FTermDetection::isXTerminal()
{ return terminal_type.xterm; }

//----------------------------------------------------------------------
inline bool FTermDetection::isAnsiTerminal()
{ return terminal_type.ansi; }

//----------------------------------------------------------------------
inline bool FTermDetection::isRxvtTerminal()
{ return terminal_type.rxvt; }

//----------------------------------------------------------------------
inline bool FTermDetection::isUrxvtTerminal()
{ return terminal_type.urxvt; }

//----------------------------------------------------------------------
inline bool FTermDetection::isMltermTerminal()
{ return terminal_type.mlterm; }

//----------------------------------------------------------------------
inline bool FTermDetection::isPuttyTerminal()
{ return terminal_type.putty; }

//----------------------------------------------------------------------
inline bool FTermDetection::isKdeTerminal()
{ return terminal_type.kde_konsole; }

//----------------------------------------------------------------------
inline bool FTermDetection::isGnomeTerminal()
{ return terminal_type.gnome_terminal; }

//----------------------------------------------------------------------
inline bool FTermDetection::isKtermTerminal()
{ return terminal_type.kterm; }

//----------------------------------------------------------------------
inline bool FTermDetection::isTeraTerm()
{ return terminal_type.tera_term; }

//----------------------------------------------------------------------
inline bool FTermDetection::isCygwinTerminal()
{ return terminal_type.cygwin; }

//----------------------------------------------------------------------
inline bool FTermDetection::isMinttyTerm()
{ return terminal_type.mintty; }

//----------------------------------------------------------------------
inline bool FTermDetection::isLinuxTerm()
{ return terminal_type.linux_con; }

//----------------------------------------------------------------------
inline bool FTermDetection::isFreeBSDTerm()
{ return terminal_type.freebsd_con; }

//----------------------------------------------------------------------
inline bool FTermDetection::isNetBSDTerm()
{ return terminal_type.netbsd_con; }

//----------------------------------------------------------------------
inline bool FTermDetection::isOpenBSDTerm()
{ return terminal_type.openbsd_con; }

//----------------------------------------------------------------------
inline bool FTermDetection::isSunTerminal()
{ return terminal_type.sun_con; }

//----------------------------------------------------------------------
inline bool FTermDetection::isScreenTerm()
{ return terminal_type.screen; }

//----------------------------------------------------------------------
inline bool FTermDetection::isTmuxTerm()
{ return terminal_type.tmux; }

//----------------------------------------------------------------------
inline bool FTermDetection::hasTerminalDetection()
{ return terminal_detection; }

//----------------------------------------------------------------------
inline void FTermDetection::setXTerminal (bool on)
{ terminal_type.xterm = on; }

//----------------------------------------------------------------------
inline void FTermDetection::setAnsiTerminal (bool on)
{ terminal_type.ansi = on; }

//----------------------------------------------------------------------
inline void FTermDetection::setRxvtTerminal (bool on)
{ terminal_type.rxvt = on; }

//----------------------------------------------------------------------
inline void FTermDetection::setUrxvtTerminal (bool on)
{ terminal_type.urxvt = on; }

//----------------------------------------------------------------------
inline void FTermDetection::setMltermTerminal (bool on)
{ terminal_type.mlterm = on; }

//----------------------------------------------------------------------
inline void FTermDetection::setPuttyTerminal (bool on)
{ terminal_type.putty = on; }

//----------------------------------------------------------------------
inline void FTermDetection::setKdeTerminal (bool on)
{ terminal_type.kde_konsole = on; }

//----------------------------------------------------------------------
inline void FTermDetection::setGnomeTerminal (bool on)
{ terminal_type.gnome_terminal = on; }

//----------------------------------------------------------------------
inline void FTermDetection::setKtermTerminal (bool on)
{ terminal_type.kterm = on; }

//----------------------------------------------------------------------
inline void FTermDetection::setTeraTerm (bool on)
{ terminal_type.tera_term = on; }

//----------------------------------------------------------------------
inline void FTermDetection::setCygwinTerminal (bool on)
{ terminal_type.cygwin = on; }

//----------------------------------------------------------------------
inline void FTermDetection::setMinttyTerm (bool on)
{ terminal_type.mintty = on; }

//----------------------------------------------------------------------
inline void FTermDetection::setLinuxTerm (bool on)
{ terminal_type.linux_con = on; }

//----------------------------------------------------------------------
inline void FTermDetection::setFreeBSDTerm (bool on)
{ terminal_type.freebsd_con = on; }

//----------------------------------------------------------------------
inline void FTermDetection::setNetBSDTerm (bool on)
{ terminal_type.netbsd_con = on; }

//----------------------------------------------------------------------
inline void FTermDetection::setOpenBSDTerm (bool on)
{ terminal_type.openbsd_con = on; }

//----------------------------------------------------------------------
inline void FTermDetection::setSunTerminal (bool on)
{ terminal_type.sun_con = on; }

//----------------------------------------------------------------------
inline void FTermDetection::setScreenTerm (bool on)
{ terminal_type.screen = on; }

//----------------------------------------------------------------------
inline void FTermDetection::setTmuxTerm (bool on)
{ terminal_type.tmux = on; }

//----------------------------------------------------------------------
inline void FTermDetection::setTerminalDetection (bool on)
{ terminal_detection = on; }

}  // namespace finalcut

#endif  // FTERMDETECTION_H
