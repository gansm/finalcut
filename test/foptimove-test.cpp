/***********************************************************************
* foptimove-test.cpp - FOptiMove unit tests                            *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2026 Markus Gans                                      *
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

#include <iomanip>
#include <string>

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/SourceLine.h>
#include <cppunit/TestAssert.h>

#include <final/final.h>


#define CPPUNIT_ASSERT_STRING(expected, actual) \
            check_string (expected, actual, CPPUNIT_SOURCELINE())

//----------------------------------------------------------------------
void check_string ( const std::string& s1
                  , const std::string& s2
                  , const CppUnit::SourceLine& sourceLine )
{
  if ( s1 == s2 )  // Strings are equal
    return;

  ::CppUnit::Asserter::fail ("Strings are not equal", sourceLine);
}

//----------------------------------------------------------------------
class DirectLogger final : public finalcut::FLog
{
  public:
    // Constructor
    DirectLogger() = default;

    // Destructor
    ~DirectLogger() noexcept override;

    void info (const std::string& entry) override
    {
      output << entry << "\r" << std::endl;
    }

    void warn (const std::string&) override
    {
      // An implementation is not required in this context
    }

    void error (const std::string&) override
    {
      // An implementation is not required in this context
    }


    void debug (const std::string&) override
    {
      // An implementation is not required in this context
    }

    void flush() override
    {
      output.flush();
    }

    void setOutputStream (const std::ostream& os) override
    { output.rdbuf(os.rdbuf()); }

    void setLineEnding (LineEnding) override
    {
      // An implementation is not required in this context
    }


    void enableTimestamp() override
    {
      // An implementation is not required in this context
    }


    void disableTimestamp() override
    {
      // An implementation is not required in this context
    }


  private:
    // Data member
    std::ostream output{std::cerr.rdbuf()};
};

//----------------------------------------------------------------------
DirectLogger::~DirectLogger() noexcept = default;  // destructor


//----------------------------------------------------------------------
// class FOptiMoveTest
//----------------------------------------------------------------------

class FOptiMoveTest : public CPPUNIT_NS::TestFixture
{
  public:
    FOptiMoveTest();

  protected:
    void classNameTest();
    void noArgumentTest();
    void homeTest();
    void fromLeftToRightTest();
    void ansiTest();
    void vt100Test();
    void xtermTest();
    void rxvtTest();
    void linuxTest();
    void cygwinTest();
    void puttyTest();
    void teratermTest();
    void wyse50Test();

  private:
    auto printSequence (const std::string&) -> std::string;

    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FOptiMoveTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (noArgumentTest);
    CPPUNIT_TEST (homeTest);
    CPPUNIT_TEST (fromLeftToRightTest);
    CPPUNIT_TEST (ansiTest);
    CPPUNIT_TEST (vt100Test);
    CPPUNIT_TEST (xtermTest);
    CPPUNIT_TEST (rxvtTest);
    CPPUNIT_TEST (linuxTest);
    CPPUNIT_TEST (cygwinTest);
    CPPUNIT_TEST (puttyTest);
    CPPUNIT_TEST (teratermTest);
    CPPUNIT_TEST (wyse50Test);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};

//----------------------------------------------------------------------
FOptiMoveTest::FOptiMoveTest()
{
  finalcut::FTermcap::init();
  finalcut::FTermcap::setPutCharFunction (::putchar);
  auto putstr = [] (const char* str, uInt32){ return ::fputs(str, stdout); };
  finalcut::FTermcap::setPutStringFunction (putstr);
}

//----------------------------------------------------------------------
void FOptiMoveTest::classNameTest()
{
  finalcut::FOptiMove opti_move;
  const finalcut::FString& classname = opti_move.getClassName();
  CPPUNIT_ASSERT ( classname == "FOptiMove");
}

//----------------------------------------------------------------------
void FOptiMoveTest::noArgumentTest()
{
  finalcut::FTermcap::clearMotionCache();
  finalcut::FOptiMove om;
  CPPUNIT_ASSERT_STRING (om.moveCursor (1, 1, 5, 5).data, CSI "6;6H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 9, 9).data, CSI "10;10H");

  // Delete all presets
  om.set_tabular ({nullptr, 0});
  om.set_back_tab ({nullptr, 0});
  om.set_cursor_home ({nullptr, 0});
  om.set_cursor_to_ll ({nullptr, 0});
  om.set_carriage_return ({nullptr, 0});
  om.set_cursor_up ({nullptr, 0});
  om.set_cursor_down ({nullptr, 0});
  om.set_cursor_right ({nullptr, 0});
  om.set_cursor_left ({nullptr, 0});
  om.set_cursor_address ({nullptr, 0});
  om.set_column_address ({nullptr, 0});
  om.set_row_address ({nullptr, 0});
  om.set_parm_up_cursor ({nullptr, 0});
  om.set_parm_down_cursor ({nullptr, 0});
  om.set_parm_right_cursor ({nullptr, 0});
  om.set_parm_left_cursor ({nullptr, 0});

  CPPUNIT_ASSERT (! om.moveCursor (1, 1, 5, 5).data);
}

//----------------------------------------------------------------------
void FOptiMoveTest::homeTest()
{
  finalcut::FTermcap::clearMotionCache();
  int baud = 4800;
  finalcut::FOptiMove om(baud);
  om.setTermSize (80, 24);
  om.set_cursor_home ({CSI "H", 3});
  om.set_cursor_to_ll ({CSI "X", 3});
  om.set_carriage_return ({"\r", 1});
  om.set_cursor_up ({CSI "A", 3});
  om.set_cursor_down ({CSI "B", 3});
  om.set_cursor_right ({CSI "C", 3});
  om.set_cursor_left ({CSI "D", 3});
  om.set_parm_up_cursor ({CSI "%p1%dA", 8});
  om.set_parm_down_cursor ({CSI "%p1%dB", 8});
  om.set_parm_right_cursor ({CSI "%p1%dC", 8});
  om.set_parm_left_cursor ({CSI "%p1%dD", 8});

  // Upper home (first line, first column)
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 10, 0, 0).data, CSI "H");
  // Lower home (last line, first column)
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 10, 0, 23).data, CSI "X");
}

//----------------------------------------------------------------------
void FOptiMoveTest::fromLeftToRightTest()
{
  finalcut::FTermcap::clearMotionCache();
  int baud = 38400;
  finalcut::FOptiMove om(baud);
  om.setTermSize (80, 24);
  om.setTabStop (8);
  om.set_auto_left_margin (true);
  om.set_eat_newline_glitch (false);
  om.set_carriage_return ({"\r", 1});
  om.set_cursor_up ({ESC "M", 2});
  om.set_cursor_down ({ESC "D", 2});
  om.set_cursor_right ({CSI "C", 3});
  om.set_cursor_left ({"\b", 1});
  om.set_cursor_address ({CSI "%i%p1%d;%p2%dH", 16});
  om.set_column_address ({CSI "%i%p1%dG", 10});
  om.set_row_address ({CSI "%i%p1%dd", 10});
  om.set_parm_up_cursor ({CSI "%p1%dA", 8});
  om.set_parm_down_cursor ({CSI "%p1%dB", 8});
  om.set_parm_right_cursor ({CSI "%p1%dC", 8});
  om.set_parm_left_cursor ({CSI "%p1%dD", 8});

  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 2, 79, 1).data, "\r\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 2, 79, 2).data, "\r\b" ESC "D");
}

//----------------------------------------------------------------------
void FOptiMoveTest::ansiTest()
{
  finalcut::FTermcap::clearMotionCache();
  finalcut::FOptiMove om;
  om.setTermSize (80, 25);
  om.setBaudRate (19200);
  om.setTabStop (8);
  om.set_tabular ({"\t", 1});
  om.set_back_tab ({CSI "Z", 3});
  om.set_cursor_home ({CSI "H", 3});
  om.set_cursor_to_ll ({nullptr, 0});
  om.set_carriage_return ({"\r", 1});
  om.set_cursor_up ({CSI "A", 3});
  om.set_cursor_down ({CSI "B", 3});
  om.set_cursor_right ({CSI "C", 3});
  om.set_cursor_left ({CSI "D", 3});
  om.set_cursor_address ({CSI "%i%p1%d;%p2%dH", 16});
  om.set_column_address ({CSI "%i%p1%dG", 10});
  om.set_row_address ({CSI "%i%p1%dd", 10});
  om.set_parm_up_cursor ({CSI "%p1%dA", 8});
  om.set_parm_down_cursor ({CSI "%p1%dB", 8});
  om.set_parm_right_cursor ({CSI "%p1%dC", 8});
  om.set_parm_left_cursor ({CSI "%p1%dD", 8});

  CPPUNIT_ASSERT_STRING (om.moveCursor (0, 0, 5, 5).data, CSI "6;6H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 0, 0).data, CSI "H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 1).data, "\r");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 2).data, "\r" CSI "B");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 10, 4).data, CSI "C");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 4, 9, 4).data, CSI "D");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 11, 4).data, CSI "12G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (11, 4, 9, 4).data, CSI "10G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (1, 0, 8, 0).data, "\t");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 1).data, CSI "B");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 1, 16, 0).data, CSI "A");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 2).data, CSI "3d");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 2, 16, 0).data, CSI "1d");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 2, 79, 2).data, CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 75, 20).data, CSI "21;76H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (39, 0, 32, 0).data, CSI "Z");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 0, 8, 0).data, "\r\t");

  // xold is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (99, 10, 79, 10).data, CSI "11;80H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (-3, 33, 50, 10).data, CSI "11;51H");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, 33, 23, 10).data, CSI "11;24H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, -3, 12, 10).data, CSI "11;13H");

  // xnew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 22, 100, 22).data, CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 22, -5, 22).data, "\r");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 22, 53, 40).data, CSI "25d");
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 2, 53, -3).data, CSI "1d");

  finalcut::FApplication::setLog(std::make_shared<DirectLogger>());
  std::clog << std::endl;
  finalcut::printDurations(om);
}

//----------------------------------------------------------------------
void FOptiMoveTest::vt100Test()
{
  finalcut::FTermcap::clearMotionCache();
  finalcut::FOptiMove om;
  om.setTermSize (80, 24);
  om.setBaudRate (1200);
  om.setTabStop (8);
  om.set_eat_newline_glitch (true);
  om.set_tabular ({"\t", 1});
  om.set_back_tab ({nullptr, 0});
  om.set_cursor_home ({CSI "H", 3});
  om.set_cursor_to_ll ({nullptr, 0});
  om.set_carriage_return ({"\r", 1});
  om.set_cursor_up ({CSI "A$<2>", 7});
  om.set_cursor_down ({"\n", 1});
  om.set_cursor_right ({CSI "C$<2>", 7});
  om.set_cursor_left ({"\b", 1});
  om.set_cursor_address ({CSI "%i%p1%d;%p2%dH$<5>", 20});
  om.set_column_address ({nullptr, 0});
  om.set_row_address ({nullptr, 0});
  om.set_parm_up_cursor ({CSI "%p1%dA", 8});
  om.set_parm_down_cursor ({CSI "%p1%dB", 8});
  om.set_parm_right_cursor ({CSI "%p1%dC", 8});
  om.set_parm_left_cursor ({CSI "%p1%dD", 8});

  CPPUNIT_ASSERT_STRING (om.moveCursor (0, 0, 5, 5).data, CSI "6;6H$<5>");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 0, 0).data, CSI "H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 1).data, "\r");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 2).data, "\r\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 10, 4).data, CSI "C$<2>");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 4, 9, 4).data, "\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 11, 4).data, CSI "2C");
  CPPUNIT_ASSERT_STRING (om.moveCursor (11, 4, 9, 4).data, "\b\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (1, 0, 8, 0).data, "\t");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 1).data, "\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 1, 16, 0).data, CSI "A$<2>");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 2).data, "\n\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 2, 16, 0).data, CSI "2A");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 2, 79, 2).data, CSI "76C");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 75, 20).data, CSI "21;76H$<5>");
  CPPUNIT_ASSERT_STRING (om.moveCursor (39, 0, 32, 0).data, CSI "7D");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 0, 8, 0).data, "\b\b");

  // xold is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (99, 10, 79, 10).data, CSI "11;80H$<5>");
  CPPUNIT_ASSERT_STRING (om.moveCursor (-3, 33, 50, 10).data, CSI "11;51H$<5>");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, 33, 23, 10).data, CSI "11;24H$<5>");
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, -3, 12, 10).data, CSI "11;13H$<5>");

  // xnew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 22, 100, 22).data, CSI "26C");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 22, -5, 22).data, "\r");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 22, 53, 40).data, "\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 2, 53, -3).data, CSI "2A");

  finalcut::FApplication::setLog(std::make_shared<DirectLogger>());
  std::clog << std::endl;
  finalcut::printDurations(om);
}

//----------------------------------------------------------------------
void FOptiMoveTest::xtermTest()
{
  finalcut::FTermcap::clearMotionCache();
  finalcut::FOptiMove om;
  om.setTermSize (80, 25);
  om.setBaudRate (38400);
  om.setTabStop (8);
  om.set_eat_newline_glitch (true);
  om.set_tabular ({"\t", 1});
  om.set_back_tab ({CSI "Z", 3});
  om.set_cursor_home ({CSI "H", 3});
  om.set_cursor_to_ll ({nullptr, 0});
  om.set_carriage_return ({"\r", 1});
  om.set_cursor_up ({CSI "A", 3});
  om.set_cursor_down ({"\n", 1});
  om.set_cursor_right ({CSI "C", 3});
  om.set_cursor_left ({"\b", 1});
  om.set_cursor_address ({CSI "%i%p1%d;%p2%dH", 16});
  om.set_column_address ({CSI "%i%p1%dG", 10});
  om.set_row_address ({CSI "%i%p1%dd", 10});
  om.set_parm_up_cursor ({CSI "%p1%dA", 8});
  om.set_parm_down_cursor ({CSI "%p1%dB", 8});
  om.set_parm_right_cursor ({CSI "%p1%dC", 8});
  om.set_parm_left_cursor ({CSI "%p1%dD", 8});

  CPPUNIT_ASSERT_STRING (om.moveCursor (0, 0, 5, 5).data, CSI "6;6H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 0, 0).data, CSI "H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 1).data, "\r");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 2).data, "\r\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 10, 4).data, CSI "C");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 4, 9, 4).data, "\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 11, 4).data, CSI "12G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (11, 4, 9, 4).data, "\b\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (1, 0, 8, 0).data, "\t");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 1).data, "\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 1, 16, 0).data, CSI "A");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 2).data, "\n\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 2, 16, 0).data, CSI "1d");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 2, 79, 2).data, CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 75, 20).data, CSI "21;76H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (39, 0, 32, 0).data, CSI "Z");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 0, 8, 0).data, "\r\t");

  // xold is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (99, 10, 79, 10).data, CSI "11;80H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (-3, 33, 50, 10).data, CSI "11;51H");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, 33, 23, 10).data, CSI "11;24H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, -3, 12, 10).data, CSI "11;13H");

  // xnew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 22, 100, 22).data, CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 22, -5, 22).data, "\r");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 23, 53, 40).data, "\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 2, 53, -3).data, CSI "1d");

  finalcut::FApplication::setLog(std::make_shared<DirectLogger>());
  std::clog << std::endl;
  finalcut::printDurations(om);
}

//----------------------------------------------------------------------
void FOptiMoveTest::rxvtTest()
{
  finalcut::FTermcap::clearMotionCache();
  finalcut::FOptiMove om;
  om.setTermSize (80, 25);
  om.setBaudRate (38400);
  om.setTabStop (8);
  om.set_eat_newline_glitch (true);
  om.set_tabular ({"\t", 1});
  om.set_back_tab ({nullptr, 0});
  om.set_cursor_home ({CSI "H", 3});
  om.set_cursor_to_ll ({nullptr, 0});
  om.set_carriage_return ({"\r", 1});
  om.set_cursor_up ({CSI "A", 3});
  om.set_cursor_down ({"\n", 1});
  om.set_cursor_right ({CSI "C", 3});
  om.set_cursor_left ({"\b", 1});
  om.set_cursor_address ({CSI "%i%p1%d;%p2%dH", 16});
  om.set_column_address ({CSI "%i%p1%dG", 10});
  om.set_row_address ({CSI "%i%p1%dd", 10});
  om.set_parm_up_cursor ({CSI "%p1%dA", 8});
  om.set_parm_down_cursor ({CSI "%p1%dB", 8});
  om.set_parm_right_cursor ({CSI "%p1%dC", 8});
  om.set_parm_left_cursor ({CSI "%p1%dD", 8});

  CPPUNIT_ASSERT_STRING (om.moveCursor (0, 0, 5, 5).data, CSI "6;6H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 0, 0).data, CSI "H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 1).data, "\r");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 2).data, "\r\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 10, 4).data, CSI "C");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 4, 9, 4).data, "\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 11, 4).data, CSI "12G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (11, 4, 9, 4).data, "\b\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (1, 0, 8, 0).data, "\t");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 1).data, "\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 1, 16, 0).data, CSI "A");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 2).data, "\n\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 2, 16, 0).data, CSI "1d");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 2, 79, 2).data, CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 75, 20).data, CSI "21;76H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (39, 0, 32, 0).data, CSI "33G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 0, 8, 0).data, "\b\b");

  // xold is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (99, 10, 79, 10).data, CSI "11;80H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (-3, 33, 50, 10).data, CSI "11;51H");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, 33, 23, 10).data, CSI "11;24H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, -3, 12, 10).data, CSI "11;13H");

  // xnew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 22, 100, 22).data, CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 22, -5, 22).data, "\r");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 23, 53, 40).data, "\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 2, 53, -3).data, CSI "1d");

  finalcut::FApplication::setLog(std::make_shared<DirectLogger>());
  std::clog << std::endl;
  finalcut::printDurations(om);
}

//----------------------------------------------------------------------
void FOptiMoveTest::linuxTest()
{
  finalcut::FTermcap::clearMotionCache();
  finalcut::FOptiMove om;
  om.setTermSize (80, 25);
  om.setBaudRate (38400);
  om.setTabStop (8);
  om.set_eat_newline_glitch (true);
  om.set_tabular ({"\t", 1});
  om.set_back_tab ({CSI "Z", 3});
  om.set_cursor_home ({CSI "H", 3});
  om.set_cursor_to_ll ({nullptr, 0});
  om.set_carriage_return ({"\r", 1});
  om.set_cursor_up ({CSI "A", 3});
  om.set_cursor_down ({"\n", 1});
  om.set_cursor_right ({CSI "C", 3});
  om.set_cursor_left ({"\b", 1});
  om.set_cursor_address ({CSI "%i%p1%d;%p2%dH", 16});
  om.set_column_address ({CSI "%i%p1%dG", 10});
  om.set_row_address ({CSI "%i%p1%dd", 10});
  om.set_parm_up_cursor ({CSI "%p1%dA", 8});
  om.set_parm_down_cursor ({CSI "%p1%dB", 8});
  om.set_parm_right_cursor ({CSI "%p1%dC", 8});
  om.set_parm_left_cursor ({CSI "%p1%dD", 8});

  CPPUNIT_ASSERT_STRING (om.moveCursor (0, 0, 5, 5).data, CSI "6;6H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 0, 0).data, CSI "H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 1).data, "\r");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 2).data, "\r\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 10, 4).data, CSI "C");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 4, 9, 4).data, "\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 11, 4).data, CSI "12G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (11, 4, 9, 4).data, "\b\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (1, 0, 8, 0).data, "\t");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 1).data, "\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 1, 16, 0).data, CSI "A");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 2).data, "\n\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 2, 16, 0).data, CSI "1d");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 2, 79, 2).data, CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 75, 20).data, CSI "21;76H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (39, 0, 32, 0).data, CSI "Z");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 0, 8, 0).data, "\r\t");

  // xold is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (99, 10, 79, 10).data, CSI "11;80H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (-3, 33, 50, 10).data, CSI "11;51H");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, 33, 23, 10).data, CSI "11;24H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, -3, 12, 10).data, CSI "11;13H");

  // xnew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 22, 100, 22).data, CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 22, -5, 22).data, "\r");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 23, 53, 40).data, "\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 2, 53, -3).data, CSI "1d");

  finalcut::FApplication::setLog(std::make_shared<DirectLogger>());
  std::clog << std::endl;
  finalcut::printDurations(om);
}

//----------------------------------------------------------------------
void FOptiMoveTest::cygwinTest()
{
  finalcut::FTermcap::clearMotionCache();
  finalcut::FOptiMove om;
  om.setTermSize (80, 25);
  om.setBaudRate (38400);
  om.setTabStop (8);
  om.set_tabular ({"\t", 1});
  om.set_back_tab ({CSI "Z", 3});
  om.set_cursor_home ({CSI "H", 3});
  om.set_cursor_to_ll ({nullptr, 0});
  om.set_carriage_return ({"\r", 1});
  om.set_cursor_up ({CSI "A", 3});
  om.set_cursor_down ({CSI "B", 3});
  om.set_cursor_right ({CSI "C", 3});
  om.set_cursor_left ({"\b", 1});
  om.set_cursor_address ({CSI "%i%p1%d;%p2%dH", 16});
  om.set_column_address ({CSI "%i%p1%dG", 10});
  om.set_row_address ({CSI "%i%p1%dd", 10});
  om.set_parm_up_cursor ({CSI "%p1%dA", 8});
  om.set_parm_down_cursor ({CSI "%p1%dB", 8});
  om.set_parm_right_cursor ({CSI "%p1%dC", 8});
  om.set_parm_left_cursor ({CSI "%p1%dD", 8});

  CPPUNIT_ASSERT_STRING ( printSequence(om.moveCursor (1, 2, 3, 4).data)
                         , "Esc [ 5 ; 4 H " );
  CPPUNIT_ASSERT_STRING (om.moveCursor (0, 0, 5, 5).data, CSI "6;6H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 0, 0).data, CSI "H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 1).data, "\r");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 2).data, "\r" CSI "B");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 10, 4).data, CSI "C");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 4, 9, 4).data, "\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 11, 4).data, CSI "12G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (11, 4, 9, 4).data, "\b\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (1, 0, 8, 0).data, "\t");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 1).data, CSI "B");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 1, 16, 0).data, CSI "A");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 2).data, CSI "3d");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 2, 16, 0).data, CSI "1d");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 2, 79, 2).data, CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 75, 20).data, CSI "21;76H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (39, 0, 32, 0).data, CSI "Z");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 0, 8, 0).data, "\r\t");

  // xold is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (99, 10, 79, 10).data, CSI "11;80H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (-3, 33, 50, 10).data, CSI "11;51H");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, 33, 23, 10).data, CSI "11;24H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, -3, 12, 10).data, CSI "11;13H");

  // xnew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 22, 100, 22).data, CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 22, -5, 22).data, "\r");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 23, 53, 40).data, CSI "B");
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 2, 53, -3).data, CSI "1d");

  finalcut::FApplication::setLog(std::make_shared<DirectLogger>());
  std::clog << std::endl;
  finalcut::printDurations(om);
}

//----------------------------------------------------------------------
void FOptiMoveTest::puttyTest()
{
  finalcut::FTermcap::clearMotionCache();
  finalcut::FOptiMove om;
  om.setTermSize (80, 25);
  om.setBaudRate (38400);
  om.setTabStop (8);
  om.set_auto_left_margin (true);
  om.set_eat_newline_glitch (true);
  om.set_tabular ({"\t", 1});
  om.set_back_tab ({CSI "Z", 3});
  om.set_cursor_home ({CSI "H", 3});
  om.set_cursor_to_ll ({nullptr, 0});
  om.set_carriage_return ({"\r", 1});
  om.set_cursor_up ({ESC "M", 2});
  om.set_cursor_down ({ESC "D", 2});
  om.set_cursor_right ({CSI "C", 3});
  om.set_cursor_left ({"\b", 1});
  om.set_cursor_address ({CSI "%i%p1%d;%p2%dH", 16});
  om.set_column_address ({CSI "%i%p1%dG", 10});
  om.set_row_address ({CSI "%i%p1%dd", 10});
  om.set_parm_up_cursor ({CSI "%p1%dA", 8});
  om.set_parm_down_cursor ({CSI "%p1%dB", 8});
  om.set_parm_right_cursor ({CSI "%p1%dC", 8});
  om.set_parm_left_cursor ({CSI "%p1%dD", 8});

  CPPUNIT_ASSERT_STRING (om.moveCursor (0, 0, 5, 5).data, CSI "6;6H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 0, 0).data, CSI "H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 1).data, "\r");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 2).data, "\r" ESC "D");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 10, 4).data, CSI "C");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 4, 9, 4).data, "\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 11, 4).data, CSI "12G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (11, 4, 9, 4).data, "\b\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (1, 0, 8, 0).data, "\t");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 1).data, ESC "D");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 1, 16, 0).data, ESC "M");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 2).data, ESC "D" ESC "D");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 2, 16, 0).data, ESC "M" ESC "M");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 2, 79, 2).data, CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 75, 20).data, CSI "21;76H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (39, 0, 32, 0).data, CSI "Z");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 0, 8, 0).data, "\r\t");

  // xold is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (99, 10, 79, 10).data, CSI "11;80H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (-3, 33, 50, 10).data, CSI "11;51H");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, 33, 23, 10).data, CSI "11;24H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, -3, 12, 10).data, CSI "11;13H");

  // xnew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 22, 100, 22).data, CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 22, -5, 22).data, "\r");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 23, 53, 40).data, ESC "D");
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 2, 53, -3).data, ESC "M" ESC "M");

  finalcut::FApplication::setLog(std::make_shared<DirectLogger>());
  std::clog << std::endl;
  finalcut::printDurations(om);
}

//----------------------------------------------------------------------
void FOptiMoveTest::teratermTest()
{
  finalcut::FTermcap::clearMotionCache();
  finalcut::FOptiMove om;
  om.setTermSize (80, 25);
  om.setBaudRate (38400);

  finalcut::FOptiMove::TermEnv optimove_env =
  {
    {
      {CSI "A", 3},                // Cursor up
      {"\n", 1},                   // Cursor down
      {"\b", 1},                   // Cursor left
      {CSI "C", 3},                // Cursor right
      {CSI "H", 3},                // Cursor home
      {nullptr, 0},                // Cursor to ll
      {"\r", 1},                   // Carriage return
      {"\t", 1},                   // Tabular
      {nullptr, 0}                 // Back tabular
    },
    {
      {CSI "%p1%dA", 8},           // Parm up cursor
      {CSI "%p1%dB", 8},           // Parm down cursor
      {CSI "%p1%dD", 8},           // Parm left cursor
      {CSI "%p1%dC", 8},           // Parm right cursor
      {CSI "%i%p1%d;%p2%dH", 16},  // Cursor address
      {CSI "%i%p1%dG", 10},        // Column address
      {CSI "%i%p1%dd", 10}         // Row address
    },
    {
      {CSI "%p1%dX", 8},           // Erase characters
      {nullptr, 0},                // Repeat character
      {nullptr, 0},                // Repeat character
      {CSI "1K", 4},               // Clear to beginning of line
      {CSI "K", 3}                 // Clear to end of line
    },
    8,                             // Tab stop
    false,                         // Automatic left margin
    true                           // Eat newline glitch
  };

  om.setTermEnvironment(optimove_env);

  CPPUNIT_ASSERT_STRING (om.moveCursor (0, 0, 5, 5).data, CSI "6;6H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 0, 0).data, CSI "H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 1).data, "\r");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 2).data, "\r\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 10, 4).data, CSI "C");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 4, 9, 4).data, "\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 11, 4).data, CSI "12G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (11, 4, 9, 4).data, "\b\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (1, 0, 8, 0).data, "\t");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 1).data, "\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 1, 16, 0).data, CSI "A");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 2).data, "\n\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 2, 16, 0).data, CSI "1d");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 2, 79, 2).data, CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 75, 20).data, CSI "21;76H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (39, 0, 32, 0).data, CSI "33G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 0, 8, 0).data, "\b\b");

  // xold is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (99, 10, 79, 10).data, CSI "11;80H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (-3, 33, 50, 10).data, CSI "11;51H");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, 33, 23, 10).data, CSI "11;24H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, -3, 12, 10).data, CSI "11;13H");

  // xnew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 22, 100, 22).data, CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 22, -5, 22).data, "\r");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 23, 53, 40).data, "\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 2, 53, -3).data, CSI "1d");

  finalcut::FApplication::setLog(std::make_shared<DirectLogger>());
  std::clog << std::endl;
  finalcut::printDurations(om);
}


//----------------------------------------------------------------------
void FOptiMoveTest::wyse50Test()
{
  finalcut::FTermcap::clearMotionCache();
  finalcut::FOptiMove om;
  om.setTermSize (80, 25);
  om.setBaudRate (38400);
  om.set_auto_left_margin (true);
  om.set_tabular ({"\t", 1});
  om.set_back_tab ({ESC "I", 2});
  om.set_cursor_home ({"\036", 1});
  om.set_cursor_to_ll ({"\036\v", 2});
  om.set_carriage_return ({"\r", 1});
  om.set_cursor_up ({"\v", 1});
  om.set_cursor_down ({"\n", 1});
  om.set_cursor_right ({"\f", 1});
  om.set_cursor_left ({"\b", 1});
  om.set_cursor_address ({ESC "=%p1%' '%+%c%p2%' '%+%c", 24});
  om.set_column_address ({nullptr, 0});
  om.set_row_address ({nullptr, 0});
  om.set_parm_up_cursor ({nullptr, 0});
  om.set_parm_down_cursor ({nullptr, 0});
  om.set_parm_right_cursor ({nullptr, 0});
  om.set_parm_left_cursor ({nullptr, 0});

  //std::cout << "\nSequence: "
  //          << printSequence(om.moveCursor (1, 2, 3, 4))
  //          << "\n";

  CPPUNIT_ASSERT_STRING (om.moveCursor (0, 0, 5, 5).data, ESC "=%%");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 0, 0).data, "\036");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 1).data, "\r");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 2).data, "\r\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 10, 4).data, "\f");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 4, 9, 4).data, "\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 11, 4).data, "\f\f");
  CPPUNIT_ASSERT_STRING (om.moveCursor (11, 4, 9, 4).data, "\b\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (1, 0, 8, 0).data, ESC "= (");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 1).data, "\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 1, 16, 0).data, "\v");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 2).data, "\n\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 2, 16, 0).data, "\v\v");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 2, 79, 2).data, "\r\b\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 75, 20).data, ESC "=4k");
  CPPUNIT_ASSERT_STRING (om.moveCursor (39, 0, 32, 0).data, ESC "= @");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 0, 8, 0).data, "\b\b");

  // xold is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (99, 10, 79, 10).data, ESC "=*o");
  CPPUNIT_ASSERT_STRING (om.moveCursor (-3, 33, 50, 10).data, ESC "=*R");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, 33, 23, 10).data, ESC "=*7");
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, -3, 12, 10).data, ESC "=*,");

  // xnew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 22, 100, 22).data, "\r\b\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 22, -5, 22).data, "\r");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 23, 53, 40).data, "\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 2, 53, -3).data, "\v\v");

  finalcut::FApplication::setLog(std::make_shared<DirectLogger>());
  std::clog << std::endl;
  finalcut::printDurations(om);
}

//----------------------------------------------------------------------
auto FOptiMoveTest::printSequence (const std::string& s) -> std::string
{
  std::string sequence;
  const std::string ctrl_character[] =
  {
    "NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL",
    "BS",  "Tab", "LF",  "VT",  "FF",  "CR",  "SO",  "SI",
    "DLE", "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB",
    "CAN", "EM",  "SUB", "Esc", "FS",  "GS",  "RS",  "US",
    "Space"
  };

  for (char ch : s)
  {
    if ( ch < 0x21 )
      sequence += ctrl_character[uChar(ch)];
    else
      sequence += ch;

    sequence += ' ';
  }

  return sequence;
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FOptiMoveTest);

// The general unit test main part
#include <main-test.inc>
