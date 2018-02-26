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
    void caseTest();
    void equalTest();
    void exceptionTest();

  private:
    FString* s;

  // Adds code needed to register the test suite
  CPPUNIT_TEST_SUITE (FStringTest);

  CPPUNIT_TEST (NoArgumentTest);
  CPPUNIT_TEST (caseTest);
  CPPUNIT_TEST (equalTest);
  CPPUNIT_TEST (exceptionTest);

  // End of test suite definition
  CPPUNIT_TEST_SUITE_END();
};
#pragma pack(pop)

//----------------------------------------------------------------------
void FStringTest::setUp()
{
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
  const std::string s1 = "abc";
  FString s2 = s1;
  std::string s3 = s2.toString();
  CPPUNIT_ASSERT ( s1 == s3 );

  FString one_char('a');
  const char ch = 'a';
  CPPUNIT_ASSERT ( one_char == ch );
  CPPUNIT_ASSERT ( ch == one_char.c_str()[0] );
  CPPUNIT_ASSERT ( one_char.getLength() == 1 );

  const wchar_t wch = L'a';
  CPPUNIT_ASSERT ( one_char == wch );
  CPPUNIT_ASSERT ( wch == one_char.wc_str()[0] );

  FString str(L"abc");
  const char cstr[] = "abc";
  CPPUNIT_ASSERT ( str == cstr );
  CPPUNIT_ASSERT ( str.getLength() == 3 );
  CPPUNIT_ASSERT ( str.getUTF8length() == 3 );
  CPPUNIT_ASSERT ( strncmp(cstr, str.c_str(), 3) == 0 );

  const wchar_t wcstr[] = L"abc";
  CPPUNIT_ASSERT ( str == wcstr );
  CPPUNIT_ASSERT ( wcsncmp(wcstr, str.wc_str(), 3) == 0 );

  FString str2(L"abc");
  CPPUNIT_ASSERT ( str == str2 );

  CPPUNIT_ASSERT ( s->getLength() == 1 );
  CPPUNIT_ASSERT ( s->c_str()[0] == 'c');
  CPPUNIT_ASSERT ( s->c_str()[0] != 's');
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
