/***********************************************************************
* fcolorpair-test.cpp - FColorPair unit tests                          *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2025 Markus Gans                                      *
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
    FColorPairTest() = default;

  protected:
    void classNameTest();
    void noArgumentTest();
    void copyConstructorTest();
    void assignmentTest();
    void compareTest();
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
    CPPUNIT_TEST (compareTest);
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
  CPPUNIT_ASSERT ( pair.getForegroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( pair.getBackgroundColor() == finalcut::FColor::Default );
}

//----------------------------------------------------------------------
void FColorPairTest::copyConstructorTest()
{
  const finalcut::FColorPair p1a;
  finalcut::FColorPair p1b (p1a);
  CPPUNIT_ASSERT ( p1b.getForegroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( p1b.getBackgroundColor() == finalcut::FColor::Default );

  const finalcut::FColorPair p2a (finalcut::FColor::Yellow );
  const finalcut::FColorPair p2b (p2a);
  CPPUNIT_ASSERT ( p2b.getForegroundColor() == finalcut::FColor::Yellow );
  CPPUNIT_ASSERT ( p2b.getBackgroundColor() == finalcut::FColor::Default );

  const finalcut::FColorPair p3a (finalcut::FColor::Red, finalcut::FColor::Black);
  const finalcut::FColorPair p3b (p3a);
  CPPUNIT_ASSERT ( p3b.getForegroundColor() == finalcut::FColor::Red );
  CPPUNIT_ASSERT ( p3b.getBackgroundColor() == finalcut::FColor::Black );
}

//----------------------------------------------------------------------
void FColorPairTest::assignmentTest()
{
  const finalcut::FColorPair p1a;
  const finalcut::FColorPair p1b = p1a;
  CPPUNIT_ASSERT ( p1b.getForegroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( p1b.getBackgroundColor() == finalcut::FColor::Default );

  const finalcut::FColorPair p2a (finalcut::FColor::Yellow );
  const finalcut::FColorPair p2b = p2a;
  CPPUNIT_ASSERT ( p2b.getForegroundColor() == finalcut::FColor::Yellow );
  CPPUNIT_ASSERT ( p2b.getBackgroundColor() == finalcut::FColor::Default );

  const finalcut::FColorPair p3a (finalcut::FColor::Red, finalcut::FColor::Black);
  const finalcut::FColorPair p3b = p3a;
  CPPUNIT_ASSERT ( p3b.getForegroundColor() == finalcut::FColor::Red );
  CPPUNIT_ASSERT ( p3b.getBackgroundColor() == finalcut::FColor::Black );
}

//----------------------------------------------------------------------
void FColorPairTest::compareTest()
{
  finalcut::FColorPair p1;
  p1.setForegroundColor (finalcut::FColor::NavyBlue);
  finalcut::FColorPair p2 (finalcut::FColor::NavyBlue);
  p2.setBackgroundColor (finalcut::FColor::Default);
  CPPUNIT_ASSERT ( p1 == p2 );

  p2.setBackgroundColor (finalcut::FColor::MediumSpringGreen);
  CPPUNIT_ASSERT ( p1 != p2 );
}

//----------------------------------------------------------------------
void FColorPairTest::setColorTest()
{
  finalcut::FColorPair p1;
  p1.setForegroundColor (finalcut::FColor::DarkGreen);
  CPPUNIT_ASSERT ( p1.getForegroundColor() == finalcut::FColor::DarkGreen );
  CPPUNIT_ASSERT ( p1.getBackgroundColor() == finalcut::FColor::Default );

  finalcut::FColorPair p2 (finalcut::FColor::Yellow);
  p2.setBackgroundColor (finalcut::FColor::DeepSkyBlue1);
  CPPUNIT_ASSERT ( p2.getForegroundColor() == finalcut::FColor::Yellow );
  CPPUNIT_ASSERT ( p2.getBackgroundColor() == finalcut::FColor::DeepSkyBlue1 );

  finalcut::FColorPair p3 (finalcut::FColor::Red, finalcut::FColor::Black);
  p3.setColorPair (finalcut::FColor::NavyBlue, finalcut::FColor::SandyBrown);
  CPPUNIT_ASSERT ( p3.getForegroundColor() == finalcut::FColor::NavyBlue );
  CPPUNIT_ASSERT ( p3.getBackgroundColor() == finalcut::FColor::SandyBrown );
  finalcut::FColorPair p4 (finalcut::FColor::DarkBlue, finalcut::FColor::Grey66);
  p3.setColorPair (p4);
  CPPUNIT_ASSERT ( p3.getForegroundColor() == finalcut::FColor::DarkBlue );
  CPPUNIT_ASSERT ( p3.getBackgroundColor() == finalcut::FColor::Grey66 );
}

//----------------------------------------------------------------------
void FColorPairTest::swapTest()
{
  finalcut::FColorPair p1;
  p1.swap();
  CPPUNIT_ASSERT ( p1.getForegroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( p1.getBackgroundColor() == finalcut::FColor::Default );

  finalcut::FColorPair p2 (finalcut::FColor::LightBlue );
  p2.swap();
  CPPUNIT_ASSERT ( p2.getForegroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( p2.getBackgroundColor() == finalcut::FColor::LightBlue );

  finalcut::FColorPair p3 (finalcut::FColor::Cyan, finalcut::FColor::White);
  p3.swap();
  CPPUNIT_ASSERT ( p3.getForegroundColor() == finalcut::FColor::White );
  CPPUNIT_ASSERT ( p3.getBackgroundColor() == finalcut::FColor::Cyan );
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FColorPairTest);

// The general unit test main part
#include <main-test.inc>
