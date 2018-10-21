/***********************************************************************
* fobject-test.cpp - FPoint unit tests                                 *
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

class FObject_protected : public finalcut::FObject
{
  public:
    FObject_protected()
      : count(0)
    { }

    bool event (finalcut::FEvent* ev)
    {
      return finalcut::FObject::event(ev);
    }

    TimerList* getTimerList() const
    {
      return finalcut::FObject::getTimerList();
    }

    uInt processEvent()
    {
      return processTimerEvent();
    }

    virtual void performTimerAction (const FObject*, const finalcut::FEvent*)
    {
      std::cout << ".";
      fflush(stdout);
      count++;
    }

    // Data Member
    uInt count;
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
    void noArgumentTest();
    void childObjectTest();
    void removeParentTest();
    void addTest();
    void delTest();
    void iteratorTest();
    void timeTest();
    void timerTest();
    void performTimerActionTest();

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FObjectTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (noArgumentTest);
    CPPUNIT_TEST (childObjectTest);
    CPPUNIT_TEST (removeParentTest);
    CPPUNIT_TEST (addTest);
    CPPUNIT_TEST (delTest);
    CPPUNIT_TEST (iteratorTest);
    CPPUNIT_TEST (timeTest);
    CPPUNIT_TEST (timerTest);
    CPPUNIT_TEST (performTimerActionTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};
#pragma pack(pop)

//----------------------------------------------------------------------
void FObjectTest::classNameTest()
{
  finalcut::FObject o;
  const char* const classname = o.getClassName();
  CPPUNIT_ASSERT ( std::strcmp(classname, "FObject") == 0 );
}

//----------------------------------------------------------------------
void FObjectTest::noArgumentTest()
{
  finalcut::FObject o1;
  finalcut::FObject o2;
  CPPUNIT_ASSERT ( ! o1.hasParent() );
  CPPUNIT_ASSERT ( o1.getParent() == 0 );
  CPPUNIT_ASSERT ( ! o1.hasChildren() );
  CPPUNIT_ASSERT ( o1.getChild(0) == 0 );
  CPPUNIT_ASSERT ( o1.getChild(1) == 0 );
  CPPUNIT_ASSERT ( o1.numOfChildren() == 0 );
  finalcut::FObject::FObjectList& children_list = o1.getChildren();
  CPPUNIT_ASSERT ( children_list.begin() == o1.begin() );
  CPPUNIT_ASSERT ( children_list.begin() == o1.end() );
  CPPUNIT_ASSERT ( children_list.end() == o1.begin() );
  CPPUNIT_ASSERT ( children_list.end() == o1.end() );
  CPPUNIT_ASSERT ( ! o1.isChild(&o2) );
  CPPUNIT_ASSERT ( ! o1.isDirectChild(&o2) );
  CPPUNIT_ASSERT ( ! o1.isWidget() );
  CPPUNIT_ASSERT ( o1.isInstanceOf("FObject") );
  CPPUNIT_ASSERT ( ! o1.isTimerInUpdating() );

  FObject_protected t;
  finalcut::FEvent* ev = new finalcut::FEvent(finalcut::fc::None_Event);
  CPPUNIT_ASSERT ( ! t.event(ev) );
  delete ev;

  ev = new finalcut::FEvent(finalcut::fc::Timer_Event);
  CPPUNIT_ASSERT ( t.event(ev) );
  delete ev;

  CPPUNIT_ASSERT ( ! finalcut::fc::emptyFString::get().isNull() );
  CPPUNIT_ASSERT ( finalcut::fc::emptyFString::get().isEmpty() );
}

//----------------------------------------------------------------------
void FObjectTest::childObjectTest()
{/*
  *  obj -> c1 -> c5 -> c6
  *      -> c2
  *      -> c3
  *      -> c4
  */
  finalcut::FObject obj;
  finalcut::FObject* c1 = new finalcut::FObject(&obj);
  finalcut::FObject* c2 = new finalcut::FObject(&obj);
  finalcut::FObject* c3 = new finalcut::FObject(&obj);
  finalcut::FObject* c4 = new finalcut::FObject(&obj);
  finalcut::FObject* c5 = new finalcut::FObject(c1);
  finalcut::FObject* c6 = new finalcut::FObject(c5);
  finalcut::FObject* c7 = new finalcut::FObject();

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
  const finalcut::FObject::FObjectList& children_list2 = c1->getChildren();
  CPPUNIT_ASSERT ( children_list2.begin() == c1->begin() );
  CPPUNIT_ASSERT ( children_list2.begin() != c1->end() );
  CPPUNIT_ASSERT ( children_list2.end() != c1->begin() );
  CPPUNIT_ASSERT ( children_list2.end() == c1->end() );
  CPPUNIT_ASSERT ( ! c1->isDirectChild(c7) );
  CPPUNIT_ASSERT ( ! c1->isWidget() );
  CPPUNIT_ASSERT ( c1->isInstanceOf("FObject") );
  CPPUNIT_ASSERT ( ! c1->isTimerInUpdating() );
}

