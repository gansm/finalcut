/***********************************************************************
* ftermopenbsd-test.cpp - FTermOpenBSD unit tests                      *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2019 Markus Gans                                           *
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

#include <limits>

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


namespace test
{

//----------------------------------------------------------------------
// class FSystemTest
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FSystemTest : public finalcut::FSystem
{
  public:
    // Constructor
    FSystemTest();

    // Destructor
    virtual ~FSystemTest();

    // Methods
    virtual uChar    inPortByte (uShort) override;
    virtual void     outPortByte (uChar, uShort) override;
    virtual int      isTTY (int) override;
    virtual int      ioctl (int, uLong, ...) override;
    virtual int      open (const char*, int, ...) override;
    virtual int      close (int) override;
    virtual FILE*    fopen (const char*, const char*) override;
    virtual int      fclose (FILE*) override;
    virtual int      putchar (int) override;
    virtual int      tputs (const char*, int, int (*)(int)) override;
    virtual uid_t    getuid() override;

  private:
    kbd_t kbdencoding = 512;
};
#pragma pack(pop)


// constructors and destructor
//----------------------------------------------------------------------
FSystemTest::FSystemTest()  // constructor
{
}

//----------------------------------------------------------------------
FSystemTest::~FSystemTest()  // destructor
{
}


// public methods of FSystemTest
//----------------------------------------------------------------------
uChar FSystemTest::inPortByte (uShort)
{
  return 0;
}

//----------------------------------------------------------------------
void FSystemTest::outPortByte (uChar, uShort)
{
}

//----------------------------------------------------------------------
int FSystemTest::isTTY (int fd)
{
  std::cerr << "Call: isatty (fd=" << fd << ")\n";
  return 1;
}

//----------------------------------------------------------------------
int FSystemTest::ioctl (int fd, uLong request, ...)
{
  va_list args;
  void* argp;
  std::string req_string;
  int ret_val = -1;

  va_start (args, request);
  argp = va_arg (args, void*);

  switch ( request )
  {
    case WSKBDIO_GETENCODING:
    {
      req_string = "WSKBDIO_GETENCODING";
      kbd_t* kbd_enc = static_cast<kbd_t*>(argp);
      *kbd_enc = kbdencoding;
      ret_val = 0;
      break;
    }

    case WSKBDIO_SETENCODING:
    {
      req_string = "WSKBDIO_SETENCODING";
      kbd_t* kbd_enc = static_cast<kbd_t*>(argp);
      kbdencoding = *kbd_enc;
      ret_val = 0;
      break;
    }

    case TIOCGWINSZ:
    {
      req_string = "TIOCGWINSZ";
      struct winsize* win_size = static_cast<winsize*>(argp);
      win_size->ws_col = 80;
      win_size->ws_row = 25;
      ret_val = 0;
      break;
    }
  }

  va_end (args);

  std::cerr << "Call: ioctl (fd=" << fd
            << ", request=" << req_string
            << "(0x" << std::hex << request << ")"
            << ", argp=" << argp << std::dec << ")\n";
  return ret_val;
}

//----------------------------------------------------------------------
int FSystemTest::open (const char* pathname, int flags, ...)
{
  va_list args;
  va_start (args, flags);
  mode_t mode = static_cast<mode_t>(va_arg (args, int));
  va_end (args);

  std::cerr << "Call: open (pathname=\"" << pathname
            << "\", flags=" << flags
            << ", mode=" << mode << ")\n";

  return 0;
}

//----------------------------------------------------------------------
int FSystemTest::close (int fildes)
{
  std::cerr << "Call: close (fildes=" << fildes << ")\n";
  return 0;
}

//----------------------------------------------------------------------
FILE* FSystemTest::fopen (const char* path, const char* mode)
{
  std::cerr << "Call: fopen (path=" << path
            << ", mode=" << mode << ")\n";
  return 0;
}

//----------------------------------------------------------------------
int FSystemTest::fclose (FILE* fp)
{
  std::cerr << "Call: fclose (fp=" << fp << ")\n";
  return 0;
}

//----------------------------------------------------------------------
int FSystemTest::putchar (int c)
{
#if defined(__sun) && defined(__SVR4)
      return std::putchar(char(c));
#else
      return std::putchar(c);
#endif
}

//----------------------------------------------------------------------
int FSystemTest::tputs (const char* str, int affcnt, int (*putc)(int))
{
  return ::tputs (str, affcnt, putc);
}

//----------------------------------------------------------------------
uid_t FSystemTest::getuid()
{
  return 0;
}

}  // namespace test


//----------------------------------------------------------------------
// class ftermopenbsdTest
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class ftermopenbsdTest : public CPPUNIT_NS::TestFixture, test::ConEmu
{
  public:
    ftermopenbsdTest();

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
#pragma pack(pop)

//----------------------------------------------------------------------
ftermopenbsdTest::ftermopenbsdTest()
{

}

//----------------------------------------------------------------------
void ftermopenbsdTest::classNameTest()
{
  const finalcut::FTermOpenBSD p;
  const char* const classname = p.getClassName();
  CPPUNIT_ASSERT ( std::strcmp(classname, "FTermOpenBSD") == 0 );
}

//----------------------------------------------------------------------
void ftermopenbsdTest::netbsdConsoleTest()
{
  finalcut::FTermData* data;
  finalcut::FSystem* fsys;
  fsys = new test::FSystemTest();
  finalcut::FTerm::setFSystem(fsys);
  finalcut::FTermDetection* term_detection;
  std::cout << "\n";
  data = finalcut::FTerm::getFTermData();

  auto& encoding_list = data->getEncodingList();
  encoding_list["UTF-8"] = finalcut::fc::UTF8;
  encoding_list["UTF8"]  = finalcut::fc::UTF8;
  encoding_list["VT100"] = finalcut::fc::VT100;
  encoding_list["PC"]    = finalcut::fc::PC;
  encoding_list["ASCII"] = finalcut::fc::ASCII;

  data->setTermEncoding(finalcut::fc::VT100);
  data->setBaudrate(9600);
  data->setTermType("wsvt25");
  data->setTermFileName("/dev/ttyE1");
  data->setTTYFileDescriptor(0);
  data->supportShadowCharacter (false);
  data->supportHalfBlockCharacter (false);
  data->supportCursorOptimisation (true);
  data->setCursorHidden (true);
  data->useAlternateScreen (false);
  data->setASCIIConsole (true);
  data->setVT100Console (false);
  data->setUTF8Console (false);
  data->setUTF8 (false);
  data->setNewFont (false);
  data->setVGAFont (false);
  data->setMonochron (false);
  data->setTermResized (false);

  term_detection = finalcut::FTerm::getFTermDetection();
  term_detection->setTerminalDetection(true);
  finalcut::FTermOpenBSD netbsd;

  //term_detection->setNetBSDTerm(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
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

    netbsd.init();
    term_detection->detect();
    finalcut::FTerm::detectTermSize();

    CPPUNIT_ASSERT ( isatty(0) == 1 );
    CPPUNIT_ASSERT ( term_detection->isNetBSDTerm() );
    CPPUNIT_ASSERT ( data->getTermGeometry().getWidth() == 80 );
    CPPUNIT_ASSERT ( data->getTermGeometry().getHeight() == 25 );
    CPPUNIT_ASSERT ( ! data->hasShadowCharacter() );
    CPPUNIT_ASSERT ( ! data->hasHalfBlockCharacter() );

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

  netbsd.finish();
}

//----------------------------------------------------------------------
void ftermopenbsdTest::openbsdConsoleTest()
{
  finalcut::FTermData* data;
  finalcut::FSystem* fsys;
  fsys = new test::FSystemTest();
  finalcut::FTerm::setFSystem(fsys);
  finalcut::FTermDetection* term_detection;
  std::cout << "\n";
  data = finalcut::FTerm::getFTermData();

  auto& encoding_list = data->getEncodingList();
  encoding_list["UTF-8"] = finalcut::fc::UTF8;
  encoding_list["UTF8"]  = finalcut::fc::UTF8;
  encoding_list["VT100"] = finalcut::fc::VT100;
  encoding_list["PC"]    = finalcut::fc::PC;
  encoding_list["ASCII"] = finalcut::fc::ASCII;

  data->setTermEncoding(finalcut::fc::VT100);
  data->setBaudrate(9600);
  data->setTermType("vt220");
  data->setTermFileName("/dev/ttyC0");
  data->setTTYFileDescriptor(0);
  data->supportShadowCharacter (false);
  data->supportHalfBlockCharacter (false);
  data->supportCursorOptimisation (true);
  data->setCursorHidden (true);
  data->useAlternateScreen (false);
  data->setASCIIConsole (true);
  data->setVT100Console (false);
  data->setUTF8Console (false);
  data->setUTF8 (false);
  data->setNewFont (false);
  data->setVGAFont (false);
  data->setMonochron (false);
  data->setTermResized (false);

  term_detection = finalcut::FTerm::getFTermDetection();
  term_detection->setTerminalDetection(true);
  finalcut::FTermOpenBSD openbsd;

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
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

    openbsd.init();
    term_detection->detect();
    finalcut::FTerm::detectTermSize();

    CPPUNIT_ASSERT ( isatty(0) == 1 );
    CPPUNIT_ASSERT ( term_detection->isOpenBSDTerm() );
    CPPUNIT_ASSERT ( data->getTermGeometry().getWidth() == 80 );
    CPPUNIT_ASSERT ( data->getTermGeometry().getHeight() == 25 );
    CPPUNIT_ASSERT ( ! data->hasShadowCharacter() );
    CPPUNIT_ASSERT ( ! data->hasHalfBlockCharacter() );
    CPPUNIT_ASSERT_CSTRING ( term_detection->getTermType(), C_STR("pccon") );
    
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

  openbsd.finish();
}


// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (ftermopenbsdTest);

// The general unit test main part
#include <main-test.inc>
