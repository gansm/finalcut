/***********************************************************************
* fstyle-test.cpp - FStyle unit tests                                  *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2021 Markus Gans                                      *
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
    FStyleTest() = default;

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
  CPPUNIT_ASSERT ( style.getStyle() == finalcut::Style::None );
}

//----------------------------------------------------------------------
void FStyleTest::copyConstructorTest()
{
  const finalcut::FStyle style1a;
  CPPUNIT_ASSERT ( style1a.getStyle() == finalcut::Style::None );
  finalcut::FStyle style1b (style1a);
  CPPUNIT_ASSERT ( style1b.getStyle() == finalcut::Style::None );

  const finalcut::FStyle style2a (finalcut::Style::Bold );
  CPPUNIT_ASSERT ( style2a.getStyle() == finalcut::Style::Bold );
  const finalcut::FStyle style2b (style2a);
  CPPUNIT_ASSERT ( style2b.getStyle() == finalcut::Style::Bold );

  const finalcut::FStyle style3a (finalcut::Style::Bold + finalcut::Style::Dim);
  CPPUNIT_ASSERT ( style3a.getStyle() == (finalcut::Style::Bold | finalcut::Style::Dim) );
  const finalcut::FStyle style3b (style3a);
  CPPUNIT_ASSERT ( style3b.getStyle() == (finalcut::Style::Bold | finalcut::Style::Dim) );
}

//----------------------------------------------------------------------
void FStyleTest::assignmentTest()
{
  const finalcut::FStyle style1a;
  const finalcut::FStyle style1b = style1a;
  CPPUNIT_ASSERT ( style1b.getStyle() == finalcut::Style::None );

  const finalcut::FStyle style2a (finalcut::Style::Italic );
  const finalcut::FStyle style2b = style2a;
  CPPUNIT_ASSERT ( style2b.getStyle() == finalcut::Style::Italic );

  const finalcut::FStyle style3a (finalcut::Style::Underline | finalcut::Style::Blink);
  const finalcut::FStyle style3b = style3a;
  CPPUNIT_ASSERT ( style3b.getStyle() == (finalcut::Style::Underline + finalcut::Style::Blink) );

  finalcut::FStyle style4 = finalcut::FStyle();
  CPPUNIT_ASSERT ( style4.getStyle() == finalcut::Style::None );
  style4 = finalcut::FStyle(finalcut::Style::Bold | finalcut::Style::Dim);
  CPPUNIT_ASSERT ( style4.getStyle() == (finalcut::Style::Bold | finalcut::Style::Dim) );
}

//----------------------------------------------------------------------
void FStyleTest::setStyleTest()
{
  finalcut::FStyle style1;
  style1.setStyle (finalcut::Style::Reverse);
  CPPUNIT_ASSERT ( style1.getStyle() == finalcut::Style::Reverse );

  finalcut::FStyle style2 (finalcut::Style::Reverse);
  style2.setStyle (finalcut::Style::Standout);
  CPPUNIT_ASSERT ( style2.getStyle() == finalcut::Style::Standout );

  finalcut::FStyle style3 (finalcut::Style::Protected | finalcut::Style::CrossedOut);
  style3.setStyle (finalcut::Style::Invisible);
  CPPUNIT_ASSERT ( style3.getStyle() == finalcut::Style::Invisible );
  finalcut::FStyle style4 (finalcut::Style::DoubleUnderline);
  style3.setStyle (style4);
  CPPUNIT_ASSERT ( style3.getStyle() == finalcut::Style::DoubleUnderline );

  finalcut::FStyle style5 (finalcut::Style::Transparent);
  CPPUNIT_ASSERT ( style5.getStyle() == finalcut::Style::Transparent );
  style5.setStyle (finalcut::Style::None);
  CPPUNIT_ASSERT ( style5.getStyle() == finalcut::Style::None );
  style5.setStyle (finalcut::Style::ColorOverlay);
  CPPUNIT_ASSERT ( style5.getStyle() == finalcut::Style::ColorOverlay );
  style5.setStyle (finalcut::Style::InheritBackground);
  CPPUNIT_ASSERT ( style5.getStyle() == finalcut::Style::InheritBackground );
  style5.setStyle (finalcut::Style::None + finalcut::Style::Dim);
  CPPUNIT_ASSERT ( style5.getStyle() == finalcut::Style::Dim );
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FStyleTest);

// The general unit test main part
#include <main-test.inc>
