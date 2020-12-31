/***********************************************************************
* ftermdetection.cpp - Detection of the terminal type                  *
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

// static class attributes
FTermDetection::FTerminalType FTermDetection::terminal_type{};
FTermDetection::colorEnv      FTermDetection::color_env{};
FTermDetection::secondaryDA   FTermDetection::secondary_da{};
char                          FTermDetection::termtype[256]{};
char                          FTermDetection::ttytypename[256]{};
bool                          FTermDetection::decscusr_support{};
bool                          FTermDetection::terminal_detection{};
bool                          FTermDetection::color256{};
const FString*                FTermDetection::answer_back{nullptr};
const FString*                FTermDetection::sec_da{nullptr};
int                           FTermDetection::gnome_terminal_id{};

#if DEBUG
  char FTermDetection::termtype_256color[256]{};
  char FTermDetection::termtype_Answerback[256]{};
  char FTermDetection::termtype_SecDA[256]{};
#endif  // DEBUG


//----------------------------------------------------------------------
// class FTermDetection
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FTermDetection::FTermDetection()
{
  // Preset to true
  terminal_detection = true;

  // Preset to false
  decscusr_support = false;

  // Gnome terminal id from SecDA
  // Example: vte version 0.40.0 = 0 * 100 + 40 * 100 + 0 = 4000
  //                      a.b.c  = a * 100 +  b * 100 + c
  gnome_terminal_id = 0;

  // Set default ttytype file
  std::strncpy (ttytypename, "/etc/ttytype", sizeof(ttytypename));
  ttytypename[sizeof(ttytypename) - 1] = '\0';
}

//----------------------------------------------------------------------
FTermDetection::~FTermDetection()  // destructor
{
  deallocation();
}


// public methods of FTermDetection
//----------------------------------------------------------------------
#if DEBUG
const FString& FTermDetection::getAnswerbackString()
{
  return ( answer_back ) ? *answer_back : fc::emptyFString::get();
}

//----------------------------------------------------------------------
const FString& FTermDetection::getSecDAString()
{
  return ( sec_da ) ? *sec_da : fc::emptyFString::get();
}
#endif

//----------------------------------------------------------------------
void FTermDetection::setTtyTypeFileName (const char ttytype_filename[])
{
  if ( ! ttytype_filename )
    return;

  std::strncpy (ttytypename, ttytype_filename, sizeof(ttytypename));
  ttytypename[sizeof(ttytypename) - 1] = '\0';
}

//----------------------------------------------------------------------
void FTermDetection::detect()
{
  deallocation();

  // Set the variable 'termtype' to the predefined type of the terminal
  getSystemTermType();

  // Analysis the termtype
  termtypeAnalysis();

  // Terminal detection
  detectTerminal();
}


// private methods of FTermDetection
//----------------------------------------------------------------------
void FTermDetection::deallocation()
{
  if ( sec_da )
    delete sec_da;

  if ( answer_back )
    delete answer_back;
}

//----------------------------------------------------------------------
void FTermDetection::getSystemTermType()
{
  // Import the untrusted environment variable TERM
  const auto& term_env = std::getenv("TERM");
  const auto& fterm_data = FTerm::getFTermData();
  const auto& termfilename = fterm_data->getTermFileName();

  if ( term_env )
  {
    // Save name in termtype
    std::strncpy (termtype, term_env, sizeof(termtype));
    termtype[sizeof(termtype) - 1] = '\0';
    return;
  }
  else if ( *termfilename )  // 1st fallback: use the teminal file name
  {
    if ( getTTYtype() )  // Look into /etc/ttytype
      return;

#if F_HAVE_GETTTYNAM
    if ( getTTYSFileEntry() )  // Look into /etc/ttys
      return;
#endif
  }

  // 2nd fallback: use vt100 if not found
  std::strncpy (termtype, "vt100", sizeof(termtype));
  termtype[sizeof(termtype) - 1] = '\0';
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
  const auto& fterm_data = FTerm::getFTermData();
  const char* termfilename = fterm_data->getTermFileName();
  const char* term_basename = std::strrchr(termfilename, '/');

  if ( term_basename == nullptr )
    term_basename = termfilename;
  else
    term_basename++;

  std::FILE* fp{};
  std::array<char, BUFSIZ> str{};
  const auto& fsystem = FTerm::getFSystem();

  if ( (fp = fsystem->fopen(ttytypename, "r")) == nullptr )
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
      std::strncpy (termtype, type, sizeof(termtype));
      termtype[sizeof(termtype) - 1] = '\0';
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
  const auto& fterm_data = FTerm::getFTermData();
  const char* termfilename = fterm_data->getTermFileName();
  const char* term_basename = std::strrchr(termfilename, '/');

  if ( term_basename == nullptr )
    term_basename = termfilename;
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
      std::strncpy (termtype, type, sizeof(termtype));
      termtype[sizeof(termtype) - 1] = '\0';
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
  if ( std::strncmp(termtype, "cygwin", 6) == 0 )
    terminal_type.cygwin = true;

  // rxvt terminal emulator (native MS Window System port) on cygwin
  if ( std::strncmp(termtype, "rxvt-cygwin-native", 18) == 0 )
    terminal_type.rxvt = true;

  // Ansi terminal
  if ( std::strncmp(termtype, "ansi", 4) == 0 )
  {
    terminal_detection = false;
    terminal_type.ansi = true;
  }

  // Sun Microsystems workstation console
  if ( std::strncmp(termtype, "sun", 3) == 0 )
  {
    terminal_detection = false;
    terminal_type.sun_con = true;
  }

  // Kterm
  if ( std::strncmp(termtype, "kterm", 5) == 0 )
  {
    terminal_detection = false;
    terminal_type.kterm = true;
  }

  // mlterm
  if ( std::strncmp(termtype, "mlterm", 6) == 0 )
    terminal_type.mlterm = true;

  // rxvt
  if ( std::strncmp(termtype, "rxvt", 4) == 0 )
    terminal_type.rxvt = true;

  // urxvt
  if ( std::strncmp(termtype, "rxvt-unicode", 12) == 0 )
    terminal_type.urxvt = true;

  // screen/tmux
  if ( std::strncmp(termtype, "screen", 6) == 0 )
  {
    terminal_type.screen = true;
    const char* tmux = std::getenv("TMUX");

    if ( tmux && std::strlen(tmux) != 0 )
      terminal_type.tmux = true;
  }

  // Linux console
  if ( std::strncmp(termtype, "linux", 5) == 0
    || std::strncmp(termtype, "con", 3) == 0 )
    terminal_type.linux_con = true;

  // NetBSD workstation console
  if ( std::strncmp(termtype, "wsvt25", 6) == 0 )
    terminal_type.netbsd_con = true;
}

//----------------------------------------------------------------------
void FTermDetection::detectTerminal()
{
  // Terminal detection

  const char* new_termtype{nullptr};

  if ( terminal_detection )
  {
    FTermios::setCaptureSendCharacters();
    const auto& keyboard = FTerm::getFKeyboard();
    keyboard->setNonBlockingInput();

    // Initialize 256 colors terminals
    new_termtype = init_256colorTerminal();

    // Identify the terminal via the answerback-message
    new_termtype = parseAnswerbackMsg (new_termtype);

    // Identify the terminal via the secondary device attributes (SEC_DA)
    new_termtype = parseSecDA (new_termtype);

    // Determines the maximum number of colors
    new_termtype = determineMaxColor(new_termtype);

    keyboard->unsetNonBlockingInput();
    FTermios::unsetCaptureSendCharacters();
  }

  //
  // Additional termtype analysis
  //

  // Test if the terminal is a xterm
  if ( std::strncmp(termtype, "xterm", 5) == 0
    || std::strncmp(termtype, "Eterm", 5) == 0 )
  {
    terminal_type.xterm = true;

    // Each xterm should be able to use at least 16 colors
    if ( ! new_termtype && std::strlen(termtype) == 5 )
      new_termtype = "xterm-16color";
  }
  else if ( std::strncmp(termtype, "ansi", 4) == 0 )  // ANSI detection
    terminal_type.ansi = true;

  // set the new environment variable TERM
  if ( new_termtype )
  {
    setenv("TERM", new_termtype, 1);
    std::strncpy (termtype, new_termtype, sizeof(termtype));
    termtype[sizeof(termtype) - 1] = '\0';
  }

#if defined(__CYGWIN__)
  const auto& fterm_data = FTerm::getFTermData();
  const auto& termfilename = fterm_data->getTermFileName();

  // Fixes problem with mouse input
  if ( std::strncmp(termfilename, "/dev/cons", 9) == 0 )
    FKeyboard::setNonBlockingInputSupport(false);
#endif
}

//----------------------------------------------------------------------
const char* FTermDetection::init_256colorTerminal()
{
  const char* new_termtype{nullptr};

  if ( get256colorEnvString() )
    color256 = true;
  else if ( std::strstr (termtype, "256color") )
    color256 = true;
  else
    color256 = false;

  new_termtype = termtype_256color_quirks();

#if DEBUG
  if ( new_termtype )
  {
    std::strncpy ( termtype_256color
                 , new_termtype
                 , sizeof(termtype_256color) );
    termtype_256color[sizeof(termtype_256color) - 1] = '\0';
  }
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

  if ( color_env.string1 != nullptr )
    return true;

  if ( color_env.string2 != nullptr )
    return true;

  if ( color_env.string3 != nullptr )
    return true;

  if ( color_env.string4 != nullptr )
    return true;

  if ( color_env.string5 != nullptr )
    return true;

  if ( color_env.string6 != nullptr )
    return true;

  if ( color_env.string7 != nullptr )
    return true;

  return false;
}

//----------------------------------------------------------------------
const char* FTermDetection::termtype_256color_quirks()
{
  const char* new_termtype{nullptr};

  if ( color_env.string2
    || (color_env.string1
      && std::strncmp(color_env.string1, "gnome-terminal", 14) == 0) )
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

  if ( std::strncmp(termtype, "xterm", 5) == 0 )
    new_termtype = "xterm-256color";

  if ( std::strncmp(termtype, "screen", 6) == 0 )
    new_termtype = "screen-256color";

  if ( std::strncmp(termtype, "Eterm", 5) == 0 )
    new_termtype = "Eterm-256color";

  if ( std::strncmp(termtype, "mlterm", 6) == 0 )
    new_termtype = "mlterm-256color";

  if ( std::strncmp(termtype, "rxvt", 4) != 0
    && color_env.string1
    && std::strncmp(color_env.string1, "rxvt-xpm", 8) == 0 )
  {
    new_termtype = "rxvt-256color";
    terminal_type.rxvt = true;
  }

  if ( (color_env.string5 && std::strlen(color_env.string5) > 0)
    || (color_env.string6 && std::strlen(color_env.string6) > 0) )
  {
    terminal_type.kde_konsole = true;
    new_termtype = "konsole-256color";
  }

  if ( color_env.string3 && std::strlen(color_env.string3) > 0 )
    decscusr_support = true;

  return new_termtype;
}

//----------------------------------------------------------------------
const char* FTermDetection::determineMaxColor (const char current_termtype[])
{
  // Determine xterm maximum number of colors via OSC 4

  const char* new_termtype = current_termtype;
  const auto& keyboard = FTerm::getFKeyboard();
  keyboard->setNonBlockingInput();

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

  keyboard->unsetNonBlockingInput();
  return new_termtype;
}

//----------------------------------------------------------------------
FString FTermDetection::getXTermColorName (FColor color)
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
    std::size_t n = std::strlen(buf.data());

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
const char* FTermDetection::parseAnswerbackMsg (const char current_termtype[])
{
  const char* new_termtype = current_termtype;
  const auto& keyboard = FTerm::getFKeyboard();
  keyboard->setNonBlockingInput();
  // send ENQ and read the answerback message
  const auto& ans = getAnswerbackMsg();
  keyboard->unsetNonBlockingInput();

  try
  {
    answer_back = new FString(ans);
  }
  catch (const std::bad_alloc&)
  {
    badAllocOutput ("FString");
    return nullptr;
  }

  if ( *answer_back == "PuTTY" )
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
  if ( new_termtype )
  {
    std::strncpy ( termtype_Answerback
                 , new_termtype
                 , sizeof(termtype_Answerback) - 1);
    termtype_Answerback[sizeof(termtype_Answerback) - 1] = '\0';
  }
#endif  // DEBUG

  return new_termtype;
}

//----------------------------------------------------------------------
FString FTermDetection::getAnswerbackMsg()
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
const char* FTermDetection::parseSecDA (const char current_termtype[])
{
  // The Linux console and older cygwin terminals knows no Sec_DA
  if ( isLinuxTerm() || isCygwinTerminal() )
    return current_termtype;

   // Secondary device attributes (SEC_DA) <- decTerminalID string
  const auto& ans = getSecDA();

  try
  {
    // Secondary device attributes (SEC_DA) <- decTerminalID string
    sec_da = new FString(ans);
  }
  catch (const std::bad_alloc&)
  {
    badAllocOutput ("FString");
    return current_termtype;
  }

  if ( sec_da->getLength() < 6 )
    return current_termtype;

  // remove the first 3 bytes ("\033[>")
  FString temp{sec_da->right(sec_da->getLength() - 3)};
  // remove the last byte ("c")
  temp.remove(temp.getLength() - 1, 1);
  // split into components
  const FStringList sec_da_list = temp.split(';');

  const uLong num_components = sec_da_list.size();

  // The second device attribute (SEC_DA) always has 3 parameters,
  // otherwise it usually has a copy of the device attribute (primary DA)
  if ( num_components < 3 )
    return current_termtype;

  const FString* sec_da_components = &sec_da_list[0];

  if ( sec_da_components[0].isEmpty() )
    return current_termtype;

  // Read the terminal type
  secondary_da.terminal_id_type = str2int(sec_da_components[0]);

  // Read the terminal (firmware) version
  secondary_da.terminal_id_version = str2int(sec_da_components[1]);

  // Read the terminal hardware option
  secondary_da.terminal_id_hardware = str2int(sec_da_components[2]);

  const char* new_termtype = secDA_Analysis(current_termtype);

#if DEBUG
  if ( new_termtype )
  {
    std::strncpy (termtype_SecDA, new_termtype, sizeof(termtype_SecDA));
    termtype_SecDA[sizeof(termtype_SecDA) - 1] = '\0';
  }
#endif  // DEBUG

  return new_termtype;
}

//----------------------------------------------------------------------
int FTermDetection::str2int (const FString& s)
{
  // This is not a general string to integer conversion method.
  // It is only used in this class to convert the device attribute
  // parameters into numbers.

  constexpr int ERROR = -1;

  if ( ! s )
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
FString FTermDetection::getSecDA()
{
  FString sec_da_str{""};

  int a{0};
  int b{0};
  int c{0};
  const int stdin_no{FTermios::getStdIn()};
  const int stdout_no{FTermios::getStdOut()};
  fd_set ifds{};
  struct timeval tv{};
  constexpr auto& SECDA{ESC "[>c"};

  // Get the secondary device attributes
  if ( write(stdout_no, SECDA, std::strlen(SECDA)) == -1 )
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
const char* FTermDetection::secDA_Analysis (const char current_termtype[])
{
  const char* new_termtype = current_termtype;

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
      new_termtype = secDA_Analysis_32(current_termtype);
      break;

    case 41:  // DEC VT420
    case 61:  // DEC VT510
    case 64:  // DEC VT520
      break;

    case 65:  // DEC VT525
      new_termtype = secDA_Analysis_65(current_termtype);
      break;

    case 67:  // Cygwin
      new_termtype = secDA_Analysis_67(current_termtype);
      break;

    case 77:  // mintty
      new_termtype = secDA_Analysis_77(current_termtype);
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
inline const char* FTermDetection::secDA_Analysis_0 (const char current_termtype[])
{
  // Terminal ID 0 - DEC VT100

  const char* new_termtype = current_termtype;

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
inline const char* FTermDetection::secDA_Analysis_1 (const char current_termtype[])
{
  // Terminal ID 1 - DEC VT220

  const char* new_termtype = current_termtype;
  new_termtype = secDA_Analysis_vte(new_termtype);
  return new_termtype;
}

//----------------------------------------------------------------------
inline const char* FTermDetection::secDA_Analysis_24 (const char current_termtype[])
{
  // Terminal ID 24 - DEC VT320

  const char* new_termtype = current_termtype;

#if defined(__NetBSD__) || defined(__OpenBSD__) || defined(UNIT_TEST)

  if ( secondary_da.terminal_id_version == 20
     && FTermOpenBSD::isBSDConsole() )
  {
    // NetBSD/OpenBSD workstation console
    if ( std::strncmp(termtype, "wsvt25", 6) == 0 )
      terminal_type.netbsd_con = true;
    else if ( std::strncmp(termtype, "vt220", 5) == 0 )
    {
      terminal_type.openbsd_con = true;
      new_termtype = "pccon";
    }
  }

#endif  // defined(__NetBSD__) || defined(__OpenBSD__) || defined(UNIT_TEST)

  return new_termtype;
}

//----------------------------------------------------------------------
inline const char* FTermDetection::secDA_Analysis_32 (const char[])
{
  // Terminal ID 32 - Tera Term

  terminal_type.tera_term = true;
  const char* new_termtype = "teraterm";
  return new_termtype;
}

//----------------------------------------------------------------------
inline const char* FTermDetection::secDA_Analysis_65 (const char current_termtype[])
{
  // Terminal ID 65 - DEC VT525 and VTE >= 0.53.0

  return secDA_Analysis_1 (current_termtype);
}

//----------------------------------------------------------------------
inline const char* FTermDetection::secDA_Analysis_67 (const char[])
{
  // Terminal ID 67 - cygwin

  terminal_type.cygwin = true;
  const char* new_termtype = "cygwin";
  std::fflush(stdout);
  return new_termtype;
}

//----------------------------------------------------------------------
inline const char* FTermDetection::secDA_Analysis_77 (const char[])
{
  // Terminal ID 77 - mintty

  terminal_type.mintty = true;
  decscusr_support = true;
  const char* new_termtype = "xterm-256color";
  std::fflush(stdout);
  return new_termtype;
}

//----------------------------------------------------------------------
inline const char* FTermDetection::secDA_Analysis_82()
{
  // Terminal ID 82 - rxvt

  const char* new_termtype{};
  terminal_type.rxvt = true;

  if ( std::strncmp(termtype, "rxvt-cygwin-native", 18) == 0 )
    new_termtype = "rxvt-16color";
  else
    new_termtype = "rxvt";

  return new_termtype;
}

//----------------------------------------------------------------------
inline const char* FTermDetection::secDA_Analysis_83 (const char current_termtype[])
{
  // Terminal ID 83 - screen

  const char* new_termtype = current_termtype;
  terminal_type.screen = true;
  return new_termtype;
}

//----------------------------------------------------------------------
inline const char* FTermDetection::secDA_Analysis_84 (const char current_termtype[])
{
  // Terminal ID 84 - tmux

  const char* new_termtype = current_termtype;
  terminal_type.screen = true;
  terminal_type.tmux = true;
  return new_termtype;
}

//----------------------------------------------------------------------
inline const char* FTermDetection::secDA_Analysis_85()
{
  // Terminal ID 85 - rxvt-unicode

  const char* new_termtype{};
  terminal_type.rxvt = true;
  terminal_type.urxvt = true;

  if ( std::strncmp(termtype, "rxvt-", 5) != 0 )
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
inline const char* FTermDetection::secDA_Analysis_vte (const char current_termtype[])
{
  // VTE terminal library
  // (Since VTE ) the terminal ID has changed from 1 to 65)

  const char* new_termtype = current_termtype;

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

}  // namespace finalcut
