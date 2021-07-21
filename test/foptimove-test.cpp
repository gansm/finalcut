/***********************************************************************
* foptimove-test.cpp - FOptiMove unit tests                            *
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
                  , CppUnit::SourceLine sourceLine )
{
  if ( s1 == s2 )  // Strings are equal
    return;

  ::CppUnit::Asserter::fail ("Strings are not equal", sourceLine);
}


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
    std::string printSequence (const std::string&);

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
  finalcut::FOptiMove om;
  CPPUNIT_ASSERT_STRING (om.moveCursor (1, 1, 5, 5), CSI "6;6H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 9, 9), CSI "10;10H");

  // Delete all presets
  om.set_tabular (0);
  om.set_back_tab (0);
  om.set_cursor_home (0);
  om.set_cursor_to_ll (0);
  om.set_carriage_return (0);
  om.set_cursor_up (0);
  om.set_cursor_down (0);
  om.set_cursor_right (0);
  om.set_cursor_left (0);
  om.set_cursor_address (0);
  om.set_column_address (0);
  om.set_row_address (0);
  om.set_parm_up_cursor (0);
  om.set_parm_down_cursor (0);
  om.set_parm_right_cursor (0);
  om.set_parm_left_cursor (0);

  CPPUNIT_ASSERT (om.moveCursor (1, 1, 5, 5).empty());
}

//----------------------------------------------------------------------
void FOptiMoveTest::homeTest()
{
  int baud = 4800;
  finalcut::FOptiMove om(baud);
  om.setTermSize (80, 24);
  om.set_cursor_home (CSI "H");
  om.set_cursor_to_ll (CSI "X");
  om.set_carriage_return ("\r");
  om.set_cursor_up (CSI "A");
  om.set_cursor_down (CSI "B");
  om.set_cursor_right (CSI "C");
  om.set_cursor_left (CSI "D");
  om.set_parm_up_cursor (CSI "%p1%dA");
  om.set_parm_down_cursor (CSI "%p1%dB");
  om.set_parm_right_cursor (CSI "%p1%dC");
  om.set_parm_left_cursor (CSI "%p1%dD");

  // Upper home (first line, first column)
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 10, 0, 0), CSI "H");
  // Lower home (last line, first column)
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 10, 0, 23), CSI "X");
}

//----------------------------------------------------------------------
void FOptiMoveTest::fromLeftToRightTest()
{
  int baud = 38400;
  finalcut::FOptiMove om(baud);
  om.setTermSize (80, 24);
  om.setTabStop (8);
  om.set_auto_left_margin (true);
  om.set_eat_newline_glitch (false);
  om.set_carriage_return ("\r");
  om.set_cursor_up (ESC "M");
  om.set_cursor_down (ESC "D");
  om.set_cursor_right (CSI "C");
  om.set_cursor_left ("\b");
  om.set_cursor_address (CSI "%i%p1%d;%p2%dH");
  om.set_column_address (CSI "%i%p1%dG");
  om.set_row_address (CSI "%i%p1%dd");
  om.set_parm_up_cursor (CSI "%p1%dA");
  om.set_parm_down_cursor (CSI "%p1%dB");
  om.set_parm_right_cursor (CSI "%p1%dC");
  om.set_parm_left_cursor (CSI "%p1%dD");

  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 2, 79, 1), "\r\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 2, 79, 2), "\r\b" ESC "D");
}

//----------------------------------------------------------------------
void FOptiMoveTest::ansiTest()
{
  finalcut::FOptiMove om;
  om.setTermSize (80, 25);
  om.setBaudRate (19200);
  om.setTabStop (8);
  om.set_tabular ("\t");
  om.set_back_tab (CSI "Z");
  om.set_cursor_home (CSI "H");
  om.set_carriage_return ("\r");
  om.set_cursor_up (CSI "A");
  om.set_cursor_down (CSI "B");
  om.set_cursor_right (CSI "C");
  om.set_cursor_left (CSI "D");
  om.set_cursor_address (CSI "%i%p1%d;%p2%dH");
  om.set_column_address (CSI "%i%p1%dG");
  om.set_row_address (CSI "%i%p1%dd");
  om.set_parm_up_cursor (CSI "%p1%dA");
  om.set_parm_down_cursor (CSI "%p1%dB");
  om.set_parm_right_cursor (CSI "%p1%dC");
  om.set_parm_left_cursor (CSI "%p1%dD");

  CPPUNIT_ASSERT_STRING (om.moveCursor (0, 0, 5, 5), CSI "6;6H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 0, 0), CSI "H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 1), "\r");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 2), "\r" CSI "B");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 10, 4), CSI "C");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 4, 9, 4), CSI "D");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 11, 4), CSI "12G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (11, 4, 9, 4), CSI "10G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (1, 0, 8, 0), "\t");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 1), CSI "B");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 1, 16, 0), CSI "A");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 2), CSI "3d");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 2, 16, 0), CSI "1d");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 2, 79, 2), CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 75, 20), CSI "21;76H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (39, 0, 32, 0), CSI "Z");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 0, 8, 0), "\r\t");

  // xold is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (99, 10, 79, 10), CSI "11;80H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (-3, 33, 50, 10), CSI "11;51H");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, 33, 23, 10), CSI "11;24H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, -3, 12, 10), CSI "11;13H");

  // xnew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 22, 100, 22), CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 22, -5, 22), "\r");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 22, 53, 40), CSI "25d");
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 2, 53, -3), CSI "1d");
}

//----------------------------------------------------------------------
void FOptiMoveTest::vt100Test()
{
  finalcut::FOptiMove om;
  om.setTermSize (80, 24);
  om.setBaudRate (1200);
  om.setTabStop (8);
  om.set_eat_newline_glitch (true);
  om.set_tabular ("\t");
  om.set_cursor_home (CSI "H");
  om.set_carriage_return ("\r");
  om.set_cursor_up (CSI "A$<2>");
  om.set_cursor_down ("\n");
  om.set_cursor_right (CSI "C$<2>");
  om.set_cursor_left ("\b");
  om.set_cursor_address (CSI "%i%p1%d;%p2%dH$<5>");
  om.set_parm_up_cursor (CSI "%p1%dA");
  om.set_parm_down_cursor (CSI "%p1%dB");
  om.set_parm_right_cursor (CSI "%p1%dC");
  om.set_parm_left_cursor (CSI "%p1%dD");

  CPPUNIT_ASSERT_STRING (om.moveCursor (0, 0, 5, 5), CSI "6;6H$<5>");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 0, 0), CSI "H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 1), "\r");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 2), "\r\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 10, 4), CSI "C$<2>");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 4, 9, 4), "\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 11, 4), CSI "2C");
  CPPUNIT_ASSERT_STRING (om.moveCursor (11, 4, 9, 4), "\b\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (1, 0, 8, 0), "\t");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 1), "\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 1, 16, 0), CSI "A$<2>");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 2), "\n\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 2, 16, 0), CSI "2A");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 2, 79, 2), CSI "76C");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 75, 20), CSI "21;76H$<5>");
  CPPUNIT_ASSERT_STRING (om.moveCursor (39, 0, 32, 0), CSI "7D");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 0, 8, 0), "\b\b");

  // xold is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (99, 10, 79, 10), CSI "11;80H$<5>");
  CPPUNIT_ASSERT_STRING (om.moveCursor (-3, 33, 50, 10), CSI "11;51H$<5>");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, 33, 23, 10), CSI "11;24H$<5>");
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, -3, 12, 10), CSI "11;13H$<5>");

  // xnew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 22, 100, 22), CSI "26C");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 22, -5, 22), "\r");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 22, 53, 40), "\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 2, 53, -3), CSI "2A");
}

//----------------------------------------------------------------------
void FOptiMoveTest::xtermTest()
{
  finalcut::FOptiMove om;
  om.setTermSize (80, 25);
  om.setBaudRate (38400);
  om.setTabStop (8);
  om.set_eat_newline_glitch (true);
  om.set_tabular ("\t");
  om.set_back_tab (CSI "Z");
  om.set_cursor_home (CSI "H");
  om.set_carriage_return ("\r");
  om.set_cursor_up (CSI "A");
  om.set_cursor_down ("\n");
  om.set_cursor_right (CSI "C");
  om.set_cursor_left ("\b");
  om.set_cursor_address (CSI "%i%p1%d;%p2%dH");
  om.set_column_address (CSI "%i%p1%dG");
  om.set_row_address (CSI "%i%p1%dd");
  om.set_parm_up_cursor (CSI "%p1%dA");
  om.set_parm_down_cursor (CSI "%p1%dB");
  om.set_parm_right_cursor (CSI "%p1%dC");
  om.set_parm_left_cursor (CSI "%p1%dD");

  CPPUNIT_ASSERT_STRING (om.moveCursor (0, 0, 5, 5), CSI "6;6H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 0, 0), CSI "H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 1), "\r");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 2), "\r\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 10, 4), CSI "C");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 4, 9, 4), "\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 11, 4), CSI "12G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (11, 4, 9, 4), "\b\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (1, 0, 8, 0), "\t");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 1), "\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 1, 16, 0), CSI "A");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 2), "\n\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 2, 16, 0), CSI "1d");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 2, 79, 2), CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 75, 20), CSI "21;76H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (39, 0, 32, 0), CSI "Z");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 0, 8, 0), "\r\t");

  // xold is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (99, 10, 79, 10), CSI "11;80H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (-3, 33, 50, 10), CSI "11;51H");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, 33, 23, 10), CSI "11;24H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, -3, 12, 10), CSI "11;13H");

  // xnew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 22, 100, 22), CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 22, -5, 22), "\r");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 23, 53, 40), "\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 2, 53, -3), CSI "1d");
}

//----------------------------------------------------------------------
void FOptiMoveTest::rxvtTest()
{
  finalcut::FOptiMove om;
  om.setTermSize (80, 25);
  om.setBaudRate (38400);
  om.setTabStop (8);
  om.set_eat_newline_glitch (true);
  om.set_tabular ("\t");
  om.set_cursor_home (CSI "H");
  om.set_carriage_return ("\r");
  om.set_cursor_up (CSI "A");
  om.set_cursor_down ("\n");
  om.set_cursor_right (CSI "C");
  om.set_cursor_left ("\b");
  om.set_cursor_address (CSI "%i%p1%d;%p2%dH");
  om.set_column_address (CSI "%i%p1%dG");
  om.set_row_address (CSI "%i%p1%dd");
  om.set_parm_up_cursor (CSI "%p1%dA");
  om.set_parm_down_cursor (CSI "%p1%dB");
  om.set_parm_right_cursor (CSI "%p1%dC");
  om.set_parm_left_cursor (CSI "%p1%dD");

  CPPUNIT_ASSERT_STRING (om.moveCursor (0, 0, 5, 5), CSI "6;6H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 0, 0), CSI "H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 1), "\r");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 2), "\r\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 10, 4), CSI "C");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 4, 9, 4), "\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 11, 4), CSI "12G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (11, 4, 9, 4), "\b\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (1, 0, 8, 0), "\t");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 1), "\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 1, 16, 0), CSI "A");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 2), "\n\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 2, 16, 0), CSI "1d");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 2, 79, 2), CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 75, 20), CSI "21;76H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (39, 0, 32, 0), CSI "33G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 0, 8, 0), "\b\b");

  // xold is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (99, 10, 79, 10), CSI "11;80H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (-3, 33, 50, 10), CSI "11;51H");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, 33, 23, 10), CSI "11;24H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, -3, 12, 10), CSI "11;13H");

  // xnew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 22, 100, 22), CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 22, -5, 22), "\r");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 23, 53, 40), "\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 2, 53, -3), CSI "1d");
}

//----------------------------------------------------------------------
void FOptiMoveTest::linuxTest()
{
  finalcut::FOptiMove om;
  om.setTermSize (80, 25);
  om.setBaudRate (38400);
  om.setTabStop (8);
  om.set_eat_newline_glitch (true);
  om.set_tabular ("\t");
  om.set_back_tab (CSI "Z");
  om.set_cursor_home (CSI "H");
  om.set_carriage_return ("\r");
  om.set_cursor_up (CSI "A");
  om.set_cursor_down ("\n");
  om.set_cursor_right (CSI "C");
  om.set_cursor_left ("\b");
  om.set_cursor_address (CSI "%i%p1%d;%p2%dH");
  om.set_column_address (CSI "%i%p1%dG");
  om.set_row_address (CSI "%i%p1%dd");
  om.set_parm_up_cursor (CSI "%p1%dA");
  om.set_parm_down_cursor (CSI "%p1%dB");
  om.set_parm_right_cursor (CSI "%p1%dC");
  om.set_parm_left_cursor (CSI "%p1%dD");

  CPPUNIT_ASSERT_STRING (om.moveCursor (0, 0, 5, 5), CSI "6;6H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 0, 0), CSI "H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 1), "\r");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 2), "\r\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 10, 4), CSI "C");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 4, 9, 4), "\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 11, 4), CSI "12G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (11, 4, 9, 4), "\b\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (1, 0, 8, 0), "\t");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 1), "\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 1, 16, 0), CSI "A");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 2), "\n\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 2, 16, 0), CSI "1d");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 2, 79, 2), CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 75, 20), CSI "21;76H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (39, 0, 32, 0), CSI "Z");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 0, 8, 0), "\r\t");

  // xold is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (99, 10, 79, 10), CSI "11;80H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (-3, 33, 50, 10), CSI "11;51H");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, 33, 23, 10), CSI "11;24H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, -3, 12, 10), CSI "11;13H");

  // xnew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 22, 100, 22), CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 22, -5, 22), "\r");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 23, 53, 40), "\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 2, 53, -3), CSI "1d");
}

//----------------------------------------------------------------------
void FOptiMoveTest::cygwinTest()
{
  finalcut::FOptiMove om;
  om.setTermSize (80, 25);
  om.setBaudRate (38400);
  om.setTabStop (8);
  om.set_tabular ("\t");
  om.set_back_tab (CSI "Z");
  om.set_cursor_home (CSI "H");
  om.set_carriage_return ("\r");
  om.set_cursor_up (CSI "A");
  om.set_cursor_down (CSI "B");
  om.set_cursor_right (CSI "C");
  om.set_cursor_left ("\b");
  om.set_cursor_address (CSI "%i%p1%d;%p2%dH");
  om.set_column_address (CSI "%i%p1%dG");
  om.set_row_address (CSI "%i%p1%dd");
  om.set_parm_up_cursor (CSI "%p1%dA");
  om.set_parm_down_cursor (CSI "%p1%dB");
  om.set_parm_right_cursor (CSI "%p1%dC");
  om.set_parm_left_cursor (CSI "%p1%dD");

  CPPUNIT_ASSERT_STRING ( printSequence(om.moveCursor (1, 2, 3, 4)).c_str()
                         , "Esc [ 5 ; 4 H " );
  CPPUNIT_ASSERT_STRING (om.moveCursor (0, 0, 5, 5), CSI "6;6H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 0, 0), CSI "H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 1), "\r");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 2), "\r" CSI "B");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 10, 4), CSI "C");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 4, 9, 4), "\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 11, 4), CSI "12G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (11, 4, 9, 4), "\b\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (1, 0, 8, 0), "\t");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 1), CSI "B");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 1, 16, 0), CSI "A");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 2), CSI "3d");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 2, 16, 0), CSI "1d");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 2, 79, 2), CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 75, 20), CSI "21;76H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (39, 0, 32, 0), CSI "Z");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 0, 8, 0), "\r\t");

  // xold is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (99, 10, 79, 10), CSI "11;80H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (-3, 33, 50, 10), CSI "11;51H");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, 33, 23, 10), CSI "11;24H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, -3, 12, 10), CSI "11;13H");

  // xnew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 22, 100, 22), CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 22, -5, 22), "\r");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 23, 53, 40), CSI "B");
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 2, 53, -3), CSI "1d");
}

//----------------------------------------------------------------------
void FOptiMoveTest::puttyTest()
{
  finalcut::FOptiMove om;
  om.setTermSize (80, 25);
  om.setBaudRate (38400);
  om.setTabStop (8);
  om.set_auto_left_margin (true);
  om.set_eat_newline_glitch (true);
  om.set_tabular ("\t");
  om.set_back_tab (CSI "Z");
  om.set_cursor_home (CSI "H");
  om.set_carriage_return ("\r");
  om.set_cursor_up (ESC "M");
  om.set_cursor_down (ESC "D");
  om.set_cursor_right (CSI "C");
  om.set_cursor_left ("\b");
  om.set_cursor_address (CSI "%i%p1%d;%p2%dH");
  om.set_column_address (CSI "%i%p1%dG");
  om.set_row_address (CSI "%i%p1%dd");
  om.set_parm_up_cursor (CSI "%p1%dA");
  om.set_parm_down_cursor (CSI "%p1%dB");
  om.set_parm_right_cursor (CSI "%p1%dC");
  om.set_parm_left_cursor (CSI "%p1%dD");

  CPPUNIT_ASSERT_STRING (om.moveCursor (0, 0, 5, 5), CSI "6;6H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 0, 0), CSI "H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 1), "\r");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 2), "\r" ESC "D");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 10, 4), CSI "C");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 4, 9, 4), "\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 11, 4), CSI "12G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (11, 4, 9, 4), "\b\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (1, 0, 8, 0), "\t");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 1), ESC "D");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 1, 16, 0), ESC "M");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 2), ESC "D" ESC "D");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 2, 16, 0), ESC "M" ESC "M");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 2, 79, 2), CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 75, 20), CSI "21;76H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (39, 0, 32, 0), CSI "Z");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 0, 8, 0), "\r\t");

  // xold is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (99, 10, 79, 10), CSI "11;80H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (-3, 33, 50, 10), CSI "11;51H");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, 33, 23, 10), CSI "11;24H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, -3, 12, 10), CSI "11;13H");

  // xnew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 22, 100, 22), CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 22, -5, 22), "\r");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 23, 53, 40), ESC "D");
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 2, 53, -3), ESC "M" ESC "M");
}

//----------------------------------------------------------------------
void FOptiMoveTest::teratermTest()
{
  finalcut::FOptiMove om;
  om.setTermSize (80, 25);
  om.setBaudRate (38400);

  finalcut::FOptiMove::TermEnv optimove_env =
  {
    CSI "H",               // Cursor home
    "\r",                  // Carriage return
    0,                            // Cursor to ll
    "\t",                  // Tabular
    0,                            // Back tabular
    CSI "A",               // Cursor up
    "\n",                  // Cursor down
    "\b",                  // Cursor left
    CSI "C",               // Cursor right
    CSI "%i%p1%d;%p2%dH",  // Cursor address
    CSI "%i%p1%dG",        // Column address
    CSI "%i%p1%dd",        // Row address
    CSI "%p1%dA",          // Parm up cursor
    CSI "%p1%dB",          // Parm down cursor
    CSI "%p1%dD",          // Parm left cursor
    CSI "%p1%dC",          // Parm right cursor
    CSI "%p1%dX",          // Erase characters
    0,                            // Repeat character
    CSI "1K",              // Clear to beginning of line
    CSI "K",               // Clear to end of line
    8,                            // Tab stop
    false,                        // Automatic left margin
    true                          // Eat newline glitch
  };

  om.setTermEnvironment(optimove_env);

  CPPUNIT_ASSERT_STRING (om.moveCursor (0, 0, 5, 5), CSI "6;6H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 0, 0), CSI "H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 1), "\r");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 2), "\r\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 10, 4), CSI "C");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 4, 9, 4), "\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 11, 4), CSI "12G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (11, 4, 9, 4), "\b\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (1, 0, 8, 0), "\t");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 1), "\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 1, 16, 0), CSI "A");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 2), "\n\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 2, 16, 0), CSI "1d");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 2, 79, 2), CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 75, 20), CSI "21;76H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (39, 0, 32, 0), CSI "33G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 0, 8, 0), "\b\b");

  // xold is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (99, 10, 79, 10), CSI "11;80H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (-3, 33, 50, 10), CSI "11;51H");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, 33, 23, 10), CSI "11;24H");
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, -3, 12, 10), CSI "11;13H");

  // xnew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 22, 100, 22), CSI "80G");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 22, -5, 22), "\r");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 23, 53, 40), "\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 2, 53, -3), CSI "1d");
}


//----------------------------------------------------------------------
void FOptiMoveTest::wyse50Test()
{
  finalcut::FOptiMove om;
  om.setTermSize (80, 25);
  om.setBaudRate (38400);
  om.set_auto_left_margin (true);
  om.set_tabular ("\t");
  om.set_back_tab (ESC "I");
  om.set_cursor_home ("\036");
  om.set_cursor_to_ll ("\036\v");
  om.set_carriage_return ("\r");
  om.set_cursor_up ("\v");
  om.set_cursor_down ("\n");
  om.set_cursor_right ("\f");
  om.set_cursor_left ("\b");
  om.set_cursor_address (ESC "=%p1%' '%+%c%p2%' '%+%c");

  //std::cout << "\nSequence: "
  //          << printSequence(om.moveCursor (1, 2, 3, 4))
  //          << "\n";

  CPPUNIT_ASSERT_STRING (om.moveCursor (0, 0, 5, 5), ESC "=%%");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 0, 0), "\036");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 1), "\r");
  CPPUNIT_ASSERT_STRING (om.moveCursor (79, 1, 0, 2), "\r\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 10, 4), "\f");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 4, 9, 4), "\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (9, 4, 11, 4), "\f\f");
  CPPUNIT_ASSERT_STRING (om.moveCursor (11, 4, 9, 4), "\b\b");
  CPPUNIT_ASSERT_STRING (om.moveCursor (1, 0, 8, 0), ESC "= (");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 1), "\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 1, 16, 0), "\v");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 0, 16, 2), "\n\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (16, 2, 16, 0), "\v\v");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 2, 79, 2), "\r\b\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (5, 5, 75, 20), ESC "=4k");
  CPPUNIT_ASSERT_STRING (om.moveCursor (39, 0, 32, 0), ESC "= @");
  CPPUNIT_ASSERT_STRING (om.moveCursor (10, 0, 8, 0), "\b\b");

  // xold is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (99, 10, 79, 10), ESC "=*o");
  CPPUNIT_ASSERT_STRING (om.moveCursor (-3, 33, 50, 10), ESC "=*R");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, 33, 23, 10), ESC "=*7");
  CPPUNIT_ASSERT_STRING (om.moveCursor (23, -3, 12, 10), ESC "=*,");

  // xnew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 22, 100, 22), "\r\b\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (3, 22, -5, 22), "\r");

  // ynew is outside screen
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 23, 53, 40), "\n");
  CPPUNIT_ASSERT_STRING (om.moveCursor (53, 2, 53, -3), "\v\v");
}

//----------------------------------------------------------------------
std::string FOptiMoveTest::printSequence (const std::string& s)
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

  for (std::string::size_type i = 0; i < s.length(); ++i)
  {
    char ch = s[i];

    if ( ch < 0x21 )
      sequence += ctrl_character[uInt(ch)];
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
