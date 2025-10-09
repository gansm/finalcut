/***********************************************************************
* fstring-test.cpp - FString unit tests                                *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2025 Markus Gans                                      *
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

#include <langinfo.h>
#include <unistd.h>
#define __STDC_LIMIT_MACROS
#include <cstdint>
#include <clocale>
#include <iomanip>
#include <string>
#include <utility>
#include <vector>

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

#define CPPUNIT_ASSERT_WCSTRING(expected, actual) \
            check_c_wstring (expected, actual, CPPUNIT_SOURCELINE())

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
void check_c_wstring ( const wchar_t* s1
                     , const wchar_t* s2
                     , const CppUnit::SourceLine& sourceLine )
{
  if ( s1 == nullptr && s2 == nullptr )  // Strings are equal
    return;

  if ( s1 && s2 && std::wcscmp (s1, s2) == 0 )  // Strings are equal
      return;

  ::CppUnit::Asserter::fail ("Strings are not equal", sourceLine);
}


//----------------------------------------------------------------------
// class FStringTest
//----------------------------------------------------------------------

class FStringTest : public CPPUNIT_NS::TestFixture
{
  public:
    FStringTest() = default;

    void setUp() override;
    void tearDown() override;

  protected:
    void classNameTest();
    void noArgumentTest();
    void initLengthTest();
    void capacityTest();
    void copyConstructorTest();
    void moveConstructorTest();
    void assignmentTest();
    void additionAssignmentTest();
    void additionTest();
    void caseTest();
    void equalTest();
    void notEqualTest();
    void lessEqualTest();
    void lessTest();
    void greaterEqualTest();
    void greaterTest();
    void streamInsertionTest();
    void streamExtractionTest();
    void subscriptOperatorTest();
    void iteratorTest();
    void functionCallOperatorTest();
    void formatTest();
    void convertToNumberTest();
    void convertFromNumberTest();
    void exceptionTest();
    void trimTest();
    void subStringTest();
    void insertTest();
    void replaceTest();
    void overwriteTest();
    void removeTest();
    void includesTest();
    void controlCodesTest();
    void caseCompareTest();
    void swapTest();
    void hashTest();

  private:
    finalcut::FString* s{nullptr};

    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FStringTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (noArgumentTest);
    CPPUNIT_TEST (initLengthTest);
    CPPUNIT_TEST (capacityTest);
    CPPUNIT_TEST (copyConstructorTest);
    CPPUNIT_TEST (moveConstructorTest);
    CPPUNIT_TEST (assignmentTest);
    CPPUNIT_TEST (additionAssignmentTest);
    CPPUNIT_TEST (additionTest);
    CPPUNIT_TEST (caseTest);
    CPPUNIT_TEST (equalTest);
    CPPUNIT_TEST (notEqualTest);
    CPPUNIT_TEST (lessEqualTest);
    CPPUNIT_TEST (lessTest);
    CPPUNIT_TEST (greaterEqualTest);
    CPPUNIT_TEST (greaterTest);
    CPPUNIT_TEST (streamInsertionTest);
    CPPUNIT_TEST (streamExtractionTest);
    CPPUNIT_TEST (subscriptOperatorTest);
    CPPUNIT_TEST (iteratorTest);
    CPPUNIT_TEST (functionCallOperatorTest);
    CPPUNIT_TEST (formatTest);
    CPPUNIT_TEST (convertToNumberTest);
    CPPUNIT_TEST (convertFromNumberTest);
    CPPUNIT_TEST (exceptionTest);
    CPPUNIT_TEST (trimTest);
    CPPUNIT_TEST (subStringTest);
    CPPUNIT_TEST (insertTest);
    CPPUNIT_TEST (replaceTest);
    CPPUNIT_TEST (overwriteTest);
    CPPUNIT_TEST (removeTest);
    CPPUNIT_TEST (includesTest);
    CPPUNIT_TEST (controlCodesTest);
    CPPUNIT_TEST (caseCompareTest);
    CPPUNIT_TEST (swapTest);
    CPPUNIT_TEST (hashTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};


//----------------------------------------------------------------------
void FStringTest::setUp()
{
  std::setlocale(LC_CTYPE, "");
  s = new finalcut::FString('c');
}

//----------------------------------------------------------------------
void FStringTest::tearDown()
{
  delete s;
}

//----------------------------------------------------------------------
void FStringTest::classNameTest()
{
  finalcut::FString str;
  const finalcut::FString& classname = str.getClassName();
  CPPUNIT_ASSERT ( classname == "FString" );
}

//----------------------------------------------------------------------
void FStringTest::noArgumentTest()
{
  finalcut::FString empty;
  CPPUNIT_ASSERT ( empty.isEmpty() );
  CPPUNIT_ASSERT ( empty.getLength() == 0 );
  CPPUNIT_ASSERT ( empty.length() == 0 );
  CPPUNIT_ASSERT ( empty.size() == 0 );
  CPPUNIT_ASSERT ( empty.capacity() < std::wstring().max_size() );
  CPPUNIT_ASSERT ( empty.max_size() == std::wstring().max_size() );
  CPPUNIT_ASSERT ( empty.wc_str()[0] == L'\0' );
  CPPUNIT_ASSERT ( empty.data()[0] == L'\0' );
  CPPUNIT_ASSERT ( empty.c_str()[0] == '\0' );
  CPPUNIT_ASSERT_EQUAL ( empty.toString(), std::string() );
  CPPUNIT_ASSERT ( strlen(finalcut::FString(99).c_str()) == 0 );
  CPPUNIT_ASSERT ( wcslen(finalcut::FString(99).wc_str()) == 0 );
  CPPUNIT_ASSERT ( wcslen(finalcut::FString(99).data()) == 0 );
  CPPUNIT_ASSERT ( strlen(finalcut::FString("").c_str()) == 0 );
  CPPUNIT_ASSERT ( wcslen(finalcut::FString("").wc_str()) == 0 );
  CPPUNIT_ASSERT ( wcslen(finalcut::FString("").data()) == 0 );

  char* cstr = empty.c_str();
  CPPUNIT_ASSERT ( cstr[0] == '\0' );
  CPPUNIT_ASSERT_CSTRING ( cstr, "" );
  wchar_t* wcstr = empty.wc_str();
  CPPUNIT_ASSERT_WCSTRING ( wcstr, L"" );
  CPPUNIT_ASSERT ( wcstr[0] == L'\0' );
  const wchar_t* data = empty.data();
  CPPUNIT_ASSERT_WCSTRING ( data, L"" );
  CPPUNIT_ASSERT ( data[0] == L'\0' );
  std::string str = empty.toString();
  CPPUNIT_ASSERT ( str.length() == 0 );
  CPPUNIT_ASSERT ( str.size() == 0 );
  CPPUNIT_ASSERT ( str.empty() );
  const finalcut::FString fstr = str;
  CPPUNIT_ASSERT ( fstr.isEmpty() );
  CPPUNIT_ASSERT ( fstr.capacity() < std::wstring().max_size() );

  cstr = nullptr;
  CPPUNIT_ASSERT ( empty == cstr );
  CPPUNIT_ASSERT ( finalcut::FString(std::string()).isEmpty() );
  CPPUNIT_ASSERT ( finalcut::FString(char(0)).isEmpty() );
  wcstr = nullptr;
  CPPUNIT_ASSERT ( empty == wcstr );
  CPPUNIT_ASSERT ( finalcut::FString(std::wstring()).isEmpty() );
  CPPUNIT_ASSERT ( finalcut::FString(wchar_t(0)).isEmpty() );

  CPPUNIT_ASSERT ( ! empty.includes('A') );
  CPPUNIT_ASSERT ( ! empty.includes(L'A') );
  CPPUNIT_ASSERT ( ! empty.includes("123") );
  CPPUNIT_ASSERT ( ! empty.includes(L"123") );
  CPPUNIT_ASSERT ( ! empty.includes(std::string("123")) );
  CPPUNIT_ASSERT ( ! empty.includes(std::wstring(L"123")) );
  CPPUNIT_ASSERT ( ! empty.includes(finalcut::FString("123")) );

  std::ostringstream out;
  out << empty;
  CPPUNIT_ASSERT ( out.str() == "" );

  // Fill the empty string with "123"
  empty << "123";
  CPPUNIT_ASSERT_EQUAL ( empty, finalcut::FString(L"123") );

  empty.clear();
  CPPUNIT_ASSERT ( empty.isEmpty() );
}

//----------------------------------------------------------------------
void FStringTest::initLengthTest()
{
  const finalcut::FString s1(0);
  CPPUNIT_ASSERT ( s1.getLength() == 0 );
  CPPUNIT_ASSERT ( s1.size() == 0 );
  CPPUNIT_ASSERT ( s1.length() == 0 );
  CPPUNIT_ASSERT ( s1.capacity() < std::wstring().max_size() );
  CPPUNIT_ASSERT ( s1.max_size() == std::wstring().max_size() );
  CPPUNIT_ASSERT ( s1.isEmpty() );

  constexpr int         x1 = 10;
  constexpr std::size_t x2 = 10;
  const finalcut::FString s2(x1);
  CPPUNIT_ASSERT ( s2.getLength() == 10 );
  CPPUNIT_ASSERT ( s2.size() == 10 );
  CPPUNIT_ASSERT ( s2.length() == 10 );
  CPPUNIT_ASSERT ( s2.capacity() >= 10 );
  CPPUNIT_ASSERT ( s2.max_size() == std::wstring().max_size() );
  CPPUNIT_ASSERT ( ! s2.isEmpty() );

  const finalcut::FString s3(x2);
  CPPUNIT_ASSERT ( s3.getLength() == 10 );
  CPPUNIT_ASSERT ( s3.size() == 10 );
  CPPUNIT_ASSERT ( s3.length() == 10 );
  CPPUNIT_ASSERT ( s3.capacity() >= 10 );
  CPPUNIT_ASSERT ( s3.max_size() == std::wstring().max_size() );
  CPPUNIT_ASSERT ( ! s3.isEmpty() );

  const finalcut::FString s4(0, L'-');
  CPPUNIT_ASSERT ( s4.getLength() == 0 );
  CPPUNIT_ASSERT ( s4.size() == 0 );
  CPPUNIT_ASSERT ( s4.length() == 0 );
  CPPUNIT_ASSERT ( s4.capacity() < std::wstring().max_size() );
  CPPUNIT_ASSERT ( s4.max_size() == std::wstring().max_size() );
  CPPUNIT_ASSERT ( s4.isEmpty() );

  const finalcut::FString s5(0, '-');
  CPPUNIT_ASSERT ( s5.getLength() == 0 );
  CPPUNIT_ASSERT ( s5.size() == 0 );
  CPPUNIT_ASSERT ( s5.length() == 0 );
  CPPUNIT_ASSERT ( s5.capacity() < std::wstring().max_size() );
  CPPUNIT_ASSERT ( s5.max_size() == std::wstring().max_size() );
  CPPUNIT_ASSERT ( s5.isEmpty() );

  const finalcut::FString s6(0, char(0));
  CPPUNIT_ASSERT ( s6.getLength() == 0 );
  CPPUNIT_ASSERT ( s6.size() == 0 );
  CPPUNIT_ASSERT ( s6.length() == 0 );
  CPPUNIT_ASSERT ( s6.capacity() < std::wstring().max_size() );
  CPPUNIT_ASSERT ( s6.max_size() == std::wstring().max_size() );
  CPPUNIT_ASSERT ( s6.isEmpty() );

  const finalcut::FString s7(x1, '-');
  CPPUNIT_ASSERT ( s7.getLength() == 10 );
  CPPUNIT_ASSERT ( s7.size() == 10 );
  CPPUNIT_ASSERT ( s7.length() == 10 );
  CPPUNIT_ASSERT ( s7.capacity() >= 10 );
  CPPUNIT_ASSERT ( s7.max_size() == std::wstring().max_size() );
  CPPUNIT_ASSERT ( ! s7.isEmpty() );

  const finalcut::FString s8(x2, '-');
  CPPUNIT_ASSERT ( s8.getLength() == 10 );
  CPPUNIT_ASSERT ( s8.size() == 10 );
  CPPUNIT_ASSERT ( s8.length() == 10 );
  CPPUNIT_ASSERT ( s8.capacity() >= 10 );
  CPPUNIT_ASSERT ( s8.max_size() == std::wstring().max_size() );
  CPPUNIT_ASSERT ( ! s8.isEmpty() );

  const finalcut::FString s9(x1, L'-');
  CPPUNIT_ASSERT ( s9.getLength() == 10 );
  CPPUNIT_ASSERT ( s9.size() == 10 );
  CPPUNIT_ASSERT ( s9.length() == 10 );
  CPPUNIT_ASSERT ( s9.capacity() >= 10 );
  CPPUNIT_ASSERT ( s9.max_size() == std::wstring().max_size() );
  CPPUNIT_ASSERT ( ! s9.isEmpty() );

  const finalcut::FString s10(x2, L'-');
  CPPUNIT_ASSERT ( s10.getLength() == 10 );
  CPPUNIT_ASSERT ( s10.size() == 10 );
  CPPUNIT_ASSERT ( s10.length() == 10 );
  CPPUNIT_ASSERT ( s10.capacity() >= 10 );
  CPPUNIT_ASSERT ( s10.max_size() == std::wstring().max_size() );
  CPPUNIT_ASSERT ( ! s10.isEmpty() );

  const finalcut::FString s11(x2, wchar_t(0));
  CPPUNIT_ASSERT ( s11.getLength() == 10 );
  CPPUNIT_ASSERT ( s11.size() == 10 );
  CPPUNIT_ASSERT ( s11.length() == 10 );
  CPPUNIT_ASSERT ( s11.capacity() >= 10 );
  CPPUNIT_ASSERT ( s11.max_size() == std::wstring().max_size() );
  CPPUNIT_ASSERT ( ! s11.isEmpty() );

  const finalcut::FString s12(x1, finalcut::UniChar::Pound);
  CPPUNIT_ASSERT ( s12.getLength() == 10 );
  CPPUNIT_ASSERT ( s12.size() == 10 );
  CPPUNIT_ASSERT ( s12.length() == 10 );
  CPPUNIT_ASSERT ( s12.capacity() >= 10 );
  CPPUNIT_ASSERT ( s12.max_size() == std::wstring().max_size() );
  CPPUNIT_ASSERT ( ! s12.isEmpty() );

  const finalcut::FString s13(x2, finalcut::UniChar::BlackDiamondSuit);
  CPPUNIT_ASSERT ( s13.getLength() == 10 );
  CPPUNIT_ASSERT ( s13.size() == 10 );
  CPPUNIT_ASSERT ( s13.length() == 10 );
  CPPUNIT_ASSERT ( s13.capacity() >= 10 );
  CPPUNIT_ASSERT ( s13.max_size() == std::wstring().max_size() );
  CPPUNIT_ASSERT ( ! s13.isEmpty() );
}

//----------------------------------------------------------------------
void FStringTest::capacityTest()
{
  finalcut::FString str{};
  const std::string::size_type start_cap{101u};
  str.reserve(start_cap);
  CPPUNIT_ASSERT ( str.capacity() >= start_cap );

  const auto temp_cap = str.capacity();
  str.resize(1010u);
  CPPUNIT_ASSERT ( str.capacity() >= start_cap );
  CPPUNIT_ASSERT ( str.capacity() >= temp_cap );

  str.resize(10u, L'🤔');
  CPPUNIT_ASSERT ( str.capacity() >= start_cap );
  CPPUNIT_ASSERT ( str.capacity() >= temp_cap );

  str.shrink_to_fit();
  CPPUNIT_ASSERT ( str.capacity() <= temp_cap );
  CPPUNIT_ASSERT ( str.capacity() <= start_cap );
}

//----------------------------------------------------------------------
void FStringTest::copyConstructorTest()
{
  finalcut::FString s1("abc");
  const finalcut::FString s2(s1);
  s1.clear();
  CPPUNIT_ASSERT ( s2 == L"abc" );
  CPPUNIT_ASSERT_CSTRING ( s2.c_str(), "abc" );
  CPPUNIT_ASSERT_WCSTRING ( s2.wc_str(), L"abc" );
  CPPUNIT_ASSERT_WCSTRING ( s2.data(), L"abc" );
  CPPUNIT_ASSERT ( s2.getLength() == 3 );
  CPPUNIT_ASSERT ( s2.capacity() >= 3 );
}

//----------------------------------------------------------------------
void FStringTest::moveConstructorTest()
{
  finalcut::FString s1("abc");
  const finalcut::FString s2{std::move(s1)};
  CPPUNIT_ASSERT ( s2 == L"abc" );
  CPPUNIT_ASSERT_CSTRING ( s2.c_str(), "abc" );
  CPPUNIT_ASSERT_WCSTRING ( s2.wc_str(), L"abc" );
  CPPUNIT_ASSERT_WCSTRING ( s2.data(), L"abc" );
  CPPUNIT_ASSERT ( s2.getLength() == 3 );
  CPPUNIT_ASSERT ( s2.capacity() >= 3 );
}

//----------------------------------------------------------------------
void FStringTest::assignmentTest()
{
  finalcut::FString s1;
  s1 = static_cast<finalcut::FString>(0);
  CPPUNIT_ASSERT ( ! s1 );
  CPPUNIT_ASSERT ( s1.isEmpty() );

  s1 = std::wstring();
  CPPUNIT_ASSERT ( ! s1 );
  CPPUNIT_ASSERT ( s1.isEmpty() );

#if __cplusplus >= 201703L
  s1 = std::wstring_view();
  CPPUNIT_ASSERT ( ! s1 );
  CPPUNIT_ASSERT ( s1.isEmpty() );
#endif

  s1 = std::string();
  CPPUNIT_ASSERT ( ! s1 );
  CPPUNIT_ASSERT ( s1.isEmpty() );

#if __cplusplus >= 201703L
  s1 = std::string_view();
  CPPUNIT_ASSERT ( ! s1 );
  CPPUNIT_ASSERT ( s1.isEmpty() );
#endif

  s1 = static_cast<wchar_t*>(nullptr);
  CPPUNIT_ASSERT ( ! s1 );
  CPPUNIT_ASSERT ( s1.isEmpty() );

  s1 = static_cast<char*>(nullptr);
  CPPUNIT_ASSERT ( ! s1 );
  CPPUNIT_ASSERT ( s1.isEmpty() );

  s1 = wchar_t(0);
  CPPUNIT_ASSERT ( ! s1 );
  CPPUNIT_ASSERT ( s1.isEmpty() );

  s1 = char(0);
  CPPUNIT_ASSERT ( ! s1 );
  CPPUNIT_ASSERT ( s1.isEmpty() );

  const finalcut::FString s2("abc");
  s1 = s2;
  CPPUNIT_ASSERT ( s1 );
  CPPUNIT_ASSERT ( s1 == L"abc" );
  CPPUNIT_ASSERT_CSTRING ( s1.c_str(), "abc" );
  CPPUNIT_ASSERT_WCSTRING ( s1.wc_str(), L"abc" );
  CPPUNIT_ASSERT ( s1.getLength() == 3 );
  CPPUNIT_ASSERT ( s1.capacity() >= 3 );

  const std::wstring s3(L"def");
  s1 = s3;
  CPPUNIT_ASSERT ( s1 );
  CPPUNIT_ASSERT ( s1 == L"def" );
  CPPUNIT_ASSERT_CSTRING ( s1.c_str(), "def" );
  CPPUNIT_ASSERT_WCSTRING ( s1.wc_str(), L"def" );
  CPPUNIT_ASSERT ( s1.getLength() == 3 );
  CPPUNIT_ASSERT ( s1.capacity() >= 3 );

#if __cplusplus >= 201703L
  const std::wstring_view s4(L"wsv");
  s1 = s4;
  CPPUNIT_ASSERT ( s1 );
  CPPUNIT_ASSERT ( s1 == L"wsv" );
  CPPUNIT_ASSERT ( s1.getLength() == 3 );
  CPPUNIT_ASSERT ( s1.capacity() >= 3 );
#endif

  const std::string s5("ghi");
  s1 = s5;
  CPPUNIT_ASSERT ( s1 );
  CPPUNIT_ASSERT ( s1 == L"ghi" );
  CPPUNIT_ASSERT ( s1.getLength() == 3 );
  CPPUNIT_ASSERT ( s1.capacity() >= 3 );

#if __cplusplus >= 201703L
  const std::string_view s6("svi");
  s1 = s6;
  CPPUNIT_ASSERT ( s1 );
  CPPUNIT_ASSERT ( s1 == L"svi" );
  CPPUNIT_ASSERT ( s1.getLength() == 3 );
  CPPUNIT_ASSERT ( s1.capacity() >= 3 );
#endif

  constexpr wchar_t s7[] = L"abc";
  s1 = s7;
  CPPUNIT_ASSERT ( s1 );
  CPPUNIT_ASSERT ( s1 == L"abc" );
  CPPUNIT_ASSERT ( s1.getLength() == 3 );
  CPPUNIT_ASSERT ( s1.capacity() >= 3 );

  constexpr char s8[] = "def";
  s1 = s8;
  CPPUNIT_ASSERT ( s1 );
  CPPUNIT_ASSERT ( s1 == L"def" );
  CPPUNIT_ASSERT ( s1.getLength() == 3 );
  CPPUNIT_ASSERT ( s1.capacity() >= 3 );

  constexpr wchar_t s9 = L'#';
  s1 = s9;
  CPPUNIT_ASSERT ( s1 );
  CPPUNIT_ASSERT ( s1 == L"#" );
  CPPUNIT_ASSERT ( s1.getLength() == 1 );
  CPPUNIT_ASSERT ( s1.capacity() >= 1 );

  constexpr char s10 = '%';
  s1 = s10;
  CPPUNIT_ASSERT ( s1 );
  CPPUNIT_ASSERT ( s1 == L"%" );
  CPPUNIT_ASSERT ( s1.getLength() == 1 );
  CPPUNIT_ASSERT ( s1.capacity() >= 1 );

  s1.setString("A character string");
  CPPUNIT_ASSERT ( s1 );
  CPPUNIT_ASSERT ( s1 == "A character string" );

  s1.setString(L"A wide character string");
  // Set the identical content again.
  // Requires no string replacement (new string = string)
  s1.setString(L"A wide character string");
  CPPUNIT_ASSERT ( s1 );
  CPPUNIT_ASSERT ( s1 == L"A wide character string" );

  s1.setString("");
  CPPUNIT_ASSERT ( ! s1 );

  s1.setString(L"");
  CPPUNIT_ASSERT ( ! s1 );

  s1.setString("");
  CPPUNIT_ASSERT ( s1 == "" );

  s1.setString(L"");
  CPPUNIT_ASSERT ( s1 == L"" );

  s1.setString("");
  CPPUNIT_ASSERT ( s1.isEmpty() );

  s1.setString(L"");
  CPPUNIT_ASSERT ( s1.isEmpty() );

  s1.setString("");
  CPPUNIT_ASSERT ( s1.isEmpty() );

  s1.setString(L"");
  CPPUNIT_ASSERT ( s1.isEmpty() );

  constexpr wchar_t* wc = nullptr;
  s1.setString(wc);
  CPPUNIT_ASSERT ( s1.isEmpty() );
  CPPUNIT_ASSERT ( ! s1 );

  constexpr char* c = nullptr;
  s1.setString(c);
  CPPUNIT_ASSERT ( s1.isEmpty() );
  CPPUNIT_ASSERT ( ! s1 );

  // Move assignment operator
  auto empty = finalcut::FString(0);
  const finalcut::FString s11 = std::move(empty);
  CPPUNIT_ASSERT ( ! s11 );
  CPPUNIT_ASSERT ( s11.isEmpty() );

  finalcut::FString s12("abc");
  const finalcut::FString s13 = std::move(s12);
  CPPUNIT_ASSERT ( s13 );
  CPPUNIT_ASSERT ( s13 == L"abc" );
  CPPUNIT_ASSERT ( s13.getLength() == 3 );
  CPPUNIT_ASSERT ( s13.capacity() >= 3 );
}

//----------------------------------------------------------------------
void FStringTest::additionAssignmentTest()
{
  finalcut::FString s1;
  s1 += finalcut::FString("abc");
  CPPUNIT_ASSERT ( s1 == L"abc" );
  s1 += finalcut::FString("def");
  CPPUNIT_ASSERT ( s1 == L"abcdef" );
  s1 += finalcut::FString();
  CPPUNIT_ASSERT ( s1 == L"abcdef" );
  s1 += finalcut::FString("");
  CPPUNIT_ASSERT ( s1 == L"abcdef" );

  s1.clear();
  CPPUNIT_ASSERT ( s1.isEmpty() );
  s1 += std::wstring(L"abc");
  CPPUNIT_ASSERT ( s1 == L"abc" );
  s1 += std::wstring(L"def");
  CPPUNIT_ASSERT ( s1 == L"abcdef" );

  s1.clear();
  s1 += const_cast<wchar_t*>(L"abc");
  CPPUNIT_ASSERT ( s1 == L"abc" );
  s1 += const_cast<wchar_t*>(L"def");
  CPPUNIT_ASSERT ( s1 == L"abcdef" );

  s1.clear();
  s1 += std::wstring(L"abc");
  CPPUNIT_ASSERT ( s1 == L"abc" );
  s1 += std::wstring(L"def");
  CPPUNIT_ASSERT ( s1 == L"abcdef" );

#if __cplusplus >= 201703L
  s1.clear();
  s1 += std::wstring_view(L"abc");
  CPPUNIT_ASSERT ( s1 == L"abc" );
  s1 += std::wstring_view(L"def");
  CPPUNIT_ASSERT ( s1 == L"abcdef" );
#endif

  s1.clear();
  s1 += std::string("abc");
  CPPUNIT_ASSERT ( s1 == L"abc" );
  s1 += std::string("def");
  CPPUNIT_ASSERT ( s1 == L"abcdef" );

#if __cplusplus >= 201703L
  s1.clear();
  s1 += std::string_view("abc");
  CPPUNIT_ASSERT ( s1 == L"abc" );
  s1 += std::string_view("def");
  CPPUNIT_ASSERT ( s1 == L"abcdef" );
#endif

  s1.clear();
  s1 += const_cast<char*>("abc");
  CPPUNIT_ASSERT ( s1 == L"abc" );
  s1 += const_cast<char*>("def");
  CPPUNIT_ASSERT ( s1 == L"abcdef" );

  s1.clear();
  s1 += wchar_t('a');
  CPPUNIT_ASSERT ( s1 == L"a" );
  s1 += wchar_t('b');
  CPPUNIT_ASSERT ( s1 == L"ab" );

  s1.clear();
  s1 += char('a');
  CPPUNIT_ASSERT ( s1 == L"a" );
  s1 += char('b');
  CPPUNIT_ASSERT ( s1 == L"ab" );
}

//----------------------------------------------------------------------
void FStringTest::additionTest()
{
  // const finalcut::FString + ...
  const finalcut::FString s1("abc");
  CPPUNIT_ASSERT ( s1.getLength() == 3 );
  CPPUNIT_ASSERT ( *(s1.c_str() + s1.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(s1.wc_str() + s1.getLength()) == L'\0' );
  CPPUNIT_ASSERT ( s1 + finalcut::FString("def") == L"abcdef" );
  CPPUNIT_ASSERT ( s1 + std::wstring(L"def") == L"abcdef" );
#if __cplusplus >= 201703L
  CPPUNIT_ASSERT ( s1 + std::wstring_view(L"def") == L"abcdef" );
#endif
  CPPUNIT_ASSERT ( s1 + const_cast<wchar_t*>(L"def") == L"abcdef" );
  CPPUNIT_ASSERT ( s1 + std::string("def") == L"abcdef" );
#if __cplusplus >= 201703L
  CPPUNIT_ASSERT ( s1 + std::string_view("def") == L"abcdef" );
#endif
  CPPUNIT_ASSERT ( s1 + const_cast<char*>("def") == L"abcdef" );
  CPPUNIT_ASSERT ( s1 + wchar_t(L'd') == L"abcd" );
  CPPUNIT_ASSERT ( s1 + char('d') == L"abcd" );

  // finalcut::FString + ...
  finalcut::FString s2("abc");
  CPPUNIT_ASSERT ( s2.getLength() == 3 );
  CPPUNIT_ASSERT ( *(s2.c_str() + s2.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(s2.wc_str() + s2.getLength()) == L'\0' );
  CPPUNIT_ASSERT ( s2 + finalcut::FString("def") == L"abcdef" );
  CPPUNIT_ASSERT ( s2 + std::wstring(L"def") == L"abcdef" );
#if __cplusplus >= 201703L
  CPPUNIT_ASSERT ( s2 + std::wstring_view(L"def") == L"abcdef" );
#endif
  CPPUNIT_ASSERT ( s2 + const_cast<wchar_t*>(L"def") == L"abcdef" );
  CPPUNIT_ASSERT ( s2 + std::string("def") == L"abcdef" );
#if __cplusplus >= 201703L
  CPPUNIT_ASSERT ( s2 + std::string_view("def") == L"abcdef" );
#endif
  CPPUNIT_ASSERT ( s2 + const_cast<char*>("def") == L"abcdef" );
  CPPUNIT_ASSERT ( s2 + wchar_t(L'd') == L"abcd" );
  CPPUNIT_ASSERT ( s2 + char('d') == L"abcd" );

  // Empty const finalcut::FString + ...
  const finalcut::FString s3;
  CPPUNIT_ASSERT ( s3.getLength() == 0 );
  CPPUNIT_ASSERT ( s3.c_str()[0] == '\0' );
  CPPUNIT_ASSERT ( s3.wc_str()[0] == L'\0' );
  CPPUNIT_ASSERT ( s3 + finalcut::FString("def") == L"def" );
  CPPUNIT_ASSERT ( s3 + std::wstring(L"def") == L"def" );
#if __cplusplus >= 201703L
  CPPUNIT_ASSERT ( s3 + std::wstring_view(L"def") == L"def" );
#endif
  CPPUNIT_ASSERT ( s3 + const_cast<wchar_t*>(L"def") == L"def" );
  CPPUNIT_ASSERT ( s3 + std::string("def") == L"def" );
#if __cplusplus >= 201703L
  CPPUNIT_ASSERT ( s3 + std::string_view("def") == L"def" );
#endif
  CPPUNIT_ASSERT ( s3 + const_cast<char*>("def") == L"def" );
  CPPUNIT_ASSERT ( s3 + wchar_t(L'd') == L"d" );
  CPPUNIT_ASSERT ( s3 + char('d') == L"d" );

  // Empty finalcut::FString + ...
  finalcut::FString s4;
  CPPUNIT_ASSERT ( s4.getLength() == 0 );
  CPPUNIT_ASSERT ( s4.c_str()[0] == '\0' );
  CPPUNIT_ASSERT ( s4.wc_str()[0] == L'\0' );
  CPPUNIT_ASSERT ( s4 + finalcut::FString("def") == L"def" );
  CPPUNIT_ASSERT ( s4 + std::wstring(L"def") == L"def" );
#if __cplusplus >= 201703L
  CPPUNIT_ASSERT ( s4 + std::wstring_view(L"def") == L"def" );
#endif
  CPPUNIT_ASSERT ( s4 + const_cast<wchar_t*>(L"def") == L"def" );
  CPPUNIT_ASSERT ( s4 + std::string("def") == L"def" );
#if __cplusplus >= 201703L
  CPPUNIT_ASSERT ( s4 + std::string_view("def") == L"def" );
#endif
  CPPUNIT_ASSERT ( s4 + const_cast<char*>("def") == L"def" );
  CPPUNIT_ASSERT ( s4 + wchar_t(L'd') == L"d" );
  CPPUNIT_ASSERT ( s4 + char('d') == L"d" );

  // Other string types + finalcut::FString
  const std::wstring& s5 = L"abc";
  CPPUNIT_ASSERT ( s5 + finalcut::FString("def") == L"abcdef" );

  constexpr wchar_t s6[] = L"abc";
  CPPUNIT_ASSERT ( s6 + finalcut::FString("def") == L"abcdef" );

  const std::string& s7 = "abc";
  CPPUNIT_ASSERT ( s7 + finalcut::FString("def") == L"abcdef" );

#if __cplusplus >= 201703L
  const std::string_view& s8 = "abc";
  const std::wstring_view& s9 = L"abc";
  CPPUNIT_ASSERT ( s8 + finalcut::FString("def") == L"abcdef" );
  CPPUNIT_ASSERT ( s9 + finalcut::FString("def") == L"abcdef" );
#endif

  constexpr char s10[] = "abc";
  CPPUNIT_ASSERT ( s10 + finalcut::FString("def") == L"abcdef" );

  constexpr wchar_t c1 = L'a';
  CPPUNIT_ASSERT ( c1 + s5 == L"aabc" );
  CPPUNIT_ASSERT ( c1 + finalcut::FString("def") == L"adef" );

  constexpr char c2 = 'a';
  CPPUNIT_ASSERT ( c2 + s7 == "aabc" );
  CPPUNIT_ASSERT ( c2 + finalcut::FString("def") == L"adef" );

  // Other string types + empty const finalcut::FString
  CPPUNIT_ASSERT ( s5 + s3 == L"abc" );
  CPPUNIT_ASSERT ( s6 + s3 == L"abc" );
  CPPUNIT_ASSERT ( s7 + s3 == L"abc" );
#if __cplusplus >= 201703L
  CPPUNIT_ASSERT ( s8 + s3 == L"abc" );
  CPPUNIT_ASSERT ( s9 + s3 == L"abc" );
#endif
  CPPUNIT_ASSERT ( s10 + s3 == L"abc" );
  CPPUNIT_ASSERT ( c1 + s3 == L"a" );
  CPPUNIT_ASSERT ( c1 + s3 == L"a" );
  CPPUNIT_ASSERT ( c2 + s3 == "a" );

  // Other string types + empty finalcut::FString
  CPPUNIT_ASSERT ( s5 + s4 == L"abc" );
  CPPUNIT_ASSERT ( s6 + s4 == L"abc" );
  CPPUNIT_ASSERT ( s7 + s4 == L"abc" );
#if __cplusplus >= 201703L
  CPPUNIT_ASSERT ( s8 + s4 == L"abc" );
  CPPUNIT_ASSERT ( s9 + s4 == L"abc" );
#endif
  CPPUNIT_ASSERT ( s10 + s4 == L"abc" );
  CPPUNIT_ASSERT ( c1 + s4 == L"a" );
  CPPUNIT_ASSERT ( c1 + s4 == L"a" );
  CPPUNIT_ASSERT ( c2 + s4 == "a" );
}

//----------------------------------------------------------------------
void FStringTest::caseTest()
{
  const finalcut::FString str1("abc");
  CPPUNIT_ASSERT ( str1.toUpper() == "ABC" );

  const finalcut::FString str2("XYZ");
  CPPUNIT_ASSERT ( str2.toLower() == "xyz" );
}

//----------------------------------------------------------------------
void FStringTest::equalTest()
{
  // std::string -> finalcut::FString -> std::string
  const std::string s1 = "string";
  finalcut::FString fs = s1;
  const std::string s2 = fs.toString();
#if __cplusplus >= 201703L
  const std::string_view s3 = fs.toString();
  const std::string_view s4 = fs.toString();
#endif
  CPPUNIT_ASSERT ( s1 == s2 );
  CPPUNIT_ASSERT_CSTRING ( s1.c_str(), s2.c_str() );
  CPPUNIT_ASSERT ( s1.size() == 6 );
  CPPUNIT_ASSERT ( *(s1.c_str() + s1.size()) == '\0' );
#if __cplusplus >= 201703L
  CPPUNIT_ASSERT ( s1 == s3 );
  CPPUNIT_ASSERT ( s1 == s4 );
#endif
  CPPUNIT_ASSERT_CSTRING ( s1.c_str(), s2.data() );

  // finalcut::FString == std::string, char[], char* wchar_t[], wchar_t*
  CPPUNIT_ASSERT ( fs == s1 );
  CPPUNIT_ASSERT ( fs == "string" );
  CPPUNIT_ASSERT ( fs == L"string" );
#if __cplusplus >= 201703L
  CPPUNIT_ASSERT ( fs == s3 );
  CPPUNIT_ASSERT ( fs == s4 );
#endif
  const char* cstring = "string";
  const wchar_t* wcstring = L"string";
  CPPUNIT_ASSERT ( fs == cstring );
  CPPUNIT_ASSERT ( fs == wcstring );
  const finalcut::FString _null_str1{};
  cstring = nullptr;
  wcstring = nullptr;
  CPPUNIT_ASSERT ( _null_str1 == cstring );
  CPPUNIT_ASSERT ( _null_str1 == wcstring );

  // std::wstring -> finalcut::FString -> std::wstring
  const std::wstring ws1 = L"wide string";
  fs = ws1;
  std::wstring ws2 = fs.wc_str();
  CPPUNIT_ASSERT ( ws1 == ws2 );
  CPPUNIT_ASSERT_WCSTRING ( ws1.c_str(), ws2.c_str() );
  CPPUNIT_ASSERT ( ws1.size() == 11 );
  CPPUNIT_ASSERT ( *(ws1.c_str() + ws1.size()) == L'\0' );

  const finalcut::FString one_char('a');
  constexpr char ch = 'a';
  CPPUNIT_ASSERT ( one_char == ch );
  CPPUNIT_ASSERT ( ch == one_char.c_str()[0] );
  CPPUNIT_ASSERT ( one_char.getLength() == 1 );
  CPPUNIT_ASSERT ( one_char.capacity() >= 1 );

  constexpr wchar_t wch = L'a';
  CPPUNIT_ASSERT ( one_char == wch );
  CPPUNIT_ASSERT ( wch == one_char.wc_str()[0] );
  CPPUNIT_ASSERT ( L'\0' == one_char.wc_str()[1] );  // pos == size

  const finalcut::FString str(L"abc");
  const finalcut::FString str2(L"abc");
  CPPUNIT_ASSERT ( str == str2 );

  constexpr char cstr[] = "abc";
  CPPUNIT_ASSERT ( str == cstr );
  CPPUNIT_ASSERT_CSTRING ( str.c_str(), cstr );
  CPPUNIT_ASSERT ( str.getLength() == 3 );
  CPPUNIT_ASSERT ( str.capacity() >= 3 );
  CPPUNIT_ASSERT ( strncmp(cstr, str.c_str(), 3) == 0 );

  constexpr wchar_t wcstr[] = L"abc";
  CPPUNIT_ASSERT ( str == wcstr );
  CPPUNIT_ASSERT_WCSTRING ( str.wc_str(), wcstr );
  CPPUNIT_ASSERT ( wcsncmp(wcstr, str.wc_str(), 3) == 0 );

  const std::string st = "abc";
  CPPUNIT_ASSERT ( str == st );

#if __cplusplus >= 201703L
  const std::string_view stv = "abc";
  CPPUNIT_ASSERT ( str == stv );
#endif

  const std::wstring wst = L"abc";
  CPPUNIT_ASSERT ( str == wst );
  CPPUNIT_ASSERT_WCSTRING ( str.wc_str(), wst.c_str() );

#if __cplusplus >= 201703L
  const std::wstring_view wstv = L"abc";
  CPPUNIT_ASSERT ( str == wstv );
#endif

  const finalcut::FString null_str1{};
  const finalcut::FString null_str2{};
  CPPUNIT_ASSERT ( ! (str == null_str2) );
  CPPUNIT_ASSERT ( ! (null_str1 == str) );
  CPPUNIT_ASSERT ( null_str1 == null_str2 );
  cstring = nullptr;
  wcstring = nullptr;
  CPPUNIT_ASSERT ( _null_str1 == cstring );
  CPPUNIT_ASSERT ( _null_str1 == wcstring );
  CPPUNIT_ASSERT_CSTRING ( null_str1.c_str(), null_str2.c_str() );
  CPPUNIT_ASSERT_WCSTRING ( null_str1.wc_str(), null_str2.wc_str() );

  CPPUNIT_ASSERT ( s->c_str()[0] == 'c');
  CPPUNIT_ASSERT ( s->getLength() == 1 );
  CPPUNIT_ASSERT ( s->capacity() >= 1 );
}

//----------------------------------------------------------------------
void FStringTest::notEqualTest()
{
  const finalcut::FString one_char(finalcut::UniChar::Bullet);
  constexpr char ch = '!';
  CPPUNIT_ASSERT ( one_char != ch );
  CPPUNIT_ASSERT ( ch != one_char.c_str()[0] );
  CPPUNIT_ASSERT ( one_char.getLength() == 1 );
  CPPUNIT_ASSERT ( one_char.capacity() >= 1 );

  // finalcut::FString != std::string, char[], char* wchar_t[], wchar_t*
  CPPUNIT_ASSERT ( one_char != std::string("!") );
  CPPUNIT_ASSERT ( one_char != "!" );
  CPPUNIT_ASSERT ( one_char != L"!" );
#if __cplusplus >= 201703L
  CPPUNIT_ASSERT ( one_char != std::string_view("!") );
  CPPUNIT_ASSERT ( one_char != std::wstring_view(L"!") );
#endif
  const char* cstring = "!";
  const wchar_t* wcstring = L"!";
  CPPUNIT_ASSERT ( one_char != cstring );
  CPPUNIT_ASSERT ( one_char != wcstring );
  cstring = nullptr;
  wcstring = nullptr;
  CPPUNIT_ASSERT ( one_char != cstring );
  CPPUNIT_ASSERT ( one_char != wcstring );

  constexpr wchar_t wch = L'_';
  CPPUNIT_ASSERT ( one_char != wch );
  CPPUNIT_ASSERT ( wch != one_char.wc_str()[0] );

  const finalcut::FString s1 = L"ABC";  // latin letter
  const finalcut::FString s2 = L"АВС";  // cyrillic letters
  CPPUNIT_ASSERT ( s1 != s2 );

  constexpr char cstr[] = "abc";
  CPPUNIT_ASSERT ( s1 != cstr );
  CPPUNIT_ASSERT ( s1.getLength() == 3 );
  CPPUNIT_ASSERT ( strlen(s1.c_str()) == 3 );
  CPPUNIT_ASSERT ( s2.getLength() == 3 );
  CPPUNIT_ASSERT ( strlen(s2.c_str()) == 6 );
  CPPUNIT_ASSERT ( s1.capacity() >= 3 );
  CPPUNIT_ASSERT ( s2.capacity() >= 3 );
  CPPUNIT_ASSERT ( strncmp(cstr, s1.c_str(), 3) != 0 );

  constexpr wchar_t wcstr[] = L"abc";
  CPPUNIT_ASSERT ( s1 != wcstr );
  CPPUNIT_ASSERT ( wcsncmp(wcstr, s1.wc_str(), 3) != 0 );

#if __cplusplus >= 201703L
  const std::string_view stv = "abc";
  const std::wstring_view wstv = L"abc";
  CPPUNIT_ASSERT ( s1 != stv );
  CPPUNIT_ASSERT ( s1 != wstv );
#endif

  const std::string st = "abc";
  CPPUNIT_ASSERT ( s1 != st );

  const std::wstring wst = L"abc";
  CPPUNIT_ASSERT ( s1 != wst );

  const finalcut::FString null_str1{};
  const finalcut::FString null_str2{};
  CPPUNIT_ASSERT ( s1 != null_str2 );
  CPPUNIT_ASSERT ( null_str1 != s1 );
  CPPUNIT_ASSERT ( ! (null_str1 != null_str2) );

  CPPUNIT_ASSERT ( s->c_str()[0] != 's');
}

//----------------------------------------------------------------------
void FStringTest::lessEqualTest()
{
  const finalcut::FString one_char('x');
  constexpr char ch = 'z';
  CPPUNIT_ASSERT ( one_char <= ch );

  constexpr wchar_t wch = L'z';
  CPPUNIT_ASSERT ( one_char <= wch );
  // finalcut::FString <= std::string, char[], char* wchar_t[], wchar_t*
  CPPUNIT_ASSERT ( one_char <= std::string("x") );
  CPPUNIT_ASSERT ( one_char <= std::string("y") );
#if __cplusplus >= 201703L
  CPPUNIT_ASSERT ( one_char <= std::wstring_view(L"x") );
  CPPUNIT_ASSERT ( one_char <= std::wstring_view(L"y") );
  CPPUNIT_ASSERT ( one_char <= std::string_view("x") );
  CPPUNIT_ASSERT ( one_char <= std::string_view("y") );
#endif
  CPPUNIT_ASSERT ( one_char <= "x" );
  CPPUNIT_ASSERT ( one_char <= "y" );
  CPPUNIT_ASSERT ( one_char <= L"x" );
  CPPUNIT_ASSERT ( one_char <= L"y" );
  const char* cstring = "x";
  const wchar_t* wcstring = L"x";
  CPPUNIT_ASSERT ( one_char <= cstring );
  CPPUNIT_ASSERT ( one_char <= wcstring );
  cstring = "y";
  wcstring = L"y";
  CPPUNIT_ASSERT ( one_char <= cstring );
  CPPUNIT_ASSERT ( one_char <= wcstring );
  cstring = nullptr;
  wcstring = nullptr;
  CPPUNIT_ASSERT ( ! (one_char <= cstring) );
  CPPUNIT_ASSERT ( ! (one_char <= wcstring) );

  const finalcut::FString s1 = L"xyz";
  const finalcut::FString s2 = L"xyz";
  const finalcut::FString s3 = L"xzz";
  CPPUNIT_ASSERT ( s1 <= s2 && s1 == s2 );
  CPPUNIT_ASSERT ( s1 <= s3 && s1 != s3 );

  constexpr char cstr1[] = "xyz";
  constexpr char cstr2[] = "xzz";
  CPPUNIT_ASSERT ( s1 <= cstr1 && s1 == cstr1 );
  CPPUNIT_ASSERT ( s1 <= cstr2 && s1 != cstr2 );

  constexpr wchar_t wcstr1[] = L"xyz";
  constexpr wchar_t wcstr2[] = L"xzz";
  CPPUNIT_ASSERT ( s1 <= wcstr1 && s1 == wcstr1 );
  CPPUNIT_ASSERT ( s1 <= wcstr2 && s1 != wcstr2 );

  const std::string st1 = "xyz";
  const std::string st2 = "xzz";
  CPPUNIT_ASSERT ( s1 <= st1 && s1 == st1 );
  CPPUNIT_ASSERT ( s1 <= st2 && s1 != st2 );

#if __cplusplus >= 201703L
  const std::string_view stv1 = "xyz";
  const std::string_view stv2 = "xzz";
  const std::wstring_view wstv1 = L"xyz";
  const std::wstring_view wstv2 = L"xzz";
  CPPUNIT_ASSERT ( s1 <= stv1 && s1 == stv1 );
  CPPUNIT_ASSERT ( s1 <= stv2 && s1 != stv2 );
  CPPUNIT_ASSERT ( s1 <= wstv1 && s1 == wstv1 );
  CPPUNIT_ASSERT ( s1 <= wstv2 && s1 != wstv2 );
#endif

  const std::wstring wst1 = L"xyz";
  const std::wstring wst2 = L"xzz";
  CPPUNIT_ASSERT ( s1 <= wst1 && s1 == wst1 );
  CPPUNIT_ASSERT ( s1 <= wst2 && s1 != wst2 );

  const finalcut::FString null_str1{};
  const finalcut::FString null_str2{};
  const finalcut::FString empty("");
  CPPUNIT_ASSERT ( ! (s1 <= null_str2) );
  CPPUNIT_ASSERT ( null_str1 <= s2 );
  CPPUNIT_ASSERT ( null_str1 <= null_str2 );
  CPPUNIT_ASSERT ( ! (s1 <= empty) );
}

//----------------------------------------------------------------------
void FStringTest::lessTest()
{
  const finalcut::FString one_char('x');
  constexpr char ch = 'z';
  CPPUNIT_ASSERT ( one_char < ch );

  constexpr wchar_t wch = L'z';
  CPPUNIT_ASSERT ( one_char < wch );
  // finalcut::FString < std::string, char[], char* wchar_t[], wchar_t*
  CPPUNIT_ASSERT ( one_char < std::string("y") );
#if __cplusplus >= 201703L
  CPPUNIT_ASSERT ( one_char < std::wstring_view(L"y") );
  CPPUNIT_ASSERT ( one_char < std::string_view("y") );
#endif
  CPPUNIT_ASSERT ( one_char < "y" );
  CPPUNIT_ASSERT ( one_char < L"y" );
  const char* cstring = "y";
  const wchar_t* wcstring = L"y";
  CPPUNIT_ASSERT ( one_char < cstring );
  CPPUNIT_ASSERT ( one_char < wcstring );
  cstring = nullptr;
  wcstring = nullptr;
  CPPUNIT_ASSERT ( ! (one_char < cstring) );
  CPPUNIT_ASSERT ( ! (one_char < wcstring) );

  const finalcut::FString s1 = L"xyz";
  const finalcut::FString s2 = L"xzz";
  CPPUNIT_ASSERT ( s1 < s2 );

  constexpr char cstr[] = "xzz";
  CPPUNIT_ASSERT ( s1 < cstr );

  constexpr wchar_t wcstr[] = L"xzz";
  CPPUNIT_ASSERT ( s1 < wcstr );

#if __cplusplus >= 201703L
  const std::string_view stv = "xzz";
  const std::wstring_view wstv = L"xzz";
  CPPUNIT_ASSERT ( s1 < stv  );
  CPPUNIT_ASSERT ( s1 < wstv  );
#endif

  const std::string st = "xzz";
  CPPUNIT_ASSERT ( s1 < st  );

  const std::wstring wst = L"xzz";
  CPPUNIT_ASSERT ( s1 < wst );

  const finalcut::FString null_str1{};
  const finalcut::FString null_str2{};
  CPPUNIT_ASSERT ( ! (s1 < null_str2) );
  CPPUNIT_ASSERT ( null_str1 < s2 );
  CPPUNIT_ASSERT ( ! (null_str1 < null_str2) );
}

//----------------------------------------------------------------------
void FStringTest::greaterEqualTest()
{
  const finalcut::FString one_char('x');
  constexpr char ch = 'x';
  CPPUNIT_ASSERT ( one_char >= ch );

  constexpr wchar_t wch = L'x';
  CPPUNIT_ASSERT ( one_char >= wch );
  // finalcut::FString >= std::string, char[], char* wchar_t[], wchar_t*
  CPPUNIT_ASSERT ( one_char >= std::string("w") );
  CPPUNIT_ASSERT ( one_char >= std::string("x") );
#if __cplusplus >= 201703L
  CPPUNIT_ASSERT ( one_char >= std::wstring_view(L"w") );
  CPPUNIT_ASSERT ( one_char >= std::wstring_view(L"x") );
  CPPUNIT_ASSERT ( one_char >= std::string_view("w") );
  CPPUNIT_ASSERT ( one_char >= std::string_view("x") );
#endif
  CPPUNIT_ASSERT ( one_char >= "w" );
  CPPUNIT_ASSERT ( one_char >= "x" );
  CPPUNIT_ASSERT ( one_char >= L"w" );
  CPPUNIT_ASSERT ( one_char >= L"x" );
  const char* cstring = "w";
  const wchar_t* wcstring = L"w";
  CPPUNIT_ASSERT ( one_char >= cstring );
  CPPUNIT_ASSERT ( one_char >= wcstring );
  cstring = "x";
  wcstring = L"x";
  CPPUNIT_ASSERT ( one_char >= cstring );
  CPPUNIT_ASSERT ( one_char >= wcstring );
  cstring = nullptr;
  wcstring = nullptr;
  CPPUNIT_ASSERT ( one_char >= cstring );
  CPPUNIT_ASSERT ( one_char >= wcstring );

  const finalcut::FString s1 = L"xyz";
  const finalcut::FString s2 = L"xyz";
  const finalcut::FString s3 = L"xxz";
  CPPUNIT_ASSERT ( s1 >= s2 && s1 == s2 );
  CPPUNIT_ASSERT ( s1 >= s3 && s1 != s3 );

  constexpr char cstr1[] = "xyz";
  constexpr char cstr2[] = "xxz";
  CPPUNIT_ASSERT ( s1 >= cstr1 && s1 == cstr1 );
  CPPUNIT_ASSERT ( s1 >= cstr2 && s1 != cstr2 );

  constexpr wchar_t wcstr1[] = L"xyz";
  constexpr wchar_t wcstr2[] = L"xxz";
  CPPUNIT_ASSERT ( s1 >= wcstr1 && s1 == wcstr1 );
  CPPUNIT_ASSERT ( s1 >= wcstr2 && s1 != wcstr2 );

#if __cplusplus >= 201703L
  const std::string_view stv1 = "xyz";
  const std::string_view stv2 = "xxz";
  const std::wstring_view wstv1 = L"xyz";
  const std::wstring_view wstv2 = L"xxz";
  CPPUNIT_ASSERT ( s1 >= stv1 && s1 == stv1 );
  CPPUNIT_ASSERT ( s1 >= stv2 && s1 != stv2 );
  CPPUNIT_ASSERT ( s1 >= wstv1 && s1 == wstv1 );
  CPPUNIT_ASSERT ( s1 >= wstv2 && s1 != wstv2 );
#endif

  const std::string st1 = "xyz";
  const std::string st2 = "xxz";
  CPPUNIT_ASSERT ( s1 >= st1 && s1 == st1 );
  CPPUNIT_ASSERT ( s1 >= st2 && s1 != st2 );

  const std::wstring wst1 = L"xyz";
  const std::wstring wst2 = L"xxz";
  CPPUNIT_ASSERT ( s1 >= wst1 && s1 == wst1 );
  CPPUNIT_ASSERT ( s1 >= wst2 && s1 != wst2 );

  const finalcut::FString null_str1{};
  const finalcut::FString null_str2{};
  const finalcut::FString empty("");
  CPPUNIT_ASSERT ( s1 >= null_str2 );
  CPPUNIT_ASSERT ( ! (null_str1 >= s2) );
  CPPUNIT_ASSERT ( null_str1 >= null_str2 );
  CPPUNIT_ASSERT ( ! (s1 <= empty) );
}

//----------------------------------------------------------------------
void FStringTest::greaterTest()
{
  const finalcut::FString one_char('x');
  constexpr char ch = 'w';
  CPPUNIT_ASSERT ( one_char > ch );

  constexpr wchar_t wch = L'w';
  CPPUNIT_ASSERT ( one_char > wch );
  // finalcut::FString > std::string, char[], char* wchar_t[], wchar_t*
  CPPUNIT_ASSERT ( one_char > std::string("w") );
#if __cplusplus >= 201703L
  CPPUNIT_ASSERT ( one_char > std::wstring_view(L"w") );
  CPPUNIT_ASSERT ( one_char > std::string_view("w") );
#endif
  CPPUNIT_ASSERT ( one_char > "w" );
  CPPUNIT_ASSERT ( one_char > L"w" );
  const char* cstring = "w";
  const wchar_t* wcstring = L"w";
  CPPUNIT_ASSERT ( one_char > cstring );
  CPPUNIT_ASSERT ( one_char > wcstring );
  cstring = nullptr;
  wcstring = nullptr;
  CPPUNIT_ASSERT ( one_char > cstring );
  CPPUNIT_ASSERT ( one_char > wcstring );

  const finalcut::FString s1 = L"xyz";
  const finalcut::FString s2 = L"xww";
  CPPUNIT_ASSERT ( s1 > s2 );

  constexpr char cstr[] = "xww";
  CPPUNIT_ASSERT ( s1 > cstr );

  constexpr wchar_t wcstr[] = L"xww";
  CPPUNIT_ASSERT ( s1 > wcstr );

#if __cplusplus >= 201703L
  const std::string_view stv = "xww";
  const std::wstring_view wstv = L"xww";
  CPPUNIT_ASSERT ( s1 > stv );
  CPPUNIT_ASSERT ( s1 > wstv );
#endif

  const std::string st = "xww";
  CPPUNIT_ASSERT ( s1 > st  );

  const std::wstring wst = L"xww";
  CPPUNIT_ASSERT ( s1 > wst );

  const finalcut::FString null_str1{};
  const finalcut::FString null_str2{};
  CPPUNIT_ASSERT ( s1 > null_str2 );
  CPPUNIT_ASSERT ( ! (null_str1 > s2) );
  CPPUNIT_ASSERT ( ! (null_str1 > null_str2) );
}

//----------------------------------------------------------------------
void FStringTest::streamInsertionTest()
{
  finalcut::FString out;
  out << finalcut::FString("ABC");
  CPPUNIT_ASSERT ( out == L"ABC" );
  CPPUNIT_ASSERT ( out.getLength() == 3 );
  CPPUNIT_ASSERT ( *(out.c_str() + out.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(out.wc_str() + out.getLength()) == L'\0' );

  out.clear();
  out << std::string("ABC");
  CPPUNIT_ASSERT ( out == L"ABC" );

#if __cplusplus >= 201703L
  out.clear();
  out << std::string_view("ABC");
  CPPUNIT_ASSERT ( out == L"ABC" );
#endif

  out.clear();
  out << std::wstring(L"ABC");
  CPPUNIT_ASSERT ( out == L"ABC" );

#if __cplusplus >= 201703L
  out.clear();
  out << std::wstring_view(L"ABC");
  CPPUNIT_ASSERT ( out == L"ABC" );
#endif

  out.clear();
  out << const_cast<wchar_t*>(L"ABC");
  CPPUNIT_ASSERT ( out == L"ABC" );

  out.clear();
  out << const_cast<char*>("ABC");
  CPPUNIT_ASSERT ( out == L"ABC" );

  out.clear();
  out << finalcut::UniChar::Euro;
  CPPUNIT_ASSERT ( out == L"€" );

  out.clear();
  out << wchar_t(L'A');
  CPPUNIT_ASSERT ( out == L"A" );

  out.clear();
  out << char('A');
  CPPUNIT_ASSERT ( out == L"A" );

  out.clear();
  out << sInt8(INT_LEAST8_MAX);
  CPPUNIT_ASSERT ( out == L"127" );

  out.clear();
  out << sInt8(INT_LEAST8_MIN);
  CPPUNIT_ASSERT ( out == L"-128" );

  out.clear();
  out << uInt8(UINT_LEAST8_MAX);
  CPPUNIT_ASSERT ( out == L"255" );

  out.clear();
  out << sInt16(INT_LEAST16_MAX);
  CPPUNIT_ASSERT ( out == L"32767" );

  out.clear();
  out << sInt16(INT_LEAST16_MIN);
  CPPUNIT_ASSERT ( out == L"-32768" );

  out.clear();
  out << uInt16(UINT_LEAST16_MAX);
  CPPUNIT_ASSERT ( out == L"65535" );

  out.clear();
  out << sInt32(INT_LEAST32_MAX);
  CPPUNIT_ASSERT ( out == L"2147483647" );

  out.clear();
  out << sInt32(INT_LEAST32_MIN);
  CPPUNIT_ASSERT ( out == L"-2147483648" );

  out.clear();
  out << uInt32(UINT_LEAST32_MAX);
  CPPUNIT_ASSERT ( out == L"4294967295" );

  out.clear();
  out << sInt64(INT_LEAST64_MAX);
  CPPUNIT_ASSERT ( out == L"9223372036854775807" );

  out.clear();
  out << sInt64(INT_LEAST64_MIN);
  CPPUNIT_ASSERT ( out == L"-9223372036854775808" );

  out.clear();
  out << uInt64(UINT_LEAST64_MAX);
  CPPUNIT_ASSERT ( out == L"18446744073709551615" );

  out.clear();
  out << std::wint_t(WINT_MAX);
  CPPUNIT_ASSERT ( out == L"4294967295" );

  out.clear();
  out << std::size_t(999999999);
  CPPUNIT_ASSERT ( out == L"999999999" );

  out.clear();
  out << int(1234567);
  CPPUNIT_ASSERT ( out == L"1234567" );

  out.clear();
  out << int(-1234567);
  CPPUNIT_ASSERT ( out == L"-1234567" );

  out.clear();
  out << uInt(12345678u);
  CPPUNIT_ASSERT ( out == L"12345678" );

  out.clear();
  out << long(-34721053343141);
  CPPUNIT_ASSERT ( out == L"-34721053343141" );

  out.clear();
  out << uLong(4670148723459u);
  CPPUNIT_ASSERT ( out == L"4670148723459" );

  out.clear();
  out << float(3.14159);
  CPPUNIT_ASSERT ( out == L"3.14159" );

  out.clear();
  out << double(3.1415926535);
  CPPUNIT_ASSERT ( out == L"3.1415926535" );

  out.clear();
  out << lDouble(3.141592653589793238L);
  CPPUNIT_ASSERT ( out == L"3.14159265358979324" );

  out.clear();
  std::ostringstream ostream;
  ostream << out;
  CPPUNIT_ASSERT ( ostream.str() == "" );
  ostream << std::setfill('*') << std::setw(5) << out;
  CPPUNIT_ASSERT ( ostream.str() == "*****" );
  out = "abc";
  ostream.str("");
  ostream << out;
  CPPUNIT_ASSERT ( ostream.str() == "abc" );

  out.clear();
  std::wostringstream wostream;
  wostream << out;
  CPPUNIT_ASSERT ( wostream.str() == L"" );
  wostream << std::setfill(L'+') << std::setw(7) << out;
  CPPUNIT_ASSERT ( wostream.str() == L"+++++++" );
  out = L"def";
  wostream.str(L"");
  wostream << out;
  CPPUNIT_ASSERT ( wostream.str() == L"def" );
}

//----------------------------------------------------------------------
void FStringTest::streamExtractionTest()
{
  finalcut::FString in_1;
  finalcut::FString("ABC") >> in_1;
  CPPUNIT_ASSERT ( in_1 == "ABC" );
  CPPUNIT_ASSERT ( in_1.getLength() == 3 );
  CPPUNIT_ASSERT ( *(in_1.c_str() + in_1.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(in_1.wc_str() + in_1.getLength()) == L'\0' );

  std::wstring in_2;
  finalcut::FString("ABC") >> in_2;
  CPPUNIT_ASSERT ( in_2 == L"ABC" );

  std::string in_3;
  finalcut::FString("ABC") >> in_3;
  CPPUNIT_ASSERT ( in_3 == "ABC" );

  wchar_t in_4;
  finalcut::FString("A") >> in_4;
  CPPUNIT_ASSERT ( in_4 == L'A' );

  char in_5;
  finalcut::FString("A") >> in_5;
  CPPUNIT_ASSERT ( in_5 == L'A' );

  sInt16 in_6;
  finalcut::FString("-12345") >> in_6;
  CPPUNIT_ASSERT ( in_6 == -12345 );

  uInt16 in_7;
  finalcut::FString("33333") >> in_7;
  CPPUNIT_ASSERT ( in_7 == 33333 );

  int in_8;
  finalcut::FString("-12345678") >> in_8;
  CPPUNIT_ASSERT ( in_8 == -12345678 );

  uInt in_9;
  finalcut::FString("99999999") >> in_9;
  CPPUNIT_ASSERT ( in_9 == 99999999 );

  long in_10;
  finalcut::FString("-1234567890") >> in_10;
  CPPUNIT_ASSERT ( in_10 == -1234567890 );

  uLong in_11;
  finalcut::FString("9999999999999") >> in_11;
  CPPUNIT_ASSERT ( in_11 == 9999999999999 );

  float in_12;
  finalcut::FString("2.71828") >> in_12;
  CPPUNIT_ASSERT ( std::fabs(in_12 - 2.71828f) <= FLT_EPSILON );

  double in_13;
  finalcut::FString("2.7182818284590452353") >> in_13;
  CPPUNIT_ASSERT ( std::fabs(in_13 - 2.7182818284590452353) <= DBL_EPSILON  );

  finalcut::FString in;
  std::istringstream istream("abc");
  istream >> in;
  CPPUNIT_ASSERT ( in == "abc" );

  std::wistringstream wistream(L"abc");
  wistream >> in;
  CPPUNIT_ASSERT ( in == "abc" );
}

//----------------------------------------------------------------------
void FStringTest::subscriptOperatorTest()
{
  finalcut::FString s(3);
  CPPUNIT_ASSERT ( s[0] == L'\0' );
  CPPUNIT_ASSERT ( s[1] == L'\0' );
  CPPUNIT_ASSERT ( s[2] == L'\0' );
  CPPUNIT_ASSERT ( s[3] == L'\0' );  // pos == size
  CPPUNIT_ASSERT ( s.at(0) == L'\0' );
  CPPUNIT_ASSERT ( s.at(1) == L'\0' );
  CPPUNIT_ASSERT ( s.at(2) == L'\0' );

  s[0] = L'A';
  s[1] = L'B';
  s[2] = L'C';
  CPPUNIT_ASSERT ( s[0] == L'A' );
  CPPUNIT_ASSERT ( s[1] == L'B' );
  CPPUNIT_ASSERT ( s[2] == L'C' );
  CPPUNIT_ASSERT ( s[3] == L'\0' );  // pos == size
  CPPUNIT_ASSERT ( s.at(0) == L'A' );
  CPPUNIT_ASSERT ( s.at(1) == L'B' );
  CPPUNIT_ASSERT ( s.at(2) == L'C' );
  CPPUNIT_ASSERT ( s == L"ABC" );
  CPPUNIT_ASSERT ( s.getLength() == 3 );
  CPPUNIT_ASSERT ( *(s.c_str() + s.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(s.wc_str() + s.getLength()) == L'\0' );
}

//----------------------------------------------------------------------
void FStringTest::iteratorTest()
{
  const finalcut::FString& str = "123456789";
  CPPUNIT_ASSERT ( str.front() == L'1' );
  CPPUNIT_ASSERT ( str.back() == L'9' );

  finalcut::FString::const_iterator iter = str.begin();
  CPPUNIT_ASSERT ( (*iter) == L'1' );
  ++iter;
  CPPUNIT_ASSERT ( (*iter) == L'2' );
  ++iter;
  CPPUNIT_ASSERT ( (*iter) == L'3' );
  ++iter;
  CPPUNIT_ASSERT ( (*iter) == L'4' );
  ++iter;
  CPPUNIT_ASSERT ( (*iter) == L'5' );
  ++iter;
  CPPUNIT_ASSERT ( (*iter) == L'6' );
  ++iter;
  CPPUNIT_ASSERT ( (*iter) == L'7' );
  ++iter;
  CPPUNIT_ASSERT ( (*iter) == L'8' );
  ++iter;
  CPPUNIT_ASSERT ( (*iter) == L'9' );
  ++iter;
  CPPUNIT_ASSERT ( iter == str.end() );

  finalcut::FString str2("bcdefg");
  CPPUNIT_ASSERT ( str2.front() == L'b' );
  CPPUNIT_ASSERT ( str2.back() == L'g' );
  finalcut::FString::iterator iter2 = str2.begin();

  while ( iter2 != str2.end() )
  {
    *iter2 -= 1;
    ++iter2;
  }

  CPPUNIT_ASSERT ( str2 == L"abcdef" );

  finalcut::FString str3(L"~~~~~");
  CPPUNIT_ASSERT ( str3.front() == L'~' );
  CPPUNIT_ASSERT ( str3.back() == L'~' );
  CPPUNIT_ASSERT ( str3.front() == str3.back() );
  CPPUNIT_ASSERT ( &str3.front() != &str3.back() );
  auto iter3 = str3.cbegin();

  while ( iter3 != str3.cend() )
    ++iter3;

  CPPUNIT_ASSERT ( iter3 == str3.cend() );
  CPPUNIT_ASSERT ( std::distance(str3.cbegin(), iter3) == 5 );

  finalcut::FString str4("012345");
  CPPUNIT_ASSERT ( str4.front() == L'0' );
  CPPUNIT_ASSERT ( str4.back() == L'5' );
  finalcut::FString::reverse_iterator iter4 = str4.rbegin();

  while ( iter4 != str4.rend() )
  {
    *iter4 += 1;
    ++iter4;
  }

  CPPUNIT_ASSERT ( str4 == L"123456" );
  CPPUNIT_ASSERT ( iter4 == str4.rend() );
  CPPUNIT_ASSERT ( std::distance(str4.rbegin(), iter4) == 6 );

  finalcut::FString str5(L"+-----+");
  CPPUNIT_ASSERT ( str5.front() == L'+' );
  CPPUNIT_ASSERT ( str5.back() == L'+' );
  CPPUNIT_ASSERT ( str5.front() == str5.back() );
  CPPUNIT_ASSERT ( &str5.front() != &str5.back() );
  auto iter5 = str5.crbegin();

  while ( iter5 != str5.crend() )
    ++iter5;

  CPPUNIT_ASSERT ( iter5 == str5.crend() );
  CPPUNIT_ASSERT ( std::distance(str5.crbegin(), iter5) == 7 );
}

//----------------------------------------------------------------------
void FStringTest::functionCallOperatorTest()
{
  finalcut::FString str = L"test";
  CPPUNIT_ASSERT_EQUAL ( str, str() );

  finalcut::FString copy = str();
  copy << L"string";
  CPPUNIT_ASSERT ( str() == "test" );
  CPPUNIT_ASSERT ( copy  == "teststring" );
}

//----------------------------------------------------------------------
void FStringTest::formatTest()
{
  finalcut::FString str1;
  int num = 3;
  char location[] = "zoo";
  str1.sprintf ("There are %d lions in the %s", num, location);
  CPPUNIT_ASSERT ( str1 == "There are 3 lions in the zoo" );

  str1.sprintf (finalcut::FString("%2d times"), 42);
  CPPUNIT_ASSERT ( str1 == "42 times" );

  finalcut::FString str2;
  str2.sprintf (L"It costs only %d cent", 50);
  CPPUNIT_ASSERT ( str2 == "It costs only 50 cent" );

  str2.sprintf ( L"Add a looo" + finalcut::FString(2048, 'o') + "ooong %S"
               , L"string" );
  CPPUNIT_ASSERT ( str2 == "Add a looo" + finalcut::FString(2048, 'o')
                           + "ooong string" );

  const finalcut::FString null_fstring{};
  str2.sprintf (null_fstring, 0);
  CPPUNIT_ASSERT ( str2.isEmpty() );

  constexpr wchar_t* null_wstring = nullptr;
  str2.sprintf (null_wstring, 0);
  CPPUNIT_ASSERT ( str2.isEmpty() );

  constexpr char* null_string = nullptr;
  str2.sprintf (null_string, 0);
  CPPUNIT_ASSERT ( str2.isEmpty() );

  std::setlocale (LC_NUMERIC, "C");
  finalcut::FString fnum1, fnum2;

#if defined(__LP64__) || defined(_LP64)
  // 64-bit architecture
  fnum1.setFormatedNumber(0xffffffffffffffffu, '\'');
  CPPUNIT_ASSERT ( fnum1 == "18'446'744'073'709'551'615" );

  fnum2.setFormatedNumber(-9223372036854775807);
  CPPUNIT_ASSERT ( fnum2 == "-9 223 372 036 854 775 807" );

  fnum2.setFormatedNumber(long(9223372036854775807), '\0');
  CPPUNIT_ASSERT ( fnum2 == "9 223 372 036 854 775 807" );

  fnum2.setFormatedNumber(uLong(9223372036854775807u), '\0');
  CPPUNIT_ASSERT ( fnum2 == "9 223 372 036 854 775 807" );

  fnum2.setFormatedNumber(sInt64(9223372036854775807), '\0');
  CPPUNIT_ASSERT ( fnum2 == "9 223 372 036 854 775 807" );

  fnum2.setFormatedNumber(uInt64(9223372036854775807u), '\0');
  CPPUNIT_ASSERT ( fnum2 == "9 223 372 036 854 775 807" );

  // Non-breaking space
  //   value: 0xa0 = \240
  //   UTF-8: 0xc2 0xa0 = \302 \204
  fnum2.setFormatedNumber(uInt64(9223372036854775807u), L"\240");
  CPPUNIT_ASSERT ( fnum2 == "9\302\240" "223\302\240" "372\302\240"
                            "036\302\240" "854\302\240" "775\302\240"
                            "807" );
#else
  // 32-bit architecture
  fnum1.setFormatedNumber(0xffffffffu, '\'');
  CPPUNIT_ASSERT ( fnum1 == "4'294'967'295" );

  fnum2.setFormatedNumber(-2147483647);
  CPPUNIT_ASSERT ( fnum2 == "-2 147 483 647" );

  fnum2.setFormatedNumber(long(2147483647), '\0');
  CPPUNIT_ASSERT ( fnum2 == "2 147 483 647" );

  fnum2.setFormatedNumber(uLong(2147483647u), '\0');
  CPPUNIT_ASSERT ( fnum2 == "2 147 483 647" );

  fnum2.setFormatedNumber(sInt32(2147483647), '\0');
  CPPUNIT_ASSERT ( fnum2 == "2 147 483 647" );

  fnum2.setFormatedNumber(uInt32(2147483647u), '\0');
  CPPUNIT_ASSERT ( fnum2 == "2 147 483 647" );

  // Non-breaking space
  //   value: 0xa0 = \240
  //   UTF-8: 0xc2 0xa0 = \302 \204
  fnum2.setFormatedNumber(uInt32(2147483647u), L"\240");
  CPPUNIT_ASSERT ( fnum2 == "2\302\240" "147\302\240" "483\302\240"
                            "647" );
#endif

  fnum1.setFormatedNumber(sInt16(-2048), '_');
  CPPUNIT_ASSERT ( fnum1 == "-2_048" );

  fnum2.setFormatedNumber(uInt16(65535u));
  CPPUNIT_ASSERT ( fnum2 == "65 535" );

  fnum1.setFormatedNumber(sInt8(-123), '*');
  CPPUNIT_ASSERT ( fnum1 == "-123" );

  fnum2.setFormatedNumber(uInt8(255u));
  CPPUNIT_ASSERT ( fnum2 == "255" );
}

//----------------------------------------------------------------------
void FStringTest::convertToNumberTest()
{
  finalcut::FString str = "-127";
  CPPUNIT_ASSERT ( str.toShort() == -127 );

  str = "255";
  CPPUNIT_ASSERT ( str.toUShort() == 255U );

  str = "0";
  CPPUNIT_ASSERT ( str.toInt() == 0 );

  str = "-32768";
  CPPUNIT_ASSERT ( str.toInt() == -32768 );

  str = "65535";
  CPPUNIT_ASSERT ( str.toUInt() == 65535U );

  str = "-2147483647";
  CPPUNIT_ASSERT ( str.toLong() == -2147483647 );

  str = "+987654321";
  CPPUNIT_ASSERT ( str.toLong() == 987654321 );

  str = "4294967295";
  CPPUNIT_ASSERT ( str.toULong() == 4294967295U );

  str = "+1234567890";
  CPPUNIT_ASSERT ( str.toULong() == 1234567890U );

  str = "3.14159";
  CPPUNIT_ASSERT ( str.toFloat() == 3.14159F );

  str = "-3.14159";
  CPPUNIT_ASSERT ( str.toFloat() == -3.14159F );

  str = "3.141592653589793238";
  CPPUNIT_ASSERT ( str.toDouble() == 3.141592653589793238 );

  str = "-3.141592653589793238";
  CPPUNIT_ASSERT ( str.toDouble() == -3.141592653589793238 );
}

//----------------------------------------------------------------------
void FStringTest::convertFromNumberTest()
{
  constexpr sInt8       n1  = -12;
  constexpr uInt8       n2  =  12U;
  constexpr sInt16      n3  = -1234;
  constexpr uInt16      n4  =  1234U;
  constexpr int         n5  = -12345;
  constexpr uInt        n6  =  12345U;
  constexpr sInt32      n7  = -12345;
  constexpr uInt32      n8  =  12345U;
  constexpr long        n9  = -12345678;
  constexpr uLong       n10 =  12345678U;
  constexpr sInt64      n11 = -12345678;
  constexpr uInt64      n12 =  12345678U;
  constexpr std::wint_t n13 =  12345678;
  constexpr std::size_t n14 =  12345678;
  constexpr float       n15 =  1234.56F;
  constexpr double      n16 =  1234.5678;
  constexpr lDouble     n17 =  12345.67890L;

  CPPUNIT_ASSERT ( int(finalcut::getPrecision<float>()) == FLT_DIG );
  CPPUNIT_ASSERT ( int(finalcut::getPrecision<double>()) == DBL_DIG );
  CPPUNIT_ASSERT ( int(finalcut::getPrecision<lDouble>()) == LDBL_DIG );

  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n1) == "-12" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n2) == "12" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n3) == "-1234" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n4) == "1234" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n5) == "-12345" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n6) == "12345" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n7) == "-12345" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n8) == "12345" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n9) == "-12345678" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n10) == "12345678" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n11) == "-12345678" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n12) == "12345678" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n13) == "12345678" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n14) == "12345678" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n15) == "1234.56" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n16) == "1234.5678" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n17) == "12345.6789" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n15, 0) == "1e+03" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n16, 0) == "1e+03" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n17, 0) == "1e+04" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n15, 100)
                   == "1234.56005859375" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n16, 100)
                   == "1234.567800000000033833202905952930450439453125" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n17, 100)
                   == "12345.67889999999999961488583721802569925785064697265625" );
}

//----------------------------------------------------------------------
void FStringTest::exceptionTest()
{
  CPPUNIT_ASSERT_THROW ( finalcut::FString("abc").toULong()
                       , std::invalid_argument );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("").toULong()
                       , std::invalid_argument );

  CPPUNIT_ASSERT_THROW ( finalcut::FString().toULong()
                       , std::invalid_argument );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("abc")[4]
                       , std::out_of_range );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("abc")[-1]
                       , std::out_of_range );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("abc").at(4)
                       , std::out_of_range );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("abc").at(-1)
                       , std::out_of_range );

  const finalcut::FString const_fstring("const");

  CPPUNIT_ASSERT_THROW ( const_fstring[6]
                       , std::out_of_range );

  CPPUNIT_ASSERT_THROW ( const_fstring[-1]
                       , std::out_of_range );

  CPPUNIT_ASSERT_THROW ( const_fstring.at(6)
                       , std::out_of_range );

  CPPUNIT_ASSERT_THROW ( const_fstring.at(-1)
                       , std::out_of_range );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("99999").toShort()
                       , std::overflow_error );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("-99999").toShort()
                       , std::underflow_error );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("99999").toUShort()
                       , std::overflow_error );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("-1").toUShort()
                       , std::underflow_error );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("9999999999").toInt()
                       , std::overflow_error );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("-9999999999").toInt()
                       , std::underflow_error );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("9999999999").toUInt()
                       , std::overflow_error );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("-1").toUInt()
                       , std::underflow_error );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("9999999999999999999").toLong()
                       , std::overflow_error );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("-9999999999999999999").toLong()
                       , std::underflow_error );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("99999999999999999999").toULong()
                       , std::overflow_error );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("-1").toULong()
                       , std::underflow_error );

  CPPUNIT_ASSERT_THROW ( finalcut::FString().toLong()
                       , std::invalid_argument );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("").toLong()
                       , std::invalid_argument );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("one").toLong()
                       , std::invalid_argument );

  CPPUNIT_ASSERT_THROW ( finalcut::FString().toULong()
                       , std::invalid_argument );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("").toULong()
                       , std::invalid_argument );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("one").toULong()
                       , std::invalid_argument );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("1E+42").toFloat()
                       , std::overflow_error );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("-1E+42").toFloat()
                       , std::overflow_error );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("1.19209290E-08").toFloat()
                       , std::underflow_error );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("1.7976931348623157E+309").toDouble()
                       , std::overflow_error );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("-1.7976931348623157E+309").toDouble()
                       , std::overflow_error );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("2.225074e-310").toDouble()
                       , std::underflow_error );

  CPPUNIT_ASSERT_THROW ( finalcut::FString().toDouble()
                       , std::invalid_argument );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("").toDouble()
                       , std::invalid_argument );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("one").toDouble()
                       , std::invalid_argument );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("ABC").insert(finalcut::FString("abc"), 4)
                       , std::out_of_range );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("ABC").insert(finalcut::FString("abc"), -1)
                       , std::out_of_range );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("ABC").insert(L"abc", 4)
                       , std::out_of_range );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("ABC").insert(L"abc", -1)
                       , std::out_of_range );
}

//----------------------------------------------------------------------
void FStringTest::trimTest()
{
  const finalcut::FString& trim_str1 = L"\r\n\t  A string \n\t";
  CPPUNIT_ASSERT ( trim_str1.getLength() == 16 );
  CPPUNIT_ASSERT ( *(trim_str1.c_str() + trim_str1.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(trim_str1.wc_str() + trim_str1.getLength()) == L'\0' );
  CPPUNIT_ASSERT ( trim_str1.rtrim() == L"\r\n\t  A string" );
  CPPUNIT_ASSERT ( trim_str1.rtrim().getLength() == 13 );
  CPPUNIT_ASSERT ( *(trim_str1.rtrim().c_str() + trim_str1.rtrim().getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(trim_str1.rtrim().wc_str() + trim_str1.rtrim().getLength()) == L'\0' );
  CPPUNIT_ASSERT ( trim_str1.ltrim() == L"A string \n\t" );
  CPPUNIT_ASSERT ( trim_str1.ltrim().getLength() == 11 );
  CPPUNIT_ASSERT ( *(trim_str1.ltrim().c_str() + trim_str1.ltrim().getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(trim_str1.ltrim().wc_str() + trim_str1.ltrim().getLength()) == L'\0' );
  CPPUNIT_ASSERT ( trim_str1.trim() == L"A string" );
  CPPUNIT_ASSERT ( trim_str1.trim().getLength() == 8 );
  CPPUNIT_ASSERT ( *(trim_str1.trim().c_str() + trim_str1.trim().getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(trim_str1.trim().wc_str() + trim_str1.trim().getLength()) == L'\0' );

  const finalcut::FString& trim_str2 = L"\n  \n\n";
  CPPUNIT_ASSERT ( trim_str2.rtrim().isEmpty() );
  CPPUNIT_ASSERT ( trim_str2.rtrim().getLength() == 0 );
  CPPUNIT_ASSERT ( trim_str2.rtrim().capacity() < std::wstring().max_size() );
  CPPUNIT_ASSERT ( *(trim_str2.rtrim().c_str() + trim_str2.rtrim().getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(trim_str2.rtrim().wc_str() + trim_str2.rtrim().getLength()) == L'\0' );
  CPPUNIT_ASSERT ( trim_str2.ltrim().isEmpty() );
  CPPUNIT_ASSERT ( trim_str2.ltrim().getLength() == 0 );
  CPPUNIT_ASSERT ( trim_str2.ltrim().capacity() < std::wstring().max_size() );
  CPPUNIT_ASSERT ( *(trim_str2.ltrim().c_str() + trim_str2.ltrim().getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(trim_str2.ltrim().wc_str() + trim_str2.ltrim().getLength()) == L'\0' );

  const finalcut::FString trim_str3{};
  CPPUNIT_ASSERT ( trim_str3.ltrim().isEmpty() );
  CPPUNIT_ASSERT ( trim_str3.ltrim().getLength() == 0 );
  CPPUNIT_ASSERT ( trim_str3.ltrim().capacity() < std::wstring().max_size() );
  CPPUNIT_ASSERT ( trim_str3.rtrim().isEmpty() );
  CPPUNIT_ASSERT ( trim_str3.rtrim().isEmpty() );
  CPPUNIT_ASSERT ( trim_str3.rtrim().getLength() == 0 );
  CPPUNIT_ASSERT ( trim_str3.rtrim().capacity() < std::wstring().max_size() );
  CPPUNIT_ASSERT ( trim_str3.trim().isEmpty() );
  CPPUNIT_ASSERT ( trim_str3.trim().getLength() == 0 );
  CPPUNIT_ASSERT ( trim_str3.trim().capacity() < std::wstring().max_size() );

  const finalcut::FString trim_str4 = "x";
  CPPUNIT_ASSERT ( trim_str4.ltrim() == "x" );
  CPPUNIT_ASSERT ( ! trim_str4.ltrim().isEmpty() );
  CPPUNIT_ASSERT ( trim_str4.ltrim().getLength() == 1 );
  CPPUNIT_ASSERT ( trim_str4.ltrim().capacity() < std::wstring().max_size() );
  CPPUNIT_ASSERT ( trim_str4.rtrim() == "x" );
  CPPUNIT_ASSERT ( ! trim_str4.rtrim().isEmpty() );
  CPPUNIT_ASSERT ( trim_str4.rtrim().getLength() == 1 );
  CPPUNIT_ASSERT ( trim_str4.rtrim().capacity() < std::wstring().max_size() );
  CPPUNIT_ASSERT ( trim_str4.trim() == "x" );
  CPPUNIT_ASSERT ( ! trim_str4.trim().isEmpty() );
  CPPUNIT_ASSERT ( trim_str4.trim().getLength() == 1 );
  CPPUNIT_ASSERT ( trim_str4.trim().capacity() < std::wstring().max_size() );
}

//----------------------------------------------------------------------
void FStringTest::subStringTest()
{
  finalcut::FString str1("Look behind you, a three-headed monkey!");
  CPPUNIT_ASSERT ( str1.left(uInt(11)) == L"Look behind" );
  CPPUNIT_ASSERT ( str1.left(int(11)) == L"Look behind" );
  CPPUNIT_ASSERT ( str1.left(11).getLength() == 11 );
  CPPUNIT_ASSERT ( *(str1.left(11).c_str() + str1.left(11).getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str1.left(11).wc_str() + str1.left(11).getLength()) == L'\0' );
  CPPUNIT_ASSERT ( str1.left(999)
                   == L"Look behind you, a three-headed monkey!" );
  CPPUNIT_ASSERT ( str1.left(999).getLength() == 39 );
  CPPUNIT_ASSERT ( str1.left(-5)
                   == L"Look behind you, a three-headed monkey!" );
  CPPUNIT_ASSERT ( str1.left(-5).getLength() == 39 );
  CPPUNIT_ASSERT ( str1.left(0) == L"" );
  CPPUNIT_ASSERT ( str1.left(0).isEmpty() );
  CPPUNIT_ASSERT ( finalcut::FString().left(5).isEmpty() );
  CPPUNIT_ASSERT ( finalcut::FString("").left(5).isEmpty() );
  CPPUNIT_ASSERT ( finalcut::FString("").left(5).getLength() == 0 );

  CPPUNIT_ASSERT ( str1.right(uInt(7)) == L"monkey!" );
  CPPUNIT_ASSERT ( str1.right(int(7)) == L"monkey!" );
  CPPUNIT_ASSERT ( str1.right(7).getLength() == 7 );
  CPPUNIT_ASSERT ( *(str1.right(7).c_str() + str1.right(7).getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str1.right(7).wc_str() + str1.right(7).getLength()) == L'\0' );
  CPPUNIT_ASSERT ( str1.right(999)
                   == L"Look behind you, a three-headed monkey!" );
  CPPUNIT_ASSERT ( str1.right(999).getLength() == 39 );
  CPPUNIT_ASSERT ( str1.right(-5)
                   == L"Look behind you, a three-headed monkey!" );
  CPPUNIT_ASSERT ( str1.right(-5).getLength() == 39 );
  CPPUNIT_ASSERT ( str1.right(0) == L"" );
  CPPUNIT_ASSERT ( str1.right(0).isEmpty() );
  CPPUNIT_ASSERT ( finalcut::FString().right(5).isEmpty() );
  CPPUNIT_ASSERT ( finalcut::FString("").right(5).isEmpty() );
  CPPUNIT_ASSERT ( finalcut::FString("").right(5).getLength() == 0 );

  CPPUNIT_ASSERT ( str1.mid(uInt(18), uInt(21))
                   == L"a three-headed monkey" );
  CPPUNIT_ASSERT ( str1.mid(int(18), int(21))
                   == L"a three-headed monkey" );
  CPPUNIT_ASSERT ( str1.mid(18, 21).getLength() == 21 );
  CPPUNIT_ASSERT ( *(str1.mid(18, 21).c_str() + str1.mid(18, 21).getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str1.mid(18, 21).wc_str() + str1.mid(18, 21).getLength()) == L'\0' );
  CPPUNIT_ASSERT ( str1.mid(1, 999)
                   == L"Look behind you, a three-headed monkey!" );
  CPPUNIT_ASSERT ( str1.mid(1, 999).getLength() == 39 );
  CPPUNIT_ASSERT ( str1.mid(5, 0) == L"" );
  CPPUNIT_ASSERT ( str1.mid(-5, 2) == L"" );
  CPPUNIT_ASSERT ( str1.mid(0, 0) == L"" );
  CPPUNIT_ASSERT ( str1.mid(0, 5) == L"Look " );
  CPPUNIT_ASSERT ( str1.mid(0, 0).isEmpty() );
  CPPUNIT_ASSERT ( finalcut::FString().mid(5, 0).isEmpty() );
  CPPUNIT_ASSERT ( finalcut::FString("").mid(5, 0).isEmpty() );
  CPPUNIT_ASSERT ( str1.mid(5, 0).getLength() == 0 );

  finalcut::FString mid_str{"ABCDE"};
  CPPUNIT_ASSERT ( mid_str.mid(1, 2) == L"AB" );
  CPPUNIT_ASSERT ( mid_str.mid(2, 3) == L"BCD" );
  CPPUNIT_ASSERT ( mid_str.mid(3, 10) == L"CDE" );
  CPPUNIT_ASSERT ( mid_str.mid(0, 2) == L"AB" );
  CPPUNIT_ASSERT ( mid_str.mid(6, 2) == L"" );
  CPPUNIT_ASSERT ( mid_str.mid(3, 0) == L"" );

  finalcut::FStringList string_parts = str1.split(" ");
  finalcut::FStringList string_list;
  string_list.push_back("Look");
  string_list.push_back("behind");
  string_list.push_back("you,");
  string_list.push_back("a");
  string_list.push_back("three-headed");
  string_list.push_back("monkey!");
  CPPUNIT_ASSERT ( string_parts == string_list );

  string_parts = str1.split(L',');  //  wchar_t
  string_list.clear();
  string_list.push_back("Look behind you");
  string_list.push_back(" a three-headed monkey!");
  CPPUNIT_ASSERT ( string_parts == string_list );

  string_parts = str1.split(',');  // char
  CPPUNIT_ASSERT ( string_parts == string_list );

  string_parts = finalcut::FString().split(':');
  CPPUNIT_ASSERT ( string_parts.empty() );
  CPPUNIT_ASSERT ( string_parts.size() == 0 );

  finalcut::FString str2{};  // Empty string
  string_parts = str2.split("x");
  CPPUNIT_ASSERT ( str2.isEmpty() );
  CPPUNIT_ASSERT ( string_parts.empty() );
  CPPUNIT_ASSERT ( string_parts.size() == 0 );

  str2 = "abcdefghijkl";
  string_parts = str2.split("x");  // The delimiter does not exist
  CPPUNIT_ASSERT ( ! str2.isEmpty() );
  CPPUNIT_ASSERT ( ! string_parts.empty() );
  CPPUNIT_ASSERT ( string_parts.size() == 1 );

  string_parts = str2.split("");  // The delimiter is empty
  CPPUNIT_ASSERT ( ! str2.isEmpty() );
  CPPUNIT_ASSERT ( ! string_parts.empty() );
  CPPUNIT_ASSERT ( string_parts.size() == 1 );
}

//----------------------------------------------------------------------
void FStringTest::insertTest()
{
  finalcut::FString str1 = "ABC";
  const finalcut::FString str2 = "xyz";
  CPPUNIT_ASSERT ( str1.getLength() == 3 );
  CPPUNIT_ASSERT ( str1.insert(str2, 0) == "xyzABC" );
  CPPUNIT_ASSERT ( str1.getLength() == 6 );
  CPPUNIT_ASSERT ( *(str1.c_str() + str1.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str1.wc_str() + str1.getLength()) == L'\0' );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.getLength() == 3 );
  CPPUNIT_ASSERT ( str1.insert(str2, std::size_t(1)) == "AxyzBC" );
  CPPUNIT_ASSERT ( str1.getLength() == 6 );
  CPPUNIT_ASSERT ( *(str1.c_str() + str1.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str1.wc_str() + str1.getLength()) == L'\0' );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.getLength() == 3 );
  CPPUNIT_ASSERT ( str1.insert(str2, 2) == "ABxyzC" );
  CPPUNIT_ASSERT ( str1.getLength() == 6 );
  CPPUNIT_ASSERT ( *(str1.c_str() + str1.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str1.wc_str() + str1.getLength()) == L'\0' );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.getLength() == 3 );
  CPPUNIT_ASSERT ( str1.insert(str2, 3) == "ABCxyz" );
  CPPUNIT_ASSERT ( str1.getLength() == 6 );
  CPPUNIT_ASSERT ( *(str1.c_str() + str1.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str1.wc_str() + str1.getLength()) == L'\0' );

  str1 = "ABC";
  constexpr wchar_t str3[] = L"xyz";
  CPPUNIT_ASSERT ( str1.getLength() == 3 );
  CPPUNIT_ASSERT ( str1.insert(str3, 0) == "xyzABC" );
  CPPUNIT_ASSERT ( str1.getLength() == 6 );
  CPPUNIT_ASSERT ( *(str1.c_str() + str1.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str1.wc_str() + str1.getLength()) == L'\0' );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.getLength() == 3 );
  CPPUNIT_ASSERT ( str1.insert(str3, 1) == "AxyzBC" );
  CPPUNIT_ASSERT ( str1.getLength() == 6 );
  CPPUNIT_ASSERT ( *(str1.c_str() + str1.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str1.wc_str() + str1.getLength()) == L'\0' );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.getLength() == 3 );
  CPPUNIT_ASSERT ( str1.insert(str3, 2) == "ABxyzC" );
  CPPUNIT_ASSERT ( str1.getLength() == 6 );
  CPPUNIT_ASSERT ( *(str1.c_str() + str1.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str1.wc_str() + str1.getLength()) == L'\0' );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.getLength() == 3 );
  CPPUNIT_ASSERT ( str1.insert(str3, 3) == "ABCxyz" );
  CPPUNIT_ASSERT ( str1.getLength() == 6 );
  CPPUNIT_ASSERT ( *(str1.c_str() + str1.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str1.wc_str() + str1.getLength()) == L'\0' );

  str1 = "ABC";
  constexpr char str4[] = "xyz";
  CPPUNIT_ASSERT ( str1.getLength() == 3 );
  CPPUNIT_ASSERT ( str1.insert(str4, 0) == "xyzABC" );
  CPPUNIT_ASSERT ( str1.getLength() == 6 );
  CPPUNIT_ASSERT ( *(str1.c_str() + str1.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str1.wc_str() + str1.getLength()) == L'\0' );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.getLength() == 3 );
  CPPUNIT_ASSERT ( str1.insert(str4, 1) == "AxyzBC" );
  CPPUNIT_ASSERT ( str1.getLength() == 6 );
  CPPUNIT_ASSERT ( *(str1.c_str() + str1.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str1.wc_str() + str1.getLength()) == L'\0' );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.getLength() == 3 );
  CPPUNIT_ASSERT ( str1.insert(str4, 2) == "ABxyzC" );
  CPPUNIT_ASSERT ( str1.getLength() == 6 );
  CPPUNIT_ASSERT ( *(str1.c_str() + str1.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str1.wc_str() + str1.getLength()) == L'\0' );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.getLength() == 3 );
  CPPUNIT_ASSERT ( str1.insert(str4, 3) == "ABCxyz" );
  CPPUNIT_ASSERT ( str1.getLength() == 6 );
  CPPUNIT_ASSERT ( *(str1.c_str() + str1.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str1.wc_str() + str1.getLength()) == L'\0' );

  str1 = "ABC";
  constexpr wchar_t wc = L'*';
  CPPUNIT_ASSERT ( str1.getLength() == 3 );
  CPPUNIT_ASSERT ( str1.insert(wc, 0) == "*ABC" );
  CPPUNIT_ASSERT ( str1.getLength() == 4 );
  CPPUNIT_ASSERT ( *(str1.c_str() + str1.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str1.wc_str() + str1.getLength()) == L'\0' );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.getLength() == 3 );
  CPPUNIT_ASSERT ( str1.insert(wc, 1) == "A*BC" );
  CPPUNIT_ASSERT ( str1.getLength() == 4 );
  CPPUNIT_ASSERT ( *(str1.c_str() + str1.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str1.wc_str() + str1.getLength()) == L'\0' );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.getLength() == 3 );
  CPPUNIT_ASSERT ( str1.insert(wc, 2) == "AB*C" );
  CPPUNIT_ASSERT ( str1.getLength() == 4 );
  CPPUNIT_ASSERT ( *(str1.c_str() + str1.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str1.wc_str() + str1.getLength()) == L'\0' );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.getLength() == 3 );
  CPPUNIT_ASSERT ( str1.insert(wc, 3) == "ABC*" );
  CPPUNIT_ASSERT ( str1.getLength() == 4 );
  CPPUNIT_ASSERT ( *(str1.c_str() + str1.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str1.wc_str() + str1.getLength()) == L'\0' );

  str1 = "ABC";
  constexpr char c = '*';
  CPPUNIT_ASSERT ( str1.getLength() == 3 );
  CPPUNIT_ASSERT ( str1.insert(c, 0) == "*ABC" );
  CPPUNIT_ASSERT ( str1.getLength() == 4 );
  CPPUNIT_ASSERT ( *(str1.c_str() + str1.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str1.wc_str() + str1.getLength()) == L'\0' );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.getLength() == 3 );
  CPPUNIT_ASSERT ( str1.insert(c, 1) == "A*BC" );
  CPPUNIT_ASSERT ( str1.getLength() == 4 );
  CPPUNIT_ASSERT ( *(str1.c_str() + str1.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str1.wc_str() + str1.getLength()) == L'\0' );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.getLength() == 3 );
  CPPUNIT_ASSERT ( str1.insert(c, 2) == "AB*C" );
  CPPUNIT_ASSERT ( str1.getLength() == 4 );
  CPPUNIT_ASSERT ( *(str1.c_str() + str1.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str1.wc_str() + str1.getLength()) == L'\0' );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.getLength() == 3 );
  CPPUNIT_ASSERT ( str1.insert(c, 3) == "ABC*" );
  CPPUNIT_ASSERT ( str1.getLength() == 4 );
  CPPUNIT_ASSERT ( *(str1.c_str() + str1.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str1.wc_str() + str1.getLength()) == L'\0' );
}

//----------------------------------------------------------------------
void FStringTest::replaceTest()
{
  const finalcut::FString str     = "Look behind you, a three-headed monkey!";
  finalcut::FString       s1      = str;
  const finalcut::FString from1   = "three";
  const std::wstring      from2   = L"three";
  constexpr wchar_t       from3[] = L"three";
#if __cplusplus >= 201703L
  const std::wstring_view from4   = L"three";
#endif
  const std::string       from5   = "three";
#if __cplusplus >= 201703L
  const std::string_view  from6   = "three";
#endif
  constexpr char          from7[] = "three";
  constexpr wchar_t       from8   = L',';
  constexpr char          from9   = ',';
  const finalcut::FString to1     = L'3';
  const std::wstring      to2     = L"3";
  constexpr wchar_t       to3[]   = L"3";
#if __cplusplus >= 201703L
  const std::wstring_view to4     = L"3";
#endif
  const std::string       to5     = "3";
#if __cplusplus >= 201703L
  const std::string_view  to6     = "3";
#endif
  constexpr char          to7[]   = "3";
  constexpr wchar_t       to8     = '3';
  constexpr char          to9     = '3';

  CPPUNIT_ASSERT ( s1.replace(from1, to1)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from1, to2)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from1, to3)
                   == "Look behind you, a 3-headed monkey!" );
#if __cplusplus >= 201703L
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from1, to4)
                   == "Look behind you, a 3-headed monkey!" );
#endif
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from1, to5)
                   == "Look behind you, a 3-headed monkey!" );
#if __cplusplus >= 201703L
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from1, to6)
                   == "Look behind you, a 3-headed monkey!" );
#endif
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from1, to7)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from1, to8)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from1, to9)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from2, to1)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from2, to2)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from2, to3)
                   == "Look behind you, a 3-headed monkey!" );
#if __cplusplus >= 201703L
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from2, to4)
                   == "Look behind you, a 3-headed monkey!" );
#endif
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from2, to5)
                   == "Look behind you, a 3-headed monkey!" );
#if __cplusplus >= 201703L
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from2, to6)
                   == "Look behind you, a 3-headed monkey!" );
#endif
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from2, to7)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from2, to8)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from2, to9)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from3, to1)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from3, to2)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from3, to3)
                   == "Look behind you, a 3-headed monkey!" );
#if __cplusplus >= 201703L
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from3, to4)
                   == "Look behind you, a 3-headed monkey!" );
#endif
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from3, to5)
                   == "Look behind you, a 3-headed monkey!" );
#if __cplusplus >= 201703L
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from3, to6)
                   == "Look behind you, a 3-headed monkey!" );
#endif
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from3, to7)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from3, to8)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from3, to9)
                   == "Look behind you, a 3-headed monkey!" );
#if __cplusplus >= 201703L
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from4, to1)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from4, to2)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from4, to3)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from4, to4)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from4, to5)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from4, to6)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from4, to7)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from4, to8)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from4, to9)
                   == "Look behind you, a 3-headed monkey!" );
#endif
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from5, to1)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from5, to2)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from5, to3)
                   == "Look behind you, a 3-headed monkey!" );
#if __cplusplus >= 201703L
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from5, to4)
                   == "Look behind you, a 3-headed monkey!" );
#endif
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from5, to5)
                   == "Look behind you, a 3-headed monkey!" );
#if __cplusplus >= 201703L
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from5, to6)
                   == "Look behind you, a 3-headed monkey!" );
#endif
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from5, to7)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from5, to8)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from5, to9)
                   == "Look behind you, a 3-headed monkey!" );
#if __cplusplus >= 201703L
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from6, to1)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from6, to2)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from6, to3)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from6, to4)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from6, to5)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from6, to6)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from6, to7)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from6, to8)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from6, to9)
                   == "Look behind you, a 3-headed monkey!" );
#endif
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from7, to1)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from7, to2)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from7, to3)
                   == "Look behind you, a 3-headed monkey!" );
#if __cplusplus >= 201703L
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from7, to4)
                   == "Look behind you, a 3-headed monkey!" );
#endif
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from7, to5)
                   == "Look behind you, a 3-headed monkey!" );
#if __cplusplus >= 201703L
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from7, to6)
                   == "Look behind you, a 3-headed monkey!" );
#endif
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from7, to7)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from7, to8)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from7, to9)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from8, to1)
                   == "Look behind you3 a three-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from8, to2)
                   == "Look behind you3 a three-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from8, to3)
                   == "Look behind you3 a three-headed monkey!" );
#if __cplusplus >= 201703L
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from8, to4)
                   == "Look behind you3 a three-headed monkey!" );
#endif
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from8, to5)
                   == "Look behind you3 a three-headed monkey!" );
#if __cplusplus >= 201703L
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from8, to6)
                   == "Look behind you3 a three-headed monkey!" );
#endif
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from8, to7)
                   == "Look behind you3 a three-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from8, to8)
                   == "Look behind you3 a three-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from8, to9)
                   == "Look behind you3 a three-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from9, to1)
                   == "Look behind you3 a three-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from9, to2)
                   == "Look behind you3 a three-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from9, to3)
                   == "Look behind you3 a three-headed monkey!" );
#if __cplusplus >= 201703L
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from9, to4)
                   == "Look behind you3 a three-headed monkey!" );
#endif
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from9, to5)
                   == "Look behind you3 a three-headed monkey!" );
#if __cplusplus >= 201703L
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from9, to6)
                   == "Look behind you3 a three-headed monkey!" );
#endif
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from9, to7)
                   == "Look behind you3 a three-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from9, to8)
                   == "Look behind you3 a three-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from9, to9)
                   == "Look behind you3 a three-headed monkey!" );

  s1 = "A big ball and a small ball";
  CPPUNIT_ASSERT ( s1.replace("ball", "globe")
                   == "A big globe and a small globe" );
  CPPUNIT_ASSERT ( s1.getLength() == 27 );
  CPPUNIT_ASSERT ( s1.replace("ball", "globe").getLength() == 29 );
  CPPUNIT_ASSERT ( *(s1.replace("ball", "globe").c_str()
                   + s1.replace("ball", "globe").getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(s1.replace("ball", "globe").wc_str()
                   + s1.replace("ball", "globe").getLength()) == L'\0' );

  s1 = "ABC";
  finalcut::FString empty;
  CPPUNIT_ASSERT ( s1.replace('B', "") == "AC" );
  CPPUNIT_ASSERT ( s1.replace('B', "").getLength() == 2 );
  CPPUNIT_ASSERT ( s1.replace(L'B', "") == "AC" );
  CPPUNIT_ASSERT ( s1.replace(L'B', "").getLength() == 2 );
  CPPUNIT_ASSERT ( s1.replace(from1, empty) == "ABC" );
  CPPUNIT_ASSERT ( s1.replace(from1, empty).getLength() == 3 );
  CPPUNIT_ASSERT ( s1.replace(from3, empty) == "ABC" );
  CPPUNIT_ASSERT ( s1.replace(from3, empty).getLength() == 3 );
#if __cplusplus >= 201703L
  CPPUNIT_ASSERT ( s1.replace(from4, empty) == "ABC" );
  CPPUNIT_ASSERT ( s1.replace(from4, empty).getLength() == 3 );
  CPPUNIT_ASSERT ( s1.replace(from6, empty) == "ABC" );
  CPPUNIT_ASSERT ( s1.replace(from6, empty).getLength() == 3 );
#endif
  CPPUNIT_ASSERT ( s1.replace(from7, to7) == "ABC" );
  CPPUNIT_ASSERT ( s1.replace(from7, to7).getLength() == 3 );
  CPPUNIT_ASSERT ( s1.replace(empty, to1) == "ABC" );
  CPPUNIT_ASSERT ( s1.replace(empty, to1).getLength() == 3 );
  CPPUNIT_ASSERT ( s1.replace(from8, empty) == "ABC"  );
  CPPUNIT_ASSERT ( s1.replace(from8, empty).getLength() == 3 );

  empty = "";
  CPPUNIT_ASSERT ( s1.replace(from1, empty) == "ABC" );
  CPPUNIT_ASSERT ( s1.replace(from3, empty) == "ABC" );
#if __cplusplus >= 201703L
  CPPUNIT_ASSERT ( s1.replace(from4, to4) == "ABC" );
  CPPUNIT_ASSERT ( s1.replace(from6, to6) == "ABC" );
#endif
  CPPUNIT_ASSERT ( s1.replace(from7, to7) == "ABC" );
  CPPUNIT_ASSERT ( s1.replace(empty, to1) == "ABC" );
  CPPUNIT_ASSERT ( s1.replace(from8, empty) == "ABC"  );

  s1.clear();
  CPPUNIT_ASSERT ( s1.replace(from1, to1).isEmpty() );
  CPPUNIT_ASSERT ( s1.replace(from8, to1).isEmpty() );

  CPPUNIT_ASSERT ( s1.replace(from7, to7).isEmpty() );
  CPPUNIT_ASSERT ( s1.replace(from9, to9).isEmpty() );
}

//----------------------------------------------------------------------
void FStringTest::overwriteTest()
{
  // finalcut::FString
  finalcut::FString str = "abcdefghijklm";
  CPPUNIT_ASSERT ( str.getLength() == 13 );
  CPPUNIT_ASSERT ( str.overwrite("+++++++", 3) == "abc+++++++klm" );
  CPPUNIT_ASSERT ( str.getLength() == 13 );
  CPPUNIT_ASSERT ( *(str.c_str() + str.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str.wc_str() + str.getLength()) == L'\0' );
  CPPUNIT_ASSERT ( str.overwrite(".............") == "............." );
  CPPUNIT_ASSERT ( str.getLength() == 13 );
  CPPUNIT_ASSERT ( *(str.c_str() + str.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str.wc_str() + str.getLength()) == L'\0' );
  CPPUNIT_ASSERT ( str.overwrite(",,,,,,,,,,,,,,,") == ",,,,,,,,,,,,,,," );
  CPPUNIT_ASSERT ( str.getLength() == 15 );
  CPPUNIT_ASSERT ( *(str.c_str() + str.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str.wc_str() + str.getLength()) == L'\0' );
  str = "abc";
  CPPUNIT_ASSERT ( str.getLength() == 3 );
  CPPUNIT_ASSERT ( str.overwrite("+++++", 99) == "abc+++++" );
  CPPUNIT_ASSERT ( str.getLength() == 8 );
  CPPUNIT_ASSERT ( *(str.c_str() + str.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str.wc_str() + str.getLength()) == L'\0' );
  str = "abc";
  CPPUNIT_ASSERT ( str.overwrite("+++", -5) == "+++" );
  CPPUNIT_ASSERT ( str.getLength() == 3 );
  CPPUNIT_ASSERT ( *(str.c_str() + str.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str.wc_str() + str.getLength()) == L'\0' );

  // Wide string
  str = "abcdefghijklm";
  CPPUNIT_ASSERT ( str.overwrite(L"+++++++", 3) == "abc+++++++klm" );
  CPPUNIT_ASSERT ( str.overwrite(L".............") == "............." );
  CPPUNIT_ASSERT ( str.overwrite(L",,,,,,,,,,,,,,,") == ",,,,,,,,,,,,,,," );
  str = "abc";
  CPPUNIT_ASSERT ( str.overwrite(L"+++++", 99) == "abc+++++" );
  str = "abc";
  CPPUNIT_ASSERT ( str.overwrite(L"+++", -5) == "+++" );

  // Wide char
  str = "abcdefghijklm";
  CPPUNIT_ASSERT ( str.overwrite(L'+', 3) == "abc+efghijklm" );
  CPPUNIT_ASSERT ( str.overwrite(L'.') == ".bc+efghijklm" );
  CPPUNIT_ASSERT ( str.overwrite(L',', 12) == ".bc+efghijkl," );
  CPPUNIT_ASSERT ( str.overwrite(L'.', 13) == ".bc+efghijkl,." );
  str = "abc";
  CPPUNIT_ASSERT ( str.overwrite(L'+', 99) == "abc+" );
  str = "abc";
  CPPUNIT_ASSERT ( str.overwrite(L'+', -5) == "+bc" );
}

//----------------------------------------------------------------------
void FStringTest::removeTest()
{
  finalcut::FString str = "ABCDE";
  CPPUNIT_ASSERT ( str.remove(2, 2) == "ABE" );
  CPPUNIT_ASSERT ( str.getLength() == 3 );
  CPPUNIT_ASSERT ( *(str.c_str() + str.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str.wc_str() + str.getLength()) == L'\0' );

  // Remove last character
  CPPUNIT_ASSERT ( str.remove(2, 1) == "AB" );
  CPPUNIT_ASSERT ( str.getLength() == 2 );
  CPPUNIT_ASSERT ( *(str.c_str() + str.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str.wc_str() + str.getLength()) == L'\0' );

  // Remove after the last character
  CPPUNIT_ASSERT ( str.remove(2, 1) == "AB" );
  CPPUNIT_ASSERT ( str.getLength() == 2 );
  CPPUNIT_ASSERT ( *(str.c_str() + str.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str.wc_str() + str.getLength()) == L'\0' );

  // Remove after the last character
  CPPUNIT_ASSERT ( str.remove(2, 5) == "AB" );
  CPPUNIT_ASSERT ( str.getLength() == 2 );
  CPPUNIT_ASSERT ( *(str.c_str() + str.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str.wc_str() + str.getLength()) == L'\0' );

  str = "ABCDE";
  CPPUNIT_ASSERT ( str.getLength() == 5 );
  CPPUNIT_ASSERT ( str.remove(2, 99) == "AB" );
  CPPUNIT_ASSERT ( str.getLength() == 2 );
  CPPUNIT_ASSERT ( *(str.c_str() + str.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str.wc_str() + str.getLength()) == L'\0' );

  // Remove after the last character
  CPPUNIT_ASSERT ( str.remove(99, 1) == "AB" );
  CPPUNIT_ASSERT ( str.getLength() == 2 );
  CPPUNIT_ASSERT ( *(str.c_str() + str.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str.wc_str() + str.getLength()) == L'\0' );

  // Erase
  str = "ABCDE";
  CPPUNIT_ASSERT ( str.erase(2, 2) == "ABE" );
  CPPUNIT_ASSERT ( str.getLength() == 3 );
  CPPUNIT_ASSERT ( *(str.c_str() + str.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str.wc_str() + str.getLength()) == L'\0' );

  // Erase last character
  CPPUNIT_ASSERT ( str.erase(2, 1) == "AB" );
  CPPUNIT_ASSERT ( str.getLength() == 2 );
  CPPUNIT_ASSERT ( *(str.c_str() + str.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str.wc_str() + str.getLength()) == L'\0' );

  // Erase after the last character
  CPPUNIT_ASSERT ( str.erase(2, 1) == "AB" );
  CPPUNIT_ASSERT ( str.getLength() == 2 );
  CPPUNIT_ASSERT ( *(str.c_str() + str.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str.wc_str() + str.getLength()) == L'\0' );

  // Erase first character
  CPPUNIT_ASSERT ( str.erase(0, 1) == "B" );
  CPPUNIT_ASSERT ( str.getLength() == 1 );
  CPPUNIT_ASSERT ( *(str.c_str() + str.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str.wc_str() + str.getLength()) == L'\0' );

  // Erase all characters
  CPPUNIT_ASSERT ( str.erase(0, finalcut::FString::npos) == "" );
  CPPUNIT_ASSERT ( str.getLength() == 0 );
  CPPUNIT_ASSERT ( *(str.c_str() + str.getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(str.wc_str() + str.getLength()) == L'\0' );
}

//----------------------------------------------------------------------
void FStringTest::includesTest()
{
  const finalcut::FString str = "Look behind you, a three-headed monkey!";
  const finalcut::FString empty1{};
  constexpr wchar_t*      empty2    = nullptr;
  constexpr char*         empty3    = nullptr;
  const finalcut::FString search1   = "you";
  const finalcut::FString search2   = "me";
  constexpr wchar_t       search3[] = L"you";
  constexpr wchar_t       search4[] = L"me";
  constexpr char          search5[] = "you";
  constexpr char          search6[] = "me";
  constexpr wchar_t       search7   = L'y';
  constexpr wchar_t       search8   = L'&';
  constexpr char          search9   = 'y';
  constexpr char          search10  = '&';

  CPPUNIT_ASSERT ( ! str.includes(static_cast<finalcut::FString>(0)) );
  CPPUNIT_ASSERT ( ! str.includes(empty1) );
  CPPUNIT_ASSERT ( str.includes(search1) );
  CPPUNIT_ASSERT ( ! str.includes(search2) );
  CPPUNIT_ASSERT ( ! empty1.includes(search1) );

  CPPUNIT_ASSERT ( ! str.includes(static_cast<wchar_t*>(nullptr)) );
  CPPUNIT_ASSERT ( ! str.includes(empty2) );
  CPPUNIT_ASSERT ( str.includes(search3) );
  CPPUNIT_ASSERT ( ! str.includes(search4) );
  CPPUNIT_ASSERT ( ! empty1.includes(search3) );

  CPPUNIT_ASSERT ( ! str.includes(static_cast<char*>(nullptr)) );
  CPPUNIT_ASSERT ( ! str.includes(empty3) );
  CPPUNIT_ASSERT ( str.includes(search5) );
  CPPUNIT_ASSERT ( ! str.includes(search6) );
  CPPUNIT_ASSERT ( ! empty1.includes(search5) );

  CPPUNIT_ASSERT ( ! str.includes(wchar_t(L'\0')) );
  CPPUNIT_ASSERT ( str.includes(search7) );
  CPPUNIT_ASSERT ( ! str.includes(search8) );
  CPPUNIT_ASSERT ( ! empty1.includes(search7) );

  CPPUNIT_ASSERT ( ! str.contains(char('\0')) );
  CPPUNIT_ASSERT ( str.contains(search9) );
  CPPUNIT_ASSERT ( ! str.contains(search10) );
  CPPUNIT_ASSERT ( ! empty1.contains(search9) );

  CPPUNIT_ASSERT ( str.find("monkey", 0) == 32 );
  CPPUNIT_ASSERT ( str.find("monkey", 32) == 32 );
  CPPUNIT_ASSERT ( str.find("monkey", 33) == finalcut::FString::npos );
  CPPUNIT_ASSERT ( str.find("MONKEY", 0) == finalcut::FString::npos );
  CPPUNIT_ASSERT ( str.find("o", 3) == 13 );

  CPPUNIT_ASSERT ( str.rfind("monkey", 38) == 32 );
  CPPUNIT_ASSERT ( str.rfind("monkey", 32) == 32 );
  CPPUNIT_ASSERT ( str.rfind("monkey", 31) == finalcut::FString::npos );
  CPPUNIT_ASSERT ( str.rfind("me") == finalcut::FString::npos );
  CPPUNIT_ASSERT ( str.rfind("o") == 33 );
}

//----------------------------------------------------------------------
void FStringTest::controlCodesTest()
{
  finalcut::FString bs_str = "t\b\bTesT\bt";
  CPPUNIT_ASSERT ( bs_str.getLength() == 9 );
  CPPUNIT_ASSERT ( bs_str.removeBackspaces() == "Test" );
  CPPUNIT_ASSERT ( bs_str.removeBackspaces().getLength() == 4 );
  CPPUNIT_ASSERT ( *(bs_str.removeBackspaces().c_str()
                   + bs_str.removeBackspaces().getLength()) == '\0' );
  CPPUNIT_ASSERT ( *(bs_str.removeBackspaces().wc_str()
                   + bs_str.removeBackspaces().getLength()) == L'\0' );
  bs_str = "ABC\b\b\b\b";
  CPPUNIT_ASSERT ( bs_str.removeBackspaces() == "" );
  CPPUNIT_ASSERT ( bs_str.removeBackspaces().isEmpty() );
  CPPUNIT_ASSERT ( bs_str.removeBackspaces().getLength() == 0 );

  bs_str = "abc\b";
  CPPUNIT_ASSERT ( bs_str.removeBackspaces() == "ab" );
  CPPUNIT_ASSERT ( ! bs_str.removeBackspaces().isEmpty() );
  CPPUNIT_ASSERT ( bs_str.removeBackspaces().getLength() == 2 );

  bs_str = "\babc";  // Ignore the backspaces at the beginning
  CPPUNIT_ASSERT ( bs_str.removeBackspaces() == "abc" );
  CPPUNIT_ASSERT ( ! bs_str.removeBackspaces().isEmpty() );
  CPPUNIT_ASSERT ( bs_str.removeBackspaces().getLength() == 3 );

  bs_str = "a\b\b";  // Backspace with nothing to delete
  CPPUNIT_ASSERT ( bs_str.removeBackspaces() == "" );
  CPPUNIT_ASSERT ( bs_str.removeBackspaces().isEmpty() );
  CPPUNIT_ASSERT ( bs_str.removeBackspaces().getLength() == 0 );

  bs_str = "ab\bcd";
  CPPUNIT_ASSERT ( bs_str.removeBackspaces() == "acd" );
  CPPUNIT_ASSERT ( ! bs_str.removeBackspaces().isEmpty() );
  CPPUNIT_ASSERT ( bs_str.removeBackspaces().getLength() == 3 );

  bs_str = "\b\b\b";  // Multiple backspaces at start
  CPPUNIT_ASSERT ( bs_str.removeBackspaces() == "" );
  CPPUNIT_ASSERT ( bs_str.removeBackspaces().isEmpty() );
  CPPUNIT_ASSERT ( bs_str.removeBackspaces().getLength() == 0 );

  bs_str = "";
  CPPUNIT_ASSERT ( bs_str.removeBackspaces() == "" );
  CPPUNIT_ASSERT ( bs_str.removeBackspaces().isEmpty() );
  CPPUNIT_ASSERT ( bs_str.removeBackspaces().getLength() == 0 );

  finalcut::FString del_str = "apple \177\177\177pietree";
  CPPUNIT_ASSERT ( del_str.getLength() == 16 );
  CPPUNIT_ASSERT ( del_str.removeDel() == "apple tree" );
  CPPUNIT_ASSERT ( del_str.removeDel().getLength() == 10 );

  del_str = "\177\177\177\177ABC";
  CPPUNIT_ASSERT ( del_str.getLength() == 7 );
  CPPUNIT_ASSERT ( del_str.removeDel() == "" );
  CPPUNIT_ASSERT ( del_str.removeDel().isEmpty() );
  CPPUNIT_ASSERT ( del_str.removeDel().getLength() == 0 );

  del_str = "abc\177";  // DEL at end, nothing to delete
  CPPUNIT_ASSERT ( del_str.removeDel() == "abc" );
  CPPUNIT_ASSERT ( ! del_str.removeDel().isEmpty() );
  CPPUNIT_ASSERT ( del_str.removeDel().getLength() == 3 );

  del_str = "ab\177c";  // DEL deletes 'c'
  CPPUNIT_ASSERT ( del_str.removeDel() == "ab" );
  CPPUNIT_ASSERT ( ! del_str.removeDel().isEmpty() );
  CPPUNIT_ASSERT ( del_str.removeDel().getLength() == 2 );

  del_str = "a\177\177bc";  // Two DELs delete 'b' and 'c'
  CPPUNIT_ASSERT ( del_str.removeDel() == "a" );
  CPPUNIT_ASSERT ( ! del_str.removeDel().isEmpty() );
  CPPUNIT_ASSERT ( del_str.removeDel().getLength() == 1 );

  del_str = "\177abc";  // DEL deletes 'a'
  CPPUNIT_ASSERT ( del_str.removeDel() == "bc" );
  CPPUNIT_ASSERT ( ! del_str.removeDel().isEmpty() );
  CPPUNIT_ASSERT ( del_str.removeDel().getLength() == 2 );

  del_str = "\177\177\177";  // DELs with nothing to delete
  CPPUNIT_ASSERT ( del_str.removeDel() == "" );
  CPPUNIT_ASSERT ( del_str.removeDel().isEmpty() );
  CPPUNIT_ASSERT ( del_str.removeDel().getLength() == 0 );

  del_str = "a\177b\177c";  // Each DEL deletes next char
  CPPUNIT_ASSERT ( del_str.removeDel() == "a" );
  CPPUNIT_ASSERT ( ! del_str.removeDel().isEmpty() );
  CPPUNIT_ASSERT ( del_str.removeDel().getLength() == 1 );

  finalcut::FString tab_str = "one line";
  CPPUNIT_ASSERT ( tab_str.getLength() == 8 );
  CPPUNIT_ASSERT ( tab_str.expandTabs() == "one line" );
  CPPUNIT_ASSERT ( tab_str.expandTabs().getLength() == 8 );
  CPPUNIT_ASSERT ( tab_str.expandTabs(4) == "one line" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(2) == "one line" );
  tab_str = "one\ttwo";
  CPPUNIT_ASSERT ( tab_str.getLength() == 7 );
  CPPUNIT_ASSERT ( tab_str.expandTabs() == "one     two" );
  CPPUNIT_ASSERT ( tab_str.expandTabs().getLength() == 11 );
  CPPUNIT_ASSERT ( tab_str.expandTabs(4) == "one two" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(2) == "one two" );
  tab_str = "one\t\btwo";
  CPPUNIT_ASSERT ( tab_str.getLength() == 8 );
  CPPUNIT_ASSERT ( tab_str.expandTabs() == "one     \btwo" );
  CPPUNIT_ASSERT ( tab_str.expandTabs().getLength() == 12 );
  CPPUNIT_ASSERT ( tab_str.expandTabs(4) == "one \btwo" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(2) == "one \btwo" );
  tab_str = "1\t2\t2";
  CPPUNIT_ASSERT ( tab_str.getLength() == 5 );
  CPPUNIT_ASSERT ( tab_str.expandTabs() == "1       2       2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs().getLength() == 17 );
  CPPUNIT_ASSERT ( tab_str.expandTabs(4) == "1   2   2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(2) == "1 2 2" );
  tab_str = "12\t22\t2";
  CPPUNIT_ASSERT ( tab_str.getLength() == 7 );
  CPPUNIT_ASSERT ( tab_str.expandTabs() == "12      22      2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs().getLength() == 17 );
  CPPUNIT_ASSERT ( tab_str.expandTabs(4) == "12  22  2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(2) == "12  22  2" );
  tab_str = "123\t222\t2";
  CPPUNIT_ASSERT ( tab_str.getLength() == 9 );
  CPPUNIT_ASSERT ( tab_str.expandTabs() == "123     222     2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs().getLength() == 17 );
  CPPUNIT_ASSERT ( tab_str.expandTabs(4) == "123 222 2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(2) == "123 222 2" );
  tab_str = "1234\t2222\t2";
  CPPUNIT_ASSERT ( tab_str.getLength() == 11 );
  CPPUNIT_ASSERT ( tab_str.expandTabs() == "1234    2222    2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs().getLength() == 17 );
  CPPUNIT_ASSERT ( tab_str.expandTabs(4) == "1234    2222    2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(2) == "1234  2222  2" );
  tab_str = "12345\t22222\t2";
  CPPUNIT_ASSERT ( tab_str.getLength() == 13 );
  CPPUNIT_ASSERT ( tab_str.expandTabs() == "12345   22222   2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs().getLength() == 17 );
  CPPUNIT_ASSERT ( tab_str.expandTabs(4) == "12345   22222   2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(2) == "12345 22222 2" );
  tab_str = "123456\t222222\t2";
  CPPUNIT_ASSERT ( tab_str.getLength() == 15 );
  CPPUNIT_ASSERT ( tab_str.expandTabs() == "123456  222222  2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs().getLength() == 17 );
  CPPUNIT_ASSERT ( tab_str.expandTabs(4) == "123456  222222  2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(2) == "123456  222222  2" );
  tab_str = "1234567\t2222222\t2";
  CPPUNIT_ASSERT ( tab_str.getLength() == 17 );
  CPPUNIT_ASSERT ( tab_str.expandTabs() == "1234567 2222222 2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs().getLength() == 17 );
  CPPUNIT_ASSERT ( tab_str.expandTabs(4) == "1234567 2222222 2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(2) == "1234567 2222222 2" );
  tab_str = "12345678\t22222222\t2";
  CPPUNIT_ASSERT ( tab_str.getLength() == 19 );
  CPPUNIT_ASSERT ( tab_str.expandTabs()
                   == "12345678        22222222        2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs().getLength() == 33 );
  CPPUNIT_ASSERT ( tab_str.expandTabs(4)
                   == "12345678    22222222    2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(2)
                   == "12345678  22222222  2" );
  tab_str = "12345678\t2";
  CPPUNIT_ASSERT ( tab_str.getLength() == 10 );
  CPPUNIT_ASSERT ( tab_str.expandTabs() == "12345678        2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs().getLength() == 17 );
  CPPUNIT_ASSERT ( tab_str.expandTabs(4) == "12345678    2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(2) == "12345678  2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(0) == "12345678\t2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(-1) == "12345678\t2" );

  // C0 control codes (0x01 - 0x1f) - without null (0x00)
  finalcut::FString c0(0x1f);

  for (auto i = 0; i < 0x1f; i++)
    c0[i] = i + 1;

  CPPUNIT_ASSERT ( c0.getLength() == 31 );
  CPPUNIT_ASSERT ( c0.replaceControlCodes()
                   == "␁␂␃␄␅␆␇␈␉␊␋␌␍␎␏␐␑␒␓␔␕␖␗␘␙␚␛␜␝␞␟" );
  CPPUNIT_ASSERT ( c0.replaceControlCodes().getLength() == 31 );

  c0 = "t\b\bTes\177Tt";
  CPPUNIT_ASSERT ( c0.replaceControlCodes() == "t␈␈Tes␡Tt" );

  c0 = "\U0010ffff\r\n"; // non printable
  CPPUNIT_ASSERT ( c0.replaceControlCodes() == " ␍␊" );

  // C1 control codes (0x80 - 0x9f)
  // Used as print characters in some character sets
  finalcut::FString c1(0x20);

  for (auto i = 0; i <= 0x1f; i++)
    c1[i] = i + 0x80;

  CPPUNIT_ASSERT ( c1.replaceControlCodes() == finalcut::FString(32, L' ') );

  finalcut::FString ctrl_code_str = "\x01\x02";
  CPPUNIT_ASSERT ( ctrl_code_str.replaceControlCodes() == "␁␂" );
  CPPUNIT_ASSERT ( ! ctrl_code_str.replaceControlCodes().isEmpty() );
  CPPUNIT_ASSERT ( ctrl_code_str.replaceControlCodes().getLength() == 2 );

  ctrl_code_str = L"abc\x7f";
  CPPUNIT_ASSERT ( ctrl_code_str.replaceControlCodes() == "abc␡" );
  CPPUNIT_ASSERT ( ! ctrl_code_str.replaceControlCodes().isEmpty() );
  CPPUNIT_ASSERT ( ctrl_code_str.replaceControlCodes().getLength() == 4 );

  ctrl_code_str = L"test\200";
  CPPUNIT_ASSERT ( ctrl_code_str.replaceControlCodes() == "test " );
  CPPUNIT_ASSERT ( ! ctrl_code_str.replaceControlCodes().isEmpty() );
  CPPUNIT_ASSERT ( ctrl_code_str.replaceControlCodes().getLength() == 5 );

  ctrl_code_str = L"normal text";
  CPPUNIT_ASSERT ( ctrl_code_str.replaceControlCodes() == "normal text" );
  CPPUNIT_ASSERT ( ! ctrl_code_str.replaceControlCodes().isEmpty() );
  CPPUNIT_ASSERT ( ctrl_code_str.replaceControlCodes().getLength() == 11 );

  ctrl_code_str = L"";
  CPPUNIT_ASSERT ( ctrl_code_str.replaceControlCodes() == "" );
  CPPUNIT_ASSERT ( ctrl_code_str.replaceControlCodes().isEmpty() );
  CPPUNIT_ASSERT ( ctrl_code_str.replaceControlCodes().getLength() == 0 );
}

//----------------------------------------------------------------------
void FStringTest::caseCompareTest()
{
  auto s1 = finalcut::FString(L"Appel");
  auto s2 = finalcut::FString(L"apartment");
  auto s3 = finalcut::FString(L"ball");

  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare(s1, s1) == 0 );  // same object
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare(L"ApP", s1) < 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare(s1, L"App") > 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare(s1, s2) > 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare(s1, s3) < 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare(s2, s1) < 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare(s3, s1) > 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare("HOUSE", "house") == 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare("hOuSe", "HoUsE") == 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare("house", "houseparty") < 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare("houseparty", "house") > 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare("small", "large") > 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare("large", "small") < 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare("paragraph", "Paragraph") == 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare("paragrapH", "parAgRaph") == 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare("paragraph", "paraLyzed") < 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare("paraLyzed", "paragraph") > 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare("para", "paragraph") < 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare("paragraph", "para") > 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare(L"\311mile", L"\351mile") < 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare(L"\351mile", L"\311mile") > 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare(L"\303\266zg\303\274r", L"\303\226ZG\303\234R") > 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare(L"\303\226ZG\303\234R", L"\303\266zg\303\274r") < 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare(L"turkish", L"TURK\304\260SH") < 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare(L"TURK\304\260SH", L"turkish") > 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare(L"ONE 🃏", L"one 🃏") == 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare(L"ONE 🃏🃏", L"one 🃏") > 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare(L"ONE 🃏", L"one 🃏🃏") < 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare(L"🔥 Fire", L"🔥 fire") == 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare(L"🔥🔥 Fire", L"🔥 fire") > 0 );
  CPPUNIT_ASSERT ( finalcut::FStringCaseCompare(L"🔥 Fire", L"🔥🔥 fire") < 0 );
}

//----------------------------------------------------------------------
void FStringTest::swapTest()
{
  finalcut::FString s1 ("first string");
  CPPUNIT_ASSERT ( s1.getLength() == 12 );
  CPPUNIT_ASSERT ( ! s1.isEmpty() );
  CPPUNIT_ASSERT_CSTRING ( s1.c_str(), "first string" );
  CPPUNIT_ASSERT_WCSTRING ( s1.wc_str(), L"first string" );
  CPPUNIT_ASSERT ( s1 == L"first string" );

  finalcut::FString s2 ("second string");
  s1.swap(s2);
  CPPUNIT_ASSERT ( s1.getLength() == 13 );
  CPPUNIT_ASSERT ( ! s1.isEmpty() );
  CPPUNIT_ASSERT_CSTRING ( s1.c_str(), "second string" );
  CPPUNIT_ASSERT_WCSTRING ( s1.wc_str(), L"second string" );
  CPPUNIT_ASSERT ( s1 == L"second string" );
  CPPUNIT_ASSERT ( s2.getLength() == 12 );
  CPPUNIT_ASSERT ( ! s2.isEmpty() );
  CPPUNIT_ASSERT_CSTRING ( s2.c_str(), "first string" );
  CPPUNIT_ASSERT_WCSTRING ( s2.wc_str(), L"first string" );
  CPPUNIT_ASSERT ( s2 == L"first string" );

  s2.swap (s1);
  CPPUNIT_ASSERT ( s1.getLength() == 12 );
  CPPUNIT_ASSERT ( ! s1.isEmpty() );
  CPPUNIT_ASSERT_CSTRING ( s1.c_str(), "first string" );
  CPPUNIT_ASSERT_WCSTRING ( s1.wc_str(), L"first string" );
  CPPUNIT_ASSERT ( s1 == L"first string" );
  CPPUNIT_ASSERT ( s2.getLength() == 13 );
  CPPUNIT_ASSERT ( ! s2.isEmpty() );
  CPPUNIT_ASSERT_CSTRING ( s2.c_str(), "second string" );
  CPPUNIT_ASSERT_WCSTRING ( s2.wc_str(), L"second string" );
  CPPUNIT_ASSERT ( s2 == L"second string" );
}

//----------------------------------------------------------------------
void FStringTest::hashTest()
{
  std::wstring ws = L"Coding for weeks can save hours of planning.";
  finalcut::FString fs("Coding for weeks can save hours of planning.");
  CPPUNIT_ASSERT ( std::hash<std::wstring>{}(ws) == std::hash<finalcut::FString>{}(fs) );
  fs.setNumber(512);
  CPPUNIT_ASSERT ( std::hash<std::wstring>{}(ws) != std::hash<finalcut::FString>{}(fs) );
  ws.assign(L"256");
  CPPUNIT_ASSERT ( std::hash<std::wstring>{}(ws) != std::hash<finalcut::FString>{}(fs) );
  ws.assign(L"512");
  CPPUNIT_ASSERT ( std::hash<std::wstring>{}(ws) == std::hash<finalcut::FString>{}(fs) );
  fs.setString(L"FINAL CUT");
  ws.assign(L"FINAL CUT");
  CPPUNIT_ASSERT ( std::hash<std::wstring>{}(ws) == std::hash<finalcut::FString>{}(fs) );
  fs << " widget toolkit";
  CPPUNIT_ASSERT ( std::hash<std::wstring>{}(ws) != std::hash<finalcut::FString>{}(fs) );
  ws.append(L" widget toolkit");
  CPPUNIT_ASSERT ( std::hash<std::wstring>{}(ws) == std::hash<finalcut::FString>{}(fs) );
  std::wstring lorem_ipsum =
      L"Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam "
      L"nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam "
      L"erat, sed diam voluptua. At vero eos et accusam et justo duo dolores "
      L"et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est "
      L"Lorem ipsum dolor sit amet.";
  CPPUNIT_ASSERT ( std::hash<std::wstring>{}(lorem_ipsum) != std::hash<finalcut::FString>{}(fs) );
  fs.setString(lorem_ipsum);
  CPPUNIT_ASSERT ( std::hash<std::wstring>{}(lorem_ipsum) == std::hash<finalcut::FString>{}(fs) );
  ws.assign(L"✂️ 𝓕𝒾𝓃𝒶𝓁 𝓒𝓊𝓉");
  fs.setString("✂️ 𝓕𝒾𝓃𝒶𝓁 𝓒𝓊𝓉");
  CPPUNIT_ASSERT ( std::hash<std::wstring>{}(ws) == std::hash<finalcut::FString>{}(fs) );
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FStringTest);

// The general unit test main part
#include <main-test.inc>
