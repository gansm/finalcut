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
    void vga2ansi();
    void ansiTest();
    void vt100Test();

  private:
    std::string printSequence (char*);

    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FOptiAttrTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (noArgumentTest);
    CPPUNIT_TEST (vga2ansi);
    CPPUNIT_TEST (ansiTest);
    CPPUNIT_TEST (vt100Test);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};
#pragma pack(pop)


//----------------------------------------------------------------------
void FOptiAttrTest::classNameTest()
{
  FOptiAttr opti_attr;
  const char* const classname = opti_attr.getClassName();
  CPPUNIT_ASSERT_CSTRING ( classname, "FOptiAttr");
}

//----------------------------------------------------------------------
void FOptiAttrTest::noArgumentTest()
{
  FOptiAttr::char_data* ch = new FOptiAttr::char_data();
  FOptiAttr oa;
  oa.initialize();

  // isNormal test
  CPPUNIT_ASSERT ( ! oa.isNormal(ch) );
  ch->fg_color = fc::Default;
  CPPUNIT_ASSERT ( ! oa.isNormal(ch) );
  ch->bg_color = fc::Default;
  CPPUNIT_ASSERT ( oa.isNormal(ch) );

  // Null test
  FOptiAttr::char_data* ch_null = 0;
  CPPUNIT_ASSERT ( oa.changeAttribute(ch, ch) == 0 );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(ch, ch_null), C_STR("") );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(ch_null, ch), C_STR("") );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(ch_null, ch_null), C_STR("") );
  delete ch;
}

