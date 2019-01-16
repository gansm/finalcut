/***********************************************************************
* fsize-test.cpp - FSize unit tests                                    *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2019 Markus Gans                                           *
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

#include <limits>

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <final/final.h>

//----------------------------------------------------------------------
// class FSizeTest
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FSizeTest : public CPPUNIT_NS::TestFixture
{
  public:
    FSizeTest()
    { }

  protected:
    void classNameTest();
    void noArgumentTest();
    void copyConstructorTest();
    void assignmentTest();
    void additionAssignmentTest();
    void subtractionAssignmentTest();
    void equalTest();
    void notEqualTest();
    void additionTest();
    void subtractionTest();
    void referenceTest();
    void streamInsertionTest();
    void streamExtractionTest();

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FSizeTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (noArgumentTest);
    CPPUNIT_TEST (copyConstructorTest);
    CPPUNIT_TEST (assignmentTest);
    CPPUNIT_TEST (additionAssignmentTest);
    CPPUNIT_TEST (subtractionAssignmentTest);
    CPPUNIT_TEST (equalTest);
    CPPUNIT_TEST (notEqualTest);
    CPPUNIT_TEST (additionTest);
    CPPUNIT_TEST (subtractionTest);
    CPPUNIT_TEST (referenceTest);
    CPPUNIT_TEST (streamInsertionTest);
    CPPUNIT_TEST (streamExtractionTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};
#pragma pack(pop)

//----------------------------------------------------------------------
void FSizeTest::classNameTest()
{
  finalcut::FSize p;
  const char* const classname = p.getClassName();
  CPPUNIT_ASSERT ( std::strcmp(classname, "FSize") == 0 );
}

//----------------------------------------------------------------------
void FSizeTest::noArgumentTest()
{
  const finalcut::FSize size{};
  CPPUNIT_ASSERT ( size.getWidth() == 0 );
  CPPUNIT_ASSERT ( size.getHeight() == 0 );
  CPPUNIT_ASSERT ( size.isEmpty() );
}

//----------------------------------------------------------------------
void FSizeTest::copyConstructorTest()
{
  const finalcut::FSize s1 (333, 80);
  const finalcut::FSize s2 (s1);
  CPPUNIT_ASSERT ( s2.getWidth() == 333 );
  CPPUNIT_ASSERT ( s2.getHeight() == 80 );
}

//----------------------------------------------------------------------
void FSizeTest::assignmentTest()
{
  const finalcut::FSize s1 (0,100);
  CPPUNIT_ASSERT ( s1.getWidth() == 0 );
  CPPUNIT_ASSERT ( s1.getHeight() == 100 );

  finalcut::FSize s2;
  s2 = s1;
  CPPUNIT_ASSERT ( s2.getWidth() == 0 );
  CPPUNIT_ASSERT ( s2.getHeight() == 100 );

  s2.setSize (80, 24);
  CPPUNIT_ASSERT ( s2.getWidth() == 80 );
  CPPUNIT_ASSERT ( s2.getHeight() == 24 );

  s2.setWidth(40);
  CPPUNIT_ASSERT ( s2.getWidth() == 40 );
  CPPUNIT_ASSERT ( s2.getHeight() == 24 );

  s2.setHeight(12);
  CPPUNIT_ASSERT ( s2.getWidth() == 40 );
  CPPUNIT_ASSERT ( s2.getHeight() == 12 );

  // Value limit
  const finalcut::FSize s3 ( std::numeric_limits<std::size_t>::min()
                           , std::numeric_limits<std::size_t>::max() );
  CPPUNIT_ASSERT ( s3.getWidth() == std::numeric_limits<std::size_t>::min() );
  CPPUNIT_ASSERT ( s3.getHeight() == std::numeric_limits<std::size_t>::max() );
}

//----------------------------------------------------------------------
void FSizeTest::additionAssignmentTest()
{
  finalcut::FSize s1 (1,2);
  s1 += finalcut::FSize{3,1};
  CPPUNIT_ASSERT ( s1.getWidth() == 4 );
  CPPUNIT_ASSERT ( s1.getHeight() == 3 );

  s1 += finalcut::FSize{0, 0};
  CPPUNIT_ASSERT ( s1.getWidth() == 4 );
  CPPUNIT_ASSERT ( s1.getHeight() == 3 );

  s1 += finalcut::FSize{1, 2};
  CPPUNIT_ASSERT ( s1.getWidth() == 5 );
  CPPUNIT_ASSERT ( s1.getHeight() == 5 );
  CPPUNIT_ASSERT ( ! s1.isEmpty() );

  // Value limit
  finalcut::FSize s2 ( std::numeric_limits<std::size_t>::max()
                     , std::numeric_limits<std::size_t>::min() );
  CPPUNIT_ASSERT ( s2.getWidth() == std::numeric_limits<std::size_t>::max() );
  CPPUNIT_ASSERT ( s2.getHeight() == std::numeric_limits<std::size_t>::min() );
  s2 += finalcut::FSize{1, 1};
  CPPUNIT_ASSERT ( s2.getWidth() == std::numeric_limits<std::size_t>::max() );
  CPPUNIT_ASSERT ( s2.getHeight() == 1 );
}

//----------------------------------------------------------------------
void FSizeTest::subtractionAssignmentTest()
{
  finalcut::FSize s1 (10, 20);
  s1 -= finalcut::FSize (2, 5);
  CPPUNIT_ASSERT ( s1.getWidth() == 8 );
  CPPUNIT_ASSERT ( s1.getHeight() == 15 );

  s1 -= finalcut::FSize (1, 0);
  CPPUNIT_ASSERT ( s1.getWidth() == 7 );
  CPPUNIT_ASSERT ( s1.getHeight() == 15 );
  CPPUNIT_ASSERT ( ! s1.isEmpty() );

  s1 -= finalcut::FSize (0, 5);

  CPPUNIT_ASSERT ( s1.getWidth() == 7 );
  CPPUNIT_ASSERT ( s1.getHeight() == 10 );
  CPPUNIT_ASSERT ( ! s1.isEmpty() );

  s1 -= finalcut::FSize (2, 222);
  CPPUNIT_ASSERT ( s1.getWidth() == 5 );
  CPPUNIT_ASSERT ( s1.getHeight() == 0 );
  CPPUNIT_ASSERT ( ! s1.isEmpty() );

  s1 -= finalcut::FSize (1, 0);
  CPPUNIT_ASSERT ( s1.getWidth() == 4 );
  CPPUNIT_ASSERT ( s1.getHeight() == 0 );
  CPPUNIT_ASSERT ( ! s1.isEmpty() );

  s1 -= (finalcut::FSize (3, 0) + finalcut::FSize (1, 0));
  CPPUNIT_ASSERT ( s1.getWidth() == 0 );
  CPPUNIT_ASSERT ( s1.getHeight() == 0 );
  CPPUNIT_ASSERT ( s1.isEmpty() );

  // Value limit
  finalcut::FSize s2 ( std::numeric_limits<std::size_t>::max()
                     , std::numeric_limits<std::size_t>::min() );
  CPPUNIT_ASSERT ( s2.getWidth() == std::numeric_limits<std::size_t>::max() );
  CPPUNIT_ASSERT ( s2.getHeight() == std::numeric_limits<std::size_t>::min() );
  CPPUNIT_ASSERT ( ! s2.isEmpty() );
  s2 -= finalcut::FSize ( std::numeric_limits<std::size_t>::max(),
                          std::numeric_limits<std::size_t>::min() );
  CPPUNIT_ASSERT ( s2.getWidth() == 0 );
  CPPUNIT_ASSERT ( s2.getHeight() == 0 );
  CPPUNIT_ASSERT ( s2.isEmpty() );
  s2 -= finalcut::FSize(10, 10);
  CPPUNIT_ASSERT ( s2.getWidth() == 0 );
  CPPUNIT_ASSERT ( s2.getHeight() == 0 );
  CPPUNIT_ASSERT ( s2.isEmpty() );
}

//----------------------------------------------------------------------
void FSizeTest::equalTest()
{
  const finalcut::FSize s1 (1,2);
  const finalcut::FSize s2 (1,2);
  CPPUNIT_ASSERT ( s1 == s2 );
  CPPUNIT_ASSERT ( finalcut::FSize(1,2) == s2 );
  CPPUNIT_ASSERT ( s1 == finalcut::FSize(1,2) );
  const finalcut::FSize s3{};
  const finalcut::FSize s4{};
  CPPUNIT_ASSERT ( s3 == s4 );
}

//----------------------------------------------------------------------
void FSizeTest::notEqualTest()
{
  const finalcut::FSize s1 (3,5);
  const finalcut::FSize s2 (2,4);
  CPPUNIT_ASSERT ( s1 != s2 );
  CPPUNIT_ASSERT ( finalcut::FSize(1,2) != s2 );
  CPPUNIT_ASSERT ( s1 != finalcut::FSize(2,4) );
  CPPUNIT_ASSERT ( finalcut::FSize() != s2 );
  CPPUNIT_ASSERT ( s1 != finalcut::FSize() );
}

//----------------------------------------------------------------------
void FSizeTest::additionTest()
{
  const finalcut::FSize s1 (1 ,2);
  const finalcut::FSize s2 (5 ,8);
  const finalcut::FSize s3 = s1 + s2;
  CPPUNIT_ASSERT ( s3.getWidth() == 6 );
  CPPUNIT_ASSERT ( s3.getHeight() == 10 );
  CPPUNIT_ASSERT ( s1 + s2 == finalcut::FSize(6 ,10) );
  CPPUNIT_ASSERT ( s1 + finalcut::FSize() == s1 );
  CPPUNIT_ASSERT ( finalcut::FSize() + s2 == s2 );
  CPPUNIT_ASSERT ( finalcut::FSize() + finalcut::FSize() == finalcut::FSize() );
}

//----------------------------------------------------------------------
void FSizeTest::subtractionTest()
{
  const finalcut::FSize s1 (100, 20);
  const finalcut::FSize s2 (10, 3);
  const finalcut::FSize s3 = s1 - s2;
  CPPUNIT_ASSERT ( s3.getWidth() == 90 );
  CPPUNIT_ASSERT ( s3.getHeight() == 17 );
  CPPUNIT_ASSERT ( s1 - s2 == finalcut::FSize(90, 17) );
  CPPUNIT_ASSERT ( s1 - finalcut::FSize() == s1 );
  CPPUNIT_ASSERT ( finalcut::FSize() - finalcut::FSize() == finalcut::FSize() );
  CPPUNIT_ASSERT ( s3 - finalcut::FSize(100, 20) == finalcut::FSize(0, 0) );
}

//----------------------------------------------------------------------
void FSizeTest::referenceTest()
{
  finalcut::FSize s1 (1,1);
  CPPUNIT_ASSERT ( s1.getWidth() == 1 );
  CPPUNIT_ASSERT ( s1.getHeight() == 1 );

  s1.width_ref()++;
  s1.height_ref()++;
  CPPUNIT_ASSERT ( s1.getWidth() == 2 );
  CPPUNIT_ASSERT ( s1.getHeight() == 2 );

  std::size_t& width = s1.width_ref();
  std::size_t& height = s1.height_ref();
  width += 4;
  height += 2;
  CPPUNIT_ASSERT ( s1.getWidth() == 6 );
  CPPUNIT_ASSERT ( s1.getHeight() == 4 );
}

//----------------------------------------------------------------------
void FSizeTest::streamInsertionTest()
{
  finalcut::FSize out;
  std::stringstream stream;
  stream.str("10 5");
  stream >> out;
  CPPUNIT_ASSERT ( out.getWidth() == 10 );
  CPPUNIT_ASSERT ( out.getHeight() == 5 );

  stream.clear();
  stream.str("0 9");
  stream >> out;
  CPPUNIT_ASSERT ( out.getWidth() == 0 );
  CPPUNIT_ASSERT ( out.getHeight() == 9 );
}

//----------------------------------------------------------------------
void FSizeTest::streamExtractionTest()
{
  finalcut::FSize in;
  in.setSize (7, 5);
  std::stringstream stream;
  stream << in;
  CPPUNIT_ASSERT ( stream.str() == "7 5" );

  in.setSize (127, 150);
  stream.clear();
  stream.str("");
  stream << in;
  CPPUNIT_ASSERT ( stream.str() == "127 150" );
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FSizeTest);

// The general unit test main part
#include <main-test.inc>
