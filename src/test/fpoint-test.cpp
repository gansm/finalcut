/***********************************************************************
* fpoint-test.cpp - FPoint unit tests                                  *
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

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <final/final.h>

//----------------------------------------------------------------------
// class FPointTest
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FPointTest : public CPPUNIT_NS::TestFixture
{
  public:
    FPointTest()
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

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FPointTest);

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

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};
#pragma pack(pop)

//----------------------------------------------------------------------
void FPointTest::classNameTest()
{
  FPoint p;
  const char* const classname = p.getClassName();
  CPPUNIT_ASSERT ( std::strcmp(classname, "FPoint") == 0 );
}

//----------------------------------------------------------------------
void FPointTest::noArgumentTest()
{
  const FPoint point;
  CPPUNIT_ASSERT ( point.getX() == 0 );
  CPPUNIT_ASSERT ( point.getY() == 0 );
  CPPUNIT_ASSERT ( point.isNull() );
}

//----------------------------------------------------------------------
void FPointTest::copyConstructorTest()
{
  const FPoint p1 (15,10);
  const FPoint p2 (p1);
  CPPUNIT_ASSERT ( p2.getX() == 15 );
  CPPUNIT_ASSERT ( p2.getY() == 10 );
}

//----------------------------------------------------------------------
void FPointTest::assignmentTest()
{
  const FPoint p1 (-99,100);
  CPPUNIT_ASSERT ( p1.getX() == -99 );
  CPPUNIT_ASSERT ( p1.getY() == 100 );

  FPoint p2;
  p2 = p1;
  CPPUNIT_ASSERT ( p2.getX() == -99 );
  CPPUNIT_ASSERT ( p2.getY() == 100 );

  p2 = -p1;
  CPPUNIT_ASSERT ( p2.getX() == 99 );
  CPPUNIT_ASSERT ( p2.getY() == -100 );

  p2.setPoint (80, 24);
  CPPUNIT_ASSERT ( p2.getX() == 80 );
  CPPUNIT_ASSERT ( p2.getY() == 24 );

  p2.setX(40);
  CPPUNIT_ASSERT ( p2.getX() == 40 );
  CPPUNIT_ASSERT ( p2.getY() == 24 );

  p2.setY(12);
  CPPUNIT_ASSERT ( p2.getX() == 40 );
  CPPUNIT_ASSERT ( p2.getY() == 12 );

  // Value limit exceeded
  const FPoint p3 (-999999,1000000);
  CPPUNIT_ASSERT ( p3.getX() != -999999 );
  CPPUNIT_ASSERT ( p3.getY() != 1000000 );
}

//----------------------------------------------------------------------
void FPointTest::additionAssignmentTest()
{
  FPoint p1 (1,2);
  p1 += FPoint (3,1);
  CPPUNIT_ASSERT ( p1.getX() == 4 );
  CPPUNIT_ASSERT ( p1.getY() == 3 );

  p1 += FPoint (-4,-3);
  CPPUNIT_ASSERT ( p1.getX() == 0 );
  CPPUNIT_ASSERT ( p1.getY() == 0 );
  CPPUNIT_ASSERT ( p1.isNull() );

  // Value limit exceeded
  FPoint p2 (18000,-18000);
  CPPUNIT_ASSERT ( p2.getX() == 18000 );
  CPPUNIT_ASSERT ( p2.getY() == -18000 );
  p2 += FPoint (18000,-18000);
  CPPUNIT_ASSERT ( p2.getX() != 36000 );
  CPPUNIT_ASSERT ( p2.getY() != -36000 );
}

//----------------------------------------------------------------------
void FPointTest::subtractionAssignmentTest()
{
  FPoint p1 (10,20);
  p1 -= FPoint (5,5);
  CPPUNIT_ASSERT ( p1.getX() == 5 );
  CPPUNIT_ASSERT ( p1.getY() == 15 );

  p1 -= FPoint (-5,20);
  CPPUNIT_ASSERT ( p1.getX() == 10 );
  CPPUNIT_ASSERT ( p1.getY() == -5 );
  CPPUNIT_ASSERT ( ! p1.isNull() );

  p1 -= FPoint (-10,0);
  CPPUNIT_ASSERT ( p1.getX() == 20 );
  CPPUNIT_ASSERT ( p1.getY() == -5 );
  CPPUNIT_ASSERT ( ! p1.isNull() );

  p1 -= FPoint (20,0);
  CPPUNIT_ASSERT ( p1.getX() == 0 );
  CPPUNIT_ASSERT ( p1.getY() == -5 );
  CPPUNIT_ASSERT ( ! p1.isNull() );

  p1 -= FPoint (0,-6);
  CPPUNIT_ASSERT ( p1.getX() == 0 );
  CPPUNIT_ASSERT ( p1.getY() == 1 );
  CPPUNIT_ASSERT ( ! p1.isNull() );

  p1 -= FPoint (1,0);
  CPPUNIT_ASSERT ( p1.getX() == -1 );
  CPPUNIT_ASSERT ( p1.getY() == 1 );
  CPPUNIT_ASSERT ( ! p1.isNull() );

  p1 -= (FPoint (0,1) + FPoint (-1,0));
  CPPUNIT_ASSERT ( p1.getX() == 0 );
  CPPUNIT_ASSERT ( p1.getY() == 0 );
  CPPUNIT_ASSERT ( p1.isNull() );

  // Value limit exceeded
  FPoint p2 (18000,-18000);
  CPPUNIT_ASSERT ( p2.getX() == 18000 );
  CPPUNIT_ASSERT ( p2.getY() == -18000 );
  p2 += FPoint (18000,-18000);
  CPPUNIT_ASSERT ( p2.getX() != 36000 );
  CPPUNIT_ASSERT ( p2.getY() != -36000 );
}

//----------------------------------------------------------------------
void FPointTest::equalTest()
{
  const FPoint p1 (1,2);
  const FPoint p2 (1,2);
  CPPUNIT_ASSERT ( p1 == p2 );
  CPPUNIT_ASSERT ( FPoint(1,2) == p2 );
  CPPUNIT_ASSERT ( p1 == FPoint(1,2) );
  CPPUNIT_ASSERT ( FPoint() == FPoint() );
  CPPUNIT_ASSERT ( FPoint() == -FPoint() );
}

//----------------------------------------------------------------------
void FPointTest::notEqualTest()
{
  const FPoint p1 (1,2);
  const FPoint p2 (2,4);
  CPPUNIT_ASSERT ( p1 != p2 );
  CPPUNIT_ASSERT ( FPoint(1,2) != p2 );
  CPPUNIT_ASSERT ( p1 != FPoint(2,4) );
  CPPUNIT_ASSERT ( FPoint() != p2 );
  CPPUNIT_ASSERT ( p1 != FPoint() );
}

//----------------------------------------------------------------------
void FPointTest::additionTest()
{
  const FPoint p1 (1,2);
  const FPoint p2 (5,8);
  const FPoint p3 = p1 + p2;
  CPPUNIT_ASSERT ( p3.getX() == 6 );
  CPPUNIT_ASSERT ( p3.getY() == 10 );
  CPPUNIT_ASSERT ( p1 + p2 == FPoint(6,10) );
  CPPUNIT_ASSERT ( p1 + FPoint() == p1 );
  CPPUNIT_ASSERT ( FPoint() + p2 == p2 );
  CPPUNIT_ASSERT ( FPoint() + FPoint() == FPoint() );
}

//----------------------------------------------------------------------
void FPointTest::subtractionTest()
{
  const FPoint p1 (-5,-20);
  const FPoint p2 (0,-30);
  const FPoint p3 = p1 - p2;
  CPPUNIT_ASSERT ( p3.getX() == -5 );
  CPPUNIT_ASSERT ( p3.getY() == 10 );
  CPPUNIT_ASSERT ( p1 - p2 == FPoint(-5,10) );
  CPPUNIT_ASSERT ( p1 - FPoint() == p1 );
  CPPUNIT_ASSERT ( FPoint() - FPoint() == FPoint() );
}

//----------------------------------------------------------------------
void FPointTest::referenceTest()
{
  FPoint p1 (1,1);
  CPPUNIT_ASSERT ( p1.getX() == 1 );
  CPPUNIT_ASSERT ( p1.getY() == 1 );

  p1.x_ref()++;
  p1.y_ref()++;
  CPPUNIT_ASSERT ( p1.getX() == 2 );
  CPPUNIT_ASSERT ( p1.getY() == 2 );

  short& x = p1.x_ref();
  short& y = p1.y_ref();
  x += 4;
  y += 2;
  CPPUNIT_ASSERT ( p1.getX() == 6 );
  CPPUNIT_ASSERT ( p1.getY() == 4 );
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FPointTest);

// The general unit test main part
#include <main-test.inc>
