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
    void NoArgumentTest();
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
    void GreaterEqualTest();
    void GreaterTest();
    void streamInsertionTest();
    void streamExtractionTest();
    void subscriptOperatorTest();
    void functionCallOperatorTest();
    void formatTest();
    void convertToNumberTest();
    void exceptionTest();
    void trimTest();
    void subStringTest();

  private:
    FString* s;

    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FStringTest);

    CPPUNIT_TEST (NoArgumentTest);
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
    CPPUNIT_TEST (GreaterEqualTest);
    CPPUNIT_TEST (GreaterTest);
    CPPUNIT_TEST (streamInsertionTest);
    CPPUNIT_TEST (streamExtractionTest);
    CPPUNIT_TEST (subscriptOperatorTest);
    CPPUNIT_TEST (functionCallOperatorTest);
    CPPUNIT_TEST (formatTest);
    CPPUNIT_TEST (convertToNumberTest);
    CPPUNIT_TEST (exceptionTest);
    CPPUNIT_TEST (trimTest);
    CPPUNIT_TEST (subStringTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};
#pragma pack(pop)

//----------------------------------------------------------------------
void FStringTest::setUp()
{
  std::setlocale(LC_CTYPE, "");
  s = new FString('c');
}

//----------------------------------------------------------------------
void FStringTest::tearDown()
{
  delete s;
}

//----------------------------------------------------------------------
void FStringTest::NoArgumentTest()
{
  FString empty;
  CPPUNIT_ASSERT ( empty.isNull() );
  CPPUNIT_ASSERT ( empty.isEmpty() );
  CPPUNIT_ASSERT ( empty.getLength() == 0 );
  CPPUNIT_ASSERT ( empty.getUTF8length() == 0 );
  CPPUNIT_ASSERT ( empty.wc_str() == 0 );
  CPPUNIT_ASSERT ( empty.c_str() == 0 );
  CPPUNIT_ASSERT_EQUAL ( empty.toString(), std::string() );

  char* cstr = empty.c_str();
  CPPUNIT_ASSERT ( cstr == 0 );
  wchar_t* wcstr = empty.wc_str();
  CPPUNIT_ASSERT ( wcstr == 0 );
  std::string str = empty.toString();
  CPPUNIT_ASSERT ( str.length() == 0 );
  CPPUNIT_ASSERT ( str.size() == 0 );
  CPPUNIT_ASSERT ( str.empty() );
  const FString fstr = str;
  CPPUNIT_ASSERT ( ! fstr.isNull() );
  CPPUNIT_ASSERT ( fstr.isEmpty() );

  cstr = 0;
  CPPUNIT_ASSERT ( empty == cstr );
  wcstr = 0;
  CPPUNIT_ASSERT ( empty == wcstr );

  CPPUNIT_ASSERT ( ! empty.includes('A') );
  CPPUNIT_ASSERT ( ! empty.includes(L'A') );
  CPPUNIT_ASSERT ( ! empty.includes("123") );
  CPPUNIT_ASSERT ( ! empty.includes(L"123") );
  CPPUNIT_ASSERT ( ! empty.includes(std::string("123")) );
  CPPUNIT_ASSERT ( ! empty.includes(std::wstring(L"123")) );
  CPPUNIT_ASSERT ( ! empty.includes(FString("123")) );

  std::ostringstream out;
  out << empty;
  CPPUNIT_ASSERT ( out.str() == "" );

  // Fill the empty string with "123"
  empty << "123";
  CPPUNIT_ASSERT_EQUAL ( empty, FString(L"123") );

  empty.clear();
  CPPUNIT_ASSERT ( empty.isNull() );
}

