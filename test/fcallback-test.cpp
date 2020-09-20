/***********************************************************************
* fcallback-test.cpp - FCallback unit tests                            *
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
// class Widget
//----------------------------------------------------------------------

class Widget
{
  public:
    template <typename... Args>
    void addCallback (const finalcut::FString& cb_signal, Args&&... args)
    {
      cb.addCallback (cb_signal, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void delCallback (Args&&... args)
    {
      cb.delCallback (std::forward<Args>(args)...);
    }

    void emitCallback (const finalcut::FString& emit_signal)
    {
      cb.emitCallback (emit_signal);
    }

  private:
    finalcut::FCallback cb{};
};


//----------------------------------------------------------------------
// class cb_class
//----------------------------------------------------------------------

class cb_class final : public finalcut::FWidget
{
  public:
    cb_class (int i, finalcut::FWidget* parent)
      : finalcut::FWidget{parent}
      , data{i}
    { }

    void cb_method_ptr (int* value)
    {
      (*value)--;
    }

    void cb_method_ptr_const (int* value) const
    {
      *value -= 4;
    }

    void cb_method_ref (int& value)
    {
      value -= data;
    }

    void cb_method_ref_const (int& value) const
    {
      value -= (2 * data);
    }

  private:
    int data;
};

//----------------------------------------------------------------------
// functions
//----------------------------------------------------------------------
void cb_function_ptr (int* value)
{
  (*value)++;
}

//----------------------------------------------------------------------
void cb_function_ref (int& value)
{
  value += 2;
}

//----------------------------------------------------------------------
// class FCallbackTest
//----------------------------------------------------------------------

class FCallbackTest : public CPPUNIT_NS::TestFixture
{
  public:
    FCallbackTest()
    { }

  protected:
    void classNameTest();
    void memberFunctionPointerCallbackTest();
    void instanceWithFunctionObjectCallbackTest();
    void functionObjectCallbackTest();
    void functionObjectReferenceCallbackTest();
    void functionReferenceCallbackTest();
    void functionPointerCallbackTest();
    void ownWidgetTest();

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FCallbackTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (memberFunctionPointerCallbackTest);
    CPPUNIT_TEST (instanceWithFunctionObjectCallbackTest);
    CPPUNIT_TEST (functionObjectCallbackTest);
    CPPUNIT_TEST (functionObjectReferenceCallbackTest);
    CPPUNIT_TEST (functionReferenceCallbackTest);
    CPPUNIT_TEST (functionPointerCallbackTest);
    CPPUNIT_TEST (ownWidgetTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();

    // Data member
    static finalcut::FWidget root_widget;
};

// static class attributes
finalcut::FWidget FCallbackTest::root_widget{nullptr};

//----------------------------------------------------------------------
void FCallbackTest::classNameTest()
{
  const finalcut::FCallback cb;
  const finalcut::FString& classname = cb.getClassName();
  CPPUNIT_ASSERT ( classname == "FCallback" );
}

//----------------------------------------------------------------------
void FCallbackTest::memberFunctionPointerCallbackTest()
{
  finalcut::FCallback cb{};
  cb_class c{5, &root_widget};
  int i{75};
  using Object = decltype(&c);
  using MemberFunctionPointer = decltype(&cb_class::cb_method_ptr);

  CPPUNIT_ASSERT ( 0 == std::is_member_function_pointer<Object>::value );
  CPPUNIT_ASSERT ( 0 == std::is_function<typename std::remove_pointer<Object>::type>::value );
  CPPUNIT_ASSERT ( 0 == std::is_function<Object>::value );
  CPPUNIT_ASSERT ( 1 == std::is_pointer<Object>::value );
  CPPUNIT_ASSERT ( 1 == std::is_object<Object>::value );
  CPPUNIT_ASSERT ( 0 == std::is_class<Object>::value );

  CPPUNIT_ASSERT ( 1 == std::is_member_function_pointer<MemberFunctionPointer>::value );
  CPPUNIT_ASSERT ( 0 == std::is_function<typename std::remove_pointer<MemberFunctionPointer>::type>::value );
  CPPUNIT_ASSERT ( 0 == std::is_function<MemberFunctionPointer>::value );
  CPPUNIT_ASSERT ( 0 == std::is_pointer<MemberFunctionPointer>::value );
  CPPUNIT_ASSERT ( 1 == std::is_object<MemberFunctionPointer>::value );
  CPPUNIT_ASSERT ( 0 == std::is_class<MemberFunctionPointer>::value );

  CPPUNIT_ASSERT ( cb.getCallbackCount() == 0 );

  cb.addCallback ("clicked", &c, &cb_class::cb_method_ptr, &i);
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 1 );

  cb.addCallback ("activate", &c, &cb_class::cb_method_ptr_const, &i);
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 2 );
  CPPUNIT_ASSERT ( i == 75 );

  cb.emitCallback ("pressed");
  CPPUNIT_ASSERT ( i == 75 );

  cb.emitCallback("clicked");
  CPPUNIT_ASSERT ( i == 74 );

  cb.delCallback ("clicked");
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 1 );

  cb.emitCallback ("clicked");
  CPPUNIT_ASSERT ( i == 74 );

  cb.emitCallback ("activate");
  CPPUNIT_ASSERT ( i == 70 );


  cb.delCallback ("activate");
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 0 );

  cb.emitCallback ("activate");
  CPPUNIT_ASSERT ( i == 70 );

  cb.addCallback ("on", &c, &cb_class::cb_method_ref, std::ref(i));
  cb.addCallback ("on", &c, &cb_class::cb_method_ref_const, std::ref(i));
  cb.emitCallback ("on");
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 2 );
  CPPUNIT_ASSERT ( i == 55 );

  cb.delCallback (&c);
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 0 );
  cb.emitCallback ("on");
  CPPUNIT_ASSERT ( i == 55 );
}

//----------------------------------------------------------------------
void FCallbackTest::instanceWithFunctionObjectCallbackTest()
{
  finalcut::FCallback cb{};
  cb_class c{15, &root_widget};
  int i{100};
  auto func1 = std::bind(&cb_class::cb_method_ptr, &c, &i);
  auto func2 = std::bind(&cb_class::cb_method_ptr_const, &c, &i);
  auto func3 = std::bind(&cb_class::cb_method_ref, &c, std::ref(i));
  auto func4 = std::bind(&cb_class::cb_method_ref_const, &c, std::ref(i));
  auto func5 = std::bind(&cb_class::cb_method_ptr, &c, std::placeholders::_1);
  auto func6 = std::bind(&cb_class::cb_method_ref, &c, std::placeholders::_1);
  using Object = decltype(&c);
  using ClassObject = decltype(func1);

  CPPUNIT_ASSERT ( 0 == std::is_member_function_pointer<Object>::value );
  CPPUNIT_ASSERT ( 0 == std::is_function<typename std::remove_pointer<Object>::type>::value );
  CPPUNIT_ASSERT ( 0 == std::is_function<Object>::value );
  CPPUNIT_ASSERT ( 1 == std::is_pointer<Object>::value );
  CPPUNIT_ASSERT ( 1 == std::is_object<Object>::value );
  CPPUNIT_ASSERT ( 0 == std::is_class<Object>::value );

  CPPUNIT_ASSERT ( 0 == std::is_member_function_pointer<ClassObject>::value );
  CPPUNIT_ASSERT ( 0 == std::is_function<typename std::remove_pointer<ClassObject>::type>::value );
  CPPUNIT_ASSERT ( 0 == std::is_function<ClassObject>::value );
  CPPUNIT_ASSERT ( 0 == std::is_pointer<ClassObject>::value );
  CPPUNIT_ASSERT ( 1 == std::is_object<ClassObject>::value );
  CPPUNIT_ASSERT ( 1 == std::is_class<ClassObject>::value );

  CPPUNIT_ASSERT ( cb.getCallbackCount() == 0 );

  cb.addCallback ("clicked", &c, std::move(func1));
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 1 );

  cb.addCallback ("activate", &c, std::move(func2));
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 2 );
  CPPUNIT_ASSERT ( i == 100 );

  cb.emitCallback ("pressed");
  CPPUNIT_ASSERT ( i == 100 );

  cb.emitCallback("clicked");
  CPPUNIT_ASSERT ( i == 99 );

  cb.delCallback ("clicked");
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 1 );

  cb.emitCallback ("clicked");
  CPPUNIT_ASSERT ( i == 99 );

  cb.emitCallback ("activate");
  CPPUNIT_ASSERT ( i == 95 );

  cb.delCallback ("activate");
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 0 );

  cb.emitCallback ("activate");
  CPPUNIT_ASSERT ( i == 95 );

  cb.addCallback ("on", &c, std::move(func3));
  cb.addCallback ("on", &c, std::move(func4));
  cb.emitCallback ("on");
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 2 );
  CPPUNIT_ASSERT ( i == 50 );

  cb.delCallback (&c);
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 0 );
  cb.emitCallback ("on");
  CPPUNIT_ASSERT ( i == 50 );

  cb.addCallback ("enabled", &c, std::move(func5), &i);
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 1 );

  cb.addCallback ("joined", &c, std::move(func6), std::ref(i));
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 2 );

  cb.emitCallback("enabled");
  CPPUNIT_ASSERT ( i == 49 );

  cb.delCallback ("enabled", &c);
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 1 );

  cb.emitCallback("joined");
  CPPUNIT_ASSERT ( i == 34 );

  cb.delCallback ("joined", &c);
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 0 );
}

//----------------------------------------------------------------------
void FCallbackTest::functionObjectCallbackTest()
{
  finalcut::FCallback cb{};
  int i1{2};
  int i2{3};
  auto lambda = [] (int& a, const int& b)
                {
                  a += b;
                };
  using Lambda = decltype(lambda);

  CPPUNIT_ASSERT ( 0 == std::is_member_function_pointer<Lambda>::value );
  CPPUNIT_ASSERT ( 0 == std::is_function<typename std::remove_pointer<Lambda>::type>::value );
  CPPUNIT_ASSERT ( 0 == std::is_function<Lambda>::value );
  CPPUNIT_ASSERT ( 0 == std::is_pointer<Lambda>::value );
  CPPUNIT_ASSERT ( 1 == std::is_object<Lambda>::value );
  CPPUNIT_ASSERT ( 1 == std::is_class<Lambda>::value );

  cb.addCallback
  (
    "clicked",
    std::ref(lambda),
    std::ref(i1),
    std::cref(i2)
  );
  cb.addCallback
  (
    "changed",
    [] (int* a)
    {
      (*a)++;
    },
    &i2
  );

  CPPUNIT_ASSERT ( cb.getCallbackCount() == 2 );
  CPPUNIT_ASSERT ( i1 == 2 );
  cb.emitCallback ("clicked");
  CPPUNIT_ASSERT ( i1 == 5 );

  i2 = -3;
  cb.emitCallback ("clicked");
  CPPUNIT_ASSERT ( i1 == 2 );

  cb.emitCallback ("changed");
  CPPUNIT_ASSERT ( i2 == -2 );

  cb.emitCallback ("clicked");
  CPPUNIT_ASSERT ( i1 == 0 );

  cb.delCallback("changed");
  cb.emitCallback ("changed");
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 1 );
  CPPUNIT_ASSERT ( i2 == -2 );

  cb.delCallback();
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 0 );
}

//----------------------------------------------------------------------
void FCallbackTest::functionObjectReferenceCallbackTest()
{
  finalcut::FCallback cb{};
  int i{4};
  auto lambda1 = [] (int* value)
                 {
                   *value = *value << 8;
                 };
  auto lambda2 = [] (int& value)
                 {
                   value = value >> 4;
                 };
  using NonUnionClass = decltype(lambda1);

  CPPUNIT_ASSERT ( 0 == std::is_member_function_pointer<NonUnionClass>::value );
  CPPUNIT_ASSERT ( 0 == std::is_function<typename std::remove_pointer<NonUnionClass>::type>::value );
  CPPUNIT_ASSERT ( 0 == std::is_function<NonUnionClass>::value );
  CPPUNIT_ASSERT ( 0 == std::is_pointer<NonUnionClass>::value );
  CPPUNIT_ASSERT ( 1 == std::is_object<NonUnionClass>::value );
  CPPUNIT_ASSERT ( 1 == std::is_class<NonUnionClass>::value );

  cb.addCallback ("toggled", lambda1, &i);
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 1 );
  CPPUNIT_ASSERT ( i == 4 );

  cb.emitCallback ("toggled");
  CPPUNIT_ASSERT ( i == 1024 );

  cb.addCallback ("row-selected", lambda2, std::ref(i));
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 2 );
  CPPUNIT_ASSERT ( i == 1024 );

  cb.emitCallback ("row-selected");
  CPPUNIT_ASSERT ( i == 64 );

  cb.delCallback();
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 0 );
}

//----------------------------------------------------------------------
void FCallbackTest::functionReferenceCallbackTest()
{
  finalcut::FCallback cb{};
  int i1{22};
  int i2{11};
  using Function = decltype(cb_function_ptr);

  CPPUNIT_ASSERT ( 0 == std::is_member_function_pointer<Function>::value );
  CPPUNIT_ASSERT ( 1 == std::is_function<typename std::remove_pointer<Function>::type>::value );
  CPPUNIT_ASSERT ( 1 == std::is_function<Function>::value );
  CPPUNIT_ASSERT ( 0 == std::is_pointer<Function>::value );
  CPPUNIT_ASSERT ( 0 == std::is_object<Function>::value );
  CPPUNIT_ASSERT ( 0 == std::is_class<Function>::value );

  CPPUNIT_ASSERT ( cb.getCallbackCount() == 0 );

  cb.addCallback ("clicked", cb_function_ptr, &i1);
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 1 );
  CPPUNIT_ASSERT ( i1 == 22 );

  cb.emitCallback ("clicked");
  CPPUNIT_ASSERT ( i1 == 23 );

  cb.addCallback ("clicked", cb_function_ref, std::ref(i2));
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 2 );
  CPPUNIT_ASSERT ( i2 == 11 );

  cb.emitCallback ("clicked");
  CPPUNIT_ASSERT ( i1 == 24 );
  CPPUNIT_ASSERT ( i2 == 13 );

  cb.delCallback (&cb_function_ptr);  // Delete via function pointer
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 1 );

  cb.emitCallback ("clicked");
  CPPUNIT_ASSERT ( i1 == 24 );
  CPPUNIT_ASSERT ( i2 == 15 );

  cb.delCallback (cb_function_ref);  // Delete via function
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 0 );

  cb.emitCallback ("clicked");
  CPPUNIT_ASSERT ( i1 == 24 );
  CPPUNIT_ASSERT ( i2 == 15 );
}

//----------------------------------------------------------------------
void FCallbackTest::functionPointerCallbackTest()
{
  finalcut::FCallback cb{};
  int i1{5};
  int i2{9};
  using FunctionPointer = decltype(&cb_function_ptr);

  CPPUNIT_ASSERT ( 0 == std::is_member_function_pointer<FunctionPointer>::value );
  CPPUNIT_ASSERT ( 1 == std::is_function<typename std::remove_pointer<FunctionPointer>::type>::value );
  CPPUNIT_ASSERT ( 0 == std::is_function<FunctionPointer>::value );
  CPPUNIT_ASSERT ( 1 == std::is_pointer<FunctionPointer>::value );
  CPPUNIT_ASSERT ( 1 == std::is_object<FunctionPointer>::value );
  CPPUNIT_ASSERT ( 0 == std::is_class<FunctionPointer>::value );

  CPPUNIT_ASSERT ( cb.getCallbackCount() == 0 );

  cb.addCallback ("clicked", &cb_function_ptr, &i1);
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 1 );
  CPPUNIT_ASSERT ( i1 == 5 );

  cb.emitCallback ("clicked");
  CPPUNIT_ASSERT ( i1 == 6 );

  cb.addCallback ("clicked", &cb_function_ref, std::ref(i2));
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 2 );
  CPPUNIT_ASSERT ( i2 == 9 );

  cb.emitCallback ("clicked");
  CPPUNIT_ASSERT ( i1 == 7 );
  CPPUNIT_ASSERT ( i2 == 11 );

  cb.delCallback (&cb_function_ptr);
  CPPUNIT_ASSERT ( cb.getCallbackCount() == 1 );

  cb.emitCallback ("clicked");
  CPPUNIT_ASSERT ( i1 == 7 );
  CPPUNIT_ASSERT ( i2 == 13 );
}

//----------------------------------------------------------------------
void FCallbackTest::ownWidgetTest()
{
  Widget w;
  int value = 3141593;
  int* i = &value;
  w.addCallback("focus-in", cb_function_ptr, i);
  CPPUNIT_ASSERT ( value == 3141593 );

  w.emitCallback("focus-in");
  CPPUNIT_ASSERT ( value == 3141594 );

  w.emitCallback("focus-in");
  CPPUNIT_ASSERT ( value == 3141595 );

  w.emitCallback("focus-in");
  CPPUNIT_ASSERT ( value == 3141596 );

  w.delCallback();

  w.emitCallback("focus-in");
  CPPUNIT_ASSERT ( value != 3141597 );
  CPPUNIT_ASSERT ( value == 3141596 );
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FCallbackTest);

// The general unit test main part
#include <main-test.inc>
