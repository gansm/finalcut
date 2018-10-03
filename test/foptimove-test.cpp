/***********************************************************************
* foptimove-test.cpp - FOptiMove unit tests                            *
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

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <cppunit/SourceLine.h>
#include <cppunit/TestAssert.h>

#include <iomanip>
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
// class FOptiMoveTest
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FOptiMoveTest : public CPPUNIT_NS::TestFixture
{
  public:
    FOptiMoveTest()
    { }

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
#pragma pack(pop)


//----------------------------------------------------------------------
void FOptiMoveTest::classNameTest()
{
  finalcut::FOptiMove opti_move;
  const char* const classname = opti_move.getClassName();
  CPPUNIT_ASSERT_CSTRING ( classname, "FOptiMove");
}

//----------------------------------------------------------------------
void FOptiMoveTest::noArgumentTest()
{
  finalcut::FOptiMove om;
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (1, 1, 5, 5), C_STR(CSI "6;6H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (5, 5, 9, 9), C_STR(CSI "10;10H"));

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

  CPPUNIT_ASSERT (om.moveCursor (1, 1, 5, 5) == 0);
}

//----------------------------------------------------------------------
void FOptiMoveTest::homeTest()
{
  int baud = 4800;
  finalcut::FOptiMove om(baud);
  om.setTermSize (80, 24);
  om.set_cursor_home (C_STR(CSI "H"));
  om.set_cursor_to_ll (C_STR(CSI "X"));
  om.set_carriage_return (C_STR("\r"));
  om.set_cursor_up (C_STR(CSI "A"));
  om.set_cursor_down (C_STR(CSI "B"));
  om.set_cursor_right (C_STR(CSI "C"));
  om.set_cursor_left (C_STR(CSI "D"));
  om.set_parm_up_cursor (C_STR(CSI "%p1%dA"));
  om.set_parm_down_cursor (C_STR(CSI "%p1%dB"));
  om.set_parm_right_cursor (C_STR(CSI "%p1%dC"));
  om.set_parm_left_cursor (C_STR(CSI "%p1%dD"));

  // Upper home (first line, first column)
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (10, 10, 0, 0), C_STR(CSI "H"));
  // Lower home (last line, first column)
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (10, 10, 0, 23), C_STR(CSI "X"));
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
  om.set_carriage_return (C_STR("\r"));
  om.set_cursor_up (C_STR(ESC "M"));
  om.set_cursor_down (C_STR(ESC "D"));
  om.set_cursor_right (C_STR(CSI "C"));
  om.set_cursor_left (C_STR("\b"));
  om.set_cursor_address (C_STR(CSI "%i%p1%d;%p2%dH"));
  om.set_column_address (C_STR(CSI "%i%p1%dG"));
  om.set_row_address (C_STR(CSI "%i%p1%dd"));
  om.set_parm_up_cursor (C_STR(CSI "%p1%dA"));
  om.set_parm_down_cursor (C_STR(CSI "%p1%dB"));
  om.set_parm_right_cursor (C_STR(CSI "%p1%dC"));
  om.set_parm_left_cursor (C_STR(CSI "%p1%dD"));

  CPPUNIT_ASSERT_CSTRING (om.moveCursor (3, 2, 79, 1), C_STR("\r\b"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (3, 2, 79, 2), C_STR("\r\b" ESC "D"));
}

//----------------------------------------------------------------------
void FOptiMoveTest::ansiTest()
{
  finalcut::FOptiMove om;
  om.setTermSize (80, 25);
  om.setBaudRate (19200);
  om.setTabStop (8);
  om.set_tabular (C_STR("\t"));
  om.set_back_tab (C_STR(CSI "Z"));
  om.set_cursor_home (C_STR(CSI "H"));
  om.set_carriage_return (C_STR("\r"));
  om.set_cursor_up (C_STR(CSI "A"));
  om.set_cursor_down (C_STR(CSI "B"));
  om.set_cursor_right (C_STR(CSI "C"));
  om.set_cursor_left (C_STR(CSI "D"));
  om.set_cursor_address (C_STR(CSI "%i%p1%d;%p2%dH"));
  om.set_column_address (C_STR(CSI "%i%p1%dG"));
  om.set_row_address (C_STR(CSI "%i%p1%dd"));
  om.set_parm_up_cursor (C_STR(CSI "%p1%dA"));
  om.set_parm_down_cursor (C_STR(CSI "%p1%dB"));
  om.set_parm_right_cursor (C_STR(CSI "%p1%dC"));
  om.set_parm_left_cursor (C_STR(CSI "%p1%dD"));

  CPPUNIT_ASSERT_CSTRING (om.moveCursor (0, 0, 5, 5), C_STR(CSI "6;6H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (5, 5, 0, 0), C_STR(CSI "H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (79, 1, 0, 1), C_STR("\r"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (79, 1, 0, 2), C_STR("\r" CSI "B"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (9, 4, 10, 4), C_STR(CSI "C"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (10, 4, 9, 4), C_STR(CSI "D"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (9, 4, 11, 4), C_STR(CSI "12G"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (11, 4, 9, 4), C_STR(CSI "10G"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (1, 0, 8, 0), C_STR("\t"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 0, 16, 1), C_STR(CSI "B"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 1, 16, 0), C_STR(CSI "A"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 0, 16, 2), C_STR(CSI "3d"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 2, 16, 0), C_STR(CSI "1d"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (3, 2, 79, 2), C_STR(CSI "80G"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (5, 5, 75, 20), C_STR(CSI "21;76H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (39, 0, 32, 0), C_STR(CSI "Z"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (10, 0, 8, 0), C_STR("\r\t"));

  // xold is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (99, 10, 79, 10), C_STR(CSI "11;80H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (-3, 33, 50, 10), C_STR(CSI "11;51H"));

  // ynew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (23, 33, 23, 10), C_STR(CSI "11;24H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (23, -3, 12, 10), C_STR(CSI "11;13H"));

  // xnew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 22, 100, 22), C_STR(CSI "80G"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (3, 22, -5, 22), C_STR("\r"));

  // ynew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 22, 53, 40), C_STR(CSI "25d"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 2, 53, -3), C_STR(CSI "1d"));
}

//----------------------------------------------------------------------
void FOptiMoveTest::vt100Test()
{
  finalcut::FOptiMove om;
  om.setTermSize (80, 24);
  om.setBaudRate (1200);
  om.setTabStop (8);
  om.set_eat_newline_glitch (true);
  om.set_tabular (C_STR("\t"));
  om.set_cursor_home (C_STR(CSI "H"));
  om.set_carriage_return (C_STR("\r"));
  om.set_cursor_up (C_STR(CSI "A$<2>"));
  om.set_cursor_down (C_STR("\n"));
  om.set_cursor_right (C_STR(CSI "C$<2>"));
  om.set_cursor_left (C_STR("\b"));
  om.set_cursor_address (C_STR(CSI "%i%p1%d;%p2%dH$<5>"));
  om.set_parm_up_cursor (C_STR(CSI "%p1%dA"));
  om.set_parm_down_cursor (C_STR(CSI "%p1%dB"));
  om.set_parm_right_cursor (C_STR(CSI "%p1%dC"));
  om.set_parm_left_cursor (C_STR(CSI "%p1%dD"));

  CPPUNIT_ASSERT_CSTRING (om.moveCursor (0, 0, 5, 5), C_STR(CSI "6;6H$<5>"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (5, 5, 0, 0), C_STR(CSI "H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (79, 1, 0, 1), C_STR("\r"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (79, 1, 0, 2), C_STR("\r\n"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (9, 4, 10, 4), C_STR(CSI "C$<2>"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (10, 4, 9, 4), C_STR("\b"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (9, 4, 11, 4), C_STR(CSI "2C"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (11, 4, 9, 4), C_STR("\b\b"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (1, 0, 8, 0), C_STR("\t"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 0, 16, 1), C_STR("\n"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 1, 16, 0), C_STR(CSI "A$<2>"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 0, 16, 2), C_STR("\n\n"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 2, 16, 0), C_STR(CSI "2A"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (3, 2, 79, 2), C_STR(CSI "76C"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (5, 5, 75, 20), C_STR(CSI "21;76H$<5>"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (39, 0, 32, 0), C_STR(CSI "7D"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (10, 0, 8, 0), C_STR("\b\b"));

  // xold is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (99, 10, 79, 10), C_STR(CSI "11;80H$<5>"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (-3, 33, 50, 10), C_STR(CSI "11;51H$<5>"));

  // ynew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (23, 33, 23, 10), C_STR(CSI "11;24H$<5>"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (23, -3, 12, 10), C_STR(CSI "11;13H$<5>"));

  // xnew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 22, 100, 22), C_STR(CSI "26C"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (3, 22, -5, 22), C_STR("\r"));

  // ynew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 22, 53, 40), C_STR("\n"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 2, 53, -3), C_STR(CSI "2A"));
}

//----------------------------------------------------------------------
void FOptiMoveTest::xtermTest()
{
  finalcut::FOptiMove om;
  om.setTermSize (80, 25);
  om.setBaudRate (38400);
  om.setTabStop (8);
  om.set_eat_newline_glitch (true);
  om.set_tabular (C_STR("\t"));
  om.set_back_tab (C_STR(CSI "Z"));
  om.set_cursor_home (C_STR(CSI "H"));
  om.set_carriage_return (C_STR("\r"));
  om.set_cursor_up (C_STR(CSI "A"));
  om.set_cursor_down (C_STR("\n"));
  om.set_cursor_right (C_STR(CSI "C"));
  om.set_cursor_left (C_STR("\b"));
  om.set_cursor_address (C_STR(CSI "%i%p1%d;%p2%dH"));
  om.set_column_address (C_STR(CSI "%i%p1%dG"));
  om.set_row_address (C_STR(CSI "%i%p1%dd"));
  om.set_parm_up_cursor (C_STR(CSI "%p1%dA"));
  om.set_parm_down_cursor (C_STR(CSI "%p1%dB"));
  om.set_parm_right_cursor (C_STR(CSI "%p1%dC"));
  om.set_parm_left_cursor (C_STR(CSI "%p1%dD"));

  CPPUNIT_ASSERT_CSTRING (om.moveCursor (0, 0, 5, 5), C_STR(CSI "6;6H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (5, 5, 0, 0), C_STR(CSI "H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (79, 1, 0, 1), C_STR("\r"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (79, 1, 0, 2), C_STR("\r\n"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (9, 4, 10, 4), C_STR(CSI "C"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (10, 4, 9, 4), C_STR("\b"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (9, 4, 11, 4), C_STR(CSI "12G"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (11, 4, 9, 4), C_STR("\b\b"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (1, 0, 8, 0), C_STR("\t"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 0, 16, 1), C_STR("\n"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 1, 16, 0), C_STR(CSI "A"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 0, 16, 2), C_STR("\n\n"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 2, 16, 0), C_STR(CSI "1d"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (3, 2, 79, 2), C_STR(CSI "80G"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (5, 5, 75, 20), C_STR(CSI "21;76H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (39, 0, 32, 0), C_STR(CSI "Z"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (10, 0, 8, 0), C_STR("\r\t"));

  // xold is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (99, 10, 79, 10), C_STR(CSI "11;80H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (-3, 33, 50, 10), C_STR(CSI "11;51H"));

  // ynew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (23, 33, 23, 10), C_STR(CSI "11;24H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (23, -3, 12, 10), C_STR(CSI "11;13H"));

  // xnew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 22, 100, 22), C_STR(CSI "80G"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (3, 22, -5, 22), C_STR("\r"));

  // ynew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 23, 53, 40), C_STR("\n"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 2, 53, -3), C_STR(CSI "1d"));
}

//----------------------------------------------------------------------
void FOptiMoveTest::rxvtTest()
{
  finalcut::FOptiMove om;
  om.setTermSize (80, 25);
  om.setBaudRate (38400);
  om.setTabStop (8);
  om.set_eat_newline_glitch (true);
  om.set_tabular (C_STR("\t"));
  om.set_cursor_home (C_STR(CSI "H"));
  om.set_carriage_return (C_STR("\r"));
  om.set_cursor_up (C_STR(CSI "A"));
  om.set_cursor_down (C_STR("\n"));
  om.set_cursor_right (C_STR(CSI "C"));
  om.set_cursor_left (C_STR("\b"));
  om.set_cursor_address (C_STR(CSI "%i%p1%d;%p2%dH"));
  om.set_column_address (C_STR(CSI "%i%p1%dG"));
  om.set_row_address (C_STR(CSI "%i%p1%dd"));
  om.set_parm_up_cursor (C_STR(CSI "%p1%dA"));
  om.set_parm_down_cursor (C_STR(CSI "%p1%dB"));
  om.set_parm_right_cursor (C_STR(CSI "%p1%dC"));
  om.set_parm_left_cursor (C_STR(CSI "%p1%dD"));

  CPPUNIT_ASSERT_CSTRING (om.moveCursor (0, 0, 5, 5), C_STR(CSI "6;6H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (5, 5, 0, 0), C_STR(CSI "H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (79, 1, 0, 1), C_STR("\r"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (79, 1, 0, 2), C_STR("\r\n"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (9, 4, 10, 4), C_STR(CSI "C"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (10, 4, 9, 4), C_STR("\b"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (9, 4, 11, 4), C_STR(CSI "12G"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (11, 4, 9, 4), C_STR("\b\b"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (1, 0, 8, 0), C_STR("\t"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 0, 16, 1), C_STR("\n"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 1, 16, 0), C_STR(CSI "A"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 0, 16, 2), C_STR("\n\n"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 2, 16, 0), C_STR(CSI "1d"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (3, 2, 79, 2), C_STR(CSI "80G"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (5, 5, 75, 20), C_STR(CSI "21;76H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (39, 0, 32, 0), C_STR(CSI "33G"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (10, 0, 8, 0), C_STR("\b\b"));

  // xold is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (99, 10, 79, 10), C_STR(CSI "11;80H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (-3, 33, 50, 10), C_STR(CSI "11;51H"));

  // ynew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (23, 33, 23, 10), C_STR(CSI "11;24H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (23, -3, 12, 10), C_STR(CSI "11;13H"));

  // xnew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 22, 100, 22), C_STR(CSI "80G"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (3, 22, -5, 22), C_STR("\r"));

  // ynew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 23, 53, 40), C_STR("\n"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 2, 53, -3), C_STR(CSI "1d"));
}

//----------------------------------------------------------------------
void FOptiMoveTest::linuxTest()
{
  finalcut::FOptiMove om;
  om.setTermSize (80, 25);
  om.setBaudRate (38400);
  om.setTabStop (8);
  om.set_eat_newline_glitch (true);
  om.set_tabular (C_STR("\t"));
  om.set_back_tab (C_STR(CSI "Z"));
  om.set_cursor_home (C_STR(CSI "H"));
  om.set_carriage_return (C_STR("\r"));
  om.set_cursor_up (C_STR(CSI "A"));
  om.set_cursor_down (C_STR("\n"));
  om.set_cursor_right (C_STR(CSI "C"));
  om.set_cursor_left (C_STR("\b"));
  om.set_cursor_address (C_STR(CSI "%i%p1%d;%p2%dH"));
  om.set_column_address (C_STR(CSI "%i%p1%dG"));
  om.set_row_address (C_STR(CSI "%i%p1%dd"));
  om.set_parm_up_cursor (C_STR(CSI "%p1%dA"));
  om.set_parm_down_cursor (C_STR(CSI "%p1%dB"));
  om.set_parm_right_cursor (C_STR(CSI "%p1%dC"));
  om.set_parm_left_cursor (C_STR(CSI "%p1%dD"));

  CPPUNIT_ASSERT_CSTRING (om.moveCursor (0, 0, 5, 5), C_STR(CSI "6;6H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (5, 5, 0, 0), C_STR(CSI "H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (79, 1, 0, 1), C_STR("\r"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (79, 1, 0, 2), C_STR("\r\n"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (9, 4, 10, 4), C_STR(CSI "C"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (10, 4, 9, 4), C_STR("\b"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (9, 4, 11, 4), C_STR(CSI "12G"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (11, 4, 9, 4), C_STR("\b\b"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (1, 0, 8, 0), C_STR("\t"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 0, 16, 1), C_STR("\n"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 1, 16, 0), C_STR(CSI "A"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 0, 16, 2), C_STR("\n\n"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 2, 16, 0), C_STR(CSI "1d"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (3, 2, 79, 2), C_STR(CSI "80G"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (5, 5, 75, 20), C_STR(CSI "21;76H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (39, 0, 32, 0), C_STR(CSI "Z"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (10, 0, 8, 0), C_STR("\r\t"));

  // xold is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (99, 10, 79, 10), C_STR(CSI "11;80H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (-3, 33, 50, 10), C_STR(CSI "11;51H"));

  // ynew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (23, 33, 23, 10), C_STR(CSI "11;24H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (23, -3, 12, 10), C_STR(CSI "11;13H"));

  // xnew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 22, 100, 22), C_STR(CSI "80G"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (3, 22, -5, 22), C_STR("\r"));

  // ynew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 23, 53, 40), C_STR("\n"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 2, 53, -3), C_STR(CSI "1d"));
}

//----------------------------------------------------------------------
void FOptiMoveTest::cygwinTest()
{
  finalcut::FOptiMove om;
  om.setTermSize (80, 25);
  om.setBaudRate (38400);
  om.setTabStop (8);
  om.set_tabular (C_STR("\t"));
  om.set_back_tab (C_STR(CSI "Z"));
  om.set_cursor_home (C_STR(CSI "H"));
  om.set_carriage_return (C_STR("\r"));
  om.set_cursor_up (C_STR(CSI "A"));
  om.set_cursor_down (C_STR(CSI "B"));
  om.set_cursor_right (C_STR(CSI "C"));
  om.set_cursor_left (C_STR("\b"));
  om.set_cursor_address (C_STR(CSI "%i%p1%d;%p2%dH"));
  om.set_column_address (C_STR(CSI "%i%p1%dG"));
  om.set_row_address (C_STR(CSI "%i%p1%dd"));
  om.set_parm_up_cursor (C_STR(CSI "%p1%dA"));
  om.set_parm_down_cursor (C_STR(CSI "%p1%dB"));
  om.set_parm_right_cursor (C_STR(CSI "%p1%dC"));
  om.set_parm_left_cursor (C_STR(CSI "%p1%dD"));

  CPPUNIT_ASSERT_CSTRING ( printSequence(om.moveCursor (1, 2, 3, 4)).c_str()
                         , C_STR("Esc [ 5 ; 4 H ") );
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (0, 0, 5, 5), C_STR(CSI "6;6H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (5, 5, 0, 0), C_STR(CSI "H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (79, 1, 0, 1), C_STR("\r"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (79, 1, 0, 2), C_STR("\r" CSI "B"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (9, 4, 10, 4), C_STR(CSI "C"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (10, 4, 9, 4), C_STR("\b"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (9, 4, 11, 4), C_STR(CSI "12G"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (11, 4, 9, 4), C_STR("\b\b"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (1, 0, 8, 0), C_STR("\t"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 0, 16, 1), C_STR(CSI "B"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 1, 16, 0), C_STR(CSI "A"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 0, 16, 2), C_STR(CSI "3d"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 2, 16, 0), C_STR(CSI "1d"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (3, 2, 79, 2), C_STR(CSI "80G"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (5, 5, 75, 20), C_STR(CSI "21;76H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (39, 0, 32, 0), C_STR(CSI "Z"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (10, 0, 8, 0), C_STR("\r\t"));

  // xold is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (99, 10, 79, 10), C_STR(CSI "11;80H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (-3, 33, 50, 10), C_STR(CSI "11;51H"));

  // ynew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (23, 33, 23, 10), C_STR(CSI "11;24H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (23, -3, 12, 10), C_STR(CSI "11;13H"));

  // xnew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 22, 100, 22), C_STR(CSI "80G"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (3, 22, -5, 22), C_STR("\r"));

  // ynew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 23, 53, 40), C_STR(CSI "B"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 2, 53, -3), C_STR(CSI "1d"));
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
  om.set_tabular (C_STR("\t"));
  om.set_back_tab (C_STR(CSI "Z"));
  om.set_cursor_home (C_STR(CSI "H"));
  om.set_carriage_return (C_STR("\r"));
  om.set_cursor_up (C_STR(ESC "M"));
  om.set_cursor_down (C_STR(ESC "D"));
  om.set_cursor_right (C_STR(CSI "C"));
  om.set_cursor_left (C_STR("\b"));
  om.set_cursor_address (C_STR(CSI "%i%p1%d;%p2%dH"));
  om.set_column_address (C_STR(CSI "%i%p1%dG"));
  om.set_row_address (C_STR(CSI "%i%p1%dd"));
  om.set_parm_up_cursor (C_STR(CSI "%p1%dA"));
  om.set_parm_down_cursor (C_STR(CSI "%p1%dB"));
  om.set_parm_right_cursor (C_STR(CSI "%p1%dC"));
  om.set_parm_left_cursor (C_STR(CSI "%p1%dD"));

  CPPUNIT_ASSERT_CSTRING (om.moveCursor (0, 0, 5, 5), C_STR(CSI "6;6H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (5, 5, 0, 0), C_STR(CSI "H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (79, 1, 0, 1), C_STR("\r"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (79, 1, 0, 2), C_STR("\r" ESC "D"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (9, 4, 10, 4), C_STR(CSI "C"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (10, 4, 9, 4), C_STR("\b"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (9, 4, 11, 4), C_STR(CSI "12G"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (11, 4, 9, 4), C_STR("\b\b"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (1, 0, 8, 0), C_STR("\t"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 0, 16, 1), C_STR(ESC "D"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 1, 16, 0), C_STR(ESC "M"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 0, 16, 2), C_STR(ESC "D" ESC "D"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 2, 16, 0), C_STR(ESC "M" ESC "M"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (3, 2, 79, 2), C_STR(CSI "80G"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (5, 5, 75, 20), C_STR(CSI "21;76H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (39, 0, 32, 0), C_STR(CSI "Z"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (10, 0, 8, 0), C_STR("\r\t"));

  // xold is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (99, 10, 79, 10), C_STR(CSI "11;80H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (-3, 33, 50, 10), C_STR(CSI "11;51H"));

  // ynew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (23, 33, 23, 10), C_STR(CSI "11;24H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (23, -3, 12, 10), C_STR(CSI "11;13H"));

  // xnew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 22, 100, 22), C_STR(CSI "80G"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (3, 22, -5, 22), C_STR("\r"));

  // ynew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 23, 53, 40), C_STR(ESC "D"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 2, 53, -3), C_STR(ESC "M" ESC "M"));
}

//----------------------------------------------------------------------
void FOptiMoveTest::teratermTest()
{
  finalcut::FOptiMove om;
  om.setTermSize (80, 25);
  om.setBaudRate (38400);

  finalcut::FOptiMove::termEnv optimove_env =
  {
    false,                        // Automatic left margin
    true,                         // Eat newline glitch
    8,                            // Tab stop
    C_STR(CSI "H"),               // Cursor home
    C_STR("\r"),                  // Carriage return
    0,                            // Cursor to ll
    C_STR("\t"),                  // Tabular
    0,                            // Back tabular
    C_STR(CSI "A"),               // Cursor up
    C_STR("\n"),                  // Cursor down
    C_STR("\b"),                  // Cursor left
    C_STR(CSI "C"),               // Cursor right
    C_STR(CSI "%i%p1%d;%p2%dH"),  // Cursor address
    C_STR(CSI "%i%p1%dG"),        // Column address
    C_STR(CSI "%i%p1%dd"),        // Row address
    C_STR(CSI "%p1%dA"),          // Parm up cursor
    C_STR(CSI "%p1%dB"),          // Parm down cursor
    C_STR(CSI "%p1%dD"),          // Parm left cursor
    C_STR(CSI "%p1%dC"),          // Parm right cursor
    C_STR(CSI "%p1%dX"),          // Erase characters
    0,                            // Repeat character
    C_STR(CSI "1K"),              // Clear to beginning of line
    C_STR(CSI "K")                // Clear to end of line
  };

  om.setTermEnvironment(optimove_env);

  CPPUNIT_ASSERT_CSTRING (om.moveCursor (0, 0, 5, 5), C_STR(CSI "6;6H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (5, 5, 0, 0), C_STR(CSI "H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (79, 1, 0, 1), C_STR("\r"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (79, 1, 0, 2), C_STR("\r\n"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (9, 4, 10, 4), C_STR(CSI "C"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (10, 4, 9, 4), C_STR("\b"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (9, 4, 11, 4), C_STR(CSI "12G"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (11, 4, 9, 4), C_STR("\b\b"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (1, 0, 8, 0), C_STR("\t"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 0, 16, 1), C_STR("\n"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 1, 16, 0), C_STR(CSI "A"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 0, 16, 2), C_STR("\n\n"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 2, 16, 0), C_STR(CSI "1d"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (3, 2, 79, 2), C_STR(CSI "80G"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (5, 5, 75, 20), C_STR(CSI "21;76H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (39, 0, 32, 0), C_STR(CSI "33G"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (10, 0, 8, 0), C_STR("\b\b"));

  // xold is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (99, 10, 79, 10), C_STR(CSI "11;80H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (-3, 33, 50, 10), C_STR(CSI "11;51H"));

  // ynew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (23, 33, 23, 10), C_STR(CSI "11;24H"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (23, -3, 12, 10), C_STR(CSI "11;13H"));

  // xnew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 22, 100, 22), C_STR(CSI "80G"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (3, 22, -5, 22), C_STR("\r"));

  // ynew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 23, 53, 40), C_STR("\n"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 2, 53, -3), C_STR(CSI "1d"));
}


//----------------------------------------------------------------------
void FOptiMoveTest::wyse50Test()
{
  finalcut::FOptiMove om;
  om.setTermSize (80, 25);
  om.setBaudRate (38400);
  om.set_auto_left_margin (true);
  om.set_tabular (C_STR("\t"));
  om.set_back_tab (C_STR(ESC "I"));
  om.set_cursor_home (C_STR("\036"));
  om.set_cursor_to_ll (C_STR("\036\v"));
  om.set_carriage_return (C_STR("\r"));
  om.set_cursor_up (C_STR("\v"));
  om.set_cursor_down (C_STR("\n"));
  om.set_cursor_right (C_STR("\f"));
  om.set_cursor_left (C_STR("\b"));
  om.set_cursor_address (C_STR(ESC "=%p1%' '%+%c%p2%' '%+%c"));

  //std::cout << "\nSequence: "
  //          << printSequence(om.moveCursor (1, 2, 3, 4))
  //          << "\n";

  CPPUNIT_ASSERT_CSTRING (om.moveCursor (0, 0, 5, 5), C_STR(ESC "=%%"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (5, 5, 0, 0), C_STR("\036"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (79, 1, 0, 1), C_STR("\r"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (79, 1, 0, 2), C_STR("\r\n"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (9, 4, 10, 4), C_STR("\f"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (10, 4, 9, 4), C_STR("\b"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (9, 4, 11, 4), C_STR("\f\f"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (11, 4, 9, 4), C_STR("\b\b"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (1, 0, 8, 0), C_STR(ESC "= ("));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 0, 16, 1), C_STR("\n"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 1, 16, 0), C_STR("\v"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 0, 16, 2), C_STR("\n\n"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (16, 2, 16, 0), C_STR("\v\v"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (3, 2, 79, 2), C_STR("\r\b\n"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (5, 5, 75, 20), C_STR(ESC "=4k"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (39, 0, 32, 0), C_STR(ESC "= @"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (10, 0, 8, 0), C_STR("\b\b"));

  // xold is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (99, 10, 79, 10), C_STR(ESC "=*o"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (-3, 33, 50, 10), C_STR(ESC "=*R"));

  // ynew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (23, 33, 23, 10), C_STR(ESC "=*7"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (23, -3, 12, 10), C_STR(ESC "=*,"));

  // xnew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 22, 100, 22), C_STR("\r\b\n"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (3, 22, -5, 22), C_STR("\r"));

  // ynew is outside screen
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 23, 53, 40), C_STR("\n"));
  CPPUNIT_ASSERT_CSTRING (om.moveCursor (53, 2, 53, -3), C_STR("\v\v"));
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
