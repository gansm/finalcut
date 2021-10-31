/***********************************************************************
* ftermcap-test.cpp - FTermcap unit tests                              *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2021 Markus Gans                                           *
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

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <chrono>
#include <string>

#include <final/final.h>

// FTermcap string macro
#ifdef TCAP
  #undef TCAP
#endif
#define TCAP(...)  finalcut::FTermcap::strings[int(finalcut::Termcap::__VA_ARGS__)].string

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::high_resolution_clock;


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
// class FTermcapTest
//----------------------------------------------------------------------

class FTermcapTest : public CPPUNIT_NS::TestFixture
{
  public:
    FTermcapTest() = default;

  protected:
    void classNameTest();
    void initTest();
    void getFlagTest();
    void getNumberTest();
    void getStringTest();
    void encodeMotionParameterTest();
    void encodeParameterTest();
    void paddingPrintTest();
    void stringPrintTest();

  private:
    static int putchar_test (int);
    static int putstring_test (const std::string&);
    static void clear();

    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FTermcapTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (initTest);
    CPPUNIT_TEST (getFlagTest);
    CPPUNIT_TEST (getNumberTest);
    CPPUNIT_TEST (getStringTest);
    CPPUNIT_TEST (encodeMotionParameterTest);
    CPPUNIT_TEST (encodeParameterTest);
    CPPUNIT_TEST (paddingPrintTest);
    CPPUNIT_TEST (stringPrintTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();

    // Data member
    static std::string output;
};

// static class attribute
std::string FTermcapTest::output{};


//----------------------------------------------------------------------
void FTermcapTest::classNameTest()
{
  finalcut::FTermcap tcap;
  const finalcut::FString& classname = tcap.getClassName();
  CPPUNIT_ASSERT ( classname == "FTermcap" );
}

//----------------------------------------------------------------------
void FTermcapTest::initTest()
{
  // Without a terminal type
  auto& fterm_data = finalcut::FTermData::getInstance();
  CPPUNIT_ASSERT ( fterm_data.getTermType().empty() );
  finalcut::FTermcap tcap;
  CPPUNIT_ASSERT ( ! tcap.isInitialized() );
  tcap.init();
  CPPUNIT_ASSERT ( tcap.isInitialized() );
  tcap.setPutCharFunction (nullptr);
  tcap.setPutStringFunction (nullptr);
  CPPUNIT_ASSERT ( ! tcap.isInitialized() );
  tcap.setPutCharFunction (FTermcapTest::putchar_test);
  CPPUNIT_ASSERT ( ! tcap.isInitialized() );
  tcap.setPutStringFunction (FTermcapTest::putstring_test);
  CPPUNIT_ASSERT ( tcap.isInitialized() );
  CPPUNIT_ASSERT ( ! fterm_data.getTermType().empty() );
  CPPUNIT_ASSERT ( fterm_data.getTermType() == "xterm" );

  // With terminal type ansi
  fterm_data.setTermType("ansi");
  tcap.init();
  CPPUNIT_ASSERT ( fterm_data.getTermType() == "ansi" );

  // With a non-existent terminal type
  fterm_data.setTermType("bang!");
  tcap.init();
  CPPUNIT_ASSERT ( fterm_data.getTermType() == "xterm" );
  setenv ("TERM", "xterm-256color", 1);  // 256 color terminal
  auto& term_detection = finalcut::FTermDetection::getInstance();
  CPPUNIT_ASSERT ( ! term_detection.canDisplay256Colors() );
  term_detection.detect();
  CPPUNIT_ASSERT ( term_detection.canDisplay256Colors() );
  fterm_data.setTermType("bang!");
  tcap.init();
  CPPUNIT_ASSERT ( fterm_data.getTermType() == "xterm-256color" );
  fterm_data.setTermType("dumb");
  tcap.init();
  CPPUNIT_ASSERT ( fterm_data.getTermType() == "dumb" );
}

//----------------------------------------------------------------------
void FTermcapTest::getFlagTest()
{
  auto& fterm_data = finalcut::FTermData::getInstance();
  fterm_data.setTermType("ansi");
  finalcut::FTermcap tcap;
  tcap.init();
  tcap.setPutCharFunction (FTermcapTest::putchar_test);
  tcap.setPutStringFunction (FTermcapTest::putstring_test);
  CPPUNIT_ASSERT ( tcap.isInitialized() );
  CPPUNIT_ASSERT ( fterm_data.getTermType() == "ansi" );

  CPPUNIT_ASSERT ( tcap.getFlag("am") );    // Automatic right margin
  CPPUNIT_ASSERT ( ! tcap.getFlag("cc") );  // Can change color palette
  CPPUNIT_ASSERT ( ! tcap.getFlag("xo") );  // Xon/Xoff flow control
}

//----------------------------------------------------------------------
void FTermcapTest::getNumberTest()
{
  static constexpr int NA = -1;  // not_available
  auto& fterm_data = finalcut::FTermData::getInstance();
  fterm_data.setTermType("xterm");
  finalcut::FTermcap tcap;
  tcap.init();
  tcap.setPutCharFunction (FTermcapTest::putchar_test);
  tcap.setPutStringFunction (FTermcapTest::putstring_test);
  CPPUNIT_ASSERT ( tcap.isInitialized() );
  CPPUNIT_ASSERT ( fterm_data.getTermType() == "xterm" );

  CPPUNIT_ASSERT ( tcap.getNumber("Co") == 8 );   // Colors
  CPPUNIT_ASSERT ( tcap.getNumber("it") == 8 );   // Tab stop
  CPPUNIT_ASSERT ( tcap.getNumber("NC") == NA );  // Attr. without color
  CPPUNIT_ASSERT ( tcap.getNumber("co") == 80 );  // Columns
  CPPUNIT_ASSERT ( tcap.getNumber("li") == 24 );  // Lines
}

//----------------------------------------------------------------------
void FTermcapTest::getStringTest()
{
  auto& fterm_data = finalcut::FTermData::getInstance();
  fterm_data.setTermType("ansi");
  finalcut::FTermcap tcap;
  tcap.init();
  tcap.setPutCharFunction (FTermcapTest::putchar_test);
  tcap.setPutStringFunction (FTermcapTest::putstring_test);
  CPPUNIT_ASSERT ( tcap.isInitialized() );
  CPPUNIT_ASSERT ( fterm_data.getTermType() == "ansi" );

  CPPUNIT_ASSERT_CSTRING ( tcap.getString("me"), CSI "0m" );  // Exit attribute mode
  CPPUNIT_ASSERT_CSTRING ( tcap.getString("mr"), CSI "7m" );  // Enter reverse mode
  CPPUNIT_ASSERT_CSTRING ( tcap.getString("us"), CSI "4m" );  // Enter underline mode
  CPPUNIT_ASSERT_CSTRING ( tcap.getString("cd"), CSI "J" );   // Clear to end of screen
  CPPUNIT_ASSERT_CSTRING ( tcap.getString("ce"), CSI "K" );   // Clear to end of line
  CPPUNIT_ASSERT_CSTRING ( tcap.getString("cl"), CSI "H" CSI "J" );  // Clear screen
}

//----------------------------------------------------------------------
void FTermcapTest::encodeMotionParameterTest()
{
  auto& fterm_data = finalcut::FTermData::getInstance();
  fterm_data.setTermType("ansi");
  finalcut::FTermcap tcap;
  tcap.init();
  tcap.setPutCharFunction (FTermcapTest::putchar_test);
  tcap.setPutStringFunction (FTermcapTest::putstring_test);
  CPPUNIT_ASSERT ( tcap.isInitialized() );
  CPPUNIT_ASSERT ( fterm_data.getTermType() == "ansi" );
  const auto& cursor_address = tcap.getString("cm");
  CPPUNIT_ASSERT ( tcap.encodeMotionParameter(cursor_address, 10, 15) == CSI "16;11H" );
  CPPUNIT_ASSERT ( tcap.encodeMotionParameter(cursor_address, 25, 1) == CSI "2;26H" );
  CPPUNIT_ASSERT ( tcap.encodeMotionParameter(cursor_address, 0, 0) == CSI "1;1H" );
  CPPUNIT_ASSERT ( tcap.encodeMotionParameter(cursor_address, 79, 23) == CSI "24;80H" );
}

//----------------------------------------------------------------------
void FTermcapTest::encodeParameterTest()
{
  auto& fterm_data = finalcut::FTermData::getInstance();
  fterm_data.setTermType("ansi");
  finalcut::FTermcap tcap;
  tcap.init();
  tcap.setPutCharFunction (FTermcapTest::putchar_test);
  tcap.setPutStringFunction (FTermcapTest::putstring_test);
  CPPUNIT_ASSERT ( tcap.isInitialized() );
  CPPUNIT_ASSERT ( fterm_data.getTermType() == "ansi" );
  const auto& parm_insert_line = tcap.getString("AL");
  CPPUNIT_ASSERT ( tcap.encodeParameter(parm_insert_line, 7) == CSI "7L" );
  const auto& parm_left_cursor = tcap.getString("LE");
  CPPUNIT_ASSERT ( tcap.encodeParameter(parm_left_cursor, 3) == CSI "3D" );
  const auto& parm_right_cursor = tcap.getString("RI");
  CPPUNIT_ASSERT ( tcap.encodeParameter(parm_right_cursor, 4) == CSI "4C" );
  const auto& parm_down_cursor = tcap.getString("DO");
  CPPUNIT_ASSERT ( tcap.encodeParameter(parm_down_cursor, 12) == CSI "12B" );
  const auto& parm_up_cursor = tcap.getString("UP");
  CPPUNIT_ASSERT ( tcap.encodeParameter(parm_up_cursor, 5) == CSI "5A" );
  const auto& parm_delete_line = tcap.getString("DL");
  CPPUNIT_ASSERT ( tcap.encodeParameter(parm_delete_line, 9) == CSI "9M" );
}

//----------------------------------------------------------------------
void FTermcapTest::paddingPrintTest()
{
  finalcut::FTermcap tcap;
  setenv ("TERM", "xterm", 1);  // xterm has no padding character
  unsetenv("TERMCAP");
  auto& fterm_data = finalcut::FTermData::getInstance();
  fterm_data.setTermType("xterm");
  CPPUNIT_ASSERT ( ! tcap.xon_xoff_flow_control );
  tcap.init();
  tcap.setPutCharFunction (FTermcapTest::putchar_test);
  tcap.setPutStringFunction (FTermcapTest::putstring_test);
  CPPUNIT_ASSERT ( tcap.isInitialized() );
  CPPUNIT_ASSERT ( tcap.no_padding_char );

  // With an empty string
  CPPUNIT_ASSERT ( output.empty() );
  auto status = tcap.paddingPrint ({}, 1);
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::Error );
  CPPUNIT_ASSERT ( output.empty() );
  CPPUNIT_ASSERT ( output == "" );

  // '$' without '<'
  CPPUNIT_ASSERT ( output.empty() );
  status = tcap.paddingPrint ("12$34567", 1);
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( ! output.empty() );
  CPPUNIT_ASSERT ( output == "12$34567" );

  // No closing '>'
  output.clear();
  status = tcap.paddingPrint ("12$3$<4567", 1);
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( ! output.empty() );
  CPPUNIT_ASSERT ( output == "12$3$<4567" );

  // Without a digit
  output.clear();
  status = tcap.paddingPrint ("12$3$<x>4567", 1);
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( ! output.empty() );
  CPPUNIT_ASSERT ( output == "12$3$<x>4567" );

  // With 2 ms print delay
  output.clear();
  auto start = high_resolution_clock::now();
  status = tcap.paddingPrint ("1234$<2/>567", 1);
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  auto end = high_resolution_clock::now();
  auto duration_ms = int(duration_cast<milliseconds>(end - start).count());
  CPPUNIT_ASSERT ( duration_ms >= 2 );
  CPPUNIT_ASSERT ( ! output.empty() );
  CPPUNIT_ASSERT ( output == "1234567" );

  // With 20 ms print delay
  output.clear();
  start = high_resolution_clock::now();
  status = tcap.paddingPrint ("12$3$<45$<20/>67", 1);
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  end = high_resolution_clock::now();
  duration_ms = int(duration_cast<milliseconds>(end - start).count());
  CPPUNIT_ASSERT ( duration_ms >= 20 );
  CPPUNIT_ASSERT ( ! output.empty() );
  CPPUNIT_ASSERT ( output == "12$3$<4567" );

  // With a high a delay (9.999 seconds)
  output.clear();
  start = high_resolution_clock::now();
  status = tcap.paddingPrint ("1234$<9999/>567", 1);
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  end = high_resolution_clock::now();
  duration_ms = int(duration_cast<milliseconds>(end - start).count());
  CPPUNIT_ASSERT ( duration_ms >= 9999 );
  CPPUNIT_ASSERT ( ! output.empty() );
  CPPUNIT_ASSERT ( output == "1234567" );

  // With too high a delay (delay > 9999 ms = 9.999 sec)
  output.clear();
  start = high_resolution_clock::now();
  status = tcap.paddingPrint ("1234$<10000>567", 1);
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  end = high_resolution_clock::now();
  duration_ms = int(duration_cast<milliseconds>(end - start).count());
  CPPUNIT_ASSERT ( duration_ms < 20 );
  CPPUNIT_ASSERT ( ! output.empty() );
  CPPUNIT_ASSERT ( output == "1234$<10000>567" );

  // Beep has delayed output and flush
  output.clear();
  status = tcap.paddingPrint (TCAP(t_bell), 1);
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( ! output.empty() );
  CPPUNIT_ASSERT ( output == std::string(TCAP(t_bell)) );

  // Flash screen has delayed output and flush
  output.clear();
  start = high_resolution_clock::now();
  status = tcap.paddingPrint ("\033[?5h$<100/>\033[?5l", 1);
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  end = high_resolution_clock::now();
  duration_ms = int(duration_cast<milliseconds>(end - start).count());
  CPPUNIT_ASSERT ( duration_ms >= 100 );
  CPPUNIT_ASSERT ( ! output.empty() );
  CPPUNIT_ASSERT ( output == std::string("\033[?5h\033[?5l") );

  // With 5 ms prints 21 padding chars ('\0')
  setenv ("TERM", "ansi", 1);  // ansi terminals used for delay padding character
  fterm_data.setTermType("ansi");
  tcap.init();
  tcap.setPutCharFunction (FTermcapTest::putchar_test);
  tcap.setPutStringFunction (FTermcapTest::putstring_test);
  CPPUNIT_ASSERT ( ! tcap.no_padding_char );
  CPPUNIT_ASSERT ( ! tcap.xon_xoff_flow_control );
  output.clear();
  tcap.setBaudrate (38400);
  status = tcap.paddingPrint ("1234$<5*/>567", 1);
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  const auto& str1_with_0 = "1234" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" "567";
  std::string target_output(std::begin(str1_with_0), std::end(str1_with_0) - 1);
  CPPUNIT_ASSERT ( ! output.empty() );
  CPPUNIT_ASSERT ( output.length() == 28 );
  CPPUNIT_ASSERT ( output == target_output );

  // Wait 2 ms with padding characters and with 1 affected line
  output.clear();
  tcap.setBaudrate (9600);
  tcap.padding_baudrate = 0;  // no padding baudrate
  status = tcap.paddingPrint ("abc$<2*>def", 1);
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( output.length() == 6 );
  CPPUNIT_ASSERT ( output == "abcdef" );
  output.clear();
  status = tcap.paddingPrint ("abc$<2*/>def", 1);  // with "*/"
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  const auto& str2_with_0 = "abc" "\0\0" "def";
  target_output = std::string(std::begin(str2_with_0), std::end(str2_with_0) - 1);
  CPPUNIT_ASSERT ( output.length() == 8 );
  CPPUNIT_ASSERT ( output == target_output );
  output.clear();
  status = tcap.paddingPrint ("abc$<2/*>def", 1);  // with "/*"
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( output.length() == 8 );
  CPPUNIT_ASSERT ( output == target_output );
  output.clear();
  status = tcap.paddingPrint ("abc$<2/x*>def", 1);  // defekt
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( output.length() == 13 );
  CPPUNIT_ASSERT ( output == "abc$<2/x*>def" );
  output.clear();
  status = tcap.paddingPrint ("abc$<2/*x>def", 1);  // defekt
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( output.length() == 13 );
  CPPUNIT_ASSERT ( output == "abc$<2/*x>def" );
  output.clear();
  tcap.padding_baudrate = 19200;  // baudrate < padding baudrate
  status = tcap.paddingPrint ("abc$<2*>def", 1);
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( output.length() == 6 );
  CPPUNIT_ASSERT ( output == "abcdef" );
  output.clear();
  status = tcap.paddingPrint ("abc$<2*/>def", 1);  // with "*/"
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( output.length() == 8 );
  CPPUNIT_ASSERT ( output == target_output );
  output.clear();
  tcap.padding_baudrate = 9600;  // baudrate >= padding baudrate
  status = tcap.paddingPrint ("abc$<2*>def", 1);
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( output.length() == 8 );
  CPPUNIT_ASSERT ( output == target_output );

  // Wait 2 ms with padding characters and with 2 affected line
  output.clear();
  tcap.padding_baudrate = 0;  // no padding baudrate
  status = tcap.paddingPrint ("abc$<2*>>def", 2);  // double >>
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( output.length() == 7 );
  CPPUNIT_ASSERT ( output == "abc>def" );
  output.clear();
  status = tcap.paddingPrint ("abc$<2*/>def", 2);  // with "*/"
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  const auto& str3_with_0 = "abc" "\0\0\0\0" "def";
  target_output = std::string(std::begin(str3_with_0), std::end(str3_with_0) - 1);
  CPPUNIT_ASSERT ( output.length() == 10 );
  CPPUNIT_ASSERT ( output == target_output );
  output.clear();
  status = tcap.paddingPrint ("abc$<2/*>def", 2);  // with "/*"
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( output.length() == 10 );
  CPPUNIT_ASSERT ( output == target_output );
  output.clear();
  tcap.padding_baudrate = 19200;  // baudrate < padding baudrate
  status = tcap.paddingPrint ("abc$<2*>def", 2);
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( output.length() == 6 );
  CPPUNIT_ASSERT ( output == "abcdef" );
  output.clear();
  status = tcap.paddingPrint ("abc$<2*/>def", 2);  // with "*/"
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( output.length() == 10 );
  CPPUNIT_ASSERT ( output == target_output );
  output.clear();
  status = tcap.paddingPrint ("abc$<2/*>def", 2);  // with "/*"
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( output.length() == 10 );
  CPPUNIT_ASSERT ( output == target_output );
  output.clear();
  tcap.padding_baudrate = 9600;  // baudrate >= padding baudrate
  status = tcap.paddingPrint ("abc$<2*>def", 2);
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( output.length() == 10 );
  CPPUNIT_ASSERT ( output == target_output );

  // Wait 2 ms with padding characters and with 3 affected line
  output.clear();
  tcap.padding_baudrate = 0;  // no padding baudrate
  status = tcap.paddingPrint ("abc$<2*>def", 3);
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( output == "abcdef" );
  output.clear();
  status = tcap.paddingPrint ("abc$<2*/>def", 3);  // with "*/"
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  const auto& str4_with_0 = "abc" "\0\0\0\0\0\0" "def";
  target_output = std::string(std::begin(str4_with_0), std::end(str4_with_0) - 1);
  CPPUNIT_ASSERT ( output.length() == 12 );
  CPPUNIT_ASSERT ( output == target_output );
  output.clear();
  status = tcap.paddingPrint ("abc$<2/*>def", 3);  // with "/*"
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( output.length() == 12 );
  CPPUNIT_ASSERT ( output == target_output );
  output.clear();
  tcap.padding_baudrate = 19200;  // baudrate < padding baudrate
  status = tcap.paddingPrint ("abc$<2*>def", 3);
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( output.length() == 6 );
  CPPUNIT_ASSERT ( output == "abcdef" );
  output.clear();
  status = tcap.paddingPrint ("abc$<2*/>def", 3);  // with "*/"
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( output.length() == 12 );
  CPPUNIT_ASSERT ( output == target_output );
  output.clear();
  status = tcap.paddingPrint ("abc$<2/*>def", 3);  // with "/*"
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( output.length() == 12 );
  CPPUNIT_ASSERT ( output == target_output );
  output.clear();
  tcap.padding_baudrate = 9600;  // baudrate >= padding baudrate
  status = tcap.paddingPrint ("abc$<2*>def", 3);
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( output.length() == 12 );
  CPPUNIT_ASSERT ( output == target_output );

  // With decimal point
  output.clear();
  tcap.setBaudrate(19200);
  tcap.padding_baudrate = 19200;  // baudrate == padding baudrate
  status = tcap.paddingPrint ("abc$<2.>def", 1);  // 2.
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  const auto& str5_with_0 = "abc" "\0\0\0\0" "def";
  target_output = std::string(std::begin(str5_with_0), std::end(str5_with_0) - 1);
  CPPUNIT_ASSERT ( output.length() == 10 );
  CPPUNIT_ASSERT ( output == target_output );
  output.clear();
  status = tcap.paddingPrint ("abc$<2.0>def", 1);  // 2.0
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( output.length() == 10 );
  CPPUNIT_ASSERT ( output == target_output );
  output.clear();
  status = tcap.paddingPrint ("abc$<2.7>def", 1);  // 2.7
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( output.length() == 10 );
  CPPUNIT_ASSERT ( output == target_output );
  output.clear();
  status = tcap.paddingPrint ("abc$<2.77>def", 1);  // 2.77
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( output.length() == 10 );
  CPPUNIT_ASSERT ( output == target_output );
  output.clear();
}

