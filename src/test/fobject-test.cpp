/***********************************************************************
* fobject-test.cpp - FPoint unit tests                                  *
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
// class FObjectTest
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FObjectTest : public CPPUNIT_NS::TestFixture
{
  public:
    FObjectTest()
    { }

  protected:
    void classNameTest();
    void NoArgumentTest();
    void copyConstructorTest();
    void assignmentTest();

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FObjectTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (NoArgumentTest);
    CPPUNIT_TEST (copyConstructorTest);
    CPPUNIT_TEST (assignmentTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};
#pragma pack(pop)

//----------------------------------------------------------------------
void FObjectTest::classNameTest()
{
  FObject o;
  const char* const classname = o.getClassName();
  CPPUNIT_ASSERT ( std::strcmp(classname, "FObject") == 0 );
}

//----------------------------------------------------------------------
void FObjectTest::NoArgumentTest()
{
  FObject o1;
  CPPUNIT_ASSERT ( ! o1.hasParent() );
  CPPUNIT_ASSERT ( o1.getParent() == 0 );
  CPPUNIT_ASSERT ( ! o1.hasChildren() );
  CPPUNIT_ASSERT ( o1.getChild(0) == 0 );
  CPPUNIT_ASSERT ( o1.getChild(1) == 0 );
  CPPUNIT_ASSERT ( o1.numOfChildren() == 0 );
  const FObject::FObjectList& children_list = o1.getChildren();
  CPPUNIT_ASSERT ( children_list.begin() == o1.begin() );
  CPPUNIT_ASSERT ( children_list.begin() == o1.end() );
  CPPUNIT_ASSERT ( children_list.end() == o1.begin() );
  CPPUNIT_ASSERT ( children_list.end() == o1.end() );
  CPPUNIT_ASSERT ( ! o1.isChild(&o1) );
  CPPUNIT_ASSERT ( ! o1.isDirectChild(&o1) );
  CPPUNIT_ASSERT ( ! o1.isWidget() );
  CPPUNIT_ASSERT ( o1.isInstanceOf("FObject") );
  CPPUNIT_ASSERT ( ! o1.isTimerInUpdating() );

  struct test_protected : public FObject
  {
    bool test_event (FEvent* ev)
    {
      return event(ev);
    }
  };

  test_protected t;
  FEvent* ev = new FEvent(fc::None_Event);
  CPPUNIT_ASSERT ( ! t.test_event(ev) );
  delete ev;
}

//----------------------------------------------------------------------
void FObjectTest::copyConstructorTest()
{

}

//----------------------------------------------------------------------
void FObjectTest::assignmentTest()
{

}



// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FObjectTest);

// The general unit test main part
#include <main-test.inc>
