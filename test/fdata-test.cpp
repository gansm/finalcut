/***********************************************************************
* fdata-test.cpp - FCallback unit tests                                *
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
// functions
//----------------------------------------------------------------------
/*void cb_function_ptr (int* value)
{
  (*value)++;
}

//----------------------------------------------------------------------
void cb_function_ref (int& value)
{
  value += 2;
}*/

//----------------------------------------------------------------------
// class FDataTest
//----------------------------------------------------------------------

class FDataTest : public CPPUNIT_NS::TestFixture
{
  public:
    FDataTest()
    { }

  protected:
    void classNameTest();
    void dataTest();

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FDataTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (dataTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();

    // Data member
    static finalcut::FWidget root_widget;
};

// static class attributes
finalcut::FWidget FDataTest::root_widget{nullptr};

//----------------------------------------------------------------------
void FDataTest::classNameTest()
{
  const finalcut::FCallback d;
  const finalcut::FString& classname = d.getClassName();
  CPPUNIT_ASSERT ( classname == "FData" );
}

//----------------------------------------------------------------------
void FDataTest::dataTest()
{

}
// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FDataTest);

// The general unit test main part
#include <main-test.inc>
