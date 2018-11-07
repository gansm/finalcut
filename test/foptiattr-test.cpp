/***********************************************************************
* foptiattr_test.cpp - FOptiAttr unit tests                            *
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
// class FOptiAttrTest
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FOptiAttrTest : public CPPUNIT_NS::TestFixture
{
  public:
    FOptiAttrTest()
    { }

  protected:
    void classNameTest();
    void noArgumentTest();
    void vga2ansiTest();
    void fakeReverseTest();
    void ansiTest();
    void vt100Test();
    void xtermTest();
    void rxvtTest();
    void linuxTest();
    void cygwinTest();
    void puttyTest();
    void teratermTest();
    void ibmColorTest();
    void wyse50Test();

  private:
    std::string printSequence (const std::string&);

    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FOptiAttrTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (noArgumentTest);
    CPPUNIT_TEST (vga2ansiTest);
    CPPUNIT_TEST (fakeReverseTest);
    CPPUNIT_TEST (ansiTest);
    CPPUNIT_TEST (vt100Test);
    CPPUNIT_TEST (xtermTest);
    CPPUNIT_TEST (rxvtTest);
    CPPUNIT_TEST (linuxTest);
    CPPUNIT_TEST (cygwinTest);
    CPPUNIT_TEST (puttyTest);
    CPPUNIT_TEST (teratermTest);
    CPPUNIT_TEST (ibmColorTest);
    CPPUNIT_TEST (wyse50Test);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};
#pragma pack(pop)


//----------------------------------------------------------------------
void FOptiAttrTest::classNameTest()
{
  finalcut::FOptiAttr opti_attr;
  const char* const classname = opti_attr.getClassName();
  CPPUNIT_ASSERT_CSTRING ( classname, "FOptiAttr");
}

//----------------------------------------------------------------------
void FOptiAttrTest::noArgumentTest()
{
  finalcut::FOptiAttr::charData* ch = new finalcut::FOptiAttr::charData();
  finalcut::FOptiAttr oa;
  oa.initialize();

  // isNormal test
  CPPUNIT_ASSERT ( ! oa.isNormal(ch) );
  ch->fg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( ! oa.isNormal(ch) );
  ch->bg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( oa.isNormal(ch) );

  // Null test
  finalcut::FOptiAttr::charData* ch_null = 0;
  CPPUNIT_ASSERT ( oa.changeAttribute(ch, ch) == 0 );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(ch, ch_null), C_STR("") );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(ch_null, ch), C_STR("") );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(ch_null, ch_null), C_STR("") );
  delete ch;
}

//----------------------------------------------------------------------
void FOptiAttrTest::vga2ansiTest()
{
  finalcut::FOptiAttr oa;
  CPPUNIT_ASSERT (oa.vga2ansi(0) == 0);
  CPPUNIT_ASSERT (oa.vga2ansi(1) == 4);
  CPPUNIT_ASSERT (oa.vga2ansi(2) == 2);
  CPPUNIT_ASSERT (oa.vga2ansi(3) == 6);
  CPPUNIT_ASSERT (oa.vga2ansi(4) == 1);
  CPPUNIT_ASSERT (oa.vga2ansi(5) == 5);
  CPPUNIT_ASSERT (oa.vga2ansi(6) == 3);
  CPPUNIT_ASSERT (oa.vga2ansi(7) == 7);
  CPPUNIT_ASSERT (oa.vga2ansi(8) == 8);
  CPPUNIT_ASSERT (oa.vga2ansi(9) == 12);
  CPPUNIT_ASSERT (oa.vga2ansi(10) == 10);
  CPPUNIT_ASSERT (oa.vga2ansi(11) == 14);
  CPPUNIT_ASSERT (oa.vga2ansi(12) == 9);
  CPPUNIT_ASSERT (oa.vga2ansi(13) == 13);
  CPPUNIT_ASSERT (oa.vga2ansi(14) == 11);
  CPPUNIT_ASSERT (oa.vga2ansi(15) == 15);
}

//----------------------------------------------------------------------
void FOptiAttrTest::fakeReverseTest()
{
  finalcut::FOptiAttr oa;
  oa.setDefaultColorSupport();  // ANSI default color
  oa.setMaxColor (8);
  oa.setNoColorVideo (4);       // Avoid reverse (4)
  oa.set_enter_bold_mode (0);
  oa.set_exit_bold_mode (0);
  oa.set_enter_dim_mode (0);
  oa.set_exit_dim_mode (0);
  oa.set_enter_italics_mode (0);
  oa.set_exit_italics_mode (0);
  oa.set_enter_underline_mode (0);
  oa.set_exit_underline_mode (0);
  oa.set_enter_blink_mode (0);
  oa.set_exit_blink_mode (0);
  oa.set_enter_reverse_mode (0);
  oa.set_exit_reverse_mode (0);
  oa.set_enter_standout_mode (0);
  oa.set_exit_standout_mode (0);
  oa.set_enter_secure_mode (0);
  oa.set_exit_secure_mode (0);
  oa.set_enter_protected_mode (0);
  oa.set_exit_protected_mode (0);
  oa.set_enter_crossed_out_mode (0);
  oa.set_exit_crossed_out_mode (0);
  oa.set_enter_dbl_underline_mode (0);
  oa.set_exit_dbl_underline_mode (0);
  oa.set_set_attributes (0);
  oa.set_exit_attribute_mode (0);
  oa.set_enter_alt_charset_mode (0);
  oa.set_exit_alt_charset_mode (0);
  oa.set_enter_pc_charset_mode (0);
  oa.set_exit_pc_charset_mode (0);
  oa.set_a_foreground_color (C_STR(CSI "3%p1%dm"));
  oa.set_a_background_color (C_STR(CSI "4%p1%dm"));
  oa.set_foreground_color (0);
  oa.set_background_color (0);
  oa.set_term_color_pair (0);
  oa.set_orig_pair (0);
  oa.set_orig_orig_colors (0);
  oa.initialize();

  finalcut::FOptiAttr::charData* from = \
      new finalcut::FOptiAttr::charData();
  finalcut::FOptiAttr::charData* to = \
      new finalcut::FOptiAttr::charData();
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Gray text on blue background
  to->fg_color = finalcut::fc::LightGray;
  to->bg_color = finalcut::fc::Blue;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "37m" CSI "44m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse on
  to->attr.bit.reverse = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "34m" CSI "47m") );
  CPPUNIT_ASSERT ( from->fg_color == finalcut::fc::LightGray );
  CPPUNIT_ASSERT ( from->bg_color == finalcut::fc::Blue );
  CPPUNIT_ASSERT ( *from == *to );

  // Gray text on red background
  to->bg_color = finalcut::fc::Red;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "31m" CSI "47m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off
  to->attr.bit.reverse = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "37m" CSI "41m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  delete to;
  delete from;
}

//----------------------------------------------------------------------
void FOptiAttrTest::ansiTest()
{
  // Simulate an ansi terminal

  finalcut::FOptiAttr oa;
  oa.setDefaultColorSupport();  // ANSI default color
  oa.setMaxColor (8);
  oa.setNoColorVideo (3);       // Avoid standout (1) + underline mode (2)
  oa.set_enter_bold_mode (C_STR(CSI "1m"));
  oa.set_exit_bold_mode (C_STR(CSI "0m"));
  oa.set_enter_dim_mode (0);
  oa.set_exit_dim_mode (C_STR(CSI "0m"));
  oa.set_enter_italics_mode (0);
  oa.set_exit_italics_mode (0);
  oa.set_enter_underline_mode (C_STR(CSI "4m"));
  oa.set_exit_underline_mode (C_STR(CSI "1m"));
  oa.set_enter_blink_mode (C_STR(CSI "5m"));
  oa.set_exit_blink_mode (C_STR(CSI "0m"));
  oa.set_enter_reverse_mode (C_STR(CSI "7m"));
  oa.set_exit_reverse_mode (C_STR(CSI "0m"));
  oa.set_enter_standout_mode (C_STR(CSI "7m"));
  oa.set_exit_standout_mode (C_STR(CSI "m"));
  oa.set_enter_secure_mode (C_STR(CSI "8m"));
  oa.set_exit_secure_mode (C_STR(CSI "0m"));
  oa.set_enter_protected_mode (0);
  oa.set_exit_protected_mode (C_STR(CSI "0m"));
  oa.set_enter_crossed_out_mode (0);
  oa.set_exit_crossed_out_mode (C_STR(CSI "0m"));
  oa.set_enter_dbl_underline_mode (0);
  oa.set_exit_dbl_underline_mode (0);
  oa.set_set_attributes (C_STR(CSI "0;10"
                               "%?%p1%t;7%;"
                               "%?%p2%t;4%;"
                               "%?%p3%t;7%;"
                               "%?%p4%t;5%;"
                               "%?%p6%t;1%;"
                               "%?%p7%t;8%;"
                               "%?%p9%t;11%;m"));
  oa.set_exit_attribute_mode (C_STR(CSI "0m"));
  oa.set_enter_alt_charset_mode (C_STR(CSI "11m"));
  oa.set_exit_alt_charset_mode (C_STR(CSI "10m"));
  oa.set_enter_pc_charset_mode (C_STR(CSI "11m"));
  oa.set_exit_pc_charset_mode (C_STR(CSI "10m"));
  oa.set_a_foreground_color (C_STR(CSI "3%p1%dm"));
  oa.set_a_background_color (C_STR(CSI "4%p1%dm"));
  oa.set_foreground_color (0);
  oa.set_background_color (0);
  oa.set_term_color_pair (0);
  oa.set_orig_pair (C_STR(CSI "39;49m"));
  oa.set_orig_orig_colors (0);
  oa.initialize();

  finalcut::FOptiAttr::charData* from = new finalcut::FOptiAttr::charData();
  finalcut::FOptiAttr::charData* to = new finalcut::FOptiAttr::charData();
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default color + bold
  from->fg_color = finalcut::fc::Default;
  from->bg_color = finalcut::fc::Default;
  to->attr.bit.bold = true;
  to->fg_color = finalcut::fc::Default;
  to->bg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10;1m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blue text on white background + dim + italic
  to->fg_color = finalcut::fc::Blue;
  to->bg_color = finalcut::fc::White;
  to->attr.bit.dim = true;
  to->attr.bit.italic = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10;1m" CSI "34m" CSI "47m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reset attributes + default background
  to->attr.bit.bold = false;
  to->attr.bit.dim = false;
  to->attr.bit.italic = false;
  to->bg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m" CSI "34m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Red text on black background
  to->fg_color = finalcut::fc::Red;
  to->bg_color = finalcut::fc::Black;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "31m" CSI "40m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // 256 color text and background
  to->fg_color = finalcut::fc::SpringGreen3;
  to->bg_color = finalcut::fc::NavyBlue;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "32m" CSI "44m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold on (with default colors)
  to->fg_color = finalcut::fc::Default;
  to->bg_color = finalcut::fc::Default;
  to->attr.bit.bold = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10;1m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off (with default colors)
  to->attr.bit.bold = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim on (with default colors)
  to->attr.bit.dim = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off (with default colors)
  to->attr.bit.dim = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic on (with default colors)
  to->attr.bit.italic = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off (with default colors)
  to->attr.bit.italic = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline on (with default colors)
  to->attr.bit.underline = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10;4m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off (with default colors)
  to->attr.bit.underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink on (with default colors)
  to->attr.bit.blink = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10;5m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off (with default colors)
  to->attr.bit.blink = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse on (with default colors)
  to->attr.bit.reverse = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10;7m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off (with default colors)
  to->attr.bit.reverse = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout on (with default colors)
  to->attr.bit.standout = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10;7m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off (with default colors)
  to->attr.bit.standout = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible on (with default colors)
  to->attr.bit.invisible = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10;8m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off (with default colors)
  to->attr.bit.invisible = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect on (with default colors)
  to->attr.bit.protect = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off (with default colors)
  to->attr.bit.protect = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out on (with default colors)
  to->attr.bit.crossed_out = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off (with default colors)
  to->attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline on (with default colors)
  to->attr.bit.dbl_underline = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off (with default colors)
  to->attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set on (with default colors)
  to->attr.bit.alt_charset = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10;11m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off (with default colors)
  to->attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "10m" CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set on (with default colors)
  to->attr.bit.pc_charset = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10m" CSI "11m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off (with default colors)
  to->attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m" CSI "10m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Turn on all attributes (with default colors)
  to->attr.bit.pc_charset    = true;
  to->attr.bit.bold          = true;
  to->attr.bit.dim           = true;
  to->attr.bit.italic        = true;
  to->attr.bit.underline     = true;
  to->attr.bit.blink         = true;
  to->attr.bit.reverse       = true;
  to->attr.bit.standout      = true;
  to->attr.bit.invisible     = true;
  to->attr.bit.protect       = true;
  to->attr.bit.crossed_out   = true;
  to->attr.bit.dbl_underline = true;
  to->attr.bit.alt_charset   = true;
  to->attr.bit.pc_charset    = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10;7;4;7;5;1;8;11m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Cyan text on blue background
  to->fg_color = finalcut::fc::Cyan;
  to->bg_color = finalcut::fc::Blue;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "36m" CSI "44m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off
  to->attr.bit.bold = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10;7;5;8;11m" CSI "36m" CSI "44m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off
  to->attr.bit.dim = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10;7;5;8;11m" CSI "36m" CSI "44m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off
  to->attr.bit.italic = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10;7;5;8;11m"
                                 CSI "36m" CSI "44m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off
  to->attr.bit.underline = false;
  CPPUNIT_ASSERT ( *from == *to );  // because of noColorVideo = 3
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off
  to->attr.bit.blink = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10;7;8;11m" CSI "36m" CSI "44m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off
  to->attr.bit.reverse = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10;8;11m" CSI "36m" CSI "44m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off
  to->attr.bit.standout = false;
  CPPUNIT_ASSERT ( *from == *to );  // because of noColorVideo = 3
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off
  to->attr.bit.invisible = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10;11m" CSI "36m" CSI "44m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off
  to->attr.bit.protect = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10;11m" CSI "36m" CSI "44m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off
  to->attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10;11m" CSI "36m" CSI "44m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off
  to->attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10;11m" CSI "36m" CSI "44m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off
  to->attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10m" CSI "11m"  CSI "36m" CSI "44m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off
  to->attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m" CSI "10m" CSI "36m" CSI "44m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Green text color
  to->fg_color = finalcut::fc::Green;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR(CSI "32m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default text color
  to->fg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( printSequence(oa.changeAttribute(from, to)).c_str()
                         , C_STR("Esc [ 3 9 m ") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  delete to;
  delete from;
}

//----------------------------------------------------------------------
void FOptiAttrTest::vt100Test()
{
  // Simulate a vt100 terminal

  finalcut::FOptiAttr oa;
  oa.unsetDefaultColorSupport();  // No ANSI default color
  oa.setMaxColor (1);
  oa.setNoColorVideo (0);
  oa.set_enter_bold_mode (C_STR(CSI "1m$<2>"));
  oa.set_exit_bold_mode (C_STR(CSI "0m$<2>"));
  oa.set_enter_dim_mode (0);
  oa.set_exit_dim_mode (C_STR(CSI "0m$<2>"));
  oa.set_enter_italics_mode (0);
  oa.set_exit_italics_mode (0);
  oa.set_enter_underline_mode (C_STR(CSI "4m$<2>"));
  oa.set_exit_underline_mode (C_STR(CSI "m$<2>"));
  oa.set_enter_blink_mode (C_STR(CSI "5m$<2>"));
  oa.set_exit_blink_mode (C_STR(CSI "0m$<2>"));
  oa.set_enter_reverse_mode (C_STR(CSI "7m$<2>"));
  oa.set_exit_reverse_mode (C_STR(CSI "0m$<2>"));
  oa.set_enter_standout_mode (C_STR(CSI "7m$<2>"));
  oa.set_exit_standout_mode (C_STR(CSI "m$<2>"));
  oa.set_enter_secure_mode (0);
  oa.set_exit_secure_mode (C_STR(CSI "0m$<2>"));
  oa.set_enter_protected_mode (0);
  oa.set_exit_protected_mode (C_STR(CSI "0m$<2>"));
  oa.set_enter_crossed_out_mode (0);
  oa.set_exit_crossed_out_mode (C_STR(CSI "0m$<2>"));
  oa.set_enter_dbl_underline_mode (0);
  oa.set_exit_dbl_underline_mode (0);
  oa.set_set_attributes (C_STR(CSI "0"
                               "%?%p1%p6%|%t;1%;"
                               "%?%p2%t;4%;"
                               "%?%p1%p3%|%t;7%;"
                               "%?%p4%t;5%;m"
                               "%?%p9%t\016%e\017%;$<2>"));
  oa.set_exit_attribute_mode (C_STR(CSI "0m$<2>"));
  oa.set_enter_alt_charset_mode (C_STR("\016"));
  oa.set_exit_alt_charset_mode (C_STR("\017"));
  oa.set_enter_pc_charset_mode (0);
  oa.set_exit_pc_charset_mode (0);
  oa.set_a_foreground_color (C_STR(CSI "3%p1%dm"));
  oa.set_a_background_color (C_STR(CSI "4%p1%dm"));
  oa.set_foreground_color (0);
  oa.set_background_color (0);
  oa.set_term_color_pair (0);
  oa.set_orig_pair (0);
  oa.set_orig_orig_colors (0);
  oa.initialize();

  finalcut::FOptiAttr::charData* from = new finalcut::FOptiAttr::charData();
  finalcut::FOptiAttr::charData* to = new finalcut::FOptiAttr::charData();
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default color + bold
  from->fg_color = finalcut::fc::Default;
  from->bg_color = finalcut::fc::Default;
  to->attr.bit.bold = true;
  to->fg_color = finalcut::fc::Default;
  to->bg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;1m\017$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blue text on white background + dim + italic
  to->fg_color = finalcut::fc::Blue;
  to->bg_color = finalcut::fc::White;
  to->attr.bit.dim = true;
  to->attr.bit.italic = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;1m\017$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reset attributes + default background
  to->attr.bit.bold = false;
  to->attr.bit.dim = false;
  to->attr.bit.italic = false;
  to->bg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>" ) );

  // Red text on black background
  to->fg_color = finalcut::fc::Red;
  to->bg_color = finalcut::fc::Black;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR("") );

  // 256 color text and background
  to->fg_color = finalcut::fc::SpringGreen3;
  to->bg_color = finalcut::fc::NavyBlue;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );


  // Bold on (with default colors)
  to->fg_color = finalcut::fc::Default;
  to->bg_color = finalcut::fc::Default;
  to->attr.bit.bold = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;1m\017$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off (with default colors)
  to->attr.bit.bold = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim on (with default colors)
  to->attr.bit.dim = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off (with default colors)
  to->attr.bit.dim = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic on (with default colors)
  to->attr.bit.italic = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off (with default colors)
  to->attr.bit.italic = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline on (with default colors)
  to->attr.bit.underline = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;4m\017$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off (with default colors)
  to->attr.bit.underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink on (with default colors)
  to->attr.bit.blink = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;5m\017$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off (with default colors)
  to->attr.bit.blink = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse on (with default colors)
  to->attr.bit.reverse = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;7m\017$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off (with default colors)
  to->attr.bit.reverse = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout on (with default colors)
  to->attr.bit.standout = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;1;7m\017$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off (with default colors)
  to->attr.bit.standout = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible on (with default colors)
  to->attr.bit.invisible = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017$<2>") );
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT ( to->code == ' ' );
  from->code = ' ';
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off (with default colors)
  to->attr.bit.invisible = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect on (with default colors)
  to->attr.bit.protect = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off (with default colors)
  to->attr.bit.protect = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out on (with default colors)
  to->attr.bit.crossed_out = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off (with default colors)
  to->attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline on (with default colors)
  to->attr.bit.dbl_underline = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off (with default colors)
  to->attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set on (with default colors)
  to->attr.bit.alt_charset = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\016$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off (with default colors)
  to->attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR("\017" CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set on (with default colors)
  to->attr.bit.pc_charset = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off (with default colors)
  to->attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Turn on all attributes (with default colors)
  to->attr.bit.pc_charset    = true;
  to->attr.bit.bold          = true;
  to->attr.bit.dim           = true;
  to->attr.bit.italic        = true;
  to->attr.bit.underline     = true;
  to->attr.bit.blink         = true;
  to->attr.bit.reverse       = true;
  to->attr.bit.standout      = true;
  to->attr.bit.invisible     = true;
  to->attr.bit.protect       = true;
  to->attr.bit.crossed_out   = true;
  to->attr.bit.dbl_underline = true;
  to->attr.bit.alt_charset   = true;
  to->attr.bit.pc_charset    = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;1;4;7;5m\016$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Cyan text on blue background
  to->fg_color = finalcut::fc::Cyan;
  to->bg_color = finalcut::fc::Blue;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );

  // Bold off
  to->attr.bit.bold = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>\016" CSI "4m$<2>"
                                 CSI "5m$<2>" CSI "7m$<2>" CSI "7m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off
  to->attr.bit.dim = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>\016" CSI "4m$<2>"
                                 CSI "5m$<2>" CSI "7m$<2>" CSI "7m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off
  to->attr.bit.italic = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off
  to->attr.bit.underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "m$<2>\016" CSI "5m$<2>"
                                 CSI "7m$<2>" CSI "7m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off
  to->attr.bit.blink = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>\016" CSI "7m$<2>"
                                 CSI "7m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off
  to->attr.bit.reverse = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>\016" CSI "7m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off
  to->attr.bit.standout = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "m$<2>\016") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off
  to->attr.bit.invisible = false;
  CPPUNIT_ASSERT ( *from != *to );

  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>\016") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off
  to->attr.bit.protect = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>\016") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off
  to->attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>\016") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off
  to->attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off
  to->attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off
  to->attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Green text color
  to->fg_color = finalcut::fc::Green;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default text color
  to->fg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  delete to;
  delete from;
}

//----------------------------------------------------------------------
void FOptiAttrTest::xtermTest()
{
  // Simulate an xterm-256color terminal

  finalcut::FOptiAttr oa;
  oa.setDefaultColorSupport();  // ANSI default color
  oa.setMaxColor (256);
  oa.setNoColorVideo (0);
  oa.set_enter_bold_mode (C_STR(CSI "1m"));
  oa.set_exit_bold_mode (C_STR(CSI "22m"));
  oa.set_enter_dim_mode (C_STR(CSI "2m"));
  oa.set_exit_dim_mode (C_STR(CSI "22m"));
  oa.set_enter_italics_mode (C_STR(CSI "3m"));
  oa.set_exit_italics_mode (C_STR(CSI "23m"));
  oa.set_enter_underline_mode (C_STR(CSI "4m"));
  oa.set_exit_underline_mode (C_STR(CSI "24m"));
  oa.set_enter_blink_mode (C_STR(CSI "5m"));
  oa.set_exit_blink_mode (C_STR(CSI "25m"));
  oa.set_enter_reverse_mode (C_STR(CSI "7m"));
  oa.set_exit_reverse_mode (C_STR(CSI "27m"));
  oa.set_enter_standout_mode (C_STR(CSI "7m"));
  oa.set_exit_standout_mode (C_STR(CSI "27m"));
  oa.set_enter_secure_mode (C_STR(CSI "8m"));
  oa.set_exit_secure_mode (C_STR(CSI "28m"));
  oa.set_enter_protected_mode (0);
  oa.set_exit_protected_mode (C_STR(CSI "0m"));
  oa.set_enter_crossed_out_mode (C_STR(CSI "9m"));
  oa.set_exit_crossed_out_mode (C_STR(CSI "29m"));
  oa.set_enter_dbl_underline_mode (C_STR(CSI "21m"));
  oa.set_exit_dbl_underline_mode (C_STR(CSI "24m"));
  oa.set_set_attributes (C_STR("%?%p9%t" ESC "(0"
                                    "%e" ESC "(B%;" CSI "0"
                               "%?%p6%t;1%;"
                               "%?%p5%t;2%;"
                               "%?%p2%t;4%;"
                               "%?%p1%p3%|%t;7%;"
                               "%?%p4%t;5%;"
                               "%?%p7%t;8%;m"));
  oa.set_exit_attribute_mode (C_STR(CSI "0m"));
  oa.set_enter_alt_charset_mode (C_STR(ESC "(0"));
  oa.set_exit_alt_charset_mode (C_STR(ESC "(B"));
  oa.set_enter_pc_charset_mode (0);
  oa.set_exit_pc_charset_mode (0);
  oa.set_a_foreground_color (C_STR(CSI "%?%p1%{8}%<"
                                       "%t3%p1%d"
                                       "%e%p1%{16}%<"
                                       "%t9%p1%{8}%-%d"
                                       "%e38;5;%p1%d%;m"));
  oa.set_a_background_color (C_STR(CSI "%?%p1%{8}%<"
                                       "%t4%p1%d"
                                       "%e%p1%{16}%<"
                                       "%t10%p1%{8}%-%d"
                                       "%e48;5;%p1%d%;m"));
  oa.set_foreground_color (0);
  oa.set_background_color (0);
  oa.set_term_color_pair (0);
  oa.set_orig_pair (C_STR(CSI "39;49m"));
  oa.set_orig_orig_colors (0);
  oa.initialize();

  finalcut::FOptiAttr::charData* from = new finalcut::FOptiAttr::charData();
  finalcut::FOptiAttr::charData* to = new finalcut::FOptiAttr::charData();
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default color + bold
  from->fg_color = finalcut::fc::Default;
  from->bg_color = finalcut::fc::Default;
  to->attr.bit.bold = true;
  to->fg_color = finalcut::fc::Default;
  to->bg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(B" CSI "0;1m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blue text on white background + dim + italic
  to->fg_color = finalcut::fc::Blue;
  to->bg_color = finalcut::fc::White;
  to->attr.bit.dim = true;
  to->attr.bit.italic = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(B" CSI "0;1;2m" CSI "3m"
                                 CSI "34m" CSI "107m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reset attributes + default background
  to->attr.bit.bold = false;
  to->attr.bit.dim = false;
  to->attr.bit.italic = false;
  to->bg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m" CSI "34m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Red text on black background
  to->fg_color = finalcut::fc::Red;
  to->bg_color = finalcut::fc::Black;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "31m" CSI "40m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // 256 color text and background
  to->fg_color = finalcut::fc::SpringGreen3;
  to->bg_color = finalcut::fc::NavyBlue;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "38;5;42m" CSI "48;5;17m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold on (with default colors)
  to->fg_color = finalcut::fc::Default;
  to->bg_color = finalcut::fc::Default;
  to->attr.bit.bold = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(B" CSI "0;1m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off (with default colors)
  to->attr.bit.bold = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim on (with default colors)
  to->attr.bit.dim = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(B" CSI "0;2m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off (with default colors)
  to->attr.bit.dim = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic on (with default colors)
  to->attr.bit.italic = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(B" CSI "0m" CSI "3m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off (with default colors)
  to->attr.bit.italic = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline on (with default colors)
  to->attr.bit.underline = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(B" CSI "0;4m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off (with default colors)
  to->attr.bit.underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink on (with default colors)
  to->attr.bit.blink = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(B" CSI "0;5m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off (with default colors)
  to->attr.bit.blink = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse on (with default colors)
  to->attr.bit.reverse = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(B" CSI "0;7m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off (with default colors)
  to->attr.bit.reverse = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout on (with default colors)
  to->attr.bit.standout = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(B" CSI "0;7m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off (with default colors)
  to->attr.bit.standout = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible on (with default colors)
  to->attr.bit.invisible = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(B" CSI "0;8m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off (with default colors)
  to->attr.bit.invisible = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect on (with default colors)
  to->attr.bit.protect = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(B" CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off (with default colors)
  to->attr.bit.protect = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out on (with default colors)
  to->attr.bit.crossed_out = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(B" CSI "0m" CSI "9m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off (with default colors)
  to->attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline on (with default colors)
  to->attr.bit.dbl_underline = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(B" CSI "0m" CSI "21m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off (with default colors)
  to->attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set on (with default colors)
  to->attr.bit.alt_charset = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(0" CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off (with default colors)
  to->attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(B" CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set on (with default colors)
  to->attr.bit.pc_charset = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(B" CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off (with default colors)
  to->attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Turn on all attributes (with default colors)
  to->attr.bit.pc_charset    = true;
  to->attr.bit.bold          = true;
  to->attr.bit.dim           = true;
  to->attr.bit.italic        = true;
  to->attr.bit.underline     = true;
  to->attr.bit.blink         = true;
  to->attr.bit.reverse       = true;
  to->attr.bit.standout      = true;
  to->attr.bit.invisible     = true;
  to->attr.bit.protect       = true;
  to->attr.bit.crossed_out   = true;
  to->attr.bit.dbl_underline = true;
  to->attr.bit.alt_charset   = true;
  to->attr.bit.pc_charset    = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(0" CSI "0;1;2;4;7;5;8m" CSI "3m"
                                 CSI "9m" CSI "21m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Cyan text on blue background
  to->fg_color = finalcut::fc::Cyan;
  to->bg_color = finalcut::fc::Blue;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "36m" CSI "44m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off
  to->attr.bit.bold = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "22m" CSI "2m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off
  to->attr.bit.dim = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "22m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off
  to->attr.bit.italic = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "23m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off
  to->attr.bit.underline = false;
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "24m" CSI "21m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );


  // Blink off
  to->attr.bit.blink = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "25m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off
  to->attr.bit.reverse = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "27m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off
  to->attr.bit.standout = false;
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "27m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off
  to->attr.bit.invisible = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "28m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off
  to->attr.bit.protect = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m" CSI "36m" CSI "44m" ESC "(0" CSI "9m" CSI "21m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off
  to->attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "29m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off
  to->attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "24m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off
  to->attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(B") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off
  to->attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m" CSI "36m" CSI "44m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Green text color
  to->fg_color = finalcut::fc::Green;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR(CSI "32m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default text color
  to->fg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( printSequence(oa.changeAttribute(from, to)).c_str()
                         , C_STR("Esc [ 3 9 m ") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  delete to;
  delete from;
}

//----------------------------------------------------------------------
void FOptiAttrTest::rxvtTest()
{
  // Simulate an rxvt terminal

  finalcut::FOptiAttr oa;
  oa.setDefaultColorSupport();  // ANSI default color
  oa.setMaxColor (8);
  oa.setNoColorVideo (0);
  oa.set_enter_bold_mode (C_STR(CSI "1m"));
  oa.set_exit_bold_mode (C_STR(CSI "22m"));
  oa.set_enter_dim_mode (0);
  oa.set_exit_dim_mode (C_STR(CSI "22m"));
  oa.set_enter_italics_mode (0);
  oa.set_exit_italics_mode (0);
  oa.set_enter_underline_mode (C_STR(CSI "4m"));
  oa.set_exit_underline_mode (C_STR(CSI "24m"));
  oa.set_enter_blink_mode (C_STR(CSI "5m"));
  oa.set_exit_blink_mode (C_STR(CSI "25m"));
  oa.set_enter_reverse_mode (C_STR(CSI "7m"));
  oa.set_exit_reverse_mode (C_STR(CSI "27m"));
  oa.set_enter_standout_mode (C_STR(CSI "7m"));
  oa.set_exit_standout_mode (C_STR(CSI "27m"));
  oa.set_enter_secure_mode (0);
  oa.set_exit_secure_mode (C_STR(CSI "28m"));
  oa.set_enter_protected_mode (0);
  oa.set_exit_protected_mode (C_STR(CSI "0m"));
  oa.set_enter_crossed_out_mode (C_STR(CSI "9m"));
  oa.set_exit_crossed_out_mode (C_STR(CSI "29m"));
  oa.set_enter_dbl_underline_mode (C_STR(CSI "21m"));
  oa.set_exit_dbl_underline_mode (C_STR(CSI "24m"));
  oa.set_set_attributes (C_STR(CSI "0"
                               "%?%p6%t;1%;"
                               "%?%p2%t;4%;"
                               "%?%p1%p3%|%t;7%;"
                               "%?%p4%t;5%;m"
                               "%?%p9%t\016%e\017%;"));
  oa.set_exit_attribute_mode (C_STR(CSI "0m"));
  oa.set_enter_alt_charset_mode (C_STR("\016"));
  oa.set_exit_alt_charset_mode (C_STR("\017"));
  oa.set_enter_pc_charset_mode (0);
  oa.set_exit_pc_charset_mode (0);
  oa.set_a_foreground_color (C_STR(CSI "3%p1%dm"));
  oa.set_a_background_color (C_STR(CSI "4%p1%dm"));
  oa.set_foreground_color (0);
  oa.set_background_color (0);
  oa.set_term_color_pair (0);
  oa.set_orig_pair (C_STR(CSI "39;49m"));
  oa.set_orig_orig_colors (0);
  oa.initialize();

  finalcut::FOptiAttr::charData* from = new finalcut::FOptiAttr::charData();
  finalcut::FOptiAttr::charData* to = new finalcut::FOptiAttr::charData();
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default color + bold
  from->fg_color = finalcut::fc::Default;
  from->bg_color = finalcut::fc::Default;
  to->attr.bit.bold = true;
  to->fg_color = finalcut::fc::Default;
  to->bg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;1m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blue text on white background + dim + italic
  to->fg_color = finalcut::fc::Blue;
  to->bg_color = finalcut::fc::White;
  to->attr.bit.dim = true;
  to->attr.bit.italic = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;1m\017" CSI "34m" CSI "47m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reset attributes + default background
  to->attr.bit.bold = false;
  to->attr.bit.dim = false;
  to->attr.bit.italic = false;
  to->bg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m" CSI "34m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Red text on black background
  to->fg_color = finalcut::fc::Red;
  to->bg_color = finalcut::fc::Black;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "31m" CSI "40m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // 256 color text and background
  to->fg_color = finalcut::fc::SpringGreen3;
  to->bg_color = finalcut::fc::NavyBlue;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "32m" CSI "44m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold on (with default colors)
  to->fg_color = finalcut::fc::Default;
  to->bg_color = finalcut::fc::Default;
  to->attr.bit.bold = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;1m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off (with default colors)
  to->attr.bit.bold = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim on (with default colors)
  to->attr.bit.dim = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off (with default colors)
  to->attr.bit.dim = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic on (with default colors)
  to->attr.bit.italic = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off (with default colors)
  to->attr.bit.italic = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline on (with default colors)
  to->attr.bit.underline = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;4m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off (with default colors)
  to->attr.bit.underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink on (with default colors)
  to->attr.bit.blink = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;5m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off (with default colors)
  to->attr.bit.blink = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse on (with default colors)
  to->attr.bit.reverse = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;7m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off (with default colors)
  to->attr.bit.reverse = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout on (with default colors)
  to->attr.bit.standout = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;7m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off (with default colors)
  to->attr.bit.standout = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible on (with default colors)
  to->attr.bit.invisible = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017") );
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT ( to->code == ' ' );
  from->code = ' ';
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off (with default colors)
  to->attr.bit.invisible = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect on (with default colors)
  to->attr.bit.protect = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off (with default colors)
  to->attr.bit.protect = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out on (with default colors)
  to->attr.bit.crossed_out = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017" CSI "9m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off (with default colors)
  to->attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline on (with default colors)
  to->attr.bit.dbl_underline = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017" CSI "21m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off (with default colors)
  to->attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set on (with default colors)
  to->attr.bit.alt_charset = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\016") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off (with default colors)
  to->attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR("\017" CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set on (with default colors)
  to->attr.bit.pc_charset = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off (with default colors)
  to->attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Turn on all attributes (with default colors)
  to->attr.bit.pc_charset    = true;
  to->attr.bit.bold          = true;
  to->attr.bit.dim           = true;
  to->attr.bit.italic        = true;
  to->attr.bit.underline     = true;
  to->attr.bit.blink         = true;
  to->attr.bit.reverse       = true;
  to->attr.bit.standout      = true;
  to->attr.bit.invisible     = true;
  to->attr.bit.protect       = true;
  to->attr.bit.crossed_out   = true;
  to->attr.bit.dbl_underline = true;
  to->attr.bit.alt_charset   = true;
  to->attr.bit.pc_charset    = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;1;4;7;5m\016"
                                 CSI "9m" CSI "21m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Cyan text on blue background
  to->fg_color = finalcut::fc::Cyan;
  to->bg_color = finalcut::fc::Blue;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "36m" CSI "44m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off
  to->attr.bit.bold = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "22m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off
  to->attr.bit.dim = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "22m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off
  to->attr.bit.italic = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off
  to->attr.bit.underline = false;
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "24m" CSI "21m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );


  // Blink off
  to->attr.bit.blink = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "25m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off
  to->attr.bit.reverse = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "27m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off
  to->attr.bit.standout = false;
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "27m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off
  to->attr.bit.invisible = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "28m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off
  to->attr.bit.protect = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m" CSI "36m" CSI "44m\016" CSI "9m" CSI "21m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off
  to->attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "29m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off
  to->attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "24m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off
  to->attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR("\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off
  to->attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m" CSI "36m" CSI "44m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Green text color
  to->fg_color = finalcut::fc::Green;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR(CSI "32m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default text color
  to->fg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( printSequence(oa.changeAttribute(from, to)).c_str()
                         , C_STR("Esc [ 3 9 m ") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  delete to;
  delete from;
}

//----------------------------------------------------------------------
void FOptiAttrTest::linuxTest()
{
  // Simulate a Linux terminal with 16 colors

  finalcut::FOptiAttr oa;
  oa.setDefaultColorSupport();  // ANSI default color
  oa.setMaxColor (16);
  oa.setNoColorVideo (18);
  oa.set_enter_bold_mode (C_STR(CSI "1m"));
  oa.set_exit_bold_mode (C_STR(CSI "22m"));
  oa.set_enter_dim_mode (0);
  oa.set_exit_dim_mode (0);
  oa.set_enter_italics_mode (0);
  oa.set_exit_italics_mode (0);
  oa.set_enter_underline_mode (0);
  oa.set_exit_underline_mode (0);
  oa.set_enter_blink_mode (C_STR(CSI "5m"));
  oa.set_exit_blink_mode (C_STR(CSI "25m"));
  oa.set_enter_reverse_mode (C_STR(CSI "7m"));
  oa.set_exit_reverse_mode (C_STR(CSI "27m"));
  oa.set_enter_standout_mode (C_STR(CSI "7m"));
  oa.set_exit_standout_mode (C_STR(CSI "27m"));
  oa.set_enter_secure_mode (0);
  oa.set_exit_secure_mode (0);
  oa.set_enter_protected_mode (0);
  oa.set_exit_protected_mode (0);
  oa.set_enter_crossed_out_mode (0);
  oa.set_exit_crossed_out_mode (0);
  oa.set_enter_dbl_underline_mode (0);
  oa.set_exit_dbl_underline_mode (0);
  oa.set_set_attributes (C_STR(CSI "0"
                               "%?%p6%|%t;1%;"
                               "%?%p1%p3%|%t;7%;"
                               "%?%p4%t;5%;m"
                               "%?%p9%t\016%e\017%;"));
  oa.set_exit_attribute_mode (C_STR(CSI "0m\017"));
  oa.set_enter_alt_charset_mode (C_STR("\016"));
  oa.set_exit_alt_charset_mode (C_STR("\017"));
  oa.set_enter_pc_charset_mode (C_STR(CSI "11m"));
  oa.set_exit_pc_charset_mode (C_STR(CSI "10m"));
  oa.set_a_foreground_color (C_STR(CSI "3%p1%{8}%m%d"
                                   "%?%p1%{7}%>%t;1%e;22%;m"));
  oa.set_a_background_color (C_STR(CSI "4%p1%{8}%m%d"
                                   "%?%p1%{7}%>%t;5%e;25%;m"));
  oa.set_foreground_color (0);
  oa.set_background_color (0);
  oa.set_term_color_pair (0);
  oa.set_orig_pair (C_STR(CSI "39;49;25m"));
  oa.set_orig_orig_colors (C_STR(OSC "R"));
  oa.initialize();

  finalcut::FOptiAttr::charData* from = new finalcut::FOptiAttr::charData();
  finalcut::FOptiAttr::charData* to = new finalcut::FOptiAttr::charData();
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default color + bold
  from->fg_color = finalcut::fc::Default;
  from->bg_color = finalcut::fc::Default;
  to->attr.bit.bold = true;
  to->fg_color = finalcut::fc::Default;
  to->bg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;1m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blue text on white background + dim + italic
  to->fg_color = finalcut::fc::Blue;
  to->bg_color = finalcut::fc::White;
  to->attr.bit.dim = true;
  to->attr.bit.italic = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;1m\017" CSI "34;22m" CSI "47;5m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reset attributes + default background
  to->attr.bit.bold = false;
  to->attr.bit.dim = false;
  to->attr.bit.italic = false;
  to->bg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017" CSI "34;22m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Red text on black background
  to->fg_color = finalcut::fc::Red;
  to->bg_color = finalcut::fc::Black;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "31;22m" CSI "40;25m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // 256 color text and background
  to->fg_color = finalcut::fc::SpringGreen3;
  to->bg_color = finalcut::fc::NavyBlue;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "32;1m" CSI "44;25m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold on (with default colors)
  to->fg_color = finalcut::fc::Default;
  to->bg_color = finalcut::fc::Default;
  to->attr.bit.bold = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;1m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off (with default colors)
  to->attr.bit.bold = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim on (with default colors)
  to->attr.bit.dim = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off (with default colors)
  to->attr.bit.dim = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic on (with default colors)
  to->attr.bit.italic = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off (with default colors)
  to->attr.bit.italic = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline on (with default colors)
  to->attr.bit.underline = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off (with default colors)
  to->attr.bit.underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink on (with default colors)
  to->attr.bit.blink = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;5m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off (with default colors)
  to->attr.bit.blink = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse on (with default colors)
  to->attr.bit.reverse = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;7m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off (with default colors)
  to->attr.bit.reverse = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout on (with default colors)
  to->attr.bit.standout = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;7m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off (with default colors)
  to->attr.bit.standout = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible on (with default colors)
  to->attr.bit.invisible = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017") );
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT ( to->code == ' ' );
  from->code = ' ';
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off (with default colors)
  to->attr.bit.invisible = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\17") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect on (with default colors)
  to->attr.bit.protect = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off (with default colors)
  to->attr.bit.protect = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out on (with default colors)
  to->attr.bit.crossed_out = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off (with default colors)
  to->attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline on (with default colors)
  to->attr.bit.dbl_underline = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off (with default colors)
  to->attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set on (with default colors)
  to->attr.bit.alt_charset = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\016") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off (with default colors)
  to->attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR("\017" CSI "0m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set on (with default colors)
  to->attr.bit.pc_charset = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017" CSI "11m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off (with default colors)
  to->attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017" CSI "10m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Turn on all attributes (with default colors)
  to->attr.bit.pc_charset    = true;
  to->attr.bit.bold          = true;
  to->attr.bit.dim           = true;
  to->attr.bit.italic        = true;
  to->attr.bit.underline     = true;
  to->attr.bit.blink         = true;
  to->attr.bit.reverse       = true;
  to->attr.bit.standout      = true;
  to->attr.bit.invisible     = true;
  to->attr.bit.protect       = true;
  to->attr.bit.crossed_out   = true;
  to->attr.bit.dbl_underline = true;
  to->attr.bit.alt_charset   = true;
  to->attr.bit.pc_charset    = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;1;7;5m\016"
                                 CSI "11m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Cyan text on blue background
  to->fg_color = finalcut::fc::Cyan;
  to->bg_color = finalcut::fc::Blue;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "36;22m" CSI "44;25m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off
  to->attr.bit.bold = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "22m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off
  to->attr.bit.dim = false;
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off
  to->attr.bit.italic = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off
  to->attr.bit.underline = false;
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off
  to->attr.bit.blink = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "25m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off
  to->attr.bit.reverse = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "27m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off
  to->attr.bit.standout = false;
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "27m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off
  to->attr.bit.invisible = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off
  to->attr.bit.protect = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off
  to->attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off
  to->attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off
  to->attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR("\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off
  to->attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017" CSI "10m" CSI "36;22m" CSI "44;25m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Green text color
  to->fg_color = finalcut::fc::Green;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR(CSI "32;22m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default text color
  to->fg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( printSequence(oa.changeAttribute(from, to)).c_str()
                         , C_STR("Esc [ 3 9 m ") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  delete to;
  delete from;
}

//----------------------------------------------------------------------
void FOptiAttrTest::cygwinTest()
{
  // Simulate a CygWin terminal

  linuxTest();
}

//----------------------------------------------------------------------
void FOptiAttrTest::puttyTest()
{
  // Simulate a putty-256color terminal

  finalcut::FOptiAttr oa;
  oa.unsetDefaultColorSupport();  // No ANSI default color
  oa.setMaxColor (256);
  oa.setNoColorVideo (0);
  oa.set_enter_bold_mode (C_STR(CSI "1m"));
  oa.set_exit_bold_mode (C_STR(CSI "22m"));
  oa.set_enter_dim_mode (C_STR(CSI "2m"));
  oa.set_exit_dim_mode (C_STR(CSI "22m"));
  oa.set_enter_italics_mode (0);
  oa.set_exit_italics_mode (0);
  oa.set_enter_underline_mode (C_STR(CSI "4m"));
  oa.set_exit_underline_mode (C_STR(CSI "24m"));
  oa.set_enter_blink_mode (C_STR(CSI "5m"));
  oa.set_exit_blink_mode (C_STR(CSI "25m"));
  oa.set_enter_reverse_mode (C_STR(CSI "7m"));
  oa.set_exit_reverse_mode (C_STR(CSI "27m"));
  oa.set_enter_standout_mode (C_STR(CSI "7m"));
  oa.set_exit_standout_mode (C_STR(CSI "27m"));
  oa.set_enter_secure_mode (0);
  oa.set_exit_secure_mode (C_STR(CSI "28m"));
  oa.set_enter_protected_mode (0);
  oa.set_exit_protected_mode (C_STR(CSI "0m"));
  oa.set_enter_crossed_out_mode (C_STR(CSI "9m"));
  oa.set_exit_crossed_out_mode (C_STR(CSI "29m"));
  oa.set_enter_dbl_underline_mode (C_STR(CSI "21m"));
  oa.set_exit_dbl_underline_mode (C_STR(CSI "24m"));
  oa.set_set_attributes (C_STR(CSI "0"
                               "%?%p1%p6%|%t;1%;"
                               "%?%p5%t;2%;"
                               "%?%p2%t;4%;"
                               "%?%p1%p3%|%t;7%;"
                               "%?%p4%t;5%;m"
                               "%?%p9%t\016%e\017%;"));
  oa.set_exit_attribute_mode (C_STR(CSI "0m"));
  oa.set_enter_alt_charset_mode (C_STR("\016"));
  oa.set_exit_alt_charset_mode (C_STR("\017"));
  oa.set_enter_pc_charset_mode (C_STR(CSI "11m"));
  oa.set_exit_pc_charset_mode (C_STR(CSI "10m"));
  oa.set_a_foreground_color (C_STR(CSI "%?%p1%{8}%<"
                                       "%t3%p1%d"
                                       "%e%p1%{16}%<"
                                       "%t9%p1%{8}%-%d"
                                       "%e38;5;%p1%d%;m"));
  oa.set_a_background_color (C_STR(CSI "%?%p1%{8}%<"
                                       "%t4%p1%d"
                                       "%e%p1%{16}%<"
                                       "%t10%p1%{8}%-%d"
                                       "%e48;5;%p1%d%;m"));
  oa.set_foreground_color (0);
  oa.set_background_color (0);
  oa.set_term_color_pair (0);
  oa.set_orig_pair (C_STR(CSI "39;49m"));
  oa.set_orig_orig_colors (C_STR(OSC "R"));
  oa.initialize();


  finalcut::FOptiAttr::charData* from = new finalcut::FOptiAttr::charData();
  finalcut::FOptiAttr::charData* to = new finalcut::FOptiAttr::charData();
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default color + bold
  from->fg_color = finalcut::fc::Default;
  from->bg_color = finalcut::fc::Default;
  to->attr.bit.bold = true;
  to->fg_color = finalcut::fc::Default;
  to->bg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;1m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blue text on white background + dim + italic
  to->fg_color = finalcut::fc::Blue;
  to->bg_color = finalcut::fc::White;
  to->attr.bit.dim = true;
  to->attr.bit.italic = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;1;2m\017"
                                 CSI "34m" CSI "107m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reset attributes + default background
  to->attr.bit.bold = false;
  to->attr.bit.dim = false;
  to->attr.bit.italic = false;
  to->bg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m" CSI "39;49m" CSI "34m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Red text on black background
  to->fg_color = finalcut::fc::Red;
  to->bg_color = finalcut::fc::Black;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "31m" CSI "40m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // 256 color text and background
  to->fg_color = finalcut::fc::SpringGreen3;
  to->bg_color = finalcut::fc::NavyBlue;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "38;5;42m" CSI "48;5;17m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold on (with default colors)
  to->fg_color = finalcut::fc::Default;
  to->bg_color = finalcut::fc::Default;
  to->attr.bit.bold = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;1m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off (with default colors)
  to->attr.bit.bold = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim on (with default colors)
  to->attr.bit.dim = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;2m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off (with default colors)
  to->attr.bit.dim = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic on (with default colors)
  to->attr.bit.italic = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off (with default colors)
  to->attr.bit.italic = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline on (with default colors)
  to->attr.bit.underline = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;4m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off (with default colors)
  to->attr.bit.underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink on (with default colors)
  to->attr.bit.blink = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;5m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off (with default colors)
  to->attr.bit.blink = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse on (with default colors)
  to->attr.bit.reverse = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;7m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off (with default colors)
  to->attr.bit.reverse = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout on (with default colors)
  to->attr.bit.standout = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;1;7m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off (with default colors)
  to->attr.bit.standout = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible on (with default colors)
  to->attr.bit.invisible = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017") );
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT ( to->code == ' ' );
  from->code = ' ';
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off (with default colors)
  to->attr.bit.invisible = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect on (with default colors)
  to->attr.bit.protect = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off (with default colors)
  to->attr.bit.protect = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out on (with default colors)
  to->attr.bit.crossed_out = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017" CSI "9m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off (with default colors)
  to->attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline on (with default colors)
  to->attr.bit.dbl_underline = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017" CSI "21m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off (with default colors)
  to->attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set on (with default colors)
  to->attr.bit.alt_charset = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\016") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off (with default colors)
  to->attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR("\017" CSI "0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set on (with default colors)
  to->attr.bit.pc_charset = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017" CSI "11m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off (with default colors)
  to->attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m" CSI "10m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Turn on all attributes (with default colors)
  to->attr.bit.pc_charset    = true;
  to->attr.bit.bold          = true;
  to->attr.bit.dim           = true;
  to->attr.bit.italic        = true;
  to->attr.bit.underline     = true;
  to->attr.bit.blink         = true;
  to->attr.bit.reverse       = true;
  to->attr.bit.standout      = true;
  to->attr.bit.invisible     = true;
  to->attr.bit.protect       = true;
  to->attr.bit.crossed_out   = true;
  to->attr.bit.dbl_underline = true;
  to->attr.bit.alt_charset   = true;
  to->attr.bit.pc_charset    = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;1;2;4;7;5m\016"
                                 CSI "9m" CSI "21m" CSI "11m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Cyan text on blue background
  to->fg_color = finalcut::fc::Cyan;
  to->bg_color = finalcut::fc::Blue;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "36m" CSI "44m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off
  to->attr.bit.bold = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "22m" CSI "2m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off
  to->attr.bit.dim = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "22m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off
  to->attr.bit.italic = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off
  to->attr.bit.underline = false;
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "24m" CSI "21m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );


  // Blink off
  to->attr.bit.blink = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "25m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off
  to->attr.bit.reverse = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "27m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off
  to->attr.bit.standout = false;
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "27m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off
  to->attr.bit.invisible = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "28m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off
  to->attr.bit.protect = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m" CSI "36m" CSI "44m" "\016"
                                 CSI "11m" CSI "9m" CSI "21m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off
  to->attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "29m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off
  to->attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "24m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off
  to->attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR("\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off
  to->attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m" CSI "10m" CSI "36m" CSI "44m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Green text color
  to->fg_color = finalcut::fc::Green;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR(CSI "32m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default text color
  to->fg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( printSequence(oa.changeAttribute(from, to)).c_str()
                         , C_STR("Esc [ 3 9 ; 4 9 m Esc [ 4 4 m ") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  delete to;
  delete from;
}

//----------------------------------------------------------------------
void FOptiAttrTest::teratermTest()
{
  // Simulate a Tera Term terminal

  finalcut::FOptiAttr oa;
  oa.unsetDefaultColorSupport();  // No ANSI default color
  oa.setMaxColor (16);
  oa.setNoColorVideo (41);  // Avoid standout (1) + blink (8) + bold (32)
  oa.set_enter_bold_mode (C_STR(CSI "1m"));
  oa.set_exit_bold_mode (C_STR(CSI "22m"));
  oa.set_enter_dim_mode (0);
  oa.set_exit_dim_mode (C_STR(CSI "22m"));
  oa.set_enter_italics_mode (0);
  oa.set_exit_italics_mode (0);
  oa.set_enter_underline_mode (C_STR(CSI "4m"));
  oa.set_exit_underline_mode (C_STR(CSI "24m"));
  oa.set_enter_blink_mode (C_STR(CSI "5m"));
  oa.set_exit_blink_mode (C_STR(CSI "25m"));
  oa.set_enter_reverse_mode (C_STR(CSI "7m"));
  oa.set_exit_reverse_mode (C_STR(CSI "27m"));
  oa.set_enter_standout_mode (C_STR(CSI "7m"));
  oa.set_exit_standout_mode (C_STR(CSI "27m"));
  oa.set_enter_secure_mode (0);
  oa.set_exit_secure_mode (C_STR(CSI "28m"));
  oa.set_enter_protected_mode (0);
  oa.set_exit_protected_mode (C_STR(CSI "0m$<2>"));
  oa.set_enter_crossed_out_mode (C_STR(CSI "9m"));
  oa.set_exit_crossed_out_mode (C_STR(CSI "29m"));
  oa.set_enter_dbl_underline_mode (C_STR(CSI "21m"));
  oa.set_exit_dbl_underline_mode (C_STR(CSI "24m"));
  oa.set_set_attributes (C_STR(CSI "0"
                               "%?%p1%p6%|%t;1%;"
                               "%?%p2%t;4%;"
                               "%?%p1%p3%|%t;7%;"
                               "%?%p4%t;5%;m"
                               "%?%p9%t\016%e\017%;$<2>"));
  oa.set_exit_attribute_mode (C_STR(CSI "0m$<2>"));
  oa.set_enter_alt_charset_mode (C_STR("\016"));
  oa.set_exit_alt_charset_mode (C_STR("\017"));
  oa.set_enter_pc_charset_mode (0);
  oa.set_exit_pc_charset_mode (0);
  oa.set_a_foreground_color (C_STR(CSI "38;5;%p1%dm"));
  oa.set_a_background_color (C_STR(CSI "48;5;%p1%dm"));
  oa.set_foreground_color (0);
  oa.set_background_color (0);
  oa.set_term_color_pair (0);
  oa.set_orig_pair (C_STR(CSI "39;49m"));
  oa.set_orig_orig_colors (0);
  oa.initialize();


  finalcut::FOptiAttr::charData* from = new finalcut::FOptiAttr::charData();
  finalcut::FOptiAttr::charData* to = new finalcut::FOptiAttr::charData();
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default color + bold
  from->fg_color = finalcut::fc::Default;
  from->bg_color = finalcut::fc::Default;
  to->attr.bit.bold = true;
  to->fg_color = finalcut::fc::Default;
  to->bg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;1m\017$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blue text on white background + dim + italic
  to->fg_color = finalcut::fc::Blue;
  to->bg_color = finalcut::fc::White;
  to->attr.bit.dim = true;
  to->attr.bit.italic = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017$<2>" CSI "38;5;4m"
                                 CSI "48;5;15m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reset attributes + default background
  to->attr.bit.bold = false;
  to->attr.bit.dim = false;
  to->attr.bit.italic = false;
  to->bg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>" CSI "39;49m"
                                 CSI "38;5;4m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Red text on black background
  to->fg_color = finalcut::fc::Red;
  to->bg_color = finalcut::fc::Black;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "38;5;1m" CSI "48;5;0m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // 256 color text and background
  to->fg_color = finalcut::fc::SpringGreen3;
  to->bg_color = finalcut::fc::NavyBlue;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "38;5;10m" CSI "48;5;4m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold on (with default colors)
  to->fg_color = finalcut::fc::Default;
  to->bg_color = finalcut::fc::Default;
  to->attr.bit.bold = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;1m\017$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off (with default colors)
  to->attr.bit.bold = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim on (with default colors)
  to->attr.bit.dim = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off (with default colors)
  to->attr.bit.dim = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic on (with default colors)
  to->attr.bit.italic = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off (with default colors)
  to->attr.bit.italic = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline on (with default colors)
  to->attr.bit.underline = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;4m\017$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off (with default colors)
  to->attr.bit.underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink on (with default colors)
  to->attr.bit.blink = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;5m\017$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off (with default colors)
  to->attr.bit.blink = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse on (with default colors)
  to->attr.bit.reverse = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;7m\017$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off (with default colors)
  to->attr.bit.reverse = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout on (with default colors)
  to->attr.bit.standout = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;1;7m\017$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off (with default colors)
  to->attr.bit.standout = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible on (with default colors)
  to->attr.bit.invisible = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017$<2>") );
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT ( to->code == ' ' );
  from->code = ' ';
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off (with default colors)
  to->attr.bit.invisible = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect on (with default colors)
  to->attr.bit.protect = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off (with default colors)
  to->attr.bit.protect = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out on (with default colors)
  to->attr.bit.crossed_out = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017$<2>" CSI "9m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off (with default colors)
  to->attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline on (with default colors)
  to->attr.bit.dbl_underline = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017$<2>" CSI "21m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off (with default colors)
  to->attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set on (with default colors)
  to->attr.bit.alt_charset = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\016$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off (with default colors)
  to->attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR("\017" CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set on (with default colors)
  to->attr.bit.pc_charset = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m\017$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off (with default colors)
  to->attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Turn on all attributes (with default colors)
  to->attr.bit.pc_charset    = true;
  to->attr.bit.bold          = true;
  to->attr.bit.dim           = true;
  to->attr.bit.italic        = true;
  to->attr.bit.underline     = true;
  to->attr.bit.blink         = true;
  to->attr.bit.reverse       = true;
  to->attr.bit.standout      = true;
  to->attr.bit.invisible     = true;
  to->attr.bit.protect       = true;
  to->attr.bit.crossed_out   = true;
  to->attr.bit.dbl_underline = true;
  to->attr.bit.alt_charset   = true;
  to->attr.bit.pc_charset    = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;1;4;7;5m\016$<2>"
                                 CSI "9m" CSI "21m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Cyan text on blue background
  to->fg_color = finalcut::fc::Cyan;
  to->bg_color = finalcut::fc::Blue;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "38;5;6m" CSI "48;5;4m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off
  to->attr.bit.bold = false;
  CPPUNIT_ASSERT ( *from == *to );  // because of noColorVideo = 41
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off
  to->attr.bit.dim = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "22m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off
  to->attr.bit.italic = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off
  to->attr.bit.underline = false;
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "24m" CSI "21m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off
  to->attr.bit.blink = false;
  CPPUNIT_ASSERT ( *from == *to );  // because of noColorVideo = 41
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off
  to->attr.bit.reverse = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "27m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off
  to->attr.bit.standout = false;
  CPPUNIT_ASSERT ( *from == *to );  // because of noColorVideo = 41
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off
  to->attr.bit.invisible = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "28m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off
  to->attr.bit.protect = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>" CSI "38;5;6m" CSI "48;5;4m"
                           "\016" CSI "9m" CSI "21m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off
  to->attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "29m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off
  to->attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "24m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off
  to->attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR("\017") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off
  to->attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>" CSI "38;5;6m"
                                 CSI "48;5;4m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Green text color
  to->fg_color = finalcut::fc::Green;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR(CSI "38;5;2m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default text color
  to->fg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( printSequence(oa.changeAttribute(from, to)).c_str()
                         , C_STR("Esc [ 3 9 ; 4 9 m Esc [ 4 8 ; 5 ; 4 m ") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  delete to;
  delete from;
}

//----------------------------------------------------------------------
void FOptiAttrTest::ibmColorTest()
{
  // Simulate IBM color definitions

  finalcut::FOptiAttr oa;
  oa.unsetDefaultColorSupport();  // No ANSI default color
  oa.setMaxColor (8);
  oa.setNoColorVideo (3);  // Avoid standout (1) + underline mode (2)
  oa.set_enter_bold_mode (0);
  oa.set_exit_bold_mode (0);
  oa.set_enter_dim_mode (0);
  oa.set_exit_dim_mode (0);
  oa.set_enter_italics_mode (0);
  oa.set_exit_italics_mode (0);
  oa.set_enter_underline_mode (0);
  oa.set_exit_underline_mode (0);
  oa.set_enter_blink_mode (0);
  oa.set_exit_blink_mode (0);
  oa.set_enter_reverse_mode (0);
  oa.set_exit_reverse_mode (0);
  oa.set_enter_standout_mode (0);
  oa.set_exit_standout_mode (0);
  oa.set_enter_secure_mode (0);
  oa.set_exit_secure_mode (0);
  oa.set_enter_protected_mode (0);
  oa.set_exit_protected_mode (0);
  oa.set_enter_crossed_out_mode (0);
  oa.set_exit_crossed_out_mode (0);
  oa.set_enter_dbl_underline_mode (0);
  oa.set_exit_dbl_underline_mode (0);
  oa.set_set_attributes (0);
  oa.set_exit_attribute_mode (0);
  oa.set_enter_alt_charset_mode (0);
  oa.set_exit_alt_charset_mode (0);
  oa.set_enter_pc_charset_mode (0);
  oa.set_exit_pc_charset_mode (0);
  oa.set_a_foreground_color (0);
  oa.set_a_background_color (0);
  oa.set_foreground_color (C_STR(CSI "%?%p1%{0}%=%t30m"
                                     "%e%p1%{1}%=%t31m"
                                     "%e%p1%{2}%=%t32m"
                                     "%e%p1%{3}%=%t33m"
                                     "%e%p1%{4}%=%t34m"
                                     "%e%p1%{5}%=%t35m"
                                     "%e%p1%{6}%=%t36m"
                                     "%e%p1%{7}%=%t97m%;"));
  oa.set_background_color (C_STR(CSI "%?%p1%{0}%=%t40m"
                                     "%e%p1%{1}%=%t41m"
                                     "%e%p1%{2}%=%t42m"
                                     "%e%p1%{3}%=%t43m"
                                     "%e%p1%{4}%=%t44m"
                                     "%e%p1%{5}%=%t45m"
                                     "%e%p1%{6}%=%t46m"
                                     "%e%p1%{7}%=%t107m%;"));
  oa.set_term_color_pair (0);
  oa.set_orig_pair (C_STR(CSI "32;40m"));
  oa.set_orig_orig_colors (0);
  oa.initialize();


  finalcut::FOptiAttr::charData* from = new finalcut::FOptiAttr::charData();
  finalcut::FOptiAttr::charData* to = new finalcut::FOptiAttr::charData();
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default color + bold
  from->fg_color = finalcut::fc::Default;
  from->bg_color = finalcut::fc::Default;
  to->attr.bit.bold = true;
  to->fg_color = finalcut::fc::Default;
  to->bg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blue text on white background + dim + italic
  to->fg_color = finalcut::fc::Blue;
  to->bg_color = finalcut::fc::White;
  to->attr.bit.dim = true;
  to->attr.bit.italic = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "31m" CSI "107m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reset attributes + default background
  to->attr.bit.bold = false;
  to->attr.bit.dim = false;
  to->attr.bit.italic = false;
  to->bg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "32;40m" CSI "31m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Red text on black background
  to->fg_color = finalcut::fc::Red;
  to->bg_color = finalcut::fc::Black;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "34m" CSI "40m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // 256 color text and background
  to->fg_color = finalcut::fc::SpringGreen3;
  to->bg_color = finalcut::fc::NavyBlue;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "32m" CSI "41m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold on (with default colors)
  to->fg_color = finalcut::fc::Default;
  to->bg_color = finalcut::fc::Default;
  to->attr.bit.bold = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "32;40m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off (with default colors)
  to->attr.bit.bold = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim on (with default colors)
  to->attr.bit.dim = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off (with default colors)
  to->attr.bit.dim = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic on (with default colors)
  to->attr.bit.italic = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off (with default colors)
  to->attr.bit.italic = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline on (with default colors)
  to->attr.bit.underline = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off (with default colors)
  to->attr.bit.underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink on (with default colors)
  to->attr.bit.blink = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off (with default colors)
  to->attr.bit.blink = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse on (with default colors)
  to->attr.bit.reverse = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off (with default colors)
  to->attr.bit.reverse = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout on (with default colors)
  to->attr.bit.standout = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off (with default colors)
  to->attr.bit.standout = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible on (with default colors)
  to->attr.bit.invisible = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT ( to->code == ' ' );
  from->code = ' ';
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off (with default colors)
  to->attr.bit.invisible = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect on (with default colors)
  to->attr.bit.protect = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off (with default colors)
  to->attr.bit.protect = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out on (with default colors)
  to->attr.bit.crossed_out = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off (with default colors)
  to->attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline on (with default colors)
  to->attr.bit.dbl_underline = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off (with default colors)
  to->attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set on (with default colors)
  to->attr.bit.alt_charset = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off (with default colors)
  to->attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set on (with default colors)
  to->attr.bit.pc_charset = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off (with default colors)
  to->attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Turn on all attributes (with default colors)
  to->attr.bit.pc_charset    = true;
  to->attr.bit.bold          = true;
  to->attr.bit.dim           = true;
  to->attr.bit.italic        = true;
  to->attr.bit.underline     = true;
  to->attr.bit.blink         = true;
  to->attr.bit.reverse       = true;
  to->attr.bit.standout      = true;
  to->attr.bit.invisible     = true;
  to->attr.bit.protect       = true;
  to->attr.bit.crossed_out   = true;
  to->attr.bit.dbl_underline = true;
  to->attr.bit.alt_charset   = true;
  to->attr.bit.pc_charset    = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Cyan text on blue background
  to->fg_color = finalcut::fc::Cyan;
  to->bg_color = finalcut::fc::Blue;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "33m" CSI "41m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off
  to->attr.bit.bold = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off
  to->attr.bit.dim = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off
  to->attr.bit.italic = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off
  to->attr.bit.underline = false;
  CPPUNIT_ASSERT ( *from == *to );  // because of noColorVideo = 3
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off
  to->attr.bit.blink = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off
  to->attr.bit.reverse = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off
  to->attr.bit.standout = false;
  CPPUNIT_ASSERT ( *from == *to );  // because of noColorVideo = 3
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off
  to->attr.bit.invisible = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off
  to->attr.bit.protect = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off
  to->attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off
  to->attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off
  to->attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off
  to->attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Green text color
  to->fg_color = finalcut::fc::Green;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "32m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default text color
  to->fg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( printSequence(oa.changeAttribute(from, to)).c_str()
                         , C_STR("Esc [ 3 2 ; 4 0 m Esc [ 4 1 m ") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  delete to;
  delete from;
}

//----------------------------------------------------------------------
void FOptiAttrTest::wyse50Test()
{
  // Simulate an Wyse-50 terminal

  finalcut::FOptiAttr oa;
  finalcut::FOptiAttr::termEnv optiattr_env =
  {
    false,                              // No ANSI default color
    1,                                  // Max color
    0,                                  // No color video
    0,                                  // Enter bold
    C_STR(ESC "("  ESC "H\003"
          ESC "G0" ESC "cD"),           // Exit bold
    C_STR(ESC "Gp"),                    // Enter dim
    C_STR(ESC "("  ESC "H\003"
          ESC "G0" ESC "cD"),           // Exit dim
    0,                                  // Enter italics
    0,                                  // Exit italics
    C_STR(ESC "G8"),                    // Enter underline
    C_STR(ESC "G0"),                    // Exit underline
    C_STR(ESC "G2"),                    // Enter blink
    C_STR(ESC "("  ESC "H\003"
          ESC "G0" ESC "cD"),           // Exit blink
    C_STR(ESC "G2"),                    // Enter reverse
    C_STR(ESC "("  ESC "H\003"
          ESC "G0" ESC "cD"),           // Exit reverse
    C_STR(ESC "Gt"),                    // Enter standout
    C_STR(ESC "G0"),                    // Exit standout
    C_STR(ESC "G1"),                    // Enter secure
    C_STR(ESC "("  ESC "H\003"
          ESC "G0" ESC "cD"),           // Exit secure
    C_STR(ESC ")"),                     // Enter protected
    C_STR(ESC "("  ESC "H\003"
          ESC "G0" ESC "cD"),           // Exit protected
    0,                                  // Enter crossed out
    C_STR(ESC "("  ESC "H\003"
          ESC "G0" ESC "cD"),           // Exit crossed out
    0,                                  // Enter double underline
    0,                                  // Exit double underline
    C_STR("%?%p8%t\033)%e\033(%;"
          "%?%p9%t\033cE%e\033cD%;\033G%'0'"
          "%?%p2%t%{8}%|%;"
          "%?%p1%p3%|%p6%|%t%{4}%|%;"
          "%?%p4%t%{2}%|%;"
          "%?%p1%p5%|%t%'@'%|%;"
          "%?%p7%t%{1}%|%;%c"),         // Set attributes
    C_STR(ESC "("  ESC "H\003"
          ESC "G0" ESC "cD"),           // Exit attribute
    C_STR(ESC "cE"),                    // Enter alt charset
    C_STR(ESC "cD"),                    // Exit alt charset
    0,                                  // Enter pc charset
    0,                                  // Exit pc charset
    0,                                  // Ansi foreground color
    0,                                  // Ansi background color
    0,                                  // Foreground color
    0,                                  // Background color
    0,                                  // Term color pair
    0,                                  // Orig pair
    0                                   // Orig orig colors
  };

  oa.setTermEnvironment(optiattr_env);

  finalcut::FOptiAttr::charData* from = new finalcut::FOptiAttr::charData();
  finalcut::FOptiAttr::charData* to = new finalcut::FOptiAttr::charData();
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default color + bold
  from->fg_color = finalcut::fc::Default;
  from->bg_color = finalcut::fc::Default;
  to->attr.bit.bold = true;
  to->fg_color = finalcut::fc::Default;
  to->bg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "cD" ESC "G4") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blue text on white background + dim + italic
  to->fg_color = finalcut::fc::Blue;
  to->bg_color = finalcut::fc::White;
  to->attr.bit.dim = true;
  to->attr.bit.italic = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "cD" ESC "Gt") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reset attributes + default background
  to->attr.bit.bold = false;
  to->attr.bit.dim = false;
  to->attr.bit.italic = false;
  to->bg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "H\003" ESC "G0" ESC "cD") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Red text on black background
  to->fg_color = finalcut::fc::Red;
  to->bg_color = finalcut::fc::Black;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // 256 color text and background
  to->fg_color = finalcut::fc::SpringGreen3;
  to->bg_color = finalcut::fc::NavyBlue;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold on (with default colors)
  to->fg_color = finalcut::fc::Default;
  to->bg_color = finalcut::fc::Default;
  to->attr.bit.bold = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "cD" ESC "G4") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off (with default colors)
  to->attr.bit.bold = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "H\003" ESC "G0" ESC "cD") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim on (with default colors)
  to->attr.bit.dim = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "cD" ESC "Gp") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off (with default colors)
  to->attr.bit.dim = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "H\003" ESC "G0" ESC "cD") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic on (with default colors)
  to->attr.bit.italic = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "cD" ESC "G0") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off (with default colors)
  to->attr.bit.italic = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "H\003" ESC "G0" ESC "cD") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline on (with default colors)
  to->attr.bit.underline = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "cD" ESC "G8") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off (with default colors)
  to->attr.bit.underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "H\003" ESC "G0" ESC "cD") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink on (with default colors)
  to->attr.bit.blink = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "cD" ESC "G2") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off (with default colors)
  to->attr.bit.blink = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "H\003" ESC "G0" ESC "cD") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse on (with default colors)
  to->attr.bit.reverse = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "cD" ESC "G4") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off (with default colors)
  to->attr.bit.reverse = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "H\003" ESC "G0" ESC "cD") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout on (with default colors)
  to->attr.bit.standout = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "cD" ESC "Gt") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off (with default colors)
  to->attr.bit.standout = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "H\003" ESC "G0" ESC "cD") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible on (with default colors)
  to->attr.bit.invisible = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "cD" ESC "G1") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off (with default colors)
  to->attr.bit.invisible = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "H\003" ESC "G0" ESC "cD") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect on (with default colors)
  to->attr.bit.protect = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC ")" ESC "cD" ESC "G0") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off (with default colors)
  to->attr.bit.protect = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "H\003" ESC "G0" ESC "cD") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out on (with default colors)
  to->attr.bit.crossed_out = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "cD" ESC "G0") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off (with default colors)
  to->attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "H\003" ESC "G0" ESC "cD") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline on (with default colors)
  to->attr.bit.dbl_underline = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "cD" ESC "G0") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off (with default colors)
  to->attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "H\003" ESC "G0" ESC "cD") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set on (with default colors)
  to->attr.bit.alt_charset = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "cE" ESC "G0") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off (with default colors)
  to->attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "cD" ESC "(" ESC "H\003" ESC "G0"
                                 ESC "cD") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set on (with default colors)
  to->attr.bit.pc_charset = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "cD" ESC "G0") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off (with default colors)
  to->attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "H\003" ESC "G0" ESC "cD") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Turn on all attributes (with default colors)
  to->attr.bit.pc_charset    = true;
  to->attr.bit.bold          = true;
  to->attr.bit.dim           = true;
  to->attr.bit.italic        = true;
  to->attr.bit.underline     = true;
  to->attr.bit.blink         = true;
  to->attr.bit.reverse       = true;
  to->attr.bit.standout      = true;
  to->attr.bit.invisible     = true;
  to->attr.bit.protect       = true;
  to->attr.bit.crossed_out   = true;
  to->attr.bit.dbl_underline = true;
  to->attr.bit.alt_charset   = true;
  to->attr.bit.pc_charset    = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC ")" ESC "cE" ESC "G\177") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Cyan text on blue background
  to->fg_color = finalcut::fc::Cyan;
  to->bg_color = finalcut::fc::Blue;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off
  to->attr.bit.bold = false;
  CPPUNIT_ASSERT ( *from != *to );

  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "H\003" ESC "G0" ESC "cD"
                                 ESC "cE" ESC "Gp" ESC "G8" ESC "G2"
                                 ESC "G2" ESC "Gt" ESC "G1" ESC ")" ) );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off
  to->attr.bit.dim = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "H\003" ESC "G0" ESC "cD"
                                 ESC "cE" ESC "G8" ESC "G2" ESC "G2"
                                 ESC "Gt" ESC "G1" ESC ")" ) );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off
  to->attr.bit.italic = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off
  to->attr.bit.underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "G0" ESC "cE" ESC "G2" ESC "G2"
                                 ESC "Gt" ESC "G1" ESC ")" ) );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off
  to->attr.bit.blink = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "H\003" ESC "G0" ESC "cD"
                                 ESC "cE" ESC "G2" ESC "Gt" ESC "G1"
                                 ESC ")" ) );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off
  to->attr.bit.reverse = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "H\003" ESC "G0" ESC "cD"
                                 ESC "cE" ESC "Gt" ESC "G1" ESC ")" ) );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off
  to->attr.bit.standout = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "G0" ESC "cE" ESC "G1" ESC ")") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off
  to->attr.bit.invisible = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "H\003" ESC "G0" ESC "cD"
                                 ESC "cE" ESC ")" ) );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off
  to->attr.bit.protect = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "H\003" ESC "G0" ESC "cD"
                                 ESC "cE") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off
  to->attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "H\003" ESC "G0" ESC "cD"
                                 ESC "cE") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off
  to->attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off
  to->attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR(ESC "cD") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off
  to->attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(ESC "(" ESC "H\003" ESC "G0" ESC "cD") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Green text color
  to->fg_color = finalcut::fc::Green;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default text color
  to->fg_color = finalcut::fc::Default;
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  delete to;
  delete from;
}

//----------------------------------------------------------------------
std::string FOptiAttrTest::printSequence (const std::string& s)
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
CPPUNIT_TEST_SUITE_REGISTRATION (FOptiAttrTest);

// The general unit test main part
#include <main-test.inc>
