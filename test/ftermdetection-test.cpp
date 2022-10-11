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


//----------------------------------------------------------------------
// class FTermDetectionTest
//----------------------------------------------------------------------

class FTermDetectionTest : public CPPUNIT_NS::TestFixture, test::ConEmu
{
  public:
    FTermDetectionTest() = default;

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
void FTermDetectionTest::classNameTest()
{
  finalcut::FTermDetection d;
  const finalcut::FString& classname = d.getClassName();
  CPPUNIT_ASSERT ( classname == "FTermDetection" );
}

//----------------------------------------------------------------------
void FTermDetectionTest::ansiTest()
{
  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermDetection detect;
  setenv ("TERM", "ansi", 1);
  data.setTermType("ansi");

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    // (gdb) set follow-fork-mode child
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

    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::xterm) );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::ansi) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::rxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::urxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kde_konsole) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::gnome_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::putty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::win_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tera_term) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::cygwin) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mintty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::linux_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::freebsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::netbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::sun_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::screen) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tmux) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mlterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kitty) );
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( ! detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT ( detect.getTermType() == "ansi" );
    CPPUNIT_ASSERT ( detect.getTermType_256color() == "" );
    CPPUNIT_ASSERT ( detect.getTermType_Answerback() == "" );
    CPPUNIT_ASSERT ( detect.getAnswerbackString() == "" );
    CPPUNIT_ASSERT ( detect.getTermType_SecDA() == "" );
    CPPUNIT_ASSERT ( detect.getSecDAString() == "" );

    // Test fallback to vt100 without TERM environment variable
    unsetenv("TERM");
    data.unsetTermType(finalcut::FTermType::ansi);
    detect.detect();
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::ansi) );
    CPPUNIT_ASSERT ( detect.getTermType() == "vt100" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::ansi);
    int wstatus;

    if ( waitpid(pid, &wstatus, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;

    if ( WIFEXITED(wstatus) )
      CPPUNIT_ASSERT ( WEXITSTATUS(wstatus) == 0 );
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::xtermTest()
{
  auto& data = finalcut::FTermData::getInstance();
  auto& debug_data = finalcut::FTermDebugData::getInstance();
  auto& detect = finalcut::FTermDetection::getInstance();
  data.setTermType("xterm");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    // (gdb) set follow-fork-mode child
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

    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::xterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::ansi) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::rxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::urxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kde_konsole) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::gnome_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::putty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::win_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tera_term) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::cygwin) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mintty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::linux_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::freebsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::netbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::sun_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::screen) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tmux) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mlterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kitty) );
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT ( detect.getTermType() == "xterm-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_256color() == "xterm-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_Answerback() == "xterm-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_SecDA() == "xterm-256color" );
    CPPUNIT_ASSERT ( debug_data.getTermType_256color() == "xterm-256color" );
    CPPUNIT_ASSERT ( debug_data.getTermType_Answerback() == "xterm-256color" );
    CPPUNIT_ASSERT ( debug_data.getTermType_SecDA() == "xterm-256color" );
    CPPUNIT_ASSERT ( detect.getAnswerbackString() == "" );
    CPPUNIT_ASSERT ( detect.getSecDAString() == "\033[>19;312;0c" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::xterm);
    int wstatus;

    if ( waitpid(pid, &wstatus, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;

    if ( WIFEXITED(wstatus) )
      CPPUNIT_ASSERT ( WEXITSTATUS(wstatus) == 0 );
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::rxvtTest()
{
  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermDetection detect;
  data.setTermType("rxvt-cygwin-native");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    // (gdb) set follow-fork-mode child
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

    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::xterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::ansi) );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::rxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::urxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kde_konsole) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::gnome_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::putty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::win_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tera_term) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::cygwin) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mintty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::linux_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::freebsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::netbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::sun_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::screen) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tmux) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mlterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kitty) );
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT ( detect.getTermType() == "rxvt-16color" );
    CPPUNIT_ASSERT ( detect.getTermType_256color() == "rxvt-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_Answerback() == "rxvt-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_SecDA() == "rxvt-16color" );
    CPPUNIT_ASSERT ( detect.getAnswerbackString() == "" );
    CPPUNIT_ASSERT ( detect.getSecDAString() == "\033[>82;20710;0c" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::rxvt);
    int wstatus;

    if ( waitpid(pid, &wstatus, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;

    if ( WIFEXITED(wstatus) )
      CPPUNIT_ASSERT ( WEXITSTATUS(wstatus) == 0 );
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::urxvtTest()
{
  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermDetection detect;
  data.setTermType("rxvt-unicode-256color");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    // (gdb) set follow-fork-mode child
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

    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::xterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::ansi) );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::rxvt) );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::urxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kde_konsole) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::gnome_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::putty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::win_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tera_term) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::cygwin) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mintty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::linux_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::freebsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::netbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::sun_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::screen) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tmux) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mlterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kitty) );
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT ( detect.getTermType() == "rxvt-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_256color() == "rxvt-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_Answerback() == "rxvt-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_SecDA() == "rxvt-256color" );
    CPPUNIT_ASSERT ( detect.getAnswerbackString() == "" );
    CPPUNIT_ASSERT ( detect.getSecDAString() == "\033[>85;95;0c" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::urxvt);
    int wstatus;

    if ( waitpid(pid, &wstatus, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;

    if ( WIFEXITED(wstatus) )
      CPPUNIT_ASSERT ( WEXITSTATUS(wstatus) == 0 );
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::kdeKonsoleTest()
{
  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermDetection detect;
  data.setTermType("xterm-256color");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    // (gdb) set follow-fork-mode child
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

    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::xterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::ansi) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::rxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::urxvt) );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::kde_konsole) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::gnome_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::putty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::win_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tera_term) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::cygwin) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mintty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::linux_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::freebsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::netbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::sun_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::screen) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tmux) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mlterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kitty) );
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT ( detect.getTermType() == "konsole-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_256color() == "konsole-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_Answerback() == "konsole-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_SecDA() == "konsole-256color" );
    CPPUNIT_ASSERT ( detect.getAnswerbackString() == "" );
    CPPUNIT_ASSERT ( detect.getSecDAString() == "\033[>0;115;0c" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::kde_konsole);
    int wstatus;

    if ( waitpid(pid, &wstatus, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;

    if ( WIFEXITED(wstatus) )
      CPPUNIT_ASSERT ( WEXITSTATUS(wstatus) == 0 );
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::gnomeTerminalTest()
{
  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermDetection detect;
  data.setTermType("xterm-256color");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    // (gdb) set follow-fork-mode child
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

    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::xterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::ansi) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::rxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::urxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kde_konsole) );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::gnome_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::putty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::win_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tera_term) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::cygwin) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mintty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::linux_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::freebsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::netbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::sun_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::screen) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tmux) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mlterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kitty) );
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT ( detect.getTermType() == "gnome-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_256color() == "gnome-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_Answerback() == "gnome-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_SecDA() == "gnome-256color" );
    CPPUNIT_ASSERT ( detect.getAnswerbackString() == "" );
    CPPUNIT_ASSERT ( detect.getSecDAString() == "\033[>1;5202;0c" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::gnome_terminal);
    int wstatus;

    if ( waitpid(pid, &wstatus, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;

    if ( WIFEXITED(wstatus) )
      CPPUNIT_ASSERT ( WEXITSTATUS(wstatus) == 0 );
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::newerVteTerminalTest()
{
  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermDetection detect;
  data.setTermType("xterm-256color");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    // (gdb) set follow-fork-mode child
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

    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::xterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::ansi) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::rxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::urxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kde_konsole) );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::gnome_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::putty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::win_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tera_term) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::cygwin) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mintty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::linux_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::freebsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::netbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::sun_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::screen) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tmux) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mlterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kitty) );
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT ( detect.getTermType() == "gnome-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_256color() == "gnome-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_Answerback() == "gnome-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_SecDA() == "gnome-256color" );
    CPPUNIT_ASSERT ( detect.getAnswerbackString() == "" );
    CPPUNIT_ASSERT ( detect.getSecDAString() == "\033[>65;5300;1c" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::newer_vte_terminal);
    int wstatus;

    if ( waitpid(pid, &wstatus, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;

    if ( WIFEXITED(wstatus) )
      CPPUNIT_ASSERT ( WEXITSTATUS(wstatus) == 0 );
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::puttyTest()
{
  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermDetection detect;
  data.setTermType("xterm");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    // (gdb) set follow-fork-mode child
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

    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::xterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::ansi) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::rxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::urxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kde_konsole) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::gnome_terminal) );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::putty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::win_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tera_term) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::cygwin) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mintty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::linux_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::freebsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::netbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::sun_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::screen) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tmux) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mlterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kitty) );
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT ( detect.getTermType() == "putty-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_256color() == "" );
    CPPUNIT_ASSERT ( detect.getTermType_Answerback() == "putty" );
    CPPUNIT_ASSERT ( detect.getTermType_SecDA() == "putty" );
    CPPUNIT_ASSERT ( detect.getAnswerbackString() == "PuTTY" );
    CPPUNIT_ASSERT ( detect.getSecDAString() == "\033[>0;136;0c" );

    enableConEmuDebug(true);
    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::putty);
    int wstatus;

    if ( waitpid(pid, &wstatus, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;

    if ( WIFEXITED(wstatus) )
      CPPUNIT_ASSERT ( WEXITSTATUS(wstatus) == 0 );
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::windowsTerminalTest()
{
  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermDetection detect;
  data.setTermType("xterm");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    // (gdb) set follow-fork-mode child
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

    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::xterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::ansi) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::rxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::urxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kde_konsole) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::gnome_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::putty) );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::win_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tera_term) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::cygwin) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mintty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::linux_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::freebsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::netbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::sun_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::screen) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tmux) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mlterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kitty) );
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT ( detect.getTermType() == "xterm-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_256color() == "xterm-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_Answerback() == "xterm-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_SecDA() == "xterm-256color" );
    CPPUNIT_ASSERT ( detect.getAnswerbackString() == "" );
    CPPUNIT_ASSERT ( detect.getSecDAString() == "\033[>0;10;1c" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::win_terminal);
    int wstatus;

    if ( waitpid(pid, &wstatus, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;

    if ( WIFEXITED(wstatus) )
      CPPUNIT_ASSERT ( WEXITSTATUS(wstatus) == 0 );
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::teraTermTest()
{
  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermDetection detect;
  data.setTermType("xterm");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    // (gdb) set follow-fork-mode child
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

    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::xterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::ansi) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::rxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::urxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kde_konsole) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::gnome_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::putty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::win_terminal) );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::tera_term) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::cygwin) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mintty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::linux_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::freebsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::netbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::sun_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::screen) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tmux) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mlterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kitty) );
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT ( detect.getTermType() == "teraterm" );
    CPPUNIT_ASSERT ( detect.getTermType_256color() == "" );
    CPPUNIT_ASSERT ( detect.getTermType_Answerback() == "" );
    CPPUNIT_ASSERT ( detect.getTermType_SecDA() == "teraterm" );
    CPPUNIT_ASSERT ( detect.getAnswerbackString() == "" );
    CPPUNIT_ASSERT ( detect.getSecDAString() == "\033[>32;278;0c" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::tera_term);
    int wstatus;

    if ( waitpid(pid, &wstatus, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;

    if ( WIFEXITED(wstatus) )
      CPPUNIT_ASSERT ( WEXITSTATUS(wstatus) == 0 );
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::cygwinTest()
{
  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermDetection detect;
  data.setTermType("cygwin");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    // (gdb) set follow-fork-mode child
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

    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::xterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::ansi) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::rxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::urxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kde_konsole) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::gnome_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::putty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::win_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tera_term) );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::cygwin) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mintty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::linux_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::freebsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::netbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::sun_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::screen) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tmux) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mlterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kitty) );
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT ( detect.getTermType() == "cygwin" );
    CPPUNIT_ASSERT ( detect.getTermType_256color() == "" );
    CPPUNIT_ASSERT ( detect.getTermType_Answerback() == "" );
    CPPUNIT_ASSERT ( detect.getTermType_SecDA() == "" );
    CPPUNIT_ASSERT ( detect.getAnswerbackString() == "" );
    CPPUNIT_ASSERT ( detect.getSecDAString() == "" );

    // Use Sec DA without TERM environment variable
    unsetenv("TERM");
    data.unsetTermType(finalcut::FTermType::cygwin);
    detect.detect();
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::cygwin) );
    CPPUNIT_ASSERT ( detect.getTermType() == "cygwin" );
    CPPUNIT_ASSERT ( detect.getSecDAString() == "\033[>67;200502;0c" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::cygwin);
    int wstatus;

    if ( waitpid(pid, &wstatus, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;

    if ( WIFEXITED(wstatus) )
      CPPUNIT_ASSERT ( WEXITSTATUS(wstatus) == 0 );
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::minttyTest()
{
  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermDetection detect;
  data.setTermType("xterm-256color");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    // (gdb) set follow-fork-mode child
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

    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::xterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::ansi) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::rxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::urxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kde_konsole) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::gnome_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::putty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::win_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tera_term) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::cygwin) );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::mintty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::linux_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::freebsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::netbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::sun_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::screen) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tmux) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mlterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kitty) );
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT ( detect.getTermType() == "xterm-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_256color() == "xterm-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_Answerback() == "xterm-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_SecDA() == "xterm-256color" );
    CPPUNIT_ASSERT ( detect.getAnswerbackString() == "" );
    CPPUNIT_ASSERT ( detect.getSecDAString() == "\033[>77;20402;0c" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::mintty);
    int wstatus;

    if ( waitpid(pid, &wstatus, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;

    if ( WIFEXITED(wstatus) )
      CPPUNIT_ASSERT ( WEXITSTATUS(wstatus) == 0 );
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::linuxTest()
{
  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermDetection detect;
  data.setTermType("linux");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    // (gdb) set follow-fork-mode child
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

    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::xterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::ansi) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::rxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::urxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kde_konsole) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::gnome_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::putty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::win_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tera_term) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::cygwin) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mintty) );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::linux_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::freebsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::netbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::sun_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::screen) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tmux) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mlterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kitty) );
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT ( detect.getTermType() == "linux" );
    CPPUNIT_ASSERT ( detect.getTermType_256color() == "" );
    CPPUNIT_ASSERT ( detect.getTermType_Answerback() == "" );
    CPPUNIT_ASSERT ( detect.getTermType_SecDA() == "" );
    CPPUNIT_ASSERT ( detect.getAnswerbackString() == "" );
    CPPUNIT_ASSERT ( detect.getSecDAString() == "" );

    // Test fallback to vt100 without TERM environment variable
    unsetenv("TERM");
    data.unsetTermType(finalcut::FTermType::linux_con);
    detect.detect();
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::linux_con) );
    CPPUNIT_ASSERT ( detect.getTermType() == "vt100" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::linux_con);
    int wstatus;

    if ( waitpid(pid, &wstatus, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;

    if ( WIFEXITED(wstatus) )
      CPPUNIT_ASSERT ( WEXITSTATUS(wstatus) == 0 );
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::freebsdTest()
{
  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermDetection detect;
  data.setTermType("xterm");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    // (gdb) set follow-fork-mode child
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
    data.setTermType(finalcut::FTermType::freebsd_con);  // Fake FreeBSD Console detection

    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::xterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::ansi) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::rxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::urxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kde_konsole) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::gnome_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::putty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::win_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tera_term) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::cygwin) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mintty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::linux_con) );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::freebsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::netbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::sun_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::screen) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tmux) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mlterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kitty) );
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT ( detect.getTermType() == "xterm-16color" );
    CPPUNIT_ASSERT ( detect.getTermType_256color() == "" );
    CPPUNIT_ASSERT ( detect.getTermType_Answerback() == "" );
    CPPUNIT_ASSERT ( detect.getTermType_SecDA() == "" );
    CPPUNIT_ASSERT ( detect.getAnswerbackString() == "" );
    CPPUNIT_ASSERT ( detect.getSecDAString() == "\033[>0;10;0c" );

    // Test fallback to vt100 without TERM environment variable
    unsetenv("TERM");
    data.unsetTermType(finalcut::FTermType::xterm);
    data.unsetTermType(finalcut::FTermType::freebsd_con);
    detect.detect();
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::xterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::freebsd_con) );
    CPPUNIT_ASSERT ( detect.getTermType() == "vt100" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::freebsd_con);
    int wstatus;

    if ( waitpid(pid, &wstatus, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;

    if ( WIFEXITED(wstatus) )
      CPPUNIT_ASSERT ( WEXITSTATUS(wstatus) == 0 );
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::netbsdTest()
{
  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermDetection detect;
  data.setTermType("wsvt25");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    // (gdb) set follow-fork-mode child
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
    data.setTermType(finalcut::FTermType::netbsd_con);  // Fake NetBSD Console detection

    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::xterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::ansi) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::rxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::urxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kde_konsole) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::gnome_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::putty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::win_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tera_term) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::cygwin) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mintty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::linux_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::freebsd_con) );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::netbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::sun_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::screen) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tmux) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mlterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kitty) );
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT ( detect.getTermType() == "wsvt25" );
    CPPUNIT_ASSERT ( detect.getTermType_256color() == "" );
    CPPUNIT_ASSERT ( detect.getTermType_Answerback() == "" );
    CPPUNIT_ASSERT ( detect.getTermType_SecDA() == "" );
    CPPUNIT_ASSERT ( detect.getAnswerbackString() == "" );
    CPPUNIT_ASSERT ( detect.getSecDAString() == "\033[>24;20;0c" );

    // Test fallback to vt100 without TERM environment variable
    unsetenv("TERM");
    data.unsetTermType(finalcut::FTermType::netbsd_con);
    detect.detect();
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::freebsd_con) );
    CPPUNIT_ASSERT ( detect.getTermType() == "vt100" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::netbsd_con);
    int wstatus;

    if ( waitpid(pid, &wstatus, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;

    if ( WIFEXITED(wstatus) )
      CPPUNIT_ASSERT ( WEXITSTATUS(wstatus) == 0 );
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::openbsdTest()
{
  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermDetection detect;
  data.setTermType("vt220");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    // (gdb) set follow-fork-mode child
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
    data.setTermType(finalcut::FTermType::openbsd_con);  // Fake OpenBSD Console detection

    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::xterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::ansi) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::rxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::urxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kde_konsole) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::gnome_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::putty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::win_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tera_term) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::cygwin) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mintty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::linux_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::freebsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::netbsd_con) );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::sun_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::screen) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tmux) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mlterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kitty) );
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT ( detect.getTermType() == "vt220" );
    CPPUNIT_ASSERT ( detect.getTermType_256color() == "" );
    CPPUNIT_ASSERT ( detect.getTermType_Answerback() == "" );
    CPPUNIT_ASSERT ( detect.getTermType_SecDA() == "" );
    CPPUNIT_ASSERT ( detect.getAnswerbackString() == "" );
    CPPUNIT_ASSERT ( detect.getSecDAString() == "\033[>24;20;0c" );

    // Test fallback to vt100 without TERM environment variable
    unsetenv("TERM");
    data.unsetTermType(finalcut::FTermType::openbsd_con);
    detect.detect();
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( detect.getTermType() == "vt100" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::openbsd_con);
    int wstatus;

    if ( waitpid(pid, &wstatus, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;

    if ( WIFEXITED(wstatus) )
      CPPUNIT_ASSERT ( WEXITSTATUS(wstatus) == 0 );
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::sunTest()
{
  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermDetection detect;
  data.setTermType("sun-color");

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    // (gdb) set follow-fork-mode child
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

    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::xterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::ansi) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::rxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::urxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kde_konsole) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::gnome_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::putty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::win_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tera_term) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::cygwin) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mintty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::linux_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::freebsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::netbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::sun_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::screen) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tmux) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mlterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kitty) );
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( ! detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT ( detect.getTermType() == "sun-color" );
    CPPUNIT_ASSERT ( detect.getTermType_256color() == "" );
    CPPUNIT_ASSERT ( detect.getTermType_Answerback() == "" );
    CPPUNIT_ASSERT ( detect.getTermType_SecDA() == "" );
    CPPUNIT_ASSERT ( detect.getAnswerbackString() == "" );
    CPPUNIT_ASSERT ( detect.getSecDAString() == "" );

    // Test fallback to vt100 without TERM environment variable
    unsetenv("TERM");
    data.unsetTermType(finalcut::FTermType::sun_con);
    detect.detect();
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::sun_con) );
    CPPUNIT_ASSERT ( detect.getTermType() == "vt100" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::sun_con);
    int wstatus;

    if ( waitpid(pid, &wstatus, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;

    if ( WIFEXITED(wstatus) )
      CPPUNIT_ASSERT ( WEXITSTATUS(wstatus) == 0 );
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::screenTest()
{
  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermDetection detect;
  data.setTermType("screen");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    // (gdb) set follow-fork-mode child
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

    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::xterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::ansi) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::rxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::urxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kde_konsole) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::gnome_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::putty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::win_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tera_term) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::cygwin) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mintty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::linux_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::freebsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::netbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::sun_con) );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::screen) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tmux) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mlterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kitty) );
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT ( detect.getTermType() == "screen" );
    CPPUNIT_ASSERT ( detect.getTermType_256color() == "" );
    CPPUNIT_ASSERT ( detect.getTermType_Answerback() == "" );
    CPPUNIT_ASSERT ( detect.getTermType_SecDA() == "" );
    CPPUNIT_ASSERT ( detect.getAnswerbackString() == "" );
    CPPUNIT_ASSERT ( detect.getSecDAString() == "\033[>83;40201;0c" );

    setenv ("XTERM_VERSION", "XTerm(312)", 1);
    detect.detect();
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.getTermType() == "screen-256color" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::screen);
    int wstatus;

    if ( waitpid(pid, &wstatus, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;

    if ( WIFEXITED(wstatus) )
      CPPUNIT_ASSERT ( WEXITSTATUS(wstatus) == 0 );
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::tmuxTest()
{
  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermDetection detect;
  data.setTermType("screen");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    // (gdb) set follow-fork-mode child
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

    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::xterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::ansi) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::rxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::urxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kde_konsole) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::gnome_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::putty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::win_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tera_term) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::cygwin) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mintty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::linux_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::freebsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::netbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::sun_con) );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::screen) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kterm) );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::tmux) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mlterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kitty) );
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT ( detect.getTermType() == "screen" );
    CPPUNIT_ASSERT ( detect.getTermType_256color() == "" );
    CPPUNIT_ASSERT ( detect.getTermType_Answerback() == "" );
    CPPUNIT_ASSERT ( detect.getTermType_SecDA() == "" );
    CPPUNIT_ASSERT ( detect.getAnswerbackString() == "" );
    CPPUNIT_ASSERT ( detect.getSecDAString() == "\033[>84;0;0c" );

    setenv ("VTE_VERSION", "3801", 1);
    detect.detect();
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.getTermType() == "screen-256color" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal simulation
    startConEmuTerminal (ConEmu::console::tmux);

    if ( waitpid(pid, nullptr, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::ktermTest()
{
  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermDetection detect;
  data.setTermType("kterm");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    // (gdb) set follow-fork-mode child
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

    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::xterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::ansi) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::rxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::urxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kde_konsole) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::gnome_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::putty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::win_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tera_term) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::cygwin) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mintty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::linux_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::freebsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::netbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::sun_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::screen) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tmux) );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::kterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mlterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kitty) );
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( ! detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT ( detect.getTermType() == "kterm" );
    CPPUNIT_ASSERT ( detect.getTermType_256color() == "" );
    CPPUNIT_ASSERT ( detect.getTermType_Answerback() == "" );
    CPPUNIT_ASSERT ( detect.getTermType_SecDA() == "" );
    CPPUNIT_ASSERT ( detect.getAnswerbackString() == "" );
    CPPUNIT_ASSERT ( detect.getSecDAString() == "" );

    // Test fallback to vt100 without TERM environment variable
    unsetenv("TERM");
    data.unsetTermType(finalcut::FTermType::kterm);
    detect.detect();
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kterm) );
    CPPUNIT_ASSERT ( detect.getTermType() == "vt100" );
    // kterm sends an incorrect secondary DA ("\033[?1;2c")
    CPPUNIT_ASSERT ( detect.getSecDAString() == "" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::kterm);
    int wstatus;

    if ( waitpid(pid, &wstatus, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;

    if ( WIFEXITED(wstatus) )
      CPPUNIT_ASSERT ( WEXITSTATUS(wstatus) == 0 );
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::mltermTest()
{
  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermDetection detect;
  data.setTermType("mlterm");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    // (gdb) set follow-fork-mode child
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

    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::xterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::ansi) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::rxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::urxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kde_konsole) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::gnome_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::putty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::win_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tera_term) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::cygwin) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mintty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::linux_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::freebsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::netbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::sun_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::screen) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tmux) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kterm) );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::mlterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kitty) );
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT ( detect.getTermType() == "mlterm-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_256color() == "mlterm-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_Answerback() == "mlterm-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_SecDA() == "mlterm-256color" );
    CPPUNIT_ASSERT ( detect.getAnswerbackString() == "" );
    CPPUNIT_ASSERT ( detect.getSecDAString() == "\033[>24;279;0c" );

    setenv ("TERM", "mlterm", 1);
    unsetenv("COLORFGBG");
    detect.detect();
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.getTermType() == "xterm-256color" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::mlterm);
    int wstatus;

    if ( waitpid(pid, &wstatus, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;

    if ( WIFEXITED(wstatus) )
      CPPUNIT_ASSERT ( WEXITSTATUS(wstatus) == 0 );
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::kittyTest()
{
  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermDetection detect;
  data.setTermType("xterm-kitty");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    // (gdb) set follow-fork-mode child
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

    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::xterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::ansi) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::rxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::urxvt) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kde_konsole) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::gnome_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::putty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::win_terminal) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tera_term) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::cygwin) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mintty) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::linux_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::freebsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::netbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::sun_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::screen) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tmux) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kterm) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mlterm) );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::kitty) );
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT ( detect.getTermType() == "xterm-kitty" );
    CPPUNIT_ASSERT ( detect.getTermType_256color() == "xterm-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_Answerback() == "xterm-256color" );
    CPPUNIT_ASSERT ( detect.getTermType_SecDA() == "xterm-kitty" );
    CPPUNIT_ASSERT ( detect.getAnswerbackString() == "" );
    CPPUNIT_ASSERT ( detect.getSecDAString() == "\033[>1;4000;13c" );

    auto kitty_version = data.getKittyVersion();
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
    int wstatus;

    if ( waitpid(pid, &wstatus, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;

    if ( WIFEXITED(wstatus) )
      CPPUNIT_ASSERT ( WEXITSTATUS(wstatus) == 0 );
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
    // (gdb) set follow-fork-mode child
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
    auto& data = finalcut::FTermData::getInstance();

    // Test /dev/tty3 with linux
    data.setTermFileName("/dev/tty3");
    detect.detect();
    CPPUNIT_ASSERT ( detect.getTermType() == "linux" );

    // Test /dev/ttyp0 with vt100
    data.setTermFileName("/dev/ttyp0");
    detect.detect();
    CPPUNIT_ASSERT ( detect.getTermType() == "vt100" );

    // Test non-existent /dev/tty8 with fallback to vt100
    data.setTermFileName("/dev/tty8");
    detect.detect();
    CPPUNIT_ASSERT ( detect.getTermType() == "vt100" );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::ansi);
    int wstatus;

    if ( waitpid(pid, &wstatus, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;

    if ( WIFEXITED(wstatus) )
      CPPUNIT_ASSERT ( WEXITSTATUS(wstatus) == 0 );
  }

  unlink("new-root-dir/etc/ttytype");
  rmdir("new-root-dir/etc");
  rmdir("new-root-dir");
}


// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FTermDetectionTest);

// The general unit test main part
#include <main-test.inc>
