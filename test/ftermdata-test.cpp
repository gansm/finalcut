/***********************************************************************
* ftermdata-test.cpp - FTermData unit tests                            *
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

#define CPPUNIT_ASSERT_CSTRING(expected, actual) \
            check_c_string (expected, actual, CPPUNIT_SOURCELINE())

//----------------------------------------------------------------------
void check_c_string ( const char* s1
                    , const char* s2
                    , CppUnit::SourceLine sourceLine )
{
  if ( s1 == 0 && s2 == 0 )  // Strings are equal
    return;

  if ( s1 && s2 && std::strcmp (s1, s2) == 0 )  // Strings are equal
      return;

  ::CppUnit::Asserter::fail ("Strings are not equal", sourceLine);
}


//----------------------------------------------------------------------
// class FTermDataTest
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FTermDataTest : public CPPUNIT_NS::TestFixture
{
  public:
    FTermDataTest()
    { }

  protected:
    void classNameTest();
    void defaultDataTest();
    void dataTest();

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FTermDataTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (defaultDataTest);
    CPPUNIT_TEST (dataTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};
#pragma pack(pop)

//----------------------------------------------------------------------
void FTermDataTest::classNameTest()
{
  finalcut::FTermData d;
  const char* const classname = d.getClassName();
  CPPUNIT_ASSERT ( std::strcmp(classname, "FTermData") == 0 );
}

//----------------------------------------------------------------------
void FTermDataTest::defaultDataTest()
{
  finalcut::FTermData data;
  CPPUNIT_ASSERT ( data.getEncodingList().size() == 0 );
  CPPUNIT_ASSERT ( data.getTermEncoding() == finalcut::fc::UNKNOWN );
  CPPUNIT_ASSERT ( data.getTermGeometry() == finalcut::FRect() );
  CPPUNIT_ASSERT ( data.getTTYFileDescriptor() ==  -1 );
  CPPUNIT_ASSERT ( data.getBaudrate() == 0 );
  CPPUNIT_ASSERT_CSTRING ( data.getTermType(), C_STR("") );
  CPPUNIT_ASSERT_CSTRING ( data.getTermFileName(), C_STR("") );
  CPPUNIT_ASSERT ( data.getXtermFont() == finalcut::FString() );
  CPPUNIT_ASSERT ( data.getXtermTitle() == finalcut::FString() );

#if DEBUG
  CPPUNIT_ASSERT ( data.getFramebufferBpp() == -1 );
#endif

  CPPUNIT_ASSERT ( data.hasShadowCharacter() == true );
  CPPUNIT_ASSERT ( data.hasHalfBlockCharacter() == true );
  CPPUNIT_ASSERT ( data.hasCursorOptimisation() == true );
  CPPUNIT_ASSERT ( data.isCursorHidden() == false );
  CPPUNIT_ASSERT ( data.hasAlternateScreen() == true );
  CPPUNIT_ASSERT ( data.hasASCIIConsole() == false );
  CPPUNIT_ASSERT ( data.hasVT100Console() == false );
  CPPUNIT_ASSERT ( data.hasUTF8Console() == false );
  CPPUNIT_ASSERT ( data.isUTF8() == false );
  CPPUNIT_ASSERT ( data.isNewFont() == false );
  CPPUNIT_ASSERT ( data.isVGAFont() == false );
  CPPUNIT_ASSERT ( data.isMonochron() == false );
  CPPUNIT_ASSERT ( data.hasTermResized() == false );
}

//----------------------------------------------------------------------
void FTermDataTest::dataTest()
{
  finalcut::FTermData data;

  CPPUNIT_ASSERT ( data.getEncodingList().size() == 0 );
  finalcut::FTermData::encodingMap& encoding_list = data.getEncodingList();
  encoding_list["UTF8"]  = finalcut::fc::UTF8;
  encoding_list["UTF-8"] = finalcut::fc::UTF8;
  encoding_list["VT100"] = finalcut::fc::VT100;
  encoding_list["PC"]    = finalcut::fc::PC;
  encoding_list["ASCII"] = finalcut::fc::ASCII;
  finalcut::FTermData::encodingMap& enc_list = data.getEncodingList();
  CPPUNIT_ASSERT ( enc_list.size() == 5 );
  CPPUNIT_ASSERT ( enc_list["UTF8"] == finalcut::fc::UTF8 );
  CPPUNIT_ASSERT ( enc_list["UTF-8"] == finalcut::fc::UTF8 );
  CPPUNIT_ASSERT ( enc_list["VT100"] == finalcut::fc::VT100 );
  CPPUNIT_ASSERT ( enc_list["PC"] == finalcut::fc::PC );
  CPPUNIT_ASSERT ( enc_list["ASCII"] == finalcut::fc::ASCII );

  CPPUNIT_ASSERT ( data.getTermEncoding() == finalcut::fc::UNKNOWN );
  data.setTermEncoding(finalcut::fc::UTF8);
  CPPUNIT_ASSERT ( data.getTermEncoding() == finalcut::fc::UTF8 );
  data.setTermEncoding(finalcut::fc::VT100);
  CPPUNIT_ASSERT ( data.getTermEncoding() == finalcut::fc::VT100 );
  data.setTermEncoding(finalcut::fc::PC);
  CPPUNIT_ASSERT ( data.getTermEncoding() == finalcut::fc::PC );
  data.setTermEncoding(finalcut::fc::ASCII);
  CPPUNIT_ASSERT ( data.getTermEncoding() == finalcut::fc::ASCII );
  data.setTermEncoding(finalcut::fc::UNKNOWN);
  CPPUNIT_ASSERT ( data.getTermEncoding() == finalcut::fc::UNKNOWN );

  CPPUNIT_ASSERT ( data.getTermGeometry() == finalcut::FRect() );
  data.getTermGeometry().setSize(10, 10);
  data.getTermGeometry().setPos(3, 5);
  CPPUNIT_ASSERT ( data.getTermGeometry() == finalcut::FRect(3, 5, 10, 10) );

  CPPUNIT_ASSERT ( data.getTTYFileDescriptor() ==  -1 );
  data.setTTYFileDescriptor (1);
  CPPUNIT_ASSERT ( data.getTTYFileDescriptor() ==  1 );
  CPPUNIT_ASSERT ( data.getBaudrate() == 0 );
  data.setBaudrate(38400);
  CPPUNIT_ASSERT ( data.getBaudrate() != 9600 );
  CPPUNIT_ASSERT ( data.getBaudrate() == 38400 );

  CPPUNIT_ASSERT_CSTRING ( data.getTermType(), C_STR("") );
  data.setTermType("linux");
  CPPUNIT_ASSERT_CSTRING ( data.getTermType(), C_STR("linux") );

  CPPUNIT_ASSERT_CSTRING ( data.getTermFileName(), C_STR("") );
  data.setTermFileName("/dev/pts/2");
  CPPUNIT_ASSERT_CSTRING ( data.getTermFileName(), C_STR("/dev/pts/2") );

  CPPUNIT_ASSERT ( data.getXtermFont() == finalcut::FString() );
  data.setXtermFont("terminus-20");
  CPPUNIT_ASSERT ( data.getXtermFont() == finalcut::FString("terminus-20") );

  CPPUNIT_ASSERT ( data.getXtermTitle() == finalcut::FString() );
  data.setXtermTitle("Terminal");
  CPPUNIT_ASSERT ( data.getXtermTitle() == finalcut::FString("Terminal") );

#if DEBUG
  CPPUNIT_ASSERT ( data.getFramebufferBpp() == -1 );
  data.setFramebufferBpp(32);
  CPPUNIT_ASSERT ( data.getFramebufferBpp() == 32 );
#endif

  CPPUNIT_ASSERT ( data.hasShadowCharacter() == true );
  data.supportShadowCharacter (false);
  CPPUNIT_ASSERT ( data.hasShadowCharacter() == false );

  CPPUNIT_ASSERT ( data.hasHalfBlockCharacter() == true );
  data.supportHalfBlockCharacter (false);
  CPPUNIT_ASSERT ( data.hasHalfBlockCharacter() == false );

  CPPUNIT_ASSERT ( data.hasCursorOptimisation() == true );
  data.supportCursorOptimisation (false);
  CPPUNIT_ASSERT ( data.hasCursorOptimisation() == false );

  CPPUNIT_ASSERT ( data.isCursorHidden() == false );
  data.setCursorHidden (true);
  CPPUNIT_ASSERT ( data.isCursorHidden() == true );

  CPPUNIT_ASSERT ( data.hasAlternateScreen() == true );
  data.useAlternateScreen (false);
  CPPUNIT_ASSERT ( data.hasAlternateScreen() == false );

  CPPUNIT_ASSERT ( data.hasASCIIConsole() == false );
  data.setASCIIConsole (true);
  CPPUNIT_ASSERT ( data.hasASCIIConsole() == true );

  CPPUNIT_ASSERT ( data.hasVT100Console() == false );
  data.setVT100Console (true);
  CPPUNIT_ASSERT ( data.hasVT100Console() == true );

  CPPUNIT_ASSERT ( data.hasUTF8Console() == false );
  data.setUTF8Console (true);
  CPPUNIT_ASSERT ( data.hasUTF8Console() == true );

  CPPUNIT_ASSERT ( data.isUTF8() == false );
  data.setUTF8 (true);
  CPPUNIT_ASSERT ( data.isUTF8() == true );

  CPPUNIT_ASSERT ( data.isNewFont() == false );
  data.setNewFont (true);
  CPPUNIT_ASSERT ( data.isNewFont() == true );

  CPPUNIT_ASSERT ( data.isVGAFont() == false );
  data.setVGAFont (true);
  CPPUNIT_ASSERT ( data.isVGAFont() == true );

  CPPUNIT_ASSERT ( data.isMonochron() == false );
  data.setMonochron (true);
  CPPUNIT_ASSERT ( data.isMonochron() == true );

  CPPUNIT_ASSERT ( data.hasTermResized() == false );
  data.setTermResized (true);
  CPPUNIT_ASSERT ( data.hasTermResized() == true );
}


// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FTermDataTest);

// The general unit test main part
#include <main-test.inc>

