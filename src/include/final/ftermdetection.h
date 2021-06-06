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
      uInt8 kitty          : 1;
      uInt8                : 3;  // padding bits
    };

    struct kittyVersion
    {
      int primary{0};
      int secondary{0};
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
    FString               getClassName() const;
    const FString&        getTermType() const;
    int                   getGnomeTerminalID() const;
    kittyVersion          getKittyVersion() const;
    FTerminalType&        getTermTypeStruct();

#if DEBUG
    const FString&        getAnswerbackString() const;
    const FString&        getSecDAString() const;
    const FString&        getTermType_256color() const;
    const FString&        getTermType_Answerback() const;
    const FString&        getTermType_SecDA() const;
#endif

    // Inquiries
    bool                  isAnsiTerminal() const;
    bool                  isXTerminal() const;
    bool                  isRxvtTerminal() const;
    bool                  isUrxvtTerminal() const;
    bool                  isKdeTerminal() const;
    bool                  isGnomeTerminal() const;
    bool                  isPuttyTerminal() const;
    bool                  isWindowsTerminal() const;
    bool                  isTeraTerm() const;
    bool                  isCygwinTerminal() const;
    bool                  isMinttyTerm() const;
    bool                  isLinuxTerm() const;
    bool                  isFreeBSDTerm() const;
    bool                  isNetBSDTerm() const;
    bool                  isOpenBSDTerm() const;
    bool                  isSunTerminal() const;
    bool                  isScreenTerm() const;
    bool                  isTmuxTerm() const;
    bool                  isKtermTerminal() const;
    bool                  isMltermTerminal() const;
    bool                  isKittyTerminal() const;
    bool                  canDisplay256Colors() const;
    bool                  hasTerminalDetection() const;
    bool                  hasSetCursorStyleSupport() const;

    // Mutators
    void                  setAnsiTerminal (bool = true);
    void                  setXTerminal (bool = true);
    void                  setRxvtTerminal (bool = true);
    void                  setUrxvtTerminal (bool = true);
    void                  setKdeTerminal (bool = true);
    void                  setGnomeTerminal (bool = true);
    void                  setPuttyTerminal (bool = true);
    void                  setWindowsTerminal (bool = true);
    void                  setTeraTerm (bool = true);
    void                  setCygwinTerminal (bool = true);
    void                  setMinttyTerm (bool = true);
    void                  setLinuxTerm (bool = true);
    void                  setFreeBSDTerm (bool = true);
    void                  setNetBSDTerm (bool = true);
    void                  setOpenBSDTerm (bool = true);
    void                  setSunTerminal (bool = true);
    void                  setScreenTerm (bool = true);
    void                  setTmuxTerm (bool = true);
    void                  setKtermTerminal (bool = true);
    void                  setMltermTerminal (bool = true);
    void                  setKittyTerminal (bool = true);
    void                  setTerminalDetection (bool = true);
    void                  setTtyTypeFileName (const FString&);

    // Methods
    void                  detect();

  private:
    struct colorEnv
    {
      char* string1{nullptr};
      char* string2{nullptr};
      char* string3{nullptr};
      char* string4{nullptr};
      char* string5{nullptr};
      char* string6{nullptr};
      char* string7{nullptr};
      char* string8{nullptr};
    };

    struct secondaryDA
    {
      int terminal_id_type{-1};
      int terminal_id_version{-1};
      int terminal_id_hardware{-1};
    };

    // Methods
    void                  getSystemTermType();
    bool                  getTTYtype();
#if F_HAVE_GETTTYNAM
    bool                  getTTYSFileEntry();
#endif
    void                  termtypeAnalysis();
    void                  detectTerminal();
    FString               init_256colorTerminal();
    bool                  get256colorEnvString();
    FString               termtype_256color_quirks();
    FString           determineMaxColor (const FString&);
    FString               getXTermColorName (FColor) const;
    FString               parseAnswerbackMsg (const FString&);
    FString               getAnswerbackMsg() const;
    FString               parseSecDA (const FString&);
    int                   str2int (const FString&) const;
    FString               getSecDA() const;
    FString               secDA_Analysis (const FString&);
    FString               secDA_Analysis_0 (const FString&);
    FString               secDA_Analysis_1 (const FString&);
    FString               secDA_Analysis_24 (const FString&);
    FString               secDA_Analysis_32 ();
    FString               secDA_Analysis_65 (const FString&);
    FString               secDA_Analysis_67 ();
    FString               secDA_Analysis_77 ();
    FString               secDA_Analysis_82 ();
    FString               secDA_Analysis_83 (const FString&);
    FString               secDA_Analysis_84 (const FString&);
    FString               secDA_Analysis_85 ();
    FString               secDA_Analysis_vte (const FString&);
    FString               secDA_Analysis_kitty (const FString&);

    // Data members
#if DEBUG
    FString        termtype_256color{};
    FString        termtype_Answerback{};
    FString        termtype_SecDA{};
#endif
    FString        termtype{};
    FString        ttytypename{"/etc/ttytype"};  // Default ttytype file
    bool           decscusr_support{false};      // Preset to false
    bool           terminal_detection{true};     // Preset to true
    bool           color256{};
    // Gnome terminal id from SecDA
    // Example: vte version 0.40.0 = 0 * 100 + 40 * 100 + 0 = 4000
    //                      a.b.c  = a * 100 +  b * 100 + c
    int            gnome_terminal_id{0};
    FString        answer_back{};
    FString        sec_da{};
    FTerminalType  terminal_type{};
    colorEnv       color_env{};
    kittyVersion   kitty_version{};
    secondaryDA    secondary_da{};
};


