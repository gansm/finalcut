/***********************************************************************
* ftermcapquirks-test.cpp - FTermcapQuirks unit tests                  *
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

#include <string>

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <final/final.h>

#define CPPUNIT_ASSERT_CSTRING(expected, actual) \
            check_c_string (expected, actual, CPPUNIT_SOURCELINE())

//----------------------------------------------------------------------
void check_c_string ( const char* s1
                    , const char* s2
                    , const CppUnit::SourceLine& sourceLine )
{
  if ( s1 == nullptr && s2 == nullptr )  // Strings are equal
    return;

  if ( s1 && s2 && std::strcmp (s1, s2) == 0 )  // Strings are equal
      return;

  ::CppUnit::Asserter::fail ("Strings are not equal", sourceLine);
}

//----------------------------------------------------------------------
namespace test
{

struct TermcapString
{
  const char* data;
  uInt32 length{};
};

struct tcap_map
{
  TermcapString string;
  char tname[alignof(char*)];
};

static tcap_map tcap[] =
{
  { {nullptr, 0}, {"bl"} },  // bell
  { {nullptr, 0}, {"ec"} },  // erase_chars
  { {nullptr, 0}, {"cl"} },  // clear_screen
  { {nullptr, 0}, {"cd"} },  // clr_eos
  { {nullptr, 0}, {"ce"} },  // clr_eol
  { {nullptr, 0}, {"cb"} },  // clr_bol
  { {nullptr, 0}, {"ho"} },  // cursor_home
  { {nullptr, 0}, {"ll"} },  // cursor_to_ll
  { {nullptr, 0}, {"cr"} },  // carriage_return
  { {nullptr, 0}, {"ta"} },  // tab
  { {nullptr, 0}, {"bt"} },  // back_tab
  { {nullptr, 0}, {"ip"} },  // insert_padding
  { {nullptr, 0}, {"ic"} },  // insert_character
  { {nullptr, 0}, {"IC"} },  // parm_ich
  { {nullptr, 0}, {"rp"} },  // repeat_char
  { {nullptr, 0}, {"lr"} },  // repeat_last_char
  { {nullptr, 0}, {"Ic"} },  // initialize_color
  { {nullptr, 0}, {"Ip"} },  // initialize_pair
  { {nullptr, 0}, {"AF"} },  // set_a_foreground
  { {nullptr, 0}, {"AB"} },  // set_a_background
  { {nullptr, 0}, {"Sf"} },  // set_foreground
  { {nullptr, 0}, {"Sb"} },  // set_background
  { {nullptr, 0}, {"sp"} },  // set_color_pair
  { {nullptr, 0}, {"op"} },  // orig_pair
  { {nullptr, 0}, {"oc"} },  // orig_colors
  { {nullptr, 0}, {"NC"} },  // no_color_video
  { {nullptr, 0}, {"cm"} },  // cursor_address
  { {nullptr, 0}, {"ch"} },  // column_address
  { {nullptr, 0}, {"cv"} },  // row_address
  { {nullptr, 0}, {"vs"} },  // cursor_visible
  { {nullptr, 0}, {"vi"} },  // cursor_invisible
  { {nullptr, 0}, {"ve"} },  // cursor_normal
  { {nullptr, 0}, {"up"} },  // cursor_up
  { {nullptr, 0}, {"do"} },  // cursor_down
  { {nullptr, 0}, {"le"} },  // cursor_left
  { {nullptr, 0}, {"nd"} },  // cursor_right
  { {nullptr, 0}, {"UP"} },  // parm_up_cursor
  { {nullptr, 0}, {"DO"} },  // parm_down_cursor
  { {nullptr, 0}, {"LE"} },  // parm_left_cursor
  { {nullptr, 0}, {"RI"} },  // parm_right_cursor
  { {nullptr, 0}, {"sc"} },  // save_cursor
  { {nullptr, 0}, {"rc"} },  // restore_cursor
  { {nullptr, 0}, {"Ss"} },  // set cursor style
  { {nullptr, 0}, {"sf"} },  // scroll_forward
  { {nullptr, 0}, {"sr"} },  // scroll_reverse
  { {nullptr, 0}, {"ti"} },  // enter_ca_mode
  { {nullptr, 0}, {"te"} },  // exit_ca_mode
  { {nullptr, 0}, {"eA"} },  // enable_acs
  { {nullptr, 0}, {"md"} },  // enter_bold_mode
  { {nullptr, 0}, {"me"} },  // exit_bold_mode
  { {nullptr, 0}, {"mh"} },  // enter_dim_mode
  { {nullptr, 0}, {"me"} },  // exit_dim_mode
  { {nullptr, 0}, {"ZH"} },  // enter_italics_mode
  { {nullptr, 0}, {"ZR"} },  // exit_italics_mode
  { {nullptr, 0}, {"us"} },  // enter_underline_mode
  { {nullptr, 0}, {"ue"} },  // exit_underline_mode
  { {nullptr, 0}, {"mb"} },  // enter_blink_mode
  { {nullptr, 0}, {"me"} },  // exit_blink_mode
  { {nullptr, 0}, {"mr"} },  // enter_reverse_mode
  { {nullptr, 0}, {"me"} },  // exit_reverse_mode
  { {nullptr, 0}, {"so"} },  // enter_standout_mode
  { {nullptr, 0}, {"se"} },  // exit_standout_mode
  { {nullptr, 0}, {"mk"} },  // enter_secure_mode
  { {nullptr, 0}, {"me"} },  // exit_secure_mode
  { {nullptr, 0}, {"mp"} },  // enter_protected_mode
  { {nullptr, 0}, {"me"} },  // exit_protected_mode
  { {nullptr, 0}, {"XX"} },  // enter_crossed_out_mode
  { {nullptr, 0}, {"me"} },  // exit_crossed_out_mode
  { {nullptr, 0}, {"Us"} },  // enter_dbl_underline_mode
  { {nullptr, 0}, {"Ue"} },  // exit_dbl_underline_mode
  { {nullptr, 0}, {"sa"} },  // set_attributes
  { {nullptr, 0}, {"me"} },  // exit_attribute_mode
  { {nullptr, 0}, {"as"} },  // enter_alt_charset_mode
  { {nullptr, 0}, {"ae"} },  // exit_alt_charset_mode
  { {nullptr, 0}, {"S2"} },  // enter_pc_charset_mode
  { {nullptr, 0}, {"S3"} },  // exit_pc_charset_mode
  { {nullptr, 0}, {"im"} },  // enter_insert_mode
  { {nullptr, 0}, {"ei"} },  // exit_insert_mode
  { {nullptr, 0}, {"SA"} },  // enter_am_mode
  { {nullptr, 0}, {"RA"} },  // exit_am_mode
  { {nullptr, 0}, {"ac"} },  // acs_chars
  { {nullptr, 0}, {"ks"} },  // keypad_xmit
  { {nullptr, 0}, {"ke"} },  // keypad_local
  { {nullptr, 0}, {"Km"} },  // key_mouse
  { {nullptr, 0}, {"\0"} }
};

}  // namespace test


//----------------------------------------------------------------------
// class FTermcapQuirksTest
//----------------------------------------------------------------------

class FTermcapQuirksTest : public CPPUNIT_NS::TestFixture
{
  public:
    FTermcapQuirksTest() = default;

  protected:
    void classNameTest();
    void generalTest();
    void repeatLastChar();
    void xtermTest();
#if defined(__FreeBSD__) || defined(__DragonFly__)
    void freebsdTest();
#endif
    void cygwinTest();
    void linuxTest();
    void rxvtTest();
    void vteTest();
    void kittyTest();
    void puttyTest();
    void teratermTest();
    void sunTest();
    void screenTest();

  private:
    auto printSequence (const std::string&) -> std::string;

    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FTermcapQuirksTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (generalTest);
    CPPUNIT_TEST (repeatLastChar);
    CPPUNIT_TEST (xtermTest);
#if defined(__FreeBSD__) || defined(__DragonFly__)
    CPPUNIT_TEST (freebsdTest);
#endif
    CPPUNIT_TEST (cygwinTest);
    CPPUNIT_TEST (linuxTest);
    CPPUNIT_TEST (rxvtTest);
    CPPUNIT_TEST (vteTest);
    CPPUNIT_TEST (kittyTest);
    CPPUNIT_TEST (puttyTest);
    CPPUNIT_TEST (teratermTest);
    CPPUNIT_TEST (sunTest);
    CPPUNIT_TEST (screenTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};

//----------------------------------------------------------------------
void FTermcapQuirksTest::classNameTest()
{
  finalcut::FTermcapQuirks q;
  const finalcut::FString& classname = q.getClassName();
  CPPUNIT_ASSERT ( classname == "FTermcapQuirks" );
}

//----------------------------------------------------------------------
void FTermcapQuirksTest::generalTest()
{
  auto& caps = finalcut::FTermcap::strings;
  static constexpr int last_item = int(sizeof(test::tcap)
                                 / sizeof(test::tcap[0])) - 1;

  for (std::size_t i = 0; i < last_item; i++)
  {
    caps[i].string.data   = test::tcap[i].string.data;
    caps[i].string.length = test::tcap[i].string.length;

    std::copy ( std::begin(test::tcap[i].tname)
              , std::end(test::tcap[i].tname)
              , caps[i].tname.begin() );
  }

  finalcut::FTermcap::tabstop = -1;
  finalcut::FTermcap::attr_without_color = -1;
  finalcut::FTermcap::can_change_color_palette = false;
  finalcut::FTermcapQuirks quirks;
  quirks.terminalFixup();

  CPPUNIT_ASSERT ( finalcut::FTermcap::tabstop == 8 );
  CPPUNIT_ASSERT ( finalcut::FTermcap::attr_without_color == 0 );
  CPPUNIT_ASSERT ( finalcut::FTermcap::can_change_color_palette );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_set_a_foreground)].string.data
                         , CSI "3%p1%dm" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_set_a_background)].string.data
                         , CSI "4%p1%dm" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_initialize_color)].string.data
                         , OSC "P%p1%x"
                               "%p2%{255}%*%{1000}%/%02x"
                               "%p3%{255}%*%{1000}%/%02x"
                               "%p4%{255}%*%{1000}%/%02x" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_enter_ca_mode)].string.data
                         , ESC "7" CSI "?47h"  );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_ca_mode)].string.data
                         , CSI "?47l" ESC "8" CSI "m" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_cursor_address)].string.data
                         , CSI "%i%p1%d;%p2%dH" );
  // Non standard ECMA-48 (ANSI X3.64) terminal
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_enter_dbl_underline_mode)].string.data
                         , nullptr );
  caps[int(finalcut::Termcap::t_exit_underline_mode)].string.data = CSI "24m";
  caps[int(finalcut::Termcap::t_exit_underline_mode)].string.length = 5;
  quirks.terminalFixup();
  // Standard ECMA-48 (ANSI X3.64) terminal
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_enter_dbl_underline_mode)].string.data
                         , CSI "21m" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_dbl_underline_mode)].string.data
                         , CSI "24m" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_bold_mode)].string.data
                         , CSI "22m" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_dim_mode)].string.data
                         , CSI "22m" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_underline_mode)].string.data
                         , CSI "24m" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_blink_mode)].string.data
                         , CSI "25m" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_reverse_mode)].string.data
                         , CSI "27m" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_secure_mode)].string.data
                         , CSI "28m" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_enter_crossed_out_mode)].string.data
                         , CSI "9m" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_crossed_out_mode)].string.data
                         , CSI "29m" );
  CPPUNIT_ASSERT_CSTRING ( printSequence(std::string(caps[int(finalcut::Termcap::t_enter_ca_mode)].string.data)).c_str()
                         , "Esc 7 Esc [ ? 4 7 h " );
}

//----------------------------------------------------------------------
void FTermcapQuirksTest::repeatLastChar()
{
  auto& caps = finalcut::FTermcap::strings;
  static constexpr int last_item = int(sizeof(test::tcap)
                                 / sizeof(test::tcap[0])) - 1;

  for (std::size_t i = 0; i < last_item; i++)
  {
    caps[i].string.data   = test::tcap[i].string.data;
    caps[i].string.length = test::tcap[i].string.length;

    std::copy ( std::begin(test::tcap[i].tname)
              , std::end(test::tcap[i].tname)
              , caps[i].tname.begin() );
  }

  finalcut::FTermcapQuirks quirks;
  caps[int(finalcut::Termcap::t_repeat_char)].string.data = "%p1%c\033[%p2%{1}%-%db";
  caps[int(finalcut::Termcap::t_repeat_char)].string.length = 19;
  CPPUNIT_ASSERT ( ! caps[int(finalcut::Termcap::t_repeat_last_char)].string.data );
  quirks.terminalFixup();
  CPPUNIT_ASSERT ( caps[int(finalcut::Termcap::t_repeat_last_char)].string.data );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_repeat_last_char)].string.data
                         , "\033[%p1%{1}%-%db" );
}

//----------------------------------------------------------------------
void FTermcapQuirksTest::xtermTest()
{
  auto& caps = finalcut::FTermcap::strings;
  static constexpr int last_item = int(sizeof(test::tcap)
                                 / sizeof(test::tcap[0])) - 1;

  for (std::size_t i = 0; i < last_item; i++)
  {
    caps[i].string.data   = test::tcap[i].string.data;
    caps[i].string.length = test::tcap[i].string.length;

    std::copy ( std::begin(test::tcap[i].tname)
              , std::end(test::tcap[i].tname)
              , caps[i].tname.begin() );
  }

  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermcapQuirks quirks;
  finalcut::FTermcap::can_change_color_palette = false;
  data.setTermType (finalcut::FTermType::xterm);
  data.setTermType ("xterm");
  quirks.terminalFixup();

  CPPUNIT_ASSERT ( finalcut::FTermcap::can_change_color_palette );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_initialize_color)].string.data
                         , OSC "4;%p1%d;rgb:"
                               "%p2%{255}%*%{1000}%/%2.2X/"
                               "%p3%{255}%*%{1000}%/%2.2X/"
                               "%p4%{255}%*%{1000}%/%2.2X" ESC "\\");
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_cursor_invisible)].string.data
                         , CSI "?25l" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_cursor_normal)].string.data
                         , CSI "?12l" CSI "?25h" );
  data.unsetTermType (finalcut::FTermType::xterm);
}

#if defined(__FreeBSD__) || defined(__DragonFly__)
//----------------------------------------------------------------------
void FTermcapQuirksTest::freebsdTest()
{
  auto& caps = finalcut::FTermcap::strings;
  static constexpr int last_item = int(sizeof(test::tcap)
                                 / sizeof(test::tcap[0])) - 1;

  for (std::size_t i = 0; i < last_item; i++)
  {
    caps[i].string.data   = test::tcap[i].string.data;
    caps[i].string.length = test::tcap[i].string.length;

    std::copy ( std::begin(test::tcap[i].tname)
              , std::end(test::tcap[i].tname)
              , caps[i].tname.begin() );
  }

  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermcap::attr_without_color = -1;
  finalcut::FTermcapQuirks quirks;
  data.setTermType (finalcut::FTermType::freebsd_con);
  data.setTermType ("xterm-16color");
  quirks.setFTermData(&data);
  quirks.setFTermDetection (&detect);
  quirks.terminalFixup();

  CPPUNIT_ASSERT ( finalcut::FTermcap::attr_without_color == 18 );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_acs_chars)].string.data
                         , "-\036.\0370\333"
                           "a\260f\370g\361"
                           "h\261j\331k\277"
                           "l\332m\300n\305"
                           "q\304t\303u\264"
                           "v\301w\302x\263"
                           "y\363z\362~\371" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_set_attributes)].string.data
                         , CSI "0"
                               "%?%p1%p6%|%t;1%;"
                               "%?%p2%t;4%;"
                               "%?%p1%p3%|%t;7%;"
                               "%?%p4%t;5%;m"
                               "%?%p9%t\016%e\017%;" );
  data.unsetTermType (finalcut::FTermType::freebsd_con);
}
#endif

//----------------------------------------------------------------------
void FTermcapQuirksTest::cygwinTest()
{
  auto& caps = finalcut::FTermcap::strings;
  static constexpr int last_item = int(sizeof(test::tcap)
                                 / sizeof(test::tcap[0])) - 1;

  for (std::size_t i = 0; i < last_item; i++)
  {
    caps[i].string.data   = test::tcap[i].string.data;
    caps[i].string.length = test::tcap[i].string.length;

    std::copy ( std::begin(test::tcap[i].tname)
              , std::end(test::tcap[i].tname)
              , caps[i].tname.begin() );
  }

  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermcap::background_color_erase = false;
  finalcut::FTermcapQuirks quirks;
  data.setTermType (finalcut::FTermType::cygwin);
  data.setTermType ("cygwin");
  quirks.terminalFixup();

  CPPUNIT_ASSERT ( finalcut::FTermcap::background_color_erase == true );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_cursor_invisible)].string.data
                         , CSI "?25l" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_cursor_visible)].string.data
                         , CSI "?25h" );
  data.unsetTermType (finalcut::FTermType::cygwin);
}

//----------------------------------------------------------------------
void FTermcapQuirksTest::linuxTest()
{
  auto& caps = finalcut::FTermcap::strings;
  static constexpr int last_item = int(sizeof(test::tcap)
                                 / sizeof(test::tcap[0])) - 1;

  for (std::size_t i = 0; i < last_item; i++)
  {
    caps[i].string.data   = test::tcap[i].string.data;
    caps[i].string.length = test::tcap[i].string.length;

    std::copy ( std::begin(test::tcap[i].tname)
              , std::end(test::tcap[i].tname)
              , caps[i].tname.begin() );
  }

  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermcap::max_color = 8;
  finalcut::FTermcap::attr_without_color = -1;
  finalcut::FTermcapQuirks quirks;
  data.setTermType (finalcut::FTermType::linux_con);
  data.setTermType ("linux");
  quirks.terminalFixup();

  // 8 colors
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_set_a_foreground)].string.data
                         , CSI "3%p1%dm" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_set_a_background)].string.data
                         , CSI "4%p1%dm" );
  CPPUNIT_ASSERT ( finalcut::FTermcap::attr_without_color == 18 );

  // 16 colors
  finalcut::FTermcap::max_color = 16;
  quirks.terminalFixup();

  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_set_a_foreground)].string.data
                         , CSI "3%p1%{8}%m%d%?%p1%{7}%>%t;1%e;22%;m" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_set_a_background)].string.data
                         , CSI "4%p1%{8}%m%d%?%p1%{7}%>%t;5%e;25%;m" );
  CPPUNIT_ASSERT ( finalcut::FTermcap::attr_without_color == 30 );

  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_set_attributes)].string.data
                         , CSI "0"
                               "%?%p6%t;1%;"
                               "%?%p1%p3%|%t;7%;"
                               "%?%p4%t;5%;m"
                               "%?%p9%t\016%e\017%;" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_enter_alt_charset_mode)].string.data
                         , "\016" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_alt_charset_mode)].string.data
                         , "\017" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_attribute_mode)].string.data
                         , CSI "0m\017" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_bold_mode)].string.data
                         , CSI "22m" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_blink_mode)].string.data
                         , CSI "25m" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_reverse_mode)].string.data
                         , CSI "27m" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_secure_mode)].string.data
                         , nullptr );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_protected_mode)].string.data
                         , nullptr );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_crossed_out_mode)].string.data
                         , nullptr );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_orig_pair)].string.data
                         , CSI "39;49;25m" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_enter_dim_mode)].string.data
                         , nullptr );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_dim_mode)].string.data
                         , nullptr );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_enter_underline_mode)].string.data
                         , nullptr );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_underline_mode)].string.data
                         , nullptr );
  data.unsetTermType (finalcut::FTermType::linux_con);
}

//----------------------------------------------------------------------
void FTermcapQuirksTest::rxvtTest()
{
  auto& caps = finalcut::FTermcap::strings;
  static constexpr int last_item = int(sizeof(test::tcap)
                                 / sizeof(test::tcap[0])) - 1;

  for (std::size_t i = 0; i < last_item; i++)
  {
    caps[i].string.data   = test::tcap[i].string.data;
    caps[i].string.length = test::tcap[i].string.length;

    std::copy ( std::begin(test::tcap[i].tname)
              , std::end(test::tcap[i].tname)
              , caps[i].tname.begin() );
  }

  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermcapQuirks quirks;
  data.setTermType (finalcut::FTermType::rxvt);
  data.setTermType ("rxvt");
  quirks.terminalFixup();

  // rxvt
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_enter_alt_charset_mode)].string.data
                         , nullptr );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_alt_charset_mode)].string.data
                         , nullptr );
  // rxvt-16color
  data.setTermType ("rxvt-16color");
  quirks.terminalFixup();
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_enter_alt_charset_mode)].string.data
                         , ESC "(0" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_alt_charset_mode)].string.data
                         , ESC "(B" );

  // urxvt
  data.setTermType (finalcut::FTermType::urxvt);
  quirks.terminalFixup();
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_set_a_foreground)].string.data
                         , CSI "%?%p1%{8}%<%t%p1%{30}%+%e%p1%'R'%+%;%dm" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_set_a_background)].string.data
                         , CSI "%?%p1%{8}%<%t%p1%'('%+%e%p1%{92}%+%;%dm" );

  data.unsetTermType (finalcut::FTermType::urxvt);
  data.unsetTermType (finalcut::FTermType::rxvt);
}

//----------------------------------------------------------------------
void FTermcapQuirksTest::vteTest()
{
  auto& caps = finalcut::FTermcap::strings;
  static constexpr int last_item = int(sizeof(test::tcap)
                                 / sizeof(test::tcap[0])) - 1;

  for (std::size_t i = 0; i < last_item; i++)
  {
    caps[i].string.data   = test::tcap[i].string.data;
    caps[i].string.length = test::tcap[i].string.length;

    std::copy ( std::begin(test::tcap[i].tname)
              , std::end(test::tcap[i].tname)
              , caps[i].tname.begin() );
  }

  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermcap::attr_without_color = -1;
  finalcut::FTermcapQuirks quirks;
  data.setTermType (finalcut::FTermType::gnome_terminal);
  data.setTermType ("gnome-256color");
  quirks.terminalFixup();

  CPPUNIT_ASSERT ( finalcut::FTermcap::attr_without_color == 0 );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_underline_mode)].string.data
                         , CSI "24m" );

  data.unsetTermType (finalcut::FTermType::gnome_terminal);
}

//----------------------------------------------------------------------
void FTermcapQuirksTest::kittyTest()
{
  auto& caps = finalcut::FTermcap::strings;
  static constexpr int last_item = int(sizeof(test::tcap)
                                 / sizeof(test::tcap[0])) - 1;

  for (std::size_t i = 0; i < last_item; i++)
  {
    caps[i].string.data   = test::tcap[i].string.data;
    caps[i].string.length = test::tcap[i].string.length;

    std::copy ( std::begin(test::tcap[i].tname)
              , std::end(test::tcap[i].tname)
              , caps[i].tname.begin() );
  }

  caps[int(finalcut::Termcap::t_enter_ca_mode)].string.data = CSI "?1049h";
  caps[int(finalcut::Termcap::t_exit_ca_mode)].string.data = CSI "?1049l";
  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermcapQuirks quirks;
  data.setTermType (finalcut::FTermType::kitty);
  data.setTermType ("xterm-kitty");
  quirks.terminalFixup();

  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_enter_ca_mode)].string.data
                         , CSI "?1049h" CSI "22;0;0t" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_ca_mode)].string.data
                         , CSI "?1049l" CSI "23;0;0t" );

  data.unsetTermType (finalcut::FTermType::kitty);
}

//----------------------------------------------------------------------
void FTermcapQuirksTest::puttyTest()
{
  auto& caps = finalcut::FTermcap::strings;
  static constexpr int last_item = int(sizeof(test::tcap)
                                 / sizeof(test::tcap[0])) - 1;

  for (std::size_t i = 0; i < last_item; i++)
  {
    caps[i].string.data   = test::tcap[i].string.data;
    caps[i].string.length = test::tcap[i].string.length;

    std::copy ( std::begin(test::tcap[i].tname)
              , std::end(test::tcap[i].tname)
              , caps[i].tname.begin() );
  }

  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermcap::background_color_erase = false;
  finalcut::FTermcap::can_change_color_palette = false;
  finalcut::FTermcap::osc_support = false;
  finalcut::FTermcap::attr_without_color = -1;
  finalcut::FTermcapQuirks quirks;
  data.setTermType (finalcut::FTermType::putty);
  data.setTermType ("putty");
  quirks.terminalFixup();

  CPPUNIT_ASSERT ( finalcut::FTermcap::background_color_erase == true );
  CPPUNIT_ASSERT ( finalcut::FTermcap::osc_support == true );
  CPPUNIT_ASSERT ( finalcut::FTermcap::attr_without_color == 0 );
  CPPUNIT_ASSERT ( finalcut::FTermcap::can_change_color_palette );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_initialize_color)].string.data
                         , OSC "P%p1%x"
                               "%p2%{255}%*%{1000}%/%02x"
                               "%p3%{255}%*%{1000}%/%02x"
                               "%p4%{255}%*%{1000}%/%02x" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_set_a_foreground)].string.data
                         , CSI "%?%p1%{8}%<"
                               "%t3%p1%d"
                               "%e%p1%{16}%<"
                               "%t9%p1%{8}%-%d"
                               "%e38;5;%p1%d%;m" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_set_a_background)].string.data
                         , CSI "%?%p1%{8}%<"
                               "%t4%p1%d"
                               "%e%p1%{16}%<"
                               "%t10%p1%{8}%-%d"
                               "%e48;5;%p1%d%;m" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_set_attributes)].string.data
                         , CSI "0"
                               "%?%p1%p6%|%t;1%;"
                               "%?%p5%t;2%;"
                               "%?%p2%t;4%;"
                               "%?%p1%p3%|%t;7%;"
                               "%?%p4%t;5%;m"
                               "%?%p9%t\016%e\017%;" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_enter_dim_mode)].string.data
                         , CSI "2m" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_dim_mode)].string.data
                         , CSI "22m" );

  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_clr_bol)].string.data
                         , CSI "1K" );

  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_orig_pair)].string.data
                         , CSI "39;49m" );

  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_orig_colors)].string.data
                         , OSC "R" );

  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_column_address)].string.data
                         , CSI "%i%p1%dG" );

  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_row_address)].string.data
                         , CSI "%i%p1%dd" );

  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_enable_acs)].string.data
                         , ESC "(B" ESC ")0" );

  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_enter_am_mode)].string.data
                         , CSI "?7h" );

  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_am_mode)].string.data
                         , CSI "?7l" );

  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_enter_pc_charset_mode)].string.data
                         , CSI "11m" );

  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_pc_charset_mode)].string.data
                         , CSI "10m" );

  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_key_mouse)].string.data
                        , CSI "M" );

  data.unsetTermType (finalcut::FTermType::putty);
}

//----------------------------------------------------------------------
void FTermcapQuirksTest::teratermTest()
{
  auto& caps = finalcut::FTermcap::strings;
  static constexpr int last_item = int(sizeof(test::tcap)
                                 / sizeof(test::tcap[0])) - 1;

  for (std::size_t i = 0; i < last_item; i++)
  {
    caps[i].string.data   = test::tcap[i].string.data;
    caps[i].string.length = test::tcap[i].string.length;

    std::copy ( std::begin(test::tcap[i].tname)
              , std::end(test::tcap[i].tname)
              , caps[i].tname.begin() );
  }

  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermcap::eat_nl_glitch = false;
  finalcut::FTermcapQuirks quirks;
  data.setTermType (finalcut::FTermType::tera_term);
  data.setTermType ("teraterm");
  quirks.terminalFixup();

  CPPUNIT_ASSERT ( finalcut::FTermcap::eat_nl_glitch == true );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_set_a_foreground)].string.data
                         , CSI "38;5;%p1%dm" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_set_a_background)].string.data
                         , CSI "48;5;%p1%dm" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_exit_attribute_mode)].string.data
                         , CSI "0m" SI );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_orig_pair)].string.data
                         , CSI "39;49m" );

  data.unsetTermType (finalcut::FTermType::tera_term);
}

//----------------------------------------------------------------------
void FTermcapQuirksTest::sunTest()
{
  auto& caps = finalcut::FTermcap::strings;
  static constexpr int last_item = int(sizeof(test::tcap)
                                 / sizeof(test::tcap[0])) - 1;

  for (std::size_t i = 0; i < last_item; i++)
  {
    caps[i].string.data   = test::tcap[i].string.data;
    caps[i].string.length = test::tcap[i].string.length;

    std::copy ( std::begin(test::tcap[i].tname)
              , std::end(test::tcap[i].tname)
              , caps[i].tname.begin() );
  }

  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermcap::eat_nl_glitch = false;
  finalcut::FTermcap::tabstop = -1;
  finalcut::FTermcapQuirks quirks;
  data.setTermType (finalcut::FTermType::sun_con);
  data.setTermType ("sun-color");
  quirks.terminalFixup();

  CPPUNIT_ASSERT ( finalcut::FTermcap::eat_nl_glitch == true );
  CPPUNIT_ASSERT ( finalcut::FTermcap::tabstop == 8 );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_parm_up_cursor)].string.data
                         , CSI "%p1%dA" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_parm_down_cursor)].string.data
                         , CSI "%p1%dB" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_parm_right_cursor)].string.data
                         , CSI "%p1%dC" );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_parm_left_cursor)].string.data
                         , CSI "%p1%dD" );
  auto& fkey_cap_table = finalcut::FKeyMap::getInstance().getKeyCapMap();

  for (auto& fkey_cap : fkey_cap_table)
  {
    if ( std::memcmp(fkey_cap.tname.data(), "K2", 2) == 0
      && finalcut::stringLength(fkey_cap.tname.data()) == 2 )  // center of keypad
      CPPUNIT_ASSERT_CSTRING ( fkey_cap.string
                             , CSI "218z" );

    if ( std::memcmp(fkey_cap.tname.data(), "kb", 2) == 0 )  // backspace key
      CPPUNIT_ASSERT_CSTRING ( fkey_cap.string
                             , "\b" );

    if ( std::memcmp(fkey_cap.tname.data(), "kD", 2) == 0
      && std::strlen(fkey_cap.tname.data()) == 2 )  // delete-character key
      CPPUNIT_ASSERT_CSTRING ( fkey_cap.string
                             , "\177" );

    if ( std::memcmp(fkey_cap.tname.data(), "@7", 2) == 0
      && finalcut::stringLength(fkey_cap.tname.data()) == 2 )  // end key
      CPPUNIT_ASSERT_CSTRING ( fkey_cap.string
                             , CSI "220z" );

    if ( std::memcmp(fkey_cap.tname.data(), "k;", 2) == 0 )  // F10 function key
      CPPUNIT_ASSERT_CSTRING ( fkey_cap.string
                             , CSI "233z" );

    if ( std::memcmp(fkey_cap.tname.data(), "F1", 2) == 0 )  // F11 function key
      CPPUNIT_ASSERT_CSTRING ( fkey_cap.string
                             , CSI "234z" );

    if ( std::memcmp(fkey_cap.tname.data(), "F2", 2) == 0 )  // F12 function key
      CPPUNIT_ASSERT_CSTRING ( fkey_cap.string
                             , CSI "235z" );
    if ( std::memcmp(fkey_cap.tname.data(), "kh", 2) == 0
      && finalcut::stringLength(fkey_cap.tname.data()) == 2 )  // home key
      CPPUNIT_ASSERT_CSTRING ( fkey_cap.string
                             , CSI "214z" );

    if ( std::memcmp(fkey_cap.tname.data(), "kI", 2) == 0
      && finalcut::stringLength(fkey_cap.tname.data()) == 2 )  // insert-character key
      CPPUNIT_ASSERT_CSTRING ( fkey_cap.string
                             , CSI "247z" );

    if ( std::memcmp(fkey_cap.tname.data(), "kN", 2) == 0 )  // next-page key
      CPPUNIT_ASSERT_CSTRING ( fkey_cap.string
                             , CSI "222z" );

    if ( std::memcmp(fkey_cap.tname.data(), "%7", 2) == 0 )  // options key
      CPPUNIT_ASSERT_CSTRING ( fkey_cap.string
                             , CSI "194z" );

    if ( std::memcmp(fkey_cap.tname.data(), "kP", 2) == 0 )  // prev-page key
      CPPUNIT_ASSERT_CSTRING ( fkey_cap.string
                             , CSI "216z" );

    if ( std::memcmp(fkey_cap.tname.data(), "&5", 2) == 0 )  // resume key
      CPPUNIT_ASSERT_CSTRING ( fkey_cap.string
                             , CSI "193z" );

    if ( std::memcmp(fkey_cap.tname.data(), "&8", 2) == 0 )  // undo key
      CPPUNIT_ASSERT_CSTRING ( fkey_cap.string
                             , CSI "195z" );

    if ( std::memcmp(fkey_cap.tname.data(), "K2", 2) == 0
      && finalcut::stringLength(fkey_cap.tname.data()) == 2 )  // center of keypad
      CPPUNIT_ASSERT_CSTRING ( fkey_cap.string
                             , CSI "218z" );

    if ( std::memcmp(fkey_cap.tname.data(), "kDx", 3) == 0 )  // keypad delete
      CPPUNIT_ASSERT_CSTRING ( fkey_cap.string
                             , CSI "249z" );

    if ( std::memcmp(fkey_cap.tname.data(), "@8x", 3) == 0 )  // enter/send key
      CPPUNIT_ASSERT_CSTRING ( fkey_cap.string
                             , CSI "250z" );

    if ( std::memcmp(fkey_cap.tname.data(), "KP1", 3) == 0 )  // keypad slash
      CPPUNIT_ASSERT_CSTRING ( fkey_cap.string
                             , CSI "212z" );

    if ( std::memcmp(fkey_cap.tname.data(), "KP2", 3) == 0 )  // keypad asterisk
      CPPUNIT_ASSERT_CSTRING ( fkey_cap.string
                             , CSI "213z" );

    if ( std::memcmp(fkey_cap.tname.data(), "KP3", 3) == 0 )  // keypad minus sign
      CPPUNIT_ASSERT_CSTRING ( fkey_cap.string
                             , CSI "254z" );

    if ( std::memcmp(fkey_cap.tname.data(), "KP4", 3) == 0 )  // keypad plus sign
      CPPUNIT_ASSERT_CSTRING ( fkey_cap.string
                             , CSI "253z" );
  }

  data.unsetTermType (finalcut::FTermType::sun_con);
}

//----------------------------------------------------------------------
void FTermcapQuirksTest::screenTest()
{
  auto& caps = finalcut::FTermcap::strings;
  static constexpr int last_item = int(sizeof(test::tcap)
                                 / sizeof(test::tcap[0])) - 1;

  for (std::size_t i = 0; i < last_item; i++)
  {
    caps[i].string.data   = test::tcap[i].string.data;
    caps[i].string.length = test::tcap[i].string.length;

    std::copy ( std::begin(test::tcap[i].tname)
              , std::end(test::tcap[i].tname)
              , caps[i].tname.begin() );
  }

  auto& data = finalcut::FTermData::getInstance();
  finalcut::FTermcapQuirks quirks;
  finalcut::FTermcap::can_change_color_palette = false;
  data.setTermType (finalcut::FTermType::screen);
  data.setTermType ("screen-256color");
  quirks.terminalFixup();

  CPPUNIT_ASSERT ( finalcut::FTermcap::can_change_color_palette );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_initialize_color)].string.data
                         , ESC "P" OSC "4;%p1%d;rgb:"
                           "%p2%{255}%*%{1000}%/%2.2X/"
                           "%p3%{255}%*%{1000}%/%2.2X/"
                           "%p4%{255}%*%{1000}%/%2.2X" BEL ESC "\\" );


  data.setTermType (finalcut::FTermType::tmux);

  caps[int(finalcut::Termcap::t_initialize_color)].string.data = nullptr;
  finalcut::FTermcap::can_change_color_palette = false;
  quirks.terminalFixup();

  CPPUNIT_ASSERT ( finalcut::FTermcap::can_change_color_palette );
  CPPUNIT_ASSERT_CSTRING ( caps[int(finalcut::Termcap::t_initialize_color)].string.data
                         , ESC "P" ESC OSC "4;%p1%d;#"
                           "%p2%{255}%*%{1000}%/%2.2X"
                           "%p3%{255}%*%{1000}%/%2.2X"
                           "%p4%{255}%*%{1000}%/%2.2X" BEL ESC "\\" );

  data.unsetTermType (finalcut::FTermType::tmux);
  data.unsetTermType (finalcut::FTermType::screen);
}


// private methods of FOptiMoveTest
//----------------------------------------------------------------------
auto FTermcapQuirksTest::printSequence (const std::string& s) -> std::string
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
CPPUNIT_TEST_SUITE_REGISTRATION (FTermcapQuirksTest);

// The general unit test main part
#include <main-test.inc>
