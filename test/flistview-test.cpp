/***********************************************************************
* flistview-test.cpp - FEvent unit tests                               *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2026 Markus Gans                                           *
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

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <final/final.h>

//----------------------------------------------------------------------
// class FListViewTest
//----------------------------------------------------------------------
class FListViewTest : public CPPUNIT_NS::TestFixture
{
  public:
    FListViewTest() = default;

  protected:
    void flistViewItemSetDataTest();
    void setCheckedTest();

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FListViewTest);
    // Add a methods to the test suite
    CPPUNIT_TEST (flistViewItemSetDataTest);
    CPPUNIT_TEST (setCheckedTest);
    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};

//----------------------------------------------------------------------
void FListViewTest::flistViewItemSetDataTest()
{
  finalcut::FListView list{};
  list.addColumn("Test");
  list.setColumnSortType (1, finalcut::SortType::Name);
  finalcut::FStringList strList{"item 01"};
  auto iter = list.insert(strList);
  finalcut::FListViewItem* item = static_cast<finalcut::FListViewItem*>(*iter);
  const int expected = 42;
  item->setData(expected);
  const auto result = item->getData<int>();
  CPPUNIT_ASSERT_EQUAL(expected, result);
}

//----------------------------------------------------------------------
void FListViewTest::setCheckedTest()
{
  finalcut::FListView list{};
  list.addColumn("Test");
  list.setColumnSortType (1, finalcut::SortType::Name);
  finalcut::FStringList strList{"item 01"};
  auto iter = list.insert(strList);
  finalcut::FListViewItem* item = static_cast<finalcut::FListViewItem*>(*iter);
  item->setCheckable(true);
  item->setChecked(false);
  CPPUNIT_ASSERT(!item->isChecked());
  item->setChecked(true);
  CPPUNIT_ASSERT(item->isChecked());
  int value{0};
  list.addCallback("changed", [&value] () { value++;} );
  list.emitCallback("changed");
  CPPUNIT_ASSERT_MESSAGE("\"changed\" callback failed", value == 1 );
  finalcut::FKeyEvent event (finalcut::Event::KeyPress, finalcut::FKey::Space);
  // Should result in FListView::toggleCheckbox() being called, which in turn
  // calls FListView::processChanged(), emitting a "changed" event
  list.onKeyPress(&event);
  CPPUNIT_ASSERT_MESSAGE("\"changed\" event not emitted", value == 2 );
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FListViewTest);

// The general unit test main part
#include <main-test.inc>
