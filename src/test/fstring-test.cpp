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

#include <final/final.h>

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


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
    void NoArgument();
    void equalTest();
    void exceptionTest();

  private:
    FString* s;

  // Adds code needed to register the test suite
  CPPUNIT_TEST_SUITE (FStringTest);

  CPPUNIT_TEST (NoArgument);
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
void FStringTest::NoArgument()
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
  FString fstr = str;
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
}

//----------------------------------------------------------------------
void FStringTest::equalTest()
{
  // std::string -> FString -> std::string
  std::string s1;
  FString s2 = s1;
  std::string s3 = s2.toString();
  CPPUNIT_ASSERT ( s1 == s3 );

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
