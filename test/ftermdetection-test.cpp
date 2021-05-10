/***********************************************************************
* ftermdetection-test.cpp - FTermDetection unit tests                  *
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

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <sys/wait.h>
#include <sys/mman.h>

#include <conemu.h>
#include <final/final.h>

#define CPPUNIT_ASSERT_CSTRING(expected, actual) \
            check_c_string (expected, actual, CPPUNIT_SOURCELINE())

//----------------------------------------------------------------------
void check_c_string ( const char* s1
                    , const char* s2
                    , CppUnit::SourceLine sourceLine )
{
  if ( s1 == 0 && s2 == 0 )  // Strings are equal
    return;

  if ( s1 && s2 && std::strcmp (s1, s2) == 0 )  // Strings are equal
      return;

  ::CppUnit::Asserter::fail ("Strings are not equal", sourceLine);
}


//----------------------------------------------------------------------
// class FTermDetectionTest
//----------------------------------------------------------------------

class FTermDetectionTest : public CPPUNIT_NS::TestFixture, test::ConEmu
{
  public:
    FTermDetectionTest();
    ~FTermDetectionTest();

  protected:
    void classNameTest();
    void ansiTest();
    void xtermTest();
    void rxvtTest();
    void urxvtTest();
    void kdeKonsoleTest();
    void gnomeTerminalTest();
    void newerVteTerminalTest();
    void puttyTest();
    void windowsTerminalTest();
    void teraTermTest();
    void cygwinTest();
    void minttyTest();
    void linuxTest();
    void freebsdTest();
    void netbsdTest();
    void openbsdTest();
    void sunTest();
    void screenTest();
    void tmuxTest();
    void ktermTest();
    void mltermTest();
    void kittyTest();
    void ttytypeTest();

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FTermDetectionTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (ansiTest);
    CPPUNIT_TEST (xtermTest);
    CPPUNIT_TEST (rxvtTest);
    CPPUNIT_TEST (urxvtTest);
    CPPUNIT_TEST (kdeKonsoleTest);
    CPPUNIT_TEST (gnomeTerminalTest);
    CPPUNIT_TEST (newerVteTerminalTest);
    CPPUNIT_TEST (puttyTest);
    CPPUNIT_TEST (windowsTerminalTest);
    CPPUNIT_TEST (teraTermTest);
    CPPUNIT_TEST (cygwinTest);
    CPPUNIT_TEST (minttyTest);
    CPPUNIT_TEST (linuxTest);
    CPPUNIT_TEST (freebsdTest);
    CPPUNIT_TEST (netbsdTest);
    CPPUNIT_TEST (openbsdTest);
    CPPUNIT_TEST (sunTest);
    CPPUNIT_TEST (screenTest);
    CPPUNIT_TEST (tmuxTest);
    CPPUNIT_TEST (ktermTest);
    CPPUNIT_TEST (mltermTest);
    CPPUNIT_TEST (kittyTest);
    CPPUNIT_TEST (ttytypeTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};

//----------------------------------------------------------------------
FTermDetectionTest::FTermDetectionTest()
{ }

//----------------------------------------------------------------------
FTermDetectionTest::~FTermDetectionTest()
{ }

//----------------------------------------------------------------------
void FTermDetectionTest::classNameTest()
{
  finalcut::FTermDetection d;
  const finalcut::FString& classname = d.getClassName();
  CPPUNIT_ASSERT ( classname == "FTermDetection" );
}

//----------------------------------------------------------------------
void FTermDetectionTest::ansiTest()
{
  auto& data = finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  setenv ("TERM", "ansi", 1);
  data.setTermType("ansi");

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    setenv ("TERM", "ansi", 1);
    unsetenv("TERMCAP");
    unsetenv("COLORTERM");
    unsetenv("COLORFGBG");
    unsetenv("VTE_VERSION");
    unsetenv("XTERM_VERSION");
    unsetenv("ROXTERM_ID");
    unsetenv("KONSOLE_DBUS_SESSION");
    unsetenv("KONSOLE_DCOP");
    unsetenv("TMUX");
    unsetenv("KITTY_WINDOW_ID");
    detect.detect();

    CPPUNIT_ASSERT ( ! detect.isXTerminal() );
    CPPUNIT_ASSERT ( detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isWindowsTerminal() );
    CPPUNIT_ASSERT ( ! detect.isTeraTerm() );
    CPPUNIT_ASSERT ( ! detect.isCygwinTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMinttyTerm() );
    CPPUNIT_ASSERT ( ! detect.isLinuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isFreeBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isNetBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isOpenBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isSunTerminal() );
    CPPUNIT_ASSERT ( ! detect.isScreenTerm() );
    CPPUNIT_ASSERT ( ! detect.isTmuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKittyTerminal() );
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( ! detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), "ansi" );

    // Test fallback to vt100 without TERM environment variable
    unsetenv("TERM");
    detect.setAnsiTerminal(false);
    detect.detect();
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), "vt100" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::ansi);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::xtermTest()
{
  auto& data = finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType("xterm");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    setenv ("TERM", "xterm", 1);
    setenv ("XTERM_VERSION", "XTerm(312)", 1);
    unsetenv("TERMCAP");
    unsetenv("COLORTERM");
    unsetenv("COLORFGBG");
    unsetenv("VTE_VERSION");
    unsetenv("ROXTERM_ID");
    unsetenv("KONSOLE_DBUS_SESSION");
    unsetenv("KONSOLE_DCOP");
    unsetenv("TMUX");
    unsetenv("KITTY_WINDOW_ID");
    detect.detect();

    CPPUNIT_ASSERT ( detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isWindowsTerminal() );
    CPPUNIT_ASSERT ( ! detect.isTeraTerm() );
    CPPUNIT_ASSERT ( ! detect.isCygwinTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMinttyTerm() );
    CPPUNIT_ASSERT ( ! detect.isLinuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isFreeBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isNetBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isOpenBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isSunTerminal() );
    CPPUNIT_ASSERT ( ! detect.isScreenTerm() );
    CPPUNIT_ASSERT ( ! detect.isTmuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKittyTerminal() );
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( detect.hasSetCursorStyleSupport() );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::xterm);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::rxvtTest()
{
  auto& data = finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType("rxvt-cygwin-native");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    setenv ("TERM", "rxvt-cygwin-native", 1);
    setenv ("COLORTERM", "rxvt-xpm", 1);
    setenv ("COLORFGBG", "default;default", 1);
    unsetenv("TERMCAP");
    unsetenv("VTE_VERSION");
    unsetenv("XTERM_VERSION");
    unsetenv("ROXTERM_ID");
    unsetenv("KONSOLE_DBUS_SESSION");
    unsetenv("KONSOLE_DCOP");
    unsetenv("TMUX");
    unsetenv("KITTY_WINDOW_ID");
    detect.detect();

    CPPUNIT_ASSERT ( ! detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isWindowsTerminal() );
    CPPUNIT_ASSERT ( ! detect.isTeraTerm() );
    CPPUNIT_ASSERT ( ! detect.isCygwinTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMinttyTerm() );
    CPPUNIT_ASSERT ( ! detect.isLinuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isFreeBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isNetBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isOpenBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isSunTerminal() );
    CPPUNIT_ASSERT ( ! detect.isScreenTerm() );
    CPPUNIT_ASSERT ( ! detect.isTmuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKittyTerminal() );
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), "rxvt-16color" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::rxvt);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::urxvtTest()
{
  auto& data = finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType("rxvt-unicode-256color");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    setenv ("TERM", "rxvt-unicode-256color", 1);
    setenv ("COLORTERM", "rxvt-xpm", 1);
    setenv ("COLORFGBG", "default;default;0", 1);
    unsetenv("TERMCAP");
    unsetenv("VTE_VERSION");
    unsetenv("XTERM_VERSION");
    unsetenv("ROXTERM_ID");
    unsetenv("KONSOLE_DBUS_SESSION");
    unsetenv("KONSOLE_DCOP");
    unsetenv("TMUX");
    unsetenv("KITTY_WINDOW_ID");
    detect.detect();

    CPPUNIT_ASSERT ( ! detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isWindowsTerminal() );
    CPPUNIT_ASSERT ( ! detect.isTeraTerm() );
    CPPUNIT_ASSERT ( ! detect.isCygwinTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMinttyTerm() );
    CPPUNIT_ASSERT ( ! detect.isLinuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isFreeBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isNetBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isOpenBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isSunTerminal() );
    CPPUNIT_ASSERT ( ! detect.isScreenTerm() );
    CPPUNIT_ASSERT ( ! detect.isTmuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKittyTerminal() );
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::urxvt);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::kdeKonsoleTest()
{
  auto& data = finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType("xterm-256color");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    setenv ("TERM", "xterm-256color", 1);
    setenv ("COLORTERM", "truecolor", 1);
    setenv ("KONSOLE_DBUS_SERVICE", "DCOPRef(konsole-11768,konsole)", 1);
    setenv ("KONSOLE_DCOP", ":1.77", 1);
    unsetenv("COLORFGBG");
    unsetenv("TERMCAP");
    unsetenv("VTE_VERSION");
    unsetenv("XTERM_VERSION");
    unsetenv("ROXTERM_ID");
    unsetenv("TMUX");
    unsetenv("KITTY_WINDOW_ID");
    detect.detect();

    CPPUNIT_ASSERT ( detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isWindowsTerminal() );
    CPPUNIT_ASSERT ( ! detect.isTeraTerm() );
    CPPUNIT_ASSERT ( ! detect.isCygwinTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMinttyTerm() );
    CPPUNIT_ASSERT ( ! detect.isLinuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isFreeBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isNetBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isOpenBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isSunTerminal() );
    CPPUNIT_ASSERT ( ! detect.isScreenTerm() );
    CPPUNIT_ASSERT ( ! detect.isTmuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKittyTerminal() );
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::kde_konsole);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::gnomeTerminalTest()
{
  auto& data = finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType("xterm-256color");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    setenv ("TERM", "xterm-256color", 1);
    setenv ("COLORTERM", "truecolor", 1);
    setenv ("VTE_VERSION", "5202", 1);
    unsetenv("COLORFGBG");
    unsetenv("TERMCAP");
    unsetenv("XTERM_VERSION");
    unsetenv("ROXTERM_ID");
    unsetenv("KONSOLE_DBUS_SESSION");
    unsetenv("KONSOLE_DCOP");
    unsetenv("TMUX");
    unsetenv("KITTY_WINDOW_ID");
    detect.detect();

    CPPUNIT_ASSERT ( detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isWindowsTerminal() );
    CPPUNIT_ASSERT ( ! detect.isTeraTerm() );
    CPPUNIT_ASSERT ( ! detect.isCygwinTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMinttyTerm() );
    CPPUNIT_ASSERT ( ! detect.isLinuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isFreeBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isNetBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isOpenBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isSunTerminal() );
    CPPUNIT_ASSERT ( ! detect.isScreenTerm() );
    CPPUNIT_ASSERT ( ! detect.isTmuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKittyTerminal() );
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( detect.hasSetCursorStyleSupport() );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::gnome_terminal);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::newerVteTerminalTest()
{
  auto& data = finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType("xterm-256color");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    setenv ("TERM", "xterm-256color", 1);
    setenv ("COLORTERM", "truecolor", 1);
    setenv ("VTE_VERSION", "5300", 1);
    unsetenv("COLORFGBG");
    unsetenv("TERMCAP");
    unsetenv("XTERM_VERSION");
    unsetenv("ROXTERM_ID");
    unsetenv("KONSOLE_DBUS_SESSION");
    unsetenv("KONSOLE_DCOP");
    unsetenv("TMUX");
    unsetenv("KITTY_WINDOW_ID");
    detect.detect();

    CPPUNIT_ASSERT ( detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isWindowsTerminal() );
    CPPUNIT_ASSERT ( ! detect.isTeraTerm() );
    CPPUNIT_ASSERT ( ! detect.isCygwinTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMinttyTerm() );
    CPPUNIT_ASSERT ( ! detect.isLinuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isFreeBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isNetBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isOpenBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isSunTerminal() );
    CPPUNIT_ASSERT ( ! detect.isScreenTerm() );
    CPPUNIT_ASSERT ( ! detect.isTmuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKittyTerminal() );
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( detect.hasSetCursorStyleSupport() );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::newer_vte_terminal);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::puttyTest()
{
  auto& data = finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType("xterm");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    setenv ("TERM", "xterm", 1);
    unsetenv("TERMCAP");
    unsetenv("COLORTERM");
    unsetenv("COLORFGBG");
    unsetenv("VTE_VERSION");
    unsetenv("XTERM_VERSION");
    unsetenv("ROXTERM_ID");
    unsetenv("KONSOLE_DBUS_SESSION");
    unsetenv("KONSOLE_DCOP");
    unsetenv("TMUX");
    unsetenv("KITTY_WINDOW_ID");
    detect.detect();

    CPPUNIT_ASSERT ( detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isWindowsTerminal() );
    CPPUNIT_ASSERT ( ! detect.isTeraTerm() );
    CPPUNIT_ASSERT ( ! detect.isCygwinTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMinttyTerm() );
    CPPUNIT_ASSERT ( ! detect.isLinuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isFreeBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isNetBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isOpenBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isSunTerminal() );
    CPPUNIT_ASSERT ( ! detect.isScreenTerm() );
    CPPUNIT_ASSERT ( ! detect.isTmuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKittyTerminal() );
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );

    enableConEmuDebug(true);
    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::putty);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::windowsTerminalTest()
{
  auto& data = finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType("xterm");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    setenv ("TERM", "xterm-256color", 1);
    unsetenv("TERMCAP");
    unsetenv("COLORTERM");
    unsetenv("COLORFGBG");
    unsetenv("VTE_VERSION");
    unsetenv("XTERM_VERSION");
    unsetenv("ROXTERM_ID");
    unsetenv("KONSOLE_DBUS_SESSION");
    unsetenv("KONSOLE_DCOP");
    unsetenv("TMUX");
    unsetenv("KITTY_WINDOW_ID");
    setenv ("WT_PROFILE_ID", "{61c54cbd-c2a6-5271-96e7-009a87ff44bf}", 1);
    setenv ("WT_SESSION", "4dc413a1-5ed9-46d4-b4e0-5a2fec7acb44", 1);
    detect.detect();

    CPPUNIT_ASSERT ( detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( detect.isWindowsTerminal() );
    CPPUNIT_ASSERT ( ! detect.isTeraTerm() );
    CPPUNIT_ASSERT ( ! detect.isCygwinTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMinttyTerm() );
    CPPUNIT_ASSERT ( ! detect.isLinuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isFreeBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isNetBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isOpenBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isSunTerminal() );
    CPPUNIT_ASSERT ( ! detect.isScreenTerm() );
    CPPUNIT_ASSERT ( ! detect.isTmuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKittyTerminal() );
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::win_terminal);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::teraTermTest()
{
  auto& data = finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType("xterm");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    setenv ("TERM", "xterm", 1);
    unsetenv("TERMCAP");
    unsetenv("COLORTERM");
    unsetenv("COLORFGBG");
    unsetenv("VTE_VERSION");
    unsetenv("XTERM_VERSION");
    unsetenv("ROXTERM_ID");
    unsetenv("KONSOLE_DBUS_SESSION");
    unsetenv("KONSOLE_DCOP");
    unsetenv("TMUX");
    unsetenv("KITTY_WINDOW_ID");
    detect.detect();

    CPPUNIT_ASSERT ( detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isWindowsTerminal() );
    CPPUNIT_ASSERT ( detect.isTeraTerm() );
    CPPUNIT_ASSERT ( ! detect.isCygwinTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMinttyTerm() );
    CPPUNIT_ASSERT ( ! detect.isLinuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isFreeBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isNetBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isOpenBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isSunTerminal() );
    CPPUNIT_ASSERT ( ! detect.isScreenTerm() );
    CPPUNIT_ASSERT ( ! detect.isTmuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKittyTerminal() );
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::tera_term);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::cygwinTest()
{
  auto& data = finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType("cygwin");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    setenv ("TERM", "cygwin", 1);
    unsetenv("TERMCAP");
    unsetenv("COLORTERM");
    unsetenv("COLORFGBG");
    unsetenv("VTE_VERSION");
    unsetenv("XTERM_VERSION");
    unsetenv("ROXTERM_ID");
    unsetenv("KONSOLE_DBUS_SESSION");
    unsetenv("KONSOLE_DCOP");
    unsetenv("TMUX");
    unsetenv("KITTY_WINDOW_ID");
    detect.detect();

    CPPUNIT_ASSERT ( ! detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isWindowsTerminal() );
    CPPUNIT_ASSERT ( ! detect.isTeraTerm() );
    CPPUNIT_ASSERT ( detect.isCygwinTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMinttyTerm() );
    CPPUNIT_ASSERT ( ! detect.isLinuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isFreeBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isNetBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isOpenBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isSunTerminal() );
    CPPUNIT_ASSERT ( ! detect.isScreenTerm() );
    CPPUNIT_ASSERT ( ! detect.isTmuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKittyTerminal() );
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::cygwin);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::minttyTest()
{
  auto& data = finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType("xterm-256color");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    setenv ("TERM", "xterm-256color", 1);
    unsetenv("TERMCAP");
    unsetenv("COLORTERM");
    unsetenv("COLORFGBG");
    unsetenv("VTE_VERSION");
    unsetenv("XTERM_VERSION");
    unsetenv("ROXTERM_ID");
    unsetenv("KONSOLE_DBUS_SESSION");
    unsetenv("KONSOLE_DCOP");
    unsetenv("TMUX");
    unsetenv("KITTY_WINDOW_ID");
    detect.detect();

    CPPUNIT_ASSERT ( detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isWindowsTerminal() );
    CPPUNIT_ASSERT ( ! detect.isTeraTerm() );
    CPPUNIT_ASSERT ( ! detect.isCygwinTerminal() );
    CPPUNIT_ASSERT ( detect.isMinttyTerm() );
    CPPUNIT_ASSERT ( ! detect.isLinuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isFreeBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isNetBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isOpenBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isSunTerminal() );
    CPPUNIT_ASSERT ( ! detect.isScreenTerm() );
    CPPUNIT_ASSERT ( ! detect.isTmuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKittyTerminal() );
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( detect.hasSetCursorStyleSupport() );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::mintty);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::linuxTest()
{
  auto& data = finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType("linux");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    setenv ("TERM", "linux", 1);
    unsetenv("TERMCAP");
    unsetenv("COLORTERM");
    unsetenv("COLORFGBG");
    unsetenv("VTE_VERSION");
    unsetenv("XTERM_VERSION");
    unsetenv("ROXTERM_ID");
    unsetenv("KONSOLE_DBUS_SESSION");
    unsetenv("KONSOLE_DCOP");
    unsetenv("TMUX");
    unsetenv("KITTY_WINDOW_ID");
    detect.detect();

    CPPUNIT_ASSERT ( ! detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isWindowsTerminal() );
    CPPUNIT_ASSERT ( ! detect.isTeraTerm() );
    CPPUNIT_ASSERT ( ! detect.isCygwinTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMinttyTerm() );
    CPPUNIT_ASSERT ( detect.isLinuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isFreeBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isNetBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isOpenBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isSunTerminal() );
    CPPUNIT_ASSERT ( ! detect.isScreenTerm() );
    CPPUNIT_ASSERT ( ! detect.isTmuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKittyTerminal() );
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );

    // Test fallback to vt100 without TERM environment variable
    unsetenv("TERM");
    detect.setLinuxTerm(false);
    detect.detect();
    CPPUNIT_ASSERT ( ! detect.isLinuxTerm() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), "vt100" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::linux_con);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::freebsdTest()
{
  auto& data = finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType("xterm");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    setenv ("TERM", "xterm", 1);
    unsetenv("TERMCAP");
    unsetenv("COLORTERM");
    unsetenv("COLORFGBG");
    unsetenv("VTE_VERSION");
    unsetenv("XTERM_VERSION");
    unsetenv("ROXTERM_ID");
    unsetenv("KONSOLE_DBUS_SESSION");
    unsetenv("KONSOLE_DCOP");
    unsetenv("TMUX");
    unsetenv("KITTY_WINDOW_ID");
    detect.detect();
    detect.setFreeBSDTerm (true);  // Fake FreeBSD Console detection

    CPPUNIT_ASSERT ( detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isWindowsTerminal() );
    CPPUNIT_ASSERT ( ! detect.isTeraTerm() );
    CPPUNIT_ASSERT ( ! detect.isCygwinTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMinttyTerm() );
    CPPUNIT_ASSERT ( ! detect.isLinuxTerm() );
    CPPUNIT_ASSERT ( detect.isFreeBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isNetBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isOpenBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isSunTerminal() );
    CPPUNIT_ASSERT ( ! detect.isScreenTerm() );
    CPPUNIT_ASSERT ( ! detect.isTmuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKittyTerminal() );
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );

    // Test fallback to vt100 without TERM environment variable
    unsetenv("TERM");
    detect.setXTerminal (false);
    detect.setFreeBSDTerm(false);
    detect.detect();
    CPPUNIT_ASSERT ( ! detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isFreeBSDTerm() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), "vt100" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::freebsd_con);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::netbsdTest()
{
  auto& data = finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType("wsvt25");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    setenv ("TERM", "wsvt25", 1);
    unsetenv("TERMCAP");
    unsetenv("COLORTERM");
    unsetenv("COLORFGBG");
    unsetenv("VTE_VERSION");
    unsetenv("XTERM_VERSION");
    unsetenv("ROXTERM_ID");
    unsetenv("KONSOLE_DBUS_SESSION");
    unsetenv("KONSOLE_DCOP");
    unsetenv("TMUX");
    unsetenv("KITTY_WINDOW_ID");
    detect.detect();
    detect.setNetBSDTerm (true);  // Fake NetBSD Console detection

    CPPUNIT_ASSERT ( ! detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isWindowsTerminal() );
    CPPUNIT_ASSERT ( ! detect.isTeraTerm() );
    CPPUNIT_ASSERT ( ! detect.isCygwinTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMinttyTerm() );
    CPPUNIT_ASSERT ( ! detect.isLinuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isFreeBSDTerm() );
    CPPUNIT_ASSERT ( detect.isNetBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isOpenBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isSunTerminal() );
    CPPUNIT_ASSERT ( ! detect.isScreenTerm() );
    CPPUNIT_ASSERT ( ! detect.isTmuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKittyTerminal() );
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );

    // Test fallback to vt100 without TERM environment variable
    unsetenv("TERM");
    detect.setNetBSDTerm(false);
    detect.detect();
    CPPUNIT_ASSERT ( ! detect.isFreeBSDTerm() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), "vt100" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::netbsd_con);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::openbsdTest()
{
  auto& data = finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType("vt220");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    setenv ("TERM", "vt220", 1);
    unsetenv("TERMCAP");
    unsetenv("COLORTERM");
    unsetenv("COLORFGBG");
    unsetenv("VTE_VERSION");
    unsetenv("XTERM_VERSION");
    unsetenv("ROXTERM_ID");
    unsetenv("KONSOLE_DBUS_SESSION");
    unsetenv("KONSOLE_DCOP");
    unsetenv("TMUX");
    unsetenv("KITTY_WINDOW_ID");
    detect.detect();
    detect.setOpenBSDTerm (true);  // Fake OpenBSD Console detection

    CPPUNIT_ASSERT ( ! detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isWindowsTerminal() );
    CPPUNIT_ASSERT ( ! detect.isTeraTerm() );
    CPPUNIT_ASSERT ( ! detect.isCygwinTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMinttyTerm() );
    CPPUNIT_ASSERT ( ! detect.isLinuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isFreeBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isNetBSDTerm() );
    CPPUNIT_ASSERT ( detect.isOpenBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isSunTerminal() );
    CPPUNIT_ASSERT ( ! detect.isScreenTerm() );
    CPPUNIT_ASSERT ( ! detect.isTmuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKittyTerminal() );
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );

    // Test fallback to vt100 without TERM environment variable
    unsetenv("TERM");
    detect.setOpenBSDTerm(false);
    detect.detect();
    CPPUNIT_ASSERT ( ! detect.isOpenBSDTerm() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), "vt100" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::openbsd_con);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::sunTest()
{
  auto& data = finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType("sun-color");

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    setenv ("TERM", "sun-color", 1);
    unsetenv("TERMCAP");
    unsetenv("COLORTERM");
    unsetenv("COLORFGBG");
    unsetenv("VTE_VERSION");
    unsetenv("XTERM_VERSION");
    unsetenv("ROXTERM_ID");
    unsetenv("KONSOLE_DBUS_SESSION");
    unsetenv("KONSOLE_DCOP");
    unsetenv("TMUX");
    unsetenv("KITTY_WINDOW_ID");
    detect.detect();

    CPPUNIT_ASSERT ( ! detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isWindowsTerminal() );
    CPPUNIT_ASSERT ( ! detect.isTeraTerm() );
    CPPUNIT_ASSERT ( ! detect.isCygwinTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMinttyTerm() );
    CPPUNIT_ASSERT ( ! detect.isLinuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isFreeBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isNetBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isOpenBSDTerm() );
    CPPUNIT_ASSERT ( detect.isSunTerminal() );
    CPPUNIT_ASSERT ( ! detect.isScreenTerm() );
    CPPUNIT_ASSERT ( ! detect.isTmuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKittyTerminal() );
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( ! detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );

    // Test fallback to vt100 without TERM environment variable
    unsetenv("TERM");
    detect.setSunTerminal(false);
    detect.detect();
    CPPUNIT_ASSERT ( ! detect.isSunTerminal() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), "vt100" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::sun_con);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::screenTest()
{
  auto& data = finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType("screen");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    setenv ("TERM", "screen", 1);
    setenv ("TERMCAP", "SC|screen|VT 100/ANSI X3.64 virtual terminal:...", 1);
    unsetenv("COLORTERM");
    unsetenv("COLORFGBG");
    unsetenv("VTE_VERSION");
    unsetenv("XTERM_VERSION");
    unsetenv("ROXTERM_ID");
    unsetenv("KONSOLE_DBUS_SESSION");
    unsetenv("KONSOLE_DCOP");
    unsetenv("TMUX");
    unsetenv("KITTY_WINDOW_ID");
    detect.detect();

    CPPUNIT_ASSERT ( ! detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isWindowsTerminal() );
    CPPUNIT_ASSERT ( ! detect.isTeraTerm() );
    CPPUNIT_ASSERT ( ! detect.isCygwinTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMinttyTerm() );
    CPPUNIT_ASSERT ( ! detect.isLinuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isFreeBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isNetBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isOpenBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isSunTerminal() );
    CPPUNIT_ASSERT ( detect.isScreenTerm() );
    CPPUNIT_ASSERT ( ! detect.isTmuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKittyTerminal() );
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), "screen" );

    setenv ("XTERM_VERSION", "XTerm(312)", 1);
    detect.detect();
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), "screen-256color" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::screen);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::tmuxTest()
{
  auto& data = finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType("screen");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    setenv ("TERM", "screen", 1);
    setenv ("TMUX", "/tmp/tmux-1000/default,7844,0", 1);
    setenv ("TMUX_PANE", "%0", 1);
    unsetenv("TERMCAP");
    unsetenv("COLORTERM");
    unsetenv("COLORFGBG");
    unsetenv("VTE_VERSION");
    unsetenv("XTERM_VERSION");
    unsetenv("ROXTERM_ID");
    unsetenv("KONSOLE_DBUS_SESSION");
    unsetenv("KONSOLE_DCOP");
    unsetenv("KITTY_WINDOW_ID");
    detect.detect();

    CPPUNIT_ASSERT ( ! detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isWindowsTerminal() );
    CPPUNIT_ASSERT ( ! detect.isTeraTerm() );
    CPPUNIT_ASSERT ( ! detect.isCygwinTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMinttyTerm() );
    CPPUNIT_ASSERT ( ! detect.isLinuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isFreeBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isNetBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isOpenBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isSunTerminal() );
    CPPUNIT_ASSERT ( detect.isScreenTerm() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
    CPPUNIT_ASSERT ( detect.isTmuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKittyTerminal() );
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), "screen" );

    setenv ("VTE_VERSION", "3801", 1);
    detect.detect();
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), "screen-256color" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal simulation
    startConEmuTerminal (ConEmu::console::tmux);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::ktermTest()
{
  auto& data = finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType("kterm");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    setenv ("TERM", "kterm", 1);
    unsetenv("TERMCAP");
    unsetenv("COLORTERM");
    unsetenv("COLORFGBG");
    unsetenv("VTE_VERSION");
    unsetenv("XTERM_VERSION");
    unsetenv("ROXTERM_ID");
    unsetenv("KONSOLE_DBUS_SESSION");
    unsetenv("KONSOLE_DCOP");
    unsetenv("TMUX");
    unsetenv("KITTY_WINDOW_ID");
    detect.detect();

    CPPUNIT_ASSERT ( ! detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isWindowsTerminal() );
    CPPUNIT_ASSERT ( ! detect.isTeraTerm() );
    CPPUNIT_ASSERT ( ! detect.isCygwinTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMinttyTerm() );
    CPPUNIT_ASSERT ( ! detect.isLinuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isFreeBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isNetBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isOpenBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isSunTerminal() );
    CPPUNIT_ASSERT ( ! detect.isScreenTerm() );
    CPPUNIT_ASSERT ( ! detect.isTmuxTerm() );
    CPPUNIT_ASSERT ( detect.isKtermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKittyTerminal() );
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( ! detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );

    // Test fallback to vt100 without TERM environment variable
    unsetenv("TERM");
    detect.setKtermTerminal(false);
    detect.detect();
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), "vt100" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::kterm);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::mltermTest()
{
  auto& data = finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType("mlterm");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    setenv ("TERM", "mlterm", 1);
    setenv ("MLTERM", "3.8.4", 1);
    setenv ("COLORFGBG", "default;default", 1);
    unsetenv("TERMCAP");
    unsetenv("COLORTERM");
    unsetenv("VTE_VERSION");
    unsetenv("XTERM_VERSION");
    unsetenv("ROXTERM_ID");
    unsetenv("KONSOLE_DBUS_SESSION");
    unsetenv("KONSOLE_DCOP");
    unsetenv("TMUX");
    unsetenv("KITTY_WINDOW_ID");
    detect.detect();

    CPPUNIT_ASSERT ( ! detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isWindowsTerminal() );
    CPPUNIT_ASSERT ( ! detect.isTeraTerm() );
    CPPUNIT_ASSERT ( ! detect.isCygwinTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMinttyTerm() );
    CPPUNIT_ASSERT ( ! detect.isLinuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isFreeBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isNetBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isOpenBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isSunTerminal() );
    CPPUNIT_ASSERT ( ! detect.isScreenTerm() );
    CPPUNIT_ASSERT ( ! detect.isTmuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
    CPPUNIT_ASSERT ( detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKittyTerminal() );
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), "mlterm-256color" );

    setenv ("TERM", "mlterm", 1);
    unsetenv("COLORFGBG");
    detect.detect();
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), "xterm-256color" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::mlterm);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::kittyTest()
{
  auto& data = finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType("xterm-kitty");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    setenv ("TERM", "xterm-kitty", 1);
    setenv ("KITTY_WINDOW_ID", "1", 1);
    setenv ("COLORTERM", "truecolor", 1);
    unsetenv("TERMCAP");
    unsetenv("VTE_VERSION");
    unsetenv("XTERM_VERSION");
    unsetenv("ROXTERM_ID");
    unsetenv("KONSOLE_DBUS_SESSION");
    unsetenv("KONSOLE_DCOP");
    unsetenv("TMUX");
    detect.detect();

    CPPUNIT_ASSERT ( ! detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isWindowsTerminal() );
    CPPUNIT_ASSERT ( ! detect.isTeraTerm() );
    CPPUNIT_ASSERT ( ! detect.isCygwinTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMinttyTerm() );
    CPPUNIT_ASSERT ( ! detect.isLinuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isFreeBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isNetBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isOpenBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isSunTerminal() );
    CPPUNIT_ASSERT ( ! detect.isScreenTerm() );
    CPPUNIT_ASSERT ( ! detect.isTmuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( detect.isKittyTerminal() );
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), "xterm-kitty" );

    auto kitty_version = detect.getKittyVersion();
    CPPUNIT_ASSERT (  kitty_version.primary == 0 );
    CPPUNIT_ASSERT (  kitty_version.secondary == 13 );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::kitty);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::ttytypeTest()
{
  // Test without TERM environment variable

  if ( mkdir("new-root-dir", 0755) == -1 )
    if ( errno != EEXIST )
      return;

  if ( mkdir("new-root-dir/etc", 0755) == -1 )
    if ( errno != EEXIST )
      return;

  // Write a own /etc/ttytype file
  std::ofstream ttytype ("new-root-dir/etc/ttytype");

  if ( ! ttytype.is_open() )
  {
    rmdir("new-root-dir/etc");
    rmdir("new-root-dir");
    return;
  }

  ttytype << "linux" << "\t" << "tty1" << std::endl;
  ttytype << "linux" << "\t" << "tty2" << std::endl;
  ttytype << "linux" << "\t" << "tty3" << std::endl;
  ttytype << "linux" << "\t" << "tty4" << std::endl;
  ttytype << "linux" << "\t" << "tty5" << std::endl;
  ttytype << "linux" << "\t" << "tty6" << std::endl;
  ttytype << "vt100" << "\t" << "ttyp0" << std::endl;
  ttytype << "vt100" << "\t" << "ttyp1" << std::endl;
  ttytype << "vt100" << "\t" << "ttyp2" << std::endl;
  ttytype << "vt100" << "\t" << "ttyp3" << std::endl;
  ttytype << "vt100" << "\t" << "ttyp4" << std::endl;
  ttytype << "vt100" << "\t" << "ttyp5" << std::endl;
  ttytype << "vt100" << "\t" << "ttyp6" << std::endl;
  ttytype.close();

  finalcut::FTermDetection detect;
  detect.setTerminalDetection(true);
  detect.setTtyTypeFileName("new-root-dir/etc/ttytype");

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    unsetenv("TERM");
    unsetenv("TERMCAP");
    unsetenv("COLORTERM");
    unsetenv("COLORFGBG");
    unsetenv("VTE_VERSION");
    unsetenv("XTERM_VERSION");
    unsetenv("ROXTERM_ID");
    unsetenv("KONSOLE_DBUS_SESSION");
    unsetenv("KONSOLE_DCOP");
    unsetenv("TMUX");
    unsetenv("KITTY_WINDOW_ID");
    auto& data = finalcut::FTerm::getFTermData();

    // Test /dev/tty3 with linux
    data.setTermFileName("/dev/tty3");
    detect.detect();
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), "linux" );

    // Test /dev/ttyp0 with vt100
    data.setTermFileName("/dev/ttyp0");
    detect.detect();
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), "vt100" );

    // Test non-existent /dev/tty8 with fallback to vt100
    data.setTermFileName("/dev/tty8");
    detect.detect();
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), "vt100" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::ansi);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }

  unlink("new-root-dir/etc/ttytype");
  rmdir("new-root-dir/etc");
  rmdir("new-root-dir");
}


// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FTermDetectionTest);

// The general unit test main part
#include <main-test.inc>