//----------------------------------------------------------------------
void FStringTest::initLengthTest()
{
  FString s1(0);
  CPPUNIT_ASSERT ( s1.getLength() == 0 );
  CPPUNIT_ASSERT ( s1.isNull() );
  CPPUNIT_ASSERT ( s1.isEmpty() );

  int  x1 = 10;
  uInt x2 = 10;
  FString s2(x1);
  CPPUNIT_ASSERT ( s2.getLength() == 10 );
  CPPUNIT_ASSERT ( ! s2.isNull() );
  CPPUNIT_ASSERT ( s2.isEmpty() );

  FString s3(x2);
  CPPUNIT_ASSERT ( s3.getLength() == 10 );
  CPPUNIT_ASSERT ( ! s3.isNull() );
  CPPUNIT_ASSERT ( s3.isEmpty() );

  FString s4(0, L'-');
  CPPUNIT_ASSERT ( s4.getLength() == 0 );
  CPPUNIT_ASSERT ( s4.isNull() );
  CPPUNIT_ASSERT ( s4.isEmpty() );

  FString s5(0, '-');
  CPPUNIT_ASSERT ( s5.getLength() == 0 );
  CPPUNIT_ASSERT ( s5.isNull() );
  CPPUNIT_ASSERT ( s5.isEmpty() );

  FString s6(0, char(0));
  CPPUNIT_ASSERT ( s6.getLength() == 0 );
  CPPUNIT_ASSERT ( s6.isNull() );
  CPPUNIT_ASSERT ( s6.isEmpty() );

  FString s7(x1, '-');
  CPPUNIT_ASSERT ( s7.getLength() == 10 );
  CPPUNIT_ASSERT ( ! s7.isNull() );
  CPPUNIT_ASSERT ( ! s7.isEmpty() );

  FString s8(x2, '-');
  CPPUNIT_ASSERT ( s8.getLength() == 10 );
  CPPUNIT_ASSERT ( ! s8.isNull() );
  CPPUNIT_ASSERT ( ! s8.isEmpty() );

  FString s9(x1, L'-');
  CPPUNIT_ASSERT ( s9.getLength() == 10 );
  CPPUNIT_ASSERT ( ! s9.isNull() );
  CPPUNIT_ASSERT ( ! s9.isEmpty() );

  FString s10(x2, L'-');
  CPPUNIT_ASSERT ( s10.getLength() == 10 );
  CPPUNIT_ASSERT ( ! s10.isNull() );
  CPPUNIT_ASSERT ( ! s10.isEmpty() );

  FString s11(x2, wchar_t(0));
  CPPUNIT_ASSERT ( s11.getLength() == 10 );
  CPPUNIT_ASSERT ( ! s11.isNull() );
  CPPUNIT_ASSERT ( s11.isEmpty() );
}

//----------------------------------------------------------------------
void FStringTest::copyConstructorTest()
{
  const FString s1("abc");
  FString s2(s1);
  CPPUNIT_ASSERT ( s2 == L"abc" );
  CPPUNIT_ASSERT ( s2.getLength() == 3 );
}

//----------------------------------------------------------------------
void FStringTest::assignmentTest()
{
  FString s1;
  s1 = static_cast<FString>(0);
  CPPUNIT_ASSERT ( s1.isNull() );
  CPPUNIT_ASSERT ( s1.isEmpty() );

  s1 = std::wstring();
  CPPUNIT_ASSERT ( s1.isNull() );
  CPPUNIT_ASSERT ( s1.isEmpty() );

  s1 = std::string();
  CPPUNIT_ASSERT ( s1.isNull() );
  CPPUNIT_ASSERT ( s1.isEmpty() );

  s1 = static_cast<wchar_t*>(0);
  CPPUNIT_ASSERT ( s1.isNull() );
  CPPUNIT_ASSERT ( s1.isEmpty() );

  s1 = static_cast<char*>(0);
  CPPUNIT_ASSERT ( s1.isNull() );
  CPPUNIT_ASSERT ( s1.isEmpty() );

  s1 = wchar_t(0);
  CPPUNIT_ASSERT ( s1.isNull() );
  CPPUNIT_ASSERT ( s1.isEmpty() );

  s1 = char(0);
  CPPUNIT_ASSERT ( s1.isNull() );
  CPPUNIT_ASSERT ( s1.isEmpty() );

  const FString s2("abc");
  s1 = s2;
  CPPUNIT_ASSERT ( s1 == L"abc" );
  CPPUNIT_ASSERT ( s1.getLength() == 3 );

  const std::wstring s3(L"def");
  s1 = s3;
  CPPUNIT_ASSERT ( s1 == L"def" );
  CPPUNIT_ASSERT ( s1.getLength() == 3 );

  const std::string s4("ghi");
  s1 = s4;
  CPPUNIT_ASSERT ( s1 == L"ghi" );
  CPPUNIT_ASSERT ( s1.getLength() == 3 );

  const wchar_t s5[] = L"abc";
  s1 = s5;
  CPPUNIT_ASSERT ( s1 == L"abc" );
  CPPUNIT_ASSERT ( s1.getLength() == 3 );

  const char s6[] = "def";
  s1 = s6;
  CPPUNIT_ASSERT ( s1 == L"def" );
  CPPUNIT_ASSERT ( s1.getLength() == 3 );

  const wchar_t s7 = L'#';
  s1 = s7;
  CPPUNIT_ASSERT ( s1 == L"#" );
  CPPUNIT_ASSERT ( s1.getLength() == 1 );

  const char s8 = '%';
  s1 = s8;
  CPPUNIT_ASSERT ( s1 == L"%" );
  CPPUNIT_ASSERT ( s1.getLength() == 1 );
}