//----------------------------------------------------------------------
void FObjectTest::removeParentTest()
{/*
  *  obj -> child
  */
  finalcut::FObject* obj =  new finalcut::FObject();
  finalcut::FObject* child = new finalcut::FObject(obj);

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
  finalcut::FObject* obj =  new finalcut::FObject();
  finalcut::FObject* child = new finalcut::FObject();

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
  finalcut::FObject* obj =  new finalcut::FObject();
  finalcut::FObject* child = new finalcut::FObject(obj);

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

//----------------------------------------------------------------------
void FObjectTest::iteratorTest()
{/*
  *  obj -> child1
  *      -> child2
  *      -> child3
  */
  finalcut::FObject* obj =  new finalcut::FObject();
  finalcut::FObject* child1 = new finalcut::FObject(obj);
  finalcut::FObject* child2 = new finalcut::FObject(obj);
  finalcut::FObject* child3 = new finalcut::FObject(obj);

  CPPUNIT_ASSERT ( child1->getParent() == obj );
  CPPUNIT_ASSERT ( child2->getParent() == obj );
  CPPUNIT_ASSERT ( child3->getParent() == obj );

  finalcut::FObject::constFObjectIterator c_iter, c_last;
  c_iter = obj->begin();
  c_last = obj->end();
  int i = 0;

  while ( c_iter != c_last )
  {
    i++;
    ++c_iter;
  }

  CPPUNIT_ASSERT ( obj->numOfChildren() == i );
  CPPUNIT_ASSERT ( i == 3 );

  finalcut::FObject::FObjectIterator iter, last;
  iter = obj->begin();
  last = obj->end();
  i = 0;

  while ( iter != last )
  {
    i++;
    ++iter;
  }

  CPPUNIT_ASSERT ( obj->numOfChildren() == i );
  CPPUNIT_ASSERT ( i == 3 );

  delete obj;
}

//----------------------------------------------------------------------
void FObjectTest::timeTest()
{
  struct timeval time1;
  long timeout = 750000;  // 750 ms
  finalcut::FObject::getCurrentTime(&time1);
  CPPUNIT_ASSERT ( ! finalcut::FObject::isTimeout (&time1, timeout) );
  sleep(1);
  CPPUNIT_ASSERT ( finalcut::FObject::isTimeout (&time1, timeout) );
  time1.tv_sec = 300;
  time1.tv_usec = 2000000;  // > 1000000 µs to test diff underflow
  CPPUNIT_ASSERT ( finalcut::FObject::isTimeout (&time1, timeout) );
}

//----------------------------------------------------------------------
void FObjectTest::timerTest()
{
  using finalcut::operator +;
  using finalcut::operator -;
  using finalcut::operator +=;
  using finalcut::operator <;

  FObject_protected t1;
  FObject_protected t2;
  int id1, id2;
  CPPUNIT_ASSERT ( t1.getTimerList()->empty() );
  id1 = t1.addTimer(300);
  CPPUNIT_ASSERT ( ! t1.getTimerList()->empty() );
  CPPUNIT_ASSERT ( t1.getTimerList()->size() == 1 );
  id2 = t1.addTimer(900);
  CPPUNIT_ASSERT ( t1.getTimerList()->size() == 2 );
  CPPUNIT_ASSERT ( &t1 != &t2 );
  CPPUNIT_ASSERT ( id1 != id2 );
  t1.delTimer (id1);
  CPPUNIT_ASSERT ( t1.getTimerList()->size() == 1 );
  t1.delTimer (id2);
  CPPUNIT_ASSERT ( t1.getTimerList()->empty() );
  CPPUNIT_ASSERT ( t1.getTimerList()->size() == 0 );

  id1 = t1.addTimer(45);
  id2 = t1.addTimer(95);
  t1.delTimer (id2);
  CPPUNIT_ASSERT ( t1.getTimerList()->size() == 1 );
  t1.delTimer (id1);
  CPPUNIT_ASSERT ( t1.getTimerList()->empty() );
  CPPUNIT_ASSERT ( t1.getTimerList()->size() == 0 );

  CPPUNIT_ASSERT ( ! t1.delTimer (id1) );  // id double delete
  CPPUNIT_ASSERT ( ! t1.delAllTimer() );

  t1.addTimer(250);
  t1.addTimer(500);
  t2.addTimer(750);
  t2.addTimer(1000);
  CPPUNIT_ASSERT_EQUAL ( t1.getTimerList(), t2.getTimerList() );
  CPPUNIT_ASSERT ( t1.getTimerList()->size() == 4 );
  CPPUNIT_ASSERT ( t2.getTimerList()->size() == 4 );

  t1.delOwnTimer();
  CPPUNIT_ASSERT ( t1.getTimerList()->size() == 2 );
  CPPUNIT_ASSERT ( t2.getTimerList()->size() == 2 );

  t1.addTimer(250);
  CPPUNIT_ASSERT ( t1.getTimerList()->size() == 3 );
  CPPUNIT_ASSERT ( t2.getTimerList()->size() == 3 );

  t2.delAllTimer();
  CPPUNIT_ASSERT ( t1.getTimerList()->empty() );
  CPPUNIT_ASSERT ( t2.getTimerList()->empty() );
  CPPUNIT_ASSERT ( t1.getTimerList()->size() == 0 );
  CPPUNIT_ASSERT ( t2.getTimerList()->size() == 0 );

  timeval tv1 = { 1321006271, 0 };
  timeval tv2 = { 27166271, 0 };
  timeval tv_sum = tv1 + tv2;
  CPPUNIT_ASSERT ( tv_sum.tv_sec == 1348172542 );
  CPPUNIT_ASSERT ( tv_sum.tv_usec == 0 );

  timeval tv_difference = tv1 - tv2;
  CPPUNIT_ASSERT ( tv_difference.tv_sec == 1293840000 );
  CPPUNIT_ASSERT ( tv_difference.tv_usec == 0 );

  tv_sum += tv2;
  CPPUNIT_ASSERT ( tv_sum.tv_sec == 1375338813 );
  CPPUNIT_ASSERT ( tv_sum.tv_usec == 0 );

  CPPUNIT_ASSERT ( tv2 < tv1 );
  CPPUNIT_ASSERT ( ! (tv1 < tv2) );
  CPPUNIT_ASSERT ( tv1 < tv_sum );
  CPPUNIT_ASSERT ( ! (tv_sum < tv1) );
  CPPUNIT_ASSERT ( tv2 < tv_sum );
  CPPUNIT_ASSERT ( ! (tv_sum < tv2) );
  CPPUNIT_ASSERT ( tv_difference < tv_sum );
  CPPUNIT_ASSERT ( ! (tv_sum < tv_difference) );

  tv1.tv_usec = tv2.tv_usec = 600000;
  tv_sum = tv1 + tv2;
  CPPUNIT_ASSERT ( tv_sum.tv_sec == 1348172543 );
  CPPUNIT_ASSERT ( tv_sum.tv_usec == 200000 );

  tv1.tv_usec = 654321;
  tv2.tv_usec = 123456;
  tv_difference = tv1 - tv2;
  CPPUNIT_ASSERT ( tv_difference.tv_sec == 1293840000 );
  CPPUNIT_ASSERT ( tv_difference.tv_usec == 530865 );

  tv2.tv_usec = 999888;
  tv_sum += tv2;
  CPPUNIT_ASSERT ( tv_sum.tv_sec == 1375338815 );
  CPPUNIT_ASSERT ( tv_sum.tv_usec == 199888 );

  CPPUNIT_ASSERT ( tv2 < tv1 );
  CPPUNIT_ASSERT ( ! (tv1 < tv2) );
  CPPUNIT_ASSERT ( tv_difference < tv_sum );
  CPPUNIT_ASSERT ( ! (tv_sum < tv_difference) );

  CPPUNIT_ASSERT ( ! t1.delTimer(0) );
  CPPUNIT_ASSERT ( ! t1.delTimer(-1) );
}

//----------------------------------------------------------------------
void FObjectTest::performTimerActionTest()
{
  FObject_protected t;
  uInt num_events = 0;
  uInt loop = 0;
  t.addTimer(100);

  while ( loop < 10 )
  {
    num_events += t.processEvent();
    // Wait 100 ms
    nanosleep ((const struct timespec[]){{0, 100000000L}}, NULL);
    loop++;
  }

  CPPUNIT_ASSERT ( loop == 10 );
  CPPUNIT_ASSERT ( num_events == 9 );
  CPPUNIT_ASSERT ( t.count == 9 );
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FObjectTest);

// The general unit test main part
#include <main-test.inc>
