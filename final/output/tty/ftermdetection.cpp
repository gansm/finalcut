/***********************************************************************
* ftermdetection.cpp - Detection of the terminal type                  *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2023 Markus Gans                                      *
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

#if defined(__CYGWIN__)
  #include "final/fconfig.h"  // includes _GNU_SOURCE
#endif

#include <array>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "final/fapplication.h"
#include "final/fc.h"
#include "final/ftypes.h"
#include "final/input/fkeyboard.h"
#include "final/output/tty/ftermcap.h"
#include "final/output/tty/ftermdata.h"
#include "final/output/tty/ftermdetection.h"
#include "final/output/tty/fterm_functions.h"
#include "final/output/tty/fterm.h"
#include "final/output/tty/ftermios.h"
#include "final/util/emptyfstring.h"
#include "final/util/flog.h"
#include "final/util/fsystem.h"

#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  #include "final/output/tty/ftermfreebsd.h"
#endif

#if defined(__NetBSD__) || defined(__OpenBSD__) || defined(UNIT_TEST)
  #include "final/output/tty/ftermopenbsd.h"
#endif

namespace finalcut
{

//----------------------------------------------------------------------
// class FTermDetection
//----------------------------------------------------------------------

// public methods of FTermDetection
//----------------------------------------------------------------------
auto FTermDetection::getInstance() -> FTermDetection&
{
  static const auto& term_detection = std::make_unique<FTermDetection>();
  return *term_detection;
}

//----------------------------------------------------------------------
#if DEBUG
auto FTermDetection::getAnswerbackString() const & -> const FString&
{
  // Get the answerback message that was output after
  // sending the enquiry character (ENQ)
  return answer_back;
}

//----------------------------------------------------------------------
auto FTermDetection::getSecDAString() const & -> const FString&
{
  // Get the secondary device attributes (SEC_DA)
  return sec_da;
}
#endif

//----------------------------------------------------------------------
void FTermDetection::setTtyTypeFileName (const FString& ttytype_filename)
{
  if ( ! ttytype_filename )
    return;

  ttytypename = ttytype_filename;
}

//----------------------------------------------------------------------
void FTermDetection::detect()
{
  // Set the variable 'termtype' to the predefined type of the terminal
  getSystemTermType();

  // Analysis the termtype
  termtypeAnalysis();

  // Terminal detection
  detectTerminal();
}


// private methods of FTermDetection
//----------------------------------------------------------------------
void FTermDetection::getSystemTermType()
{
  // Import the untrusted environment variable TERM
  const auto& term_env = std::getenv("TERM");
  const auto& termfilename = FTermData::getInstance().getTermFileName();

  if ( term_env )
  {
    // Save name in termtype
    termtype = term_env;
    return;
  }

  if ( ! termfilename.empty() )  // 1st fallback: use the teminal file name
  {
    if ( getTTYtype() )  // Look into /etc/ttytype
      return;

#if F_HAVE_GETTTYNAM
    if ( getTTYSFileEntry() )  // Look into /etc/ttys
      return;
#endif
  }

  // 2nd fallback: use vt100 if not found
  termtype = "vt100";
}

//----------------------------------------------------------------------
auto FTermDetection::getTTYtype() -> bool
{
  // Analyse /etc/ttytype and get the term name
  // ------------------------------------------
  // file format:
  // <terminal type> <whitespace> <tty name>
  //
  // Example:
  // linux  tty1
  // vt100  ttys0

  auto* term_basename = getTermBasename();
  std::FILE* file_ptr{};
  std::array<char, BUFSIZ> str{};
  static const auto& fsystem = FSystem::getInstance();

  if ( (file_ptr = fsystem->fopen(ttytypename.c_str(), "r")) == nullptr )
    return false;

  // Read and parse the file
  while ( fgets(str.data(), str.size() - 1, file_ptr) != nullptr )
  {
    const char* type{nullptr};  // nullptr == not found
    const char* name{nullptr};
    char* p = str.data();

    while ( *p )
    {
      if ( std::isspace(uChar(*p)) )
        *p = '\0';
      else if ( type == nullptr )
        type = p;
      else if ( name == nullptr && p != str.data() && p[-1] == '\0' )
        name = p;

      p++;
    }

    if ( type != nullptr && name != nullptr && ! std::strcmp(name, term_basename) )
    {
      // Save name in termtype
      termtype = type;
      fsystem->fclose(file_ptr);
      return true;
    }
  }

  fsystem->fclose(file_ptr);
  return false;
}

#if F_HAVE_GETTTYNAM
//----------------------------------------------------------------------
auto FTermDetection::getTTYSFileEntry() -> bool
{
  // Analyse /etc/ttys and get the term name (used in BSD Unix)

  auto* term_basename = getTermBasename();
  const struct ttyent* ttys_entryt;
  ttys_entryt = getttynam(term_basename);

  if ( ttys_entryt )
  {
    const auto& type = ttys_entryt->ty_type;

    if ( type != nullptr )
    {
      // Save name in termtype
      termtype = type;
      endttyent();
      return true;
    }
  }

  endttyent();
  return false;
}
#endif  // F_HAVE_GETTTYNAM

//----------------------------------------------------------------------
auto FTermDetection::getTermBasename() const -> const char*
{
  // Get the terminal basename
  const auto& termfilename = FTermData::getInstance().getTermFileName();
  const char* term_basename = std::strrchr(termfilename.data(), '/');

  if ( term_basename == nullptr )
    term_basename = termfilename.data();
  else
    term_basename++;

  return term_basename;
}

//----------------------------------------------------------------------
void FTermDetection::termtypeAnalysis()
{
  static auto& fterm_data = FTermData::getInstance();
  const TermTypeMap term_type_map =
  {
    // Cygwin console
    { L"cygwin"             , FTermType::cygwin },
    // rxvt terminal emulator (native MS Window System port) on cygwin
    { L"rxvt-cygwin-native" , FTermType::rxvt },
    // ANSI X3.64 terminal
    { L"ansi"               , FTermType::ansi },
    // Sun Microsystems workstation console
    { L"sun"                , FTermType::sun_con },
    // Kterm
    { L"kterm"              , FTermType::kterm },
    // mlterm
    { L"mlterm"             , FTermType::mlterm },
    // st - simple terminal
    { L"st-256color"        , FTermType::stterm },
    // rxvt
    { L"rxvt"               , FTermType::rxvt },
    // urxvt
    { L"rxvt-unicode"       , FTermType::urxvt },
    // screen or tmux with termtype screen
    { L"screen"             , FTermType::screen },
    // tmux
    { L"tmux"               , FTermType::tmux },
    // Linux console
    { L"linux"              , FTermType::linux_con },
    { L"con"                , FTermType::linux_con },
    // NetBSD workstation console
    { L"wsvt25"             , FTermType::netbsd_con },
    // kitty
    { L"xterm-kitty"        , FTermType::kitty },
    // alacritty
    { L"alacritty"          , FTermType::xterm }
  };

  auto iter = findMatchingTerm(term_type_map);

  if ( iter == term_type_map.end() )  // not found
    return;

  fterm_data.setTermType (iter->second);

  if ( isTerminalWithoutDetection() )
    terminal_detection = false;

  handleScreenAndTmux();
}

//----------------------------------------------------------------------
inline auto FTermDetection::findMatchingTerm (const TermTypeMap& map) -> TermTypeMap::const_iterator
{
  auto search_predicate = [this] (const auto& pair)
  {
    return startsWithTermType(pair.first);
  };

  return std::find_if (map.begin(), map.end(), search_predicate);
}

//----------------------------------------------------------------------
inline auto FTermDetection::isTerminalWithoutDetection() const -> bool
{
  static const auto& fterm_data = FTermData::getInstance();

  return fterm_data.isTermType(FTermType::ansi)
      || fterm_data.isTermType(FTermType::sun_con)
      || fterm_data.isTermType(FTermType::kterm);
}

//----------------------------------------------------------------------
inline void FTermDetection::handleScreenAndTmux() const
{
  static auto& fterm_data = FTermData::getInstance();

  if ( fterm_data.isTermType(FTermType::screen) )
  {
    auto tmux = std::getenv("TMUX");

    if ( tmux && tmux[0] != '\0' )
      fterm_data.setTermType (FTermType::tmux);
  }
  else if ( fterm_data.isTermType(FTermType::tmux) )
    fterm_data.setTermType (FTermType::screen);
}

//----------------------------------------------------------------------
void FTermDetection::detectTerminal()
{
  // Terminal detection

  FString new_termtype{};

  if ( terminal_detection )
  {
    FTermios::setCaptureSendCharacters();
    static auto& keyboard = FKeyboard::getInstance();
    keyboard.setNonBlockingInput();

    // Initialize 256 colors terminals
    new_termtype = init_256colorTerminal();

    // Identify the terminal via the answerback-message
    new_termtype = parseAnswerbackMsg (new_termtype);

    // Identify the terminal via the secondary device attributes (SEC_DA)
    new_termtype = parseSecDA (new_termtype);

    // Determines the maximum number of colors
    new_termtype = determineMaxColor(new_termtype);

    keyboard.unsetNonBlockingInput();
    FTermios::unsetCaptureSendCharacters();
  }

  //
  // Additional termtype analysis
  //
  static auto& fterm_data = FTermData::getInstance();

  // Test if the terminal is a xterm
  if ( startsWithTermType(L"xterm") || startsWithTermType(L"Eterm") )
  {
    fterm_data.setTermType (FTermType::xterm);

    // Each xterm should be able to use at least 16 colors
    if ( ! new_termtype && termtype.getLength() == 5 )
      new_termtype = "xterm-16color";
  }
  else if ( startsWithTermType(L"ansi") )  // ANSI detection
    fterm_data.setTermType (FTermType::ansi);

  // set the new environment variable TERM
  if ( new_termtype )
  {
    setenv("TERM", new_termtype.c_str(), 1);
    termtype = std::move(new_termtype);
  }

#if defined(__CYGWIN__)
  const auto& termfilename = fterm_data.getTermFileName();

  // Fixes problem with mouse input
  if ( termfilename.substr(0, 9) == "/dev/cons" )
    FKeyboard::setNonBlockingInputSupport(false);
#endif
}

//----------------------------------------------------------------------
auto FTermDetection::init_256colorTerminal() -> FString
{
  if ( get256colorEnvString() || termtype.includes("256color") )
    color256 = true;
  else
    color256 = false;

  FString new_termtype = termtype_256color_quirks();

#if DEBUG
  if ( ! new_termtype.isEmpty() )
    termtype_256color = new_termtype;
#endif  // DEBUG

  return new_termtype;
}

//----------------------------------------------------------------------
auto FTermDetection::get256colorEnvString() -> bool
{
  // Enable 256 color capabilities
  color_env.string1 = std::getenv("COLORTERM");
  color_env.string2 = std::getenv("VTE_VERSION");
  color_env.string3 = std::getenv("XTERM_VERSION");
  color_env.string4 = std::getenv("ROXTERM_ID");
  color_env.string5 = std::getenv("KONSOLE_DBUS_SESSION");
  color_env.string6 = std::getenv("KONSOLE_DCOP");
  color_env.string7 = std::getenv("COLORFGBG");
  color_env.string8 = std::getenv("KITTY_WINDOW_ID");
  return ( ! color_env.string1.isEmpty()
        || ! color_env.string2.isEmpty()
        || ! color_env.string3.isEmpty()
        || ! color_env.string4.isEmpty()
        || ! color_env.string5.isEmpty()
        || ! color_env.string6.isEmpty()
        || ! color_env.string7.isEmpty()
        || ! color_env.string8.isEmpty() );
}

//----------------------------------------------------------------------
auto FTermDetection::termtype_256color_quirks() -> FString
{
  FString new_termtype{};
  static auto& fterm_data = FTermData::getInstance();

  if ( ! color_env.string2.isEmpty()
    || color_env.string1 == "gnome-terminal" )
  {
    fterm_data.setTermType (FTermType::gnome_terminal);
    // Each gnome-terminal should be able to use 256 colors
    color256 = true;

    if ( ! fterm_data.isTermType(FTermType::screen) )
    {
      new_termtype = "gnome-256color";
      return new_termtype;
    }
  }

  if ( ! color256 )
    return new_termtype;

  if ( startsWithTermType(L"xterm") )
    new_termtype = "xterm-256color";

  if ( startsWithTermType(L"screen") )
    new_termtype = "screen-256color";

  if ( startsWithTermType(L"Eterm") )
    new_termtype = "Eterm-256color";

  if ( startsWithTermType(L"mlterm") )
    new_termtype = "mlterm-256color";

  if ( startsWithTermType(L"rxvt")
    && color_env.string1.left(8) == "rxvt-xpm" )
  {
    new_termtype = "rxvt-256color";
    fterm_data.setTermType (FTermType::rxvt);
  }

  if ( color_env.string5.getLength() > 0
    || color_env.string6.getLength() > 0 )
  {
    fterm_data.setTermType (FTermType::kde_konsole);
    new_termtype = "konsole-256color";
  }

  if ( color_env.string3.getLength() > 0 )
    decscusr_support = true;

  return new_termtype;
}

//----------------------------------------------------------------------
auto FTermDetection::determineMaxColor (const FString& current_termtype) -> FString
{
  // Determine xterm maximum number of colors via OSC 4

  FString new_termtype{current_termtype};
  static const auto& fterm_data = FTermData::getInstance();
  static auto& keyboard = FKeyboard::getInstance();
  keyboard.setNonBlockingInput();

  if ( ! color256
    && ! fterm_data.isTermType ( FTermType::cygwin
                               | FTermType::tera_term
                               | FTermType::linux_con
                               | FTermType::netbsd_con )
    && ! getXTermColorName(FColor(0)).isEmpty() )
  {
    if ( ! getXTermColorName(FColor(255)).isEmpty() )
    {
      color256 = true;

      if ( fterm_data.isTermType(FTermType::putty) )
        new_termtype = "putty-256color";
      else
        new_termtype = "xterm-256color";
    }
    else if ( ! getXTermColorName(FColor(87)).isEmpty() )
    {
      new_termtype = "xterm-88color";
    }
    else if ( ! getXTermColorName(FColor(15)).isEmpty() )
    {
      new_termtype = "xterm-16color";
    }
  }

  keyboard.unsetNonBlockingInput();
  return new_termtype;
}

//----------------------------------------------------------------------
auto FTermDetection::getXTermColorName (FColor color) const -> FString
{
  // get color
  auto index = uInt16(color);
  std::fprintf (stdout, OSC "4;%hu;?" BEL, index);
  std::fflush (stdout);
  std::array<char, 35> temp{};
  auto pos = captureTerminalInput(temp, 150'000);
  constexpr auto parse = "\033]4;%10hu;%509[^\n]s";
  std::array<char, 30> buf{};

  if ( pos > 4 && std::sscanf(temp.data(), parse, &index, buf.data()) == 2 )
  {
    auto n = stringLength(buf.data());

    // BEL + '\0' = string terminator
    if ( n >= 6 && buf[n - 1] == BEL[0] && buf[n] == '\0' )
      buf[n - 1] = '\0';

    // Esc + \ = OSC string terminator (mintty)
    if ( n >= 6 && buf[n - 2] == ESC[0] && buf[n - 1] == '\\' )
      buf[n - 2] = '\0';

    return buf.data();
  }

  return {};
}

//----------------------------------------------------------------------
auto FTermDetection::parseAnswerbackMsg (const FString& current_termtype) -> FString
{
  FString new_termtype{current_termtype};
  static auto& keyboard = FKeyboard::getInstance();
  keyboard.setNonBlockingInput();
  // send ENQ and read the answerback message
  answer_back = getAnswerbackMsg();
  keyboard.unsetNonBlockingInput();

  if ( answer_back == "PuTTY" )
  {
    static auto& fterm_data = FTermData::getInstance();
    fterm_data.setTermType (FTermType::putty);

    if ( color256 )
      new_termtype = "putty-256color";
    else
      new_termtype = "putty";
  }

#if !defined(UNIT_TEST)
  // Some terminals like cygwin or the Windows terminal
  // have to delete the printed character '♣'
  std::fprintf (stdout, "\r " BS);
  std::fflush (stdout);
#endif  // !defined(UNIT_TEST)

#if DEBUG
  if ( ! new_termtype.isEmpty() )
    termtype_Answerback = new_termtype;
#endif  // DEBUG

  return new_termtype;
}

//----------------------------------------------------------------------
auto FTermDetection::getAnswerbackMsg() const -> FString
{
  // Send enquiry character
  std::putchar (ENQ[0]);
  std::fflush(stdout);
  std::array<char, 10> temp{};
  auto pos = captureTerminalInput(temp, 150'000);

  if ( pos > 0 )
    return temp.data();

  return {};
}

//----------------------------------------------------------------------
auto FTermDetection::parseSecDA (const FString& current_termtype) -> FString
{
  // The Linux console and older cygwin terminals knows no Sec_DA

  const auto& fterm_data = FTermData::getInstance();

  if ( fterm_data.isTermType(FTermType::linux_con | FTermType::cygwin) )
    return current_termtype;

  // Secondary device attributes (SEC_DA) <- decTerminalID string
  sec_da = getSecDA();

  if ( sec_da.getLength() < 6 )
    return current_termtype;

  // remove the first 3 bytes ("\033[>")
  FString temp(sec_da.right(sec_da.getLength() - 3));
  // remove the last byte ("c")
  temp.remove(temp.getLength() - 1, 1);
  // split into components
  const auto& sec_da_components = temp.split(';');
  const auto& num_components = sec_da_components.size();

  // The second device attribute (SEC_DA) always has 3 parameters,
  // otherwise it usually has a copy of the device attribute (primary DA)
  if ( num_components < 3 )
    return current_termtype;

  if ( sec_da_components[0].isEmpty() )
    return current_termtype;

  // Read the terminal type
  secondary_da.terminal_id_type = str2int(sec_da_components[0]);

  // Read the terminal (firmware) version
  secondary_da.terminal_id_version = str2int(sec_da_components[1]);

  // Read the terminal hardware option
  secondary_da.terminal_id_hardware = str2int(sec_da_components[2]);

  const auto& new_termtype = secDA_Analysis(current_termtype);

#if DEBUG
  if ( ! new_termtype.isEmpty() )
    termtype_SecDA = new_termtype;
#endif  // DEBUG

  return new_termtype;
}

//----------------------------------------------------------------------
auto FTermDetection::str2int (const FString& s) const -> int
{
  // This is not a general string to integer conversion method.
  // It is only used in this class to convert the device attribute
  // parameters into numbers.

  constexpr int ERROR = -1;

  if ( s.isEmpty() )
    return ERROR;

  try
  {
    return s.toInt();
  }
  catch (const std::invalid_argument&)
  {
    return ERROR;
  }
  catch (const std::underflow_error&)
  {
    return ERROR;
  }
  catch (const std::overflow_error&)
  {
    return ERROR;
  }
}

//----------------------------------------------------------------------
auto FTermDetection::getSecDA() const -> FString
{
  const auto& stdout_no{FTermios::getStdOut()};
  const std::string SECDA{ESC "[>c"};

  // Get the secondary device attributes
  if ( write(stdout_no, SECDA.data(), SECDA.length()) == -1 )
    return {};

  std::fflush(stdout);
  std::array<char, 40> temp{};
  auto isWithout_c = [] (const auto& t) { return ! std::strchr(t.data(), 'c'); };
  auto pos = captureTerminalInput(temp, 600'000, isWithout_c);
  constexpr auto parse = "\033[>%10d;%10d;%10dc";
  FString sec_da_str{""};
  int a{0};
  int b{0};
  int c{0};

  if ( pos > 3 && std::sscanf(temp.data(), parse, &a, &b, &c) == 3 )
    sec_da_str.sprintf("\033[>%d;%d;%dc", a, b, c);

  return sec_da_str;
}

//----------------------------------------------------------------------
auto FTermDetection::secDA_Analysis (const FString& current_termtype) -> FString
{
  FString new_termtype{current_termtype};
  int terminal_id_type = secondary_da.terminal_id_type;

  // Define a map that associates terminal ID types with functions
  std::unordered_map<int, std::function<FString(const FString&)>> analysis_functions =
  {
    // DEC VT100
    { 0, [this] (const auto& s) { return secDA_Analysis_0(s); } },
    // DEC VT220
    { 1, [this] (const auto& s) { return secDA_Analysis_1(s); } },
    // DEC VT240
    { 2, [this] (const auto& s) { return secDA_Analysis_24(s); } },
    // DEC VT330
    { 18, [this] (const auto& s) { return secDA_Analysis_24(s); } },
    // DEC VT340
    { 19, [this] (const auto& s) { return secDA_Analysis_24(s); } },
    // DEC VT320
    { 24, [this] (const auto& s) { return secDA_Analysis_24(s); } },
    // Tera Term
    { 32, [this] (const auto&) { return secDA_Analysis_32(); } },
    // DEC VT420
    { 41, [] (const auto& s) { return s; } },
    // DEC VT510
    { 61, [] (const auto& s) { return s; } },
    // DEC VT520
    { 64, [] (const auto& s) { return s; } },
    // DEC VT525
    { 65, [this] (const auto& s) { return secDA_Analysis_65(s); } },
    // Cygwin
    { 67, [this] (const auto&) { return secDA_Analysis_67(); } },
    // mintty
    { 77, [this] (const auto&) { return secDA_Analysis_77(); } },
    // rxvt
    { 82, [this] (const auto&) { return secDA_Analysis_82(); } },
    // screen
    { 83, [this] (const auto& s) { return secDA_Analysis_83(s); } },
    // tmux
    { 84, [this] (const auto& s) { return secDA_Analysis_84(s); } },
    // rxvt-unicode
    { 85, [this] (const auto&) { return secDA_Analysis_85(); } }
  };

  // Check if the terminal ID type is in the map
  if ( analysis_functions.find(terminal_id_type) != analysis_functions.end() )
    new_termtype = analysis_functions[terminal_id_type](current_termtype);

  correctFalseAssumptions(terminal_id_type);
  return new_termtype;
}

//----------------------------------------------------------------------
inline auto FTermDetection::secDA_Analysis_0 (const FString& current_termtype) const -> FString
{
  // Terminal ID 0 - DEC VT100

  FString new_termtype{current_termtype};
  auto& fterm_data = FTermData::getInstance();

  if ( secondary_da.terminal_id_version == 10
    && secondary_da.terminal_id_hardware == 1 )
    fterm_data.setTermType (FTermType::win_terminal);  // Windows Terminal >= 1.2
  else if ( secondary_da.terminal_id_version == 115 )
    fterm_data.setTermType (FTermType::kde_konsole);
  else if ( secondary_da.terminal_id_version == 136 )
    fterm_data.setTermType (FTermType::putty);  // PuTTY

#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  if ( FTermFreeBSD::isFreeBSDConsole() )
    fterm_data.setTermType (FTermType::freebsd_con);
#endif

  return new_termtype;
}

//----------------------------------------------------------------------
inline auto FTermDetection::secDA_Analysis_1 (const FString& current_termtype) -> FString
{
  // Terminal ID 1 - DEC VT220

  FString new_termtype{current_termtype};
  const auto& fterm_data = FTermData::getInstance();

  if ( fterm_data.isTermType(FTermType::kitty) )
    new_termtype = secDA_Analysis_kitty(new_termtype);
  else
    new_termtype = secDA_Analysis_vte(new_termtype);

  return new_termtype;
}

//----------------------------------------------------------------------
inline auto FTermDetection::secDA_Analysis_24 (const FString& current_termtype) -> FString
{
  // Terminal ID 24 - DEC VT320

  FString new_termtype{current_termtype};

#if defined(__NetBSD__) || defined(__OpenBSD__) || defined(UNIT_TEST)

  if ( secondary_da.terminal_id_version == 20
     && FTermOpenBSD::isBSDConsole() )
  {
    // NetBSD/OpenBSD workstation console
    auto& fterm_data = FTermData::getInstance();

    if ( startsWithTermType(L"wsvt25") )
      fterm_data.setTermType (FTermType::netbsd_con);
    else if ( startsWithTermType(L"vt220") )
    {
      fterm_data.setTermType (FTermType::openbsd_con);
      new_termtype = "pccon";
    }
  }

#endif  // defined(__NetBSD__) || defined(__OpenBSD__) || defined(UNIT_TEST)

  return new_termtype;
}

//----------------------------------------------------------------------
inline auto FTermDetection::secDA_Analysis_32() const -> FString
{
  // Terminal ID 32 - Tera Term

  auto& fterm_data = FTermData::getInstance();
  fterm_data.setTermType (FTermType::tera_term);
  return "teraterm";
}

//----------------------------------------------------------------------
inline auto FTermDetection::secDA_Analysis_65 (const FString& current_termtype) -> FString
{
  // Terminal ID 65 - DEC VT525 and VTE >= 0.53.0

  return secDA_Analysis_1 (current_termtype);
}

//----------------------------------------------------------------------
inline auto FTermDetection::secDA_Analysis_67() const -> FString
{
  // Terminal ID 67 - cygwin

  auto& fterm_data = FTermData::getInstance();
  fterm_data.setTermType (FTermType::cygwin);
  std::fflush(stdout);
  return "cygwin";
}

//----------------------------------------------------------------------
inline auto FTermDetection::secDA_Analysis_77() -> FString
{
  // Terminal ID 77 - mintty

  auto& fterm_data = FTermData::getInstance();
  fterm_data.setTermType (FTermType::mintty);
  decscusr_support = true;
  std::fflush(stdout);
  return "xterm-256color";
}

//----------------------------------------------------------------------
inline auto FTermDetection::secDA_Analysis_82() const -> FString
{
  // Terminal ID 82 - rxvt

  auto& fterm_data = FTermData::getInstance();
  fterm_data.setTermType (FTermType::rxvt);

  if ( termtype == "rxvt-cygwin-native" )
    return FString("rxvt-16color");

  return FString("rxvt");
}

//----------------------------------------------------------------------
inline auto FTermDetection::secDA_Analysis_83 (const FString& current_termtype) const -> FString
{
  // Terminal ID 83 - screen

  auto& fterm_data = FTermData::getInstance();
  fterm_data.setTermType (FTermType::screen);
  return current_termtype;
}

//----------------------------------------------------------------------
inline auto FTermDetection::secDA_Analysis_84 (const FString& current_termtype) const -> FString
{
  // Terminal ID 84 - tmux

  auto& fterm_data = FTermData::getInstance();
  fterm_data.setTermType (FTermType::screen);
  fterm_data.setTermType (FTermType::tmux);
  return current_termtype;
}

//----------------------------------------------------------------------
inline auto FTermDetection::secDA_Analysis_85() const -> FString
{
  // Terminal ID 85 - rxvt-unicode

  auto& fterm_data = FTermData::getInstance();
  fterm_data.setTermType (FTermType::rxvt);
  fterm_data.setTermType (FTermType::urxvt);

  if ( ! startsWithTermType(L"rxvt-") )
    return termtype;

  if ( color256 )
    return FString("rxvt-256color");

  return FString("rxvt");
}

//----------------------------------------------------------------------
inline auto FTermDetection::secDA_Analysis_vte (const FString& current_termtype) -> FString
{
  // VTE terminal library
  // (Since VTE 0.53.0 the terminal ID has changed from 1 to 65)

  FString new_termtype{current_termtype};

  if ( secondary_da.terminal_id_version > 1000 )
  {
    auto& fterm_data = FTermData::getInstance();
    fterm_data.setTermType (FTermType::gnome_terminal);
    // Each gnome-terminal should be able to use 256 colors
    color256 = true;
    new_termtype = "gnome-256color";
    const auto& id = secondary_da.terminal_id_version;
    fterm_data.setGnomeTerminalID(id);

    // VTE 0.40.0 or higher and gnome-terminal 3.16 or higher
    if ( id >= 4000 )
      decscusr_support = true;
  }

  return new_termtype;
}

//----------------------------------------------------------------------
inline auto FTermDetection::secDA_Analysis_kitty (const FString& current_termtype) -> FString
{
  // kitty

  FString new_termtype{current_termtype};

  if ( secondary_da.terminal_id_version > 3999 )
  {
    // All kitty terminals can use 256 colors
    color256 = true;
    new_termtype = "xterm-kitty";
    const auto& n1 = secondary_da.terminal_id_version - 4000;
    const auto& n2 = secondary_da.terminal_id_hardware;
    FTermData::kittyVersion kitty_version { n1, n2 };
    FTermData::getInstance().setKittyVersion(kitty_version);
  }

  return new_termtype;
}

//----------------------------------------------------------------------
inline void FTermDetection::correctFalseAssumptions (int terminal_id_type) const
{
  // Correct false assumptions
  auto& fterm_data = FTermData::getInstance();

  if ( fterm_data.isTermType(FTermType::gnome_terminal)
    && terminal_id_type != 1
    && terminal_id_type != 65 )
    fterm_data.unsetTermType (FTermType::gnome_terminal);

  if ( fterm_data.isTermType(FTermType::kde_konsole)
    && terminal_id_type != 0 )
    fterm_data.unsetTermType (FTermType::kde_konsole);
}

}  // namespace finalcut