//----------------------------------------------------------------------
void FStringTest::additionAssignmentTest()
{
  FString s1;
  s1 += FString("abc");
  CPPUNIT_ASSERT ( s1 == L"abc" );
  s1 += FString("def");
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
  FString s1("abc");
  FString s2 = s1 + FString("def");
  CPPUNIT_ASSERT ( s2 == L"abcdef" );

  s2.clear();
  s2 = s1 + std::wstring(L"def");
  CPPUNIT_ASSERT ( s2 == L"abcdef" );

  s2.clear();
  s2 = s1 + const_cast<wchar_t*>(L"def");
  CPPUNIT_ASSERT ( s2 == L"abcdef" );

  s2.clear();
  s2 = s1 + std::string("def");
  CPPUNIT_ASSERT ( s2 == L"abcdef" );

  s2.clear();
  s2 = s1 + const_cast<char*>("def");
  CPPUNIT_ASSERT ( s2 == L"abcdef" );

  s2.clear();
  s2 = s1 + wchar_t(L'd');
  CPPUNIT_ASSERT ( s2 == L"abcd" );

  s2.clear();
  s2 = s1 + char('d');
  CPPUNIT_ASSERT ( s2 == L"abcd" );
}

//----------------------------------------------------------------------
void FStringTest::caseTest()
{
  FString str1("abc");
  CPPUNIT_ASSERT ( str1.toUpper() == "ABC" );

  FString str2("XYZ");
  CPPUNIT_ASSERT ( str2.toLower() == "xyz" );
}

//----------------------------------------------------------------------
void FStringTest::equalTest()
{
  // std::string -> FString -> std::string
  const std::string s1 = "string";
  FString fs = s1;
  const std::string s2 = fs.toString();
  CPPUNIT_ASSERT ( s1 == s2 );

  // std::wstring -> FString -> std::wstring
  const std::wstring ws1 = L"wide string";
  fs = ws1;
  std::wstring ws2 = fs.wc_str();
  CPPUNIT_ASSERT ( ws1 == ws2 );

  const FString one_char('a');
  const char ch = 'a';
  CPPUNIT_ASSERT ( one_char == ch );
  CPPUNIT_ASSERT ( ch == one_char.c_str()[0] );
  CPPUNIT_ASSERT ( one_char.getLength() == 1 );

  const wchar_t wch = L'a';
  CPPUNIT_ASSERT ( one_char == wch );
  CPPUNIT_ASSERT ( wch == one_char.wc_str()[0] );

  const FString str(L"abc");
  const FString str2(L"abc");
  CPPUNIT_ASSERT ( str == str2 );

  const char cstr[] = "abc";
  CPPUNIT_ASSERT ( str == cstr );
  CPPUNIT_ASSERT ( str.getLength() == 3 );
  CPPUNIT_ASSERT ( str.getUTF8length() == 3 );
  CPPUNIT_ASSERT ( strncmp(cstr, str.c_str(), 3) == 0 );

  const wchar_t wcstr[] = L"abc";
  CPPUNIT_ASSERT ( str == wcstr );
  CPPUNIT_ASSERT ( wcsncmp(wcstr, str.wc_str(), 3) == 0 );

  const std::string st = "abc";
  CPPUNIT_ASSERT ( str == st );

  const std::wstring wst = L"abc";
  CPPUNIT_ASSERT ( str == wst );

  const FString null_str1;
  const FString null_str2;
  CPPUNIT_ASSERT ( ! (str == null_str2) );
  CPPUNIT_ASSERT ( ! (null_str1 == str) );
  CPPUNIT_ASSERT ( null_str1 == null_str2 );

  CPPUNIT_ASSERT ( s->c_str()[0] == 'c');
  CPPUNIT_ASSERT ( s->getLength() == 1 );
}

