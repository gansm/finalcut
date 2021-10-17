/***********************************************************************
* ftermdata-test.cpp - FTermData unit tests                            *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2021 Markus Gans                                      *
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

class FTermDataTest : public CPPUNIT_NS::TestFixture
{
  public:
    FTermDataTest()
    { }

  protected:
    void classNameTest();
    void defaultDataTest();
    void TermTypeTest();
    void dataTest();

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FTermDataTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (defaultDataTest);
    CPPUNIT_TEST (TermTypeTest);
    CPPUNIT_TEST (dataTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};

//----------------------------------------------------------------------
void FTermDataTest::classNameTest()
{
  finalcut::FTermData d;
  const finalcut::FString& classname = d.getClassName();
  CPPUNIT_ASSERT ( classname == "FTermData" );
}

//----------------------------------------------------------------------
void FTermDataTest::defaultDataTest()
{
  finalcut::FTermData data;
  CPPUNIT_ASSERT ( data.getEncodingList().size() == 0 );
  CPPUNIT_ASSERT ( data.getTerminalEncoding() == finalcut::Encoding::Unknown );
  CPPUNIT_ASSERT ( data.getTerminalGeometry() == finalcut::FRect() );
  CPPUNIT_ASSERT ( data.getTTYFileDescriptor() ==  -1 );
  CPPUNIT_ASSERT ( data.getBaudrate() == 0 );
  CPPUNIT_ASSERT_CSTRING ( data.getTermType().data(), "" );
  CPPUNIT_ASSERT_CSTRING ( data.getTermFileName().data(), "" );
  CPPUNIT_ASSERT ( data.getGnomeTerminalID() == 0 );
  CPPUNIT_ASSERT ( data.getKittyVersion().primary == 0 );
  CPPUNIT_ASSERT ( data.getKittyVersion().secondary == 0 );
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
  CPPUNIT_ASSERT ( data.isInAlternateScreen() == false );
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
  auto& encoding_list = data.getEncodingList();
  encoding_list["UTF8"]  = finalcut::Encoding::UTF8;
  encoding_list["UTF-8"] = finalcut::Encoding::UTF8;
  encoding_list["VT100"] = finalcut::Encoding::VT100;
  encoding_list["PC"]    = finalcut::Encoding::PC;
  encoding_list["ASCII"] = finalcut::Encoding::ASCII;
  auto& enc_list = data.getEncodingList();
  CPPUNIT_ASSERT ( enc_list.size() == 5 );
  CPPUNIT_ASSERT ( enc_list["UTF8"] == finalcut::Encoding::UTF8 );
  CPPUNIT_ASSERT ( enc_list["UTF-8"] == finalcut::Encoding::UTF8 );
  CPPUNIT_ASSERT ( enc_list["VT100"] == finalcut::Encoding::VT100 );
  CPPUNIT_ASSERT ( enc_list["PC"] == finalcut::Encoding::PC );
  CPPUNIT_ASSERT ( enc_list["ASCII"] == finalcut::Encoding::ASCII );

  CPPUNIT_ASSERT ( data.getTerminalEncoding() == finalcut::Encoding::Unknown );
  data.setTermEncoding(finalcut::Encoding::UTF8);
  CPPUNIT_ASSERT ( data.getTerminalEncoding() == finalcut::Encoding::UTF8 );
  data.setTermEncoding(finalcut::Encoding::VT100);
  CPPUNIT_ASSERT ( data.getTerminalEncoding() == finalcut::Encoding::VT100 );
  data.setTermEncoding(finalcut::Encoding::PC);
  CPPUNIT_ASSERT ( data.getTerminalEncoding() == finalcut::Encoding::PC );
  data.setTermEncoding(finalcut::Encoding::ASCII);
  CPPUNIT_ASSERT ( data.getTerminalEncoding() == finalcut::Encoding::ASCII );
  data.setTermEncoding(finalcut::Encoding::Unknown);
  CPPUNIT_ASSERT ( data.getTerminalEncoding() == finalcut::Encoding::Unknown );

  CPPUNIT_ASSERT ( data.getCharSubstitutionMap().isEmpty() );
  auto& character_map = data.getCharSubstitutionMap();
  character_map.setCharMapping({L'€', 'E'});
  character_map.setCharMapping({L'µ', L'u'});
  character_map.setCharMapping({wchar_t(finalcut::UniChar::Bullet), '*'});
  character_map.setCharMapping({wchar_t(finalcut::UniChar::FullBlock), wchar_t(finalcut::UniChar::MediumShade)});
  auto& char_map = data.getCharSubstitutionMap();
  CPPUNIT_ASSERT ( ! char_map.isEmpty() );
  CPPUNIT_ASSERT ( char_map.getMappedChar(L'€') == 'E' );
  CPPUNIT_ASSERT ( char_map.getMappedChar(L'µ') == L'u' );
  CPPUNIT_ASSERT ( char_map.getMappedChar(wchar_t(finalcut::UniChar::Bullet)) == '*' );
  CPPUNIT_ASSERT ( char_map.getMappedChar(wchar_t(finalcut::UniChar::FullBlock))
                   == wchar_t(finalcut::UniChar::MediumShade) );

  CPPUNIT_ASSERT ( data.getTerminalGeometry() == finalcut::FRect() );
  data.getTerminalGeometry().setSize(10, 10);
  data.getTerminalGeometry().setPos(3, 5);
  CPPUNIT_ASSERT ( data.getTerminalGeometry() == finalcut::FRect(3, 5, 10, 10) );

  CPPUNIT_ASSERT ( data.getTTYFileDescriptor() ==  -1 );
  data.setTTYFileDescriptor (1);
  CPPUNIT_ASSERT ( data.getTTYFileDescriptor() ==  1 );
  CPPUNIT_ASSERT ( data.getBaudrate() == 0 );
  data.setBaudrate(38400);
  CPPUNIT_ASSERT ( data.getBaudrate() != 9600 );
  CPPUNIT_ASSERT ( data.getBaudrate() == 38400 );

  CPPUNIT_ASSERT ( data.getTermType() == "" );
  data.setTermType("linux");
  CPPUNIT_ASSERT ( data.getTermType() == "linux" );

  CPPUNIT_ASSERT ( data.getTermFileName() == "" );
  data.setTermFileName("/dev/pts/2");
  CPPUNIT_ASSERT ( data.getTermFileName() == "/dev/pts/2" );

  data.setGnomeTerminalID(5402);
  CPPUNIT_ASSERT ( data.getGnomeTerminalID() == 5402 );

  data.setKittyVersion( {0, 13} );
  CPPUNIT_ASSERT ( data.getKittyVersion().primary == 0 );
  CPPUNIT_ASSERT ( data.getKittyVersion().secondary == 13 );

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

  CPPUNIT_ASSERT ( data.isInAlternateScreen() == false );
  data.setAlternateScreenInUse (true);
  CPPUNIT_ASSERT ( data.isInAlternateScreen() == true );


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
  data.setTermResized (true);  // one signal
  CPPUNIT_ASSERT ( data.hasTermResized() == true );
  data.setTermResized (true);  // two signals
  CPPUNIT_ASSERT ( data.hasTermResized() == true );
  data.setTermResized (false);  // one signal
  CPPUNIT_ASSERT ( data.hasTermResized() == true );
  data.setTermResized (false);  // no signal
  CPPUNIT_ASSERT ( data.hasTermResized() == false );
}

//----------------------------------------------------------------------
void FTermDataTest::TermTypeTest()
{
  finalcut::FTermData data;

  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::xterm) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::ansi) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::rxvt) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::urxvt) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kde_konsole) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::gnome_terminal) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::putty) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::win_terminal) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tera_term) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::cygwin) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mintty) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::linux_con) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::freebsd_con) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::netbsd_con) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::sun_con) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::screen) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tmux) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kterm) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mlterm) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kitty) );

  CPPUNIT_ASSERT ( ! data.isTermType( finalcut::FTermType::xterm
                                    | finalcut::FTermType::ansi
                                    | finalcut::FTermType::rxvt
                                    | finalcut::FTermType::urxvt
                                    | finalcut::FTermType::kde_konsole
                                    | finalcut::FTermType::gnome_terminal
                                    | finalcut::FTermType::putty
                                    | finalcut::FTermType::win_terminal
                                    | finalcut::FTermType::tera_term
                                    | finalcut::FTermType::cygwin
                                    | finalcut::FTermType::mintty
                                    | finalcut::FTermType::linux_con
                                    | finalcut::FTermType::freebsd_con
                                    | finalcut::FTermType::netbsd_con
                                    | finalcut::FTermType::openbsd_con
                                    | finalcut::FTermType::sun_con
                                    | finalcut::FTermType::screen
                                    | finalcut::FTermType::tmux
                                    | finalcut::FTermType::kterm
                                    | finalcut::FTermType::mlterm
                                    | finalcut::FTermType::kitty) );

  data.setTermType (finalcut::FTermType::ansi);

  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::xterm) );
  CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::ansi) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::rxvt) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::urxvt) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kde_konsole) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::gnome_terminal) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::putty) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::win_terminal) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tera_term) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::cygwin) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mintty) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::linux_con) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::freebsd_con) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::netbsd_con) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::sun_con) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::screen) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tmux) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kterm) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mlterm) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kitty) );

  CPPUNIT_ASSERT ( data.isTermType( finalcut::FTermType::xterm
                                  | finalcut::FTermType::ansi
                                  | finalcut::FTermType::rxvt
                                  | finalcut::FTermType::urxvt
                                  | finalcut::FTermType::kde_konsole
                                  | finalcut::FTermType::gnome_terminal
                                  | finalcut::FTermType::putty
                                  | finalcut::FTermType::win_terminal
                                  | finalcut::FTermType::tera_term
                                  | finalcut::FTermType::cygwin
                                  | finalcut::FTermType::mintty
                                  | finalcut::FTermType::linux_con
                                  | finalcut::FTermType::freebsd_con
                                  | finalcut::FTermType::netbsd_con
                                  | finalcut::FTermType::openbsd_con
                                  | finalcut::FTermType::sun_con
                                  | finalcut::FTermType::screen
                                  | finalcut::FTermType::tmux
                                  | finalcut::FTermType::kterm
                                  | finalcut::FTermType::mlterm
                                  | finalcut::FTermType::kitty) );

  data.unsetTermType (finalcut::FTermType::ansi);
  data.setTermType (finalcut::FTermType::xterm);
  data.setTermType (finalcut::FTermType::putty);

  CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::xterm) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::ansi) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::rxvt) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::urxvt) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kde_konsole) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::gnome_terminal) );
  CPPUNIT_ASSERT ( data.isTermType(finalcut::FTermType::putty) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::win_terminal) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tera_term) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::cygwin) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mintty) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::linux_con) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::freebsd_con) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::netbsd_con) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::openbsd_con) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::sun_con) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::screen) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::tmux) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kterm) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::mlterm) );
  CPPUNIT_ASSERT ( ! data.isTermType(finalcut::FTermType::kitty) );

  CPPUNIT_ASSERT ( data.isTermType( finalcut::FTermType::xterm
                                  | finalcut::FTermType::ansi
                                  | finalcut::FTermType::rxvt
                                  | finalcut::FTermType::urxvt
                                  | finalcut::FTermType::kde_konsole
                                  | finalcut::FTermType::gnome_terminal
                                  | finalcut::FTermType::putty
                                  | finalcut::FTermType::win_terminal
                                  | finalcut::FTermType::tera_term
                                  | finalcut::FTermType::cygwin
                                  | finalcut::FTermType::mintty
                                  | finalcut::FTermType::linux_con
                                  | finalcut::FTermType::freebsd_con
                                  | finalcut::FTermType::netbsd_con
                                  | finalcut::FTermType::openbsd_con
                                  | finalcut::FTermType::sun_con
                                  | finalcut::FTermType::screen
                                  | finalcut::FTermType::tmux
                                  | finalcut::FTermType::kterm
                                  | finalcut::FTermType::mlterm
                                  | finalcut::FTermType::kitty) );

  const auto v1_enum = finalcut::FTermType::xterm;
  const auto v1_value = finalcut::FTermTypeT(finalcut::FTermType::xterm);
  const auto v2_enum = finalcut::FTermType::putty;
  const auto v2_value = finalcut::FTermTypeT(finalcut::FTermType::putty);

  CPPUNIT_ASSERT ( ( v1_enum |  v2_enum) == (v1_value | v2_value) );
  CPPUNIT_ASSERT ( ( v1_enum | v2_value) == (v1_value | v2_value) );
  CPPUNIT_ASSERT ( (v1_value |  v2_enum) == (v1_value | v2_value) );
  CPPUNIT_ASSERT ( (v1_value | v2_value) == (v1_value | v2_value) );

  CPPUNIT_ASSERT ( ( v1_enum |  v2_enum) == (v1_value |  v2_enum) );
  CPPUNIT_ASSERT ( ( v1_enum | v2_value) == (v1_value |  v2_enum) );
  CPPUNIT_ASSERT ( (v1_value |  v2_enum) == (v1_value |  v2_enum) );
  CPPUNIT_ASSERT ( (v1_value | v2_value) == (v1_value |  v2_enum) );

  CPPUNIT_ASSERT ( ( v1_enum |  v2_enum) == ( v1_enum | v2_value) );
  CPPUNIT_ASSERT ( ( v1_enum | v2_value) == ( v1_enum | v2_value) );
  CPPUNIT_ASSERT ( (v1_value |  v2_enum) == ( v1_enum | v2_value) );
  CPPUNIT_ASSERT ( (v1_value | v2_value) == ( v1_enum | v2_value) );

  CPPUNIT_ASSERT ( ( v1_enum |  v2_enum) == ( v1_enum |  v2_enum) );
  CPPUNIT_ASSERT ( ( v1_enum | v2_value) == ( v1_enum |  v2_enum) );
  CPPUNIT_ASSERT ( (v1_value |  v2_enum) == ( v1_enum |  v2_enum) );
  CPPUNIT_ASSERT ( (v1_value | v2_value) == ( v1_enum |  v2_enum) );

  auto mask = finalcut::FTermTypeT(0);
  CPPUNIT_ASSERT ( ! data.isTermType(mask) );
  mask = finalcut::FTermType::xterm | finalcut::FTermType::putty;
  CPPUNIT_ASSERT ( data.isTermType(mask) );
  mask = mask | finalcut::FTermType::rxvt;
  mask = finalcut::FTermType::mintty | mask;
  data.unsetTermType (finalcut::FTermType::xterm);
  CPPUNIT_ASSERT ( data.isTermType(mask) );
  data.unsetTermType (finalcut::FTermType::putty);
  CPPUNIT_ASSERT ( ! data.isTermType(mask) );
  mask = 0;
  CPPUNIT_ASSERT ( ! data.isTermType(mask) );
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FTermDataTest);

// The general unit test main part
#include <main-test.inc>

