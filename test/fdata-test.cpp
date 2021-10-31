/***********************************************************************
* fdata-test.cpp - FData unit tests                                    *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2020-2021 Markus Gans                                      *
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
float my_function()
{
  return 13.45F;
}

//----------------------------------------------------------------------
long int my_function2 (long int i)
{
  return 2 * i;
}

//----------------------------------------------------------------------
long int my_function3 (long int i)
{
  return 3 * i;
}


//----------------------------------------------------------------------
// class FDataTest
//----------------------------------------------------------------------

class FDataTest : public CPPUNIT_NS::TestFixture
{
  public:
    FDataTest() = default;

  protected:
    void classNameTest();
    void fdataTest();
    void copyConstructorTest();
    void moveConstructorTest();
    void copyAssignmentTest();
    void moveAssignmentTest();
    void makeFDataTest();

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FDataTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (fdataTest);
    CPPUNIT_TEST (copyConstructorTest);
    CPPUNIT_TEST (moveConstructorTest);
    CPPUNIT_TEST (copyAssignmentTest);
    CPPUNIT_TEST (moveAssignmentTest);
    CPPUNIT_TEST (makeFDataTest);

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
  auto da = finalcut::FDataAccess();
  const finalcut::FString& classname2 = da.getClassName();
  CPPUNIT_ASSERT ( classname2 == "FDataAccess" );

  auto d = finalcut::FData<void*>(nullptr);
  const finalcut::FString& classname = d.getClassName();
  CPPUNIT_ASSERT ( classname == "FData" );
}

//----------------------------------------------------------------------
void FDataTest::fdataTest()
{
  // nummber
  auto data1 = finalcut::FData<int>(5);
  CPPUNIT_ASSERT ( data1.isInitializedCopy() );
  CPPUNIT_ASSERT ( ! data1.isInitializedReference() );
  std::stringstream stream{};
  stream << data1;
  CPPUNIT_ASSERT ( stream.str() == "5" );
  int i1{data1};
  CPPUNIT_ASSERT ( i1 == 5 );
  CPPUNIT_ASSERT ( data1() == 5 );
  CPPUNIT_ASSERT ( data1.get() == 5 );
  data1.get()++;
  CPPUNIT_ASSERT ( data1.get() == 6 );
  data1.set(3);
  CPPUNIT_ASSERT ( data1.get() == 3 );
  data1 << 8;
  CPPUNIT_ASSERT ( data1.get() == 8 );

  // int value
  int integer_value = 10;
  auto data2 = finalcut::FData<int>(std::ref(integer_value));
  CPPUNIT_ASSERT ( ! data2.isInitializedCopy() );
  CPPUNIT_ASSERT ( data2.isInitializedReference() );
  stream.clear();
  stream.str("");
  stream << data2;
  CPPUNIT_ASSERT ( stream.str() == "10" );
  int i2{data2};
  CPPUNIT_ASSERT ( i2 == 10 );
  CPPUNIT_ASSERT ( data2() == 10 );
  CPPUNIT_ASSERT ( data2.get() == 10 );
  data2.get()--;
  CPPUNIT_ASSERT ( data2.get() == 9 );
  CPPUNIT_ASSERT ( integer_value == 9 );
  data2.set(7);
  CPPUNIT_ASSERT ( data2.get() == 7 );
  CPPUNIT_ASSERT ( integer_value == 7 );
  data2 << 15;
  CPPUNIT_ASSERT ( integer_value == 15 );

  // const int value
  const int const_integer_value = 12;
  auto data3 = finalcut::FData<const int>(const_integer_value);
  CPPUNIT_ASSERT ( ! data3.isInitializedCopy() );
  CPPUNIT_ASSERT ( data3.isInitializedReference() );
  stream.clear();
  stream.str("");
  stream << data3;
  CPPUNIT_ASSERT ( stream.str() == "12" );
  int i3{data3};
  CPPUNIT_ASSERT ( i3 == 12 );
  CPPUNIT_ASSERT ( data3() == 12 );
  CPPUNIT_ASSERT ( data3.get() == 12 );

  // Function via pointer
  auto data4 = finalcut::FData<std::function<float()>>(&my_function);
  CPPUNIT_ASSERT ( data4.isInitializedCopy() );
  CPPUNIT_ASSERT ( ! data4.isInitializedReference() );
  stream.clear();
  stream.str("");
  stream << data4()();
  CPPUNIT_ASSERT ( stream.str() == "13.45" );
  CPPUNIT_ASSERT ( data4()() == 13.45F );
  CPPUNIT_ASSERT ( data4.get()() == 13.45F );

  // Function via reference -> remove reference + add pointer
  auto data5 = finalcut::FData<std::function<float()>>(my_function);
  CPPUNIT_ASSERT ( data5.isInitializedCopy() );
  CPPUNIT_ASSERT ( ! data5.isInitializedReference() );
  stream.clear();
  stream.str("");
  stream << data5()();
  CPPUNIT_ASSERT ( stream.str() == "13.45" );
  CPPUNIT_ASSERT ( data5()() == 13.45F );
  CPPUNIT_ASSERT ( data5.get()() == 13.45F );

  // Function with parameter via pointer
  auto data6 = finalcut::FData<std::function<long int(long int)>>(&my_function2);
  CPPUNIT_ASSERT ( data6.isInitializedCopy() );
  CPPUNIT_ASSERT ( ! data6.isInitializedReference() );
  stream.clear();
  stream.str("");
  stream << data6()(4L);
  CPPUNIT_ASSERT ( stream.str() == "8" );
  CPPUNIT_ASSERT ( data6()(4) == 8L );
  CPPUNIT_ASSERT ( data6.get()(4) == 8L );

  // std::string
  auto data7 = finalcut::FData<std::string>("123");
  CPPUNIT_ASSERT ( data7.isInitializedCopy() );
  CPPUNIT_ASSERT ( ! data7.isInitializedReference() );
  stream.clear();
  stream.str("");
  stream << data7;
  CPPUNIT_ASSERT ( stream.str() == "123" );
  std::string str{data7};
  CPPUNIT_ASSERT ( str == "123" );
  CPPUNIT_ASSERT ( data7() == "123" );
  CPPUNIT_ASSERT ( data7.get() == "123" );
  data7.get() += "4";
  CPPUNIT_ASSERT ( data7.get() == "1234" );
  data7.set("abc");
  CPPUNIT_ASSERT ( data7.get() == "abc" );
  data7 << "xyz";
  CPPUNIT_ASSERT ( data7.get() == "xyz" );
}

//----------------------------------------------------------------------
void FDataTest::copyConstructorTest()
{
  // value copy
  auto data1 = finalcut::FData<int>(2);
  CPPUNIT_ASSERT ( data1.isInitializedCopy() );
  CPPUNIT_ASSERT ( data1.get() == 2 );
  auto data2 = finalcut::FData<int>(data1);
  CPPUNIT_ASSERT ( data2.isInitializedCopy() );
  data1.get()++;
  CPPUNIT_ASSERT ( data1.get() == 3 );
  CPPUNIT_ASSERT ( data2.get() == 2 );

  // reference copy
  uInt n = 100;
  auto data3 = finalcut::FData<uInt>(n);
  CPPUNIT_ASSERT ( data3.isInitializedReference() );
  CPPUNIT_ASSERT ( data3.get() == 100 );
  auto data4 = finalcut::FData<uInt>(data3);
  CPPUNIT_ASSERT ( data4.isInitializedReference() );
  data3.get()--;
  CPPUNIT_ASSERT ( data3.get() == 99 );
  CPPUNIT_ASSERT ( data4.get() == 99 );
}

//----------------------------------------------------------------------
void FDataTest::moveConstructorTest()
{
  // value copy
  auto data1 = finalcut::FData<int>(-5);
  CPPUNIT_ASSERT ( data1.isInitializedCopy() );
  CPPUNIT_ASSERT ( data1.get() == -5 );
  auto data1_copy = data1;
  auto data2 = finalcut::FData<int>(std::move(data1));
  CPPUNIT_ASSERT ( data2.isInitializedCopy() );
  data1_copy.get() += 10;
  CPPUNIT_ASSERT ( data1_copy.get() == 5 );
  CPPUNIT_ASSERT ( data2.get() == -5 );

  // reference copy
  long int n = 0xfffffff;
  auto data3 = finalcut::FData<long int>(n);
  CPPUNIT_ASSERT ( data3.isInitializedReference() );
  CPPUNIT_ASSERT ( data3.get() == 0xfffffff );
  auto data3_copy = data3;
  auto data4 = finalcut::FData<long int>(std::move(data3));
  CPPUNIT_ASSERT ( data4.isInitializedReference() );
  data3_copy.get()++;
  CPPUNIT_ASSERT ( data3_copy.get() == 0x10000000 );
  CPPUNIT_ASSERT ( data4.get() == 0x10000000 );
}

//----------------------------------------------------------------------
void FDataTest::copyAssignmentTest()
{
  // value copy
  auto data1 = finalcut::FData<char>(123);
  CPPUNIT_ASSERT ( data1.isInitializedCopy() );
  CPPUNIT_ASSERT ( data1.get() == 123 );
  finalcut::FData<char> data2{0};
  data2 = data1;
  CPPUNIT_ASSERT ( data2.isInitializedCopy() );
  data1.get() -= 100;
  CPPUNIT_ASSERT ( data1.get() == 23 );
  CPPUNIT_ASSERT ( data2.get() == 123 );

  // reference copy
  double c = 299792458.0;  // Speed of light
  auto data3 = finalcut::FData<double>(c);
  CPPUNIT_ASSERT ( data3.isInitializedReference() );
  CPPUNIT_ASSERT ( data3.get() == 299792458 );
  finalcut::FData<double> data4{0.0};
  data4 = data3;
  CPPUNIT_ASSERT ( data4.isInitializedReference() );
  data4.get() -= 343.2;  // Speed of sound

  CPPUNIT_ASSERT ( data3.get() == 299792114.8 );
  CPPUNIT_ASSERT ( data4.get() == 299792114.8 );
}

//----------------------------------------------------------------------
void FDataTest::moveAssignmentTest()
{
  // value copy
  auto data1 = finalcut::FData<float>(9.81F);
  CPPUNIT_ASSERT ( data1.isInitializedCopy() );
  CPPUNIT_ASSERT ( data1.get() == 9.81F );
  finalcut::FData<float> data2{0};
  auto data1_copy = data1;
  data2 = std::move(data1);
  CPPUNIT_ASSERT ( data2.isInitializedCopy() );
  data1_copy.get() -= 0.81;
  CPPUNIT_ASSERT ( data1_copy.get() == 9.0F );
  CPPUNIT_ASSERT ( data2.get() == 9.81F );

  // reference copy
  auto au = 149597870700LL;  // Astronomical unit
  auto data3 = finalcut::FData<long long int>(au);
  CPPUNIT_ASSERT ( data3.isInitializedReference() );
  CPPUNIT_ASSERT ( data3.get() == 149597870700LL );
  finalcut::FData<long long int> data4{0};
  auto data3_copy = data3;
  data4 = std::move(data3);
  CPPUNIT_ASSERT ( data4.isInitializedReference() );
  data4.get() /= 2LL;
  CPPUNIT_ASSERT ( data3_copy.get() == 74798935350LL );
  CPPUNIT_ASSERT ( data4.get() == 74798935350LL );
}

//----------------------------------------------------------------------
void FDataTest::makeFDataTest()
{
  finalcut::FDataAccess* data_pointer{nullptr};

  // Array
  using ThreeInts = int[3];
  ThreeInts int_array{2, 1, 4};
  data_pointer = finalcut::makeFData(int_array);
  const auto& ints = static_cast<finalcut::FData<finalcut::clean_fdata_t<ThreeInts>>&>(*data_pointer).get();
  CPPUNIT_ASSERT ( ints[0] == 2 );
  CPPUNIT_ASSERT ( ints[1] == 1 );
  CPPUNIT_ASSERT ( ints[2] == 4 );
  delete data_pointer;

  // Integer
  int n = 9;
  data_pointer = finalcut::makeFData(std::move(n));
  auto& data = static_cast<finalcut::FData<int>&>(*data_pointer);
  CPPUNIT_ASSERT ( data.isInitializedCopy() );
  CPPUNIT_ASSERT ( ! data.isInitializedReference() );
  auto& n2 = data.get();
  CPPUNIT_ASSERT ( n2 == 9 );
  n2++;
  CPPUNIT_ASSERT ( n2 == 10 );
  CPPUNIT_ASSERT ( data() == 10 );
  CPPUNIT_ASSERT ( data_pointer->get<int>() == 10 );
  data_pointer->set<int>(33);
  CPPUNIT_ASSERT ( data_pointer->get<int>() == 33 );
  delete data_pointer;

  // Function
  data_pointer = finalcut::makeFData(my_function2);
  const auto& func = static_cast<finalcut::FData<finalcut::clean_fdata_t<long int (*) (long int)>>&>(*data_pointer).get();
  CPPUNIT_ASSERT ( func(128) == 256 );
  data_pointer->set<long int (*) (long int)>(&my_function3);
  CPPUNIT_ASSERT ( func(128) == 384 );
  delete data_pointer;
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FDataTest);

// The general unit test main part
#include <main-test.inc>
