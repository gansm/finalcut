/***********************************************************************
* fstyle-test.cpp - FStyle unit tests                                  *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019 Markus Gans                                           *
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
// class FStyleTest
//----------------------------------------------------------------------

class FStyleTest : public CPPUNIT_NS::TestFixture
{
  public:
    FStyleTest()
    { }

  protected:
    void classNameTest();
    void noArgumentTest();
    void copyConstructorTest();
    void assignmentTest();
    void setStyleTest();

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FStyleTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (noArgumentTest);
    CPPUNIT_TEST (copyConstructorTest);
    CPPUNIT_TEST (assignmentTest);
    CPPUNIT_TEST (setStyleTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};


//----------------------------------------------------------------------
void FStyleTest::classNameTest()
{
  const finalcut::FStyle s;
  const finalcut::FString& classname = s.getClassName();
  CPPUNIT_ASSERT ( classname == "FStyle" );
}

//----------------------------------------------------------------------
void FStyleTest::noArgumentTest()
{
  const finalcut::FStyle style{};
  CPPUNIT_ASSERT ( style.getStyle() == finalcut::fc::Reset );
}

//----------------------------------------------------------------------
void FStyleTest::copyConstructorTest()
{
  const finalcut::FStyle style1a;
  CPPUNIT_ASSERT ( style1a.getStyle() == finalcut::fc::Reset );
  finalcut::FStyle style1b (style1a);
  CPPUNIT_ASSERT ( style1b.getStyle() == finalcut::fc::Reset );

  const finalcut::FStyle style2a (finalcut::fc::Bold );
  CPPUNIT_ASSERT ( style2a.getStyle() == finalcut::fc::Bold );
  const finalcut::FStyle style2b (style2a);
  CPPUNIT_ASSERT ( style2b.getStyle() == finalcut::fc::Bold );

  const finalcut::FStyle style3a (finalcut::fc::Bold + finalcut::fc::Dim);
  CPPUNIT_ASSERT ( style3a.getStyle() == (finalcut::fc::Bold | finalcut::fc::Dim) );
  const finalcut::FStyle style3b (style3a);
  CPPUNIT_ASSERT ( style3b.getStyle() == (finalcut::fc::Bold | finalcut::fc::Dim) );
}

//----------------------------------------------------------------------
void FStyleTest::assignmentTest()
{
  const finalcut::FStyle style1a;
  const finalcut::FStyle style1b = style1a;
  CPPUNIT_ASSERT ( style1b.getStyle() == finalcut::fc::Reset );

  const finalcut::FStyle style2a (finalcut::fc::Italic );
  const finalcut::FStyle style2b = style2a;
  CPPUNIT_ASSERT ( style2b.getStyle() == finalcut::fc::Italic );

  const finalcut::FStyle style3a (finalcut::fc::Underline | finalcut::fc::Blink);
  const finalcut::FStyle style3b = style3a;
  CPPUNIT_ASSERT ( style3b.getStyle() == (finalcut::fc::Underline + finalcut::fc::Blink) );

  finalcut::FStyle style4 = finalcut::FStyle();
  CPPUNIT_ASSERT ( style4.getStyle() == finalcut::fc::Reset );
  style4 = finalcut::FStyle(finalcut::fc::Bold | finalcut::fc::Dim);
  CPPUNIT_ASSERT ( style4.getStyle() == (finalcut::fc::Bold | finalcut::fc::Dim) );
}

//----------------------------------------------------------------------
void FStyleTest::setStyleTest()
{
  finalcut::FStyle style1;
  style1.setStyle (finalcut::fc::Reverse);
  CPPUNIT_ASSERT ( style1.getStyle() == finalcut::fc::Reverse );

  finalcut::FStyle style2 (finalcut::fc::Reverse);
  style2.setStyle (finalcut::fc::Standout);
  CPPUNIT_ASSERT ( style2.getStyle() == finalcut::fc::Standout );

  finalcut::FStyle style3 (finalcut::fc::Protected | finalcut::fc::CrossedOut);
  style3.setStyle (finalcut::fc::Invisible);
  CPPUNIT_ASSERT ( style3.getStyle() == finalcut::fc::Invisible );
  finalcut::FStyle style4 (finalcut::fc::DoubleUnderline);
  style3.setStyle (style4);
  CPPUNIT_ASSERT ( style3.getStyle() == finalcut::fc::DoubleUnderline );

  finalcut::FStyle style5 (finalcut::fc::Transparent);
  CPPUNIT_ASSERT ( style5.getStyle() == finalcut::fc::Transparent );
  style5.setStyle (finalcut::fc::Reset);
  CPPUNIT_ASSERT ( style5.getStyle() == finalcut::fc::Reset );
  style5.setStyle (finalcut::fc::ColorOverlay);
  CPPUNIT_ASSERT ( style5.getStyle() == finalcut::fc::ColorOverlay );
  style5.setStyle (finalcut::fc::InheritBackground);
  CPPUNIT_ASSERT ( style5.getStyle() == finalcut::fc::InheritBackground );
  style5.setStyle (finalcut::fc::Reset + finalcut::fc::Dim);
  CPPUNIT_ASSERT ( style5.getStyle() == finalcut::fc::Dim );
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FStyleTest);

// The general unit test main part
#include <main-test.inc>