//----------------------------------------------------------------------
void FOptiAttrTest::vga2ansi()
{
  FOptiAttr oa;
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
void FOptiAttrTest::ansiTest()
{
  FOptiAttr oa;
  oa.setDefaultColorSupport();  // ANSI default color
//oa.setCygwinTerminal();       // Cygwin bold color fix
  oa.setNoColorVideo (3);       // Advid standout (1) + underline mode (2)
  oa.setMaxColor (8);
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

  FOptiAttr::char_data* from = new FOptiAttr::char_data();
  FOptiAttr::char_data* to = new FOptiAttr::char_data();
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default color + bold
  from->fg_color = fc::Default;
  from->bg_color = fc::Default;
  to->attr.bit.bold = true;
  to->fg_color = fc::Default;
  to->bg_color = fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;10;1m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blue text on white background + dim + italic
  to->fg_color = fc::Blue;
  to->bg_color = fc::White;
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
  to->bg_color = fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m" CSI "34m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Red text on black background
  to->fg_color = fc::Red;
  to->bg_color = fc::Black;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "31m" CSI "40m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // 256 color text and background
  to->fg_color = fc::SpringGreen3;
  to->bg_color = fc::NavyBlue;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "32m" CSI "44m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold on (with default colors)
  to->fg_color = fc::Default;
  to->bg_color = fc::Default;
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
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off (with default colors)
  to->attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m") );
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
  to->fg_color = fc::Cyan;
  to->bg_color = fc::Blue;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "36m" CSI "44m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off
  to->attr.bit.bold = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m" CSI "36m" CSI "44m" CSI "11m"
                                 CSI "5m" CSI "7m" CSI "8m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off
  to->attr.bit.dim = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m" CSI "36m" CSI "44m" CSI "11m"
                                 CSI "5m" CSI "7m" CSI "8m") );
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
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m" CSI "36m" CSI "44m"
                                 CSI "11m" CSI "7m" CSI "8m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off
  to->attr.bit.reverse = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m" CSI "36m" CSI "44m"
                                 CSI "11m" CSI "8m") );
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
                         , C_STR(CSI "0m" CSI "36m" CSI "44m" CSI "11m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off
  to->attr.bit.protect = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m" CSI "36m" CSI "44m" CSI "11m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off
  to->attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m" CSI "36m" CSI "44m" CSI "11m") );
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
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m" CSI "10m" CSI "36m" CSI "44m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Green text color
  to->fg_color = fc::Green;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR(CSI "32m") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default text color
  to->fg_color = fc::Default;
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
  FOptiAttr oa;
//oa.setDefaultColorSupport();  // ANSI default color
//oa.setCygwinTerminal();       // Cygwin bold color fix
  oa.setNoColorVideo (0);
  oa.setMaxColor (1);
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

  FOptiAttr::char_data* from = new FOptiAttr::char_data();
  FOptiAttr::char_data* to = new FOptiAttr::char_data();
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default color + bold
  from->fg_color = fc::Default;
  from->bg_color = fc::Default;
  to->attr.bit.bold = true;
  to->fg_color = fc::Default;
  to->bg_color = fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;1m\017$<2>") );
  CPPUNIT_ASSERT ( *from == *to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blue text on white background + dim + italic
  to->fg_color = fc::Blue;
  to->bg_color = fc::White;
  to->attr.bit.dim = true;
  to->attr.bit.italic = true;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0;1m\017$<2>") );

  // Reset attributes + default background
  to->attr.bit.bold = false;
  to->attr.bit.dim = false;
  to->attr.bit.italic = false;
  to->bg_color = fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>" ) );

  // Red text on black background
  to->fg_color = fc::Red;
  to->bg_color = fc::Black;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR("") );

  // 256 color text and background
  to->fg_color = fc::SpringGreen3;
  to->bg_color = fc::NavyBlue;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );


  // Bold on (with default colors)
  to->fg_color = fc::Default;
  to->bg_color = fc::Default;
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
  CPPUNIT_ASSERT ( from->code = ' ' );
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
                         , C_STR(CSI "0m$<2>") );
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
  to->fg_color = fc::Cyan;
  to->bg_color = fc::Blue;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );

  // Bold off
  to->attr.bit.bold = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>\016" CSI "4m$<2>"
                                 CSI "5m$<2>" CSI "7m$<2>" CSI "7m$<2>") );
  CPPUNIT_ASSERT ( *from != *to ) ;
  CPPUNIT_ASSERT ( from->fg_color == fc::Default );
  CPPUNIT_ASSERT ( from->bg_color == fc::Default );

  // Dim off
  to->attr.bit.dim = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>\016" CSI "4m$<2>"
                                 CSI "5m$<2>" CSI "7m$<2>" CSI "7m$<2>") );
  CPPUNIT_ASSERT ( *from != *to ) ;
  CPPUNIT_ASSERT ( from->fg_color == fc::Default );
  CPPUNIT_ASSERT ( from->bg_color == fc::Default );

  // Italic off
  to->attr.bit.italic = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT ( from->fg_color == fc::Default );
  CPPUNIT_ASSERT ( from->bg_color == fc::Default );

  // Underline off
  to->attr.bit.underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "m$<2>\016" CSI "5m$<2>"
                                 CSI "7m$<2>" CSI "7m$<2>") );
  CPPUNIT_ASSERT ( *from != *to ) ;
  CPPUNIT_ASSERT ( from->fg_color == fc::Default );
  CPPUNIT_ASSERT ( from->bg_color == fc::Default );

  // Blink off
  to->attr.bit.blink = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>\016" CSI "7m$<2>"
                                 CSI "7m$<2>") );
  CPPUNIT_ASSERT ( *from != *to ) ;
  CPPUNIT_ASSERT ( from->fg_color == fc::Default );
  CPPUNIT_ASSERT ( from->bg_color == fc::Default );

  // Reverse off
  to->attr.bit.reverse = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>\016" CSI "7m$<2>") );
  CPPUNIT_ASSERT ( *from != *to ) ;
  CPPUNIT_ASSERT ( from->fg_color == fc::Default );
  CPPUNIT_ASSERT ( from->bg_color == fc::Default );

  // Standout off
  to->attr.bit.standout = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "m$<2>\016") );
  CPPUNIT_ASSERT ( *from != *to ) ;
  CPPUNIT_ASSERT ( from->fg_color == fc::Default );
  CPPUNIT_ASSERT ( from->bg_color == fc::Default );

  // Invisible off
  to->attr.bit.invisible = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>\016") );
  CPPUNIT_ASSERT ( *from != *to ) ;
  CPPUNIT_ASSERT ( from->fg_color == fc::Default );
  CPPUNIT_ASSERT ( from->bg_color == fc::Default );

  // Protect off
  to->attr.bit.protect = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>\016") );
  CPPUNIT_ASSERT ( *from != *to ) ;
  CPPUNIT_ASSERT ( from->fg_color == fc::Default );
  CPPUNIT_ASSERT ( from->bg_color == fc::Default );

  // Crossed out off
  to->attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , C_STR(CSI "0m$<2>\016") );
  CPPUNIT_ASSERT ( *from != *to ) ;
  CPPUNIT_ASSERT ( from->fg_color == fc::Default );
  CPPUNIT_ASSERT ( from->bg_color == fc::Default );

  // Double underline off
  to->attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from != *to ) ;
  CPPUNIT_ASSERT ( from->fg_color == fc::Default );
  CPPUNIT_ASSERT ( from->bg_color == fc::Default );

  // Alternate character set off
  to->attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("\017") );
  CPPUNIT_ASSERT ( *from != *to ) ;
  CPPUNIT_ASSERT ( from->fg_color == fc::Default );
  CPPUNIT_ASSERT ( from->bg_color == fc::Default );

  // PC character set off
  to->attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR(CSI "0m$<2>") );
  CPPUNIT_ASSERT ( *from != *to ) ;
  CPPUNIT_ASSERT ( from->fg_color == fc::Default );
  CPPUNIT_ASSERT ( from->bg_color == fc::Default );

  // Green text color
  to->fg_color = fc::Green;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from != *to ) ;
  CPPUNIT_ASSERT ( from->fg_color == fc::Default );
  CPPUNIT_ASSERT ( from->bg_color == fc::Default );

  // Default text color
  to->fg_color = fc::Default;
  CPPUNIT_ASSERT ( *from != *to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), C_STR("") );
  CPPUNIT_ASSERT ( *from != *to ) ;
  CPPUNIT_ASSERT ( from->fg_color == fc::Default );
  CPPUNIT_ASSERT ( from->bg_color == fc::Default );

  delete to;
  delete from;
}

//----------------------------------------------------------------------
std::string FOptiAttrTest::printSequence (char* str)
{
  std::ostringstream sequence;

  if ( ! str )
    return "";

  const std::string& s(str);

  for (std::string::size_type i = 0; i < s.length(); ++i)
  {
    switch ( int(s[i]) )
    {
      case 0x08:
        sequence << "BS ";
        break;

      case 0x09:
        sequence << "TAB ";
        break;

      case 0x0a:
        sequence << "LF ";
        break;

      case 0x0d:
        sequence << "CR ";
        break;

      case 0x0e:
        sequence << "SO ";
        break;

      case 0x0f:
        sequence << "SI ";
        break;

      case 0x1b:
        sequence << "Esc ";
        break;

      default:
        sequence << s[i];
        sequence << ' ';
    }
  }

  return sequence.str();
}


// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FOptiAttrTest);

// The general unit test main part
#include <main-test.inc>
