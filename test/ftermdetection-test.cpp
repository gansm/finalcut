/***********************************************************************
* ftermdetection-test.cpp - FTermDetection unit tests                  *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2018-2019 Markus Gans                                      *
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
    void mltermTest();
    void puttyTest();
    void kdeKonsoleTest();
    void gnomeTerminalTest();
    void newerVteTerminalTest();
    void ktermTest();
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
    CPPUNIT_TEST (mltermTest);
    CPPUNIT_TEST (puttyTest);
    CPPUNIT_TEST (kdeKonsoleTest);
    CPPUNIT_TEST (gnomeTerminalTest);
    CPPUNIT_TEST (newerVteTerminalTest);
    CPPUNIT_TEST (ktermTest);
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
  finalcut::FTermData& data = *finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  setenv ("TERM", "ansi", 1);
  data.setTermType(C_STR("ansi"));

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
    detect.detect();

    CPPUNIT_ASSERT ( ! detect.isXTerminal() );
    CPPUNIT_ASSERT ( detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
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
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( ! detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), C_STR("ansi") );

    // Test fallback to vt100 without TERM environment variable
    unsetenv("TERM");
    detect.setAnsiTerminal(false);
    detect.detect();
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), C_STR("vt100") );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::ansi);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::xtermTest()
{
  finalcut::FTermData& data = *finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType(C_STR("xterm"));
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
    detect.detect();

    CPPUNIT_ASSERT ( detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
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
    startConEmuTerminal (ConEmu::xterm);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::rxvtTest()
{
  finalcut::FTermData& data = *finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType(C_STR("rxvt-cygwin-native"));
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
    detect.detect();

    CPPUNIT_ASSERT ( ! detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
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
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), C_STR("rxvt-cygwin-native") );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::rxvt);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::urxvtTest()
{
  finalcut::FTermData& data = *finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType(C_STR("rxvt-unicode-256color"));
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
    detect.detect();

    CPPUNIT_ASSERT ( ! detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
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
    startConEmuTerminal (ConEmu::urxvt);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::mltermTest()
{
  finalcut::FTermData& data = *finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType(C_STR("mlterm"));
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
    detect.detect();

    CPPUNIT_ASSERT ( ! detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
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
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), C_STR("mlterm-256color") );

    setenv ("TERM", "mlterm", 1);
    unsetenv("COLORFGBG");
    detect.detect();
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), C_STR("xterm-256color") );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::mlterm);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::puttyTest()
{
  finalcut::FTermData& data = *finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType(C_STR("xterm"));
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
    detect.detect();

    CPPUNIT_ASSERT ( detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
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
    startConEmuTerminal (ConEmu::putty);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::kdeKonsoleTest()
{
  finalcut::FTermData& data = *finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType(C_STR("xterm-256color"));
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
    detect.detect();

    CPPUNIT_ASSERT ( detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
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
    startConEmuTerminal (ConEmu::kde_konsole);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::gnomeTerminalTest()
{
  finalcut::FTermData& data = *finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType(C_STR("xterm-256color"));
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

    detect.detect();

    CPPUNIT_ASSERT ( detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
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
    startConEmuTerminal (ConEmu::gnome_terminal);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::newerVteTerminalTest()
{
  finalcut::FTermData& data = *finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType(C_STR("xterm-256color"));
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

    detect.detect();

    CPPUNIT_ASSERT ( detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
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
    startConEmuTerminal (ConEmu::newer_vte_terminal);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::ktermTest()
{
  finalcut::FTermData& data = *finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType(C_STR("kterm"));
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

    detect.detect();

    CPPUNIT_ASSERT ( ! detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( detect.isKtermTerminal() );
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
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( ! detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );

    // Test fallback to vt100 without TERM environment variable
    unsetenv("TERM");
    detect.setKtermTerminal(false);
    detect.detect();
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), C_STR("vt100") );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::kterm);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::teraTermTest()
{
  finalcut::FTermData& data = *finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType(C_STR("xterm"));
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

    detect.detect();

    CPPUNIT_ASSERT ( detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
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
    startConEmuTerminal (ConEmu::tera_term);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::cygwinTest()
{
  finalcut::FTermData& data = *finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType(C_STR("cygwin"));
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

    detect.detect();

    CPPUNIT_ASSERT ( ! detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
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
    startConEmuTerminal (ConEmu::cygwin);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::minttyTest()
{
  finalcut::FTermData& data = *finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType(C_STR("xterm-256color"));
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

    detect.detect();

    CPPUNIT_ASSERT ( detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
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
    startConEmuTerminal (ConEmu::mintty);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::linuxTest()
{
  finalcut::FTermData& data = *finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType(C_STR("linux"));
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

    detect.detect();

    CPPUNIT_ASSERT ( ! detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
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
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );

    // Test fallback to vt100 without TERM environment variable
    unsetenv("TERM");
    detect.setLinuxTerm(false);
    detect.detect();
    CPPUNIT_ASSERT ( ! detect.isLinuxTerm() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), C_STR("vt100") );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::linux_con);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::freebsdTest()
{
  finalcut::FTermData& data = *finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType(C_STR("xterm"));
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

    detect.detect();
    detect.setFreeBSDTerm (true);  // Fake FreeBSD Console detection

    CPPUNIT_ASSERT ( detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
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
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), C_STR("vt100") );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::freebsd_con);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::netbsdTest()
{
  finalcut::FTermData& data = *finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType(C_STR("wsvt25"));
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

    detect.detect();
    detect.setNetBSDTerm (true);  // Fake NetBSD Console detection

    CPPUNIT_ASSERT ( ! detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
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
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );

    // Test fallback to vt100 without TERM environment variable
    unsetenv("TERM");
    detect.setNetBSDTerm(false);
    detect.detect();
    CPPUNIT_ASSERT ( ! detect.isFreeBSDTerm() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), C_STR("vt100") );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::netbsd_con);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::openbsdTest()
{
  finalcut::FTermData& data = *finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType(C_STR("vt220"));
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

    detect.detect();
    detect.setOpenBSDTerm (true);  // Fake OpenBSD Console detection

    CPPUNIT_ASSERT ( ! detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
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
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );

    // Test fallback to vt100 without TERM environment variable
    unsetenv("TERM");
    detect.setOpenBSDTerm(false);
    detect.detect();
    CPPUNIT_ASSERT ( ! detect.isOpenBSDTerm() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), C_STR("vt100") );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::openbsd_con);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::sunTest()
{
  finalcut::FTermData& data = *finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType(C_STR("sun-color"));

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

    detect.detect();

    CPPUNIT_ASSERT ( ! detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
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
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( ! detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );

    // Test fallback to vt100 without TERM environment variable
    unsetenv("TERM");
    detect.setSunTerminal(false);
    detect.detect();
    CPPUNIT_ASSERT ( ! detect.isSunTerminal() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), C_STR("vt100") );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::sun_con);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::screenTest()
{
  finalcut::FTermData& data = *finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType(C_STR("screen"));
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

    detect.detect();

    CPPUNIT_ASSERT ( ! detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
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
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), C_STR("screen") );

    setenv ("XTERM_VERSION", "XTerm(312)", 1);
    detect.detect();
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), C_STR("screen-256color") );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::screen);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}

//----------------------------------------------------------------------
void FTermDetectionTest::tmuxTest()
{
  finalcut::FTermData& data = *finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  data.setTermType(C_STR("screen"));
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

    detect.detect();

    CPPUNIT_ASSERT ( ! detect.isXTerminal() );
    CPPUNIT_ASSERT ( ! detect.isAnsiTerminal() );
    CPPUNIT_ASSERT ( ! detect.isRxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isUrxvtTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMltermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isPuttyTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKdeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isGnomeTerminal() );
    CPPUNIT_ASSERT ( ! detect.isKtermTerminal() );
    CPPUNIT_ASSERT ( ! detect.isTeraTerm() );
    CPPUNIT_ASSERT ( ! detect.isCygwinTerminal() );
    CPPUNIT_ASSERT ( ! detect.isMinttyTerm() );
    CPPUNIT_ASSERT ( ! detect.isLinuxTerm() );
    CPPUNIT_ASSERT ( ! detect.isFreeBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isNetBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isOpenBSDTerm() );
    CPPUNIT_ASSERT ( ! detect.isSunTerminal() );
    CPPUNIT_ASSERT ( detect.isScreenTerm() );
    CPPUNIT_ASSERT ( detect.isTmuxTerm() );
    CPPUNIT_ASSERT ( ! detect.canDisplay256Colors() );
    CPPUNIT_ASSERT ( detect.hasTerminalDetection() );
    CPPUNIT_ASSERT ( ! detect.hasSetCursorStyleSupport() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), C_STR("screen") );

    setenv ("VTE_VERSION", "3801", 1);
    detect.detect();
    CPPUNIT_ASSERT ( detect.canDisplay256Colors() );
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), C_STR("screen-256color") );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal simulation
    startConEmuTerminal (ConEmu::tmux);

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

  finalcut::FTermData& data = *finalcut::FTerm::getFTermData();
  finalcut::FTermDetection detect;
  detect.setTerminalDetection(true);
  detect.setTtyTypeFileName(C_STR("new-root-dir/etc/ttytype"));

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

    // Test /dev/tty3 with linux
    data.setTermFileName(C_STR("/dev/tty3"));
    detect.detect();
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), C_STR("linux") );

    // Test /dev/ttyp0 with vt100
    data.setTermFileName(C_STR("/dev/ttyp0"));
    detect.detect();
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), C_STR("vt100") );

    // Test non-existent /dev/tty8 with fallback to vt100
    data.setTermFileName(C_STR("/dev/tty8"));
    detect.detect();
    CPPUNIT_ASSERT_CSTRING ( detect.getTermType(), C_STR("vt100") );

    printConEmuDebug();
    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::ansi);

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
