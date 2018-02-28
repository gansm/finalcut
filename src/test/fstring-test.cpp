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
    { }

    void setUp();
    void tearDown();

  protected:
    void NoArgumentTest();
    void initLengthTest();
    void copyConstructorTest();
    void caseTest();
    void equalTest();
    void lessEqualTest();
    void lessTest();
    void GreaterEqualTest();
    void GreaterTest();
    void notEqualTest();
    void exceptionTest();

  private:
    FString* s;

  // Adds code needed to register the test suite
  CPPUNIT_TEST_SUITE (FStringTest);

  CPPUNIT_TEST (NoArgumentTest);
  CPPUNIT_TEST (initLengthTest);
  CPPUNIT_TEST (copyConstructorTest);
  CPPUNIT_TEST (caseTest);
  CPPUNIT_TEST (equalTest);
  CPPUNIT_TEST (notEqualTest);
  CPPUNIT_TEST (lessEqualTest);
  CPPUNIT_TEST (lessTest);
  CPPUNIT_TEST (GreaterEqualTest);
  CPPUNIT_TEST (GreaterTest);
  CPPUNIT_TEST (exceptionTest);

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
  CPPUNIT_ASSERT ( fstr == empty );
  CPPUNIT_ASSERT ( empty == '\0' );
  CPPUNIT_ASSERT ( empty == L'\0' );
  cstr = 0;
  CPPUNIT_ASSERT ( empty == cstr );
  wcstr = 0;
  CPPUNIT_ASSERT ( empty == std::string() );
  CPPUNIT_ASSERT ( empty == std::wstring() );

  CPPUNIT_ASSERT ( ! empty.includes('A') );
  CPPUNIT_ASSERT ( ! empty.includes(L'A') );
  CPPUNIT_ASSERT ( ! empty.includes("123") );
  CPPUNIT_ASSERT ( ! empty.includes(L"123") );
  CPPUNIT_ASSERT ( ! empty.includes(std::string("123")) );
  CPPUNIT_ASSERT ( ! empty.includes(std::wstring(L"123")) );
  CPPUNIT_ASSERT ( ! empty.includes(FString("123")) );

  std::stringstream out;
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
  CPPUNIT_ASSERT ( s2.getLength() == 10 );
  CPPUNIT_ASSERT ( ! s2.isNull() );
  CPPUNIT_ASSERT ( s2.isEmpty() );

  FString s4(x1, '-');
  CPPUNIT_ASSERT ( s4.getLength() == 10 );
  CPPUNIT_ASSERT ( ! s4.isNull() );
  CPPUNIT_ASSERT ( ! s4.isEmpty() );

  FString s5(x2, '-');
  CPPUNIT_ASSERT ( s5.getLength() == 10 );
  CPPUNIT_ASSERT ( ! s5.isNull() );
  CPPUNIT_ASSERT ( ! s5.isEmpty() );

  FString s6(x1, L'-');
  CPPUNIT_ASSERT ( s6.getLength() == 10 );
  CPPUNIT_ASSERT ( ! s6.isNull() );
  CPPUNIT_ASSERT ( ! s6.isEmpty() );

  FString s7(x2, L'-');
  CPPUNIT_ASSERT ( s7.getLength() == 10 );
  CPPUNIT_ASSERT ( ! s7.isNull() );
  CPPUNIT_ASSERT ( ! s7.isEmpty() );
}

//----------------------------------------------------------------------
void FStringTest::copyConstructorTest()
{
  FString s1("abc");
  FString s2(s1);
  CPPUNIT_ASSERT ( s2 == L"abc" );
  CPPUNIT_ASSERT ( s2.getLength() == 3 );
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
  CPPUNIT_ASSERT ( ! (s1 <= null_str2) );
  CPPUNIT_ASSERT ( null_str1 <= s2 );
  CPPUNIT_ASSERT ( null_str1 <= null_str2 );
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
  CPPUNIT_ASSERT ( s1 >= null_str2 );
  CPPUNIT_ASSERT ( ! (null_str1 >= s2) );
  CPPUNIT_ASSERT ( null_str1 >= null_str2 );
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
void FStringTest::exceptionTest()
{
  CPPUNIT_ASSERT_THROW ( FString("abc").toULong()
                       , std::invalid_argument );
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FStringTest);

// The general unit test main part
#include <main-test.inc>
