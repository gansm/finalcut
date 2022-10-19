/***********************************************************************
* fterm_functions-test.cpp - FTerm function unit tests                 *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2021-2022 Markus Gans                                      *
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
#include <cwchar>
#include <limits>
#include <memory>

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <conemu.h>
#include <final/final.h>

//----------------------------------------------------------------------
// class FTermFunctionsTest
//----------------------------------------------------------------------

class FTermFunctionsTest : public CPPUNIT_NS::TestFixture, test::ConEmu
{
  public:
    FTermFunctionsTest()
    {
      auto ret = std::setlocale (LC_CTYPE, "en_US.UTF-8");

      if ( ! ret )
        ret = std::setlocale (LC_CTYPE, "C.UTF-8");

      if ( ret )
        fwide(stdout, 1);  // Makes stream wide-character oriented
    }

  protected:
    void env2uintTest();
    void exitMessageTest();
    void rgb2ColorIndexTest();
    void isReverseNewFontcharTest();
    void cp437Test();
    void utf8Test();
    void FullWidthHalfWidthTest();
    void combiningCharacterTest();
    void readCursorPosTest();

  private:
    // Constant
    constexpr static std::size_t NOT_FOUND = static_cast<std::size_t>(-1);

    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FTermFunctionsTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (env2uintTest);
    CPPUNIT_TEST (exitMessageTest);
    CPPUNIT_TEST (rgb2ColorIndexTest);
    CPPUNIT_TEST (isReverseNewFontcharTest);
    CPPUNIT_TEST (cp437Test);
    CPPUNIT_TEST (utf8Test);
    CPPUNIT_TEST (FullWidthHalfWidthTest);
    CPPUNIT_TEST (combiningCharacterTest);
    CPPUNIT_TEST (readCursorPosTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();

    // Data member
    finalcut::FVTerm fvterm{};  // Needed for FVTerm::getFOutput()
};


//----------------------------------------------------------------------
void FTermFunctionsTest::env2uintTest()
{
  setenv ("NUM1", "123", 1);
  CPPUNIT_ASSERT ( finalcut::env2uint("NUM1") == 123 );
  setenv ("NUM2", "4294967295", 1);
  CPPUNIT_ASSERT ( finalcut::env2uint("NUM2") == UINT_MAX );
  // Overflow
  setenv ("NUM3", "999999999999999999999999999999999", 1);
  CPPUNIT_ASSERT ( finalcut::env2uint("NUM3") == UINT_MAX );
  // not unsigned!
  setenv ("NUM4", "-123", 1);
  CPPUNIT_ASSERT ( finalcut::env2uint("NUM4") == 0 );
  // Invalid
  CPPUNIT_ASSERT ( finalcut::env2uint("NON_EXISTENT_VARIABLE") == 0 );
}

//----------------------------------------------------------------------
void FTermFunctionsTest::exitMessageTest()
{
  CPPUNIT_ASSERT ( finalcut::getExitMessage() == "" );
  CPPUNIT_ASSERT ( finalcut::getExitMessage().length() == 0 );

  finalcut::setExitMessage("No tty found");
  CPPUNIT_ASSERT ( finalcut::getExitMessage() == "No tty found" );
  CPPUNIT_ASSERT ( finalcut::getExitMessage().length() == 12 );

  finalcut::setExitMessage( finalcut::getExitMessage() + "!" );
  CPPUNIT_ASSERT ( finalcut::getExitMessage() == "No tty found!" );
  CPPUNIT_ASSERT ( finalcut::getExitMessage().length() == 13 );

  auto fstring = finalcut::FString("Encoding not found");
  finalcut::setExitMessage(fstring);
  CPPUNIT_ASSERT ( finalcut::getExitMessage() == "Encoding not found" );
  CPPUNIT_ASSERT ( finalcut::getExitMessage().length() == 18 );

  finalcut::setExitMessage("");  // Reset the exit message
  CPPUNIT_ASSERT ( finalcut::getExitMessage() == "" );
  CPPUNIT_ASSERT ( finalcut::getExitMessage().length() == 0 );
}

//----------------------------------------------------------------------
void FTermFunctionsTest::rgb2ColorIndexTest()
{
  // #000000
  CPPUNIT_ASSERT ( finalcut::rgb2ColorIndex (0x00, 0x00, 0x00) == 16 );
  // #000000
  CPPUNIT_ASSERT ( finalcut::rgb2ColorIndex (0xff, 0xff, 0xff) == 231 );
  // #76eeff
  CPPUNIT_ASSERT ( finalcut::rgb2ColorIndex (0x76, 0xee, 0xff) == 123 );
  // #d26fb7
  CPPUNIT_ASSERT ( finalcut::rgb2ColorIndex (0xd2, 0x6f, 0xb7) == 176 );
  // #896a53
  CPPUNIT_ASSERT ( finalcut::rgb2ColorIndex (0x89, 0x6a, 0x53) == 138 );
  // #ebf59c
  CPPUNIT_ASSERT ( finalcut::rgb2ColorIndex (0xeb, 0xf5, 0x9c) == 229 );
  // #c89f07
  CPPUNIT_ASSERT ( finalcut::rgb2ColorIndex (0xc8, 0x9f, 0x07) == 178 );
  // #61cb55
  CPPUNIT_ASSERT ( finalcut::rgb2ColorIndex (0x61, 0xcb, 0x55) == 114 );
  // #f16d4d
  CPPUNIT_ASSERT ( finalcut::rgb2ColorIndex (0xf1, 0x6d, 0x4d) == 210 );
  // #96c0af
  CPPUNIT_ASSERT ( finalcut::rgb2ColorIndex (0x96, 0xc0, 0xaf) == 151 );
  // #341bb6
  CPPUNIT_ASSERT ( finalcut::rgb2ColorIndex (0x34, 0x1b, 0xb6) == 62 );
  // #da1240
  CPPUNIT_ASSERT ( finalcut::rgb2ColorIndex (0xda, 0x12, 0x40) == 161 );
  // #6f2cce
  CPPUNIT_ASSERT ( finalcut::rgb2ColorIndex (0x6f, 0x2c, 0xce) == 98 );
  // #f19a1d
  CPPUNIT_ASSERT ( finalcut::rgb2ColorIndex (0xf1, 0x9a, 0x1d) == 215 );
  // #0e1068
  CPPUNIT_ASSERT ( finalcut::rgb2ColorIndex (0x0e, 0x10, 0x68) == 18 );
  // #cc9099
  CPPUNIT_ASSERT ( finalcut::rgb2ColorIndex (0xcc, 0x90, 0x99) == 181 );
  // #b9c3bb
  CPPUNIT_ASSERT ( finalcut::rgb2ColorIndex (0xb9, 0xc3, 0xbb) == 188 );
  // #094132
  CPPUNIT_ASSERT ( finalcut::rgb2ColorIndex (0x09, 0x41, 0x32) == 23 );
}

//----------------------------------------------------------------------
void FTermFunctionsTest::isReverseNewFontcharTest()
{
  CPPUNIT_ASSERT ( ! finalcut::isReverseNewFontchar(L'A') );
  CPPUNIT_ASSERT ( ! finalcut::isReverseNewFontchar(L'£') );
  CPPUNIT_ASSERT ( finalcut::isReverseNewFontchar(wchar_t(finalcut::UniChar::NF_rev_left_arrow2)) );
  CPPUNIT_ASSERT ( finalcut::isReverseNewFontchar(wchar_t(finalcut::UniChar::NF_rev_right_arrow2)) );
  CPPUNIT_ASSERT ( finalcut::isReverseNewFontchar(wchar_t(finalcut::UniChar::NF_rev_border_corner_upper_right)) );
  CPPUNIT_ASSERT ( finalcut::isReverseNewFontchar(wchar_t(finalcut::UniChar::NF_rev_border_line_right)) );
  CPPUNIT_ASSERT ( finalcut::isReverseNewFontchar(wchar_t(finalcut::UniChar::NF_rev_border_line_vertical_left)) );
  CPPUNIT_ASSERT ( finalcut::isReverseNewFontchar(wchar_t(finalcut::UniChar::NF_rev_border_corner_lower_right)) );
  CPPUNIT_ASSERT ( finalcut::isReverseNewFontchar(wchar_t(finalcut::UniChar::NF_rev_up_arrow2)) );
  CPPUNIT_ASSERT ( finalcut::isReverseNewFontchar(wchar_t(finalcut::UniChar::NF_rev_down_arrow2)) );
  CPPUNIT_ASSERT ( finalcut::isReverseNewFontchar(wchar_t(finalcut::UniChar::NF_rev_up_arrow1)) );
  CPPUNIT_ASSERT ( finalcut::isReverseNewFontchar(wchar_t(finalcut::UniChar::NF_rev_down_arrow1)) );
  CPPUNIT_ASSERT ( finalcut::isReverseNewFontchar(wchar_t(finalcut::UniChar::NF_rev_left_arrow1)) );
  CPPUNIT_ASSERT ( finalcut::isReverseNewFontchar(wchar_t(finalcut::UniChar::NF_rev_right_arrow1)) );
  CPPUNIT_ASSERT ( finalcut::isReverseNewFontchar(wchar_t(finalcut::UniChar::NF_rev_menu_button1)) );
  CPPUNIT_ASSERT ( finalcut::isReverseNewFontchar(wchar_t(finalcut::UniChar::NF_rev_menu_button2)) );
  CPPUNIT_ASSERT ( finalcut::isReverseNewFontchar(wchar_t(finalcut::UniChar::NF_rev_up_pointing_triangle1)) );
  CPPUNIT_ASSERT ( finalcut::isReverseNewFontchar(wchar_t(finalcut::UniChar::NF_rev_down_pointing_triangle1)) );
  CPPUNIT_ASSERT ( finalcut::isReverseNewFontchar(wchar_t(finalcut::UniChar::NF_rev_up_pointing_triangle2)) );
  CPPUNIT_ASSERT ( finalcut::isReverseNewFontchar(wchar_t(finalcut::UniChar::NF_rev_down_pointing_triangle2)) );
  CPPUNIT_ASSERT ( finalcut::isReverseNewFontchar(wchar_t(finalcut::UniChar::NF_rev_menu_button3)) );
  CPPUNIT_ASSERT ( finalcut::isReverseNewFontchar(wchar_t(finalcut::UniChar::NF_rev_border_line_right_and_left)) );
  CPPUNIT_ASSERT ( ! finalcut::isReverseNewFontchar(wchar_t(finalcut::UniChar::NF_shadow_box_checked)) );
  CPPUNIT_ASSERT ( ! finalcut::isReverseNewFontchar(wchar_t(finalcut::UniChar::NF_Bullet)) );
  CPPUNIT_ASSERT ( ! finalcut::isReverseNewFontchar(wchar_t(finalcut::UniChar::SquareRoot)) );
}

//----------------------------------------------------------------------
void FTermFunctionsTest::cp437Test()
{
  // CP437 -> Unicode
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x01) == L'☺' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x02) == L'☻' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x03) == L'♥' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x04) == L'♦' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x05) == L'♣' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x06) == L'♠' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x07) == L'•' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x08) == L'◘' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x09) == L'○' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x0a) == L'◙' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x0b) == L'♂' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x0c) == L'♀' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x0d) == L'♪' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x0e) == L'♫' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x0f) == L'☼' );

  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x10) == L'►' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x11) == L'◄' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x12) == L'↕' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x13) == L'‼' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x14) == L'¶' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x15) == L'§' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x16) == L'▬' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x17) == L'↨' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x18) == L'↑' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x19) == L'↓' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x1a) == L'→' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x1b) == L'←' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x1c) == L'∟' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x1d) == L'↔' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x1e) == L'▲' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x1f) == L'▼' );

  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x9b) == L'¢' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x9c) == L'£' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x9d) == L'¥' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0x9e) == L'₧' );

  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0xb0) == L'░' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0xb1) == L'▒' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0xb2) == L'▓' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0xdb) == L'█' );

  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0xdf) == L'▀' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0xdc) == L'▄' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0xdd) == L'▌' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0xde) == L'▐' );

  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0xc4) == L'─' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0xb3) == L'│' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0xc3) == L'├' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0xb4) == L'┤' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0xc5) == L'┼' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0xbf) == L'┐' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0xc0) == L'└' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0xda) == L'┌' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0xd9) == L'┘' );

  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0xf4) == L'⌠' );
  CPPUNIT_ASSERT ( finalcut::cp437_to_unicode(0xf5) == L'⌡' );

  // Unicode -> CP437
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'☺') == 0x01 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'☻') == 0x02 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'♥') == 0x03 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'♦') == 0x04 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'♣') == 0x05 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'♠') == 0x06 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'•') == 0x07 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'◘') == 0x08 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'○') == 0x09 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'◙') == 0x0a );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'♂') == 0x0b );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'♀') == 0x0c );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'♪') == 0x0d );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'♫') == 0x0e );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'☼') == 0x0f );

  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'►') == 0x10 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'◄') == 0x11 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'↕') == 0x12 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'‼') == 0x13 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'¶') == 0x14 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'§') == 0x15 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'▬') == 0x16 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'↨') == 0x17 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'↑') == 0x18 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'↓') == 0x19 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'→') == 0x1a );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'←') == 0x1b );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'∟') == 0x1c );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'↔') == 0x1d );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'▲') == 0x1e );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'▼') == 0x1f );

  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'¢') == 0x9b );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'£') == 0x9c );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'¥') == 0x9d );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'₧') == 0x9e );

  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'░') == 0xb0 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'▒') == 0xb1 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'▓') == 0xb2 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'█') == 0xdb );

  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'▀') == 0xdf );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'▄') == 0xdc );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'▌') == 0xdd );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'▐') == 0xde );

  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'─') == 0xc4 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'│') == 0xb3 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'├') == 0xc3 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'┤') == 0xb4 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'┼') == 0xc5 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'┐') == 0xbf );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'└') == 0xc0 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'┌') == 0xda );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'┘') == 0xd9 );

  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'⌠') == 0xf4 );
  CPPUNIT_ASSERT ( finalcut::unicode_to_cp437(L'⌡') == 0xf5 );
}

//----------------------------------------------------------------------
void FTermFunctionsTest::utf8Test()
{
  //-----------------
  // 1 byte character
  //-----------------
  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'\0')
                   == std::string({char(0x00)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'\b')
                   == std::string({char(0x08)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L' ')
                   == std::string({char(0x20)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'!')
                   == std::string({char(0x21)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'"')
                   == std::string({char(0x22)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'#')
                   == std::string({char(0x23)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'$')
                   == std::string({char(0x24)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'%')
                   == std::string({char(0x25)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'&')
                   == std::string({char(0x26)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'\'')
                   == std::string({char(0x27)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'(')
                   == std::string({char(0x28)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L')')
                   == std::string({char(0x29)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'*')
                   == std::string({char(0x2a)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'+')
                   == std::string({char(0x2b)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L',')
                   == std::string({char(0x2c)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'-')
                   == std::string({char(0x2d)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'.')
                   == std::string({char(0x2e)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'/')
                   == std::string({char(0x2f)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'0')
                   == std::string({char(0x30)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'1')
                   == std::string({char(0x31)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'2')
                   == std::string({char(0x32)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'3')
                   == std::string({char(0x33)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'4')
                   == std::string({char(0x34)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'5')
                   == std::string({char(0x35)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'6')
                   == std::string({char(0x36)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'7')
                   == std::string({char(0x37)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'8')
                   == std::string({char(0x38)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'9')
                   == std::string({char(0x39)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L':')
                   == std::string({char(0x3a)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L';')
                   == std::string({char(0x3b)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'<')
                   == std::string({char(0x3c)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'=')
                   == std::string({char(0x3d)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'>')
                   == std::string({char(0x3e)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'?')
                   == std::string({char(0x3f)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'@')
                   == std::string({char(0x40)}) );
  // Last 1 byte character
  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'\177')
                   == std::string({char(0x7f)}) );

  //-----------------
  // 2 byte character
  //-----------------
  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'\200')
                   == std::string({char(0xc2), char(0x80)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'ö')
                   == std::string({char(0xc3), char(0xb6)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'æ')
                   == std::string({char(0xc3), char(0xa6)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'ç')
                   == std::string({char(0xc3), char(0xa7)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'è')
                   == std::string({char(0xc3), char(0xa8)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'é')
                   == std::string({char(0xc3), char(0xa9)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'ê')
                   == std::string({char(0xc3), char(0xaa)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'ë')
                   == std::string({char(0xc3), char(0xab)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'ì')
                   == std::string({char(0xc3), char(0xac)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'í')
                   == std::string({char(0xc3), char(0xad)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'î')
                   == std::string({char(0xc3), char(0xae)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'ï')
                   == std::string({char(0xc3), char(0xaf)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'ð')
                   == std::string({char(0xc3), char(0xb0)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'ñ')
                   == std::string({char(0xc3), char(0xb1)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'ò')
                   == std::string({char(0xc3), char(0xb2)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'ó')
                   == std::string({char(0xc3), char(0xb3)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'ô')
                   == std::string({char(0xc3), char(0xb4)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'õ')
                   == std::string({char(0xc3), char(0xb5)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'ö')
                   == std::string({char(0xc3), char(0xb6)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(wchar_t(finalcut::UniChar::Pi))
                   == std::string({char(0xcf), char(0x80)}) );
  // Last 2 byte character
  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'\U000007ff')
                   == std::string({char(0xdf), char(0xbf)}) );

  //-----------------
  // 3 byte character
  //-----------------
  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'\U00000800')
                   == std::string({char(0xe0), char(0xa0), char(0x80)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'ࡔ')
                   == std::string({char(0xe0), char(0xa1), char(0x94)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'←')
                   == std::string({char(0xe2), char(0x86), char(0x90)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'↑')
                   == std::string({char(0xe2), char(0x86), char(0x91)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'→')
                   == std::string({char(0xe2), char(0x86), char(0x92)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'↓')
                   == std::string({char(0xe2), char(0x86), char(0x93)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'↔')
                   == std::string({char(0xe2), char(0x86), char(0x94)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'↕')
                   == std::string({char(0xe2), char(0x86), char(0x95)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'↖')
                   == std::string({char(0xe2), char(0x86), char(0x96)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'↗')
                   == std::string({char(0xe2), char(0x86), char(0x97)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'↘')
                   == std::string({char(0xe2), char(0x86), char(0x98)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'↙')
                   == std::string({char(0xe2), char(0x86), char(0x99)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'↚')
                   == std::string({char(0xe2), char(0x86), char(0x9a)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'↛')
                   == std::string({char(0xe2), char(0x86), char(0x9b)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'↜')
                   == std::string({char(0xe2), char(0x86), char(0x9c)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'↝')
                   == std::string({char(0xe2), char(0x86), char(0x9d)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'↞')
                   == std::string({char(0xe2), char(0x86), char(0x9e)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'↟')
                   == std::string({char(0xe2), char(0x86), char(0x9f)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'↠')
                   == std::string({char(0xe2), char(0x86), char(0xa0)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'☕')
                   == std::string({char(0xe2), char(0x98), char(0x95)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'㉟')
                   == std::string({char(0xe3), char(0x89), char(0x9f)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'䷀')
                   == std::string({char(0xe4), char(0xb7), char(0x80)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'頉')
                   == std::string({char(0xe9), char(0xa0), char(0x89)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'ꫦ')
                   == std::string({char(0xea), char(0xab), char(0xa6)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'')
                   == std::string({char(0xee), char(0x80), char(0x80)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(wchar_t(finalcut::UniChar::Euro))
                   == std::string({char(0xe2), char(0x82), char(0xac)}) );

  // Private Use Area
  auto newfont_char_e1b3 = finalcut::UniChar::NF_border_line_vertical;
  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(wchar_t(newfont_char_e1b3))
                   == std::string({char(0xee), char(0x86), char(0xb3)}) );

  auto newfont_char_e1f9 = finalcut::UniChar::NF_Bullet;
  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(wchar_t(newfont_char_e1f9))
                   == std::string({char(0xee), char(0x87), char(0xb9)}) );

  auto newfont_char_e1fc = finalcut::UniChar::NF_x_superior;
  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(wchar_t(newfont_char_e1fc))
                   == std::string({char(0xee), char(0x87), char(0xbc)}) );

  // Replacement character
  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'�')
                   == std::string({char(0xef), char(0xbf), char(0xbd)}) );

  // Last 3 byte character
  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'\U0000ffff')
                   == std::string({char(0xef), char(0xbf), char(0xbf)}) );

  //-----------------
  // 4 byte character
  //-----------------
  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'\U00010000')
                   == std::string({char(0xf0), char(0x90), char(0x80), char(0x80),}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'𐎈')
                   == std::string({char(0xf0), char(0x90), char(0x8e), char(0x88),}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'𖥕')
                   == std::string({char(0xf0), char(0x96), char(0xa5), char(0x95),}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'𛊹')
                   == std::string({char(0xf0), char(0x9b), char(0x8a), char(0xb9),}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'🂱')
                   == std::string({char(0xf0), char(0x9f), char(0x82), char(0xb1),}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'😀')
                   == std::string({char(0xf0), char(0x9f), char(0x98), char(0x80),}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'😁')
                   == std::string({char(0xf0), char(0x9f), char(0x98), char(0x81),}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'😂')
                   == std::string({char(0xf0), char(0x9f), char(0x98), char(0x82),}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'😃')
                   == std::string({char(0xf0), char(0x9f), char(0x98), char(0x83),}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'😄')
                   == std::string({char(0xf0), char(0x9f), char(0x98), char(0x84),}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'😅')
                   == std::string({char(0xf0), char(0x9f), char(0x98), char(0x85),}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'😆')
                   == std::string({char(0xf0), char(0x9f), char(0x98), char(0x86),}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'😇')
                   == std::string({char(0xf0), char(0x9f), char(0x98), char(0x87),}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'😈')
                   == std::string({char(0xf0), char(0x9f), char(0x98), char(0x88),}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'😉')
                   == std::string({char(0xf0), char(0x9f), char(0x98), char(0x89),}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'😊')
                   == std::string({char(0xf0), char(0x9f), char(0x98), char(0x8a),}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'😋')
                   == std::string({char(0xf0), char(0x9f), char(0x98), char(0x8b),}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'😌')
                   == std::string({char(0xf0), char(0x9f), char(0x98), char(0x8c),}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'😍')
                   == std::string({char(0xf0), char(0x9f), char(0x98), char(0x8d),}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'😎')
                   == std::string({char(0xf0), char(0x9f), char(0x98), char(0x8e),}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'😏')
                   == std::string({char(0xf0), char(0x9f), char(0x98), char(0x8f),}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'😐')
                   == std::string({char(0xf0), char(0x9f), char(0x98), char(0x90),}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'🦇')
                   == std::string({char(0xf0), char(0x9f), char(0xa6), char(0x87),}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'屮')
                   == std::string({char(0xf0), char(0xaf), char(0xa1), char(0xb8),}) );

  // Supplementary Private Use Area-A
  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(L'\U000F0000')
                   == std::string({char(0xf3), char(0xb0), char(0x80), char(0x80),}) );

  // Last valid 4 byte character
  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(wchar_t(0x1fffff))
                   == std::string({char(0xf7), char(0xbf), char(0xbf), char(0xbf),}) );

  // Invalid characters are mapped to the replacement character (U+FFFD)
  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(wchar_t(0x200000))
                   == std::string({char(0xef), char(0xbf), char(0xbd)}) );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(wchar_t(0x200000))
                   == finalcut::unicode_to_utf8(L'�') );

  CPPUNIT_ASSERT ( finalcut::unicode_to_utf8(wchar_t(INT_MAX))  // maximum 32-bit value
                   == finalcut::unicode_to_utf8(L'�') );
}

//----------------------------------------------------------------------
void FTermFunctionsTest::FullWidthHalfWidthTest()
{
  CPPUNIT_ASSERT ( finalcut::hasFullWidthSupports() );

  // Test the conversion from half-width to full-width characters
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L' ') == L"　" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'!') == L"！" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'"') == L"＂" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'#') == L"＃" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'$') == L"＄" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'%') == L"％" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'&') == L"＆" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'\'') == L"＇" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'(') == L"（" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L')') == L"）" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'*') == L"＊" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'+') == L"＋" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L',') == L"，" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'-') == L"－" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'.') == L"．" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'/') == L"／" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'0') == L"０" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'1') == L"１" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'2') == L"２" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'3') == L"３" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'4') == L"４" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'5') == L"５" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'6') == L"６" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'7') == L"７" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'8') == L"８" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'9') == L"９" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L':') == L"：" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L';') == L"；" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'<') == L"＜" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'=') == L"＝" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'>') == L"＞" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'?') == L"？" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'@') == L"＠" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'A') == L"Ａ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'B') == L"Ｂ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'C') == L"Ｃ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'D') == L"Ｄ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'E') == L"Ｅ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'F') == L"Ｆ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'G') == L"Ｇ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'H') == L"Ｈ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'I') == L"Ｉ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'J') == L"Ｊ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'K') == L"Ｋ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'L') == L"Ｌ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'M') == L"Ｍ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'N') == L"Ｎ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'O') == L"Ｏ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'P') == L"Ｐ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'Q') == L"Ｑ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'R') == L"Ｒ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'S') == L"Ｓ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'T') == L"Ｔ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'U') == L"Ｕ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'V') == L"Ｖ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'W') == L"Ｗ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'X') == L"Ｘ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'Y') == L"Ｙ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'Z') == L"Ｚ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'[') == L"［" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'\\') == L"＼" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L']') == L"］" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'^') == L"＾" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'_') == L"＿" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'`') == L"｀" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'a') == L"ａ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'b') == L"ｂ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'c') == L"ｃ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'd') == L"ｄ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'e') == L"ｅ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'f') == L"ｆ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'g') == L"ｇ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'h') == L"ｈ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'i') == L"ｉ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'j') == L"ｊ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'k') == L"ｋ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'l') == L"ｌ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'm') == L"ｍ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'n') == L"ｎ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'o') == L"ｏ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'p') == L"ｐ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'q') == L"ｑ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'r') == L"ｒ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L's') == L"ｓ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L't') == L"ｔ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'u') == L"ｕ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'v') == L"ｖ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'w') == L"ｗ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'x') == L"ｘ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'y') == L"ｙ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'z') == L"ｚ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'{') == L"｛" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'|') == L"｜" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'}') == L"｝" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'~') == L"～" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'￩') == L"←" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'￪') == L"↑" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'￫') == L"→" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'￬') == L"↓" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'¢') == L"￠" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'£') == L"￡" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'ｱ') == L"ア" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'ｲ') == L"イ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'ｳ') == L"ウ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'ｴ') == L"エ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'ﾡ') == L"ㄱ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'ﾢ') == L"ㄲ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'ﾣ') == L"ㄳ" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'ﾤ') == L"ㄴ" );
  // Only half-width
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'Ā') == L"Ā" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'ā') == L"ā" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'Ă') == L"Ă" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'ă') == L"ă" );
  CPPUNIT_ASSERT ( finalcut::getFullWidth(L'Ą') == L"Ą" );

  // Test the conversion from full-width to half-width characters
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"　") == L" " );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"！") == L"!" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"＂") == L"\"" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"＃") == L"#" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"＄") == L"$" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"％") == L"%" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"＆") == L"&" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"＇") == L"'" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"（") == L"(" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"）") == L")" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"＊") == L"*" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"＋") == L"+" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"，") == L"," );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"－") == L"-" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"．") == L"." );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"／") == L"/" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"０") == L"0" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"１") == L"1" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"２") == L"2" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"３") == L"3" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"４") == L"4" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"５") == L"5" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"６") == L"6" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"７") == L"7" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"８") == L"8" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"９") == L"9" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"：") == L":" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"；") == L";" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"＜") == L"<" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"＝") == L"=" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"＞") == L">" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"？") == L"?" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"＠") == L"@" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"Ａ") == L"A" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"Ｂ") == L"B" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"Ｃ") == L"C" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"Ｄ") == L"D" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"Ｅ") == L"E" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"Ｆ") == L"F" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"Ｇ") == L"G" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"Ｈ") == L"H" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"Ｉ") == L"I" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"Ｊ") == L"J" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"Ｋ") == L"K" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"Ｌ") == L"L" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"Ｍ") == L"M" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"Ｎ") == L"N" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"Ｏ") == L"O" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"Ｐ") == L"P" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"Ｑ") == L"Q" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"Ｒ") == L"R" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"Ｓ") == L"S" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"Ｔ") == L"T" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"Ｕ") == L"U" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"Ｖ") == L"V" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"Ｗ") == L"W" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"Ｘ") == L"X" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"Ｙ") == L"Y" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"Ｚ") == L"Z" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"［") == L"[" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"＼") == L"\\" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"］") == L"]" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"＾") == L"^" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"＿") == L"_" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"｀") == L"`" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ａ") == L"a" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ｂ") == L"b" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ｃ") == L"c" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ｄ") == L"d" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ｅ") == L"e" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ｆ") == L"f" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ｇ") == L"g" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ｈ") == L"h" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ｉ") == L"i" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ｊ") == L"j" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ｋ") == L"k" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ｌ") == L"l" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ｍ") == L"m" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ｎ") == L"n" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ｏ") == L"o" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ｐ") == L"p" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ｑ") == L"q" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ｒ") == L"r" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ｓ") == L"s" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ｔ") == L"t" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ｕ") == L"u" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ｖ") == L"v" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ｗ") == L"w" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ｘ") == L"x" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ｙ") == L"y" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ｚ") == L"z" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"｛") == L"{" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"｜") == L"|" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"｝") == L"}" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"～") == L"~" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"←" ) == L"￩");
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"↑" ) == L"￪");
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"→" ) == L"￫");
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"↓" ) == L"￬");
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"￠") == L"¢" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"￡") == L"£" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ア") == L"ｱ" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"イ") == L"ｲ" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ウ") == L"ｳ" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"エ") == L"ｴ" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ㄱ") == L"ﾡ" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ㄲ") == L"ﾢ" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ㄳ") == L"ﾣ" );
  CPPUNIT_ASSERT ( finalcut::getHalfWidth(L"ㄴ") == L"ﾤ" );

  // Column width (wchar_t)
  auto& fterm_data = finalcut::FTermData::getInstance();
  fterm_data.setTermEncoding (finalcut::Encoding::UTF8);
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"\t") == 0 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"\r") == 0 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"\n") == 0 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"\v") == 0 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L" ") == 1 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"0") == 1 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"1") == 1 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"2") == 1 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"3") == 1 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"０") == 2 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"１") == 2 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"２") == 2 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"３") == 2 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"\U00000300") == 0 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"\U00000348") == 0 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"\U0000094d") == 0 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"\U00000e37") == 0 );

  // Column width (wchar_t) in latin-1
  std::setlocale (LC_CTYPE, "C");
  fterm_data.setTermEncoding (finalcut::Encoding::VT100);
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L'─') == 1 );  // wcwidth(L'─') == -1 (for LC_CTYPE = C)
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L'│') == 1 );  // wcwidth(L'│') == -1 (for LC_CTYPE = C)
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L'├') == 1 );  // wcwidth(L'├') == -1 (for LC_CTYPE = C)
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L'┤') == 1 );  // wcwidth(L'┤') == -1 (for LC_CTYPE = C)
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L'┼') == 1 );  // wcwidth(L'┼') == -1 (for LC_CTYPE = C)
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L'┐') == 1 );  // wcwidth(L'┐') == -1 (for LC_CTYPE = C)
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L'└') == 1 );  // wcwidth(L'└') == -1 (for LC_CTYPE = C)
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L'┌') == 1 );  // wcwidth(L'┌') == -1 (for LC_CTYPE = C)
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L'┘') == 1 );  // wcwidth(L'┘') == -1 (for LC_CTYPE = C)

  auto ret = std::setlocale (LC_CTYPE, "en_US.UTF-8");

  if ( ! ret )
    ret = std::setlocale (LC_CTYPE, "C.UTF-8");

  if ( ! ret )
    return;

  fterm_data.setTermEncoding (finalcut::Encoding::UTF8);

  // Column width (FString)
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"\v\t 100") == 4 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"0123456789") == 10 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"０１２３４５６７８９") == 20 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"abc") == 3 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"ａbｃ") == 5 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"你好") == 4 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"你好 one ＣＵＴ more") == 20 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"1234567 one ＣＵＴ more") == 23 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"o\U0000031b\U00000323=\U00001ee3") == 3 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"STARGΛ̊TE") == 8 );

  // Column width (FString) with end position
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"\v\t 100", 0) == 0 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"\v\t 100", 1) == 0 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"\v\t 100", 2) == 0 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"\v\t 100", 3) == 1 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"\v\t 100", 4) == 2 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"\v\t 100", 5) == 3 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"\v\t 100", 6) == 4 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"\v\t 100", 7) == 4 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"0123", 0) == 0 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"0123", 1) == 1 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"0123", 2) == 2 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"0123", 3) == 3 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"0123", 4) == 4 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"０１２３", 0) == 0 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"０１２３", 1) == 2 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"０１２３", 2) == 4 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"０１２３", 3) == 6 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"０１２３", 4) == 8 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"你好 one ＣＵＴ more", 0) == 0 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"你好 one ＣＵＴ more", 1) == 2 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"你好 one ＣＵＴ more", 2) == 4 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"你好 one ＣＵＴ more", 3) == 5 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"你好 one ＣＵＴ more", 4) == 6 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"你好 one ＣＵＴ more", 5) == 7 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"你好 one ＣＵＴ more", 6) == 8 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"你好 one ＣＵＴ more", 7) == 9 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"你好 one ＣＵＴ more", 8) == 11 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"你好 one ＣＵＴ more", 9) == 13 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"你好 one ＣＵＴ more", 10) == 15 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"你好 one ＣＵＴ more", 11) == 16 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"你好 one ＣＵＴ more", 12) == 17 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"你好 one ＣＵＴ more", 13) == 18 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"你好 one ＣＵＴ more", 14) == 19 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"你好 one ＣＵＴ more", 15) == 20 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"1234567 one ＣＵＴ more", 0) == 0 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"1234567 one ＣＵＴ more", 1) == 1 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"1234567 one ＣＵＴ more", 2) == 2 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"1234567 one ＣＵＴ more", 3) == 3 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"1234567 one ＣＵＴ more", 4) == 4 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"1234567 one ＣＵＴ more", 5) == 5 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"1234567 one ＣＵＴ more", 6) == 6 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"1234567 one ＣＵＴ more", 7) == 7 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"1234567 one ＣＵＴ more", 8) == 8 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"1234567 one ＣＵＴ more", 9) == 9 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"1234567 one ＣＵＴ more", 10) == 10 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"1234567 one ＣＵＴ more", 11) == 11 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"1234567 one ＣＵＴ more", 12) == 12 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"1234567 one ＣＵＴ more", 13) == 14 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"1234567 one ＣＵＴ more", 14) == 16 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"1234567 one ＣＵＴ more", 15) == 18 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"1234567 one ＣＵＴ more", 16) == 19 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"1234567 one ＣＵＴ more", 17) == 20 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"1234567 one ＣＵＴ more", 18) == 21 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"1234567 one ＣＵＴ more", 19) == 22 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"1234567 one ＣＵＴ more", 20) == 23 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"o\U0000031b\U00000323=\U00001ee3", 0) == 0 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"o\U0000031b\U00000323=\U00001ee3", 1) == 1 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"o\U0000031b\U00000323=\U00001ee3", 2) == 1 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"o\U0000031b\U00000323=\U00001ee3", 3) == 1 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"o\U0000031b\U00000323=\U00001ee3", 4) == 2 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"o\U0000031b\U00000323=\U00001ee3", 5) == 3 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"STARGΛ̊TE", 0) == 0 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"STARGΛ̊TE", 1) == 1 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"STARGΛ̊TE", 2) == 2 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"STARGΛ̊TE", 3) == 3 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"STARGΛ̊TE", 4) == 4 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"STARGΛ̊TE", 5) == 5 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"STARGΛ̊TE", 6) == 6 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"STARGΛ̊TE", 7) == 6 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"STARGΛ̊TE", 8) == 7 );
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(L"STARGΛ̊TE", 9) == 8 );

  // String length from the first to the specified end column
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"\v\t 100", 0) == 0 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"\v\t 100", 1) == 3 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"\v\t 100", 2) == 4 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"\v\t 100", 3) == 5 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"\v\t 100", 4) == 6 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"\v\t 100", 5) == 6 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"0123", 0) == 0 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"0123", 1) == 1 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"0123", 2) == 2 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"0123", 3) == 3 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"0123", 4) == 4 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"０１２３", 0) == 0 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"０１２３", 1) == 1 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"０１２３", 2) == 1 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"０１２３", 3) == 2 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"０１２３", 4) == 2 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"０１２３", 5) == 3 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"０１２３", 6) == 3 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"０１２３", 7) == 4 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"０１２３", 8) == 4 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"你好 one ＣＵＴ more", 0) == 0 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"你好 one ＣＵＴ more", 1) == 1 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"你好 one ＣＵＴ more", 2) == 1 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"你好 one ＣＵＴ more", 3) == 2 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"你好 one ＣＵＴ more", 4) == 2 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"你好 one ＣＵＴ more", 5) == 3 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"你好 one ＣＵＴ more", 6) == 4 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"你好 one ＣＵＴ more", 7) == 5 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"你好 one ＣＵＴ more", 8) == 6 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"你好 one ＣＵＴ more", 9) == 7 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"你好 one ＣＵＴ more", 10) == 8 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"你好 one ＣＵＴ more", 11) == 8 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"你好 one ＣＵＴ more", 12) == 9 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"你好 one ＣＵＴ more", 13) == 9 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"你好 one ＣＵＴ more", 14) == 10 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"你好 one ＣＵＴ more", 15) == 10 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"你好 one ＣＵＴ more", 16) == 11 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"你好 one ＣＵＴ more", 17) == 12 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"你好 one ＣＵＴ more", 18) == 13 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"你好 one ＣＵＴ more", 19) == 14 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"你好 one ＣＵＴ more", 20) == 15 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"1234567 one ＣＵＴ more", 0) == 0 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"1234567 one ＣＵＴ more", 1) == 1 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"1234567 one ＣＵＴ more", 2) == 2 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"1234567 one ＣＵＴ more", 3) == 3 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"1234567 one ＣＵＴ more", 4) == 4 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"1234567 one ＣＵＴ more", 5) == 5 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"1234567 one ＣＵＴ more", 6) == 6 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"1234567 one ＣＵＴ more", 7) == 7 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"1234567 one ＣＵＴ more", 8) == 8 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"1234567 one ＣＵＴ more", 9) == 9 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"1234567 one ＣＵＴ more", 10) == 10 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"1234567 one ＣＵＴ more", 11) == 11 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"1234567 one ＣＵＴ more", 12) == 12 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"1234567 one ＣＵＴ more", 13) == 13 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"1234567 one ＣＵＴ more", 14) == 13 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"1234567 one ＣＵＴ more", 15) == 14 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"1234567 one ＣＵＴ more", 16) == 14 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"1234567 one ＣＵＴ more", 17) == 15 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"1234567 one ＣＵＴ more", 18) == 15 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"1234567 one ＣＵＴ more", 19) == 16 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"1234567 one ＣＵＴ more", 20) == 17 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"1234567 one ＣＵＴ more", 21) == 18 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"1234567 one ＣＵＴ more", 22) == 19 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"1234567 one ＣＵＴ more", 23) == 20 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"o\U0000031b\U00000323=\U00001ee3", 0) == 0 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"o\U0000031b\U00000323=\U00001ee3", 1) == 1 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"o\U0000031b\U00000323=\U00001ee3", 2) == 4 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"o\U0000031b\U00000323=\U00001ee3", 3) == 5 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"STARGΛ̊TE", 0) == 0 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"STARGΛ̊TE", 1) == 1 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"STARGΛ̊TE", 2) == 2 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"STARGΛ̊TE", 3) == 3 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"STARGΛ̊TE", 4) == 4 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"STARGΛ̊TE", 5) == 5 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"STARGΛ̊TE", 6) == 6 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"STARGΛ̊TE", 7) == 8 );
  CPPUNIT_ASSERT ( finalcut::getLengthFromColumnWidth(L"STARGΛ̊TE", 8) == 9 );

  // Column width (FChar)
  finalcut::FChar fchar{};
  std::wstring s = L"1";
  std::copy(std::begin(s), std::end(s), std::begin(fchar.ch));
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 0 );
  auto column_width = finalcut::getColumnWidth(fchar.ch[0]);
  finalcut::addColumnWidth(fchar, column_width);
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 1 );
  fchar.attr.bit.char_width = 0x00 & 0x03;
  s = L"\t";
  std::copy(std::begin(s), std::end(s), std::begin(fchar.ch));
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 0 );
  column_width = finalcut::getColumnWidth(fchar.ch[0]);
  finalcut::addColumnWidth(fchar, column_width);
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 0 );
  fchar.attr.bit.char_width = 0x00 & 0x03;
  s = L"\r";
  std::copy(std::begin(s), std::end(s), std::begin(fchar.ch));
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 0 );
  column_width = finalcut::getColumnWidth(fchar.ch[0]);
  finalcut::addColumnWidth(fchar, column_width);
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 0 );
  fchar.attr.bit.char_width = 0x00 & 0x03;
  s = L"\n";
  std::copy(std::begin(s), std::end(s), std::begin(fchar.ch));
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 0 );
  column_width = finalcut::getColumnWidth(fchar.ch[0]);
  finalcut::addColumnWidth(fchar, column_width);
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 0 );
  fchar.attr.bit.char_width = 0x00 & 0x03;
  s = L"\v";
  std::copy(std::begin(s), std::end(s), std::begin(fchar.ch));
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 0 );
  column_width = finalcut::getColumnWidth(fchar.ch[0]);
  finalcut::addColumnWidth(fchar, column_width);
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 0 );
  fchar.attr.bit.char_width = 0x00 & 0x03;
  s = L" ";
  std::copy(std::begin(s), std::end(s), std::begin(fchar.ch));
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 0 );
  column_width = finalcut::getColumnWidth(fchar.ch[0]);
  finalcut::addColumnWidth(fchar, column_width);
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 1 );
  fchar.attr.bit.char_width = 0x00 & 0x03;
  s = L"0";
  std::copy(std::begin(s), std::end(s), std::begin(fchar.ch));
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 0 );
  column_width = finalcut::getColumnWidth(fchar.ch[0]);
  finalcut::addColumnWidth(fchar, column_width);
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 1 );
  fchar.attr.bit.char_width = 0x00 & 0x03;
  s = L"1";
  std::copy(std::begin(s), std::end(s), std::begin(fchar.ch));
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 0 );
  column_width = finalcut::getColumnWidth(fchar.ch[0]);
  finalcut::addColumnWidth(fchar, column_width);
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 1 );
  fchar.attr.bit.char_width = 0x00 & 0x03;
  s = L"2";
  std::copy(std::begin(s), std::end(s), std::begin(fchar.ch));
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 0 );
  column_width = finalcut::getColumnWidth(fchar.ch[0]);
  finalcut::addColumnWidth(fchar, column_width);
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 1 );
  fchar.attr.bit.char_width = 0x00 & 0x03;
  s = L"3";
  std::copy(std::begin(s), std::end(s), std::begin(fchar.ch));
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 0 );
  column_width = finalcut::getColumnWidth(fchar.ch[0]);
  finalcut::addColumnWidth(fchar, column_width);
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 1 );
  fchar.attr.bit.char_width = 0x00 & 0x03;
  s = L"０";
  std::copy(std::begin(s), std::end(s), std::begin(fchar.ch));
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 0 );
  column_width = finalcut::getColumnWidth(fchar.ch[0]);
  finalcut::addColumnWidth(fchar, column_width);
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 2 );
  fchar.attr.bit.char_width = 0x00 & 0x03;
  s = L"１";
  std::copy(std::begin(s), std::end(s), std::begin(fchar.ch));
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 0 );
  column_width = finalcut::getColumnWidth(fchar.ch[0]);
  finalcut::addColumnWidth(fchar, column_width);
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 2 );
  fchar.attr.bit.char_width = 0x00 & 0x03;
  s = L"２";
  std::copy(std::begin(s), std::end(s), std::begin(fchar.ch));
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 0 );
  column_width = finalcut::getColumnWidth(fchar.ch[0]);
  finalcut::addColumnWidth(fchar, column_width);
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 2 );
  fchar.attr.bit.char_width = 0x00 & 0x03;
  s = L"３";
  std::copy(std::begin(s), std::end(s), std::begin(fchar.ch));
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 0 );
  column_width = finalcut::getColumnWidth(fchar.ch[0]);
  finalcut::addColumnWidth(fchar, column_width);
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 2 );
  fchar.attr.bit.char_width = 0x00 & 0x03;
  s = L"\U00000300";
  std::copy(std::begin(s), std::end(s), std::begin(fchar.ch));
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 0 );
  column_width = finalcut::getColumnWidth(fchar.ch[0]);
  finalcut::addColumnWidth(fchar, column_width);
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 0 );
  fchar.attr.bit.char_width = 0x00 & 0x03;
  s = L"\U00000348";
  std::copy(std::begin(s), std::end(s), std::begin(fchar.ch));
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 0 );
  column_width = finalcut::getColumnWidth(fchar.ch[0]);
  finalcut::addColumnWidth(fchar, column_width);
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 0 );
  fchar.attr.bit.char_width = 0x00 & 0x03;
  s = L"\U0000094d";
  std::copy(std::begin(s), std::end(s), std::begin(fchar.ch));
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 0 );
  column_width = finalcut::getColumnWidth(fchar.ch[0]);
  finalcut::addColumnWidth(fchar, column_width);
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 0 );
  fchar.attr.bit.char_width = 0x00 & 0x03;
  s = L"\U00000e37";
  std::copy(std::begin(s), std::end(s), std::begin(fchar.ch));
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 0 );
  column_width = finalcut::getColumnWidth(fchar.ch[0]);
  finalcut::addColumnWidth(fchar, column_width);
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(fchar) == 0 );
  fchar.attr.bit.char_width = 0x00 & 0x03;

  // Column width (FVTermBuffer)
  finalcut::FVTermBuffer vterm_buf{};
  vterm_buf << L"\v\t 100";
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(vterm_buf) == 4 );
  vterm_buf.clear();
  vterm_buf << L"0123456789";
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(vterm_buf) == 10 );
  vterm_buf.clear();
  vterm_buf << L"０１２３４５６７８９";
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(vterm_buf) == 20 );  // UTF-8
  vterm_buf.clear();
  fterm_data.setTermEncoding (finalcut::Encoding::PC);
  vterm_buf << L"０１２３４５６７８９";
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(vterm_buf) == 10 );  // CP-437
  vterm_buf.clear();
  fterm_data.setTermEncoding (finalcut::Encoding::UTF8);
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(vterm_buf) == 0 );  // after clear
  vterm_buf << L"abc";
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(vterm_buf) == 3 );
  vterm_buf.clear();
  vterm_buf << L"ａbｃ";
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(vterm_buf) == 5 );
  vterm_buf.clear();
  vterm_buf << L"你好";
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(vterm_buf) == 4 );
  vterm_buf.clear();
  vterm_buf << L"你好 one ＣＵＴ more";
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(vterm_buf) == 20 );
  vterm_buf.clear();
  vterm_buf << L"1234567 one ＣＵＴ more";
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(vterm_buf) == 23 );
  vterm_buf.clear();
  vterm_buf << L"o\U0000031b\U00000323=\U00001ee3";
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(vterm_buf) == 3 );
  vterm_buf.clear();
  vterm_buf << L"STARGΛ̊TE";
  CPPUNIT_ASSERT ( finalcut::getColumnWidth(vterm_buf) == 8 );

  // Extracts a substring that starts at a specified column position
  // and has a specified number of columns
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 0, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 1, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 2, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 3, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 4, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 5, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 0, 1) == L" " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 1, 1) == L" " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 2, 1) == L"1" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 3, 1) == L"0" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 4, 1) == L"0" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 5, 1) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 0, 2) == L" 1" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 1, 2) == L" 1" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 2, 2) == L"10" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 3, 2) == L"00" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 4, 2) == L"0" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 5, 2) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 0, 3) == L" 10" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 1, 3) == L" 10" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 2, 3) == L"100" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 3, 3) == L"00" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 4, 3) == L"0" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 5, 3) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 0, 4) == L" 100" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 1, 4) == L" 100" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 2, 4) == L"100" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 3, 4) == L"00" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 4, 4) == L"0" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"\v\t 100", 5, 4) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"0123", 0, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"0123", 1, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"0123", 2, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"0123", 3, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"0123", 4, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"0123", 5, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"0123", 0, 1) == L"0" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"0123", 1, 1) == L"0" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"0123", 2, 1) == L"1" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"0123", 3, 1) == L"2" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"0123", 4, 1) == L"3" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"0123", 5, 1) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"0123", 0, 2) == L"01" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"0123", 1, 2) == L"01" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"0123", 2, 2) == L"12" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"0123", 3, 2) == L"23" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"0123", 4, 2) == L"3" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"0123", 5, 2) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"0123", 0, 3) == L"012" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"0123", 1, 3) == L"012" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"0123", 2, 3) == L"123" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"0123", 3, 3) == L"23" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"0123", 4, 3) == L"3" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"0123", 5, 3) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 0, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 1, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 2, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 3, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 4, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 5, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 6, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 7, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 8, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 9, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 0, 1) == L"›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 1, 1) == L"›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 2, 1) == L"‹" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 3, 1) == L"›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 4, 1) == L"‹" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 5, 1) == L"›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 6, 1) == L"‹" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 7, 1) == L"›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 8, 1) == L"‹" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 9, 1) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 0, 2) == L"０" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 1, 2) == L"０" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 2, 2) == L"‹›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 3, 2) == L"１" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 4, 2) == L"‹›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 5, 2) == L"２" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 6, 2) == L"‹›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 7, 2) == L"３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 8, 2) == L"‹" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 9, 2) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 0, 3) == L"０›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 1, 3) == L"０›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 2, 3) == L"‹１" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 3, 3) == L"１›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 4, 3) == L"‹２" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 5, 3) == L"２›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 6, 3) == L"‹３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 7, 3) == L"３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 8, 3) == L"‹" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 9, 3) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 0, 4) == L"０１" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 1, 4) == L"０１" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 2, 4) == L"‹１›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 3, 4) == L"１２" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 4, 4) == L"‹２›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 5, 4) == L"２３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 6, 4) == L"‹３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 7, 4) == L"３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 8, 4) == L"‹" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 9, 4) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 0, 5) == L"０１›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 1, 5) == L"０１›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 2, 5) == L"‹１２" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 3, 5) == L"１２›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 4, 5) == L"‹２３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 5, 5) == L"２３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 6, 5) == L"‹３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 7, 5) == L"３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 8, 5) == L"‹" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 9, 5) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 0, 6) == L"０１２" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 1, 6) == L"０１２" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 2, 6) == L"‹１２›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 3, 6) == L"１２３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 4, 6) == L"‹２３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 5, 6) == L"２３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 6, 6) == L"‹３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 7, 6) == L"３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 8, 6) == L"‹" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 9, 6) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 0, 7) == L"０１２›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 1, 7) == L"０１２›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 2, 7) == L"‹１２３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 3, 7) == L"１２３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 4, 7) == L"‹２３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 5, 7) == L"２３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 6, 7) == L"‹３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 7, 7) == L"３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 8, 7) == L"‹" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 9, 7) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 0, 8) == L"０１２３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 1, 8) == L"０１２３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 2, 8) == L"‹１２３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 3, 8) == L"１２３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 4, 8) == L"‹２３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 5, 8) == L"２３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 6, 8) == L"‹３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 7, 8) == L"３" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 8, 8) == L"‹" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"０１２３", 9, 8) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 0, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 1, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 2, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 3, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 4, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 5, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 6, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 7, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 8, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 9, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 10, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 11, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 12, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 13, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 14, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 15, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 16, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 17, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 18, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 19, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 20, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 21, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 0, 1) == L"›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 1, 1) == L"›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 2, 1) == L"‹" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 3, 1) == L"›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 4, 1) == L"‹" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 5, 1) == L" " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 6, 1) == L"o" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 7, 1) == L"n" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 8, 1) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 9, 1) == L" " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 10, 1) == L"›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 11, 1) == L"‹" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 12, 1) == L"›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 13, 1) == L"‹" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 14, 1) == L"›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 15, 1) == L"‹" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 16, 1) == L" " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 17, 1) == L"m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 18, 1) == L"o" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 19, 1) == L"r" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 20, 1) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 21, 1) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 0, 2) == L"你" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 1, 2) == L"你" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 2, 2) == L"‹›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 3, 2) == L"好" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 4, 2) == L"‹ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 5, 2) == L" o" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 6, 2) == L"on" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 7, 2) == L"ne" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 8, 2) == L"e " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 9, 2) == L" ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 10, 2) == L"Ｃ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 11, 2) == L"‹›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 12, 2) == L"Ｕ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 13, 2) == L"‹›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 14, 2) == L"Ｔ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 15, 2) == L"‹ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 16, 2) == L" m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 17, 2) == L"mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 18, 2) == L"or" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 19, 2) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 20, 2) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 21, 2) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 0, 3) == L"你›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 1, 3) == L"你›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 2, 3) == L"‹好" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 3, 3) == L"好 " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 4, 3) == L"‹ o" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 5, 3) == L" on" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 6, 3) == L"one" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 7, 3) == L"ne " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 8, 3) == L"e ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 9, 3) == L" Ｃ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 10, 3) == L"Ｃ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 11, 3) == L"‹Ｕ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 12, 3) == L"Ｕ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 13, 3) == L"‹Ｔ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 14, 3) == L"Ｔ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 15, 3) == L"‹ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 16, 3) == L" mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 17, 3) == L"mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 18, 3) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 19, 3) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 20, 3) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 21, 3) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 0, 4) == L"你好" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 1, 4) == L"你好" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 2, 4) == L"‹好 " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 3, 4) == L"好 o" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 4, 4) == L"‹ on" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 5, 4) == L" one" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 6, 4) == L"one " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 7, 4) == L"ne ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 8, 4) == L"e Ｃ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 9, 4) == L" Ｃ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 10, 4) == L"ＣＵ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 11, 4) == L"‹Ｕ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 12, 4) == L"ＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 13, 4) == L"‹Ｔ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 14, 4) == L"Ｔ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 15, 4) == L"‹ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 16, 4) == L" mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 17, 4) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 18, 4) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 19, 4) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 20, 4) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 21, 4) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 0, 5) == L"你好 " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 1, 5) == L"你好 " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 2, 5) == L"‹好 o" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 3, 5) == L"好 on" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 4, 5) == L"‹ one" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 5, 5) == L" one " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 6, 5) == L"one ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 7, 5) == L"ne Ｃ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 8, 5) == L"e Ｃ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 9, 5) == L" ＣＵ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 10, 5) == L"ＣＵ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 11, 5) == L"‹ＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 12, 5) == L"ＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 13, 5) == L"‹Ｔ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 14, 5) == L"Ｔ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 15, 5) == L"‹ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 16, 5) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 17, 5) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 18, 5) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 19, 5) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 20, 5) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 21, 5) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 0, 6) == L"你好 o" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 1, 6) == L"你好 o" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 2, 6) == L"‹好 on" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 3, 6) == L"好 one" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 4, 6) == L"‹ one " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 5, 6) == L" one ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 6, 6) == L"one Ｃ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 7, 6) == L"ne Ｃ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 8, 6) == L"e ＣＵ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 9, 6) == L" ＣＵ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 10, 6) == L"ＣＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 11, 6) == L"‹ＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 12, 6) == L"ＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 13, 6) == L"‹Ｔ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 14, 6) == L"Ｔ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 15, 6) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 16, 6) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 17, 6) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 18, 6) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 19, 6) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 20, 6) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 21, 6) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 0, 7) == L"你好 on" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 1, 7) == L"你好 on" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 2, 7) == L"‹好 one" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 3, 7) == L"好 one " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 4, 7) == L"‹ one ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 5, 7) == L" one Ｃ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 6, 7) == L"one Ｃ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 7, 7) == L"ne ＣＵ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 8, 7) == L"e ＣＵ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 9, 7) == L" ＣＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 10, 7) == L"ＣＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 11, 7) == L"‹ＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 12, 7) == L"ＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 13, 7) == L"‹Ｔ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 14, 7) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 15, 7) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 16, 7) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 17, 7) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 18, 7) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 19, 7) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 20, 7) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 21, 7) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 0, 8) == L"你好 one" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 1, 8) == L"你好 one" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 2, 8) == L"‹好 one " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 3, 8) == L"好 one ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 4, 8) == L"‹ one Ｃ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 5, 8) == L" one Ｃ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 6, 8) == L"one ＣＵ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 7, 8) == L"ne ＣＵ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 8, 8) == L"e ＣＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 9, 8) == L" ＣＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 10, 8) == L"ＣＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 11, 8) == L"‹ＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 12, 8) == L"ＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 13, 8) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 14, 8) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 15, 8) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 16, 8) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 17, 8) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 18, 8) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 19, 8) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 20, 8) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 21, 8) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 0, 9) == L"你好 one " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 1, 9) == L"你好 one " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 2, 9) == L"‹好 one ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 3, 9) == L"好 one Ｃ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 4, 9) == L"‹ one Ｃ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 5, 9) == L" one ＣＵ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 6, 9) == L"one ＣＵ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 7, 9) == L"ne ＣＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 8, 9) == L"e ＣＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 9, 9) == L" ＣＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 10, 9) == L"ＣＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 11, 9) == L"‹ＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 12, 9) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 13, 9) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 14, 9) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 15, 9) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 16, 9) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 17, 9) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 18, 9) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 19, 9) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 20, 9) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 21, 9) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 0, 10) == L"你好 one ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 1, 10) == L"你好 one ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 2, 10) == L"‹好 one Ｃ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 3, 10) == L"好 one Ｃ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 4, 10) == L"‹ one ＣＵ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 5, 10) == L" one ＣＵ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 6, 10) == L"one ＣＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 7, 10) == L"ne ＣＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 8, 10) == L"e ＣＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 9, 10) == L" ＣＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 10, 10) == L"ＣＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 11, 10) == L"‹ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 12, 10) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 13, 10) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 14, 10) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 15, 10) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 16, 10) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 17, 10) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 18, 10) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 19, 10) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 20, 10) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 21, 10) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 0, 11) == L"你好 one Ｃ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 1, 11) == L"你好 one Ｃ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 2, 11) == L"‹好 one Ｃ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 3, 11) == L"好 one ＣＵ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 4, 11) == L"‹ one ＣＵ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 5, 11) == L" one ＣＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 6, 11) == L"one ＣＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 7, 11) == L"ne ＣＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 8, 11) == L"e ＣＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 9, 11) == L" ＣＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 10, 11) == L"ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 11, 11) == L"‹ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 12, 11) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 13, 11) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 14, 11) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 15, 11) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 16, 11) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 17, 11) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 18, 11) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 19, 11) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 20, 11) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 21, 11) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 0, 12) == L"你好 one Ｃ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 1, 12) == L"你好 one Ｃ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 2, 12) == L"‹好 one ＣＵ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 3, 12) == L"好 one ＣＵ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 4, 12) == L"‹ one ＣＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 5, 12) == L" one ＣＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 6, 12) == L"one ＣＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 7, 12) == L"ne ＣＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 8, 12) == L"e ＣＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 9, 12) == L" ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 10, 12) == L"ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 11, 12) == L"‹ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 12, 12) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 13, 12) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 14, 12) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 15, 12) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 16, 12) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 17, 12) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 18, 12) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 19, 12) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 20, 12) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 21, 12) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 0, 13) == L"你好 one ＣＵ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 1, 13) == L"你好 one ＣＵ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 2, 13) == L"‹好 one ＣＵ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 3, 13) == L"好 one ＣＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 4, 13) == L"‹ one ＣＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 5, 13) == L" one ＣＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 6, 13) == L"one ＣＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 7, 13) == L"ne ＣＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 8, 13) == L"e ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 9, 13) == L" ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 10, 13) == L"ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 11, 13) == L"‹ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 12, 13) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 13, 13) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 14, 13) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 15, 13) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 16, 13) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 17, 13) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 18, 13) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 19, 13) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 20, 13) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 21, 13) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 0, 14) == L"你好 one ＣＵ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 1, 14) == L"你好 one ＣＵ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 2, 14) == L"‹好 one ＣＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 3, 14) == L"好 one ＣＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 4, 14) == L"‹ one ＣＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 5, 14) == L" one ＣＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 6, 14) == L"one ＣＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 7, 14) == L"ne ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 8, 14) == L"e ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 9, 14) == L" ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 10, 14) == L"ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 11, 14) == L"‹ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 12, 14) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 13, 14) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 14, 14) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 15, 14) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 16, 14) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 17, 14) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 18, 14) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 19, 14) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 20, 14) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 21, 14) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 0, 15) == L"你好 one ＣＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 1, 15) == L"你好 one ＣＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 2, 15) == L"‹好 one ＣＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 3, 15) == L"好 one ＣＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 4, 15) == L"‹ one ＣＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 5, 15) == L" one ＣＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 6, 15) == L"one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 7, 15) == L"ne ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 8, 15) == L"e ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 9, 15) == L" ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 10, 15) == L"ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 11, 15) == L"‹ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 12, 15) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 13, 15) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 14, 15) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 15, 15) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 16, 15) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 17, 15) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 18, 15) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 19, 15) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 20, 15) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 21, 15) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 0, 16) == L"你好 one ＣＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 1, 16) == L"你好 one ＣＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 2, 16) == L"‹好 one ＣＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 3, 16) == L"好 one ＣＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 4, 16) == L"‹ one ＣＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 5, 16) == L" one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 6, 16) == L"one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 7, 16) == L"ne ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 8, 16) == L"e ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 9, 16) == L" ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 10, 16) == L"ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 11, 16) == L"‹ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 12, 16) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 13, 16) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 14, 16) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 15, 16) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 16, 16) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 17, 16) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 18, 16) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 19, 16) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 20, 16) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 21, 16) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 0, 17) == L"你好 one ＣＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 1, 17) == L"你好 one ＣＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 2, 17) == L"‹好 one ＣＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 3, 17) == L"好 one ＣＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 4, 17) == L"‹ one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 5, 17) == L" one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 6, 17) == L"one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 7, 17) == L"ne ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 8, 17) == L"e ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 9, 17) == L" ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 10, 17) == L"ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 11, 17) == L"‹ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 12, 17) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 13, 17) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 14, 17) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 15, 17) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 16, 17) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 17, 17) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 18, 17) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 19, 17) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 20, 17) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 21, 17) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 0, 18) == L"你好 one ＣＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 1, 18) == L"你好 one ＣＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 2, 18) == L"‹好 one ＣＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 3, 18) == L"好 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 4, 18) == L"‹ one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 5, 18) == L" one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 6, 18) == L"one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 7, 18) == L"ne ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 8, 18) == L"e ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 9, 18) == L" ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 10, 18) == L"ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 11, 18) == L"‹ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 12, 18) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 13, 18) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 14, 18) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 15, 18) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 16, 18) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 17, 18) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 18, 18) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 19, 18) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 20, 18) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 21, 18) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 0, 19) == L"你好 one ＣＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 1, 19) == L"你好 one ＣＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 2, 19) == L"‹好 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 3, 19) == L"好 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 4, 19) == L"‹ one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 5, 19) == L" one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 6, 19) == L"one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 7, 19) == L"ne ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 8, 19) == L"e ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 9, 19) == L" ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 10, 19) == L"ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 11, 19) == L"‹ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 12, 19) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 13, 19) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 14, 19) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 15, 19) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 16, 19) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 17, 19) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 18, 19) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 19, 19) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 20, 19) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 21, 19) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 0, 20) == L"你好 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 1, 20) == L"你好 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 2, 20) == L"‹好 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 3, 20) == L"好 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 4, 20) == L"‹ one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 5, 20) == L" one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 6, 20) == L"one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 7, 20) == L"ne ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 8, 20) == L"e ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 9, 20) == L" ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 10, 20) == L"ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 11, 20) == L"‹ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 12, 20) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 13, 20) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 14, 20) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 15, 20) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 16, 20) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 17, 20) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 18, 20) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 19, 20) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 20, 20) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"你好 one ＣＵＴ more", 21, 20) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 0, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 1, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 2, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 3, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 4, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 5, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 6, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 7, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 8, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 9, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 10, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 11, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 12, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 13, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 14, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 15, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 16, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 17, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 18, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 19, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 20, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 21, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 22, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 23, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 24, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 0, 1) == L"1" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 1, 1) == L"1" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 2, 1) == L"2" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 3, 1) == L"3" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 4, 1) == L"4" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 5, 1) == L"5" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 6, 1) == L"6" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 7, 1) == L"7" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 8, 1) == L" " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 9, 1) == L"o" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 10, 1) == L"n" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 11, 1) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 12, 1) == L" " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 13, 1) == L"›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 14, 1) == L"‹" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 15, 1) == L"›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 16, 1) == L"‹" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 17, 1) == L"›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 18, 1) == L"‹" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 19, 1) == L" " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 20, 1) == L"m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 21, 1) == L"o" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 22, 1) == L"r" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 23, 1) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 24, 1) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 0, 2) == L"12" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 1, 2) == L"12" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 2, 2) == L"23" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 3, 2) == L"34" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 4, 2) == L"45" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 5, 2) == L"56" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 6, 2) == L"67" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 7, 2) == L"7 " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 8, 2) == L" o" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 9, 2) == L"on" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 10, 2) == L"ne" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 11, 2) == L"e " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 12, 2) == L" ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 13, 2) == L"Ｃ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 14, 2) == L"‹›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 15, 2) == L"Ｕ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 16, 2) == L"‹›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 17, 2) == L"Ｔ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 18, 2) == L"‹ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 19, 2) == L" m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 20, 2) == L"mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 21, 2) == L"or" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 22, 2) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 23, 2) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 24, 2) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 0, 3) == L"123" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 1, 3) == L"123" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 2, 3) == L"234" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 3, 3) == L"345" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 4, 3) == L"456" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 5, 3) == L"567" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 6, 3) == L"67 " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 7, 3) == L"7 o" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 8, 3) == L" on" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 9, 3) == L"one" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 10, 3) == L"ne " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 11, 3) == L"e ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 12, 3) == L" Ｃ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 13, 3) == L"Ｃ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 14, 3) == L"‹Ｕ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 15, 3) == L"Ｕ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 16, 3) == L"‹Ｔ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 17, 3) == L"Ｔ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 18, 3) == L"‹ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 19, 3) == L" mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 20, 3) == L"mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 21, 3) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 22, 3) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 23, 3) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 24, 3) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 0, 4) == L"1234" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 1, 4) == L"1234" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 2, 4) == L"2345" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 3, 4) == L"3456" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 4, 4) == L"4567" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 5, 4) == L"567 " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 6, 4) == L"67 o" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 7, 4) == L"7 on" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 8, 4) == L" one" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 9, 4) == L"one " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 10, 4) == L"ne ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 11, 4) == L"e Ｃ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 12, 4) == L" Ｃ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 13, 4) == L"ＣＵ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 14, 4) == L"‹Ｕ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 15, 4) == L"ＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 16, 4) == L"‹Ｔ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 17, 4) == L"Ｔ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 18, 4) == L"‹ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 19, 4) == L" mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 20, 4) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 21, 4) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 22, 4) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 23, 4) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 24, 4) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 0, 5) == L"12345" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 1, 5) == L"12345" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 2, 5) == L"23456" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 3, 5) == L"34567" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 4, 5) == L"4567 " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 5, 5) == L"567 o" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 6, 5) == L"67 on" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 7, 5) == L"7 one" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 8, 5) == L" one " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 9, 5) == L"one ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 10, 5) == L"ne Ｃ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 11, 5) == L"e Ｃ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 12, 5) == L" ＣＵ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 13, 5) == L"ＣＵ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 14, 5) == L"‹ＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 15, 5) == L"ＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 16, 5) == L"‹Ｔ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 17, 5) == L"Ｔ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 18, 5) == L"‹ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 19, 5) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 20, 5) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 21, 5) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 22, 5) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 23, 5) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 24, 5) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 0, 6) == L"123456" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 1, 6) == L"123456" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 2, 6) == L"234567" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 3, 6) == L"34567 " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 4, 6) == L"4567 o" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 5, 6) == L"567 on" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 6, 6) == L"67 one" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 7, 6) == L"7 one " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 8, 6) == L" one ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 9, 6) == L"one Ｃ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 10, 6) == L"ne Ｃ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 11, 6) == L"e ＣＵ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 12, 6) == L" ＣＵ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 13, 6) == L"ＣＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 14, 6) == L"‹ＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 15, 6) == L"ＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 16, 6) == L"‹Ｔ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 17, 6) == L"Ｔ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 18, 6) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 19, 6) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 20, 6) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 21, 6) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 22, 6) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 23, 6) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 24, 6) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 0, 7) == L"1234567" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 1, 7) == L"1234567" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 2, 7) == L"234567 " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 3, 7) == L"34567 o" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 4, 7) == L"4567 on" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 5, 7) == L"567 one" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 6, 7) == L"67 one " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 7, 7) == L"7 one ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 8, 7) == L" one Ｃ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 9, 7) == L"one Ｃ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 10, 7) == L"ne ＣＵ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 11, 7) == L"e ＣＵ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 12, 7) == L" ＣＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 13, 7) == L"ＣＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 14, 7) == L"‹ＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 15, 7) == L"ＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 16, 7) == L"‹Ｔ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 17, 7) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 18, 7) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 19, 7) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 20, 7) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 21, 7) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 22, 7) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 23, 7) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 24, 7) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 0, 8) == L"1234567 " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 1, 8) == L"1234567 " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 2, 8) == L"234567 o" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 3, 8) == L"34567 on" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 4, 8) == L"4567 one" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 5, 8) == L"567 one " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 6, 8) == L"67 one ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 7, 8) == L"7 one Ｃ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 8, 8) == L" one Ｃ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 9, 8) == L"one ＣＵ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 10, 8) == L"ne ＣＵ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 11, 8) == L"e ＣＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 12, 8) == L" ＣＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 13, 8) == L"ＣＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 14, 8) == L"‹ＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 15, 8) == L"ＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 16, 8) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 17, 8) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 18, 8) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 19, 8) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 20, 8) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 21, 8) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 22, 8) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 23, 8) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 24, 8) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 0, 9) == L"1234567 o" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 1, 9) == L"1234567 o" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 2, 9) == L"234567 on" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 3, 9) == L"34567 one" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 4, 9) == L"4567 one " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 5, 9) == L"567 one ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 6, 9) == L"67 one Ｃ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 7, 9) == L"7 one Ｃ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 8, 9) == L" one ＣＵ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 9, 9) == L"one ＣＵ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 10, 9) == L"ne ＣＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 11, 9) == L"e ＣＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 12, 9) == L" ＣＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 13, 9) == L"ＣＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 14, 9) == L"‹ＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 15, 9) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 16, 9) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 17, 9) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 18, 9) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 19, 9) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 20, 9) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 21, 9) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 22, 9) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 23, 9) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 24, 9) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 0, 10) == L"1234567 on" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 1, 10) == L"1234567 on" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 2, 10) == L"234567 one" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 3, 10) == L"34567 one " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 4, 10) == L"4567 one ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 5, 10) == L"567 one Ｃ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 6, 10) == L"67 one Ｃ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 7, 10) == L"7 one ＣＵ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 8, 10) == L" one ＣＵ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 9, 10) == L"one ＣＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 10, 10) == L"ne ＣＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 11, 10) == L"e ＣＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 12, 10) == L" ＣＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 13, 10) == L"ＣＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 14, 10) == L"‹ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 15, 10) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 16, 10) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 17, 10) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 18, 10) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 19, 10) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 20, 10) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 21, 10) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 22, 10) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 23, 10) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 24, 10) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 0, 11) == L"1234567 one" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 1, 11) == L"1234567 one" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 2, 11) == L"234567 one " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 3, 11) == L"34567 one ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 4, 11) == L"4567 one Ｃ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 5, 11) == L"567 one Ｃ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 6, 11) == L"67 one ＣＵ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 7, 11) == L"7 one ＣＵ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 8, 11) == L" one ＣＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 9, 11) == L"one ＣＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 10, 11) == L"ne ＣＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 11, 11) == L"e ＣＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 12, 11) == L" ＣＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 13, 11) == L"ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 14, 11) == L"‹ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 15, 11) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 16, 11) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 17, 11) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 18, 11) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 19, 11) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 20, 11) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 21, 11) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 22, 11) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 23, 11) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 24, 11) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 0, 12) == L"1234567 one " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 1, 12) == L"1234567 one " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 2, 12) == L"234567 one ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 3, 12) == L"34567 one Ｃ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 4, 12) == L"4567 one Ｃ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 5, 12) == L"567 one ＣＵ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 6, 12) == L"67 one ＣＵ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 7, 12) == L"7 one ＣＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 8, 12) == L" one ＣＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 9, 12) == L"one ＣＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 10, 12) == L"ne ＣＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 11, 12) == L"e ＣＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 12, 12) == L" ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 13, 12) == L"ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 14, 12) == L"‹ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 15, 12) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 16, 12) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 17, 12) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 18, 12) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 19, 12) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 20, 12) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 21, 12) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 22, 12) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 23, 12) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 24, 12) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 0, 13) == L"1234567 one ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 1, 13) == L"1234567 one ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 2, 13) == L"234567 one Ｃ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 3, 13) == L"34567 one Ｃ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 4, 13) == L"4567 one ＣＵ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 5, 13) == L"567 one ＣＵ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 6, 13) == L"67 one ＣＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 7, 13) == L"7 one ＣＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 8, 13) == L" one ＣＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 9, 13) == L"one ＣＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 10, 13) == L"ne ＣＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 11, 13) == L"e ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 12, 13) == L" ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 13, 13) == L"ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 14, 13) == L"‹ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 15, 13) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 16, 13) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 17, 13) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 18, 13) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 19, 13) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 20, 13) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 21, 13) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 22, 13) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 23, 13) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 24, 13) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 0, 14) == L"1234567 one Ｃ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 1, 14) == L"1234567 one Ｃ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 2, 14) == L"234567 one Ｃ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 3, 14) == L"34567 one ＣＵ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 4, 14) == L"4567 one ＣＵ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 5, 14) == L"567 one ＣＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 6, 14) == L"67 one ＣＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 7, 14) == L"7 one ＣＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 8, 14) == L" one ＣＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 9, 14) == L"one ＣＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 10, 14) == L"ne ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 11, 14) == L"e ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 12, 14) == L" ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 13, 14) == L"ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 14, 14) == L"‹ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 15, 14) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 16, 14) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 17, 14) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 18, 14) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 19, 14) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 20, 14) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 21, 14) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 22, 14) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 23, 14) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 24, 14) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 0, 15) == L"1234567 one Ｃ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 1, 15) == L"1234567 one Ｃ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 2, 15) == L"234567 one ＣＵ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 3, 15) == L"34567 one ＣＵ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 4, 15) == L"4567 one ＣＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 5, 15) == L"567 one ＣＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 6, 15) == L"67 one ＣＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 7, 15) == L"7 one ＣＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 8, 15) == L" one ＣＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 9, 15) == L"one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 10, 15) == L"ne ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 11, 15) == L"e ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 12, 15) == L" ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 13, 15) == L"ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 14, 15) == L"‹ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 15, 15) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 16, 15) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 17, 15) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 18, 15) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 19, 15) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 20, 15) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 21, 15) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 22, 15) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 23, 15) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 24, 15) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 0, 16) == L"1234567 one ＣＵ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 1, 16) == L"1234567 one ＣＵ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 2, 16) == L"234567 one ＣＵ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 3, 16) == L"34567 one ＣＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 4, 16) == L"4567 one ＣＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 5, 16) == L"567 one ＣＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 6, 16) == L"67 one ＣＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 7, 16) == L"7 one ＣＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 8, 16) == L" one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 9, 16) == L"one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 10, 16) == L"ne ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 11, 16) == L"e ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 12, 16) == L" ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 13, 16) == L"ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 14, 16) == L"‹ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 15, 16) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 16, 16) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 17, 16) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 18, 16) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 19, 16) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 20, 16) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 21, 16) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 22, 16) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 23, 16) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 24, 16) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 0, 17) == L"1234567 one ＣＵ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 1, 17) == L"1234567 one ＣＵ›" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 2, 17) == L"234567 one ＣＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 3, 17) == L"34567 one ＣＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 4, 17) == L"4567 one ＣＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 5, 17) == L"567 one ＣＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 6, 17) == L"67 one ＣＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 7, 17) == L"7 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 8, 17) == L" one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 9, 17) == L"one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 10, 17) == L"ne ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 11, 17) == L"e ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 12, 17) == L" ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 13, 17) == L"ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 14, 17) == L"‹ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 15, 17) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 16, 17) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 17, 17) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 18, 17) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 19, 17) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 20, 17) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 21, 17) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 22, 17) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 23, 17) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 24, 17) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 0, 18) == L"1234567 one ＣＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 1, 18) == L"1234567 one ＣＵＴ" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 2, 18) == L"234567 one ＣＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 3, 18) == L"34567 one ＣＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 4, 18) == L"4567 one ＣＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 5, 18) == L"567 one ＣＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 6, 18) == L"67 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 7, 18) == L"7 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 8, 18) == L" one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 9, 18) == L"one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 10, 18) == L"ne ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 11, 18) == L"e ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 12, 18) == L" ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 13, 18) == L"ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 14, 18) == L"‹ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 15, 18) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 16, 18) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 17, 18) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 18, 18) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 19, 18) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 20, 18) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 21, 18) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 22, 18) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 23, 18) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 24, 18) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 0, 19) == L"1234567 one ＣＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 1, 19) == L"1234567 one ＣＵＴ " );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 2, 19) == L"234567 one ＣＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 3, 19) == L"34567 one ＣＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 4, 19) == L"4567 one ＣＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 5, 19) == L"567 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 6, 19) == L"67 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 7, 19) == L"7 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 8, 19) == L" one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 9, 19) == L"one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 10, 19) == L"ne ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 11, 19) == L"e ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 12, 19) == L" ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 13, 19) == L"ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 14, 19) == L"‹ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 15, 19) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 16, 19) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 17, 19) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 18, 19) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 19, 19) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 20, 19) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 21, 19) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 22, 19) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 23, 19) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 24, 19) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 0, 20) == L"1234567 one ＣＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 1, 20) == L"1234567 one ＣＵＴ m" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 2, 20) == L"234567 one ＣＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 3, 20) == L"34567 one ＣＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 4, 20) == L"4567 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 5, 20) == L"567 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 6, 20) == L"67 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 7, 20) == L"7 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 8, 20) == L" one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 9, 20) == L"one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 10, 20) == L"ne ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 11, 20) == L"e ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 12, 20) == L" ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 13, 20) == L"ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 14, 20) == L"‹ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 15, 20) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 16, 20) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 17, 20) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 18, 20) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 19, 20) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 20, 20) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 21, 20) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 22, 20) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 23, 20) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 24, 20) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 0, 21) == L"1234567 one ＣＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 1, 21) == L"1234567 one ＣＵＴ mo" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 2, 21) == L"234567 one ＣＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 3, 21) == L"34567 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 4, 21) == L"4567 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 5, 21) == L"567 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 6, 21) == L"67 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 7, 21) == L"7 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 8, 21) == L" one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 9, 21) == L"one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 10, 21) == L"ne ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 11, 21) == L"e ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 12, 21) == L" ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 13, 21) == L"ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 14, 21) == L"‹ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 15, 21) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 16, 21) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 17, 21) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 18, 21) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 19, 21) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 20, 21) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 21, 21) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 22, 21) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 23, 21) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 24, 21) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 0, 22) == L"1234567 one ＣＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 1, 22) == L"1234567 one ＣＵＴ mor" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 2, 22) == L"234567 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 3, 22) == L"34567 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 4, 22) == L"4567 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 5, 22) == L"567 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 6, 22) == L"67 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 7, 22) == L"7 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 8, 22) == L" one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 9, 22) == L"one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 10, 22) == L"ne ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 11, 22) == L"e ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 12, 22) == L" ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 13, 22) == L"ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 14, 22) == L"‹ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 15, 22) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 16, 22) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 17, 22) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 18, 22) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 19, 22) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 20, 22) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 21, 22) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 22, 22) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 23, 22) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 24, 22) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 0, 23) == L"1234567 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 1, 23) == L"1234567 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 2, 23) == L"234567 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 3, 23) == L"34567 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 4, 23) == L"4567 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 5, 23) == L"567 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 6, 23) == L"67 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 7, 23) == L"7 one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 8, 23) == L" one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 9, 23) == L"one ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 10, 23) == L"ne ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 11, 23) == L"e ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 12, 23) == L" ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 13, 23) == L"ＣＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 14, 23) == L"‹ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 15, 23) == L"ＵＴ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 16, 23) == L"‹Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 17, 23) == L"Ｔ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 18, 23) == L"‹ more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 19, 23) == L" more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 20, 23) == L"more" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 21, 23) == L"ore" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 22, 23) == L"re" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 23, 23) == L"e" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"1234567 one ＣＵＴ more", 24, 23) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"o\U0000031b\U00000323=\U00001ee3", 0, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"o\U0000031b\U00000323=\U00001ee3", 1, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"o\U0000031b\U00000323=\U00001ee3", 2, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"o\U0000031b\U00000323=\U00001ee3", 3, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"o\U0000031b\U00000323=\U00001ee3", 4, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"o\U0000031b\U00000323=\U00001ee3", 0, 1) == L"o\U0000031b\U00000323" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"o\U0000031b\U00000323=\U00001ee3", 1, 1) == L"o\U0000031b\U00000323" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"o\U0000031b\U00000323=\U00001ee3", 2, 1) == L"=" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"o\U0000031b\U00000323=\U00001ee3", 3, 1) == L"\U00001ee3" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"o\U0000031b\U00000323=\U00001ee3", 4, 1) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"o\U0000031b\U00000323=\U00001ee3", 0, 2) == L"o\U0000031b\U00000323=" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"o\U0000031b\U00000323=\U00001ee3", 1, 2) == L"o\U0000031b\U00000323=" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"o\U0000031b\U00000323=\U00001ee3", 2, 2) == L"=\U00001ee3" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"o\U0000031b\U00000323=\U00001ee3", 3, 2) == L"\U00001ee3" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"o\U0000031b\U00000323=\U00001ee3", 4, 2) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"o\U0000031b\U00000323=\U00001ee3", 0, 3) == L"o\U0000031b\U00000323=\U00001ee3" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"o\U0000031b\U00000323=\U00001ee3", 1, 3) == L"o\U0000031b\U00000323=\U00001ee3" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"o\U0000031b\U00000323=\U00001ee3", 2, 3) == L"=\U00001ee3" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"o\U0000031b\U00000323=\U00001ee3", 3, 3) == L"\U00001ee3" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"o\U0000031b\U00000323=\U00001ee3", 4, 3) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 0, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 1, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 2, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 3, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 4, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 5, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 6, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 7, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 8, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 9, 0) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 0, 1) == L"S" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 1, 1) == L"S" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 2, 1) == L"T" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 3, 1) == L"A" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 4, 1) == L"R" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 5, 1) == L"G" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 6, 1) == L"Λ̊" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 7, 1) == L"T" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 8, 1) == L"E" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 9, 1) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 0, 2) == L"ST" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 1, 2) == L"ST" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 2, 2) == L"TA" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 3, 2) == L"AR" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 4, 2) == L"RG" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 5, 2) == L"GΛ̊" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 6, 2) == L"Λ̊T" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 7, 2) == L"TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 8, 2) == L"E" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 9, 2) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 0, 3) == L"STA" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 1, 3) == L"STA" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 2, 3) == L"TAR" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 3, 3) == L"ARG" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 4, 3) == L"RGΛ̊" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 5, 3) == L"GΛ̊T" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 6, 3) == L"Λ̊TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 7, 3) == L"TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 8, 3) == L"E" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 9, 3) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 0, 4) == L"STAR" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 1, 4) == L"STAR" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 2, 4) == L"TARG" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 3, 4) == L"ARGΛ̊" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 4, 4) == L"RGΛ̊T" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 5, 4) == L"GΛ̊TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 6, 4) == L"Λ̊TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 7, 4) == L"TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 8, 4) == L"E" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 9, 4) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 0, 5) == L"STARG" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 1, 5) == L"STARG" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 2, 5) == L"TARGΛ̊" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 3, 5) == L"ARGΛ̊T" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 4, 5) == L"RGΛ̊TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 5, 5) == L"GΛ̊TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 6, 5) == L"Λ̊TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 7, 5) == L"TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 8, 5) == L"E" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 9, 5) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 0, 6) == L"STARGΛ̊" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 1, 6) == L"STARGΛ̊" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 2, 6) == L"TARGΛ̊T" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 3, 6) == L"ARGΛ̊TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 4, 6) == L"RGΛ̊TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 5, 6) == L"GΛ̊TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 6, 6) == L"Λ̊TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 7, 6) == L"TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 8, 6) == L"E" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 9, 6) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 0, 7) == L"STARGΛ̊T" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 1, 7) == L"STARGΛ̊T" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 2, 7) == L"TARGΛ̊TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 3, 7) == L"ARGΛ̊TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 4, 7) == L"RGΛ̊TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 5, 7) == L"GΛ̊TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 6, 7) == L"Λ̊TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 7, 7) == L"TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 8, 7) == L"E" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 9, 7) == L"" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 0, 8) == L"STARGΛ̊TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 1, 8) == L"STARGΛ̊TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 2, 8) == L"TARGΛ̊TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 3, 8) == L"ARGΛ̊TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 4, 8) == L"RGΛ̊TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 5, 8) == L"GΛ̊TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 6, 8) == L"Λ̊TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 7, 8) == L"TE" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 8, 8) == L"E" );
  CPPUNIT_ASSERT ( finalcut::getColumnSubString(L"STARGΛ̊TE", 9, 8) == L"" );
}

//----------------------------------------------------------------------
void FTermFunctionsTest::combiningCharacterTest()
{
  // Leading zero-width characters
  std::wstring combining = L"\U00000323\U00000300\U0000ff2f\n";  // [] [] Ｏ [NL]
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 0) == -1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 1) == -1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 2) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 3) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 0) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 1) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 2) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 3) == 1 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 0) == NOT_FOUND );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 1) == NOT_FOUND );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 2) == NOT_FOUND );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 3) == 2 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 4) == 3 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 0) == 2 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 1) == 2 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 2) == 3 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 3) == NOT_FOUND );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 4) == NOT_FOUND );

  // Characters with separate and with combined diacritical marks
  combining = L"u\U00000300=\U000000f9";  // u ` = ù
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 0) == 2 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 1) == -1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 2) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 3) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 4) == -1 );  // Position is outside the string
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 0) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 1) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 2) == 2 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 3) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 4) == -1 );  // Position is outside the string
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 0) == NOT_FOUND );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 1) == 0 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 2) == 0 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 3) == 2 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 4) == 3 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 0) == 2 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 1) == 2 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 2) == 3 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 3) == NOT_FOUND );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 4) == NOT_FOUND );

  combining = L"o\U0000031b\U00000323=\U00001ee3";  // o ‍̛   ‍̣ = ợ
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 0) == 3 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 1) == -1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 2) == -1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 3) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 4) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 0) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 1) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 2) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 3) == 3 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 4) == 1 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 0) == NOT_FOUND );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 1) == 0 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 2) == 0 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 3) == 0 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 4) == 3 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 5) == 4 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 0) == 3 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 1) == 3 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 2) == 3 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 3) == 4 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 4) == NOT_FOUND );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 5) == NOT_FOUND );

  // Many combination characters
  combining = L"v" \
              L"\U00000300\U0000032e\U00000368" \
              L"\U00000364\U00000348\U0000034b";  // v ̀  ̮  ͨ  ͤ  ͈  ͋
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 0) == 7 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 1) == -1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 2) == -1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 3) == -1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 4) == -1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 5) == -1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 6) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 0) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 1) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 2) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 3) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 4) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 5) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 6) == -1 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 0) == NOT_FOUND );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 1) == 0 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 2) == 0 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 3) == 0 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 4) == 0 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 5) == 0 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 6) == 0 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 7) == 0 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 0) == NOT_FOUND );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 1) == NOT_FOUND );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 2) == NOT_FOUND );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 3) == NOT_FOUND );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 4) == NOT_FOUND );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 5) == NOT_FOUND );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 6) == NOT_FOUND );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 7) == NOT_FOUND );

  // Trailing uncombined zero-width characters
  combining = L"a\t\U00000300\U00000323";  // a [Tab] [] []
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 0) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 1) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 2) == -1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 3) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 0) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 1) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 2) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 3) == -1 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 0) == NOT_FOUND );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 1) == 0 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 2) == 1 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 3) == 1 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 4) == 1 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 0) == 1 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 1) == NOT_FOUND );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 2) == NOT_FOUND );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 3) == NOT_FOUND );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 4) == NOT_FOUND );

  // Thai (ISO 10646-1 - UCS implementation level 2)
  combining = L"๏ แผ่นดินฮั่นเสื่อมโทรมแสนสังเวช";
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 0) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 1) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 2) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 3) == 2 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 4) == -1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 5) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 6) == 2 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 7) == -1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 8) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 9) == 3 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 10) == -1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 11) == -1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 12) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 13) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 14) == 3 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 15) == -1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 16) == -1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 17) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 18) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 19) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 20) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 21) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 22) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 23) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 24) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 25) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 26) == 2 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 27) == -1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 28) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 29) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 30) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 31) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 0) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 1) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 2) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 3) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 4) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 5) == 2 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 6) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 7) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 8) == 2 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 9) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 10) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 11) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 12) == 3 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 13) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 14) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 15) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 16) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 17) == 3 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 18) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 19) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 20) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 21) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 22) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 23) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 24) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 25) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 26) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 27) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 28) == 2 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 29) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 30) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 31) == 1 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 0) == NOT_FOUND );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 1) == 0 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 2) == 1 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 3) == 2 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 4) == 3 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 5) == 3 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 6) == 5 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 7) == 6 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 8) == 6 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 9) == 8 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 10) == 9 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 11) == 9 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 12) == 9 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 13) == 12 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 14) == 13 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 15) == 14 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 16) == 14 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 17) == 14 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 18) == 17 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 19) == 18 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 20) == 19 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 21) == 20 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 22) == 21 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 23) == 22 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 24) == 23 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 25) == 24 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 26) == 25 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 27) == 26 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 28) == 26 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 29) == 28 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 30) == 29 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 31) == 30 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 32) == 31 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 0) == 1 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 1) == 2 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 2) == 3 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 3) == 5 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 4) == 5 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 5) == 6 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 6) == 8 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 7) == 8 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 8) == 9 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 9) == 12 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 10) == 12 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 11) == 12 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 12) == 13 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 13) == 14 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 14) == 17 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 15) == 17 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 16) == 17 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 17) == 18 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 18) == 19 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 19) == 20 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 20) == 21 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 21) == 22 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 22) == 23 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 23) == 24 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 24) == 25 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 25) == 26 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 26) == 28 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 27) == 28 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 28) == 29 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 29) == 30 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 30) == 31 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 31) == NOT_FOUND );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 32) == NOT_FOUND );

  // Devanagari script (ISO 10646-1 - UCS implementation level 2)
  combining = L"पन्ह पन्ह त्र र्च कृकृ ड्ड न्ह";
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 0) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 1) == 2 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 2) == -1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 3) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 4) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 5) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 6) == 2 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 7) == -1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 8) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 9) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 10) == 2 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 11) == -1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 12) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 13) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 14) == 2 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 15) == -1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 16) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 17) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 18) == 2 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 19) == -1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 20) == 2 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 21) == -1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 22) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 23) == 2 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 24) == -1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 25) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 26) == 1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 27) == 2 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 28) == -1 );
  CPPUNIT_ASSERT ( finalcut::getCharLength(combining, 29) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 0) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 1) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 2) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 3) == 2 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 4) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 5) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 6) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 7) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 8) == 2 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 9) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 10) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 11) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 12) == 2 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 13) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 14) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 15) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 16) == 2 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 17) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 18) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 19) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 20) == 2 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 21) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 22) == 2 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 23) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 24) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 25) == 2 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 26) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 27) == 1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 28) == -1 );
  CPPUNIT_ASSERT ( finalcut::getPrevCharLength(combining, 29) == 2 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 0) == NOT_FOUND );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 1) == 0 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 2) == 1 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 3) == 1 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 4) == 3 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 5) == 4 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 6) == 5 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 7) == 6 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 8) == 6 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 9) == 8 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 10) == 9 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 11) == 10 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 12) == 10 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 13) == 12 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 14) == 13 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 15) == 14 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 16) == 14 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 17) == 16 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 18) == 17 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 19) == 18 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 20) == 18 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 21) == 20 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 22) == 20 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 23) == 22 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 24) == 23 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 25) == 23 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 26) == 25 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 27) == 26 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 28) == 27 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 29) == 27 );
  CPPUNIT_ASSERT ( finalcut::searchLeftCharBegin(combining, 30) == 29 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 0) == 1 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 1) == 3 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 2) == 3 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 3) == 4 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 4) == 5 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 5) == 6 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 6) == 8 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 7) == 8 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 8) == 9 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 9) == 10 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 10) == 12 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 11) == 12 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 12) == 13 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 13) == 14 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 14) == 16 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 15) == 16 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 16) == 17 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 17) == 18 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 18) == 20 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 19) == 20 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 20) == 22 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 21) == 22 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 22) == 23 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 23) == 25 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 24) == 25 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 25) == 26 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 26) == 27 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 27) == 29 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 28) == 29 );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 29) == NOT_FOUND );
  CPPUNIT_ASSERT ( finalcut::searchRightCharBegin(combining, 30) == NOT_FOUND );
}

//----------------------------------------------------------------------
void FTermFunctionsTest::readCursorPosTest()
{
  auto& fterm_data = finalcut::FTermData::getInstance();
  finalcut::FTermDetection detect;
  fterm_data.setTermType("xterm");
  detect.setTerminalDetection(true);

  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    const finalcut::FPoint cursor_pos = finalcut::readCursorPos();
    CPPUNIT_ASSERT ( cursor_pos.getX() == 80 );
    CPPUNIT_ASSERT ( cursor_pos.getY() == 25 );

    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::console::xterm);
    int wstatus;

    if ( waitpid(pid, &wstatus, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;

    if ( WIFEXITED(wstatus) )
      CPPUNIT_ASSERT ( WEXITSTATUS(wstatus) == 0 );
  }
}


// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FTermFunctionsTest);

// The general unit test main part
#include <main-test.inc>
