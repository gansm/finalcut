/***********************************************************************
* ftermdetection.cpp - Detection of the terminal type                  *
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

#include "final/fterm.h"
#include "final/ftermdetection.h"

namespace finalcut
{

// static class attributes
FTermDetection::terminalType FTermDetection::terminal_type = \
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
FTermDetection::colorEnv     FTermDetection::color_env;
FTermDetection::secondaryDA  FTermDetection::secondary_da;
FTermData*     FTermDetection::fterm_data = 0;
char           FTermDetection::termtype[256] = { };
char           FTermDetection::ttytypename[256] = { };
bool           FTermDetection::decscusr_support;
bool           FTermDetection::terminal_detection;
bool           FTermDetection::color256;
const FString* FTermDetection::answer_back = 0;
const FString* FTermDetection::sec_da      = 0;
int            FTermDetection::gnome_terminal_id;

#if DEBUG
  char FTermDetection::termtype_256color[256]   = { };
  char FTermDetection::termtype_Answerback[256] = { };
  char FTermDetection::termtype_SecDA[256]      = { };
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

  // Initialize the structs
  color_env.setDefault();
  secondary_da.setDefault();

  // Set default ttytype file
  std::strncpy (ttytypename, C_STR("/etc/ttytype"), sizeof(ttytypename));
  ttytypename[sizeof(ttytypename) - 1] = '\0';
}

//----------------------------------------------------------------------
FTermDetection::~FTermDetection()  // destructor
{
  if ( sec_da )
    delete sec_da;

  if ( answer_back )
    delete answer_back;
}


// public methods of FTermDetection
//----------------------------------------------------------------------
void FTermDetection::setFTermData (FTermData* data)
{
  fterm_data = data;
}

//----------------------------------------------------------------------
void FTermDetection::setTtyTypeFileName (char ttytype_filename[])
{
  if ( ! ttytype_filename )
    return;

  std::strncpy (ttytypename, ttytype_filename, sizeof(ttytypename));
  ttytypename[sizeof(ttytypename) - 1] = '\0';
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
  const char* const& term_env = std::getenv(C_STR("TERM"));
  const char* termfilename = fterm_data->getTermFileName();

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
  std::strncpy (termtype, C_STR("vt100"), sizeof(termtype));
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
  const char* termfilename = fterm_data->getTermFileName();
  const char* term_basename = std::strrchr(termfilename, '/');

  if ( term_basename == 0 )
    term_basename = termfilename;
  else
    term_basename++;

  std::FILE *fp;

  if ( (fp = std::fopen(ttytypename, "r")) != 0 )
  {
    char* p;
    char  str[BUFSIZ];

    // Read and parse the file
    while ( fgets(str, sizeof(str) - 1, fp) != 0 )
    {
      char* name;
      char* type;
      type = name = 0;  // 0 == not found
      p = str;

      while ( *p )
      {
        if ( std::isspace(uChar(*p)) )
          *p = '\0';
        else if ( type == 0 )
          type = p;
        else if ( name == 0 && p != str && p[-1] == '\0' )
          name = p;

        p++;
      }

      if ( type != 0 && name != 0 && ! std::strcmp(name, term_basename) )
      {
        // Save name in termtype
        std::strncpy (termtype, type, sizeof(termtype));
        termtype[sizeof(termtype) - 1] = '\0';
        std::fclose(fp);
        return true;
      }
    }

    std::fclose(fp);
  }

  return false;
}

#if F_HAVE_GETTTYNAM
//----------------------------------------------------------------------
bool FTermDetection::getTTYSFileEntry()
{
  // Analyse /etc/ttys and get the term name

  // get term basename
  const char* termfilename = fterm_data->getTermFileName();
  const char* term_basename = std::strrchr(termfilename, '/');

  if ( term_basename == 0 )
    term_basename = termfilename;
  else
    term_basename++;

  struct ttyent* ttys_entryt;
  ttys_entryt = getttynam(term_basename);

  if ( ttys_entryt )
  {
    char* type = ttys_entryt->ty_type;

    if ( type != 0 )
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

  // screen/tmux
  if ( std::strncmp(termtype, "screen", 6) == 0 )
  {
    terminal_type.screen = true;
    char* tmux = std::getenv("TMUX");

    if ( tmux && std::strlen(tmux) != 0 )
      terminal_type.tmux = true;
  }

  // Linux console
  if ( std::strncmp(termtype, C_STR("linux"), 5) == 0
    || std::strncmp(termtype, C_STR("con"), 3) == 0 )
    terminal_type.linux_con = true;

  // NetBSD workstation console
  if ( std::strncmp(termtype, C_STR("wsvt25"), 6) == 0 )
    terminal_type.netbsd_con = true;
}

//----------------------------------------------------------------------
void FTermDetection::detectTerminal()
{
  // Terminal detection

  char* new_termtype = 0;

  if ( terminal_detection )
  {
    FTermios::setCaptureSendCharacters();

    // Initialize 256 colors terminals
    new_termtype = init_256colorTerminal();

    // Identify the terminal via the answerback-message
    new_termtype = parseAnswerbackMsg (new_termtype);

    // Identify the terminal via the secondary device attributes (SEC_DA)
    new_termtype = parseSecDA (new_termtype);

    // Determines the maximum number of colors
    new_termtype = determineMaxColor(new_termtype);

    FTermios::unsetCaptureSendCharacters();
  }

  //
  // Additional termtype analysis
  //

  // Test if the terminal is a xterm
  if ( std::strncmp(termtype, C_STR("xterm"), 5) == 0
    || std::strncmp(termtype, C_STR("Eterm"), 5) == 0 )
  {
    terminal_type.xterm = true;

    // Each xterm should be able to use at least 16 colors
    if ( ! new_termtype && std::strlen(termtype) == 5 )
      new_termtype = C_STR("xterm-16color");
  }

  // set the new environment variable TERM
  if ( new_termtype )
  {
    setenv(C_STR("TERM"), new_termtype, 1);
    std::strncpy (termtype, new_termtype, sizeof(termtype));
    termtype[sizeof(termtype) - 1] = '\0';
  }
}

//----------------------------------------------------------------------
char* FTermDetection::init_256colorTerminal()
{
  char* new_termtype = 0;

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

  if ( color_env.string1 != 0 )
    return true;

  if ( color_env.string2 != 0 )
    return true;

  if ( color_env.string3 != 0 )
    return true;

  if ( color_env.string4 != 0 )
    return true;

  if ( color_env.string5 != 0 )
    return true;

  if ( color_env.string6 != 0 )
    return true;

  if ( color_env.string7 != 0 )
    return true;

  return false;
}

//----------------------------------------------------------------------
char* FTermDetection::termtype_256color_quirks()
{
  char* new_termtype = 0;

  if ( ! color256 )
    return new_termtype;

  if ( std::strncmp(termtype, "xterm", 5) == 0 )
    new_termtype = C_STR("xterm-256color");

  if ( std::strncmp(termtype, "screen", 6) == 0 )
    new_termtype = C_STR("screen-256color");

  if ( std::strncmp(termtype, "Eterm", 5) == 0 )
    new_termtype = C_STR("Eterm-256color");

  if ( std::strncmp(termtype, "mlterm", 6) == 0 )
    new_termtype = C_STR("mlterm-256color");

  if ( std::strncmp(termtype, "rxvt", 4) != 0
    && color_env.string1
    && std::strncmp(color_env.string1, "rxvt-xpm", 8) == 0 )
  {
    new_termtype = C_STR("rxvt-256color");
    terminal_type.rxvt = true;
  }

  if ( (color_env.string5 && std::strlen(color_env.string5) > 0)
    || (color_env.string6 && std::strlen(color_env.string6) > 0) )
    terminal_type.kde_konsole = true;

  if ( color_env.string3 && std::strlen(color_env.string3) > 0 )
    decscusr_support = true;

  if ( color_env.string2
    || (color_env.string1
      && std::strncmp(color_env.string1, "gnome-terminal", 14) == 0) )
  {
    terminal_type.gnome_terminal = true;
    // Each gnome-terminal should be able to use 256 colors
    color256 = true;

    if ( ! isScreenTerm() )
      new_termtype = C_STR("gnome-256color");
  }

  return new_termtype;
}

//----------------------------------------------------------------------
char* FTermDetection::determineMaxColor (char current_termtype[])
{
  // Determine xterm maximum number of colors via OSC 4

  char* new_termtype = current_termtype;

  if ( ! color256
    && ! isCygwinTerminal()
    && ! isTeraTerm()
    && ! isLinuxTerm()
    && ! isNetBSDTerm()
    && ! getXTermColorName(0).isEmpty() )
  {
    if ( ! getXTermColorName(255).isEmpty() )
    {
      color256 = true;

      if ( isPuttyTerminal() )
        new_termtype = C_STR("putty-256color");
      else
        new_termtype = C_STR("xterm-256color");
    }
    else if ( ! getXTermColorName(87).isEmpty() )
    {
      new_termtype = C_STR("xterm-88color");
    }
    else if ( ! getXTermColorName(15).isEmpty() )
    {
      new_termtype = C_STR("xterm-16color");
    }
  }

  return new_termtype;
}

//----------------------------------------------------------------------
const FString FTermDetection::getXTermColorName (int color)
{
  FString color_str("");

  fd_set ifds;
  struct timeval tv;
  int stdin_no = FTermios::getStdIn();

  char temp[512] = { };
  std::fprintf (stdout, OSC "4;%3d;?" BEL, color);  // get color
  std::fflush(stdout);

  FD_ZERO(&ifds);
  FD_SET(stdin_no, &ifds);
  tv.tv_sec  = 0;
  tv.tv_usec = 150000;  // 150 ms

  // read the terminal answer
  if ( select (stdin_no + 1, &ifds, 0, 0, &tv) > 0 )
  {
    if ( std::scanf("\033]4;%10d;%509[^\n]s", &color, temp) == 2 )
    {
      std::size_t n = std::strlen(temp);

      // BEL + '\0' = string terminator
      if ( n >= 6 && temp[n - 1] == BEL[0] && temp[n] == '\0' )
        temp[n - 1] = '\0';

      // Esc + \ = OSC string terminator (mintty)
      if ( n >= 6 && temp[n - 2] == ESC[0] && temp[n - 1] == '\\' )
        temp[n - 2] = '\0';

      color_str = temp;
    }
  }

  return color_str;
}

//----------------------------------------------------------------------
char* FTermDetection::parseAnswerbackMsg (char current_termtype[])
{
  char* new_termtype = current_termtype;

  // send ENQ and read the answerback message
  try
  {
    answer_back = new FString(getAnswerbackMsg());
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    return 0;
  }

  if ( *answer_back == "PuTTY" )
  {
    terminal_type.putty = true;

    if ( color256 )
      new_termtype = C_STR("putty-256color");
    else
      new_termtype = C_STR("putty");
  }

  // cygwin needs a backspace to delete the '♣' char
  if ( isCygwinTerminal() )
    FTerm::putstring (BS " " BS);

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
const FString FTermDetection::getAnswerbackMsg()
{
  FString answerback = "";

  fd_set ifds;
  struct timeval tv;
  char temp[10] = { };
  int stdin_no = FTermios::getStdIn();

  std::putchar (ENQ[0]);  // Send enquiry character
  std::fflush(stdout);

  FD_ZERO(&ifds);
  FD_SET(stdin_no, &ifds);
  tv.tv_sec  = 0;
  tv.tv_usec = 150000;  // 150 ms

  // Read the answerback message
  if ( select (stdin_no + 1, &ifds, 0, 0, &tv) > 0 )
    if ( std::fgets (temp, sizeof(temp) - 1, stdin) != 0 )
      answerback = temp;

  return answerback;
}

//----------------------------------------------------------------------
char* FTermDetection::parseSecDA (char current_termtype[])
{
  // The Linux console and older cygwin terminals knows no Sec_DA
  if ( isLinuxTerm() || isCygwinTerminal() )
    return current_termtype;

  try
  {
    // Secondary device attributes (SEC_DA) <- decTerminalID string
    sec_da = new FString(getSecDA());
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    return current_termtype;
  }

  if ( sec_da->getLength() < 6 )
    return current_termtype;

  // remove the first 3 bytes ("\033[>")
  FString temp = sec_da->right(sec_da->getLength() - 3);
  // remove the last byte ("c")
  temp.remove(temp.getLength() - 1, 1);
  // split into components
  FStringList sec_da_list = temp.split(';');

  uLong num_components = sec_da_list.size();

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

  char* new_termtype = secDA_Analysis(current_termtype);

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
  if ( ! s )
    return -1;

  try
  {
    return s.toInt();
  }
  catch (const std::exception&)
  {
    return -1;
  }
}

//----------------------------------------------------------------------
const FString FTermDetection::getSecDA()
{
  FString sec_da_str = "";

  int a = 0
    , b = 0
    , c = 0
    , stdin_no = FTermios::getStdIn()
    , stdout_no = FTermios::getStdOut();
  fd_set ifds;
  struct timeval tv;

  // Get the secondary device attributes
  ssize_t ret = write(stdout_no, SECDA, std::strlen(SECDA));

  if ( ret == -1 )
    return sec_da_str;

  std::fflush(stdout);
  FD_ZERO(&ifds);
  FD_SET(stdin_no, &ifds);
  tv.tv_sec  = 0;
  tv.tv_usec = 600000;  // 600 ms

  // Read the answer
  if ( select (stdin_no + 1, &ifds, 0, 0, &tv) == 1
    && std::scanf("\033[>%10d;%10d;%10dc", &a, &b, &c) == 3 )
      sec_da_str.sprintf("\033[>%d;%d;%dc", a, b, c);

  return sec_da_str;
}

//----------------------------------------------------------------------
char* FTermDetection::secDA_Analysis (char current_termtype[])
{
  char* new_termtype = current_termtype;

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
      new_termtype = secDA_Analysis_82(current_termtype);
      break;

    case 83:  // screen
      new_termtype = secDA_Analysis_83(current_termtype);
      break;

    case 84:  // tmux
      new_termtype = secDA_Analysis_84(current_termtype);
      break;

    case 85:  // rxvt-unicode
      new_termtype = secDA_Analysis_85(current_termtype);
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
inline char* FTermDetection::secDA_Analysis_0 (char current_termtype[])
{
  // Terminal ID 0 - DEC VT100

  char* new_termtype = current_termtype;

  if ( secondary_da.terminal_id_version == 115 )
    terminal_type.kde_konsole = true;
  else if ( secondary_da.terminal_id_version == 136 )
    terminal_type.putty = true;  // PuTTY

#if defined(__FreeBSD__) || defined(__DragonFly__)
  if ( FTermFreeBSD::isFreeBSDConsole() )
    terminal_type.freebsd_con = true;
#endif

  return new_termtype;
}

//----------------------------------------------------------------------
inline char* FTermDetection::secDA_Analysis_1 (char current_termtype[])
{
  // Terminal ID 1 - DEC VT220

  char* new_termtype = current_termtype;
  new_termtype = secDA_Analysis_vte(new_termtype);
  return new_termtype;
}

//----------------------------------------------------------------------
inline char* FTermDetection::secDA_Analysis_24 (char current_termtype[])
{
  // Terminal ID 24 - DEC VT320

  char* new_termtype = current_termtype;

#if defined(__NetBSD__) || defined(__OpenBSD__)

  if ( secondary_da.terminal_id_version == 20
     && FTermOpenBSD::isBSDConsole() )
  {
    // NetBSD/OpenBSD workstation console
    if ( std::strncmp(termtype, C_STR("wsvt25"), 6) == 0 )
      terminal_type.netbsd_con = true;
    else if ( std::strncmp(termtype, C_STR("vt220"), 5) == 0 )
    {
      terminal_type.openbsd_con = true;
      new_termtype = C_STR("pccon");
    }
  }

#endif  // defined(__NetBSD__) || defined(__OpenBSD__)

  return new_termtype;
}

//----------------------------------------------------------------------
inline char* FTermDetection::secDA_Analysis_32 (char[])
{
  // Terminal ID 32 - Tera Term

  char* new_termtype;
  terminal_type.tera_term = true;
  new_termtype = C_STR("teraterm");
  return new_termtype;
}

//----------------------------------------------------------------------
inline char* FTermDetection::secDA_Analysis_65 (char current_termtype[])
{
  // Terminal ID 65 - DEC VT525

  char* new_termtype = current_termtype;
  new_termtype = secDA_Analysis_vte(new_termtype);
  return new_termtype;
}

//----------------------------------------------------------------------
inline char* FTermDetection::secDA_Analysis_67 (char[])
{
  // Terminal ID 67 - cygwin

  char* new_termtype;
  terminal_type.cygwin = true;
  new_termtype = C_STR("cygwin");
  std::fflush(stdout);
  return new_termtype;
}

//----------------------------------------------------------------------
inline char* FTermDetection::secDA_Analysis_77 (char[])
{
  // Terminal ID 77 - mintty

  char* new_termtype;
  terminal_type.mintty = true;
  decscusr_support = true;
  new_termtype = C_STR("xterm-256color");
  std::fflush(stdout);
  return new_termtype;
}

//----------------------------------------------------------------------
inline char* FTermDetection::secDA_Analysis_82 (char current_termtype[])
{
  // Terminal ID 82 - rxvt

  char* new_termtype = current_termtype;
  terminal_type.rxvt = true;

  if ( std::strncmp(termtype, "rxvt-", 5) != 0
    && std::strncmp(termtype, "rxvt-cygwin-native", 18) == 0 )
    new_termtype = C_STR("rxvt-16color");
  else
    new_termtype = termtype;

  return new_termtype;
}

//----------------------------------------------------------------------
inline char* FTermDetection::secDA_Analysis_83 (char current_termtype[])
{
  // Terminal ID 83 - screen

  char* new_termtype = current_termtype;
  terminal_type.screen = true;
  return new_termtype;
}

//----------------------------------------------------------------------
inline char* FTermDetection::secDA_Analysis_84 (char current_termtype[])
{
  // Terminal ID 84 - tmux

  char* new_termtype = current_termtype;
  terminal_type.screen = true;
  terminal_type.tmux = true;
  return new_termtype;
}

//----------------------------------------------------------------------
inline char* FTermDetection::secDA_Analysis_85 (char current_termtype[])
{
  // Terminal ID 85 - rxvt-unicode

  char* new_termtype = current_termtype;
  terminal_type.rxvt = true;
  terminal_type.urxvt = true;

  if ( std::strncmp(termtype, "rxvt-", 5) != 0 )
  {
    if ( color256 )
      new_termtype = C_STR("rxvt-256color");
    else
      new_termtype = C_STR("rxvt");
  }
  else
    new_termtype = termtype;

  return new_termtype;
}

//----------------------------------------------------------------------
inline char* FTermDetection::secDA_Analysis_vte (char current_termtype[])
{
  // VTE terminal library
  // (Since VTE ) the terminal ID has changed from 1 to 65)

  char* new_termtype = current_termtype;

  if ( secondary_da.terminal_id_version > 1000 )
  {
    terminal_type.gnome_terminal = true;
    // Each gnome-terminal should be able to use 256 colors
    color256 = true;
    new_termtype = C_STR("gnome-256color");
    gnome_terminal_id = secondary_da.terminal_id_version;

    // VTE 0.40.0 or higher and gnome-terminal 3.16 or higher
    if ( gnome_terminal_id >= 4000 )
      decscusr_support = true;
  }

  return new_termtype;
}

}  // namespace finalcut
