/***********************************************************************
* frect-test.cpp - FRect unit tests                                    *
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
// class FRectTest
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FRectTest : public CPPUNIT_NS::TestFixture
{
  public:
    FRectTest()
    { }

  protected:
    void classNameTest();
    void noArgumentTest();
    void copyConstructorTest();
    void assignmentTest();
    void equalTest();
    void notEqualTest();
    void additionTest();
    void subtractionTest();
    void referenceTest();
    void moveTest();
    void containsTest();
    void overlapTest();
    void intersectTest();
    void combinedTest();
    void streamInsertionTest();
    void streamExtractionTest();

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FRectTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (noArgumentTest);
    CPPUNIT_TEST (copyConstructorTest);
    CPPUNIT_TEST (assignmentTest);
    CPPUNIT_TEST (equalTest);
    CPPUNIT_TEST (notEqualTest);
    CPPUNIT_TEST (additionTest);
    CPPUNIT_TEST (subtractionTest);
    CPPUNIT_TEST (referenceTest);
    CPPUNIT_TEST (moveTest);
    CPPUNIT_TEST (containsTest);
    CPPUNIT_TEST (overlapTest);
    CPPUNIT_TEST (intersectTest);
    CPPUNIT_TEST (combinedTest);
    CPPUNIT_TEST (streamInsertionTest);
    CPPUNIT_TEST (streamExtractionTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};
#pragma pack(pop)

//----------------------------------------------------------------------
void FRectTest::classNameTest()
{
  finalcut::FRect r;
  const char* const classname = r.getClassName();
  CPPUNIT_ASSERT ( std::strcmp(classname, "FRect") == 0 );
}

//----------------------------------------------------------------------
void FRectTest::noArgumentTest()
{
  const finalcut::FRect rectangle;
  CPPUNIT_ASSERT ( rectangle.getX1() == 0 );
  CPPUNIT_ASSERT ( rectangle.getY1() == 0 );
  CPPUNIT_ASSERT ( rectangle.getX2() == -1 );
  CPPUNIT_ASSERT ( rectangle.getY2() == -1 );
  CPPUNIT_ASSERT ( rectangle.isNull() );
  CPPUNIT_ASSERT ( rectangle.getWidth() == 0 );
  CPPUNIT_ASSERT ( rectangle.getHeight() == 0 );
  CPPUNIT_ASSERT ( rectangle.getPos() == finalcut::FPoint(0, 0) );
  CPPUNIT_ASSERT ( rectangle.getUpperLeftPos() == finalcut::FPoint(0, 0) );
  CPPUNIT_ASSERT ( rectangle.getUpperRightPos() == finalcut::FPoint(-1, 0) );
  CPPUNIT_ASSERT ( rectangle.getLowerLeftPos() == finalcut::FPoint(0, -1) );
  CPPUNIT_ASSERT ( rectangle.getLowerRightPos() == finalcut::FPoint(-1, -1) );
}

//----------------------------------------------------------------------
void FRectTest::copyConstructorTest()
{
  const finalcut::FRect r1(1, 1, 20, 10);
  const finalcut::FRect r2 (r1);
  CPPUNIT_ASSERT ( r2.getX() == 1 );
  CPPUNIT_ASSERT ( r2.getY() == 1 );
  CPPUNIT_ASSERT ( ! r2.isNull() );
  CPPUNIT_ASSERT ( r2.getWidth() == 20 );
  CPPUNIT_ASSERT ( r2.getHeight() == 10 );
}

//----------------------------------------------------------------------
void FRectTest::assignmentTest()
{
  const finalcut::FRect r1(3, 5, 45, 14);
  CPPUNIT_ASSERT ( r1.getX1() == 3 );
  CPPUNIT_ASSERT ( r1.getY1() == 5 );
  CPPUNIT_ASSERT ( r1.getX2() == 47 );
  CPPUNIT_ASSERT ( r1.getY2() == 18 );
  CPPUNIT_ASSERT ( r1.getWidth() == 45 );
  CPPUNIT_ASSERT ( r1.getHeight() == 14 );

  finalcut::FRect r2 (r1);
  CPPUNIT_ASSERT ( r2 == r1 );
  CPPUNIT_ASSERT ( r2.getX1() == 3 );
  CPPUNIT_ASSERT ( r2.getY1() == 5 );
  CPPUNIT_ASSERT ( r2.getX2() == 47 );
  CPPUNIT_ASSERT ( r2.getY2() == 18 );
  CPPUNIT_ASSERT ( r2.getWidth() == 45 );
  CPPUNIT_ASSERT ( r2.getHeight() == 14 );

  finalcut::FRect r3(3, 3, 10, 10);
  r3 = r2;
  CPPUNIT_ASSERT ( r3 == r2 );
  CPPUNIT_ASSERT ( r3.getX1() == 3 );
  CPPUNIT_ASSERT ( r3.getY1() == 5 );
  CPPUNIT_ASSERT ( r3.getX2() == 47 );
  CPPUNIT_ASSERT ( r3.getY2() == 18 );
  CPPUNIT_ASSERT ( r3.getWidth() == 45 );
  CPPUNIT_ASSERT ( r3.getHeight() == 14 );

  r3.setPos(finalcut::FPoint(1, 1));
  CPPUNIT_ASSERT ( r3 != r2 );
  CPPUNIT_ASSERT ( r3.getX1() == 1 );
  CPPUNIT_ASSERT ( r3.getY1() == 1 );
  CPPUNIT_ASSERT ( r3.getX2() == 45 );
  CPPUNIT_ASSERT ( r3.getY2() == 14 );
  CPPUNIT_ASSERT ( r3.getWidth() == 45 );
  CPPUNIT_ASSERT ( r3.getHeight() == 14 );

  r3.setPos(-5, -5);
  CPPUNIT_ASSERT ( r3 != r2 );
  CPPUNIT_ASSERT ( r3.getX1() == -5 );
  CPPUNIT_ASSERT ( r3.getY1() == -5 );
  CPPUNIT_ASSERT ( r3.getX2() == 39 );
  CPPUNIT_ASSERT ( r3.getY2() == 8 );
  CPPUNIT_ASSERT ( r3.getWidth() == 45 );
  CPPUNIT_ASSERT ( r3.getHeight() == 14 );

  r3.setRect(-3, -3, 6, 6);
  CPPUNIT_ASSERT ( r3.getX1() == -3 );
  CPPUNIT_ASSERT ( r3.getY1() == -3 );
  CPPUNIT_ASSERT ( r3.getX2() == 2 );
  CPPUNIT_ASSERT ( r3.getY2() == 2 );
  CPPUNIT_ASSERT ( r3.getWidth() == 6 );
  CPPUNIT_ASSERT ( r3.getHeight() == 6 );

  r3.setRect(r1);
  CPPUNIT_ASSERT ( r3 == r1 );
  CPPUNIT_ASSERT ( r3.getX1() == 3 );
  CPPUNIT_ASSERT ( r3.getY1() == 5 );
  CPPUNIT_ASSERT ( r3.getX2() == 47 );
  CPPUNIT_ASSERT ( r3.getY2() == 18 );
  CPPUNIT_ASSERT ( r3.getWidth() == 45 );
  CPPUNIT_ASSERT ( r3.getHeight() == 14 );

  r3.setX1(1);
  CPPUNIT_ASSERT ( r3 != r1 );
  CPPUNIT_ASSERT ( r3.getX1() == 1 );
  CPPUNIT_ASSERT ( r3.getY1() == 5 );
  CPPUNIT_ASSERT ( r3.getX2() == 47 );
  CPPUNIT_ASSERT ( r3.getY2() == 18 );
  CPPUNIT_ASSERT ( r3.getWidth() == 47 );
  CPPUNIT_ASSERT ( r3.getHeight() == 14 );

  r3.setY1(1);
  CPPUNIT_ASSERT ( r3.getX1() == 1 );
  CPPUNIT_ASSERT ( r3.getY1() == 1 );
  CPPUNIT_ASSERT ( r3.getX2() == 47 );
  CPPUNIT_ASSERT ( r3.getY2() == 18 );
  CPPUNIT_ASSERT ( r3.getWidth() == 47 );
  CPPUNIT_ASSERT ( r3.getHeight() == 18 );

  r3.setX2(10);
  CPPUNIT_ASSERT ( r3.getX1() == 1 );
  CPPUNIT_ASSERT ( r3.getY1() == 1 );
  CPPUNIT_ASSERT ( r3.getX2() == 10 );
  CPPUNIT_ASSERT ( r3.getY2() == 18 );
  CPPUNIT_ASSERT ( r3.getWidth() == 10 );
  CPPUNIT_ASSERT ( r3.getHeight() == 18 );

  r3.setY2(10);
  CPPUNIT_ASSERT ( r3.getX1() == 1 );
  CPPUNIT_ASSERT ( r3.getY1() == 1 );
  CPPUNIT_ASSERT ( r3.getX2() == 10 );
  CPPUNIT_ASSERT ( r3.getY2() == 10 );
  CPPUNIT_ASSERT ( r3.getWidth() == 10 );
  CPPUNIT_ASSERT ( r3.getHeight() == 10 );

  r3.setX(2);
  CPPUNIT_ASSERT ( r3.getX1() == 2 );
  CPPUNIT_ASSERT ( r3.getY1() == 1 );
  CPPUNIT_ASSERT ( r3.getX2() == 11 );
  CPPUNIT_ASSERT ( r3.getY2() == 10 );
  CPPUNIT_ASSERT ( r3.getWidth() == 10 );
  CPPUNIT_ASSERT ( r3.getHeight() == 10 );

  r3.setY(2);
  CPPUNIT_ASSERT ( r3.getX1() == 2 );
  CPPUNIT_ASSERT ( r3.getY1() == 2 );
  CPPUNIT_ASSERT ( r3.getX2() == 11 );
  CPPUNIT_ASSERT ( r3.getY2() == 11 );
  CPPUNIT_ASSERT ( r3.getWidth() == 10 );
  CPPUNIT_ASSERT ( r3.getHeight() == 10 );

  r3.setWidth(8);
  CPPUNIT_ASSERT ( r3.getX1() == 2 );
  CPPUNIT_ASSERT ( r3.getY1() == 2 );
  CPPUNIT_ASSERT ( r3.getX2() == 9 );
  CPPUNIT_ASSERT ( r3.getY2() == 11 );
  CPPUNIT_ASSERT ( r3.getWidth() == 8 );
  CPPUNIT_ASSERT ( r3.getHeight() == 10 );

  r3.setHeight(8);
  CPPUNIT_ASSERT ( r3.getX1() == 2 );
  CPPUNIT_ASSERT ( r3.getY1() == 2 );
  CPPUNIT_ASSERT ( r3.getX2() == 9 );
  CPPUNIT_ASSERT ( r3.getY2() == 9 );
  CPPUNIT_ASSERT ( r3.getWidth() == 8 );
  CPPUNIT_ASSERT ( r3.getHeight() == 8 );

  r3.setSize(5, 5);
  CPPUNIT_ASSERT ( r3.getX1() == 2 );
  CPPUNIT_ASSERT ( r3.getY1() == 2 );
  CPPUNIT_ASSERT ( r3.getX2() == 6 );
  CPPUNIT_ASSERT ( r3.getY2() == 6 );
  CPPUNIT_ASSERT ( r3.getWidth() == 5 );
  CPPUNIT_ASSERT ( r3.getHeight() == 5 );

  const finalcut::FPoint p1(3, 3);
  const finalcut::FPoint p2(30, 10);
  r3.setCoordinates (p1, p2);
  CPPUNIT_ASSERT ( r3.getX1() == 3 );
  CPPUNIT_ASSERT ( r3.getY1() == 3 );
  CPPUNIT_ASSERT ( r3.getX2() == 30 );
  CPPUNIT_ASSERT ( r3.getY2() == 10 );
  CPPUNIT_ASSERT ( r3.getWidth() == 28 );
  CPPUNIT_ASSERT ( r3.getHeight() == 8 );

  r3.setCoordinates (10, 12, 40, 50);
  CPPUNIT_ASSERT ( r3.getX1() == 10 );
  CPPUNIT_ASSERT ( r3.getY1() == 12 );
  CPPUNIT_ASSERT ( r3.getX2() == 40 );
  CPPUNIT_ASSERT ( r3.getY2() == 50 );
  CPPUNIT_ASSERT ( r3.getWidth() == 31 );
  CPPUNIT_ASSERT ( r3.getHeight() == 39 );

  finalcut::FRect r4(p1, p2);
  CPPUNIT_ASSERT ( r4.getX1() == 3 );
  CPPUNIT_ASSERT ( r4.getY1() == 3 );
  CPPUNIT_ASSERT ( r4.getX2() == 30 );
  CPPUNIT_ASSERT ( r4.getY2() == 10 );
  CPPUNIT_ASSERT ( r4.getWidth() == 28 );
  CPPUNIT_ASSERT ( r4.getHeight() == 8 );
}

//----------------------------------------------------------------------
void FRectTest::equalTest()
{
  const finalcut::FRect r1 (1, 2, 10, 20);
  const finalcut::FRect r2 (1, 2, 10, 20);
  CPPUNIT_ASSERT ( r1 == r2 );
  CPPUNIT_ASSERT ( finalcut::FRect(1, 2, 10, 20) == r2 );
  CPPUNIT_ASSERT ( r1 == finalcut::FRect(1, 2, 10, 20) );
  const finalcut::FRect r3;
  const finalcut::FRect r4;
  CPPUNIT_ASSERT ( r3 == r4 );
}

//----------------------------------------------------------------------
void FRectTest::notEqualTest()
{
  const finalcut::FRect r1 (1, 2, 10, 20);
  const finalcut::FRect r2 (1, 2, 15, 25);
  CPPUNIT_ASSERT ( r1 != r2 );
  CPPUNIT_ASSERT ( finalcut::FRect(1, 2, 10, 20) != r2 );
  CPPUNIT_ASSERT ( r1 != finalcut::FRect(3, 4, 10, 20) );
  CPPUNIT_ASSERT ( finalcut::FRect() != r2 );
  CPPUNIT_ASSERT ( r1 != finalcut::FRect() );
}

//----------------------------------------------------------------------
void FRectTest::additionTest()
{
  const finalcut::FRect r1 (1, 2, 10, 10);
  const finalcut::FPoint p (3, 5);
  const finalcut::FRect r2 = r1 + p;
  CPPUNIT_ASSERT ( r2.getX1() == 1 );
  CPPUNIT_ASSERT ( r2.getY1() == 2 );
  CPPUNIT_ASSERT ( r2.getX2() == 13 );
  CPPUNIT_ASSERT ( r2.getY2() == 16 );
  CPPUNIT_ASSERT ( r2.getWidth() == 13 );
  CPPUNIT_ASSERT ( r2.getHeight() == 15 );
}

//----------------------------------------------------------------------
void FRectTest::subtractionTest()
{
  const finalcut::FRect r1 (2, 2, 12, 12);
  const finalcut::FPoint p (5, 5);
  const finalcut::FRect r2 = r1 - p;
  CPPUNIT_ASSERT ( r2.getX1() == 2 );
  CPPUNIT_ASSERT ( r2.getY1() == 2 );
  CPPUNIT_ASSERT ( r2.getX2() == 8 );
  CPPUNIT_ASSERT ( r2.getY2() == 8 );
  CPPUNIT_ASSERT ( r2.getWidth() == 7 );
  CPPUNIT_ASSERT ( r2.getHeight() == 7 );
}

//----------------------------------------------------------------------
void FRectTest::referenceTest()
{
  finalcut::FRect r1 (1, 1, 10, 20);
  CPPUNIT_ASSERT ( r1.getX1() == 1 );
  CPPUNIT_ASSERT ( r1.getY1() == 1 );
  CPPUNIT_ASSERT ( r1.getX2() == 10 );
  CPPUNIT_ASSERT ( r1.getY2() == 20 );

  // Use references to coordinates
  r1.x1_ref()++;
  r1.y1_ref()++;
  r1.x2_ref()--;
  r1.y2_ref()--;
  CPPUNIT_ASSERT ( r1.getX1() == 2 );
  CPPUNIT_ASSERT ( r1.getY1() == 2 );
  CPPUNIT_ASSERT ( r1.getX2() == 9 );
  CPPUNIT_ASSERT ( r1.getY2() == 19 );

  int& x1 = r1.x1_ref();
  int& y1 = r1.y1_ref();
  int& x2 = r1.x2_ref();
  int& y2 = r1.y2_ref();
  x1 += 2;
  y1 -= 2;
  x2 -= 3;
  y2 += 4;
  CPPUNIT_ASSERT ( r1.getX1() == 4 );
  CPPUNIT_ASSERT ( r1.getY1() == 0 );
  CPPUNIT_ASSERT ( r1.getX2() == 6 );
  CPPUNIT_ASSERT ( r1.getY2() == 23 );
}

//----------------------------------------------------------------------
void FRectTest::moveTest()
{
  finalcut::FRect r1 (1, 2, 10, 20);
  CPPUNIT_ASSERT ( r1.getX() == 1 );
  CPPUNIT_ASSERT ( r1.getY() == 2 );
  CPPUNIT_ASSERT ( r1.getWidth() == 10 );
  CPPUNIT_ASSERT ( r1.getHeight() == 20 );
  CPPUNIT_ASSERT ( r1.getX2() == 10 );
  CPPUNIT_ASSERT ( r1.getY2() == 21 );

  const finalcut::FPoint p1 (2,3);
  r1.move(p1);
  CPPUNIT_ASSERT ( r1.getX() == 3 );
  CPPUNIT_ASSERT ( r1.getY() == 5 );
  CPPUNIT_ASSERT ( r1.getWidth() == 10 );
  CPPUNIT_ASSERT ( r1.getHeight() == 20 );
  CPPUNIT_ASSERT ( r1.getX2() == 12 );
  CPPUNIT_ASSERT ( r1.getY2() == 24 );

  r1.move (-5, -5);
  CPPUNIT_ASSERT ( r1.getX() == -2 );
  CPPUNIT_ASSERT ( r1.getY() == 0 );
  CPPUNIT_ASSERT ( r1.getWidth() == 10 );
  CPPUNIT_ASSERT ( r1.getHeight() == 20 );
  CPPUNIT_ASSERT ( r1.getX2() == 7 );
  CPPUNIT_ASSERT ( r1.getY2() == 19 );
}

//----------------------------------------------------------------------
void FRectTest::containsTest()
{
  const finalcut::FRect r1 (1, 2, 10, 20);
  CPPUNIT_ASSERT ( r1.contains(5, 5) );
  CPPUNIT_ASSERT ( ! r1.contains(0, 1) );

  const finalcut::FPoint p1 (3, 4);
  const finalcut::FPoint p2 (3, 25);
  CPPUNIT_ASSERT ( r1.contains(p1) );
  CPPUNIT_ASSERT ( ! r1.contains(p2) );

  const finalcut::FRect r2 (5, 5, 10, 20);
  const finalcut::FRect r3 (2, 3, 9, 19);
  CPPUNIT_ASSERT ( !r1.contains(r2) );
  CPPUNIT_ASSERT ( r1.contains(r3) );
}

//----------------------------------------------------------------------
void FRectTest::overlapTest()
{
  const finalcut::FRect r1 (1, 2, 10, 20);
  const finalcut::FRect r2 (5, 5, 10, 20);
  const finalcut::FRect r3 (2, 3, 9, 19);
  const finalcut::FRect r4 (10, 20, 10, 20);
  const finalcut::FRect r5 (11, 21, 10, 20);
  const finalcut::FRect r6 (-5, -3, 2, 3);
  CPPUNIT_ASSERT ( r1.overlap(r2) );
  CPPUNIT_ASSERT ( r1.overlap(r3) );
  CPPUNIT_ASSERT ( r1.overlap(r4) );
  CPPUNIT_ASSERT ( ! r1.overlap(r5) );
  CPPUNIT_ASSERT ( ! r1.overlap(r6) );
}

//----------------------------------------------------------------------
void FRectTest::intersectTest()
{
  const finalcut::FRect r1 (1, 2, 5, 5);
  const finalcut::FRect r2 (1, 2, 5, 5);
  finalcut::FRect r3 = r1.intersect(r2);
  CPPUNIT_ASSERT ( r3.getX() == 1 );
  CPPUNIT_ASSERT ( r3.getY() == 2 );
  CPPUNIT_ASSERT ( r3.getWidth() == 5 );
  CPPUNIT_ASSERT ( r3.getHeight() == 5 );
  CPPUNIT_ASSERT ( r3.getX2() == 5 );
  CPPUNIT_ASSERT ( r3.getY2() == 6 );

  const finalcut::FRect r4 (4, 2, 5, 6);
  r3 = r1.intersect(r4);
  CPPUNIT_ASSERT ( r3.getX() == 4 );
  CPPUNIT_ASSERT ( r3.getY() == 2 );
  CPPUNIT_ASSERT ( r3.getWidth() == 2 );
  CPPUNIT_ASSERT ( r3.getHeight() == 5 );
  CPPUNIT_ASSERT ( r3.getX2() == 5 );
  CPPUNIT_ASSERT ( r3.getY2() == 6 );
}

//----------------------------------------------------------------------
void FRectTest::combinedTest()
{
  const finalcut::FRect r1 (1, 2, 5, 5);
  const finalcut::FRect r2 (1, 2, 5, 5);
  finalcut::FRect r3 = r1.combined(r2);
  CPPUNIT_ASSERT ( r3.getX() == 1 );
  CPPUNIT_ASSERT ( r3.getY() == 2 );
  CPPUNIT_ASSERT ( r3.getWidth() == 5 );
  CPPUNIT_ASSERT ( r3.getHeight() == 5 );
  CPPUNIT_ASSERT ( r3.getX2() == 5 );
  CPPUNIT_ASSERT ( r3.getY2() == 6 );

  const finalcut::FRect r4 (4, 2, 5, 6);
  r3 = r1.combined(r4);
  CPPUNIT_ASSERT ( r3.getX() == 1 );
  CPPUNIT_ASSERT ( r3.getY() == 2 );
  CPPUNIT_ASSERT ( r3.getWidth() == 8 );
  CPPUNIT_ASSERT ( r3.getHeight() == 6 );
  CPPUNIT_ASSERT ( r3.getX2() == 8 );
  CPPUNIT_ASSERT ( r3.getY2() == 7 );
}

//----------------------------------------------------------------------
void FRectTest::streamInsertionTest()
{
  finalcut::FRect out;
  std::stringstream stream;
  stream.str("10 5 60 20");
  stream >> out;
  CPPUNIT_ASSERT ( out.getX1() == 10 );
  CPPUNIT_ASSERT ( out.getY1() == 5 );
  CPPUNIT_ASSERT ( out.getX2() == 60 );
  CPPUNIT_ASSERT ( out.getY2() == 20 );

  stream.clear();
  stream.str("-3 -9 5 7");
  stream >> out;
  CPPUNIT_ASSERT ( out.getX1() == -3 );
  CPPUNIT_ASSERT ( out.getY1() == -9 );
  CPPUNIT_ASSERT ( out.getX2() == 5 );
  CPPUNIT_ASSERT ( out.getY2() == 7 );
}

//----------------------------------------------------------------------
void FRectTest::streamExtractionTest()
{
  finalcut::FRect in;
  in.setCoordinates (-7, 5, -1, 10);
  std::stringstream stream;
  stream << in;
  CPPUNIT_ASSERT ( stream.str() == "-7 5 -1 10" );

  in.setCoordinates (50, 100, 127, 150);
  stream.clear();
  stream.str("");
  stream << in;
  CPPUNIT_ASSERT ( stream.str() == "50 100 127 150" );
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FRectTest);

// The general unit test main part
#include <main-test.inc>

