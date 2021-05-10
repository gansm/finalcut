/***********************************************************************
* foptiattr_test.cpp - FOptiAttr unit tests                            *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2020 Markus Gans                                      *
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

class FOptiAttrTest : public CPPUNIT_NS::TestFixture
{
  public:
    FOptiAttrTest()
    { }

  protected:
    void classNameTest();
    void noArgumentTest();
    void vga2ansiTest();
    void sgrOptimizerTest();
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
    CPPUNIT_TEST (sgrOptimizerTest);
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


//----------------------------------------------------------------------
void FOptiAttrTest::classNameTest()
{
  finalcut::FOptiAttr opti_attr;
  const finalcut::FString& classname = opti_attr.getClassName();
  CPPUNIT_ASSERT ( classname == "FOptiAttr");
}

//----------------------------------------------------------------------
void FOptiAttrTest::noArgumentTest()
{
  finalcut::FChar ch{};
  finalcut::FOptiAttr oa;
  oa.initialize();

  // isNormal test
  CPPUNIT_ASSERT ( ! oa.isNormal(ch) );
  ch.fg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( ! oa.isNormal(ch) );
  ch.bg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( oa.isNormal(ch) );
}

//----------------------------------------------------------------------
void FOptiAttrTest::sgrOptimizerTest()
{
  // Test with FOptiAttr
  // -------------------
  finalcut::FStartOptions::getFStartOptions().sgr_optimizer = true;
  finalcut::FOptiAttr oa;
  oa.setDefaultColorSupport();  // ANSI default color
  oa.setMaxColor (8);
  oa.setNoColorVideo (3);       // Avoid standout (1) + underline mode (2)
  oa.set_enter_bold_mode (CSI "1m");
  oa.set_exit_bold_mode (CSI "22m");
  oa.set_enter_dim_mode (CSI "2m");
  oa.set_exit_dim_mode (CSI "22m");
  oa.set_enter_italics_mode (CSI "3m");
  oa.set_exit_italics_mode (CSI "23m");
  oa.set_enter_underline_mode  (0);
  oa.set_exit_underline_mode  (0);
  oa.set_enter_blink_mode (CSI "5m");
  oa.set_exit_blink_mode (CSI "25m");
  oa.set_enter_reverse_mode (CSI "7m");
  oa.set_exit_reverse_mode (CSI "27m");
  oa.set_enter_standout_mode  (0);
  oa.set_exit_standout_mode  (0);
  oa.set_enter_secure_mode (CSI "8m");
  oa.set_exit_secure_mode (CSI "28m");
  oa.set_enter_protected_mode (0);
  oa.set_exit_protected_mode (CSI "0m");
  oa.set_enter_crossed_out_mode (CSI "9m");
  oa.set_exit_crossed_out_mode (CSI "29m");
  oa.set_enter_dbl_underline_mode (CSI "21m");
  oa.set_exit_dbl_underline_mode (CSI "24m");
  oa.set_set_attributes (CSI "0;10"
                         "%?%p3%t;7%;"
                         "%?%p4%t;5%;"
                         "%?%p5%t;2%;"
                         "%?%p6%t;1%;"
                         "%?%p7%t;8%;"
                         "%?%p9%t;11%;m");
  oa.set_exit_attribute_mode (CSI "0m");
  oa.set_enter_alt_charset_mode (CSI "11m");
  oa.set_exit_alt_charset_mode (CSI "10m");
  oa.set_enter_pc_charset_mode (CSI "11m");
  oa.set_exit_pc_charset_mode (CSI "10m");
  oa.set_a_foreground_color (CSI "3%p1%dm");
  oa.set_a_background_color (CSI "4%p1%dm");
  oa.set_foreground_color (0);
  oa.set_background_color (0);
  oa.set_term_color_pair (0);
  oa.set_orig_pair (CSI "39;49m");
  oa.set_orig_orig_colors (0);
  oa.initialize();

  finalcut::FChar from{};
  finalcut::FChar to{};
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blue text on white background + bold + dim + italic
  to.fg_color = finalcut::FColor::Blue;
  to.bg_color = finalcut::FColor::White;
  to.attr.bit.bold = true;
  to.attr.bit.dim = true;
  to.attr.bit.italic = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10;2;1;3;34;47m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Yellow text on Black Yellow + bold
  to.fg_color = finalcut::FColor::Yellow;
  to.bg_color = finalcut::FColor::Black;
  to.attr.bit.bold = true;
  to.attr.bit.dim = false;
  to.attr.bit.italic = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10;1;33;40m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );


  // Test only the optimizer
  // -----------------------
  finalcut::SGRoptimizer::AttributeBuffer buffer = { CSI "0;10m" CSI "11m"  CSI "36m" CSI "44m" };
  finalcut::SGRoptimizer sgr_optimizer(buffer);
  sgr_optimizer.optimize();
  CPPUNIT_ASSERT_CSTRING ( buffer.data(), CSI "0;10;11;36;44m" );

  std::strcpy(buffer.data(), CSI "0;1m" CSI "34m");
  sgr_optimizer.optimize();
  CPPUNIT_ASSERT_CSTRING ( buffer.data(), CSI "0;1;34m" );

  std::strcpy(buffer.data(), CSI "m" CSI "34m");
  sgr_optimizer.optimize();
  CPPUNIT_ASSERT_CSTRING ( buffer.data(), CSI "0;34m" );

  std::strcpy(buffer.data(), CSI "1m" CSI "m" CSI "45m");
  sgr_optimizer.optimize();
  CPPUNIT_ASSERT_CSTRING ( buffer.data(), CSI "1;0;45m" );

  std::strcpy(buffer.data(), CSI "47m");
  sgr_optimizer.optimize();
  CPPUNIT_ASSERT_CSTRING ( buffer.data(), CSI "47m" );

  std::strcpy(buffer.data(), CSI "47m" CSI "m" CSI "1m");
  sgr_optimizer.optimize();
  CPPUNIT_ASSERT_CSTRING ( buffer.data(), CSI "47;0;1m" );

  std::strcpy(buffer.data(), CSI "49m" CSI "m" CSI "0m");
  sgr_optimizer.optimize();
  CPPUNIT_ASSERT_CSTRING ( buffer.data(), CSI "49;0;0m" );

  std::strcpy(buffer.data(), CSI "m" CSI "m" CSI "m");
  sgr_optimizer.optimize();
  CPPUNIT_ASSERT_CSTRING ( buffer.data(), CSI "0;0;0m" );

  std::strcpy(buffer.data(), CSI "m");
  sgr_optimizer.optimize();
  CPPUNIT_ASSERT_CSTRING ( buffer.data(), CSI "m" );

  std::strcpy(buffer.data(), CSI "0;10;1;7m" CSI "3m" CSI "39m" CSI "49m");
  sgr_optimizer.optimize();
  CPPUNIT_ASSERT_CSTRING ( buffer.data(), CSI "0;10;1;7;3;39;49m" );

  std::strcpy(buffer.data(), CSI "m" CSI "38;5;20m" CSI "48;5;229m");
  sgr_optimizer.optimize();
  CPPUNIT_ASSERT_CSTRING ( buffer.data(), CSI "0;38;5;20;48;5;229m" );

  std::strcpy(buffer.data(), CSI "m" CSI "38;5;20m" CSI "11;16H");
  sgr_optimizer.optimize();
  CPPUNIT_ASSERT_CSTRING ( buffer.data(), CSI "0;38;5;20m" CSI "11;16H" );

  std::strcpy(buffer.data(), CSI "1;1H" CSI "m" CSI "38;5;35m");
  sgr_optimizer.optimize();
  CPPUNIT_ASSERT_CSTRING ( buffer.data(), CSI "1;1H" CSI "0;38;5;35m" );

  std::strcpy(buffer.data(), CSI "m" CSI "38;5;20m" CSI "11;16H" CSI "48;5;229m");
  sgr_optimizer.optimize();
  CPPUNIT_ASSERT_CSTRING ( buffer.data(), CSI "0;38;5;20m" CSI "11;16H" CSI "48;5;229m" );

  std::strcpy(buffer.data(), CSI "m" CSI "38;5;20m" "ABC" CSI "48;5;229m");
  sgr_optimizer.optimize();
  CPPUNIT_ASSERT_CSTRING ( buffer.data(), CSI "0;38;5;20mABC" CSI "48;5;229m" );


  std::strcpy(buffer.data(), CSI "m" CSI "1m" CSI "2m" CSI "3m" CSI "4m"
                      CSI "5m" CSI "7m" CSI "8m" CSI "9m");
  sgr_optimizer.optimize();
  CPPUNIT_ASSERT_CSTRING ( buffer.data(), CSI "0;1;2;3;4;5;7;8;9m" );

  std::strcpy(buffer.data(), CSI "0m" CSI "46;36;1m");
  sgr_optimizer.optimize();
  CPPUNIT_ASSERT_CSTRING ( buffer.data(), CSI "0;46;36;1m" );

  std::strcpy(buffer.data(), CSI "m" CSI "38;2;0;139;139m" CSI "48;2;240;255;240m");
  sgr_optimizer.optimize();
  CPPUNIT_ASSERT_CSTRING ( buffer.data(), CSI "0;38;2;0;139;139;48;2;240;255;240m" );
}

//----------------------------------------------------------------------
void FOptiAttrTest::vga2ansiTest()
{
  finalcut::FOptiAttr oa;
  CPPUNIT_ASSERT (oa.vga2ansi(finalcut::FColor(0)) == finalcut::FColor(0));
  CPPUNIT_ASSERT (oa.vga2ansi(finalcut::FColor(1)) == finalcut::FColor(4));
  CPPUNIT_ASSERT (oa.vga2ansi(finalcut::FColor(2)) == finalcut::FColor(2));
  CPPUNIT_ASSERT (oa.vga2ansi(finalcut::FColor(3)) == finalcut::FColor(6));
  CPPUNIT_ASSERT (oa.vga2ansi(finalcut::FColor(4)) == finalcut::FColor(1));
  CPPUNIT_ASSERT (oa.vga2ansi(finalcut::FColor(5)) == finalcut::FColor(5));
  CPPUNIT_ASSERT (oa.vga2ansi(finalcut::FColor(6)) == finalcut::FColor(3));
  CPPUNIT_ASSERT (oa.vga2ansi(finalcut::FColor(7)) == finalcut::FColor(7));
  CPPUNIT_ASSERT (oa.vga2ansi(finalcut::FColor(8)) == finalcut::FColor(8));
  CPPUNIT_ASSERT (oa.vga2ansi(finalcut::FColor(9)) == finalcut::FColor(12));
  CPPUNIT_ASSERT (oa.vga2ansi(finalcut::FColor(10)) == finalcut::FColor(10));
  CPPUNIT_ASSERT (oa.vga2ansi(finalcut::FColor(11)) == finalcut::FColor(14));
  CPPUNIT_ASSERT (oa.vga2ansi(finalcut::FColor(12)) == finalcut::FColor(9));
  CPPUNIT_ASSERT (oa.vga2ansi(finalcut::FColor(13)) == finalcut::FColor(13));
  CPPUNIT_ASSERT (oa.vga2ansi(finalcut::FColor(14)) == finalcut::FColor(11));
  CPPUNIT_ASSERT (oa.vga2ansi(finalcut::FColor(15)) == finalcut::FColor(15));
}

//----------------------------------------------------------------------
void FOptiAttrTest::fakeReverseTest()
{
  finalcut::FStartOptions::getFStartOptions().sgr_optimizer = false;
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
  oa.set_a_foreground_color (CSI "3%p1%dm");
  oa.set_a_background_color (CSI "4%p1%dm");
  oa.set_foreground_color (0);
  oa.set_background_color (0);
  oa.set_term_color_pair (0);
  oa.set_orig_pair (0);
  oa.set_orig_orig_colors (0);
  oa.initialize();

  finalcut::FChar from{};
  finalcut::FChar to{};
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Gray text on blue background
  to.fg_color = finalcut::FColor::LightGray;
  to.bg_color = finalcut::FColor::Blue;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "37m" CSI "44m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse on
  to.attr.bit.reverse = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "34m" CSI "47m" );
  CPPUNIT_ASSERT ( from.fg_color == finalcut::FColor::LightGray );
  CPPUNIT_ASSERT ( from.bg_color == finalcut::FColor::Blue );
  CPPUNIT_ASSERT ( from == to );

  // Gray text on red background
  to.bg_color = finalcut::FColor::Red;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "31m" CSI "47m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off
  to.attr.bit.reverse = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "37m" CSI "41m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );
}

//----------------------------------------------------------------------
void FOptiAttrTest::ansiTest()
{
  // Simulate an ansi terminal

  finalcut::FStartOptions::getFStartOptions().sgr_optimizer = false;
  finalcut::FOptiAttr oa;
  oa.setDefaultColorSupport();  // ANSI default color
  oa.setMaxColor (8);
  oa.setNoColorVideo (3);       // Avoid standout (1) + underline mode (2)
  oa.set_enter_bold_mode (CSI "1m");
  oa.set_exit_bold_mode (CSI "0m");
  oa.set_enter_dim_mode (0);
  oa.set_exit_dim_mode (CSI "0m");
  oa.set_enter_italics_mode (0);
  oa.set_exit_italics_mode (0);
  oa.set_enter_underline_mode (CSI "4m");
  oa.set_exit_underline_mode (CSI "1m");
  oa.set_enter_blink_mode (CSI "5m");
  oa.set_exit_blink_mode (CSI "0m");
  oa.set_enter_reverse_mode (CSI "7m");
  oa.set_exit_reverse_mode (CSI "0m");
  oa.set_enter_standout_mode (CSI "7m");
  oa.set_exit_standout_mode (CSI "m");
  oa.set_enter_secure_mode (CSI "8m");
  oa.set_exit_secure_mode (CSI "0m");
  oa.set_enter_protected_mode (0);
  oa.set_exit_protected_mode (CSI "0m");
  oa.set_enter_crossed_out_mode (0);
  oa.set_exit_crossed_out_mode (CSI "0m");
  oa.set_enter_dbl_underline_mode (0);
  oa.set_exit_dbl_underline_mode (0);
  oa.set_set_attributes (CSI "0;10"
                         "%?%p1%t;7%;"
                         "%?%p2%t;4%;"
                         "%?%p3%t;7%;"
                         "%?%p4%t;5%;"
                         "%?%p6%t;1%;"
                         "%?%p7%t;8%;"
                         "%?%p9%t;11%;m");
  oa.set_exit_attribute_mode (CSI "0m");
  oa.set_enter_alt_charset_mode (CSI "11m");
  oa.set_exit_alt_charset_mode (CSI "10m");
  oa.set_enter_pc_charset_mode (CSI "11m");
  oa.set_exit_pc_charset_mode (CSI "10m");
  oa.set_a_foreground_color (CSI "3%p1%dm");
  oa.set_a_background_color (CSI "4%p1%dm");
  oa.set_foreground_color (0);
  oa.set_background_color (0);
  oa.set_term_color_pair (0);
  oa.set_orig_pair (CSI "39;49m");
  oa.set_orig_orig_colors (0);
  oa.initialize();

  finalcut::FChar from{};
  finalcut::FChar to{};
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default color + bold
  from.fg_color = finalcut::FColor::Default;
  from.bg_color = finalcut::FColor::Default;
  to.attr.bit.bold = true;
  to.fg_color = finalcut::FColor::Default;
  to.bg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10;1m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blue text on white background + dim + italic
  to.fg_color = finalcut::FColor::Blue;
  to.bg_color = finalcut::FColor::White;
  to.attr.bit.dim = true;
  to.attr.bit.italic = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10;1m" CSI "34m" CSI "47m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reset attributes + default background
  to.attr.bit.bold = false;
  to.attr.bit.dim = false;
  to.attr.bit.italic = false;
  to.bg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" CSI "34m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Red text on black background
  to.fg_color = finalcut::FColor::Red;
  to.bg_color = finalcut::FColor::Black;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "31m" CSI "40m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // 256 color text and background
  to.fg_color = finalcut::FColor::SpringGreen3;
  to.bg_color = finalcut::FColor::NavyBlue;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "32m" CSI "44m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold on (with default colors)
  to.fg_color = finalcut::FColor::Default;
  to.bg_color = finalcut::FColor::Default;
  to.attr.bit.bold = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10;1m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off (with default colors)
  to.attr.bit.bold = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim on (with default colors)
  to.attr.bit.dim = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off (with default colors)
  to.attr.bit.dim = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic on (with default colors)
  to.attr.bit.italic = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off (with default colors)
  to.attr.bit.italic = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline on (with default colors)
  to.attr.bit.underline = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10;4m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off (with default colors)
  to.attr.bit.underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink on (with default colors)
  to.attr.bit.blink = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10;5m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off (with default colors)
  to.attr.bit.blink = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse on (with default colors)
  to.attr.bit.reverse = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10;7m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off (with default colors)
  to.attr.bit.reverse = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout on (with default colors)
  to.attr.bit.standout = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10;7m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off (with default colors)
  to.attr.bit.standout = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible on (with default colors)
  to.attr.bit.invisible = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10;8m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off (with default colors)
  to.attr.bit.invisible = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect on (with default colors)
  to.attr.bit.protect = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off (with default colors)
  to.attr.bit.protect = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out on (with default colors)
  to.attr.bit.crossed_out = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off (with default colors)
  to.attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline on (with default colors)
  to.attr.bit.dbl_underline = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off (with default colors)
  to.attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set on (with default colors)
  to.attr.bit.alt_charset = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10;11m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off (with default colors)
  to.attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "10m" CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set on (with default colors)
  to.attr.bit.pc_charset = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10m" CSI "11m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off (with default colors)
  to.attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" CSI "10m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Turn on all attributes (with default colors)
  to.attr.bit.pc_charset    = true;
  to.attr.bit.bold          = true;
  to.attr.bit.dim           = true;
  to.attr.bit.italic        = true;
  to.attr.bit.underline     = true;
  to.attr.bit.blink         = true;
  to.attr.bit.reverse       = true;
  to.attr.bit.standout      = true;
  to.attr.bit.invisible     = true;
  to.attr.bit.protect       = true;
  to.attr.bit.crossed_out   = true;
  to.attr.bit.dbl_underline = true;
  to.attr.bit.alt_charset   = true;
  to.attr.bit.pc_charset    = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10;7;4;7;5;1;8;11m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Cyan text on blue background
  to.fg_color = finalcut::FColor::Cyan;
  to.bg_color = finalcut::FColor::Blue;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "36m" CSI "44m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off
  to.attr.bit.bold = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10;7;5;8;11m" CSI "36m" CSI "44m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off
  to.attr.bit.dim = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10;7;5;8;11m" CSI "36m" CSI "44m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off
  to.attr.bit.italic = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10;7;5;8;11m"
                           CSI "36m" CSI "44m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off
  to.attr.bit.underline = false;
  CPPUNIT_ASSERT ( from == to );  // because of noColorVideo = 3
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off
  to.attr.bit.blink = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10;7;8;11m" CSI "36m" CSI "44m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off
  to.attr.bit.reverse = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10;8;11m" CSI "36m" CSI "44m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off
  to.attr.bit.standout = false;
  CPPUNIT_ASSERT ( from == to );  // because of noColorVideo = 3
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off
  to.attr.bit.invisible = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10;11m" CSI "36m" CSI "44m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off
  to.attr.bit.protect = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10;11m" CSI "36m" CSI "44m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off
  to.attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10;11m" CSI "36m" CSI "44m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off
  to.attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10;11m" CSI "36m" CSI "44m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off
  to.attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;10m" CSI "11m"  CSI "36m" CSI "44m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off
  to.attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" CSI "10m" CSI "36m" CSI "44m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Green text color
  to.fg_color = finalcut::FColor::Green;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), CSI "32m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default text color
  to.fg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( printSequence(oa.changeAttribute(from, to)).c_str()
                         , "Esc [ 3 9 m " );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );
}

//----------------------------------------------------------------------
void FOptiAttrTest::vt100Test()
{
  // Simulate a vt100 terminal

  finalcut::FStartOptions::getFStartOptions().sgr_optimizer = false;
  finalcut::FOptiAttr oa;
  oa.unsetDefaultColorSupport();  // No ANSI default color
  oa.setMaxColor (1);
  oa.setNoColorVideo (0);
  oa.set_enter_bold_mode (CSI "1m$<2>");
  oa.set_exit_bold_mode (CSI "0m$<2>");
  oa.set_enter_dim_mode (0);
  oa.set_exit_dim_mode (CSI "0m$<2>");
  oa.set_enter_italics_mode (0);
  oa.set_exit_italics_mode (0);
  oa.set_enter_underline_mode (CSI "4m$<2>");
  oa.set_exit_underline_mode (CSI "m$<2>");
  oa.set_enter_blink_mode (CSI "5m$<2>");
  oa.set_exit_blink_mode (CSI "0m$<2>");
  oa.set_enter_reverse_mode (CSI "7m$<2>");
  oa.set_exit_reverse_mode (CSI "0m$<2>");
  oa.set_enter_standout_mode (CSI "7m$<2>");
  oa.set_exit_standout_mode (CSI "m$<2>");
  oa.set_enter_secure_mode (0);
  oa.set_exit_secure_mode (CSI "0m$<2>");
  oa.set_enter_protected_mode (0);
  oa.set_exit_protected_mode (CSI "0m$<2>");
  oa.set_enter_crossed_out_mode (0);
  oa.set_exit_crossed_out_mode (CSI "0m$<2>");
  oa.set_enter_dbl_underline_mode (0);
  oa.set_exit_dbl_underline_mode (0);
  oa.set_set_attributes (CSI "0"
                         "%?%p1%p6%|%t;1%;"
                         "%?%p2%t;4%;"
                         "%?%p1%p3%|%t;7%;"
                         "%?%p4%t;5%;m"
                         "%?%p9%t\016%e\017%;$<2>");
  oa.set_exit_attribute_mode (CSI "0m$<2>");
  oa.set_enter_alt_charset_mode ("\016");
  oa.set_exit_alt_charset_mode ("\017");
  oa.set_enter_pc_charset_mode (0);
  oa.set_exit_pc_charset_mode (0);
  oa.set_a_foreground_color (CSI "3%p1%dm");
  oa.set_a_background_color (CSI "4%p1%dm");
  oa.set_foreground_color (0);
  oa.set_background_color (0);
  oa.set_term_color_pair (0);
  oa.set_orig_pair (0);
  oa.set_orig_orig_colors (0);
  oa.initialize();

  finalcut::FChar from{};
  finalcut::FChar to{};
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default color + bold
  from.fg_color = finalcut::FColor::Default;
  from.bg_color = finalcut::FColor::Default;
  to.attr.bit.bold = true;
  to.fg_color = finalcut::FColor::Default;
  to.bg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;1m\017$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blue text on white background + dim + italic
  to.fg_color = finalcut::FColor::Blue;
  to.bg_color = finalcut::FColor::White;
  to.attr.bit.dim = true;
  to.attr.bit.italic = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;1m\017$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reset attributes + default background
  to.attr.bit.bold = false;
  to.attr.bit.dim = false;
  to.attr.bit.italic = false;
  to.bg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>"  );

  // Red text on black background
  to.fg_color = finalcut::FColor::Red;
  to.bg_color = finalcut::FColor::Black;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , "" );

  // 256 color text and background
  to.fg_color = finalcut::FColor::SpringGreen3;
  to.bg_color = finalcut::FColor::NavyBlue;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );


  // Bold on (with default colors)
  to.fg_color = finalcut::FColor::Default;
  to.bg_color = finalcut::FColor::Default;
  to.attr.bit.bold = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;1m\017$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off (with default colors)
  to.attr.bit.bold = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim on (with default colors)
  to.attr.bit.dim = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off (with default colors)
  to.attr.bit.dim = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic on (with default colors)
  to.attr.bit.italic = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off (with default colors)
  to.attr.bit.italic = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline on (with default colors)
  to.attr.bit.underline = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;4m\017$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off (with default colors)
  to.attr.bit.underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink on (with default colors)
  to.attr.bit.blink = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;5m\017$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off (with default colors)
  to.attr.bit.blink = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse on (with default colors)
  to.attr.bit.reverse = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;7m\017$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off (with default colors)
  to.attr.bit.reverse = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout on (with default colors)
  to.attr.bit.standout = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;1;7m\017$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off (with default colors)
  to.attr.bit.standout = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible on (with default colors)
  to.attr.bit.invisible = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( to.encoded_char[0] == ' ' );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off (with default colors)
  to.attr.bit.invisible = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect on (with default colors)
  to.attr.bit.protect = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off (with default colors)
  to.attr.bit.protect = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out on (with default colors)
  to.attr.bit.crossed_out = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off (with default colors)
  to.attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline on (with default colors)
  to.attr.bit.dbl_underline = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off (with default colors)
  to.attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set on (with default colors)
  to.attr.bit.alt_charset = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\016$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off (with default colors)
  to.attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , "\017" CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set on (with default colors)
  to.attr.bit.pc_charset = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off (with default colors)
  to.attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Turn on all attributes (with default colors)
  to.attr.bit.pc_charset    = true;
  to.attr.bit.bold          = true;
  to.attr.bit.dim           = true;
  to.attr.bit.italic        = true;
  to.attr.bit.underline     = true;
  to.attr.bit.blink         = true;
  to.attr.bit.reverse       = true;
  to.attr.bit.standout      = true;
  to.attr.bit.invisible     = true;
  to.attr.bit.protect       = true;
  to.attr.bit.crossed_out   = true;
  to.attr.bit.dbl_underline = true;
  to.attr.bit.alt_charset   = true;
  to.attr.bit.pc_charset    = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;1;4;7;5m\016$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Cyan text on blue background
  to.fg_color = finalcut::FColor::Cyan;
  to.bg_color = finalcut::FColor::Blue;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );

  // Bold off
  to.attr.bit.bold = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>\016" CSI "4m$<2>"
                           CSI "5m$<2>" CSI "7m$<2>" CSI "7m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off
  to.attr.bit.dim = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>\016" CSI "4m$<2>"
                           CSI "5m$<2>" CSI "7m$<2>" CSI "7m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off
  to.attr.bit.italic = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off
  to.attr.bit.underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "m$<2>\016" CSI "5m$<2>"
                           CSI "7m$<2>" CSI "7m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off
  to.attr.bit.blink = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>\016" CSI "7m$<2>" CSI "7m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off
  to.attr.bit.reverse = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>\016" CSI "7m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off
  to.attr.bit.standout = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "m$<2>\016" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off
  to.attr.bit.invisible = false;
  CPPUNIT_ASSERT ( from != to );

  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>\016" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off
  to.attr.bit.protect = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>\016" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off
  to.attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>\016" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off
  to.attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off
  to.attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off
  to.attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Green text color
  to.fg_color = finalcut::FColor::Green;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default text color
  to.fg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );
}

//----------------------------------------------------------------------
void FOptiAttrTest::xtermTest()
{
  // Simulate an xterm-256color terminal

  finalcut::FStartOptions::getFStartOptions().sgr_optimizer = false;
  finalcut::FOptiAttr oa;
  oa.setDefaultColorSupport();  // ANSI default color
  oa.setMaxColor (256);
  oa.setNoColorVideo (0);
  oa.set_enter_bold_mode (CSI "1m");
  oa.set_exit_bold_mode (CSI "22m");
  oa.set_enter_dim_mode (CSI "2m");
  oa.set_exit_dim_mode (CSI "22m");
  oa.set_enter_italics_mode (CSI "3m");
  oa.set_exit_italics_mode (CSI "23m");
  oa.set_enter_underline_mode (CSI "4m");
  oa.set_exit_underline_mode (CSI "24m");
  oa.set_enter_blink_mode (CSI "5m");
  oa.set_exit_blink_mode (CSI "25m");
  oa.set_enter_reverse_mode (CSI "7m");
  oa.set_exit_reverse_mode (CSI "27m");
  oa.set_enter_standout_mode (CSI "7m");
  oa.set_exit_standout_mode (CSI "27m");
  oa.set_enter_secure_mode (CSI "8m");
  oa.set_exit_secure_mode (CSI "28m");
  oa.set_enter_protected_mode (0);
  oa.set_exit_protected_mode (CSI "0m");
  oa.set_enter_crossed_out_mode (CSI "9m");
  oa.set_exit_crossed_out_mode (CSI "29m");
  oa.set_enter_dbl_underline_mode (CSI "21m");
  oa.set_exit_dbl_underline_mode (CSI "24m");
  oa.set_set_attributes ("%?%p9%t" ESC "(0"
                              "%e" ESC "(B%;" CSI "0"
                         "%?%p6%t;1%;"
                         "%?%p5%t;2%;"
                         "%?%p2%t;4%;"
                         "%?%p1%p3%|%t;7%;"
                         "%?%p4%t;5%;"
                         "%?%p7%t;8%;m");
  oa.set_exit_attribute_mode (CSI "0m");
  oa.set_enter_alt_charset_mode (ESC "(0");
  oa.set_exit_alt_charset_mode (ESC "(B");
  oa.set_enter_pc_charset_mode (0);
  oa.set_exit_pc_charset_mode (0);
  oa.set_a_foreground_color (CSI "%?%p1%{8}%<"
                                 "%t3%p1%d"
                                 "%e%p1%{16}%<"
                                 "%t9%p1%{8}%-%d"
                                 "%e38;5;%p1%d%;m");
  oa.set_a_background_color (CSI "%?%p1%{8}%<"
                                 "%t4%p1%d"
                                 "%e%p1%{16}%<"
                                 "%t10%p1%{8}%-%d"
                                 "%e48;5;%p1%d%;m");
  oa.set_foreground_color (0);
  oa.set_background_color (0);
  oa.set_term_color_pair (0);
  oa.set_orig_pair (CSI "39;49m");
  oa.set_orig_orig_colors (0);
  oa.initialize();

  finalcut::FChar from{};
  finalcut::FChar to{};
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default color + bold
  from.fg_color = finalcut::FColor::Default;
  from.bg_color = finalcut::FColor::Default;
  to.attr.bit.bold = true;
  to.fg_color = finalcut::FColor::Default;
  to.bg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(B" CSI "0;1m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blue text on white background + dim + italic
  to.fg_color = finalcut::FColor::Blue;
  to.bg_color = finalcut::FColor::White;
  to.attr.bit.dim = true;
  to.attr.bit.italic = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(B" CSI "0;1;2m" CSI "3m"
                           CSI "34m" CSI "107m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reset attributes + default background
  to.attr.bit.bold = false;
  to.attr.bit.dim = false;
  to.attr.bit.italic = false;
  to.bg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" CSI "34m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Red text on black background
  to.fg_color = finalcut::FColor::Red;
  to.bg_color = finalcut::FColor::Black;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "31m" CSI "40m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // 256 color text and background
  to.fg_color = finalcut::FColor::SpringGreen3;
  to.bg_color = finalcut::FColor::NavyBlue;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "38;5;42m" CSI "48;5;17m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold on (with default colors)
  to.fg_color = finalcut::FColor::Default;
  to.bg_color = finalcut::FColor::Default;
  to.attr.bit.bold = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(B" CSI "0;1m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off (with default colors)
  to.attr.bit.bold = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim on (with default colors)
  to.attr.bit.dim = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(B" CSI "0;2m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off (with default colors)
  to.attr.bit.dim = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic on (with default colors)
  to.attr.bit.italic = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(B" CSI "0m" CSI "3m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off (with default colors)
  to.attr.bit.italic = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline on (with default colors)
  to.attr.bit.underline = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(B" CSI "0;4m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off (with default colors)
  to.attr.bit.underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink on (with default colors)
  to.attr.bit.blink = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(B" CSI "0;5m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off (with default colors)
  to.attr.bit.blink = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse on (with default colors)
  to.attr.bit.reverse = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(B" CSI "0;7m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off (with default colors)
  to.attr.bit.reverse = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout on (with default colors)
  to.attr.bit.standout = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(B" CSI "0;7m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off (with default colors)
  to.attr.bit.standout = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible on (with default colors)
  to.attr.bit.invisible = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(B" CSI "0;8m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off (with default colors)
  to.attr.bit.invisible = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect on (with default colors)
  to.attr.bit.protect = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(B" CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off (with default colors)
  to.attr.bit.protect = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out on (with default colors)
  to.attr.bit.crossed_out = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(B" CSI "0m" CSI "9m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off (with default colors)
  to.attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline on (with default colors)
  to.attr.bit.dbl_underline = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(B" CSI "0m" CSI "21m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off (with default colors)
  to.attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set on (with default colors)
  to.attr.bit.alt_charset = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(0" CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off (with default colors)
  to.attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(B" CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set on (with default colors)
  to.attr.bit.pc_charset = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(B" CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off (with default colors)
  to.attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Turn on all attributes (with default colors)
  to.attr.bit.pc_charset    = true;
  to.attr.bit.bold          = true;
  to.attr.bit.dim           = true;
  to.attr.bit.italic        = true;
  to.attr.bit.underline     = true;
  to.attr.bit.blink         = true;
  to.attr.bit.reverse       = true;
  to.attr.bit.standout      = true;
  to.attr.bit.invisible     = true;
  to.attr.bit.protect       = true;
  to.attr.bit.crossed_out   = true;
  to.attr.bit.dbl_underline = true;
  to.attr.bit.alt_charset   = true;
  to.attr.bit.pc_charset    = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(0" CSI "0;1;2;4;7;5;8m" CSI "3m"
                           CSI "9m" CSI "21m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Cyan text on blue background
  to.fg_color = finalcut::FColor::Cyan;
  to.bg_color = finalcut::FColor::Blue;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "36m" CSI "44m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off
  to.attr.bit.bold = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "22m" CSI "2m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off
  to.attr.bit.dim = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "22m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off
  to.attr.bit.italic = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "23m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off
  to.attr.bit.underline = false;
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "24m" CSI "21m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );


  // Blink off
  to.attr.bit.blink = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "25m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off
  to.attr.bit.reverse = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "27m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off
  to.attr.bit.standout = false;
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "27m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off
  to.attr.bit.invisible = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "28m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off
  to.attr.bit.protect = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" CSI "36m" CSI "44m" ESC "(0" CSI "9m" CSI "21m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off
  to.attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "29m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off
  to.attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "24m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off
  to.attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(B" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off
  to.attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" CSI "36m" CSI "44m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Green text color
  to.fg_color = finalcut::FColor::Green;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), CSI "32m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default text color
  to.fg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( printSequence(oa.changeAttribute(from, to)).c_str()
                         , "Esc [ 3 9 m " );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );
}

//----------------------------------------------------------------------
void FOptiAttrTest::rxvtTest()
{
  // Simulate an rxvt terminal

  finalcut::FStartOptions::getFStartOptions().sgr_optimizer = false;
  finalcut::FOptiAttr oa;
  oa.setDefaultColorSupport();  // ANSI default color
  oa.setMaxColor (8);
  oa.setNoColorVideo (0);
  oa.set_enter_bold_mode (CSI "1m");
  oa.set_exit_bold_mode (CSI "22m");
  oa.set_enter_dim_mode (0);
  oa.set_exit_dim_mode (CSI "22m");
  oa.set_enter_italics_mode (0);
  oa.set_exit_italics_mode (0);
  oa.set_enter_underline_mode (CSI "4m");
  oa.set_exit_underline_mode (CSI "24m");
  oa.set_enter_blink_mode (CSI "5m");
  oa.set_exit_blink_mode (CSI "25m");
  oa.set_enter_reverse_mode (CSI "7m");
  oa.set_exit_reverse_mode (CSI "27m");
  oa.set_enter_standout_mode (CSI "7m");
  oa.set_exit_standout_mode (CSI "27m");
  oa.set_enter_secure_mode (0);
  oa.set_exit_secure_mode (CSI "28m");
  oa.set_enter_protected_mode (0);
  oa.set_exit_protected_mode (CSI "0m");
  oa.set_enter_crossed_out_mode (CSI "9m");
  oa.set_exit_crossed_out_mode (CSI "29m");
  oa.set_enter_dbl_underline_mode (CSI "21m");
  oa.set_exit_dbl_underline_mode (CSI "24m");
  oa.set_set_attributes (CSI "0"
                         "%?%p6%t;1%;"
                         "%?%p2%t;4%;"
                         "%?%p1%p3%|%t;7%;"
                         "%?%p4%t;5%;m"
                         "%?%p9%t\016%e\017%;");
  oa.set_exit_attribute_mode (CSI "0m");
  oa.set_enter_alt_charset_mode ("\016");
  oa.set_exit_alt_charset_mode ("\017");
  oa.set_enter_pc_charset_mode (0);
  oa.set_exit_pc_charset_mode (0);
  oa.set_a_foreground_color (CSI "3%p1%dm");
  oa.set_a_background_color (CSI "4%p1%dm");
  oa.set_foreground_color (0);
  oa.set_background_color (0);
  oa.set_term_color_pair (0);
  oa.set_orig_pair (CSI "39;49m");
  oa.set_orig_orig_colors (0);
  oa.initialize();

  finalcut::FChar from{};
  finalcut::FChar to{};
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default color + bold
  from.fg_color = finalcut::FColor::Default;
  from.bg_color = finalcut::FColor::Default;
  to.attr.bit.bold = true;
  to.fg_color = finalcut::FColor::Default;
  to.bg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;1m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blue text on white background + dim + italic
  to.fg_color = finalcut::FColor::Blue;
  to.bg_color = finalcut::FColor::White;
  to.attr.bit.dim = true;
  to.attr.bit.italic = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;1m\017" CSI "34m" CSI "47m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reset attributes + default background
  to.attr.bit.bold = false;
  to.attr.bit.dim = false;
  to.attr.bit.italic = false;
  to.bg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" CSI "34m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Red text on black background
  to.fg_color = finalcut::FColor::Red;
  to.bg_color = finalcut::FColor::Black;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "31m" CSI "40m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // 256 color text and background
  to.fg_color = finalcut::FColor::SpringGreen3;
  to.bg_color = finalcut::FColor::NavyBlue;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "32m" CSI "44m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold on (with default colors)
  to.fg_color = finalcut::FColor::Default;
  to.bg_color = finalcut::FColor::Default;
  to.attr.bit.bold = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;1m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off (with default colors)
  to.attr.bit.bold = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim on (with default colors)
  to.attr.bit.dim = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off (with default colors)
  to.attr.bit.dim = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic on (with default colors)
  to.attr.bit.italic = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off (with default colors)
  to.attr.bit.italic = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline on (with default colors)
  to.attr.bit.underline = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;4m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off (with default colors)
  to.attr.bit.underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink on (with default colors)
  to.attr.bit.blink = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;5m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off (with default colors)
  to.attr.bit.blink = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse on (with default colors)
  to.attr.bit.reverse = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;7m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off (with default colors)
  to.attr.bit.reverse = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout on (with default colors)
  to.attr.bit.standout = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;7m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off (with default colors)
  to.attr.bit.standout = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible on (with default colors)
  to.attr.bit.invisible = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( to.encoded_char[0] == ' ' );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off (with default colors)
  to.attr.bit.invisible = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect on (with default colors)
  to.attr.bit.protect = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off (with default colors)
  to.attr.bit.protect = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out on (with default colors)
  to.attr.bit.crossed_out = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" CSI "9m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off (with default colors)
  to.attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline on (with default colors)
  to.attr.bit.dbl_underline = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" CSI "21m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off (with default colors)
  to.attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set on (with default colors)
  to.attr.bit.alt_charset = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\016" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off (with default colors)
  to.attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , "\017" CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set on (with default colors)
  to.attr.bit.pc_charset = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off (with default colors)
  to.attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Turn on all attributes (with default colors)
  to.attr.bit.pc_charset    = true;
  to.attr.bit.bold          = true;
  to.attr.bit.dim           = true;
  to.attr.bit.italic        = true;
  to.attr.bit.underline     = true;
  to.attr.bit.blink         = true;
  to.attr.bit.reverse       = true;
  to.attr.bit.standout      = true;
  to.attr.bit.invisible     = true;
  to.attr.bit.protect       = true;
  to.attr.bit.crossed_out   = true;
  to.attr.bit.dbl_underline = true;
  to.attr.bit.alt_charset   = true;
  to.attr.bit.pc_charset    = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;1;4;7;5m\016"
                           CSI "9m" CSI "21m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Cyan text on blue background
  to.fg_color = finalcut::FColor::Cyan;
  to.bg_color = finalcut::FColor::Blue;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "36m" CSI "44m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off
  to.attr.bit.bold = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "22m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off
  to.attr.bit.dim = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "22m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off
  to.attr.bit.italic = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off
  to.attr.bit.underline = false;
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "24m" CSI "21m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );


  // Blink off
  to.attr.bit.blink = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "25m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off
  to.attr.bit.reverse = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "27m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off
  to.attr.bit.standout = false;
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "27m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off
  to.attr.bit.invisible = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "28m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off
  to.attr.bit.protect = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" CSI "36m" CSI "44m\016" CSI "9m" CSI "21m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off
  to.attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "29m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off
  to.attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "24m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off
  to.attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , "\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off
  to.attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" CSI "36m" CSI "44m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Green text color
  to.fg_color = finalcut::FColor::Green;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), CSI "32m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default text color
  to.fg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( printSequence(oa.changeAttribute(from, to)).c_str()
                         , "Esc [ 3 9 m " );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );
}

//----------------------------------------------------------------------
void FOptiAttrTest::linuxTest()
{
  // Simulate a Linux terminal with 16 colors

  finalcut::FStartOptions::getFStartOptions().sgr_optimizer = false;
  finalcut::FOptiAttr oa;
  oa.setDefaultColorSupport();  // ANSI default color
  oa.setMaxColor (16);
  oa.setNoColorVideo (18);
  oa.set_enter_bold_mode (CSI "1m");
  oa.set_exit_bold_mode (CSI "22m");
  oa.set_enter_dim_mode (0);
  oa.set_exit_dim_mode (0);
  oa.set_enter_italics_mode (0);
  oa.set_exit_italics_mode (0);
  oa.set_enter_underline_mode (0);
  oa.set_exit_underline_mode (0);
  oa.set_enter_blink_mode (CSI "5m");
  oa.set_exit_blink_mode (CSI "25m");
  oa.set_enter_reverse_mode (CSI "7m");
  oa.set_exit_reverse_mode (CSI "27m");
  oa.set_enter_standout_mode (CSI "7m");
  oa.set_exit_standout_mode (CSI "27m");
  oa.set_enter_secure_mode (0);
  oa.set_exit_secure_mode (0);
  oa.set_enter_protected_mode (0);
  oa.set_exit_protected_mode (0);
  oa.set_enter_crossed_out_mode (0);
  oa.set_exit_crossed_out_mode (0);
  oa.set_enter_dbl_underline_mode (0);
  oa.set_exit_dbl_underline_mode (0);
  oa.set_set_attributes (CSI "0"
                         "%?%p6%|%t;1%;"
                         "%?%p1%p3%|%t;7%;"
                         "%?%p4%t;5%;m"
                         "%?%p9%t\016%e\017%;");
  oa.set_exit_attribute_mode (CSI "0m\017");
  oa.set_enter_alt_charset_mode ("\016");
  oa.set_exit_alt_charset_mode ("\017");
  oa.set_enter_pc_charset_mode (CSI "11m");
  oa.set_exit_pc_charset_mode (CSI "10m");
  oa.set_a_foreground_color (CSI "3%p1%{8}%m%d"
                             "%?%p1%{7}%>%t;1%e;22%;m");
  oa.set_a_background_color (CSI "4%p1%{8}%m%d"
                             "%?%p1%{7}%>%t;5%e;25%;m");
  oa.set_foreground_color (0);
  oa.set_background_color (0);
  oa.set_term_color_pair (0);
  oa.set_orig_pair (CSI "39;49;25m");
  oa.set_orig_orig_colors (OSC "R");
  oa.initialize();

  finalcut::FChar from{};
  finalcut::FChar to{};
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default color + bold
  from.fg_color = finalcut::FColor::Default;
  from.bg_color = finalcut::FColor::Default;
  to.attr.bit.bold = true;
  to.fg_color = finalcut::FColor::Default;
  to.bg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;1m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blue text on white background + dim + italic
  to.fg_color = finalcut::FColor::Blue;
  to.bg_color = finalcut::FColor::White;
  to.attr.bit.dim = true;
  to.attr.bit.italic = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;1m\017" CSI "34;22m" CSI "47;5m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reset attributes + default background
  to.attr.bit.bold = false;
  to.attr.bit.dim = false;
  to.attr.bit.italic = false;
  to.bg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" CSI "34;22m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Red text on black background
  to.fg_color = finalcut::FColor::Red;
  to.bg_color = finalcut::FColor::Black;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "31;22m" CSI "40;25m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // 256 color text and background
  to.fg_color = finalcut::FColor::SpringGreen3;
  to.bg_color = finalcut::FColor::NavyBlue;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "32;1m" CSI "44;25m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold on (with default colors)
  to.fg_color = finalcut::FColor::Default;
  to.bg_color = finalcut::FColor::Default;
  to.attr.bit.bold = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;1m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off (with default colors)
  to.attr.bit.bold = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim on (with default colors)
  to.attr.bit.dim = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off (with default colors)
  to.attr.bit.dim = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic on (with default colors)
  to.attr.bit.italic = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off (with default colors)
  to.attr.bit.italic = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline on (with default colors)
  to.attr.bit.underline = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off (with default colors)
  to.attr.bit.underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink on (with default colors)
  to.attr.bit.blink = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;5m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off (with default colors)
  to.attr.bit.blink = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse on (with default colors)
  to.attr.bit.reverse = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;7m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off (with default colors)
  to.attr.bit.reverse = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout on (with default colors)
  to.attr.bit.standout = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;7m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off (with default colors)
  to.attr.bit.standout = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible on (with default colors)
  to.attr.bit.invisible = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( to.encoded_char[0] == ' ' );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off (with default colors)
  to.attr.bit.invisible = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\17" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect on (with default colors)
  to.attr.bit.protect = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off (with default colors)
  to.attr.bit.protect = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out on (with default colors)
  to.attr.bit.crossed_out = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off (with default colors)
  to.attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline on (with default colors)
  to.attr.bit.dbl_underline = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off (with default colors)
  to.attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set on (with default colors)
  to.attr.bit.alt_charset = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\016" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off (with default colors)
  to.attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , "\017" CSI "0m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set on (with default colors)
  to.attr.bit.pc_charset = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" CSI "11m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off (with default colors)
  to.attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" CSI "10m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Turn on all attributes (with default colors)
  to.attr.bit.pc_charset    = true;
  to.attr.bit.bold          = true;
  to.attr.bit.dim           = true;
  to.attr.bit.italic        = true;
  to.attr.bit.underline     = true;
  to.attr.bit.blink         = true;
  to.attr.bit.reverse       = true;
  to.attr.bit.standout      = true;
  to.attr.bit.invisible     = true;
  to.attr.bit.protect       = true;
  to.attr.bit.crossed_out   = true;
  to.attr.bit.dbl_underline = true;
  to.attr.bit.alt_charset   = true;
  to.attr.bit.pc_charset    = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;1;7;5m\016"
                           CSI "11m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Cyan text on blue background
  to.fg_color = finalcut::FColor::Cyan;
  to.bg_color = finalcut::FColor::Blue;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "36;22m" CSI "44;25m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off
  to.attr.bit.bold = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "22m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off
  to.attr.bit.dim = false;
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off
  to.attr.bit.italic = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off
  to.attr.bit.underline = false;
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off
  to.attr.bit.blink = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "25m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off
  to.attr.bit.reverse = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "27m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off
  to.attr.bit.standout = false;
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "27m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off
  to.attr.bit.invisible = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off
  to.attr.bit.protect = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off
  to.attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off
  to.attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off
  to.attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , "\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off
  to.attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" CSI "10m" CSI "36;22m" CSI "44;25m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Green text color
  to.fg_color = finalcut::FColor::Green;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), CSI "32;22m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default text color
  to.fg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( printSequence(oa.changeAttribute(from, to)).c_str()
                         , "Esc [ 3 9 m " );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );
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

  finalcut::FStartOptions::getFStartOptions().sgr_optimizer = false;
  finalcut::FOptiAttr oa;
  oa.unsetDefaultColorSupport();  // No ANSI default color
  oa.setMaxColor (256);
  oa.setNoColorVideo (0);
  oa.set_enter_bold_mode (CSI "1m");
  oa.set_exit_bold_mode (CSI "22m");
  oa.set_enter_dim_mode (CSI "2m");
  oa.set_exit_dim_mode (CSI "22m");
  oa.set_enter_italics_mode (0);
  oa.set_exit_italics_mode (0);
  oa.set_enter_underline_mode (CSI "4m");
  oa.set_exit_underline_mode (CSI "24m");
  oa.set_enter_blink_mode (CSI "5m");
  oa.set_exit_blink_mode (CSI "25m");
  oa.set_enter_reverse_mode (CSI "7m");
  oa.set_exit_reverse_mode (CSI "27m");
  oa.set_enter_standout_mode (CSI "7m");
  oa.set_exit_standout_mode (CSI "27m");
  oa.set_enter_secure_mode (0);
  oa.set_exit_secure_mode (CSI "28m");
  oa.set_enter_protected_mode (0);
  oa.set_exit_protected_mode (CSI "0m");
  oa.set_enter_crossed_out_mode (CSI "9m");
  oa.set_exit_crossed_out_mode (CSI "29m");
  oa.set_enter_dbl_underline_mode (CSI "21m");
  oa.set_exit_dbl_underline_mode (CSI "24m");
  oa.set_set_attributes (CSI "0"
                         "%?%p1%p6%|%t;1%;"
                         "%?%p5%t;2%;"
                         "%?%p2%t;4%;"
                         "%?%p1%p3%|%t;7%;"
                         "%?%p4%t;5%;m"
                         "%?%p9%t\016%e\017%;");
  oa.set_exit_attribute_mode (CSI "0m");
  oa.set_enter_alt_charset_mode ("\016");
  oa.set_exit_alt_charset_mode ("\017");
  oa.set_enter_pc_charset_mode (CSI "11m");
  oa.set_exit_pc_charset_mode (CSI "10m");
  oa.set_a_foreground_color (CSI "%?%p1%{8}%<"
                                 "%t3%p1%d"
                                 "%e%p1%{16}%<"
                                 "%t9%p1%{8}%-%d"
                                 "%e38;5;%p1%d%;m");
  oa.set_a_background_color (CSI "%?%p1%{8}%<"
                                 "%t4%p1%d"
                                 "%e%p1%{16}%<"
                                 "%t10%p1%{8}%-%d"
                                 "%e48;5;%p1%d%;m");
  oa.set_foreground_color (0);
  oa.set_background_color (0);
  oa.set_term_color_pair (0);
  oa.set_orig_pair (CSI "39;49m");
  oa.set_orig_orig_colors (OSC "R");
  oa.initialize();


  finalcut::FChar from{};
  finalcut::FChar to{};
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default color + bold
  from.fg_color = finalcut::FColor::Default;
  from.bg_color = finalcut::FColor::Default;
  to.attr.bit.bold = true;
  to.fg_color = finalcut::FColor::Default;
  to.bg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;1m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blue text on white background + dim + italic
  to.fg_color = finalcut::FColor::Blue;
  to.bg_color = finalcut::FColor::White;
  to.attr.bit.dim = true;
  to.attr.bit.italic = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;1;2m\017" CSI "34m" CSI "107m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reset attributes + default background
  to.attr.bit.bold = false;
  to.attr.bit.dim = false;
  to.attr.bit.italic = false;
  to.bg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" CSI "39;49m" CSI "34m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Red text on black background
  to.fg_color = finalcut::FColor::Red;
  to.bg_color = finalcut::FColor::Black;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "31m" CSI "40m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // 256 color text and background
  to.fg_color = finalcut::FColor::SpringGreen3;
  to.bg_color = finalcut::FColor::NavyBlue;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "38;5;42m" CSI "48;5;17m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold on (with default colors)
  to.fg_color = finalcut::FColor::Default;
  to.bg_color = finalcut::FColor::Default;
  to.attr.bit.bold = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;1m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off (with default colors)
  to.attr.bit.bold = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim on (with default colors)
  to.attr.bit.dim = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;2m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off (with default colors)
  to.attr.bit.dim = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic on (with default colors)
  to.attr.bit.italic = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off (with default colors)
  to.attr.bit.italic = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline on (with default colors)
  to.attr.bit.underline = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;4m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off (with default colors)
  to.attr.bit.underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink on (with default colors)
  to.attr.bit.blink = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;5m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off (with default colors)
  to.attr.bit.blink = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse on (with default colors)
  to.attr.bit.reverse = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;7m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off (with default colors)
  to.attr.bit.reverse = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout on (with default colors)
  to.attr.bit.standout = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;1;7m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off (with default colors)
  to.attr.bit.standout = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible on (with default colors)
  to.attr.bit.invisible = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( to.encoded_char[0] == ' ' );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off (with default colors)
  to.attr.bit.invisible = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect on (with default colors)
  to.attr.bit.protect = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off (with default colors)
  to.attr.bit.protect = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out on (with default colors)
  to.attr.bit.crossed_out = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" CSI "9m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off (with default colors)
  to.attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline on (with default colors)
  to.attr.bit.dbl_underline = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" CSI "21m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off (with default colors)
  to.attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set on (with default colors)
  to.attr.bit.alt_charset = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\016" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off (with default colors)
  to.attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , "\017" CSI "0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set on (with default colors)
  to.attr.bit.pc_charset = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017" CSI "11m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off (with default colors)
  to.attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" CSI "10m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Turn on all attributes (with default colors)
  to.attr.bit.pc_charset    = true;
  to.attr.bit.bold          = true;
  to.attr.bit.dim           = true;
  to.attr.bit.italic        = true;
  to.attr.bit.underline     = true;
  to.attr.bit.blink         = true;
  to.attr.bit.reverse       = true;
  to.attr.bit.standout      = true;
  to.attr.bit.invisible     = true;
  to.attr.bit.protect       = true;
  to.attr.bit.crossed_out   = true;
  to.attr.bit.dbl_underline = true;
  to.attr.bit.alt_charset   = true;
  to.attr.bit.pc_charset    = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;1;2;4;7;5m\016"
                           CSI "9m" CSI "21m" CSI "11m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Cyan text on blue background
  to.fg_color = finalcut::FColor::Cyan;
  to.bg_color = finalcut::FColor::Blue;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "36m" CSI "44m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off
  to.attr.bit.bold = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "22m" CSI "2m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off
  to.attr.bit.dim = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "22m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off
  to.attr.bit.italic = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off
  to.attr.bit.underline = false;
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "24m" CSI "21m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );


  // Blink off
  to.attr.bit.blink = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "25m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off
  to.attr.bit.reverse = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "27m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off
  to.attr.bit.standout = false;
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "27m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off
  to.attr.bit.invisible = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "28m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off
  to.attr.bit.protect = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" CSI "36m" CSI "44m" "\016"
                           CSI "11m" CSI "9m" CSI "21m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off
  to.attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "29m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off
  to.attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "24m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off
  to.attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , "\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off
  to.attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m" CSI "10m" CSI "36m" CSI "44m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Green text color
  to.fg_color = finalcut::FColor::Green;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), CSI "32m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default text color
  to.fg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( printSequence(oa.changeAttribute(from, to)).c_str()
                         , "Esc [ 3 9 ; 4 9 m Esc [ 4 4 m " );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );
}

//----------------------------------------------------------------------
void FOptiAttrTest::teratermTest()
{
  // Simulate a Tera Term terminal

  finalcut::FStartOptions::getFStartOptions().sgr_optimizer = false;
  finalcut::FOptiAttr oa;
  oa.unsetDefaultColorSupport();  // No ANSI default color
  oa.setMaxColor (16);
  oa.setNoColorVideo (41);  // Avoid standout (1) + blink (8) + bold (32)
  oa.set_enter_bold_mode (CSI "1m");
  oa.set_exit_bold_mode (CSI "22m");
  oa.set_enter_dim_mode (0);
  oa.set_exit_dim_mode (CSI "22m");
  oa.set_enter_italics_mode (0);
  oa.set_exit_italics_mode (0);
  oa.set_enter_underline_mode (CSI "4m");
  oa.set_exit_underline_mode (CSI "24m");
  oa.set_enter_blink_mode (CSI "5m");
  oa.set_exit_blink_mode (CSI "25m");
  oa.set_enter_reverse_mode (CSI "7m");
  oa.set_exit_reverse_mode (CSI "27m");
  oa.set_enter_standout_mode (CSI "7m");
  oa.set_exit_standout_mode (CSI "27m");
  oa.set_enter_secure_mode (0);
  oa.set_exit_secure_mode (CSI "28m");
  oa.set_enter_protected_mode (0);
  oa.set_exit_protected_mode (CSI "0m$<2>");
  oa.set_enter_crossed_out_mode (CSI "9m");
  oa.set_exit_crossed_out_mode (CSI "29m");
  oa.set_enter_dbl_underline_mode (CSI "21m");
  oa.set_exit_dbl_underline_mode (CSI "24m");
  oa.set_set_attributes (CSI "0"
                         "%?%p1%p6%|%t;1%;"
                         "%?%p2%t;4%;"
                         "%?%p1%p3%|%t;7%;"
                         "%?%p4%t;5%;m"
                         "%?%p9%t\016%e\017%;$<2>");
  oa.set_exit_attribute_mode (CSI "0m$<2>");
  oa.set_enter_alt_charset_mode ("\016");
  oa.set_exit_alt_charset_mode ("\017");
  oa.set_enter_pc_charset_mode (0);
  oa.set_exit_pc_charset_mode (0);
  oa.set_a_foreground_color (CSI "38;5;%p1%dm");
  oa.set_a_background_color (CSI "48;5;%p1%dm");
  oa.set_foreground_color (0);
  oa.set_background_color (0);
  oa.set_term_color_pair (0);
  oa.set_orig_pair (CSI "39;49m");
  oa.set_orig_orig_colors (0);
  oa.initialize();


  finalcut::FChar from{};
  finalcut::FChar to{};
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default color + bold
  from.fg_color = finalcut::FColor::Default;
  from.bg_color = finalcut::FColor::Default;
  to.attr.bit.bold = true;
  to.fg_color = finalcut::FColor::Default;
  to.bg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;1m\017$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blue text on white background + dim + italic
  to.fg_color = finalcut::FColor::Blue;
  to.bg_color = finalcut::FColor::White;
  to.attr.bit.dim = true;
  to.attr.bit.italic = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017$<2>" CSI "38;5;4m"
                           CSI "48;5;15m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reset attributes + default background
  to.attr.bit.bold = false;
  to.attr.bit.dim = false;
  to.attr.bit.italic = false;
  to.bg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" CSI "39;49m" CSI "38;5;4m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Red text on black background
  to.fg_color = finalcut::FColor::Red;
  to.bg_color = finalcut::FColor::Black;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "38;5;1m" CSI "48;5;0m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // 256 color text and background
  to.fg_color = finalcut::FColor::SpringGreen3;
  to.bg_color = finalcut::FColor::NavyBlue;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "38;5;10m" CSI "48;5;4m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold on (with default colors)
  to.fg_color = finalcut::FColor::Default;
  to.bg_color = finalcut::FColor::Default;
  to.attr.bit.bold = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;1m\017$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off (with default colors)
  to.attr.bit.bold = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim on (with default colors)
  to.attr.bit.dim = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off (with default colors)
  to.attr.bit.dim = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic on (with default colors)
  to.attr.bit.italic = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off (with default colors)
  to.attr.bit.italic = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline on (with default colors)
  to.attr.bit.underline = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;4m\017$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off (with default colors)
  to.attr.bit.underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink on (with default colors)
  to.attr.bit.blink = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;5m\017$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off (with default colors)
  to.attr.bit.blink = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse on (with default colors)
  to.attr.bit.reverse = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;7m\017$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off (with default colors)
  to.attr.bit.reverse = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout on (with default colors)
  to.attr.bit.standout = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;1;7m\017$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off (with default colors)
  to.attr.bit.standout = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible on (with default colors)
  to.attr.bit.invisible = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( to.encoded_char[0] == ' ' );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off (with default colors)
  to.attr.bit.invisible = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect on (with default colors)
  to.attr.bit.protect = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off (with default colors)
  to.attr.bit.protect = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out on (with default colors)
  to.attr.bit.crossed_out = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017$<2>" CSI "9m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off (with default colors)
  to.attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline on (with default colors)
  to.attr.bit.dbl_underline = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017$<2>" CSI "21m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off (with default colors)
  to.attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set on (with default colors)
  to.attr.bit.alt_charset = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\016$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off (with default colors)
  to.attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , "\017" CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set on (with default colors)
  to.attr.bit.pc_charset = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m\017$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off (with default colors)
  to.attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Turn on all attributes (with default colors)
  to.attr.bit.pc_charset    = true;
  to.attr.bit.bold          = true;
  to.attr.bit.dim           = true;
  to.attr.bit.italic        = true;
  to.attr.bit.underline     = true;
  to.attr.bit.blink         = true;
  to.attr.bit.reverse       = true;
  to.attr.bit.standout      = true;
  to.attr.bit.invisible     = true;
  to.attr.bit.protect       = true;
  to.attr.bit.crossed_out   = true;
  to.attr.bit.dbl_underline = true;
  to.attr.bit.alt_charset   = true;
  to.attr.bit.pc_charset    = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0;1;4;7;5m\016$<2>"
                           CSI "9m" CSI "21m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Cyan text on blue background
  to.fg_color = finalcut::FColor::Cyan;
  to.bg_color = finalcut::FColor::Blue;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "38;5;6m" CSI "48;5;4m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off
  to.attr.bit.bold = false;
  CPPUNIT_ASSERT ( from == to );  // because of noColorVideo = 41
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off
  to.attr.bit.dim = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "22m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off
  to.attr.bit.italic = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off
  to.attr.bit.underline = false;
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "24m" CSI "21m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off
  to.attr.bit.blink = false;
  CPPUNIT_ASSERT ( from == to );  // because of noColorVideo = 41
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off
  to.attr.bit.reverse = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "27m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off
  to.attr.bit.standout = false;
  CPPUNIT_ASSERT ( from == to );  // because of noColorVideo = 41
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off
  to.attr.bit.invisible = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "28m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off
  to.attr.bit.protect = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" CSI "38;5;6m" CSI "48;5;4m"
                           "\016" CSI "9m" CSI "21m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off
  to.attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "29m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off
  to.attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "24m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off
  to.attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , "\017" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off
  to.attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "0m$<2>" CSI "38;5;6m" CSI "48;5;4m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Green text color
  to.fg_color = finalcut::FColor::Green;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), CSI "38;5;2m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default text color
  to.fg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( printSequence(oa.changeAttribute(from, to)).c_str()
                         , "Esc [ 3 9 ; 4 9 m Esc [ 4 8 ; 5 ; 4 m " );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );
}

//----------------------------------------------------------------------
void FOptiAttrTest::ibmColorTest()
{
  // Simulate IBM color definitions

  finalcut::FStartOptions::getFStartOptions().sgr_optimizer = false;
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
  oa.set_foreground_color (CSI "%?%p1%{0}%=%t30m"
                               "%e%p1%{1}%=%t31m"
                               "%e%p1%{2}%=%t32m"
                               "%e%p1%{3}%=%t33m"
                               "%e%p1%{4}%=%t34m"
                               "%e%p1%{5}%=%t35m"
                               "%e%p1%{6}%=%t36m"
                               "%e%p1%{7}%=%t97m%;");
  oa.set_background_color (CSI "%?%p1%{0}%=%t40m"
                               "%e%p1%{1}%=%t41m"
                               "%e%p1%{2}%=%t42m"
                               "%e%p1%{3}%=%t43m"
                               "%e%p1%{4}%=%t44m"
                               "%e%p1%{5}%=%t45m"
                               "%e%p1%{6}%=%t46m"
                               "%e%p1%{7}%=%t107m%;");
  oa.set_term_color_pair (0);
  oa.set_orig_pair (CSI "32;40m");
  oa.set_orig_orig_colors (0);
  oa.initialize();


  finalcut::FChar from{};
  finalcut::FChar to{};
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default color + bold
  from.fg_color = finalcut::FColor::Default;
  from.bg_color = finalcut::FColor::Default;
  to.attr.bit.bold = true;
  to.fg_color = finalcut::FColor::Default;
  to.bg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blue text on white background + dim + italic
  to.fg_color = finalcut::FColor::Blue;
  to.bg_color = finalcut::FColor::White;
  to.attr.bit.dim = true;
  to.attr.bit.italic = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "31m" CSI "107m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reset attributes + default background
  to.attr.bit.bold = false;
  to.attr.bit.dim = false;
  to.attr.bit.italic = false;
  to.bg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "32;40m" CSI "31m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Red text on black background
  to.fg_color = finalcut::FColor::Red;
  to.bg_color = finalcut::FColor::Black;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "34m" CSI "40m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // 256 color text and background
  to.fg_color = finalcut::FColor::SpringGreen3;
  to.bg_color = finalcut::FColor::NavyBlue;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "32m" CSI "41m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold on (with default colors)
  to.fg_color = finalcut::FColor::Default;
  to.bg_color = finalcut::FColor::Default;
  to.attr.bit.bold = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "32;40m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off (with default colors)
  to.attr.bit.bold = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim on (with default colors)
  to.attr.bit.dim = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off (with default colors)
  to.attr.bit.dim = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic on (with default colors)
  to.attr.bit.italic = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off (with default colors)
  to.attr.bit.italic = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline on (with default colors)
  to.attr.bit.underline = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off (with default colors)
  to.attr.bit.underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink on (with default colors)
  to.attr.bit.blink = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off (with default colors)
  to.attr.bit.blink = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse on (with default colors)
  to.attr.bit.reverse = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off (with default colors)
  to.attr.bit.reverse = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout on (with default colors)
  to.attr.bit.standout = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off (with default colors)
  to.attr.bit.standout = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible on (with default colors)
  to.attr.bit.invisible = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( to.encoded_char[0] == ' ' );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off (with default colors)
  to.attr.bit.invisible = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect on (with default colors)
  to.attr.bit.protect = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off (with default colors)
  to.attr.bit.protect = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out on (with default colors)
  to.attr.bit.crossed_out = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off (with default colors)
  to.attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline on (with default colors)
  to.attr.bit.dbl_underline = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off (with default colors)
  to.attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set on (with default colors)
  to.attr.bit.alt_charset = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off (with default colors)
  to.attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set on (with default colors)
  to.attr.bit.pc_charset = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off (with default colors)
  to.attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Turn on all attributes (with default colors)
  to.attr.bit.pc_charset    = true;
  to.attr.bit.bold          = true;
  to.attr.bit.dim           = true;
  to.attr.bit.italic        = true;
  to.attr.bit.underline     = true;
  to.attr.bit.blink         = true;
  to.attr.bit.reverse       = true;
  to.attr.bit.standout      = true;
  to.attr.bit.invisible     = true;
  to.attr.bit.protect       = true;
  to.attr.bit.crossed_out   = true;
  to.attr.bit.dbl_underline = true;
  to.attr.bit.alt_charset   = true;
  to.attr.bit.pc_charset    = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Cyan text on blue background
  to.fg_color = finalcut::FColor::Cyan;
  to.bg_color = finalcut::FColor::Blue;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "33m" CSI "41m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off
  to.attr.bit.bold = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off
  to.attr.bit.dim = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off
  to.attr.bit.italic = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off
  to.attr.bit.underline = false;
  CPPUNIT_ASSERT ( from == to );  // because of noColorVideo = 3
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off
  to.attr.bit.blink = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off
  to.attr.bit.reverse = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off
  to.attr.bit.standout = false;
  CPPUNIT_ASSERT ( from == to );  // because of noColorVideo = 3
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off
  to.attr.bit.invisible = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off
  to.attr.bit.protect = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off
  to.attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off
  to.attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off
  to.attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off
  to.attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Green text color
  to.fg_color = finalcut::FColor::Green;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , CSI "32m" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default text color
  to.fg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( printSequence(oa.changeAttribute(from, to)).c_str()
                         , "Esc [ 3 2 ; 4 0 m Esc [ 4 1 m " );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );
}

//----------------------------------------------------------------------
void FOptiAttrTest::wyse50Test()
{
  // Simulate an Wyse-50 terminal

  finalcut::FStartOptions::getFStartOptions().sgr_optimizer = false;
  finalcut::FOptiAttr oa;
  finalcut::FOptiAttr::TermEnv optiattr_env =
  {
    0,                           // Enter bold
    ESC "("  ESC "H\003"
    ESC "G0" ESC "cD",           // Exit bold
    ESC "Gp",                    // Enter dim
    ESC "("  ESC "H\003"
    ESC "G0" ESC "cD",           // Exit dim
    0,                           // Enter italics
    0,                           // Exit italics
    ESC "G8",                    // Enter underline
    ESC "G0",                    // Exit underline
    ESC "G2",                    // Enter blink
    ESC "("  ESC "H\003"
    ESC "G0" ESC "cD",           // Exit blink
    ESC "G2",                    // Enter reverse
    ESC "("  ESC "H\003"
    ESC "G0" ESC "cD",           // Exit reverse
    ESC "Gt",                    // Enter standout
    ESC "G0",                    // Exit standout
    ESC "G1",                    // Enter secure
    ESC "("  ESC "H\003"
    ESC "G0" ESC "cD",           // Exit secure
    ESC ")",                     // Enter protected
    ESC "("  ESC "H\003"
    ESC "G0" ESC "cD",           // Exit protected
    0,                           // Enter crossed out
    ESC "("  ESC "H\003"
    ESC "G0" ESC "cD",           // Exit crossed out
    0,                           // Enter double underline
    0,                           // Exit double underline
    "%?%p8%t\033)%e\033(%;"
    "%?%p9%t\033cE%e\033cD%;\033G%'0'"
    "%?%p2%t%{8}%|%;"
    "%?%p1%p3%|%p6%|%t%{4}%|%;"
    "%?%p4%t%{2}%|%;"
    "%?%p1%p5%|%t%'@'%|%;"
    "%?%p7%t%{1}%|%;%c",         // Set attributes
    ESC "("  ESC "H\003"
    ESC "G0" ESC "cD",           // Exit attribute
    ESC "cE",                    // Enter alt charset
    ESC "cD",                    // Exit alt charset
    0,                           // Enter pc charset
    0,                           // Exit pc charset
    0,                           // Ansi foreground color
    0,                           // Ansi background color
    0,                           // Foreground color
    0,                           // Background color
    0,                           // Term color pair
    0,                           // Orig pair
    0,                           // Orig orig colors
    1,                           // Max color
    0,                           // No color video
    false                        // No ANSI default color
  };

  oa.setTermEnvironment(optiattr_env);

  finalcut::FChar from{};
  finalcut::FChar to{};
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default color + bold
  from.fg_color = finalcut::FColor::Default;
  from.bg_color = finalcut::FColor::Default;
  to.attr.bit.bold = true;
  to.fg_color = finalcut::FColor::Default;
  to.bg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "cD" ESC "G4" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blue text on white background + dim + italic
  to.fg_color = finalcut::FColor::Blue;
  to.bg_color = finalcut::FColor::White;
  to.attr.bit.dim = true;
  to.attr.bit.italic = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "cD" ESC "Gt" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reset attributes + default background
  to.attr.bit.bold = false;
  to.attr.bit.dim = false;
  to.attr.bit.italic = false;
  to.bg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "H\003" ESC "G0" ESC "cD" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Red text on black background
  to.fg_color = finalcut::FColor::Red;
  to.bg_color = finalcut::FColor::Black;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // 256 color text and background
  to.fg_color = finalcut::FColor::SpringGreen3;
  to.bg_color = finalcut::FColor::NavyBlue;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold on (with default colors)
  to.fg_color = finalcut::FColor::Default;
  to.bg_color = finalcut::FColor::Default;
  to.attr.bit.bold = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "cD" ESC "G4" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off (with default colors)
  to.attr.bit.bold = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "H\003" ESC "G0" ESC "cD" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim on (with default colors)
  to.attr.bit.dim = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "cD" ESC "Gp" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off (with default colors)
  to.attr.bit.dim = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "H\003" ESC "G0" ESC "cD" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic on (with default colors)
  to.attr.bit.italic = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "cD" ESC "G0" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off (with default colors)
  to.attr.bit.italic = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "H\003" ESC "G0" ESC "cD" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline on (with default colors)
  to.attr.bit.underline = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "cD" ESC "G8" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off (with default colors)
  to.attr.bit.underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "H\003" ESC "G0" ESC "cD" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink on (with default colors)
  to.attr.bit.blink = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "cD" ESC "G2" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off (with default colors)
  to.attr.bit.blink = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "H\003" ESC "G0" ESC "cD" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse on (with default colors)
  to.attr.bit.reverse = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "cD" ESC "G4" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off (with default colors)
  to.attr.bit.reverse = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "H\003" ESC "G0" ESC "cD" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout on (with default colors)
  to.attr.bit.standout = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "cD" ESC "Gt" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off (with default colors)
  to.attr.bit.standout = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "H\003" ESC "G0" ESC "cD" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible on (with default colors)
  to.attr.bit.invisible = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "cD" ESC "G1" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off (with default colors)
  to.attr.bit.invisible = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "H\003" ESC "G0" ESC "cD" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect on (with default colors)
  to.attr.bit.protect = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC ")" ESC "cD" ESC "G0" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off (with default colors)
  to.attr.bit.protect = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "H\003" ESC "G0" ESC "cD" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out on (with default colors)
  to.attr.bit.crossed_out = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "cD" ESC "G0" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off (with default colors)
  to.attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "H\003" ESC "G0" ESC "cD" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline on (with default colors)
  to.attr.bit.dbl_underline = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "cD" ESC "G0" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off (with default colors)
  to.attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "H\003" ESC "G0" ESC "cD" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set on (with default colors)
  to.attr.bit.alt_charset = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "cE" ESC "G0" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off (with default colors)
  to.attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "cD" ESC "(" ESC "H\003" ESC "G0"
                           ESC "cD" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set on (with default colors)
  to.attr.bit.pc_charset = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "cD" ESC "G0" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off (with default colors)
  to.attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "H\003" ESC "G0" ESC "cD" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Turn on all attributes (with default colors)
  to.attr.bit.pc_charset    = true;
  to.attr.bit.bold          = true;
  to.attr.bit.dim           = true;
  to.attr.bit.italic        = true;
  to.attr.bit.underline     = true;
  to.attr.bit.blink         = true;
  to.attr.bit.reverse       = true;
  to.attr.bit.standout      = true;
  to.attr.bit.invisible     = true;
  to.attr.bit.protect       = true;
  to.attr.bit.crossed_out   = true;
  to.attr.bit.dbl_underline = true;
  to.attr.bit.alt_charset   = true;
  to.attr.bit.pc_charset    = true;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC ")" ESC "cE" ESC "G\177" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Cyan text on blue background
  to.fg_color = finalcut::FColor::Cyan;
  to.bg_color = finalcut::FColor::Blue;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Bold off
  to.attr.bit.bold = false;
  CPPUNIT_ASSERT ( from != to );

  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "H\003" ESC "G0" ESC "cD"
                           ESC "cE" ESC "Gp" ESC "G8" ESC "G2"
                           ESC "G2" ESC "Gt" ESC "G1" ESC ")" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Dim off
  to.attr.bit.dim = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "H\003" ESC "G0" ESC "cD"
                           ESC "cE" ESC "G8" ESC "G2" ESC "G2"
                           ESC "Gt" ESC "G1" ESC ")" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Italic off
  to.attr.bit.italic = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Underline off
  to.attr.bit.underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "G0" ESC "cE" ESC "G2" ESC "G2"
                           ESC "Gt" ESC "G1" ESC ")" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Blink off
  to.attr.bit.blink = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "H\003" ESC "G0" ESC "cD"
                           ESC "cE" ESC "G2" ESC "Gt" ESC "G1"
                           ESC ")" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Reverse off
  to.attr.bit.reverse = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "H\003" ESC "G0" ESC "cD"
                           ESC "cE" ESC "Gt" ESC "G1" ESC ")" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Standout off
  to.attr.bit.standout = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "G0" ESC "cE" ESC "G1" ESC ")" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Invisible off
  to.attr.bit.invisible = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "H\003" ESC "G0" ESC "cD"
                           ESC "cE" ESC ")" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Protect off
  to.attr.bit.protect = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "H\003" ESC "G0" ESC "cD"
                           ESC "cE" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Crossed out off
  to.attr.bit.crossed_out = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "H\003" ESC "G0" ESC "cD"
                           ESC "cE" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Double underline off
  to.attr.bit.dbl_underline = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Alternate character set off
  to.attr.bit.alt_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), ESC "cD" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // PC character set off
  to.attr.bit.pc_charset = false;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to)
                         , ESC "(" ESC "H\003" ESC "G0" ESC "cD" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Green text color
  to.fg_color = finalcut::FColor::Green;
  CPPUNIT_ASSERT ( from != to );
  CPPUNIT_ASSERT_CSTRING ( oa.changeAttribute(from, to), "" );
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );

  // Default text color
  to.fg_color = finalcut::FColor::Default;
  CPPUNIT_ASSERT ( from == to );
  CPPUNIT_ASSERT ( oa.changeAttribute(from, to) == 0 );
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
