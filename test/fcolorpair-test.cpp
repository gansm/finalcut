/***********************************************************************
* fcolorpair-test.cpp - FColorPair unit tests                          *
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
// class FColorPairTest
//----------------------------------------------------------------------

class FColorPairTest : public CPPUNIT_NS::TestFixture
{
  public:
    FColorPairTest()
    { }

  protected:
    void classNameTest();
    void noArgumentTest();
    void copyConstructorTest();
    void assignmentTest();
    void setColorTest();
    void swapTest();

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FColorPairTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (noArgumentTest);
    CPPUNIT_TEST (copyConstructorTest);
    CPPUNIT_TEST (assignmentTest);
    CPPUNIT_TEST (setColorTest);
    CPPUNIT_TEST (swapTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};


//----------------------------------------------------------------------
void FColorPairTest::classNameTest()
{
  const finalcut::FColorPair p;
  const finalcut::FString& classname = p.getClassName();
  CPPUNIT_ASSERT ( classname == "FColorPair" );
}

//----------------------------------------------------------------------
void FColorPairTest::noArgumentTest()
{
  const finalcut::FColorPair pair{};
  CPPUNIT_ASSERT ( pair.getForegroundColor() == finalcut::fc::Default );
  CPPUNIT_ASSERT ( pair.getBackgroundColor() == finalcut::fc::Default );
}

//----------------------------------------------------------------------
void FColorPairTest::copyConstructorTest()
{
  const finalcut::FColorPair p1a;
  finalcut::FColorPair p1b (p1a);
  CPPUNIT_ASSERT ( p1b.getForegroundColor() == finalcut::fc::Default );
  CPPUNIT_ASSERT ( p1b.getBackgroundColor() == finalcut::fc::Default );

  const finalcut::FColorPair p2a (finalcut::fc::Yellow );
  const finalcut::FColorPair p2b (p2a);
  CPPUNIT_ASSERT ( p2b.getForegroundColor() == finalcut::fc::Yellow );
  CPPUNIT_ASSERT ( p2b.getBackgroundColor() == finalcut::fc::Default );

  const finalcut::FColorPair p3a (finalcut::fc::Red, finalcut::fc::Black);
  const finalcut::FColorPair p3b (p3a);
  CPPUNIT_ASSERT ( p3b.getForegroundColor() == finalcut::fc::Red );
  CPPUNIT_ASSERT ( p3b.getBackgroundColor() == finalcut::fc::Black );
}

//----------------------------------------------------------------------
void FColorPairTest::assignmentTest()
{
  const finalcut::FColorPair p1a;
  const finalcut::FColorPair p1b = p1a;
  CPPUNIT_ASSERT ( p1b.getForegroundColor() == finalcut::fc::Default );
  CPPUNIT_ASSERT ( p1b.getBackgroundColor() == finalcut::fc::Default );

  const finalcut::FColorPair p2a (finalcut::fc::Yellow );
  const finalcut::FColorPair p2b = p2a;
  CPPUNIT_ASSERT ( p2b.getForegroundColor() == finalcut::fc::Yellow );
  CPPUNIT_ASSERT ( p2b.getBackgroundColor() == finalcut::fc::Default );

  const finalcut::FColorPair p3a (finalcut::fc::Red, finalcut::fc::Black);
  const finalcut::FColorPair p3b = p3a;
  CPPUNIT_ASSERT ( p3b.getForegroundColor() == finalcut::fc::Red );
  CPPUNIT_ASSERT ( p3b.getBackgroundColor() == finalcut::fc::Black );
}

//----------------------------------------------------------------------
void FColorPairTest::setColorTest()
{
  finalcut::FColorPair p1;
  p1.setForegroundColor (finalcut::fc::DarkGreen);
  CPPUNIT_ASSERT ( p1.getForegroundColor() == finalcut::fc::DarkGreen );
  CPPUNIT_ASSERT ( p1.getBackgroundColor() == finalcut::fc::Default );

  finalcut::FColorPair p2 (finalcut::fc::Yellow);
  p2.setBackgroundColor (finalcut::fc::DeepSkyBlue1);
  CPPUNIT_ASSERT ( p2.getForegroundColor() == finalcut::fc::Yellow );
  CPPUNIT_ASSERT ( p2.getBackgroundColor() == finalcut::fc::DeepSkyBlue1 );

  finalcut::FColorPair p3 (finalcut::fc::Red, finalcut::fc::Black);
  p3.setColorPair (finalcut::fc::NavyBlue, finalcut::fc::SandyBrown);
  CPPUNIT_ASSERT ( p3.getForegroundColor() == finalcut::fc::NavyBlue );
  CPPUNIT_ASSERT ( p3.getBackgroundColor() == finalcut::fc::SandyBrown );
  finalcut::FColorPair p4 (finalcut::fc::DarkBlue, finalcut::fc::Grey66);
  p3.setColorPair (p4);
  CPPUNIT_ASSERT ( p3.getForegroundColor() == finalcut::fc::DarkBlue );
  CPPUNIT_ASSERT ( p3.getBackgroundColor() == finalcut::fc::Grey66 );
}

//----------------------------------------------------------------------
void FColorPairTest::swapTest()
{
  finalcut::FColorPair p1;
  p1.swap();
  CPPUNIT_ASSERT ( p1.getForegroundColor() == finalcut::fc::Default );
  CPPUNIT_ASSERT ( p1.getBackgroundColor() == finalcut::fc::Default );

  finalcut::FColorPair p2 (finalcut::fc::LightBlue );
  p2.swap();
  CPPUNIT_ASSERT ( p2.getForegroundColor() == finalcut::fc::Default );
  CPPUNIT_ASSERT ( p2.getBackgroundColor() == finalcut::fc::LightBlue );

  finalcut::FColorPair p3 (finalcut::fc::Cyan, finalcut::fc::White);
  p3.swap();
  CPPUNIT_ASSERT ( p3.getForegroundColor() == finalcut::fc::White );
  CPPUNIT_ASSERT ( p3.getBackgroundColor() == finalcut::fc::Cyan );
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FColorPairTest);

// The general unit test main part
#include <main-test.inc>