//----------------------------------------------------------------------
void FStringTest::notEqualTest()
{
  const FString one_char('@');
  const char ch = '!';
  CPPUNIT_ASSERT ( one_char != ch );
  CPPUNIT_ASSERT ( ch != one_char.c_str()[0] );
  CPPUNIT_ASSERT ( one_char.getLength() == 1 );

  const wchar_t wch = L'_';
  CPPUNIT_ASSERT ( one_char != wch );
  CPPUNIT_ASSERT ( wch != one_char.wc_str()[0] );

  const FString s1 = L"ABC";  // latin letter
  const FString s2 = L"АВС";  // cyrillic letters
  CPPUNIT_ASSERT ( s1 != s2 );

  const char cstr[] = "abc";
  CPPUNIT_ASSERT ( s1 != cstr );
  CPPUNIT_ASSERT ( s1.getLength() == 3 );
  CPPUNIT_ASSERT ( strlen(s1.c_str()) == 3 );
  CPPUNIT_ASSERT ( s2.getLength() == 3 );
  CPPUNIT_ASSERT ( strlen(s2.c_str()) == 6 );
  CPPUNIT_ASSERT ( s1.getUTF8length() == 3 );
  CPPUNIT_ASSERT ( s2.getUTF8length() == 3 );
  CPPUNIT_ASSERT ( strncmp(cstr, s1.c_str(), 3) != 0 );

  const wchar_t wcstr[] = L"abc";
  CPPUNIT_ASSERT ( s1 != wcstr );
  CPPUNIT_ASSERT ( wcsncmp(wcstr, s1.wc_str(), 3) != 0 );

  const std::string st = "abc";
  CPPUNIT_ASSERT ( s1 != st );

  const std::wstring wst = L"abc";
  CPPUNIT_ASSERT ( s1 != wst );

  const FString null_str1;
  const FString null_str2;
  CPPUNIT_ASSERT ( s1 != null_str2 );
  CPPUNIT_ASSERT ( null_str1 != s1 );
  CPPUNIT_ASSERT ( ! (null_str1 != null_str2) );

  CPPUNIT_ASSERT ( s->c_str()[0] != 's');
}

//----------------------------------------------------------------------
void FStringTest::lessEqualTest()
{
  const FString one_char('x');
  const char ch = 'z';
  CPPUNIT_ASSERT ( one_char <= ch );

  const wchar_t wch = L'z';
  CPPUNIT_ASSERT ( one_char <= wch );

  const FString s1 = L"xyz";
  const FString s2 = L"xyz";
  const FString s3 = L"xzz";
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

  const FString null_str1;
  const FString null_str2;
  const FString empty("");
  CPPUNIT_ASSERT ( ! (s1 <= null_str2) );
  CPPUNIT_ASSERT ( null_str1 <= s2 );
  CPPUNIT_ASSERT ( null_str1 <= null_str2 );
  CPPUNIT_ASSERT ( ! (s1 <= empty) );
}

//----------------------------------------------------------------------
void FStringTest::lessTest()
{
  const FString one_char('x');
  const char ch = 'z';
  CPPUNIT_ASSERT ( one_char < ch );

  const wchar_t wch = L'z';
  CPPUNIT_ASSERT ( one_char < wch );

  const FString s1 = L"xyz";
  const FString s2 = L"xzz";
  CPPUNIT_ASSERT ( s1 < s2 );

  const char cstr[] = "xzz";
  CPPUNIT_ASSERT ( s1 < cstr );

  const wchar_t wcstr[] = L"xzz";
  CPPUNIT_ASSERT ( s1 < wcstr );

  const std::string st = "xzz";
  CPPUNIT_ASSERT ( s1 < st  );

  const std::wstring wst = L"xzz";
  CPPUNIT_ASSERT ( s1 < wst );

  const FString null_str1;
  const FString null_str2;
  CPPUNIT_ASSERT ( ! (s1 < null_str2) );
  CPPUNIT_ASSERT ( null_str1 < s2 );
  CPPUNIT_ASSERT ( ! (null_str1 < null_str2) );
}

//----------------------------------------------------------------------
void FStringTest::GreaterEqualTest()
{
  const FString one_char('x');
  const char ch = 'x';
  CPPUNIT_ASSERT ( one_char >= ch );

  const wchar_t wch = L'x';
  CPPUNIT_ASSERT ( one_char >= wch );

  const FString s1 = L"xyz";
  const FString s2 = L"xyz";
  const FString s3 = L"xxz";
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

  const FString null_str1;
  const FString null_str2;
  const FString empty("");
  CPPUNIT_ASSERT ( s1 >= null_str2 );
  CPPUNIT_ASSERT ( ! (null_str1 >= s2) );
  CPPUNIT_ASSERT ( null_str1 >= null_str2 );
  CPPUNIT_ASSERT ( ! (s1 <= empty) );
}

