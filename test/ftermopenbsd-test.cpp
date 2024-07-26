/***********************************************************************
* ftermopenbsd-test.cpp - FTermOpenBSD unit tests                      *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2022 Markus Gans                                      *
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

#include <term.h>
#undef buttons  // from term.h

#include <limits>
#include <string>

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <unistd.h>

#include <conemu.h>
#include <final/final.h>


namespace test
{

//----------------------------------------------------------------------
// class FSystemTest
//----------------------------------------------------------------------

class FSystemTest : public finalcut::FSystem
{
  public:
    // Constructor
    FSystemTest();

    // Methods
    auto            inPortByte (uShort) -> uChar override;
    void             outPortByte (uChar, uShort) override;
    auto              isTTY (int) const -> int override;
    auto              ioctl (int, uLong, ...) -> int override;
    auto              open (const char*, int, ...) -> int override;
    auto              close (int) -> int override;
    auto            fopen (const char*, const char*) -> FILE* override;
    auto              fputs (const char*, FILE*) -> int override;
    auto              fclose (FILE*) -> int override;
    auto              putchar (int) -> int override;
    auto            getuid() -> uid_t override;
    auto            geteuid() -> uid_t override;
    auto              getpwuid_r (uid_t, struct passwd*, char*
                                , size_t, struct passwd** ) -> int override;
    auto            realpath (const char*, char*) -> char* override;
    auto getBell() -> wskbd_bell_data&;

  private:
    kbd_t kbdencoding{512};
    wskbd_bell_data system_bell{};
};


// constructors and destructor
//----------------------------------------------------------------------
FSystemTest::FSystemTest()  // constructor
{
  // Initialize bell values
  system_bell.which  = 0;
  system_bell.pitch  = 1500;
  system_bell.period = 100;
  system_bell.volume = 50;
}


// public methods of FSystemTest
//----------------------------------------------------------------------
auto FSystemTest::inPortByte (uShort) -> uChar
{
  return 0;
}

//----------------------------------------------------------------------
void FSystemTest::outPortByte (uChar, uShort)
{
}

//----------------------------------------------------------------------
auto FSystemTest::isTTY (int fd) const -> int
{
  std::cerr << "Call: isatty (fd=" << fd << ")\n";
  return 1;
}

//----------------------------------------------------------------------
auto FSystemTest::ioctl (int fd, uLong request, ...) -> int
{
  va_list args{};
  void* argp{};
  std::string req_string{};
  int ret_val{-1};

  va_start (args, request);
  argp = va_arg (args, void*);

  switch ( request )
  {
    case WSKBDIO_GETENCODING:
    {
      req_string = "WSKBDIO_GETENCODING";
      auto kbd_enc = static_cast<kbd_t*>(argp);
      *kbd_enc = kbdencoding;
      ret_val = 0;
      break;
    }

    case WSKBDIO_SETENCODING:
    {
      req_string = "WSKBDIO_SETENCODING";
      auto kbd_enc = static_cast<kbd_t*>(argp);
      kbdencoding = *kbd_enc;
      ret_val = 0;
      break;
    }

    case WSKBDIO_GETDEFAULTBELL:
    {
      req_string = "WSKBDIO_GETDEFAULTBELL";
      auto spk = static_cast<wskbd_bell_data*>(argp);
      spk->which = 0;
      spk->pitch = 1500;
      spk->period = 100;
      spk->volume = 50;
      ret_val = 0;
      break;
    }

    case WSKBDIO_SETBELL:
    {
      req_string = "WSKBDIO_SETBELL";
      auto spk = static_cast<wskbd_bell_data*>(argp);

      if ( spk->which & WSKBD_BELL_DOPITCH )
        system_bell.pitch = spk->pitch;
      else
        system_bell.pitch = 1500;

      if ( spk->which & WSKBD_BELL_DOPERIOD )
        system_bell.period = spk->period;
      else
        system_bell.period = 100;

      if ( spk->which & WSKBD_BELL_DOVOLUME )
        system_bell.volume = spk->volume;
      else
        system_bell.volume = 50;

      spk->which = WSKBD_BELL_DOALL;
      ret_val = 0;
      break;
    }

    case TIOCGWINSZ:
      req_string = "TIOCGWINSZ";
      auto win_size = static_cast<winsize*>(argp);
      win_size->ws_col = 80;
      win_size->ws_row = 25;
      ret_val = 0;
      break;
  }

  va_end (args);

  std::cerr << "Call: ioctl (fd=" << fd
            << ", request=" << req_string
            << "(0x" << std::hex << request << ")"
            << ", argp=" << argp << std::dec << ")\n";
  return ret_val;
}

//----------------------------------------------------------------------
auto FSystemTest::open (const char* pathname, int flags, ...) -> int
{
  va_list args{};
  va_start (args, flags);
  auto mode = static_cast<mode_t>(va_arg (args, int));
  va_end (args);

  std::cerr << "Call: open (pathname=\"" << pathname
            << "\", flags=" << flags
            << ", mode=" << mode << ")\n";

  return 0;
}

//----------------------------------------------------------------------
auto FSystemTest::close (int fildes) -> int
{
  std::cerr << "Call: close (fildes=" << fildes << ")\n";
  return 0;
}

//----------------------------------------------------------------------
auto FSystemTest::fopen (const char* path, const char* mode) -> FILE*
{
  std::cerr << "Call: fopen (path=" << path
            << ", mode=" << mode << ")\n";
  return nullptr;
}

//----------------------------------------------------------------------
auto FSystemTest::fclose (FILE* fp) -> int
{
  std::cerr << "Call: fclose (fp=" << fp << ")\n";
  return 0;
}

//----------------------------------------------------------------------
auto FSystemTest::fputs (const char* str, FILE* stream) -> int
{
  return std::fputs(str, stream);
}

//----------------------------------------------------------------------
auto FSystemTest::putchar (int c) -> int
{
#if defined(__sun) && defined(__SVR4)
      return std::putchar(char(c));
#else
      return std::putchar(c);
#endif
}

//----------------------------------------------------------------------
auto FSystemTest::getuid() -> uid_t
{
  return 0;
}

//----------------------------------------------------------------------
auto FSystemTest::geteuid() -> uid_t
{
  return 0;
}

//----------------------------------------------------------------------
auto FSystemTest::getpwuid_r ( uid_t, struct passwd*, char*
                            , size_t, struct passwd** ) -> int
{
  return 0;
}

//----------------------------------------------------------------------
auto FSystemTest::realpath (const char*, char*) -> char*
{
  return const_cast<char*>("");
}

//----------------------------------------------------------------------
auto FSystemTest::getBell() -> wskbd_bell_data&
{
  return system_bell;
}

}  // namespace test


//----------------------------------------------------------------------
// class ftermopenbsdTest
//----------------------------------------------------------------------

class ftermopenbsdTest : public CPPUNIT_NS::TestFixture
                       , test::ConEmu
{
  public:
    ftermopenbsdTest() = default;

  protected:
    void classNameTest();
    void netbsdConsoleTest();
    void openbsdConsoleTest();

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (ftermopenbsdTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (netbsdConsoleTest);
    CPPUNIT_TEST (openbsdConsoleTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};

//----------------------------------------------------------------------
void ftermopenbsdTest::classNameTest()
{
  const auto& openbsd = finalcut::FTermOpenBSD::getInstance();
  const finalcut::FString& classname = openbsd.getClassName();
  CPPUNIT_ASSERT ( classname == "FTermOpenBSD" );
}

//----------------------------------------------------------------------
void ftermopenbsdTest::netbsdConsoleTest()
{
  std::unique_ptr<finalcut::FSystem> fsys = std::make_unique<test::FSystemTest>();
  finalcut::FTerm::setFSystem(fsys);
  std::cout << "\n";
  auto& data = finalcut::FTermData::getInstance();

  auto& encoding_list = data.getEncodingList();
  encoding_list["UTF-8"] = finalcut::Encoding::UTF8;
  encoding_list["UTF8"]  = finalcut::Encoding::UTF8;
  encoding_list["VT100"] = finalcut::Encoding::VT100;
  encoding_list["PC"]    = finalcut::Encoding::PC;
  encoding_list["ASCII"] = finalcut::Encoding::ASCII;

  data.setTermEncoding(finalcut::Encoding::VT100);
  data.setBaudrate(9600);
  data.setTermType("wsvt25");
  data.setTermFileName("/dev/ttyE1");
  data.setTTYFileDescriptor(0);
  data.supportShadowCharacter (false);
  data.supportHalfBlockCharacter (false);
  data.supportCursorOptimisation (true);
  data.setCursorHidden (true);
  data.useAlternateScreen (false);
  data.setASCIIConsole (true);
  data.setVT100Console (false);
  data.setUTF8Console (false);
  data.setUTF8 (false);
  data.setNewFont (false);
  data.setVGAFont (false);
  data.setMonochron (false);
  data.setTermResized (false);
  setenv ("TERM", "wsvt25", 1);

  // setupterm is needed for tputs in ncurses >= 6.1
  setupterm (static_cast<char*>(nullptr), 1, static_cast<int*>(nullptr));
  auto& term_detection = finalcut::FTermDetection::getInstance();
  term_detection.setTerminalDetection(true);
  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    // (gdb) set follow-fork-mode child
    finalcut::FTermOpenBSD netbsd;

    setenv ("TERM", "wsvt25", 1);
    setenv ("COLUMNS", "80", 1);
    setenv ("LINES", "25", 1);
    unsetenv("TERMCAP");
    unsetenv("COLORTERM");
    unsetenv("COLORFGBG");
    unsetenv("VTE_VERSION");
    unsetenv("XTERM_VERSION");
    unsetenv("ROXTERM_ID");
    unsetenv("KONSOLE_DBUS_SESSION");
    unsetenv("KONSOLE_DCOP");
    unsetenv("TMUX");

    netbsd.disableMetaSendsEscape();
    netbsd.init();
    term_detection.detect();
    finalcut::FTerm::detectTermSize();

#if DEBUG
    const finalcut::FString& sec_da = \
        finalcut::FTermDebugData::getInstance().getSecDAString();
    CPPUNIT_ASSERT ( sec_da == "\033[>24;20;0c" );
#endif

    CPPUNIT_ASSERT ( isatty(0) == 1 );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::netbsd_con) );
    CPPUNIT_ASSERT ( data.getTerminalGeometry().getWidth() == 80 );
    CPPUNIT_ASSERT ( data.getTerminalGeometry().getHeight() == 25 );
    CPPUNIT_ASSERT ( ! data.hasShadowCharacter() );
    CPPUNIT_ASSERT ( ! data.hasHalfBlockCharacter() );

    netbsd.finish();

    netbsd.enableMetaSendsEscape();
    netbsd.init();

    CPPUNIT_ASSERT ( isatty(0) == 1 );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::netbsd_con) );
    CPPUNIT_ASSERT ( data.getTerminalGeometry().getWidth() == 80 );
    CPPUNIT_ASSERT ( data.getTerminalGeometry().getHeight() == 25 );
    CPPUNIT_ASSERT ( ! data.hasShadowCharacter() );
    CPPUNIT_ASSERT ( ! data.hasHalfBlockCharacter() );

    netbsd.finish();

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
void ftermopenbsdTest::openbsdConsoleTest()
{
  std::unique_ptr<finalcut::FSystem> fsys = std::make_unique<test::FSystemTest>();
  finalcut::FTerm::setFSystem(fsys);
  std::cout << "\n";
  auto& data = finalcut::FTermData::getInstance();

  auto& encoding_list = data.getEncodingList();
  encoding_list["UTF-8"] = finalcut::Encoding::UTF8;
  encoding_list["UTF8"]  = finalcut::Encoding::UTF8;
  encoding_list["VT100"] = finalcut::Encoding::VT100;
  encoding_list["PC"]    = finalcut::Encoding::PC;
  encoding_list["ASCII"] = finalcut::Encoding::ASCII;

  data.setTermEncoding(finalcut::Encoding::VT100);
  data.setBaudrate(9600);
  data.setTermType("vt220");
  data.setTermFileName("/dev/ttyC0");
  data.setTTYFileDescriptor(0);
  data.supportShadowCharacter (false);
  data.supportHalfBlockCharacter (false);
  data.supportCursorOptimisation (true);
  data.setCursorHidden (true);
  data.useAlternateScreen (false);
  data.setASCIIConsole (true);
  data.setVT100Console (false);
  data.setUTF8Console (false);
  data.setUTF8 (false);
  data.setNewFont (false);
  data.setVGAFont (false);
  data.setMonochron (false);
  data.setTermResized (false);
  setenv ("TERM", "vt220", 1);

  // setupterm is needed for tputs in ncurses >= 6.1
  setupterm (static_cast<char*>(nullptr), 1, static_cast<int*>(nullptr));
  auto& term_detection = finalcut::FTermDetection::getInstance();
  term_detection.setTerminalDetection(true);
  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    // (gdb) set follow-fork-mode child
    finalcut::FTermOpenBSD openbsd;

    setenv ("TERM", "vt220", 1);
    setenv ("COLUMNS", "80", 1);
    setenv ("LINES", "25", 1);
    unsetenv("TERMCAP");
    unsetenv("COLORTERM");
    unsetenv("COLORFGBG");
    unsetenv("VTE_VERSION");
    unsetenv("XTERM_VERSION");
    unsetenv("ROXTERM_ID");
    unsetenv("KONSOLE_DBUS_SESSION");
    unsetenv("KONSOLE_DCOP");
    unsetenv("TMUX");

    const auto& fsystem = finalcut::FSystem::getInstance();
    auto fsystest = static_cast<test::FSystemTest*>(fsystem.get());
    wskbd_bell_data& speaker = fsystest->getBell();
    openbsd.disableMetaSendsEscape();
    openbsd.init();
    term_detection.detect();
    finalcut::FTerm::detectTermSize();

#if DEBUG
    const finalcut::FString& sec_da = \
        finalcut::FTermDebugData::getInstance().getSecDAString();
    CPPUNIT_ASSERT ( sec_da == "\033[>24;20;0c" );
#endif

    CPPUNIT_ASSERT ( isatty(0) == 1 );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::netbsd_con) );
    CPPUNIT_ASSERT ( data.getTerminalGeometry().getWidth() == 80 );
    CPPUNIT_ASSERT ( data.getTerminalGeometry().getHeight() == 25 );
    CPPUNIT_ASSERT ( ! data.hasShadowCharacter() );
    CPPUNIT_ASSERT ( ! data.hasHalfBlockCharacter() );
    CPPUNIT_ASSERT ( term_detection.getTermType() == "pccon" );

    openbsd.finish();

    openbsd.enableMetaSendsEscape();
    openbsd.init();

    CPPUNIT_ASSERT ( isatty(0) == 1 );
    CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::openbsd_con) );
    CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::netbsd_con) );
    CPPUNIT_ASSERT ( data.getTerminalGeometry().getWidth() == 80 );
    CPPUNIT_ASSERT ( data.getTerminalGeometry().getHeight() == 25 );
    CPPUNIT_ASSERT ( ! data.hasShadowCharacter() );
    CPPUNIT_ASSERT ( ! data.hasHalfBlockCharacter() );

    CPPUNIT_ASSERT ( speaker.pitch  == 1500 );
    CPPUNIT_ASSERT ( speaker.period == 100 );
    CPPUNIT_ASSERT ( speaker.volume == 50 );
    openbsd.setBeep (20, 100);     // Hz < 21
    CPPUNIT_ASSERT ( speaker.pitch  == 1500 );
    CPPUNIT_ASSERT ( speaker.period == 100 );
    CPPUNIT_ASSERT ( speaker.volume == 50 );
    openbsd.setBeep (32767, 100);  // Hz > 32766
    CPPUNIT_ASSERT ( speaker.pitch  == 1500 );
    CPPUNIT_ASSERT ( speaker.period == 100 );
    CPPUNIT_ASSERT ( speaker.volume == 50 );
    openbsd.setBeep (200, -1);     // ms < 0
    CPPUNIT_ASSERT ( speaker.pitch  == 1500 );
    CPPUNIT_ASSERT ( speaker.period == 100 );
    CPPUNIT_ASSERT ( speaker.volume == 50 );
    openbsd.setBeep (200, 2000);   // ms > 1999
    CPPUNIT_ASSERT ( speaker.pitch  == 1500 );
    CPPUNIT_ASSERT ( speaker.period == 100 );
    CPPUNIT_ASSERT ( speaker.volume == 50 );
    openbsd.setBeep (200, 100);    // 200 Hz - 100 ms
    CPPUNIT_ASSERT ( speaker.pitch  == 200 );
    CPPUNIT_ASSERT ( speaker.period == 100 );
    CPPUNIT_ASSERT ( speaker.volume == 50 );
    openbsd.resetBeep();
    CPPUNIT_ASSERT ( speaker.pitch  == 1500 );
    CPPUNIT_ASSERT ( speaker.period == 100 );
    CPPUNIT_ASSERT ( speaker.volume == 50 );

    openbsd.finish();

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


// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (ftermopenbsdTest);

// The general unit test main part
#include <main-test.inc>