//----------------------------------------------------------------------
void FTermcapTest::stringPrintTest()
{
  finalcut::FTermcap tcap;
  tcap.init();
  tcap.setPutStringFunction (nullptr);
  CPPUNIT_ASSERT ( ! tcap.isInitialized() );
  auto status = tcap.stringPrint ("Hello, World!");
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::Error );
  tcap.setPutCharFunction (FTermcapTest::putchar_test);
  tcap.setPutStringFunction (FTermcapTest::putstring_test);
  CPPUNIT_ASSERT ( tcap.isInitialized() );

  // With an empty string
  CPPUNIT_ASSERT ( output.empty() );
  status = tcap.stringPrint ({});
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::Error );
  CPPUNIT_ASSERT ( output.empty() );
  CPPUNIT_ASSERT ( output == "" );
  status = tcap.stringPrint ("");
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::Error );
  CPPUNIT_ASSERT ( output.empty() );
  CPPUNIT_ASSERT ( output == "" );
  status = tcap.stringPrint (std::string());
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::Error );
  CPPUNIT_ASSERT ( output.empty() );
  CPPUNIT_ASSERT ( output == "" );
  status = tcap.stringPrint (std::string(""));
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::Error );
  CPPUNIT_ASSERT ( output.empty() );
  CPPUNIT_ASSERT ( output == "" );

  // With string data
  status = tcap.stringPrint ("Hello");
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( ! output.empty() );
  CPPUNIT_ASSERT ( output == "Hello" );
  status = tcap.stringPrint (", World!");
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( output == "Hello, World!" );
  output.clear();
  status = tcap.stringPrint ("A open book 📖");
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( ! output.empty() );
  CPPUNIT_ASSERT ( output == "A open book 📖" );
  // Add with paddingPrint
  status = tcap.paddingPrint (std::string(1, '!'), 1);
  CPPUNIT_ASSERT ( status == finalcut::FTermcap::Status::OK );
  CPPUNIT_ASSERT ( output == "A open book 📖!" );
}

//----------------------------------------------------------------------
int FTermcapTest::putchar_test (int ch)
{
  //std::cout << std::hex << "0x" << ch << "," << std::flush;
  output.push_back(char(ch));
  return ch;
}

//----------------------------------------------------------------------
int FTermcapTest::putstring_test (const std::string& str)
{
  //std::cout << '"' << str << '"' << std::flush;
  output.append(str);
  return str.length();
}

//----------------------------------------------------------------------
void FTermcapTest::clear()
{
  output.clear();
}

//----------------------------------------------------------------------

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FTermcapTest);

// The general unit test main part
#include <main-test.inc>