//----------------------------------------------------------------------
void FStringTest::GreaterTest()
{
  const FString one_char('x');
  const char ch = 'w';
  CPPUNIT_ASSERT ( one_char > ch );

  const wchar_t wch = L'w';
  CPPUNIT_ASSERT ( one_char > wch );

  const FString s1 = L"xyz";
  const FString s2 = L"xww";
  CPPUNIT_ASSERT ( s1 > s2 );

  const char cstr[] = "xww";
  CPPUNIT_ASSERT ( s1 > cstr );

  const wchar_t wcstr[] = L"xww";
  CPPUNIT_ASSERT ( s1 > wcstr );

  const std::string st = "xww";
  CPPUNIT_ASSERT ( s1 > st  );

  const std::wstring wst = L"xww";
  CPPUNIT_ASSERT ( s1 > wst );

  const FString null_str1;
  const FString null_str2;
  CPPUNIT_ASSERT ( s1 > null_str2 );
  CPPUNIT_ASSERT ( ! (null_str1 > s2) );
  CPPUNIT_ASSERT ( ! (null_str1 > null_str2) );
}

//----------------------------------------------------------------------
void FStringTest::streamInsertionTest()
{
  FString out;
  out << FString("ABC");
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
}

//----------------------------------------------------------------------
void FStringTest::streamExtractionTest()
{
  FString in_1;
  FString("ABC") >> in_1;
  CPPUNIT_ASSERT ( in_1 == "ABC" );

  std::wstring in_2;
  FString("ABC") >> in_2;
  CPPUNIT_ASSERT ( in_2 == L"ABC" );

  std::string in_3;
  FString("ABC") >> in_3;
  CPPUNIT_ASSERT ( in_3 == "ABC" );

  wchar_t in_4;
  FString("A") >> in_4;
  CPPUNIT_ASSERT ( in_4 == L'A' );

  char in_5;
  FString("A") >> in_5;
  CPPUNIT_ASSERT ( in_5 == L'A' );

  sInt16 in_6;
  FString("-12345") >> in_6;
  CPPUNIT_ASSERT ( in_6 == -12345 );

  uInt16 in_7;
  FString("33333") >> in_7;
  CPPUNIT_ASSERT ( in_7 == 33333 );

  int in_8;
  FString("-12345678") >> in_8;
  CPPUNIT_ASSERT ( in_8 == -12345678 );

  uInt in_9;
  FString("99999999") >> in_9;
  CPPUNIT_ASSERT ( in_9 == 99999999 );

  long in_10;
  FString("-1234567890") >> in_10;
  CPPUNIT_ASSERT ( in_10 == -1234567890 );

  uLong in_11;
  FString("9999999999999") >> in_11;
  CPPUNIT_ASSERT ( in_11 == 9999999999999 );

  float in_12;
  FString("2.71828") >> in_12;
  CPPUNIT_ASSERT ( in_12 == 2.71828f );

  double in_13;
  FString("2.7182818284590452353") >> in_13;
  CPPUNIT_ASSERT ( in_13 == 2.7182818284590452353 );
}