// FTermDetection inline functions
//----------------------------------------------------------------------
inline FString FTermDetection::getClassName() const
{ return "FTermDetection"; }

//----------------------------------------------------------------------
inline const FString& FTermDetection::getTermType() const
{ return termtype; }

//----------------------------------------------------------------------
inline int FTermDetection::getGnomeTerminalID() const
{ return gnome_terminal_id; }

//----------------------------------------------------------------------
inline FTermDetection::kittyVersion FTermDetection::getKittyVersion() const
{ return kitty_version; }

//----------------------------------------------------------------------
inline FTermDetection::FTerminalType& FTermDetection::getTermTypeStruct()
{ return terminal_type; }

#if DEBUG
//----------------------------------------------------------------------
inline const FString& FTermDetection::getTermType_256color() const
{ return termtype_256color; }

//----------------------------------------------------------------------
inline const FString& FTermDetection::getTermType_Answerback() const
{ return termtype_Answerback; }

//----------------------------------------------------------------------
inline const FString& FTermDetection::getTermType_SecDA() const
{ return termtype_SecDA; }
#endif

//----------------------------------------------------------------------
inline bool FTermDetection::canDisplay256Colors() const
{ return color256; }

//----------------------------------------------------------------------
inline bool FTermDetection::hasSetCursorStyleSupport() const
{ return decscusr_support; }

//----------------------------------------------------------------------
inline bool FTermDetection::isXTerminal() const
{ return terminal_type.xterm; }

//----------------------------------------------------------------------
inline bool FTermDetection::isAnsiTerminal() const
{ return terminal_type.ansi; }

//----------------------------------------------------------------------
inline bool FTermDetection::isRxvtTerminal() const
{ return terminal_type.rxvt; }

//----------------------------------------------------------------------
inline bool FTermDetection::isUrxvtTerminal() const
{ return terminal_type.urxvt; }

//----------------------------------------------------------------------
inline bool FTermDetection::isMltermTerminal() const
{ return terminal_type.mlterm; }

//----------------------------------------------------------------------
inline bool FTermDetection::isKittyTerminal() const
{ return terminal_type.kitty; }

//----------------------------------------------------------------------
inline bool FTermDetection::isPuttyTerminal() const
{ return terminal_type.putty; }

//----------------------------------------------------------------------
inline bool FTermDetection::isWindowsTerminal() const
{ return terminal_type.win_terminal; }

//----------------------------------------------------------------------
inline bool FTermDetection::isKdeTerminal() const
{ return terminal_type.kde_konsole; }

//----------------------------------------------------------------------
inline bool FTermDetection::isGnomeTerminal() const
{ return terminal_type.gnome_terminal; }

//----------------------------------------------------------------------
inline bool FTermDetection::isKtermTerminal() const
{ return terminal_type.kterm; }

//----------------------------------------------------------------------
inline bool FTermDetection::isTeraTerm() const
{ return terminal_type.tera_term; }

//----------------------------------------------------------------------
inline bool FTermDetection::isCygwinTerminal() const
{ return terminal_type.cygwin; }

//----------------------------------------------------------------------
inline bool FTermDetection::isMinttyTerm() const
{ return terminal_type.mintty; }

//----------------------------------------------------------------------
inline bool FTermDetection::isLinuxTerm() const
{ return terminal_type.linux_con; }

//----------------------------------------------------------------------
inline bool FTermDetection::isFreeBSDTerm() const
{ return terminal_type.freebsd_con; }

//----------------------------------------------------------------------
inline bool FTermDetection::isNetBSDTerm() const
{ return terminal_type.netbsd_con; }

//----------------------------------------------------------------------
inline bool FTermDetection::isOpenBSDTerm() const
{ return terminal_type.openbsd_con; }

//----------------------------------------------------------------------
inline bool FTermDetection::isSunTerminal() const
{ return terminal_type.sun_con; }

//----------------------------------------------------------------------
inline bool FTermDetection::isScreenTerm() const
{ return terminal_type.screen; }

//----------------------------------------------------------------------
inline bool FTermDetection::isTmuxTerm() const
{ return terminal_type.tmux; }

//----------------------------------------------------------------------
inline bool FTermDetection::hasTerminalDetection() const
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
inline void FTermDetection::setKittyTerminal (bool enable)
{ terminal_type.kitty = enable; }

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
