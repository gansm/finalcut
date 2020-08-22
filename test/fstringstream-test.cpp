/***********************************************************************
* fstringstream-test.cpp - FStringStream unit tests                    *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2020 Markus Gans                                           *
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

#include <sstream>
#include <string>
#include <utility>

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <final/final.h>

//----------------------------------------------------------------------
// class FStringStreamTest
//----------------------------------------------------------------------

class FStringStreamTest : public CPPUNIT_NS::TestFixture
{
  public:
    FStringStreamTest()
    { }

  protected:
    void classNameTest();
    void defaultObjectTest();
    void moveConstructorTest();
    void assignmentTest();
    void swapTest();
    void rdbufTest();
    void fileTest();


  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FStringStreamTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (defaultObjectTest);
    CPPUNIT_TEST (moveConstructorTest);
    CPPUNIT_TEST (assignmentTest);
    CPPUNIT_TEST (swapTest);
    CPPUNIT_TEST (rdbufTest);
    CPPUNIT_TEST (fileTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};

//----------------------------------------------------------------------
void FStringStreamTest::classNameTest()
{
  finalcut::FStringStream string_stream;
  const finalcut::FString& classname = string_stream.getClassName();
  CPPUNIT_ASSERT ( classname == "FStringStream" );
}

//----------------------------------------------------------------------
void FStringStreamTest::defaultObjectTest()
{
  finalcut::FStringStream ss{std::ios_base::out};
  CPPUNIT_ASSERT ( ss.str().isNull() );
  CPPUNIT_ASSERT ( ss.str().isEmpty() );
  CPPUNIT_ASSERT ( ss.str() != "" );

  ss << "Hello";
  CPPUNIT_ASSERT ( ! ss.str().isNull() );
  CPPUNIT_ASSERT ( ! ss.str().isEmpty() );
  CPPUNIT_ASSERT ( ss.str() = "Hello" );

  ss << ", World!";
  CPPUNIT_ASSERT ( ss.str() == "Hello, World!" );
  CPPUNIT_ASSERT ( ss.str() == L"Hello, World!" );

  ss.clear();
  CPPUNIT_ASSERT ( ss.str().isNull() );
  CPPUNIT_ASSERT ( ss.str().isEmpty() );
  CPPUNIT_ASSERT ( ss.str() != "" );
  ss.clear();

  ss << "Three" << " " << "parts";
  CPPUNIT_ASSERT ( ss.str() == L"Three parts" );
  ss.clear();

  finalcut::FStringStream in{"21 45 45", std::ios_base::in};
  int n1{0};
  int n2{0};
  int n3{0};
  in >> std::hex >> n1 >> n2
     >> std::oct >> n3;
  CPPUNIT_ASSERT ( n1 == 33 );
  CPPUNIT_ASSERT ( n2 == 69 );
  CPPUNIT_ASSERT ( n3 == 37 );

  ss << "Line break at the end" << std::endl;
  CPPUNIT_ASSERT ( ss.str() == "Line break at the end\n" );
  ss.clear();

  ss << std::resetiosflags(std::ios_base::basefield)
     << 20 << " " << std::showbase << std::hex
     << 20 << " " << std::noshowbase << std::oct
     << 20 << " " << std::boolalpha
     << bool(20) << " " << std::dec
     << 20;
  CPPUNIT_ASSERT ( ss.str() == "20 0x14 24 true 20" );
  ss.clear();

  ss << "|" << std::setfill(L'-') << std::setw(6) << "|";
  CPPUNIT_ASSERT ( ss.str() == "|-----|" );
}

//----------------------------------------------------------------------
void FStringStreamTest::moveConstructorTest()
{
  finalcut::FStringStream ss1{"abc"};
  const finalcut::FStringStream ss2{std::move(ss1)};
  CPPUNIT_ASSERT ( ss2.str() == L"abc" );
  CPPUNIT_ASSERT ( ss2.str().getLength() == 3 );
  CPPUNIT_ASSERT ( ss1.str().isNull() );
  CPPUNIT_ASSERT ( ss1.str().isEmpty() );
  CPPUNIT_ASSERT ( ss1.str().getLength() == 0 );
}

//----------------------------------------------------------------------
void FStringStreamTest::assignmentTest()
{
  finalcut::FStringStream ss1{"xyz"};
  finalcut::FStringStream ss2{};
  ss2 = std::move(ss1);
  CPPUNIT_ASSERT ( ss2.str() == L"xyz" );
  CPPUNIT_ASSERT ( ss2.str().getLength() == 3 );
  CPPUNIT_ASSERT ( ss1.str().isNull() );
  CPPUNIT_ASSERT ( ss1.str().isEmpty() );
  CPPUNIT_ASSERT ( ss1.str().getLength() == 0 );
}

//----------------------------------------------------------------------
void FStringStreamTest::swapTest()
{
  finalcut::FStringStream ss1{"FStringStream"};
  finalcut::FStringStream ss2{"FINAL CUT"};
  CPPUNIT_ASSERT ( ss1.str() == "FStringStream" );
  CPPUNIT_ASSERT ( ss2.str() == "FINAL CUT" );
  ss1.swap(ss2);
  CPPUNIT_ASSERT ( ss1.str() == "FINAL CUT" );
  CPPUNIT_ASSERT ( ss2.str() == "FStringStream" );

  finalcut::FStringStream ss3{"dog"};
  finalcut::FStringStream ss4{"cat"};
  CPPUNIT_ASSERT ( ss3.str() == "dog" );
  CPPUNIT_ASSERT ( ss4.str() == "cat" );
  finalcut::swap (ss3, ss4);
  CPPUNIT_ASSERT ( ss3.str() == "cat" );
  CPPUNIT_ASSERT ( ss4.str() == "dog" );
}

//----------------------------------------------------------------------
void FStringStreamTest::rdbufTest()
{
  finalcut::FStringStream ss{};
  std::wostream os (ss.rdbuf());  // Associate stream buffer to stream
  ss.rdbuf()->sputn (L"0x", 2);
  os << std::hex << 255;
  CPPUNIT_ASSERT ( ss.str() == "0xff" );
  CPPUNIT_ASSERT ( ss.str().getLength() == 4 );

  ss.rdbuf()->str(L"");
  CPPUNIT_ASSERT ( ss.str().isNull() );
  CPPUNIT_ASSERT ( ss.str().isEmpty() );
  CPPUNIT_ASSERT ( ss.str().getLength() == 0 );
}

//----------------------------------------------------------------------
void FStringStreamTest::fileTest()
{
  std::string filename = "test.log";
  finalcut::FStringStream ss{};

  {
    std::ofstream file_stream(filename, std::ofstream::out);
    ss << "FStringStream file test\n";
    file_stream << ss.str();

    if ( file_stream.is_open() )
      file_stream.close();
  }

  std::string line{};
  std::ifstream file_stream{filename};

  if ( ! file_stream.eof() && file_stream.good() )
  {
    getline(file_stream, line);
    CPPUNIT_ASSERT ( line == "FStringStream file test" );
  }

  if ( file_stream.is_open() )
    file_stream.close();

  int ret = remove(filename.c_str());  // Delete file

  if ( ret == -1 )
  {
    std::cerr << "Cannot delete the " << filename << " file";
  }
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FStringStreamTest);

// The general unit test main part
#include <main-test.inc>

