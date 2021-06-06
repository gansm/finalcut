/***********************************************************************
* ftermdetection.cpp - Detection of the terminal type                  *
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

#if defined(__CYGWIN__)
  #include "final/fconfig.h"  // includes _GNU_SOURCE for fd_set
#endif

#include <array>

#include "final/emptyfstring.h"
#include "final/fapplication.h"
#include "final/fc.h"
#include "final/flog.h"
#include "final/fkeyboard.h"
#include "final/fsystem.h"
#include "final/fterm.h"
#include "final/ftermcap.h"
#include "final/ftermdata.h"
#include "final/ftermdetection.h"
#include "final/ftermios.h"
#include "final/ftypes.h"

#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  #include "final/ftermfreebsd.h"
#endif

#if defined(__NetBSD__) || defined(__OpenBSD__) || defined(UNIT_TEST)
  #include "final/ftermopenbsd.h"
#endif

namespace finalcut
{

//----------------------------------------------------------------------
// class FTermDetection
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FTermDetection::FTermDetection() = default;

//----------------------------------------------------------------------
FTermDetection::~FTermDetection() = default;  // destructor


// public methods of FTermDetection
//----------------------------------------------------------------------
#if DEBUG
const FString& FTermDetection::getAnswerbackString() const
{
  return answer_back;
}

//----------------------------------------------------------------------
const FString& FTermDetection::getSecDAString() const
{
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
  const auto& termfilename = FTerm::getFTermData().getTermFileName();

  if ( term_env )
  {
    // Save name in termtype
    termtype = term_env;
    return;
  }
  else if ( ! termfilename.empty() )  // 1st fallback: use the teminal file name
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
bool FTermDetection::getTTYtype()
{
  // Analyse /etc/ttytype and get the term name
  // ------------------------------------------
  // file format:
  // <terminal type> <whitespace> <tty name>
  //
  // Example:
  // linux  tty1
  // vt100  ttys0

  // Get term basename
  const auto& termfilename = FTerm::getFTermData().getTermFileName();
  const char* term_basename = std::strrchr(termfilename.data(), '/');

  if ( term_basename == nullptr )
    term_basename = termfilename.data();
  else
    term_basename++;

  std::FILE* fp{};
  std::array<char, BUFSIZ> str{};
  const auto& fsystem = FTerm::getFSystem();

  if ( (fp = fsystem->fopen(ttytypename.c_str(), "r")) == nullptr )
    return false;

  // Read and parse the file
  while ( fgets(str.data(), str.size() - 1, fp) != nullptr )
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
      fsystem->fclose(fp);
      return true;
    }
  }

  fsystem->fclose(fp);
  return false;
}

#if F_HAVE_GETTTYNAM
//----------------------------------------------------------------------
bool FTermDetection::getTTYSFileEntry()
{
  // Analyse /etc/ttys and get the term name

  // get term basename
  const auto& termfilename = FTerm::getFTermData().getTermFileName();
  const char* term_basename = std::strrchr(termfilename.data(), '/');

  if ( term_basename == nullptr )
    term_basename = termfilename.data();
  else
    term_basename++;

  const struct ttyent* ttys_entryt;
  ttys_entryt = getttynam(term_basename);

  if ( ttys_entryt )
  {
    const char* type = ttys_entryt->ty_type;

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
void FTermDetection::termtypeAnalysis()
{
  // Cygwin console

  if ( termtype.left(6) == "cygwin" )
    terminal_type.cygwin = true;

  // rxvt terminal emulator (native MS Window System port) on cygwin
  if ( termtype == "rxvt-cygwin-native" )
    terminal_type.rxvt = true;

  // Ansi terminal
  if ( termtype.left(4) == "ansi" )
  {
    terminal_detection = false;
    terminal_type.ansi = true;
  }

  // Sun Microsystems workstation console
  if ( termtype.left(3) == "sun" )
  {
    terminal_detection = false;
    terminal_type.sun_con = true;
  }

  // Kterm
  if ( termtype.left(5) == "kterm" )
  {
    terminal_detection = false;
    terminal_type.kterm = true;
  }

  // mlterm
  if ( termtype.left(6) == "mlterm" )
    terminal_type.mlterm = true;

  // rxvt
  if ( termtype.left(4) == "rxvt" )
    terminal_type.rxvt = true;

  // urxvt
  if ( termtype.left(12) == "rxvt-unicode" )
    terminal_type.urxvt = true;

  // screen/tmux
  if ( termtype.left(6) == "screen" )
  {
    terminal_type.screen = true;
    auto tmux = std::getenv("TMUX");

    if ( tmux && tmux[0] != '\0' )
      terminal_type.tmux = true;
  }

  // Linux console
  if ( termtype.left(5) == "linux" || termtype.left(3) == "con" )
    terminal_type.linux_con = true;

  // NetBSD workstation console
  if ( termtype.left(6) == "wsvt25" )
    terminal_type.netbsd_con = true;

  // kitty
  if ( termtype.left(11) == "xterm-kitty" )
    terminal_type.kitty = true;
}

//----------------------------------------------------------------------
void FTermDetection::detectTerminal()
{
  // Terminal detection

  FString new_termtype{};

  if ( terminal_detection )
  {
    FTermios::setCaptureSendCharacters();
    auto& keyboard = FTerm::getFKeyboard();
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

  // Test if the terminal is a xterm
  if ( termtype.left(5) == "xterm" || termtype.left(5) == "Eterm" )
  {
    terminal_type.xterm = true;

    // Each xterm should be able to use at least 16 colors
    if ( ! new_termtype && termtype.getLength() == 5 )
      new_termtype = "xterm-16color";
  }
  else if ( termtype.left(4) == "ansi" )  // ANSI detection
    terminal_type.ansi = true;

  // set the new environment variable TERM
  if ( new_termtype )
  {
    setenv("TERM", new_termtype.c_str(), 1);
    termtype = new_termtype;
  }

#if defined(__CYGWIN__)
  auto& fterm_data = FTerm::getFTermData();
  const auto& termfilename = fterm_data.getTermFileName();

  // Fixes problem with mouse input
  if ( termfilename.substr(0, 9) == "/dev/cons" )
    FKeyboard::setNonBlockingInputSupport(false);
#endif
}

//----------------------------------------------------------------------
FString FTermDetection::init_256colorTerminal()
{
  FString new_termtype{};

  if ( get256colorEnvString() )
    color256 = true;
  else if ( termtype.includes("256color") )
    color256 = true;
  else
    color256 = false;

  new_termtype = termtype_256color_quirks();

#if DEBUG
  if ( ! new_termtype.isEmpty() )
    termtype_256color = new_termtype;
#endif  // DEBUG

  return new_termtype;
}

//----------------------------------------------------------------------
bool FTermDetection::get256colorEnvString()
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

  if ( ! color_env.string1.isEmpty()
    || ! color_env.string2.isEmpty()
    || ! color_env.string3.isEmpty()
    || ! color_env.string4.isEmpty()
    || ! color_env.string5.isEmpty()
    || ! color_env.string6.isEmpty()
    || ! color_env.string7.isEmpty()
    || ! color_env.string8.isEmpty() )
    return true;

  return false;
}

//----------------------------------------------------------------------
FString FTermDetection::termtype_256color_quirks()
{
  FString new_termtype{};

  if ( ! color_env.string2.isEmpty()
    || color_env.string1 == "gnome-terminal" )
  {
    terminal_type.gnome_terminal = true;
    // Each gnome-terminal should be able to use 256 colors
    color256 = true;

    if ( ! isScreenTerm() )
    {
      new_termtype = "gnome-256color";
      return new_termtype;
    }
  }

  if ( ! color256 )
    return new_termtype;

  if ( termtype.left(5) == "xterm" )
    new_termtype = "xterm-256color";

  if ( termtype.left(6) == "screen" )
    new_termtype = "screen-256color";

  if ( termtype.left(5) == "Eterm" )
    new_termtype = "Eterm-256color";

  if ( termtype.left(6) == "mlterm" )
    new_termtype = "mlterm-256color";

  if ( termtype.left(4) == "rxvt"
    && color_env.string1.left(8) == "rxvt-xpm" )
  {
    new_termtype = "rxvt-256color";
    terminal_type.rxvt = true;
  }

  if ( color_env.string5.getLength() > 0
    || color_env.string6.getLength() > 0 )
  {
    terminal_type.kde_konsole = true;
    new_termtype = "konsole-256color";
  }

  if ( color_env.string3.getLength() > 0 )
    decscusr_support = true;

  return new_termtype;
}

//----------------------------------------------------------------------
FString FTermDetection::determineMaxColor (const FString& current_termtype)
{
  // Determine xterm maximum number of colors via OSC 4

  FString new_termtype{current_termtype};
  auto& keyboard = FTerm::getFKeyboard();
  keyboard.setNonBlockingInput();

  if ( ! color256
    && ! isCygwinTerminal()
    && ! isTeraTerm()
    && ! isLinuxTerm()
    && ! isNetBSDTerm()
    && ! getXTermColorName(FColor(0)).isEmpty() )
  {
    if ( ! getXTermColorName(FColor(255)).isEmpty() )
    {
      color256 = true;

      if ( isPuttyTerminal() )
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
FString FTermDetection::getXTermColorName (FColor color) const
{
  FString color_str{""};
  std::array<char, 30> buf{};
  fd_set ifds{};
  struct timeval tv{};
  const int stdin_no = FTermios::getStdIn();

  // get color
  auto index = uInt16(color);
  std::fprintf (stdout, OSC "4;%hu;?" BEL, index);
  std::fflush (stdout);
  FD_ZERO(&ifds);
  FD_SET(stdin_no, &ifds);
  tv.tv_sec  = 0;
  tv.tv_usec = 150000;  // 150 ms

  // read the terminal answer
  if ( select (stdin_no + 1, &ifds, nullptr, nullptr, &tv) < 1 )
    return color_str;

  constexpr auto parse = "\033]4;%10hu;%509[^\n]s";
  std::array<char, 35> temp{};
  std::size_t pos{0};

  do
  {
    std::size_t bytes_free = temp.size() - pos - 1;
    const ssize_t bytes = read(stdin_no, &temp[pos], bytes_free);

    if ( bytes <= 0 )
      break;

    pos += std::size_t(bytes);
  }
  while ( pos < temp.size() );

  if ( pos > 4 && std::sscanf(temp.data(), parse, &index, buf.data()) == 2 )
  {
    auto n = stringLength(buf.data());

    // BEL + '\0' = string terminator
    if ( n >= 6 && buf[n - 1] == BEL[0] && buf[n] == '\0' )
      buf[n - 1] = '\0';

    // Esc + \ = OSC string terminator (mintty)
    if ( n >= 6 && buf[n - 2] == ESC[0] && buf[n - 1] == '\\' )
      buf[n - 2] = '\0';

    color_str = buf.data();
  }

  return color_str;
}

//----------------------------------------------------------------------
FString FTermDetection::parseAnswerbackMsg (const FString& current_termtype)
{
  FString new_termtype{current_termtype};
  auto& keyboard = FTerm::getFKeyboard();
  keyboard.setNonBlockingInput();
  // send ENQ and read the answerback message
  answer_back = getAnswerbackMsg();
  keyboard.unsetNonBlockingInput();

  if ( answer_back == "PuTTY" )
  {
    terminal_type.putty = true;

    if ( color256 )
      new_termtype = "putty-256color";
    else
      new_termtype = "putty";
  }

  // Some terminals like cygwin or the Windows terminal
  // have to delete the printed character '♣'
  std::fprintf (stdout, "\r " BS);
  std::fflush (stdout);

#if DEBUG
  if ( ! new_termtype.isEmpty() )
    termtype_Answerback = new_termtype;
#endif  // DEBUG

  return new_termtype;
}

//----------------------------------------------------------------------
FString FTermDetection::getAnswerbackMsg() const
{
  FString answerback{""};
  fd_set ifds{};
  struct timeval tv{};
  const int stdin_no = FTermios::getStdIn();
  // Send enquiry character
  std::putchar (ENQ[0]);
  std::fflush(stdout);
  FD_ZERO(&ifds);
  FD_SET(stdin_no, &ifds);
  tv.tv_sec  = 0;
  tv.tv_usec = 150000;  // 150 ms

  // Read the answerback message
  if ( select (stdin_no + 1, &ifds, nullptr, nullptr, &tv) < 1 )
    return answerback;

  std::array<char, 10> temp{};
  std::size_t pos{0};

  do
  {
    std::size_t bytes_free = temp.size() - pos - 1;
    const ssize_t bytes = read(stdin_no, &temp[pos], bytes_free);

    if ( bytes <= 0 )
      break;

    pos += std::size_t(bytes);
  }
  while ( pos < temp.size() );

  if ( pos > 0 )
    answerback = temp.data();

  return answerback;
}

//----------------------------------------------------------------------
FString FTermDetection::parseSecDA (const FString& current_termtype)
{
  // The Linux console and older cygwin terminals knows no Sec_DA
  if ( isLinuxTerm() || isCygwinTerminal() )
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
  const auto sec_da_components = temp.split(';');
  const auto num_components = sec_da_components.size();

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

  FString new_termtype = secDA_Analysis(current_termtype);

#if DEBUG
  if ( ! new_termtype.isEmpty() )
    termtype_SecDA = new_termtype;
#endif  // DEBUG

  return new_termtype;
}

//----------------------------------------------------------------------
int FTermDetection::str2int (const FString& s) const
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
FString FTermDetection::getSecDA() const
{
  FString sec_da_str{""};

  int a{0};
  int b{0};
  int c{0};
  const int stdin_no{FTermios::getStdIn()};
  const int stdout_no{FTermios::getStdOut()};
  fd_set ifds{};
  struct timeval tv{};
  const std::string SECDA{ESC "[>c"};

  // Get the secondary device attributes
  if ( write(stdout_no, SECDA.data(), SECDA.length()) == -1 )
    return sec_da_str;

  std::fflush(stdout);
  FD_ZERO(&ifds);
  FD_SET(stdin_no, &ifds);
  tv.tv_sec  = 0;
  tv.tv_usec = 600000;  // 600 ms

  // Read the answer
  if ( select (stdin_no + 1, &ifds, nullptr, nullptr, &tv) < 1 )
    return sec_da_str;

  constexpr auto parse = "\033[>%10d;%10d;%10dc";
  std::array<char, 40> temp{};
  std::size_t pos{0};

  do
  {
    std::size_t bytes_free = temp.size() - pos - 1;
    const ssize_t bytes = read(stdin_no, &temp[pos], bytes_free);

    if ( bytes <= 0 )
      break;

    pos += std::size_t(bytes);
  }
  while ( pos < temp.size() && ! std::strchr(temp.data(), 'c') );

  if ( pos > 3 && std::sscanf(temp.data(), parse, &a, &b, &c) == 3 )
    sec_da_str.sprintf("\033[>%d;%d;%dc", a, b, c);

  return sec_da_str;
}

//----------------------------------------------------------------------
FString FTermDetection::secDA_Analysis (const FString& current_termtype)
{
  FString new_termtype{current_termtype};

  switch ( secondary_da.terminal_id_type )
  {
    case 0:  // DEC VT100
      new_termtype = secDA_Analysis_0(current_termtype);
      break;

    case 1:  // DEC VT220
      new_termtype = secDA_Analysis_1(current_termtype);
      break;

    case 2:   // DEC VT240
    case 18:  // DEC VT330
    case 19:  // DEC VT340

    case 24:  // DEC VT320
      new_termtype = secDA_Analysis_24(current_termtype);
      break;

    case 32:  // Tera Term
      new_termtype = secDA_Analysis_32();
      break;

    case 41:  // DEC VT420
    case 61:  // DEC VT510
    case 64:  // DEC VT520
      break;

    case 65:  // DEC VT525
      new_termtype = secDA_Analysis_65(current_termtype);
      break;

    case 67:  // Cygwin
      new_termtype = secDA_Analysis_67();
      break;

    case 77:  // mintty
      new_termtype = secDA_Analysis_77();
      break;

    case 82:  // rxvt
      new_termtype = secDA_Analysis_82();
      break;

    case 83:  // screen
      new_termtype = secDA_Analysis_83(current_termtype);
      break;

    case 84:  // tmux
      new_termtype = secDA_Analysis_84(current_termtype);
      break;

    case 85:  // rxvt-unicode
      new_termtype = secDA_Analysis_85();
      break;

    default:
      break;
  }

  // Correct false assumptions
  if ( isGnomeTerminal()
    && secondary_da.terminal_id_type != 1
    && secondary_da.terminal_id_type != 65 )
    terminal_type.gnome_terminal = false;

  if ( isKdeTerminal() && secondary_da.terminal_id_type != 0 )
    terminal_type.kde_konsole = false;

  return new_termtype;
}

//----------------------------------------------------------------------
inline FString FTermDetection::secDA_Analysis_0 (const FString& current_termtype)
{
  // Terminal ID 0 - DEC VT100

  FString new_termtype{current_termtype};

  if ( secondary_da.terminal_id_version == 10
    && secondary_da.terminal_id_hardware == 1 )
    terminal_type.win_terminal = true;  // Windows Terminal >= 1.2
  else if ( secondary_da.terminal_id_version == 115 )
    terminal_type.kde_konsole = true;
  else if ( secondary_da.terminal_id_version == 136 )
    terminal_type.putty = true;  // PuTTY

#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  if ( FTermFreeBSD::isFreeBSDConsole() )
    terminal_type.freebsd_con = true;
#endif

  return new_termtype;
}

//----------------------------------------------------------------------
inline FString FTermDetection::secDA_Analysis_1 (const FString& current_termtype)
{
  // Terminal ID 1 - DEC VT220

  FString new_termtype{current_termtype};

  if ( isKittyTerminal() )
    new_termtype = secDA_Analysis_kitty(new_termtype);
  else
    new_termtype = secDA_Analysis_vte(new_termtype);

  return new_termtype;
}

//----------------------------------------------------------------------
inline FString FTermDetection::secDA_Analysis_24 (const FString& current_termtype)
{
  // Terminal ID 24 - DEC VT320

  FString new_termtype{current_termtype};

#if defined(__NetBSD__) || defined(__OpenBSD__) || defined(UNIT_TEST)

  if ( secondary_da.terminal_id_version == 20
     && FTermOpenBSD::isBSDConsole() )
  {
    // NetBSD/OpenBSD workstation console
    if ( termtype.left(6) == "wsvt25" )
      terminal_type.netbsd_con = true;
    else if ( termtype.left(5) == "vt220" )
    {
      terminal_type.openbsd_con = true;
      new_termtype = "pccon";
    }
  }

#endif  // defined(__NetBSD__) || defined(__OpenBSD__) || defined(UNIT_TEST)

  return new_termtype;
}

//----------------------------------------------------------------------
inline FString FTermDetection::secDA_Analysis_32()
{
  // Terminal ID 32 - Tera Term

  terminal_type.tera_term = true;
  return "teraterm";
}

//----------------------------------------------------------------------
inline FString FTermDetection::secDA_Analysis_65 (const FString& current_termtype)
{
  // Terminal ID 65 - DEC VT525 and VTE >= 0.53.0

  return secDA_Analysis_1 (current_termtype);
}

//----------------------------------------------------------------------
inline FString FTermDetection::secDA_Analysis_67()
{
  // Terminal ID 67 - cygwin

  terminal_type.cygwin = true;
  std::fflush(stdout);
  return "cygwin";
}

//----------------------------------------------------------------------
inline FString FTermDetection::secDA_Analysis_77()
{
  // Terminal ID 77 - mintty

  terminal_type.mintty = true;
  decscusr_support = true;
  std::fflush(stdout);
  return "xterm-256color";
}

//----------------------------------------------------------------------
inline FString FTermDetection::secDA_Analysis_82()
{
  // Terminal ID 82 - rxvt

  FString new_termtype{};
  terminal_type.rxvt = true;

  if ( termtype == "rxvt-cygwin-native" )
    new_termtype = "rxvt-16color";
  else
    new_termtype = "rxvt";

  return new_termtype;
}

//----------------------------------------------------------------------
inline FString FTermDetection::secDA_Analysis_83 (const FString& current_termtype)
{
  // Terminal ID 83 - screen

  terminal_type.screen = true;
  return current_termtype;
}

//----------------------------------------------------------------------
inline FString FTermDetection::secDA_Analysis_84 (const FString& current_termtype)
{
  // Terminal ID 84 - tmux

  terminal_type.screen = true;
  terminal_type.tmux = true;
  return current_termtype;
}

//----------------------------------------------------------------------
inline FString FTermDetection::secDA_Analysis_85()
{
  // Terminal ID 85 - rxvt-unicode

  FString new_termtype{};
  terminal_type.rxvt = true;
  terminal_type.urxvt = true;

  if ( termtype.left(5) == "rxvt-" )
  {
    if ( color256 )
      new_termtype = "rxvt-256color";
    else
      new_termtype = "rxvt";
  }
  else
    new_termtype = termtype;

  return new_termtype;
}

//----------------------------------------------------------------------
inline FString FTermDetection::secDA_Analysis_vte (const FString& current_termtype)
{
  // VTE terminal library
  // (Since VTE ) the terminal ID has changed from 1 to 65)

  FString new_termtype{current_termtype};

  if ( secondary_da.terminal_id_version > 1000 )
  {
    terminal_type.gnome_terminal = true;
    // Each gnome-terminal should be able to use 256 colors
    color256 = true;
    new_termtype = "gnome-256color";
    gnome_terminal_id = secondary_da.terminal_id_version;

    // VTE 0.40.0 or higher and gnome-terminal 3.16 or higher
    if ( gnome_terminal_id >= 4000 )
      decscusr_support = true;
  }

  return new_termtype;
}

//----------------------------------------------------------------------
inline FString FTermDetection::secDA_Analysis_kitty (const FString& current_termtype)
{
  // kitty

  FString new_termtype{current_termtype};

  if ( secondary_da.terminal_id_version > 3999 )
  {
    // All kitty terminals can use 256 colors
    color256 = true;
    new_termtype = "xterm-kitty";
    kitty_version.primary = secondary_da.terminal_id_version - 4000;
    kitty_version.secondary = secondary_da.terminal_id_hardware;
  }

  return new_termtype;
}

}  // namespace finalcut
