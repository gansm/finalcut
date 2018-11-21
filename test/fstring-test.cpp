/***********************************************************************
* fstring-test.cpp - FString unit tests                                *
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

#include <langinfo.h>
#include <unistd.h>
#define __STDC_LIMIT_MACROS
#include <stdint.h>

#include <clocale>
#include <iomanip>
#include <string>
#include <vector>

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <final/final.h>

//----------------------------------------------------------------------
// class FStringTest
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FStringTest : public CPPUNIT_NS::TestFixture
{
  public:
    FStringTest()
      : s(0)
    { }

    void setUp();
    void tearDown();

  protected:
    void classNameTest();
    void noArgumentTest();
    void initLengthTest();
    void copyConstructorTest();
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

  private:
    finalcut::FString* s;

    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FStringTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (noArgumentTest);
    CPPUNIT_TEST (initLengthTest);
    CPPUNIT_TEST (copyConstructorTest);
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

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};
#pragma pack(pop)

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
  const char* const classname = str.getClassName();
  CPPUNIT_ASSERT ( std::strcmp(classname, "FString") == 0 );
}

//----------------------------------------------------------------------
void FStringTest::noArgumentTest()
{
  finalcut::FString empty;
  CPPUNIT_ASSERT ( empty.isNull() );
  CPPUNIT_ASSERT ( empty.isEmpty() );
  CPPUNIT_ASSERT ( empty.getLength() == 0 );
  CPPUNIT_ASSERT ( empty.capacity() == 0 );
  CPPUNIT_ASSERT ( empty.getUTF8length() == 0 );
  CPPUNIT_ASSERT ( empty.wc_str() == 0 );
  CPPUNIT_ASSERT ( empty.c_str() == 0 );
  CPPUNIT_ASSERT_EQUAL ( empty.toString(), std::string() );
  CPPUNIT_ASSERT ( strlen(finalcut::FString(99).c_str()) == 0 );
  CPPUNIT_ASSERT ( wcslen(finalcut::FString(99).wc_str()) == 0 );
  CPPUNIT_ASSERT ( strlen(finalcut::FString("").c_str()) == 0 );
  CPPUNIT_ASSERT ( wcslen(finalcut::FString("").wc_str()) == 0 );

  char* cstr = empty.c_str();
  CPPUNIT_ASSERT ( cstr == 0 );
  wchar_t* wcstr = empty.wc_str();
  CPPUNIT_ASSERT ( wcstr == 0 );
  std::string str = empty.toString();
  CPPUNIT_ASSERT ( str.length() == 0 );
  CPPUNIT_ASSERT ( str.size() == 0 );
  CPPUNIT_ASSERT ( str.empty() );
  const finalcut::FString fstr = str;
  CPPUNIT_ASSERT ( fstr.isNull() );
  CPPUNIT_ASSERT ( fstr.isEmpty() );
  CPPUNIT_ASSERT ( fstr.capacity() == 0 );

  cstr = 0;
  CPPUNIT_ASSERT ( empty == cstr );
  CPPUNIT_ASSERT ( finalcut::FString(std::string()).isEmpty() );
  CPPUNIT_ASSERT ( finalcut::FString(char(0)).isEmpty() );
  wcstr = 0;
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
  CPPUNIT_ASSERT ( empty.isNull() );
}

//----------------------------------------------------------------------
void FStringTest::initLengthTest()
{
  const finalcut::FString s1(0);
  CPPUNIT_ASSERT ( s1.getLength() == 0 );
  CPPUNIT_ASSERT ( s1.capacity() == 0 );
  CPPUNIT_ASSERT ( s1.isNull() );
  CPPUNIT_ASSERT ( s1.isEmpty() );

  const int         x1 = 10;
  const std::size_t x2 = 10;
  const finalcut::FString s2(x1);
  CPPUNIT_ASSERT ( s2.getLength() == 10 );
  CPPUNIT_ASSERT ( s2.capacity() == 25 );
  CPPUNIT_ASSERT ( ! s2.isNull() );
  CPPUNIT_ASSERT ( s2.isEmpty() );

  const finalcut::FString s3(x2);
  CPPUNIT_ASSERT ( s3.getLength() == 10 );
  CPPUNIT_ASSERT ( s3.capacity() == 25 );
  CPPUNIT_ASSERT ( ! s3.isNull() );
  CPPUNIT_ASSERT ( s3.isEmpty() );

  const finalcut::FString s4(0, L'-');
  CPPUNIT_ASSERT ( s4.getLength() == 0 );
  CPPUNIT_ASSERT ( s4.capacity() == 0 );
  CPPUNIT_ASSERT ( s4.isNull() );
  CPPUNIT_ASSERT ( s4.isEmpty() );

  const finalcut::FString s5(0, '-');
  CPPUNIT_ASSERT ( s5.getLength() == 0 );
  CPPUNIT_ASSERT ( s5.capacity() == 0 );
  CPPUNIT_ASSERT ( s5.isNull() );
  CPPUNIT_ASSERT ( s5.isEmpty() );

  const finalcut::FString s6(0, char(0));
  CPPUNIT_ASSERT ( s6.getLength() == 0 );
  CPPUNIT_ASSERT ( s6.capacity() == 0 );
  CPPUNIT_ASSERT ( s6.isNull() );
  CPPUNIT_ASSERT ( s6.isEmpty() );

  const finalcut::FString s7(x1, '-');
  CPPUNIT_ASSERT ( s7.getLength() == 10 );
  CPPUNIT_ASSERT ( ! s7.isNull() );
  CPPUNIT_ASSERT ( ! s7.isEmpty() );

  const finalcut::FString s8(x2, '-');
  CPPUNIT_ASSERT ( s8.getLength() == 10 );
  CPPUNIT_ASSERT ( s8.capacity() == 25 );
  CPPUNIT_ASSERT ( ! s8.isNull() );
  CPPUNIT_ASSERT ( ! s8.isEmpty() );

  const finalcut::FString s9(x1, L'-');
  CPPUNIT_ASSERT ( s9.getLength() == 10 );
  CPPUNIT_ASSERT ( s9.capacity() == 25 );
  CPPUNIT_ASSERT ( ! s9.isNull() );
  CPPUNIT_ASSERT ( ! s9.isEmpty() );

  const finalcut::FString s10(x2, L'-');
  CPPUNIT_ASSERT ( s10.getLength() == 10 );
  CPPUNIT_ASSERT ( s10.capacity() == 25 );
  CPPUNIT_ASSERT ( ! s10.isNull() );
  CPPUNIT_ASSERT ( ! s10.isEmpty() );

  const finalcut::FString s11(x2, wchar_t(0));
  CPPUNIT_ASSERT ( s11.getLength() == 10 );
  CPPUNIT_ASSERT ( s11.capacity() == 25 );
  CPPUNIT_ASSERT ( ! s11.isNull() );
  CPPUNIT_ASSERT ( s11.isEmpty() );
}

//----------------------------------------------------------------------
void FStringTest::copyConstructorTest()
{
  const finalcut::FString s1("abc");
  const finalcut::FString s2(s1);
  CPPUNIT_ASSERT ( s2 == L"abc" );
  CPPUNIT_ASSERT ( s2.getLength() == 3 );
  CPPUNIT_ASSERT ( s2.capacity() == 18 );
}

//----------------------------------------------------------------------
void FStringTest::assignmentTest()
{
  finalcut::FString s1;
  s1 = static_cast<finalcut::FString>(0);
  CPPUNIT_ASSERT ( ! s1 );
  CPPUNIT_ASSERT ( s1.isNull() );
  CPPUNIT_ASSERT ( s1.isEmpty() );

  s1 = std::wstring();
  CPPUNIT_ASSERT ( ! s1 );
  CPPUNIT_ASSERT ( s1.isNull() );
  CPPUNIT_ASSERT ( s1.isEmpty() );

  s1 = std::string();
  CPPUNIT_ASSERT ( ! s1 );
  CPPUNIT_ASSERT ( s1.isNull() );
  CPPUNIT_ASSERT ( s1.isEmpty() );

  s1 = static_cast<wchar_t*>(0);
  CPPUNIT_ASSERT ( ! s1 );
  CPPUNIT_ASSERT ( s1.isNull() );
  CPPUNIT_ASSERT ( s1.isEmpty() );

  s1 = static_cast<char*>(0);
  CPPUNIT_ASSERT ( ! s1 );
  CPPUNIT_ASSERT ( s1.isNull() );
  CPPUNIT_ASSERT ( s1.isEmpty() );

  s1 = wchar_t(0);
  CPPUNIT_ASSERT ( ! s1 );
  CPPUNIT_ASSERT ( s1.isNull() );
  CPPUNIT_ASSERT ( s1.isEmpty() );

  s1 = char(0);
  CPPUNIT_ASSERT ( ! s1 );
  CPPUNIT_ASSERT ( s1.isNull() );
  CPPUNIT_ASSERT ( s1.isEmpty() );

  const finalcut::FString s2("abc");
  s1 = s2;
  CPPUNIT_ASSERT ( s1 );
  CPPUNIT_ASSERT ( s1 == L"abc" );
  CPPUNIT_ASSERT ( s1.getLength() == 3 );
  CPPUNIT_ASSERT ( s1.capacity() == 18 );

  const std::wstring s3(L"def");
  s1 = s3;
  CPPUNIT_ASSERT ( s1 );
  CPPUNIT_ASSERT ( s1 == L"def" );
  CPPUNIT_ASSERT ( s1.getLength() == 3 );
  CPPUNIT_ASSERT ( s1.capacity() == 18 );

  const std::string s4("ghi");
  s1 = s4;
  CPPUNIT_ASSERT ( s1 );
  CPPUNIT_ASSERT ( s1 == L"ghi" );
  CPPUNIT_ASSERT ( s1.getLength() == 3 );
  CPPUNIT_ASSERT ( s1.capacity() == 18 );

  const wchar_t s5[] = L"abc";
  s1 = s5;
  CPPUNIT_ASSERT ( s1 );
  CPPUNIT_ASSERT ( s1 == L"abc" );
  CPPUNIT_ASSERT ( s1.getLength() == 3 );
  CPPUNIT_ASSERT ( s1.capacity() == 18 );

  const char s6[] = "def";
  s1 = s6;
  CPPUNIT_ASSERT ( s1 );
  CPPUNIT_ASSERT ( s1 == L"def" );
  CPPUNIT_ASSERT ( s1.getLength() == 3 );
  CPPUNIT_ASSERT ( s1.capacity() == 18 );

  const wchar_t s7 = L'#';
  s1 = s7;
  CPPUNIT_ASSERT ( s1 );
  CPPUNIT_ASSERT ( s1 == L"#" );
  CPPUNIT_ASSERT ( s1.getLength() == 1 );
  CPPUNIT_ASSERT ( s1.capacity() == 18 );

  const char s8 = '%';
  s1 = s8;
  CPPUNIT_ASSERT ( s1 );
  CPPUNIT_ASSERT ( s1 == L"%" );
  CPPUNIT_ASSERT ( s1.getLength() == 1 );
  CPPUNIT_ASSERT ( s1.capacity() == 18 );

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
  CPPUNIT_ASSERT ( s1 );

  s1.setString(L"");
  CPPUNIT_ASSERT ( s1 );

  s1.setString("");
  CPPUNIT_ASSERT ( s1 == "" );

  s1.setString(L"");
  CPPUNIT_ASSERT ( s1 == L"" );

  s1.setString("");
  CPPUNIT_ASSERT ( s1.isEmpty() );

  s1.setString(L"");
  CPPUNIT_ASSERT ( s1.isEmpty() );

  s1.setString("");
  CPPUNIT_ASSERT ( ! s1.isNull() );

  s1.setString(L"");
  CPPUNIT_ASSERT ( ! s1.isNull() );

  const wchar_t* wc = 0;
  s1.setString(wc);
  CPPUNIT_ASSERT ( s1.isEmpty() );
  CPPUNIT_ASSERT ( s1.isNull() );
  CPPUNIT_ASSERT ( ! s1 );

  const char* c = 0;
  s1.setString(c);
  CPPUNIT_ASSERT ( s1.isEmpty() );
  CPPUNIT_ASSERT ( s1.isNull() );
  CPPUNIT_ASSERT ( ! s1 );
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
  CPPUNIT_ASSERT ( s1.isNull() );
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
  s1 += std::string("abc");
  CPPUNIT_ASSERT ( s1 == L"abc" );
  s1 += std::string("def");
  CPPUNIT_ASSERT ( s1 == L"abcdef" );

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
  // finalcut::FString member operator
  const finalcut::FString s1("abc");
  CPPUNIT_ASSERT ( s1 + finalcut::FString("def") == L"abcdef" );
  CPPUNIT_ASSERT ( s1 + std::wstring(L"def") == L"abcdef" );
  CPPUNIT_ASSERT ( s1 + const_cast<wchar_t*>(L"def") == L"abcdef" );
  CPPUNIT_ASSERT ( s1 + std::string("def") == L"abcdef" );
  CPPUNIT_ASSERT ( s1 + const_cast<char*>("def") == L"abcdef" );
  CPPUNIT_ASSERT ( s1 + wchar_t(L'd') == L"abcd" );
  CPPUNIT_ASSERT ( s1 + char('d') == L"abcd" );

  // finalcut::FString non-member operator
  finalcut::FString s2("abc");
  CPPUNIT_ASSERT ( s2 + finalcut::FString("def") == L"abcdef" );
  CPPUNIT_ASSERT ( s2 + std::wstring(L"def") == L"abcdef" );
  CPPUNIT_ASSERT ( s2 + const_cast<wchar_t*>(L"def") == L"abcdef" );
  CPPUNIT_ASSERT ( s2 + std::string("def") == L"abcdef" );
  CPPUNIT_ASSERT ( s2 + const_cast<char*>("def") == L"abcdef" );
  CPPUNIT_ASSERT ( s2 + wchar_t(L'd') == L"abcd" );
  CPPUNIT_ASSERT ( s2 + char('d') == L"abcd" );

  const std::wstring& s3 = L"abc";
  CPPUNIT_ASSERT ( s3 + finalcut::FString("def") == L"abcdef" );

  const wchar_t s4[] = L"abc";
  CPPUNIT_ASSERT ( s4 + finalcut::FString("def") == L"abcdef" );

  const std::string& s5 = "abc";
  CPPUNIT_ASSERT ( s5 + finalcut::FString("def") == L"abcdef" );

  const char s6[] = "abc";
  CPPUNIT_ASSERT ( s6 + finalcut::FString("def") == L"abcdef" );

  const wchar_t c1 = L'a';
  CPPUNIT_ASSERT ( c1 + s3 == L"aabc" );
  CPPUNIT_ASSERT ( c1 + finalcut::FString("def") == L"adef" );

  const char c2 = 'a';
  CPPUNIT_ASSERT ( c2 + s5 == "aabc" );
  CPPUNIT_ASSERT ( c2 + finalcut::FString("def") == L"adef" );
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
  CPPUNIT_ASSERT ( s1 == s2 );

  // std::wstring -> finalcut::FString -> std::wstring
  const std::wstring ws1 = L"wide string";
  fs = ws1;
  std::wstring ws2 = fs.wc_str();
  CPPUNIT_ASSERT ( ws1 == ws2 );

  const finalcut::FString one_char('a');
  const char ch = 'a';
  CPPUNIT_ASSERT ( one_char == ch );
  CPPUNIT_ASSERT ( ch == one_char.c_str()[0] );
  CPPUNIT_ASSERT ( one_char.getLength() == 1 );
  CPPUNIT_ASSERT ( one_char.capacity() == 16 );

  const wchar_t wch = L'a';
  CPPUNIT_ASSERT ( one_char == wch );
  CPPUNIT_ASSERT ( wch == one_char.wc_str()[0] );

  const finalcut::FString str(L"abc");
  const finalcut::FString str2(L"abc");
  CPPUNIT_ASSERT ( str == str2 );

  const char cstr[] = "abc";
  CPPUNIT_ASSERT ( str == cstr );
  CPPUNIT_ASSERT ( str.getLength() == 3 );
  CPPUNIT_ASSERT ( str.getUTF8length() == 3 );
  CPPUNIT_ASSERT ( str.capacity() == 18 );
  CPPUNIT_ASSERT ( strncmp(cstr, str.c_str(), 3) == 0 );

  const wchar_t wcstr[] = L"abc";
  CPPUNIT_ASSERT ( str == wcstr );
  CPPUNIT_ASSERT ( wcsncmp(wcstr, str.wc_str(), 3) == 0 );

  const std::string st = "abc";
  CPPUNIT_ASSERT ( str == st );

  const std::wstring wst = L"abc";
  CPPUNIT_ASSERT ( str == wst );

  const finalcut::FString null_str1;
  const finalcut::FString null_str2;
  CPPUNIT_ASSERT ( ! (str == null_str2) );
  CPPUNIT_ASSERT ( ! (null_str1 == str) );
  CPPUNIT_ASSERT ( null_str1 == null_str2 );

  CPPUNIT_ASSERT ( s->c_str()[0] == 'c');
  CPPUNIT_ASSERT ( s->getLength() == 1 );
  CPPUNIT_ASSERT ( s->capacity() == 16 );
}

//----------------------------------------------------------------------
void FStringTest::notEqualTest()
{
  const finalcut::FString one_char('@');
  const char ch = '!';
  CPPUNIT_ASSERT ( one_char != ch );
  CPPUNIT_ASSERT ( ch != one_char.c_str()[0] );
  CPPUNIT_ASSERT ( one_char.getLength() == 1 );
  CPPUNIT_ASSERT ( one_char.capacity() == 16 );

  const wchar_t wch = L'_';
  CPPUNIT_ASSERT ( one_char != wch );
  CPPUNIT_ASSERT ( wch != one_char.wc_str()[0] );

  const finalcut::FString s1 = L"ABC";  // latin letter
  const finalcut::FString s2 = L"АВС";  // cyrillic letters
  CPPUNIT_ASSERT ( s1 != s2 );

  const char cstr[] = "abc";
  CPPUNIT_ASSERT ( s1 != cstr );
  CPPUNIT_ASSERT ( s1.getLength() == 3 );
  CPPUNIT_ASSERT ( strlen(s1.c_str()) == 3 );
  CPPUNIT_ASSERT ( s2.getLength() == 3 );
  CPPUNIT_ASSERT ( strlen(s2.c_str()) == 6 );
  CPPUNIT_ASSERT ( s1.getUTF8length() == 3 );
  CPPUNIT_ASSERT ( s2.getUTF8length() == 3 );
  CPPUNIT_ASSERT ( s1.capacity() == 18 );
  CPPUNIT_ASSERT ( s2.capacity() == 18 );
  CPPUNIT_ASSERT ( strncmp(cstr, s1.c_str(), 3) != 0 );

  const wchar_t wcstr[] = L"abc";
  CPPUNIT_ASSERT ( s1 != wcstr );
  CPPUNIT_ASSERT ( wcsncmp(wcstr, s1.wc_str(), 3) != 0 );

  const std::string st = "abc";
  CPPUNIT_ASSERT ( s1 != st );

  const std::wstring wst = L"abc";
  CPPUNIT_ASSERT ( s1 != wst );

  const finalcut::FString null_str1;
  const finalcut::FString null_str2;
  CPPUNIT_ASSERT ( s1 != null_str2 );
  CPPUNIT_ASSERT ( null_str1 != s1 );
  CPPUNIT_ASSERT ( ! (null_str1 != null_str2) );

  CPPUNIT_ASSERT ( s->c_str()[0] != 's');
}

//----------------------------------------------------------------------
void FStringTest::lessEqualTest()
{
  const finalcut::FString one_char('x');
  const char ch = 'z';
  CPPUNIT_ASSERT ( one_char <= ch );

  const wchar_t wch = L'z';
  CPPUNIT_ASSERT ( one_char <= wch );

  const finalcut::FString s1 = L"xyz";
  const finalcut::FString s2 = L"xyz";
  const finalcut::FString s3 = L"xzz";
  CPPUNIT_ASSERT ( s1 <= s2 && s1 == s2 );
  CPPUNIT_ASSERT ( s1 <= s3 && s1 != s3 );

  const char cstr1[] = "xyz";
  const char cstr2[] = "xzz";
  CPPUNIT_ASSERT ( s1 <= cstr1 && s1 == cstr1 );
  CPPUNIT_ASSERT ( s1 <= cstr2 && s1 != cstr2 );

  const wchar_t wcstr1[] = L"xyz";
  const wchar_t wcstr2[] = L"xzz";
  CPPUNIT_ASSERT ( s1 <= wcstr1 && s1 == wcstr1 );
  CPPUNIT_ASSERT ( s1 <= wcstr2 && s1 != wcstr2 );

  const std::string st1 = "xyz";
  const std::string st2 = "xzz";
  CPPUNIT_ASSERT ( s1 <= st1 && s1 == st1 );
  CPPUNIT_ASSERT ( s1 <= st2 && s1 != st2 );

  const std::wstring wst1 = L"xyz";
  const std::wstring wst2 = L"xzz";
  CPPUNIT_ASSERT ( s1 <= wst1 && s1 == wst1 );
  CPPUNIT_ASSERT ( s1 <= wst2 && s1 != wst2 );

  const finalcut::FString null_str1;
  const finalcut::FString null_str2;
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
  const char ch = 'z';
  CPPUNIT_ASSERT ( one_char < ch );

  const wchar_t wch = L'z';
  CPPUNIT_ASSERT ( one_char < wch );

  const finalcut::FString s1 = L"xyz";
  const finalcut::FString s2 = L"xzz";
  CPPUNIT_ASSERT ( s1 < s2 );

  const char cstr[] = "xzz";
  CPPUNIT_ASSERT ( s1 < cstr );

  const wchar_t wcstr[] = L"xzz";
  CPPUNIT_ASSERT ( s1 < wcstr );

  const std::string st = "xzz";
  CPPUNIT_ASSERT ( s1 < st  );

  const std::wstring wst = L"xzz";
  CPPUNIT_ASSERT ( s1 < wst );

  const finalcut::FString null_str1;
  const finalcut::FString null_str2;
  CPPUNIT_ASSERT ( ! (s1 < null_str2) );
  CPPUNIT_ASSERT ( null_str1 < s2 );
  CPPUNIT_ASSERT ( ! (null_str1 < null_str2) );
}

//----------------------------------------------------------------------
void FStringTest::greaterEqualTest()
{
  const finalcut::FString one_char('x');
  const char ch = 'x';
  CPPUNIT_ASSERT ( one_char >= ch );

  const wchar_t wch = L'x';
  CPPUNIT_ASSERT ( one_char >= wch );

  const finalcut::FString s1 = L"xyz";
  const finalcut::FString s2 = L"xyz";
  const finalcut::FString s3 = L"xxz";
  CPPUNIT_ASSERT ( s1 >= s2 && s1 == s2 );
  CPPUNIT_ASSERT ( s1 >= s3 && s1 != s3 );

  const char cstr1[] = "xyz";
  const char cstr2[] = "xxz";
  CPPUNIT_ASSERT ( s1 >= cstr1 && s1 == cstr1 );
  CPPUNIT_ASSERT ( s1 >= cstr2 && s1 != cstr2 );

  const wchar_t wcstr1[] = L"xyz";
  const wchar_t wcstr2[] = L"xxz";
  CPPUNIT_ASSERT ( s1 >= wcstr1 && s1 == wcstr1 );
  CPPUNIT_ASSERT ( s1 >= wcstr2 && s1 != wcstr2 );

  const std::string st1 = "xyz";
  const std::string st2 = "xxz";
  CPPUNIT_ASSERT ( s1 >= st1 && s1 == st1 );
  CPPUNIT_ASSERT ( s1 >= st2 && s1 != st2 );

  const std::wstring wst1 = L"xyz";
  const std::wstring wst2 = L"xxz";
  CPPUNIT_ASSERT ( s1 >= wst1 && s1 == wst1 );
  CPPUNIT_ASSERT ( s1 >= wst2 && s1 != wst2 );

  const finalcut::FString null_str1;
  const finalcut::FString null_str2;
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
  const char ch = 'w';
  CPPUNIT_ASSERT ( one_char > ch );

  const wchar_t wch = L'w';
  CPPUNIT_ASSERT ( one_char > wch );

  const finalcut::FString s1 = L"xyz";
  const finalcut::FString s2 = L"xww";
  CPPUNIT_ASSERT ( s1 > s2 );

  const char cstr[] = "xww";
  CPPUNIT_ASSERT ( s1 > cstr );

  const wchar_t wcstr[] = L"xww";
  CPPUNIT_ASSERT ( s1 > wcstr );

  const std::string st = "xww";
  CPPUNIT_ASSERT ( s1 > st  );

  const std::wstring wst = L"xww";
  CPPUNIT_ASSERT ( s1 > wst );

  const finalcut::FString null_str1;
  const finalcut::FString null_str2;
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

  out.clear();
  out << std::string("ABC");
  CPPUNIT_ASSERT ( out == L"ABC" );

  out.clear();
  out << std::wstring(L"ABC");
  CPPUNIT_ASSERT ( out == L"ABC" );

  out.clear();
  out << const_cast<wchar_t*>(L"ABC");
  CPPUNIT_ASSERT ( out == L"ABC" );

  out.clear();
  out << const_cast<char*>("ABC");
  CPPUNIT_ASSERT ( out == L"ABC" );

  out.clear();
  out << wchar_t(L'A');
  CPPUNIT_ASSERT ( out == L"A" );

  out.clear();
  out << char('A');
  CPPUNIT_ASSERT ( out == L"A" );

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
  out << int(1234567);
  CPPUNIT_ASSERT ( out == L"1234567" );

  out.clear();
  out << int(-1234567);
  CPPUNIT_ASSERT ( out == L"-1234567" );

  out.clear();
  out << uInt(12345678);
  CPPUNIT_ASSERT ( out == L"12345678" );

  out.clear();
  out << long(-34721053343141);
  CPPUNIT_ASSERT ( out == L"-34721053343141" );

  out.clear();
  out << uLong(4670148723459);
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

  out = "abc";
  std::ostringstream ostream;
  ostream << out;
  CPPUNIT_ASSERT ( ostream.str() == "abc" );

  std::wostringstream wostream;
  wostream << out;
  CPPUNIT_ASSERT ( wostream.str() == L"abc" );
}

//----------------------------------------------------------------------
void FStringTest::streamExtractionTest()
{
  finalcut::FString in_1;
  finalcut::FString("ABC") >> in_1;
  CPPUNIT_ASSERT ( in_1 == "ABC" );

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
  CPPUNIT_ASSERT ( in_12 == 2.71828f );

  double in_13;
  finalcut::FString("2.7182818284590452353") >> in_13;
  CPPUNIT_ASSERT ( in_13 == 2.7182818284590452353 );

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
  s[0] = L'A';
  s[1] = L'B';
  s[2] = L'C';
  CPPUNIT_ASSERT ( s[0] == L'A' );
  CPPUNIT_ASSERT ( s[1] == L'B' );
  CPPUNIT_ASSERT ( s[2] == L'C' );
  CPPUNIT_ASSERT ( s == L"ABC" );
}

//----------------------------------------------------------------------
void FStringTest::iteratorTest()
{
  const finalcut::FString& str = "123456789";
  CPPUNIT_ASSERT ( str.front() == L'1' );
  CPPUNIT_ASSERT ( str.back() == L'9' );

  finalcut::FString::iterator iter = str.begin();
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

  const finalcut::FString null_fstring;
  str2.sprintf (null_fstring, 0);
  CPPUNIT_ASSERT ( str2.isNull() );

  const wchar_t* null_wstring = 0;
  str2.sprintf (null_wstring, 0);
  CPPUNIT_ASSERT ( str2.isNull() );

  const char* null_string = 0;
  str2.sprintf (null_string, 0);
  CPPUNIT_ASSERT ( str2.isNull() );

  std::setlocale (LC_NUMERIC, "C");
  finalcut::FString fnum1, fnum2;

#if defined(__LP64__) || defined(_LP64)
  // 64-bit architecture
  fnum1.setFormatedNumber(0xffffffffffffffff, '\'');
  CPPUNIT_ASSERT ( fnum1 == "18'446'744'073'709'551'615" );

  fnum2.setFormatedNumber(-9223372036854775807);
  CPPUNIT_ASSERT ( fnum2 == "-9 223 372 036 854 775 807" );

  fnum2.setFormatedNumber(long(9223372036854775807), '\0');
  CPPUNIT_ASSERT ( fnum2 == "9 223 372 036 854 775 807" );

  fnum2.setFormatedNumber(uLong(9223372036854775807), '\0');
  CPPUNIT_ASSERT ( fnum2 == "9 223 372 036 854 775 807" );
#else
  // 32-bit architecture
  fnum1.setFormatedNumber(0xffffffff, '\'');
  CPPUNIT_ASSERT ( fnum1 == "4'294'967'295" );

  fnum2.setFormatedNumber(-2147483647);
  CPPUNIT_ASSERT ( fnum2 == "-2 147 483 647" );

  fnum2.setFormatedNumber(long(2147483647), '\0');
  CPPUNIT_ASSERT ( fnum2 == "2 147 483 647" );

  fnum2.setFormatedNumber(uLong(2147483647), '\0');
  CPPUNIT_ASSERT ( fnum2 == "2 147 483 647" );
#endif
}

//----------------------------------------------------------------------
void FStringTest::convertToNumberTest()
{
  finalcut::FString str = "-127";
  CPPUNIT_ASSERT ( str.toShort() == -127 );

  str = "255";
  CPPUNIT_ASSERT ( str.toUShort() == 255 );

  str = "-32768";
  CPPUNIT_ASSERT ( str.toInt() == -32768 );

  str = "65535";
  CPPUNIT_ASSERT ( str.toUInt() == 65535 );

  str = "-2147483647";
  CPPUNIT_ASSERT ( str.toLong() == -2147483647 );

  str = "+987654321";
  CPPUNIT_ASSERT ( str.toLong() == 987654321 );

  str = "4294967295";
  CPPUNIT_ASSERT ( str.toULong() == 4294967295 );

  str = "+1234567890";
  CPPUNIT_ASSERT ( str.toULong() == 1234567890 );

  str = "3.14159";
  CPPUNIT_ASSERT ( str.toFloat() == 3.14159f );

  str = "-3.14159";
  CPPUNIT_ASSERT ( str.toFloat() == -3.14159f );

  str = "3.141592653589793238";
  CPPUNIT_ASSERT ( str.toDouble() == 3.141592653589793238 );

  str = "-3.141592653589793238";
  CPPUNIT_ASSERT ( str.toDouble() == -3.141592653589793238 );
}

//----------------------------------------------------------------------
void FStringTest::convertFromNumberTest()
{
  const sInt16  n1 = -1234;
  const uInt16  n2 =  1234;
  const int     n3 = -12345;
  const uInt    n4 =  12345;
  const long    n5 = -12345678;
  const uLong   n6 =  12345678;
  const float   n7 =  1234.56f;
  const double  n8 =  1234.5678;
  const lDouble n9 =  12345.67890L;
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n1) == "-1234" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n2) == "1234" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n3) == "-12345" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n4) == "12345" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n5) == "-12345678" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n6) == "12345678" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n7) == "1234.56" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n8) == "1234.5678" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n9) == "12345.6789" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n7, 0) == "1e+03" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n8, 0) == "1e+03" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n9, 0) == "1e+04" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n7, 100)
                   == "1234.56005859375" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n8, 100)
                   == "1234.567800000000033833202905952930450439453125" );
  CPPUNIT_ASSERT ( finalcut::FString().setNumber(n9, 100)
                   == "12345.67889999999999961488583721802569925785064697265625" );
}

//----------------------------------------------------------------------
void FStringTest::exceptionTest()
{
  CPPUNIT_ASSERT_THROW ( finalcut::FString("abc").toULong()
                       , std::invalid_argument );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("abc")[3]
                       , std::out_of_range );

  CPPUNIT_ASSERT_THROW ( finalcut::FString("abc")[-1]
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
  CPPUNIT_ASSERT ( trim_str1.rtrim() == L"\r\n\t  A string" );
  CPPUNIT_ASSERT ( trim_str1.ltrim() == L"A string \n\t" );
  CPPUNIT_ASSERT ( trim_str1.trim() == L"A string" );

  const finalcut::FString& trim_str2 = L"\n  \n\n";
  CPPUNIT_ASSERT ( trim_str2.rtrim().isEmpty() );
  CPPUNIT_ASSERT ( ! trim_str2.rtrim().isNull() );
  CPPUNIT_ASSERT ( trim_str2.rtrim().getLength() == 0 );
  CPPUNIT_ASSERT ( trim_str2.rtrim().capacity() == 0 );
  CPPUNIT_ASSERT ( trim_str2.ltrim().isEmpty() );
  CPPUNIT_ASSERT ( ! trim_str2.ltrim().isNull() );
  CPPUNIT_ASSERT ( trim_str2.ltrim().getLength() == 0 );
  CPPUNIT_ASSERT ( trim_str2.ltrim().capacity() == 0 );

  const finalcut::FString trim_str3;
  CPPUNIT_ASSERT ( trim_str3.ltrim().isEmpty() );
  CPPUNIT_ASSERT ( trim_str3.ltrim().isEmpty() );
  CPPUNIT_ASSERT ( trim_str3.ltrim().getLength() == 0 );
  CPPUNIT_ASSERT ( trim_str3.ltrim().capacity() == 0 );
  CPPUNIT_ASSERT ( trim_str3.rtrim().isEmpty() );
  CPPUNIT_ASSERT ( trim_str3.rtrim().isEmpty() );
  CPPUNIT_ASSERT ( trim_str3.rtrim().getLength() == 0 );
  CPPUNIT_ASSERT ( trim_str3.rtrim().capacity() == 0 );
  CPPUNIT_ASSERT ( trim_str3.trim().isEmpty() );
  CPPUNIT_ASSERT ( trim_str3.trim().isNull() );
  CPPUNIT_ASSERT ( trim_str3.trim().getLength() == 0 );
  CPPUNIT_ASSERT ( trim_str3.trim().capacity() == 0 );
}

//----------------------------------------------------------------------
void FStringTest::subStringTest()
{
  finalcut::FString str1("Look behind you, a three-headed monkey!");
  CPPUNIT_ASSERT ( str1.left(uInt(11)) == L"Look behind" );
  CPPUNIT_ASSERT ( str1.left(int(11)) == L"Look behind" );
  CPPUNIT_ASSERT ( str1.left(999)
                   == L"Look behind you, a three-headed monkey!" );
  CPPUNIT_ASSERT ( str1.left(-5)
                   == L"Look behind you, a three-headed monkey!" );
  CPPUNIT_ASSERT ( str1.left(0) == L"" );
  CPPUNIT_ASSERT ( str1.left(0).isEmpty() );
  CPPUNIT_ASSERT ( ! str1.left(0).isNull() );
  CPPUNIT_ASSERT ( finalcut::FString().left(5).isNull() );
  CPPUNIT_ASSERT ( ! finalcut::FString("").left(5).isNull() );
  CPPUNIT_ASSERT ( finalcut::FString("").left(5).isEmpty() );

  CPPUNIT_ASSERT ( str1.right(uInt(7)) == L"monkey!" );
  CPPUNIT_ASSERT ( str1.right(int(7)) == L"monkey!" );
  CPPUNIT_ASSERT ( str1.right(999)
                   == L"Look behind you, a three-headed monkey!" );
  CPPUNIT_ASSERT ( str1.right(-5)
                   == L"Look behind you, a three-headed monkey!" );
  CPPUNIT_ASSERT ( str1.right(0) == L"" );
  CPPUNIT_ASSERT ( str1.right(0).isEmpty() );
  CPPUNIT_ASSERT ( ! str1.right(0).isNull() );
  CPPUNIT_ASSERT ( finalcut::FString().right(5).isNull() );
  CPPUNIT_ASSERT ( ! finalcut::FString("").right(5).isNull() );
  CPPUNIT_ASSERT ( finalcut::FString("").right(5).isEmpty() );

  CPPUNIT_ASSERT ( str1.mid(uInt(18), uInt(21))
                   == L"a three-headed monkey" );
  CPPUNIT_ASSERT ( str1.mid(int(18), int(21))
                   == L"a three-headed monkey" );
  CPPUNIT_ASSERT ( str1.mid(1, 999)
                   == L"Look behind you, a three-headed monkey!" );
  CPPUNIT_ASSERT ( str1.mid(5, 0) == L"" );
  CPPUNIT_ASSERT ( str1.mid(-5, 2) == L"" );
  CPPUNIT_ASSERT ( str1.mid(0, 0) == L"" );
  CPPUNIT_ASSERT ( str1.mid(0, 5) == L"Look " );
  CPPUNIT_ASSERT ( str1.mid(0, 0).isEmpty() );
  CPPUNIT_ASSERT ( ! str1.mid(0, 0).isNull() );
  CPPUNIT_ASSERT ( finalcut::FString().mid(5, 0).isNull() );
  CPPUNIT_ASSERT ( ! finalcut::FString("").mid(5, 0).isNull() );
  CPPUNIT_ASSERT ( finalcut::FString("").mid(5, 0).isEmpty() );

  finalcut::FStringList string_parts = str1.split(" ");
  finalcut::FStringList string_list;
  string_list.push_back("Look");
  string_list.push_back("behind");
  string_list.push_back("you,");
  string_list.push_back("a");
  string_list.push_back("three-headed");
  string_list.push_back("monkey!");
  CPPUNIT_ASSERT ( string_parts == string_list );

  string_parts = str1.split(L',');
  string_list.clear();
  string_list.push_back("Look behind you");
  string_list.push_back(" a three-headed monkey!");
  CPPUNIT_ASSERT ( string_parts == string_list );

  string_parts = str1.split(',');
  CPPUNIT_ASSERT ( string_parts == string_list );

  string_parts = finalcut::FString().split(':');
  CPPUNIT_ASSERT ( string_parts.empty() );
  CPPUNIT_ASSERT ( string_parts.size() == 0 );
}

//----------------------------------------------------------------------
void FStringTest::insertTest()
{
  finalcut::FString str1 = "ABC";
  const finalcut::FString str2 = "xyz";
  CPPUNIT_ASSERT ( str1.insert(str2, 0) == "xyzABC" );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.insert(str2, 1) == "AxyzBC" );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.insert(str2, 2) == "ABxyzC" );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.insert(str2, 3) == "ABCxyz" );

  str1 = "ABC";
  const wchar_t str3[] = L"xyz";
  CPPUNIT_ASSERT ( str1.insert(str3, 0) == "xyzABC" );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.insert(str3, 1) == "AxyzBC" );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.insert(str3, 2) == "ABxyzC" );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.insert(str3, 3) == "ABCxyz" );

  str1 = "ABC";
  const char str4[] = "xyz";
  CPPUNIT_ASSERT ( str1.insert(str4, 0) == "xyzABC" );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.insert(str4, 1) == "AxyzBC" );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.insert(str4, 2) == "ABxyzC" );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.insert(str4, 3) == "ABCxyz" );

  str1 = "ABC";
  const wchar_t wc = L'*';
  CPPUNIT_ASSERT ( str1.insert(wc, 0) == "*ABC" );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.insert(wc, 1) == "A*BC" );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.insert(wc, 2) == "AB*C" );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.insert(wc, 3) == "ABC*" );

  str1 = "ABC";
  const char c = '*';
  CPPUNIT_ASSERT ( str1.insert(c, 0) == "*ABC" );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.insert(c, 1) == "A*BC" );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.insert(c, 2) == "AB*C" );
  str1 = "ABC";
  CPPUNIT_ASSERT ( str1.insert(c, 3) == "ABC*" );
}

//----------------------------------------------------------------------
void FStringTest::replaceTest()
{
  const finalcut::FString str = "Look behind you, a three-headed monkey!";
  finalcut::FString s1 = str;
  const finalcut::FString      from1   =  "three";
  const std::wstring from2   = L"three";
  const wchar_t      from3[] = L"three";
  const std::string  from4   =  "three";
  const char         from5[] =  "three";
  const wchar_t      from6   = L',';
  const char         from7   =  ',';
  const finalcut::FString      to1     = L'3';
  const std::wstring to2     = L"3";
  const wchar_t      to3[]   = L"3";
  const std::string  to4     =  "3";
  const char         to5[]   =  "3";
  const wchar_t      to6     =  '3';
  const char         to7     =  '3';

  CPPUNIT_ASSERT ( s1.replace(from1, to1)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from1, to2)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from1, to3)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from1, to4)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from1, to5)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from1, to6)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from1, to7)
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
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from2, to4)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from2, to5)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from2, to6)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from2, to7)
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
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from3, to4)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from3, to5)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from3, to6)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from3, to7)
                   == "Look behind you, a 3-headed monkey!" );
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
  CPPUNIT_ASSERT ( s1.replace(from5, to1)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from5, to2)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from5, to3)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from5, to4)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from5, to5)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from5, to6)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from5, to7)
                   == "Look behind you, a 3-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from6, to1)
                   == "Look behind you3 a three-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from6, to2)
                   == "Look behind you3 a three-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from6, to3)
                   == "Look behind you3 a three-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from6, to4)
                   == "Look behind you3 a three-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from6, to5)
                   == "Look behind you3 a three-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from6, to6)
                   == "Look behind you3 a three-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from6, to7)
                   == "Look behind you3 a three-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from7, to1)
                   == "Look behind you3 a three-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from7, to2)
                   == "Look behind you3 a three-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from7, to3)
                   == "Look behind you3 a three-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from7, to4)
                   == "Look behind you3 a three-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from7, to5)
                   == "Look behind you3 a three-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from7, to6)
                   == "Look behind you3 a three-headed monkey!" );
  s1 = str;
  CPPUNIT_ASSERT ( s1.replace(from7, to7)
                   == "Look behind you3 a three-headed monkey!" );

  s1 = "A big ball and a small ball";
  CPPUNIT_ASSERT ( s1.replace("ball", "globe")
                   == "A big globe and a small globe" );

  s1 = "ABC";
  finalcut::FString empty;
  CPPUNIT_ASSERT ( s1.replace('B', "") == "AC" );
  CPPUNIT_ASSERT ( s1.replace(L'B', "") == "AC" );
  CPPUNIT_ASSERT ( s1.replace(from1, empty) == "ABC" );
  CPPUNIT_ASSERT ( s1.replace(from3, empty) == "ABC" );
  CPPUNIT_ASSERT ( s1.replace(from5, to5) == "ABC" );
  CPPUNIT_ASSERT ( s1.replace(empty, to1) == "ABC" );
  CPPUNIT_ASSERT ( s1.replace(from6, empty) == "ABC"  );

  empty = "";
  CPPUNIT_ASSERT ( s1.replace(from1, empty) == "ABC" );
  CPPUNIT_ASSERT ( s1.replace(from3, empty) == "ABC" );
  CPPUNIT_ASSERT ( s1.replace(from5, to5) == "ABC" );
  CPPUNIT_ASSERT ( s1.replace(empty, to1) == "ABC" );
  CPPUNIT_ASSERT ( s1.replace(from6, empty) == "ABC"  );

  s1.clear();
  CPPUNIT_ASSERT ( s1.replace(from1, to1).isNull() );
  CPPUNIT_ASSERT ( s1.replace(from1, to1).isEmpty() );
  CPPUNIT_ASSERT ( s1.replace(from6, to1).isNull() );
  CPPUNIT_ASSERT ( s1.replace(from6, to1).isEmpty() );

  CPPUNIT_ASSERT ( s1.replace(from5, to5).isNull() );
  CPPUNIT_ASSERT ( s1.replace(from5, to5).isEmpty() );
  CPPUNIT_ASSERT ( s1.replace(from7, to7).isNull() );
  CPPUNIT_ASSERT ( s1.replace(from7, to7).isEmpty() );
}

//----------------------------------------------------------------------
void FStringTest::overwriteTest()
{
  // finalcut::FString
  finalcut::FString str = "abcdefghijklm";
  CPPUNIT_ASSERT ( str.overwrite("+++++++", 3) == "abc+++++++klm" );
  CPPUNIT_ASSERT ( str.overwrite(".............") == "............." );
  CPPUNIT_ASSERT ( str.overwrite(",,,,,,,,,,,,,,,") == ",,,,,,,,,,,,,,," );
  str = "abc";
  CPPUNIT_ASSERT ( str.overwrite("+++++", 99) == "abc+++++" );
  str = "abc";
  CPPUNIT_ASSERT ( str.overwrite("+++", -5) == "+++" );

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
  CPPUNIT_ASSERT ( str.remove(2, 1) == "AB" );
  CPPUNIT_ASSERT ( str.remove(2, 1) == "AB" );
  CPPUNIT_ASSERT ( str.remove(2, 5) == "AB" );
  str = "ABCDE";
  CPPUNIT_ASSERT ( str.remove(2, 99) == "AB" );
  CPPUNIT_ASSERT ( str.remove(99, 1) == "AB" );
}

//----------------------------------------------------------------------
void FStringTest::includesTest()
{
  const finalcut::FString str = "Look behind you, a three-headed monkey!";
  const finalcut::FString empty1;
  const wchar_t* empty2 = 0;
  const char* empty3 = 0;
  const finalcut::FString search1 = "you";
  const finalcut::FString search2 = "me";
  const wchar_t search3[] = L"you";
  const wchar_t search4[] = L"me";
  const char search5[] = "you";
  const char search6[] = "me";
  const wchar_t search7 = L'y';
  const wchar_t search8 = L'&';
  const char search9 = 'y';
  const char search10 = '&';

  CPPUNIT_ASSERT ( ! str.includes(static_cast<finalcut::FString>(0)) );
  CPPUNIT_ASSERT ( ! str.includes(empty1) );
  CPPUNIT_ASSERT ( str.includes(search1) );
  CPPUNIT_ASSERT ( ! str.includes(search2) );
  CPPUNIT_ASSERT ( ! empty1.includes(search1) );

  CPPUNIT_ASSERT ( ! str.includes(static_cast<wchar_t*>(0)) );
  CPPUNIT_ASSERT ( ! str.includes(empty2) );
  CPPUNIT_ASSERT ( str.includes(search3) );
  CPPUNIT_ASSERT ( ! str.includes(search4) );
  CPPUNIT_ASSERT ( ! empty1.includes(search3) );

  CPPUNIT_ASSERT ( ! str.includes(static_cast<char*>(0)) );
  CPPUNIT_ASSERT ( ! str.includes(empty3) );
  CPPUNIT_ASSERT ( str.includes(search5) );
  CPPUNIT_ASSERT ( ! str.includes(search6) );
  CPPUNIT_ASSERT ( ! empty1.includes(search5) );

  CPPUNIT_ASSERT ( ! str.includes(wchar_t(L'\0')) );
  CPPUNIT_ASSERT ( str.includes(search7) );
  CPPUNIT_ASSERT ( ! str.includes(search8) );
  CPPUNIT_ASSERT ( ! empty1.includes(search7) );

  CPPUNIT_ASSERT ( ! str.includes(char('\0')) );
  CPPUNIT_ASSERT ( str.includes(search9) );
  CPPUNIT_ASSERT ( ! str.includes(search10) );
  CPPUNIT_ASSERT ( ! empty1.includes(search9) );
}

//----------------------------------------------------------------------
void FStringTest::controlCodesTest()
{
  finalcut::FString bs_str = "t\b\bTesT\bt";
  CPPUNIT_ASSERT ( bs_str.removeBackspaces() == "Test" );
  bs_str = "ABC\b\b\b\b";
  CPPUNIT_ASSERT ( bs_str.removeBackspaces() == "" );
  CPPUNIT_ASSERT ( bs_str.removeBackspaces().isEmpty() );

  finalcut::FString del_str = "apple \177\177\177pietree";
  CPPUNIT_ASSERT ( del_str.removeDel() == "apple tree" );
  del_str = "\177\177\177\177ABC";
  CPPUNIT_ASSERT ( del_str.removeDel() == "" );
  CPPUNIT_ASSERT ( del_str.removeDel().isEmpty() );

  finalcut::FString tab_str = "one line";
  CPPUNIT_ASSERT ( tab_str.expandTabs() == "one line" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(4) == "one line" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(2) == "one line" );
  tab_str = "one\ttwo";
  CPPUNIT_ASSERT ( tab_str.expandTabs() == "one     two" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(4) == "one two" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(2) == "one two" );
  tab_str = "one\t\btwo";
  CPPUNIT_ASSERT ( tab_str.expandTabs() == "one     \btwo" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(4) == "one \btwo" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(2) == "one \btwo" );
  tab_str = "1\t2\t2";
  CPPUNIT_ASSERT ( tab_str.expandTabs() == "1       2       2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(4) == "1   2   2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(2) == "1 2 2" );
  tab_str = "12\t22\t2";
  CPPUNIT_ASSERT ( tab_str.expandTabs() == "12      22      2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(4) == "12  22  2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(2) == "12  22  2" );
  tab_str = "123\t222\t2";
  CPPUNIT_ASSERT ( tab_str.expandTabs() == "123     222     2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(4) == "123 222 2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(2) == "123 222 2" );
  tab_str = "1234\t2222\t2";
  CPPUNIT_ASSERT ( tab_str.expandTabs() == "1234    2222    2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(4) == "1234    2222    2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(2) == "1234  2222  2" );
  tab_str = "12345\t22222\t2";
  CPPUNIT_ASSERT ( tab_str.expandTabs() == "12345   22222   2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(4) == "12345   22222   2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(2) == "12345 22222 2" );
  tab_str = "123456\t222222\t2";
  CPPUNIT_ASSERT ( tab_str.expandTabs() == "123456  222222  2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(4) == "123456  222222  2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(2) == "123456  222222  2" );
  tab_str = "1234567\t2222222\t2";
  CPPUNIT_ASSERT ( tab_str.expandTabs() == "1234567 2222222 2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(4) == "1234567 2222222 2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(2) == "1234567 2222222 2" );
  tab_str = "12345678\t22222222\t2";
  CPPUNIT_ASSERT ( tab_str.expandTabs()
                   == "12345678        22222222        2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(4)
                   == "12345678    22222222    2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(2)
                   == "12345678  22222222  2" );
  tab_str = "12345678\t2";
  CPPUNIT_ASSERT ( tab_str.expandTabs() == "12345678        2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(4) == "12345678    2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(2) == "12345678  2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(0) == "12345678\t2" );
  CPPUNIT_ASSERT ( tab_str.expandTabs(-1) == "12345678\t2" );
  finalcut::FString cc(0x20);

  for (int i = 0; i < 0x1f; i++)
    cc[i] = i + 1;

  CPPUNIT_ASSERT ( cc.replaceControlCodes()
                   == "␁␂␃␄␅␆␇␈␉␊␋␌␍␎␏␐␑␒␓␔␕␖␗␘␙␚␛␜␝␞␟" );

  for (int i = 0; i <= 0x1f; i++)
    cc[i] = i + 0x80;

  CPPUNIT_ASSERT ( cc.replaceControlCodes() == finalcut::FString(32, L' ') );

  cc = "t\b\bTes\177Tt";
  CPPUNIT_ASSERT ( cc.replaceControlCodes() == "t␈␈Tes␡Tt" );
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FStringTest);

// The general unit test main part
#include <main-test.inc>
