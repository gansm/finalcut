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
// class FObject_protected
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FObject_protected : public FObject
{
  public:
    bool event (FEvent* ev)
    {
      return FObject::event(ev);
    }
};
#pragma pack(pop)


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
    void childObjectTest();
    void removeParentTest();
    void addTest();
    void delTest();

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FObjectTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (NoArgumentTest);
    CPPUNIT_TEST (childObjectTest);
    CPPUNIT_TEST (removeParentTest);
    CPPUNIT_TEST (addTest);
    CPPUNIT_TEST (delTest);

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

  FObject_protected t;
  FEvent* ev = new FEvent(fc::None_Event);
  CPPUNIT_ASSERT ( ! t.event(ev) );
  delete ev;
}

//----------------------------------------------------------------------
void FObjectTest::childObjectTest()
{/*
  *  obj -> c1 -> c5 -> c6
  *      -> c2
  *      -> c3
  *      -> c4
  */
  FObject obj;
  FObject* c1 = new FObject(&obj);
  FObject* c2 = new FObject(&obj);
  FObject* c3 = new FObject(&obj);
  FObject* c4 = new FObject(&obj);
  FObject* c5 = new FObject(c1);
  FObject* c6 = new FObject(c5);

  CPPUNIT_ASSERT ( obj.hasChildren() );
  CPPUNIT_ASSERT ( obj.getChild(0) == 0 );
  CPPUNIT_ASSERT ( obj.getChild(1) != 0 );
  CPPUNIT_ASSERT ( obj.numOfChildren() == 4 );

  CPPUNIT_ASSERT ( obj.isChild(c1) );
  CPPUNIT_ASSERT ( obj.isChild(c2) );
  CPPUNIT_ASSERT ( obj.isChild(c3) );
  CPPUNIT_ASSERT ( obj.isChild(c4) );
  CPPUNIT_ASSERT ( obj.isChild(c5) );
  CPPUNIT_ASSERT ( obj.isChild(c6) );

  CPPUNIT_ASSERT ( obj.isDirectChild(c1) );
  CPPUNIT_ASSERT ( obj.isDirectChild(c2) );
  CPPUNIT_ASSERT ( obj.isDirectChild(c3) );
  CPPUNIT_ASSERT ( obj.isDirectChild(c4) );
  CPPUNIT_ASSERT ( ! obj.isDirectChild(c5) );
  CPPUNIT_ASSERT ( c1->isDirectChild(c5) );
  CPPUNIT_ASSERT ( ! obj.isDirectChild(c6) );
  CPPUNIT_ASSERT ( ! c1->isDirectChild(c6) );
  CPPUNIT_ASSERT ( c5->isDirectChild(c6) );

  CPPUNIT_ASSERT ( c1->hasParent() );
  CPPUNIT_ASSERT ( c1->getParent() == &obj );
  CPPUNIT_ASSERT ( c1->hasChildren() );
  CPPUNIT_ASSERT ( ! c2->hasChildren() );
  CPPUNIT_ASSERT ( c1->getChild(0) == 0 );
  CPPUNIT_ASSERT ( c1->getChild(1) != 0 );
  CPPUNIT_ASSERT ( c2->getChild(1) == 0 );
  CPPUNIT_ASSERT ( c1->numOfChildren() == 1 );
  CPPUNIT_ASSERT ( c2->numOfChildren() == 0 );
  const FObject::FObjectList& children_list2 = c1->getChildren();
  CPPUNIT_ASSERT ( children_list2.begin() == c1->begin() );
  CPPUNIT_ASSERT ( children_list2.begin() != c1->end() );
  CPPUNIT_ASSERT ( children_list2.end() != c1->begin() );
  CPPUNIT_ASSERT ( children_list2.end() == c1->end() );
  CPPUNIT_ASSERT ( ! c1->isDirectChild(c1) );
  CPPUNIT_ASSERT ( ! c1->isWidget() );
  CPPUNIT_ASSERT ( c1->isInstanceOf("FObject") );
  CPPUNIT_ASSERT ( ! c1->isTimerInUpdating() );
}

//----------------------------------------------------------------------
void FObjectTest::removeParentTest()
{/*
  *  obj -> child
  */
  FObject* obj =  new FObject();
  FObject* child = new FObject(obj);

  CPPUNIT_ASSERT ( obj->hasChildren() );
  CPPUNIT_ASSERT ( obj->numOfChildren() == 1 );
  CPPUNIT_ASSERT ( obj->isChild(child) );

  CPPUNIT_ASSERT ( child->hasParent() );
  CPPUNIT_ASSERT ( child->getParent() == obj );

  child->removeParent();
  CPPUNIT_ASSERT ( ! obj->hasChildren() );
  CPPUNIT_ASSERT ( obj->numOfChildren() == 0 );
  CPPUNIT_ASSERT ( ! obj->isChild(child) );

  CPPUNIT_ASSERT ( ! child->hasParent() );
  CPPUNIT_ASSERT ( child->getParent() != obj );

  delete child;
  delete obj;  // also deletes the child object
}

//----------------------------------------------------------------------
void FObjectTest::addTest()
{/*
  *  obj -> child
  */
  FObject* obj =  new FObject();
  FObject* child = new FObject();

  CPPUNIT_ASSERT ( ! obj->hasChildren() );
  CPPUNIT_ASSERT ( obj->numOfChildren() == 0 );
  CPPUNIT_ASSERT ( ! obj->isChild(child) );

  CPPUNIT_ASSERT ( ! child->hasParent() );
  CPPUNIT_ASSERT ( child->getParent() != obj );

  obj->addChild(child);
  CPPUNIT_ASSERT ( obj->hasChildren() );
  CPPUNIT_ASSERT ( obj->numOfChildren() == 1 );
  CPPUNIT_ASSERT ( obj->isChild(child) );

  CPPUNIT_ASSERT ( child->hasParent() );
  CPPUNIT_ASSERT ( child->getParent() == obj );

  delete obj;  // also deletes the child object
}

//----------------------------------------------------------------------
void FObjectTest::delTest()
{/*
  *  obj -> child
  */
  FObject* obj =  new FObject();
  FObject* child = new FObject(obj);
  CPPUNIT_ASSERT ( obj->hasChildren() );
  CPPUNIT_ASSERT ( obj->numOfChildren() == 1 );
  CPPUNIT_ASSERT ( obj->isChild(child) );

  CPPUNIT_ASSERT ( child->hasParent() );
  CPPUNIT_ASSERT ( child->getParent() == obj );

  obj->delChild(child);
  CPPUNIT_ASSERT ( ! obj->hasChildren() );
  CPPUNIT_ASSERT ( obj->numOfChildren() == 0 );
  CPPUNIT_ASSERT ( ! obj->isChild(child) );

  CPPUNIT_ASSERT ( ! child->hasParent() );
  CPPUNIT_ASSERT ( child->getParent() != obj );

  delete child;
  delete obj;
}



// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FObjectTest);

// The general unit test main part
#include <main-test.inc>
