/***********************************************************************
* fvtermbuffer-test.cpp - FVTermBuffer unit tests                      *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2021 Markus Gans                                           *
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

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <final/final.h>

//----------------------------------------------------------------------
// class FVTermBufferTest
//----------------------------------------------------------------------

class FVTermBufferTest : public CPPUNIT_NS::TestFixture
{
  public:
    FVTermBufferTest()
    {
      auto ret = std::setlocale(LC_CTYPE, "en_US.UTF-8");

      if ( ! ret )
      {
        if ( ! std::setlocale(LC_CTYPE, "C.UTF-8") )
          std::cerr << "No UTF-8 character set found!";
      }

      fwide(stdout, 1);  // Makes stream wide-character oriented
    }

  protected:
    void classNameTest();
    void noArgumentTest();
    void writeTest();
    void streamTest();
    void combiningCharacterTest();

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FVTermBufferTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (noArgumentTest);
    CPPUNIT_TEST (writeTest);
    CPPUNIT_TEST (streamTest);
    CPPUNIT_TEST (combiningCharacterTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();

    // Data member
    finalcut::FVTerm fvterm{};  // Needed for FVTerm::getFOutput()
};


//----------------------------------------------------------------------
void FVTermBufferTest::classNameTest()
{
  const finalcut::FVTermBuffer s;
  const finalcut::FString& classname = s.getClassName();
  CPPUNIT_ASSERT ( classname == "FVTermBuffer" );
}

//----------------------------------------------------------------------
void FVTermBufferTest::noArgumentTest()
{
  const finalcut::FVTermBuffer vterm_buf{};
  CPPUNIT_ASSERT ( vterm_buf.isEmpty() );
  CPPUNIT_ASSERT ( vterm_buf.getLength() == 0 );
  std::vector<finalcut::FChar> data{};
  CPPUNIT_ASSERT ( vterm_buf.getBuffer() == data );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer().size() == 0 );
  CPPUNIT_ASSERT ( vterm_buf.begin() == vterm_buf.end() );
  CPPUNIT_ASSERT ( finalcut::UNICODE_MAX == 5 );
}

//----------------------------------------------------------------------
void FVTermBufferTest::writeTest()
{
  auto& fterm_data = finalcut::FTermData::getInstance();
  fterm_data.setTermEncoding (finalcut::Encoding::UTF8);
  finalcut::FVTermBuffer vterm_buf{};

  // Write wchar_t
  const wchar_t wch{L'\U0000263a'};  // ☺
  vterm_buf.print(wch);
  CPPUNIT_ASSERT ( ! vterm_buf.isEmpty() );
  CPPUNIT_ASSERT ( vterm_buf.getLength() == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer().size() == 1 );
  CPPUNIT_ASSERT ( vterm_buf.begin() + 1 == vterm_buf.end() );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[0] == L'☺' );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[2] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[3] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[4] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.front().encoded_char[0] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.front().encoded_char[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.front().encoded_char[2] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.front().encoded_char[3] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.front().encoded_char[4] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.front().fg_color == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( vterm_buf.front().bg_color == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( vterm_buf.front().attr.byte[0] == 0 );
  CPPUNIT_ASSERT ( vterm_buf.front().attr.byte[1] == 0 );
  CPPUNIT_ASSERT ( vterm_buf.front().attr.byte[2] != 0 );
  CPPUNIT_ASSERT ( vterm_buf.front().attr.byte[3] == 0 );
  CPPUNIT_ASSERT ( vterm_buf.front().attr.bit.char_width == 1 );
  vterm_buf.front().attr.bit.char_width = 0;
  CPPUNIT_ASSERT ( vterm_buf.front().attr.byte[2] == 0 );
  CPPUNIT_ASSERT ( vterm_buf.toString() == finalcut::FString(wch) );

  // Clear after write
  vterm_buf.clear();
  CPPUNIT_ASSERT ( vterm_buf.isEmpty() );
  CPPUNIT_ASSERT ( vterm_buf.getLength() == 0 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer().size() == 0 );
  CPPUNIT_ASSERT ( vterm_buf.begin() == vterm_buf.end() );
  CPPUNIT_ASSERT ( vterm_buf.toString() == finalcut::FString() );

  // Write char
  const char ch{'@'};
  vterm_buf.print(ch);
  CPPUNIT_ASSERT ( ! vterm_buf.isEmpty() );
  CPPUNIT_ASSERT ( vterm_buf.getLength() == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer().size() == 1 );
  CPPUNIT_ASSERT ( vterm_buf.begin() + 1 == vterm_buf.end() );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[0] == L'@' );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[2] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[3] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[4] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.front().encoded_char[0] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.front().encoded_char[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.front().encoded_char[2] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.front().encoded_char[3] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.front().encoded_char[4] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.front().fg_color == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( vterm_buf.front().bg_color == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( vterm_buf.front().attr.byte[0] == 0 );
  CPPUNIT_ASSERT ( vterm_buf.front().attr.byte[1] == 0 );
  CPPUNIT_ASSERT ( vterm_buf.front().attr.byte[2] != 0 );
  CPPUNIT_ASSERT ( vterm_buf.front().attr.byte[3] == 0 );
  CPPUNIT_ASSERT ( vterm_buf.front().attr.bit.char_width == 1 );
  vterm_buf.front().attr.bit.char_width = 0;
  CPPUNIT_ASSERT ( vterm_buf.front().attr.byte[2] == 0 );
  CPPUNIT_ASSERT ( vterm_buf.toString() == finalcut::FString(ch) );

  // Write FString
  const finalcut::FString str = "abc…ｘｙｚ";
  vterm_buf.clear();
  vterm_buf.print(str);
  CPPUNIT_ASSERT ( ! vterm_buf.isEmpty() );
  CPPUNIT_ASSERT ( vterm_buf.getLength() == 7 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer().size() == 7 );
  CPPUNIT_ASSERT ( vterm_buf.begin() + 7 == vterm_buf.end() );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].ch[0] == L'a' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].ch[0] == L'b' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].ch[0] == L'c' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[3].ch[0] == L'…' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[4].ch[0] == L'ｘ' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[5].ch[0] == L'ｙ' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[6].ch[0] == L'ｚ' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[3].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[4].attr.bit.char_width == 2 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[5].attr.bit.char_width == 2 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[6].attr.bit.char_width == 2 );
  CPPUNIT_ASSERT ( vterm_buf.toString() == str );

  for (std::size_t i{0}; i < 7; i++)
  {
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].ch[1] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].ch[2] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].ch[3] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].ch[4] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].encoded_char[0] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].encoded_char[1] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].encoded_char[2] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].encoded_char[3] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].encoded_char[4] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].fg_color == finalcut::FColor::Default );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].bg_color == finalcut::FColor::Default );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].attr.byte[0] == 0 );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].attr.byte[1] == 0 );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].attr.byte[2] != 0 );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].attr.byte[3] == 0 );
  }

  // Write formatted output
  std::setlocale(LC_NUMERIC, "C");
  vterm_buf.clear();
  vterm_buf.printf ("%'.2f%C", 0.25 * 7.0, L'£');
  CPPUNIT_ASSERT ( ! vterm_buf.isEmpty() );
  CPPUNIT_ASSERT ( vterm_buf.getLength() == 5 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer().size() == 5 );
  CPPUNIT_ASSERT ( vterm_buf.begin() + 5 == vterm_buf.end() );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].ch[0] == L'1' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].ch[0] == L'.' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].ch[0] == L'7' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[3].ch[0] == L'5' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[4].ch[0] == L'£' );
  CPPUNIT_ASSERT ( vterm_buf.toString() == "1.75£" );

  for (std::size_t i{0}; i < 5; i++)
  {
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].ch[1] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].ch[2] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].ch[3] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].ch[4] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].encoded_char[0] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].encoded_char[1] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].encoded_char[2] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].encoded_char[3] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].encoded_char[4] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].fg_color == finalcut::FColor::Default );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].bg_color == finalcut::FColor::Default );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].attr.byte[0] == 0 );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].attr.byte[1] == 0 );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].attr.byte[2] != 0 );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].attr.byte[3] == 0 );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].attr.bit.char_width == 1 );
  }

  // Write with color
  vterm_buf.clear();
  finalcut::FVTerm::setColor(finalcut::FColor::Default, finalcut::FColor::Default);
  vterm_buf.print (L'♥');
  auto color_pair = finalcut::FColorPair(finalcut::FColor::DarkRed, finalcut::FColor::Yellow4);
  vterm_buf.print (color_pair);
  vterm_buf.print (L'☺');
  finalcut::FVTerm::setNormal();
  vterm_buf.print ("♪");
  color_pair = finalcut::FColorPair(finalcut::FColor::Black, finalcut::FColor::White);
  vterm_buf.print (color_pair);
  vterm_buf.print ("↑");
  CPPUNIT_ASSERT ( ! vterm_buf.isEmpty() );
  CPPUNIT_ASSERT ( vterm_buf.getLength() == 4 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer().size() == 4 );
  CPPUNIT_ASSERT ( vterm_buf.begin() + 4 == vterm_buf.end() );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].ch[0] == L'♥' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].ch[0] == L'☺' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].ch[0] == L'♪' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[3].ch[0] == L'↑' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].fg_color == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].bg_color == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].fg_color == finalcut::FColor::DarkRed );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].bg_color == finalcut::FColor::Yellow4 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].fg_color == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].bg_color == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[3].fg_color == finalcut::FColor::Black );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[3].bg_color == finalcut::FColor::White );
  CPPUNIT_ASSERT ( vterm_buf.toString() == "♥☺♪↑" );

  for (std::size_t i{0}; i < 4; i++)
  {
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].ch[1] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].ch[2] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].ch[3] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].ch[4] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].encoded_char[0] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].encoded_char[1] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].encoded_char[2] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].encoded_char[3] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].encoded_char[4] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].attr.byte[0] == 0 );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].attr.byte[1] == 0 );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].attr.byte[2] != 0 );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].attr.byte[3] == 0 );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].attr.bit.char_width == 1 );
  }

  // Write with style
  auto multi_color_emojis = bool( wcswidth(L"☕⛄🧸🦡", 4) == 8 );
  vterm_buf.clear();
  auto style = finalcut::FStyle(finalcut::Style::Italic | finalcut::Style::Reverse);
  vterm_buf.print (style);
  CPPUNIT_ASSERT ( uInt(style.getStyle()) == uInt(finalcut::FVTerm::getAttribute().attr.byte[0]) );
  vterm_buf.print (L'☕');
  vterm_buf.print ( finalcut::FStyle(finalcut::Style::None) );  // Reset style
  style = finalcut::FStyle(finalcut::Style::Bold | finalcut::Style::Underline);
  vterm_buf.print (style);
  vterm_buf.print (L'⛄');
  vterm_buf.print ( finalcut::FStyle(finalcut::Style::Transparent) );
  vterm_buf.print (L'🧸');
  vterm_buf.print ( finalcut::FStyle(finalcut::Style::None) );  // Reset style
  vterm_buf.print (L'🦡');
  CPPUNIT_ASSERT ( ! vterm_buf.isEmpty() );
  CPPUNIT_ASSERT ( vterm_buf.getLength() == 4 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer().size() == 4 );
  CPPUNIT_ASSERT ( vterm_buf.begin() + 4 == vterm_buf.end() );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].ch[0] == L'☕' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].attr.bit.italic == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].attr.bit.reverse == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].attr.byte[0] != 0 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].attr.byte[1] == 0 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].ch[0] == L'⛄' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].attr.bit.bold == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].attr.bit.underline == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].attr.byte[0] != 0 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].attr.byte[1] == 0 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].ch[0] == L'🧸' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].attr.bit.bold == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].attr.bit.underline == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].attr.bit.transparent == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].attr.byte[0] != 0 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].attr.byte[1] != 0 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[3].ch[0] == L'🦡' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[3].attr.bit.bold == 0 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[3].attr.bit.underline == 0 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[3].attr.bit.transparent == 0 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[3].attr.byte[0] == 0 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[3].attr.byte[1] == 0 );
  CPPUNIT_ASSERT ( vterm_buf.toString() == "☕⛄🧸🦡" );

  for (std::size_t i{0}; i < 3; i++)
  {
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].ch[1] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].ch[2] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].ch[3] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].ch[4] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].encoded_char[0] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].encoded_char[1] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].encoded_char[2] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].encoded_char[3] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].encoded_char[4] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].attr.byte[3] == 0 );

    if ( multi_color_emojis )
    {
      CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].attr.byte[2] != 0 );
      CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].attr.bit.char_width == 2 );
    }
  }
}

//----------------------------------------------------------------------
void FVTermBufferTest::streamTest()
{
  auto multi_color_emojis = bool( wcswidth(L"🚧🚀🚴", 3) == 6 );
  auto& fterm_data = finalcut::FTermData::getInstance();
  fterm_data.setTermEncoding (finalcut::Encoding::UTF8);
  finalcut::FVTermBuffer::FCharVector fchar_vec = { finalcut::FChar{} };
  CPPUNIT_ASSERT ( fchar_vec.size() == 1 );
  fchar_vec.front().ch[0] = L'🚧';
  fchar_vec.front().fg_color = finalcut::FColor::White;
  fchar_vec.front().bg_color = finalcut::FColor::Cyan;
  finalcut::addColumnWidth(fchar_vec.front());
  finalcut::FVTermBuffer vterm_buf{};
  vterm_buf.print() << L'a'
                   << finalcut::FColorPair{finalcut::FColor::Yellow, finalcut::FColor::Blue}
                   << 1
                   << finalcut::FColorPair{finalcut::FColor::Cyan, finalcut::FColor::White}
                   << finalcut::FStyle(finalcut::Style::Dim)
                   << finalcut::UniChar::NF_Bullet
                   << finalcut::FStyle(finalcut::Style::Blink)
                   << fchar_vec
                   << std::string("🚀")
                   << finalcut::FStyle(finalcut::Style::None)
                   << finalcut::FStyle(finalcut::Style::DoubleUnderline)
                   << finalcut::FColorPair{finalcut::FColor::Black, finalcut::FColor::White}
                   << std::wstring(L"🚴");

  CPPUNIT_ASSERT ( ! vterm_buf.isEmpty() );
  CPPUNIT_ASSERT ( vterm_buf.getLength() == 6 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer().size() == 6 );
  CPPUNIT_ASSERT ( vterm_buf.begin() + 6 == vterm_buf.end() );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].ch[0] == L'a' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].fg_color == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].bg_color == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].attr.byte[0] == 0 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].attr.byte[1] == 0 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].ch[0] == L'1' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].fg_color == finalcut::FColor::Yellow );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].bg_color == finalcut::FColor::Blue );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].attr.byte[0] == 0 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].attr.byte[1] == 0 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].ch[0] == L'\U0000e1f9' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].fg_color == finalcut::FColor::Cyan );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].bg_color == finalcut::FColor::White );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].attr.bit.dim == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].attr.byte[0] != 0 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].attr.byte[1] == 0 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[3].ch[0] == L'🚧' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[3].fg_color == finalcut::FColor::White );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[3].bg_color == finalcut::FColor::Cyan );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[3].attr.byte[0] == 0 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[3].attr.byte[1] == 0 );

  if ( multi_color_emojis )
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[3].attr.bit.char_width == 2 );

  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[4].ch[0] == L'🚀' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[4].fg_color == finalcut::FColor::Cyan );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[4].bg_color == finalcut::FColor::White );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[4].attr.bit.dim == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[4].attr.bit.blink == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[4].attr.byte[0] != 0 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[4].attr.byte[1] == 0 );

  if ( multi_color_emojis )
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[4].attr.bit.char_width == 2 );

  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[5].ch[0] == L'🚴' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[5].fg_color == finalcut::FColor::Black );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[5].bg_color == finalcut::FColor::White );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[5].attr.bit.dbl_underline == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[5].attr.byte[0] == 0 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[5].attr.byte[1] != 0 );

  if ( multi_color_emojis )
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[5].attr.bit.char_width == 2 );

  CPPUNIT_ASSERT ( vterm_buf.toString() == "a1\U0000e1f9🚧🚀🚴" );

  for (std::size_t i{0}; i < 6; i++)
  {
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].ch[1] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].ch[2] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].ch[3] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].ch[4] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].encoded_char[0] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].encoded_char[1] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].encoded_char[2] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].encoded_char[3] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].encoded_char[4] == L'\0' );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].attr.byte[2] != 0 );
    CPPUNIT_ASSERT ( vterm_buf.getBuffer()[i].attr.byte[3] == 0 );
  }

  // Stream into FCharVector
  fchar_vec.clear();
  CPPUNIT_ASSERT ( fchar_vec.empty() );
  CPPUNIT_ASSERT ( fchar_vec.size() == 0 );
  CPPUNIT_ASSERT ( fchar_vec.begin() == fchar_vec.end() );

  fchar_vec << vterm_buf;
  CPPUNIT_ASSERT ( ! fchar_vec.empty() );
  CPPUNIT_ASSERT ( fchar_vec.size() == 6 );
  CPPUNIT_ASSERT ( fchar_vec.begin() + 6 == fchar_vec.end() );
  CPPUNIT_ASSERT ( fchar_vec[0].ch[0] == L'a' );
  CPPUNIT_ASSERT ( fchar_vec[1].ch[0] == L'1' );
  CPPUNIT_ASSERT ( fchar_vec[2].ch[0] == L'\U0000e1f9' );
  CPPUNIT_ASSERT ( fchar_vec[3].ch[0] == L'🚧' );
  CPPUNIT_ASSERT ( fchar_vec[4].ch[0] == L'🚀' );
  CPPUNIT_ASSERT ( fchar_vec[5].ch[0] == L'🚴' );
  CPPUNIT_ASSERT ( fchar_vec[0].fg_color == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( fchar_vec[0].bg_color == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( fchar_vec[1].fg_color == finalcut::FColor::Yellow );
  CPPUNIT_ASSERT ( fchar_vec[1].bg_color == finalcut::FColor::Blue );
  CPPUNIT_ASSERT ( fchar_vec[2].fg_color == finalcut::FColor::Cyan );
  CPPUNIT_ASSERT ( fchar_vec[2].bg_color == finalcut::FColor::White );
  CPPUNIT_ASSERT ( fchar_vec[3].fg_color == finalcut::FColor::White );
  CPPUNIT_ASSERT ( fchar_vec[3].bg_color == finalcut::FColor::Cyan );
  CPPUNIT_ASSERT ( fchar_vec[4].fg_color == finalcut::FColor::Cyan );
  CPPUNIT_ASSERT ( fchar_vec[4].bg_color == finalcut::FColor::White );
  CPPUNIT_ASSERT ( fchar_vec[5].fg_color == finalcut::FColor::Black );
  CPPUNIT_ASSERT ( fchar_vec[5].bg_color == finalcut::FColor::White );
  CPPUNIT_ASSERT ( fchar_vec[0].attr.byte[0] == 0 );
  CPPUNIT_ASSERT ( fchar_vec[0].attr.byte[1] == 0 );
  CPPUNIT_ASSERT ( fchar_vec[1].attr.byte[0] == 0 );
  CPPUNIT_ASSERT ( fchar_vec[1].attr.byte[1] == 0 );
  CPPUNIT_ASSERT ( fchar_vec[2].attr.byte[0] != 0 );
  CPPUNIT_ASSERT ( fchar_vec[2].attr.byte[1] == 0 );
  CPPUNIT_ASSERT ( fchar_vec[3].attr.byte[0] == 0 );
  CPPUNIT_ASSERT ( fchar_vec[3].attr.byte[1] == 0 );
  CPPUNIT_ASSERT ( fchar_vec[4].attr.byte[0] != 0 );
  CPPUNIT_ASSERT ( fchar_vec[4].attr.byte[1] == 0 );
  CPPUNIT_ASSERT ( fchar_vec[5].attr.byte[0] == 0 );
  CPPUNIT_ASSERT ( fchar_vec[5].attr.byte[1] != 0 );
}

//----------------------------------------------------------------------
void FVTermBufferTest::combiningCharacterTest()
{
  auto& fterm_data = finalcut::FTermData::getInstance();
  fterm_data.setTermEncoding (finalcut::Encoding::UTF8);
  finalcut::FVTermBuffer vterm_buf{};
  // Skip leading zero-width characters
  std::wstring combining = L"\U00000323\U00000300\U0000ff2f\n";  // [] [] Ｏ [NL]
  vterm_buf.print(combining);
  CPPUNIT_ASSERT ( ! vterm_buf.isEmpty() );
  CPPUNIT_ASSERT ( combining.length() == 4 );
  CPPUNIT_ASSERT ( wcwidth(combining[0]) == 0 );
  CPPUNIT_ASSERT ( wcwidth(combining[1]) == 0 );
  CPPUNIT_ASSERT ( wcwidth(combining[2]) == 2 );
  CPPUNIT_ASSERT ( wcwidth(combining[3]) == -1 );
  CPPUNIT_ASSERT ( vterm_buf.getLength() == 2 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer().size() == 2 );
  CPPUNIT_ASSERT ( vterm_buf.begin() != vterm_buf.end() );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[0] == L'Ｏ' );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[2] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[3] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[4] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.front().attr.bit.char_width == 2 );
  CPPUNIT_ASSERT ( vterm_buf.back().ch[0] == L'\012' );
  CPPUNIT_ASSERT ( vterm_buf.back().ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.back().ch[2] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.back().ch[3] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.back().ch[4] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.back().attr.bit.char_width == 0 );
  CPPUNIT_ASSERT ( vterm_buf.toString() != combining );
  CPPUNIT_ASSERT ( vterm_buf.toString() == L"Ｏ\012" );

  // Characters with separate and with combined diacritical marks
  combining = L"u\U00000300=\U000000f9";  // u ` = ù
  vterm_buf.clear();
  vterm_buf.print(combining);
  CPPUNIT_ASSERT ( ! vterm_buf.isEmpty() );
  CPPUNIT_ASSERT ( combining.length() == 4 );
  CPPUNIT_ASSERT ( wcwidth(combining[0]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[1]) == 0 );
  CPPUNIT_ASSERT ( wcwidth(combining[2]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[3]) == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getLength() == 3 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer().size() == 3 );
  CPPUNIT_ASSERT ( vterm_buf.begin() != vterm_buf.end() );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[0] ==  L'u' );
  CPPUNIT_ASSERT ( int(vterm_buf.front().ch[1]) == 0x300 );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[2] ==  L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].ch[0] == L'u' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].ch[1] == L'\U00000300' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].ch[2] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].ch[0] == L'=' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].ch[0] == L'ù' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.back().ch[0] ==  L'ù' );
  CPPUNIT_ASSERT ( vterm_buf.back().ch[1] ==  L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.toString() == combining );
  CPPUNIT_ASSERT ( vterm_buf.toString() == L"ù=ù" );

  combining = L"o\U0000031b\U00000323=\U00001ee3";  // o ‍̛   ‍̣ = ợ
  vterm_buf.clear();
  vterm_buf.print(combining);
  CPPUNIT_ASSERT ( combining.length() == 5 );
  CPPUNIT_ASSERT ( wcwidth(combining[0]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[1]) == 0 );
  CPPUNIT_ASSERT ( wcwidth(combining[2]) == 0 );
  CPPUNIT_ASSERT ( wcwidth(combining[3]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[4]) == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getLength() == 3 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer().size() == 3 );
  CPPUNIT_ASSERT ( vterm_buf.begin() != vterm_buf.end() );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[0] == L'o' );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[1] == L'\U0000031b' );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[2] == L'\U00000323' );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[3] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].ch[0] == L'o' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].ch[1] == L'\U0000031b' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].ch[2] == L'\U00000323' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].ch[3] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].ch[0] == L'=' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].ch[0] == L'ợ' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.back().ch[0] == L'ợ' );
  CPPUNIT_ASSERT ( vterm_buf.back().ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.toString() == combining );
  CPPUNIT_ASSERT ( vterm_buf.toString() == L"ợ=ợ" );

  // Too many combination characters
  combining = L"v" \
              L"\U00000300\U0000032e\U00000368" \
              L"\U00000364\U00000348\U0000034b";  // v ̀  ̮  ͨ  ͤ  ͈  ͋
  vterm_buf.clear();
  vterm_buf << combining;
  CPPUNIT_ASSERT ( combining.length() == 7 );
  CPPUNIT_ASSERT ( wcwidth(combining[0]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[1]) == 0 );
  CPPUNIT_ASSERT ( wcwidth(combining[2]) == 0 );
  CPPUNIT_ASSERT ( wcwidth(combining[3]) == 0 );
  CPPUNIT_ASSERT ( wcwidth(combining[4]) == 0 );
  CPPUNIT_ASSERT ( wcwidth(combining[5]) == 0 );
  CPPUNIT_ASSERT ( wcwidth(combining[6]) == 0 );
  CPPUNIT_ASSERT ( vterm_buf.getLength() == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer().size() == 1 );
  CPPUNIT_ASSERT ( vterm_buf.begin() != vterm_buf.end() );
  CPPUNIT_ASSERT ( vterm_buf.front() == vterm_buf.back() );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[0] == L'v' );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[1] == L'\U00000300' );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[2] == L'\U0000032e' );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[3] == L'\U00000368' );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[4] == L'\U00000364' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].ch[0] == L'v' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].ch[1] == L'\U00000300' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].ch[2] == L'\U0000032e' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].ch[3] == L'\U00000368' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].ch[4] == L'\U00000364' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.back().ch[0] == L'v' );
  CPPUNIT_ASSERT ( vterm_buf.back().ch[1] == L'\U00000300' );
  CPPUNIT_ASSERT ( vterm_buf.back().ch[2] == L'\U0000032e' );
  CPPUNIT_ASSERT ( vterm_buf.back().ch[3] == L'\U00000368' );
  CPPUNIT_ASSERT ( vterm_buf.back().ch[4] == L'\U00000364' );
  CPPUNIT_ASSERT ( vterm_buf.toString() != combining );
  CPPUNIT_ASSERT ( vterm_buf.toString() == L"v̮̀ͨͤ" );

  // Ignore trailing uncombined zero-width characters
  combining = L"a\t\U00000300\U00000323";  // a [Tab] [] []
  vterm_buf.clear();
  vterm_buf.print(combining);
  CPPUNIT_ASSERT ( combining.length() == 4 );
  CPPUNIT_ASSERT ( wcwidth(combining[0]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[1]) == -1 );
  CPPUNIT_ASSERT ( wcwidth(combining[2]) == 0 );
  CPPUNIT_ASSERT ( wcwidth(combining[3]) == 0 );
  CPPUNIT_ASSERT ( vterm_buf.getLength() == 2 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer().size() == 2 );
  CPPUNIT_ASSERT ( vterm_buf.begin() != vterm_buf.end() );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[0] ==  L'a' );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[1] ==  L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].ch[0] == L'a' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].ch[0] == L'\t' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].attr.bit.char_width == 0 );
  CPPUNIT_ASSERT ( vterm_buf.back().ch[0] == L'\t' );
  CPPUNIT_ASSERT ( vterm_buf.back().ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.toString() != combining );
  CPPUNIT_ASSERT ( vterm_buf.toString() == L"a\t" );

  // Thai (ISO 10646-1 - UCS implementation level 2)
  combining = L"๏ แผ่นดินฮั่นเสื่อมโทรมแสนสังเวช";
  vterm_buf.clear();
  vterm_buf.print(combining);
  CPPUNIT_ASSERT ( combining.length() == 32 );
  CPPUNIT_ASSERT ( wcwidth(combining[0]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[1]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[2]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[3]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[4]) == 0 );
  CPPUNIT_ASSERT ( wcwidth(combining[5]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[6]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[7]) == 0 );
  CPPUNIT_ASSERT ( wcwidth(combining[8]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[9]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[10]) == 0 );
  CPPUNIT_ASSERT ( wcwidth(combining[11]) == 0 );
  CPPUNIT_ASSERT ( wcwidth(combining[12]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[13]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[14]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[15]) == 0 );
  CPPUNIT_ASSERT ( wcwidth(combining[16]) == 0 );
  CPPUNIT_ASSERT ( wcwidth(combining[17]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[18]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[19]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[20]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[21]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[22]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[23]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[24]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[25]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[26]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[27]) == 0 );
  CPPUNIT_ASSERT ( wcwidth(combining[28]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[29]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[30]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[31]) == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getLength() == 25 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer().size() == 25 );
  CPPUNIT_ASSERT ( vterm_buf.begin() != vterm_buf.end() );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[0] ==  L'๏' );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[1] ==  L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].ch[0] == L'\U00000e4f' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].ch[0] == L' ' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].ch[0] == L'\U00000e41' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[3].ch[0] == L'\U00000e1c' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[3].ch[1] == L'\U00000e48' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[3].ch[2] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[4].ch[0] == L'\U00000e19' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[4].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[5].ch[0] == L'\U00000e14' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[5].ch[1] == L'\U00000e34' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[5].ch[2] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[6].ch[0] == L'\U00000e19' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[6].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[7].ch[0] == L'\U00000e2e' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[7].ch[1] == L'\U00000e31' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[7].ch[2] == L'\U00000e48' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[7].ch[3] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[8].ch[0] == L'\U00000e19' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[8].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[9].ch[0] == L'\U00000e40' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[9].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[10].ch[0] == L'\U00000e2a' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[10].ch[1] == L'\U00000e37' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[10].ch[2] == L'\U00000e48' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[10].ch[3] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[11].ch[0] == L'\U00000e2d' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[11].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[12].ch[0] == L'\U00000e21' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[12].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[13].ch[0] == L'\U00000e42' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[13].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[14].ch[0] == L'\U00000e17' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[14].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[15].ch[0] == L'\U00000e23' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[15].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[16].ch[0] == L'\U00000e21' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[16].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[17].ch[0] == L'\U00000e41' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[17].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[18].ch[0] == L'\U00000e2a' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[18].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[19].ch[0] == L'\U00000e19' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[19].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[20].ch[0] == L'\U00000e2a' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[20].ch[1] == L'\U00000e31' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[20].ch[2] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[21].ch[0] == L'\U00000e07' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[21].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[22].ch[0] == L'\U00000e40' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[22].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[23].ch[0] == L'\U00000e27' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[23].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[24].ch[0] == L'\U00000e0a' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[24].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[3].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[4].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[5].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[6].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[7].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[8].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[9].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[10].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[11].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[12].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[13].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[14].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[15].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[16].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[17].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[18].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[19].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[20].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[21].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[22].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[23].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[24].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.back().ch[0] == L'ช' );
  CPPUNIT_ASSERT ( vterm_buf.back().ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.toString() == combining );
  CPPUNIT_ASSERT ( vterm_buf.toString() == L"๏ แผ่นดินฮั่นเสื่อมโทรมแสนสังเวช" );

  // Devanagari script (ISO 10646-1 - UCS implementation level 2)
  combining = L"पन्ह पन्ह त्र र्च कृकृ ड्ड न्ह";
  vterm_buf.clear();
  vterm_buf.print(combining);
  CPPUNIT_ASSERT ( combining.length() == 30 );
  CPPUNIT_ASSERT ( wcwidth(combining[0]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[1]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[2]) == 0 );
  CPPUNIT_ASSERT ( wcwidth(combining[3]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[4]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[5]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[6]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[7]) == 0 );
  CPPUNIT_ASSERT ( wcwidth(combining[8]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[9]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[10]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[11]) == 0 );
  CPPUNIT_ASSERT ( wcwidth(combining[12]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[13]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[14]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[15]) == 0 );
  CPPUNIT_ASSERT ( wcwidth(combining[16]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[17]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[18]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[19]) == 0 );
  CPPUNIT_ASSERT ( wcwidth(combining[20]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[21]) == 0 );
  CPPUNIT_ASSERT ( wcwidth(combining[22]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[23]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[24]) == 0 );
  CPPUNIT_ASSERT ( wcwidth(combining[25]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[26]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[27]) == 1 );
  CPPUNIT_ASSERT ( wcwidth(combining[28]) == 0 );
  CPPUNIT_ASSERT ( wcwidth(combining[29]) == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getLength() == 22 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer().size() == 22 );
  CPPUNIT_ASSERT ( vterm_buf.begin() != vterm_buf.end() );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[0] ==  L'\U0000092a' );
  CPPUNIT_ASSERT ( vterm_buf.front().ch[1] ==  L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].ch[0] == L'\U0000092a' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].ch[0] == L'\U00000928' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].ch[1] == L'\U0000094d' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].ch[2] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].ch[0] == L'\U00000939' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[3].ch[0] == L' ' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[3].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[4].ch[0] == L'\U0000092a' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[4].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[5].ch[0] == L'\U00000928' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[5].ch[1] == L'\U0000094d' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[5].ch[2] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[6].ch[0] == L'\U00000939' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[6].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[7].ch[0] == L' ' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[7].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[8].ch[0] == L'\U00000924' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[8].ch[1] == L'\U0000094d' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[8].ch[2] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[9].ch[0] == L'\U00000930' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[9].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[10].ch[0] == L' ' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[10].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[11].ch[0] == L'\U00000930' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[11].ch[1] == L'\U0000094d' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[11].ch[2] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[12].ch[0] == L'\U0000091a' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[12].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[13].ch[0] == L' ' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[13].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[14].ch[0] == L'\U00000915' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[14].ch[1] == L'\U00000943' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[14].ch[2] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[15].ch[0] == L'\U00000915' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[15].ch[1] == L'\U00000943' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[15].ch[2] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[16].ch[0] == L' ' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[16].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[17].ch[0] == L'\U00000921' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[17].ch[1] == L'\U0000094d' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[17].ch[2] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[18].ch[0] == L'\U00000921' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[18].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[19].ch[0] == L' ' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[19].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[20].ch[0] == L'\U00000928' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[20].ch[1] == L'\U0000094d' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[20].ch[2] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[21].ch[0] == L'\U00000939' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[21].ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[0].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[1].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[2].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[3].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[4].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[5].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[6].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[7].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[8].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[9].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[10].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[11].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[12].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[13].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[14].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[15].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[16].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[17].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[18].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[19].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[20].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.getBuffer()[21].attr.bit.char_width == 1 );
  CPPUNIT_ASSERT ( vterm_buf.back().ch[0] == L'\U00000939' );
  CPPUNIT_ASSERT ( vterm_buf.back().ch[1] == L'\0' );
  CPPUNIT_ASSERT ( vterm_buf.toString() == combining );
  CPPUNIT_ASSERT ( vterm_buf.toString() == L"पन्ह पन्ह त्र र्च कृकृ ड्ड न्ह" );
}


// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FVTermBufferTest);

// The general unit test main part
#include <main-test.inc>