//----------------------------------------------------------------------
void FStringTest::subscriptOperatorTest()
{
  FString s(3);
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
void FStringTest::functionCallOperatorTest()
{
  FString str = L"test";
  CPPUNIT_ASSERT_EQUAL ( str, str() );

  FString copy = str();
  copy << L"string";
  CPPUNIT_ASSERT ( str() == "test" );
  CPPUNIT_ASSERT ( copy  == "teststring" );
}

//----------------------------------------------------------------------
void FStringTest::formatTest()
{
  FString str1;
  int num = 3;
  char location[] = "zoo";
  str1.sprintf ("There are %d lions in the %s", num, location);
  CPPUNIT_ASSERT ( str1 == "There are 3 lions in the zoo" );

  FString str2;
  str2.sprintf (L"It costs only %d cent", 50);
  CPPUNIT_ASSERT ( str2 == "It costs only 50 cent" );

  std::setlocale (LC_NUMERIC, "C");
  FString fnum1, fnum2;

#if defined(__LP64__) || defined(_LP64)
  // 64-bit architecture
  fnum1.setFormatedNumber(0xffffffffffffffff, '\'');
  CPPUNIT_ASSERT ( fnum1 == "18'446'744'073'709'551'615" );

  fnum2.setFormatedNumber(-9223372036854775807);
  CPPUNIT_ASSERT ( fnum2 == "-9 223 372 036 854 775 807" );
#else
  // 32-bit architecture
  fnum1.setFormatedNumber(0xffffffff, '\'');
  CPPUNIT_ASSERT ( fnum1 == "4'294'967'295" );

  fnum2.setFormatedNumber(-2147483647);
  CPPUNIT_ASSERT ( fnum2 == "-2 147 483 647" );
#endif
}

//----------------------------------------------------------------------
void FStringTest::convertToNumberTest()
{
  FString str = "-127";
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
void FStringTest::exceptionTest()
{
  CPPUNIT_ASSERT_THROW ( FString("abc").toULong()
                       , std::invalid_argument );

  CPPUNIT_ASSERT_THROW ( FString("abc")[3]
                       , std::out_of_range );

  CPPUNIT_ASSERT_THROW ( FString("abc")[-1]
                       , std::out_of_range );

  CPPUNIT_ASSERT_THROW ( FString("99999").toShort()
                       , std::overflow_error );

  CPPUNIT_ASSERT_THROW ( FString("-99999").toShort()
                       , std::underflow_error );

  CPPUNIT_ASSERT_THROW ( FString("99999").toUShort()
                       , std::overflow_error );

  CPPUNIT_ASSERT_THROW ( FString("-1").toUShort()
                       , std::underflow_error );

  CPPUNIT_ASSERT_THROW ( FString("9999999999").toInt()
                       , std::overflow_error );

  CPPUNIT_ASSERT_THROW ( FString("-9999999999").toInt()
                       , std::underflow_error );

  CPPUNIT_ASSERT_THROW ( FString("9999999999").toUInt()
                       , std::overflow_error );

  CPPUNIT_ASSERT_THROW ( FString("-1").toUInt()
                       , std::underflow_error );

  CPPUNIT_ASSERT_THROW ( FString("9999999999999999999").toLong()
                       , std::overflow_error );

  CPPUNIT_ASSERT_THROW ( FString("-9999999999999999999").toLong()
                       , std::underflow_error );

  CPPUNIT_ASSERT_THROW ( FString("99999999999999999999").toULong()
                       , std::overflow_error );

  CPPUNIT_ASSERT_THROW ( FString("-1").toULong()
                       , std::underflow_error );

  CPPUNIT_ASSERT_THROW ( FString().toLong()
                       , std::invalid_argument );

  CPPUNIT_ASSERT_THROW ( FString("").toLong()
                       , std::invalid_argument );

  CPPUNIT_ASSERT_THROW ( FString("one").toLong()
                       , std::invalid_argument );

  CPPUNIT_ASSERT_THROW ( FString().toULong()
                       , std::invalid_argument );

  CPPUNIT_ASSERT_THROW ( FString("").toULong()
                       , std::invalid_argument );

  CPPUNIT_ASSERT_THROW ( FString("one").toULong()
                       , std::invalid_argument );

  CPPUNIT_ASSERT_THROW ( FString("1E+42").toFloat()
                       , std::overflow_error );

  CPPUNIT_ASSERT_THROW ( FString("-1E+42").toFloat()
                       , std::overflow_error );

  CPPUNIT_ASSERT_THROW ( FString("1.19209290E-08").toFloat()
                       , std::underflow_error );

  CPPUNIT_ASSERT_THROW ( FString("1.7976931348623157E+309").toDouble()
                       , std::overflow_error );

  CPPUNIT_ASSERT_THROW ( FString("-1.7976931348623157E+309").toDouble()
                       , std::overflow_error );

  CPPUNIT_ASSERT_THROW ( FString("2.225074e-310").toDouble()
                       , std::underflow_error );

  CPPUNIT_ASSERT_THROW ( FString().toDouble()
                       , std::invalid_argument );

  CPPUNIT_ASSERT_THROW ( FString("").toDouble()
                       , std::invalid_argument );

  CPPUNIT_ASSERT_THROW ( FString("one").toDouble()
                       , std::invalid_argument );
}

//----------------------------------------------------------------------
void FStringTest::trimTest()
{
  const FString& trim_str1 = L"\r\n\t  A string \n\t";
  CPPUNIT_ASSERT ( trim_str1.rtrim() == L"\r\n\t  A string" );
  CPPUNIT_ASSERT ( trim_str1.ltrim() == L"A string \n\t" );
  CPPUNIT_ASSERT ( trim_str1.trim() == L"A string" );

  const FString& trim_str2 = L"\n  \n\n";
  CPPUNIT_ASSERT ( trim_str2.rtrim().isEmpty() );
  CPPUNIT_ASSERT ( ! trim_str2.rtrim().isNull() );
  CPPUNIT_ASSERT ( trim_str2.rtrim().getLength() == 0 );

  CPPUNIT_ASSERT ( trim_str2.ltrim().isEmpty() );
  CPPUNIT_ASSERT ( ! trim_str2.ltrim().isNull() );
  CPPUNIT_ASSERT ( trim_str2.ltrim().getLength() == 0 );

  const FString trim_str3;
  CPPUNIT_ASSERT ( trim_str3.trim().isEmpty() );
  CPPUNIT_ASSERT ( trim_str3.trim().isNull() );
  CPPUNIT_ASSERT ( trim_str3.trim().getLength() == 0 );
}

//----------------------------------------------------------------------
void FStringTest::subStringTest()
{
  FString str1("Look behind you, a three-headed monkey!");
  CPPUNIT_ASSERT ( str1.left(uInt(11)) == L"Look behind" );
  CPPUNIT_ASSERT ( str1.left(int(11)) == L"Look behind" );
  CPPUNIT_ASSERT ( str1.left(999)
                   == L"Look behind you, a three-headed monkey!" );
  CPPUNIT_ASSERT ( str1.left(-5) == L"" );
  CPPUNIT_ASSERT ( str1.left(0) == L"" );
  CPPUNIT_ASSERT ( str1.left(0).isEmpty() );
  CPPUNIT_ASSERT ( ! str1.left(0).isNull() );
  CPPUNIT_ASSERT ( FString().left(5).isNull() );
  CPPUNIT_ASSERT ( ! FString("").left(5).isNull() );
  CPPUNIT_ASSERT ( FString("").left(5).isEmpty() );

  CPPUNIT_ASSERT ( str1.right(uInt(7)) == L"monkey!" );
  CPPUNIT_ASSERT ( str1.right(int(7)) == L"monkey!" );
  CPPUNIT_ASSERT ( str1.right(999)
                   == L"Look behind you, a three-headed monkey!" );
  CPPUNIT_ASSERT ( str1.right(-5) == L"" );
  CPPUNIT_ASSERT ( str1.right(0) == L"" );
  CPPUNIT_ASSERT ( str1.right(0).isEmpty() );
  CPPUNIT_ASSERT ( ! str1.right(0).isNull() );
  CPPUNIT_ASSERT ( FString().right(5).isNull() );
  CPPUNIT_ASSERT ( ! FString("").right(5).isNull() );
  CPPUNIT_ASSERT ( FString("").right(5).isEmpty() );

  CPPUNIT_ASSERT ( str1.mid(uInt(18), uInt(21))
                   == L"a three-headed monkey" );
  CPPUNIT_ASSERT ( str1.mid(int(18), int(21))
                   == L"a three-headed monkey" );
  CPPUNIT_ASSERT ( str1.mid(1, 999)
                   == L"Look behind you, a three-headed monkey!" );
  CPPUNIT_ASSERT ( str1.mid(5, 0) == L"" );
  CPPUNIT_ASSERT ( str1.mid(-5, 2) == L"" );
  CPPUNIT_ASSERT ( str1.mid(0, 0) == L"" );
  CPPUNIT_ASSERT ( str1.mid(0, 5) == L"" );
  CPPUNIT_ASSERT ( str1.mid(0, 0).isEmpty() );
  CPPUNIT_ASSERT ( ! str1.mid(0, 0).isNull() );
  CPPUNIT_ASSERT ( FString().mid(5, 0).isNull() );
  CPPUNIT_ASSERT ( ! FString("").mid(5, 0).isNull() );
  CPPUNIT_ASSERT ( FString("").mid(5, 0).isEmpty() );

  FStringList string_parts = str1.split(" ");
  FStringList string_list;
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
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FStringTest);

// The general unit test main part
#include <main-test.inc>
