/***********************************************************************
* ftermdetection.h - Detection of the terminal type                    *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2021 Markus Gans                                      *
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

namespace finalcut
{

//----------------------------------------------------------------------
// class FTermDetection
//----------------------------------------------------------------------

class FTermDetection final
{
  public:
    struct FTerminalType
    {
      // byte #0
      uInt8 ansi           : 1;
      uInt8 xterm          : 1;
      uInt8 rxvt           : 1;
      uInt8 urxvt          : 1;
      uInt8 kde_konsole    : 1;
      uInt8 gnome_terminal : 1;
      uInt8 putty          : 1;
      uInt8 win_terminal   : 1;
      // byte #1
      uInt8 tera_term      : 1;
      uInt8 cygwin         : 1;
      uInt8 mintty         : 1;
      uInt8 linux_con      : 1;
      uInt8 freebsd_con    : 1;
      uInt8 netbsd_con     : 1;
      uInt8 openbsd_con    : 1;
      uInt8 sun_con        : 1;
      // byte #2
      uInt8 screen         : 1;
      uInt8 tmux           : 1;
      uInt8 kterm          : 1;
      uInt8 mlterm         : 1;
      uInt8                : 4;  // padding bits
    };

    // Constructors
    FTermDetection();

    // Disable copy constructor
    FTermDetection (const FTermDetection&) = delete;

    // Destructor
    ~FTermDetection();

    // Disable copy assignment operator (=)
    FTermDetection& operator = (const FTermDetection&) = delete;

    // Accessor
    static FString        getClassName();
    static const char*    getTermType();
    static int            getGnomeTerminalID();
    FTerminalType&        getTermTypeStruct();

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
    static bool           isKdeTerminal();
    static bool           isGnomeTerminal();
    static bool           isPuttyTerminal();
    static bool           isWindowsTerminal();
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
    static bool           isKtermTerminal();
    static bool           isMltermTerminal();
    static bool           canDisplay256Colors();
    static bool           hasTerminalDetection();
    static bool           hasSetCursorStyleSupport();

    // Mutators
    static void           setAnsiTerminal (bool = true);
    static void           setXTerminal (bool = true);
    static void           setRxvtTerminal (bool = true);
    static void           setUrxvtTerminal (bool = true);
    static void           setKdeTerminal (bool = true);
    static void           setGnomeTerminal (bool = true);
    static void           setPuttyTerminal (bool = true);
    static void           setWindowsTerminal (bool = true);
    static void           setTeraTerm (bool = true);
    static void           setCygwinTerminal (bool = true);
    static void           setMinttyTerm (bool = true);
    static void           setLinuxTerm (bool = true);
    static void           setFreeBSDTerm (bool = true);
    static void           setNetBSDTerm (bool = true);
    static void           setOpenBSDTerm (bool = true);
    static void           setSunTerminal (bool = true);
    static void           setScreenTerm (bool = true);
    static void           setTmuxTerm (bool = true);
    static void           setKtermTerminal (bool = true);
    static void           setMltermTerminal (bool = true);
    static void           setTerminalDetection (bool = true);
    static void           setTtyTypeFileName (const char[]);

    // Methods
    static void           detect();

  private:
    struct colorEnv;      // forward declaration
    struct secondaryDA;   // forward declaration

    // Methods
    static void           deallocation();
    static void           getSystemTermType();
    static bool           getTTYtype();
#if F_HAVE_GETTTYNAM
    static bool           getTTYSFileEntry();
#endif
    static void           termtypeAnalysis();
    static void           detectTerminal();
    static const char*    init_256colorTerminal();
    static bool           get256colorEnvString();
    static const char*    termtype_256color_quirks();
    static const char*    determineMaxColor (const char[]);
    static FString        getXTermColorName (FColor);
    static const char*    parseAnswerbackMsg (const char[]);
    static FString        getAnswerbackMsg();
    static const char*    parseSecDA (const char[]);
    static int            str2int (const FString&);
    static FString        getSecDA();
    static const char*    secDA_Analysis (const char[]);
    static const char*    secDA_Analysis_0 (const char[]);
    static const char*    secDA_Analysis_1 (const char[]);
    static const char*    secDA_Analysis_24 (const char[]);
    static const char*    secDA_Analysis_32 (const char[]);
    static const char*    secDA_Analysis_65 (const char[]);
    static const char*    secDA_Analysis_67 (const char[]);
    static const char*    secDA_Analysis_77 (const char[]);
    static const char*    secDA_Analysis_82 ();
    static const char*    secDA_Analysis_83 (const char[]);
    static const char*    secDA_Analysis_84 (const char[]);
    static const char*    secDA_Analysis_85 ();
    static const char*    secDA_Analysis_vte (const char[]);

    // Data members
#if DEBUG
    static char           termtype_256color[256];
    static char           termtype_Answerback[256];
    static char           termtype_SecDA[256];
#endif
    static char           termtype[256];
    static char           ttytypename[256];
    static bool           decscusr_support;
    static bool           terminal_detection;
    static bool           color256;
    static int            gnome_terminal_id;
    static const FString* answer_back;
    static const FString* sec_da;
    static FTerminalType  terminal_type;
    static colorEnv       color_env;
    static secondaryDA    secondary_da;
};


//----------------------------------------------------------------------
// struct FTermDetection::colorEnv
//----------------------------------------------------------------------
struct FTermDetection::colorEnv
{
  char* string1{nullptr};
  char* string2{nullptr};
  char* string3{nullptr};
  char* string4{nullptr};
  char* string5{nullptr};
  char* string6{nullptr};
  char* string7{nullptr};
};

//----------------------------------------------------------------------
// struct FTermDetection::secondaryDA
//----------------------------------------------------------------------
struct FTermDetection::secondaryDA
{
  int terminal_id_type{-1};
  int terminal_id_version{-1};
  int terminal_id_hardware{-1};
};


// FTermDetection inline functions
//----------------------------------------------------------------------
inline FString FTermDetection::getClassName()
{ return "FTermDetection"; }

//----------------------------------------------------------------------
inline const char* FTermDetection::getTermType()
{ return termtype; }

//----------------------------------------------------------------------
inline int FTermDetection::getGnomeTerminalID()
{ return gnome_terminal_id; }

//----------------------------------------------------------------------
inline FTermDetection::FTerminalType& FTermDetection::getTermTypeStruct()
{ return terminal_type; }

#if DEBUG
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
inline bool FTermDetection::isWindowsTerminal()
{ return terminal_type.win_terminal; }

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
inline void FTermDetection::setXTerminal (bool enable)
{ terminal_type.xterm = enable; }

//----------------------------------------------------------------------
inline void FTermDetection::setAnsiTerminal (bool enable)
{ terminal_type.ansi = enable; }

//----------------------------------------------------------------------
inline void FTermDetection::setRxvtTerminal (bool enable)
{ terminal_type.rxvt = enable; }

//----------------------------------------------------------------------
inline void FTermDetection::setUrxvtTerminal (bool enable)
{ terminal_type.urxvt = enable; }

//----------------------------------------------------------------------
inline void FTermDetection::setMltermTerminal (bool enable)
{ terminal_type.mlterm = enable; }

//----------------------------------------------------------------------
inline void FTermDetection::setPuttyTerminal (bool enable)
{ terminal_type.putty = enable; }

//----------------------------------------------------------------------
inline void FTermDetection::setWindowsTerminal (bool enable)
{ terminal_type.win_terminal = enable; }

//----------------------------------------------------------------------
inline void FTermDetection::setKdeTerminal (bool enable)
{ terminal_type.kde_konsole = enable; }

//----------------------------------------------------------------------
inline void FTermDetection::setGnomeTerminal (bool enable)
{ terminal_type.gnome_terminal = enable; }

//----------------------------------------------------------------------
inline void FTermDetection::setKtermTerminal (bool enable)
{ terminal_type.kterm = enable; }

//----------------------------------------------------------------------
inline void FTermDetection::setTeraTerm (bool enable)
{ terminal_type.tera_term = enable; }

//----------------------------------------------------------------------
inline void FTermDetection::setCygwinTerminal (bool enable)
{ terminal_type.cygwin = enable; }

//----------------------------------------------------------------------
inline void FTermDetection::setMinttyTerm (bool enable)
{ terminal_type.mintty = enable; }

//----------------------------------------------------------------------
inline void FTermDetection::setLinuxTerm (bool enable)
{ terminal_type.linux_con = enable; }

//----------------------------------------------------------------------
inline void FTermDetection::setFreeBSDTerm (bool enable)
{ terminal_type.freebsd_con = enable; }

//----------------------------------------------------------------------
inline void FTermDetection::setNetBSDTerm (bool enable)
{ terminal_type.netbsd_con = enable; }

//----------------------------------------------------------------------
inline void FTermDetection::setOpenBSDTerm (bool enable)
{ terminal_type.openbsd_con = enable; }

//----------------------------------------------------------------------
inline void FTermDetection::setSunTerminal (bool enable)
{ terminal_type.sun_con = enable; }

//----------------------------------------------------------------------
inline void FTermDetection::setScreenTerm (bool enable)
{ terminal_type.screen = enable; }

//----------------------------------------------------------------------
inline void FTermDetection::setTmuxTerm (bool enable)
{ terminal_type.tmux = enable; }

//----------------------------------------------------------------------
inline void FTermDetection::setTerminalDetection (bool enable)
{ terminal_detection = enable; }

}  // namespace finalcut

#endif  // FTERMDETECTION_H
