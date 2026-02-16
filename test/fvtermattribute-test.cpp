/***********************************************************************
* fvtermattribute-test.cpp - FVTermAttribute unit tests                *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2021-2026 Markus Gans                                      *
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
// class FVTermAttributeTest
//----------------------------------------------------------------------

class FVTermAttributeTest : public CPPUNIT_NS::TestFixture
{
  public:
    FVTermAttributeTest() = default;

  protected:
    void classNameTest();
    void noArgumentTest();
    void afterInitTest();
    void setAttributeTest();
    void printTest();

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FVTermAttributeTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (noArgumentTest);
    CPPUNIT_TEST (afterInitTest);
    CPPUNIT_TEST (setAttributeTest);
    CPPUNIT_TEST (printTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};


//----------------------------------------------------------------------
void FVTermAttributeTest::classNameTest()
{
  const finalcut::FVTermAttribute a;
  const finalcut::FString& classname = a.getClassName();
  CPPUNIT_ASSERT ( classname == "FVTermAttribute" );
}

//----------------------------------------------------------------------
void FVTermAttributeTest::noArgumentTest()
{
  const finalcut::FVTermAttribute attribute{};
  CPPUNIT_ASSERT ( attribute.getTermForegroundColor() == finalcut::FColor(0) );
  CPPUNIT_ASSERT ( attribute.getTermBackgroundColor() == finalcut::FColor(0) );
  finalcut::FUnicode empty{L'\0', L'\0', L'\0', L'\0', L'\0'};
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );
}

//----------------------------------------------------------------------
void FVTermAttributeTest::afterInitTest()
{
  finalcut::FVTermAttribute attribute{};
  attribute.getAttribute().color.setFgColor(finalcut::FColor(3));
  attribute.getAttribute().color.setBgColor(finalcut::FColor(200));
  attribute.getAttribute().ch = {L'🗑', L'\0', L'\0', L'\0', L'\0'};
  attribute.getAttribute().attr.byte[0] = uInt8(0xff);
  attribute.getAttribute().attr.byte[1] = uInt8(0xff);
  attribute.getAttribute().attr.byte[2] = uInt8(0xff);
  attribute.getAttribute().attr.byte[3] = uInt8(0xff);
  attribute.initAttribute();
  CPPUNIT_ASSERT ( attribute.getTermForegroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getTermBackgroundColor() == finalcut::FColor::Default );
  finalcut::FUnicode empty{L'\0', L'\0', L'\0', L'\0', L'\0'};
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );
}

//----------------------------------------------------------------------
void FVTermAttributeTest::setAttributeTest()
{
  finalcut::FVTermAttribute attribute{};
  attribute.initAttribute();
  // Set color (red text on white background)
  attribute.setColor(finalcut::FColor::Red, finalcut::FColor::White );
  CPPUNIT_ASSERT ( attribute.getTermForegroundColor() == finalcut::FColor::Red );
  CPPUNIT_ASSERT ( attribute.getTermBackgroundColor() == finalcut::FColor::White );
  finalcut::FUnicode empty{L'\0', L'\0', L'\0', L'\0', L'\0'};
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Red );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::White );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );
  // Set color (light red text on black background)
  const finalcut::FColorPair color_pair (finalcut::FColor::LightRed, finalcut::FColor::Black);
  attribute.setColor(color_pair);
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::LightRed );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Black );

  // Data that will never be modified by this class
  attribute.getAttribute().attr.bit.printed = true;
  attribute.getAttribute().attr.bit.fullwidth_padding = true;
  attribute.getAttribute().attr.bit.char_width = 2;
  // Set Normal
  attribute.setNormal();
  CPPUNIT_ASSERT ( attribute.getTermForegroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getTermBackgroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] != uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.no_changes == false );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.printed == true );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.fullwidth_padding == true );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.char_width == 2 );
  attribute.getAttribute().attr.byte[2] = uInt8(0);

  // Bold
  attribute.setBold();
  CPPUNIT_ASSERT ( attribute.isBold() );
  attribute.unsetBold();
  CPPUNIT_ASSERT ( ! attribute.isBold() );
  attribute.setBold(true);
  CPPUNIT_ASSERT ( attribute.isBold() );
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] != uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.bold == true );
  attribute.setBold(false);
  CPPUNIT_ASSERT ( ! attribute.isBold() );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] == uInt8(0) );

  // Dim
  attribute.setDim();
  CPPUNIT_ASSERT ( attribute.isDim() );
  attribute.unsetDim();
  CPPUNIT_ASSERT ( ! attribute.isDim() );
  attribute.setDim(true);
  CPPUNIT_ASSERT ( attribute.isDim() );
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] != uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.dim == true );
  attribute.setDim(false);
  CPPUNIT_ASSERT ( ! attribute.isDim() );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] == uInt8(0) );

  // Italic
  attribute.setItalic();
  CPPUNIT_ASSERT ( attribute.isItalic() );
  attribute.unsetItalic();
  CPPUNIT_ASSERT ( ! attribute.isItalic() );
  attribute.setItalic(true);
  CPPUNIT_ASSERT ( attribute.isItalic() );
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] != uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.italic == true );
  attribute.setItalic(false);
  CPPUNIT_ASSERT ( ! attribute.isItalic() );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] == uInt8(0) );

  // Underline
  attribute.setUnderline();
  CPPUNIT_ASSERT ( attribute.isUnderline() );
  attribute.unsetUnderline();
  CPPUNIT_ASSERT ( ! attribute.isUnderline() );
  attribute.setUnderline(true);
  CPPUNIT_ASSERT ( attribute.isUnderline() );
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] != uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.underline == true );
  attribute.setUnderline(false);
  CPPUNIT_ASSERT ( ! attribute.isUnderline() );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] == uInt8(0) );

  // Blink
  attribute.setBlink();
  CPPUNIT_ASSERT ( attribute.isBlink() );
  attribute.unsetBlink();
  CPPUNIT_ASSERT ( ! attribute.isBlink() );
  attribute.setBlink(true);
  CPPUNIT_ASSERT ( attribute.isBlink() );
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] != uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.blink == true );
  attribute.setBlink(false);
  CPPUNIT_ASSERT ( ! attribute.isBlink() );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] == uInt8(0) );

  // Reverse
  attribute.setReverse();
  CPPUNIT_ASSERT ( attribute.isReverse() );
  attribute.unsetReverse();
  CPPUNIT_ASSERT ( ! attribute.isReverse() );
  attribute.setReverse(true);
  CPPUNIT_ASSERT ( attribute.isReverse() );
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] != uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.reverse == true );
  attribute.setReverse(false);
  CPPUNIT_ASSERT ( ! attribute.isReverse() );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] == uInt8(0) );

  // Standout
  attribute.setStandout();
  CPPUNIT_ASSERT ( attribute.isStandout() );
  attribute.unsetStandout();
  CPPUNIT_ASSERT ( ! attribute.isStandout() );
  attribute.setStandout(true);
  CPPUNIT_ASSERT ( attribute.isStandout() );
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] != uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.standout == true );
  attribute.setStandout(false);
  CPPUNIT_ASSERT ( ! attribute.isStandout() );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] == uInt8(0) );

  // Invisible
  attribute.setInvisible();
  CPPUNIT_ASSERT ( attribute.isInvisible() );
  attribute.unsetInvisible();
  CPPUNIT_ASSERT ( ! attribute.isInvisible() );
  attribute.setInvisible(true);
  CPPUNIT_ASSERT ( attribute.isInvisible() );
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] != uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.invisible == true );
  attribute.setInvisible(false);
  CPPUNIT_ASSERT ( ! attribute.isInvisible() );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] == uInt8(0) );

  // Protected
  attribute.setProtected();
  CPPUNIT_ASSERT ( attribute.isProtected() );
  attribute.unsetProtected();
  CPPUNIT_ASSERT ( ! attribute.isProtected() );
  attribute.setProtected(true);
  CPPUNIT_ASSERT ( attribute.isProtected() );
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] != uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.protect == true );
  attribute.setProtected(false);
  CPPUNIT_ASSERT ( ! attribute.isProtected() );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] == uInt8(0) );

  // Crossed out
  attribute.setCrossedOut();
  CPPUNIT_ASSERT ( attribute.isCrossedOut() );
  attribute.unsetCrossedOut();
  CPPUNIT_ASSERT ( ! attribute.isCrossedOut() );
  attribute.setCrossedOut(true);
  CPPUNIT_ASSERT ( attribute.isCrossedOut() );
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] != uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.crossed_out == true );
  attribute.setCrossedOut(false);
  CPPUNIT_ASSERT ( ! attribute.isCrossedOut() );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] == uInt8(0) );

  // Double underline
  attribute.setDoubleUnderline();
  CPPUNIT_ASSERT ( attribute.isDoubleUnderline() );
  attribute.unsetDoubleUnderline();
  CPPUNIT_ASSERT ( ! attribute.isDoubleUnderline() );
  attribute.setDoubleUnderline(true);
  CPPUNIT_ASSERT ( attribute.isDoubleUnderline() );
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] != uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.dbl_underline == true );
  attribute.setDoubleUnderline(false);
  CPPUNIT_ASSERT ( ! attribute.isDoubleUnderline() );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] == uInt8(0) );

  // Alternate character set
  attribute.setAltCharset();
  CPPUNIT_ASSERT ( attribute.isAltCharset() );
  attribute.unsetAltCharset();
  CPPUNIT_ASSERT ( ! attribute.isAltCharset() );
  attribute.setAltCharset(true);
  CPPUNIT_ASSERT ( attribute.isAltCharset() );
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] != uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.alt_charset == true );
  attribute.setAltCharset(false);
  CPPUNIT_ASSERT ( ! attribute.isAltCharset() );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] == uInt8(0) );

  // PC character set
  attribute.setPCcharset();
  CPPUNIT_ASSERT ( attribute.isPCcharset() );
  attribute.unsetPCcharset();
  CPPUNIT_ASSERT ( ! attribute.isPCcharset() );
  attribute.setPCcharset(true);
  CPPUNIT_ASSERT ( attribute.isPCcharset() );
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] != uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.pc_charset == true );
  attribute.setPCcharset(false);
  CPPUNIT_ASSERT ( ! attribute.isPCcharset() );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] == uInt8(0) );

  // Transparent
  attribute.setTransparent();
  CPPUNIT_ASSERT ( attribute.isTransparent() );
  attribute.unsetTransparent();
  CPPUNIT_ASSERT ( ! attribute.isTransparent() );
  attribute.setTransparent(true);
  CPPUNIT_ASSERT ( attribute.isTransparent() );
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] != uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.transparent == true );
  attribute.setTransparent(false);
  CPPUNIT_ASSERT ( ! attribute.isTransparent() );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] == uInt8(0) );

  // Color overlay
  attribute.setColorOverlay();
  CPPUNIT_ASSERT ( attribute.isColorOverlay() );
  attribute.unsetColorOverlay();
  CPPUNIT_ASSERT ( ! attribute.isColorOverlay() );
  attribute.setColorOverlay(true);
  CPPUNIT_ASSERT ( attribute.isColorOverlay() );
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] != uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.color_overlay == true );
  attribute.setColorOverlay(false);
  CPPUNIT_ASSERT ( ! attribute.isColorOverlay() );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] == uInt8(0) );

  // Inherit background color
  attribute.setInheritBackground();
  CPPUNIT_ASSERT ( attribute.isInheritBackground() );
  attribute.unsetInheritBackground();
  CPPUNIT_ASSERT ( ! attribute.isInheritBackground() );
  attribute.setInheritBackground(true);
  CPPUNIT_ASSERT ( attribute.isInheritBackground() );
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] != uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.inherit_background == true );
  attribute.setInheritBackground(false);
  CPPUNIT_ASSERT ( ! attribute.isInheritBackground() );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] == uInt8(0) );
}

//----------------------------------------------------------------------
void FVTermAttributeTest::printTest()
{
  finalcut::FVTermAttribute attribute{};
  attribute.initAttribute();

  // Color
  finalcut::FColorPair c1{finalcut::FColor::Yellow, finalcut::FColor::Blue };
  attribute.print(c1);
  CPPUNIT_ASSERT ( attribute.getTermForegroundColor() == finalcut::FColor::Yellow );
  CPPUNIT_ASSERT ( attribute.getTermBackgroundColor() == finalcut::FColor::Blue );
  finalcut::FUnicode empty{L'\0', L'\0', L'\0', L'\0', L'\0'};
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Yellow );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Blue );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );

  finalcut::FColorPair c2{finalcut::FColor::Grey0, finalcut::FColor::DarkSeaGreen7 };
  attribute.print(c2);
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Grey0 );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::DarkSeaGreen7 );

  attribute.setNormal();
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Default );

  // Style
  finalcut::FStyle s1{finalcut::Style::Bold + finalcut::Style::Dim};
  attribute.print(s1);
  CPPUNIT_ASSERT ( attribute.getTermForegroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getTermBackgroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] != uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.bold == true );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.dim == true );

  finalcut::FStyle s2{finalcut::Style::Italic + finalcut::Style::DoubleUnderline};
  attribute.print(s2);
  CPPUNIT_ASSERT ( attribute.getTermForegroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getTermBackgroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] != uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] != uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.bold == true );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.dim == true );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.italic == true );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.dbl_underline == true );

  attribute.print(finalcut::FStyle(finalcut::Style::None));
  CPPUNIT_ASSERT ( attribute.getTermForegroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getTermBackgroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.bold == false );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.dim == false );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.italic == false );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.dbl_underline == false );

  attribute.setNormal();
  finalcut::FStyle s3{ finalcut::Style::Underline | finalcut::Style::Blink
                     | finalcut::Style::Reverse | finalcut::Style::Transparent };
  attribute.print(s3);
  CPPUNIT_ASSERT ( attribute.getTermForegroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getTermBackgroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] != uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] != uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.underline == true );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.blink == true );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.reverse == true );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.transparent == true );

  attribute.setNormal();
  finalcut::FStyle s4{ finalcut::Style::Standout | finalcut::Style::Invisible
                     | finalcut::Style::CrossedOut | finalcut::Style::ColorOverlay };
  attribute.print(s4);
  CPPUNIT_ASSERT ( attribute.getTermForegroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getTermBackgroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] != uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] != uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.standout == true );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.invisible == true );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.crossed_out == true );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.color_overlay == true );

  attribute.setNormal();
  finalcut::FStyle s5{ finalcut::Style::Protected | finalcut::Style::InheritBackground};
  attribute.print(s5);
  CPPUNIT_ASSERT ( attribute.getTermForegroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getTermBackgroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().ch == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().encoded_char == empty );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getFgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().color.getBgColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[0] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[1] != uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[2] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.byte[3] == uInt8(0) );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.protect == true );
  CPPUNIT_ASSERT ( attribute.getAttribute().attr.bit.inherit_background == true );

  attribute.setNormal();
  CPPUNIT_ASSERT ( ! attribute.isBold() );
  CPPUNIT_ASSERT ( ! attribute.isDim() );
  CPPUNIT_ASSERT ( ! attribute.isItalic() );
  CPPUNIT_ASSERT ( ! attribute.isUnderline() );
  CPPUNIT_ASSERT ( ! attribute.isBlink() );
  CPPUNIT_ASSERT ( ! attribute.isReverse() );
  CPPUNIT_ASSERT ( ! attribute.isStandout() );
  CPPUNIT_ASSERT ( ! attribute.isInvisible() );
  CPPUNIT_ASSERT ( ! attribute.isProtected() );
  CPPUNIT_ASSERT ( ! attribute.isCrossedOut() );
  CPPUNIT_ASSERT ( ! attribute.isDoubleUnderline() );
  CPPUNIT_ASSERT ( ! attribute.isAltCharset() );
  CPPUNIT_ASSERT ( ! attribute.isPCcharset() );
  CPPUNIT_ASSERT ( ! attribute.isTransparent() );
  CPPUNIT_ASSERT ( ! attribute.isColorOverlay() );
  CPPUNIT_ASSERT ( ! attribute.isInheritBackground() );

  attribute.print(finalcut::FStyle(finalcut::Style::Bold));
  CPPUNIT_ASSERT ( attribute.isBold() );
  CPPUNIT_ASSERT ( ! attribute.isDim() );
  CPPUNIT_ASSERT ( ! attribute.isItalic() );
  CPPUNIT_ASSERT ( ! attribute.isUnderline() );
  CPPUNIT_ASSERT ( ! attribute.isBlink() );
  CPPUNIT_ASSERT ( ! attribute.isReverse() );
  CPPUNIT_ASSERT ( ! attribute.isStandout() );
  CPPUNIT_ASSERT ( ! attribute.isInvisible() );
  CPPUNIT_ASSERT ( ! attribute.isProtected() );
  CPPUNIT_ASSERT ( ! attribute.isCrossedOut() );
  CPPUNIT_ASSERT ( ! attribute.isDoubleUnderline() );
  CPPUNIT_ASSERT ( ! attribute.isAltCharset() );
  CPPUNIT_ASSERT ( ! attribute.isPCcharset() );
  CPPUNIT_ASSERT ( ! attribute.isTransparent() );
  CPPUNIT_ASSERT ( ! attribute.isColorOverlay() );
  CPPUNIT_ASSERT ( ! attribute.isInheritBackground() );

  attribute.print(finalcut::FStyle(finalcut::Style::Dim));
  CPPUNIT_ASSERT ( attribute.isBold() );
  CPPUNIT_ASSERT ( attribute.isDim() );
  CPPUNIT_ASSERT ( ! attribute.isItalic() );
  CPPUNIT_ASSERT ( ! attribute.isUnderline() );
  CPPUNIT_ASSERT ( ! attribute.isBlink() );
  CPPUNIT_ASSERT ( ! attribute.isReverse() );
  CPPUNIT_ASSERT ( ! attribute.isStandout() );
  CPPUNIT_ASSERT ( ! attribute.isInvisible() );
  CPPUNIT_ASSERT ( ! attribute.isProtected() );
  CPPUNIT_ASSERT ( ! attribute.isCrossedOut() );
  CPPUNIT_ASSERT ( ! attribute.isDoubleUnderline() );
  CPPUNIT_ASSERT ( ! attribute.isAltCharset() );
  CPPUNIT_ASSERT ( ! attribute.isPCcharset() );
  CPPUNIT_ASSERT ( ! attribute.isTransparent() );
  CPPUNIT_ASSERT ( ! attribute.isColorOverlay() );
  CPPUNIT_ASSERT ( ! attribute.isInheritBackground() );

  attribute.print(finalcut::FStyle(finalcut::Style::Italic));
  CPPUNIT_ASSERT ( attribute.isBold() );
  CPPUNIT_ASSERT ( attribute.isDim() );
  CPPUNIT_ASSERT ( attribute.isItalic() );
  CPPUNIT_ASSERT ( ! attribute.isUnderline() );
  CPPUNIT_ASSERT ( ! attribute.isBlink() );
  CPPUNIT_ASSERT ( ! attribute.isReverse() );
  CPPUNIT_ASSERT ( ! attribute.isStandout() );
  CPPUNIT_ASSERT ( ! attribute.isInvisible() );
  CPPUNIT_ASSERT ( ! attribute.isProtected() );
  CPPUNIT_ASSERT ( ! attribute.isCrossedOut() );
  CPPUNIT_ASSERT ( ! attribute.isDoubleUnderline() );
  CPPUNIT_ASSERT ( ! attribute.isAltCharset() );
  CPPUNIT_ASSERT ( ! attribute.isPCcharset() );
  CPPUNIT_ASSERT ( ! attribute.isTransparent() );
  CPPUNIT_ASSERT ( ! attribute.isColorOverlay() );
  CPPUNIT_ASSERT ( ! attribute.isInheritBackground() );

  attribute.print(finalcut::FStyle(finalcut::Style::Underline));
  CPPUNIT_ASSERT ( attribute.isBold() );
  CPPUNIT_ASSERT ( attribute.isDim() );
  CPPUNIT_ASSERT ( attribute.isItalic() );
  CPPUNIT_ASSERT ( attribute.isUnderline() );
  CPPUNIT_ASSERT ( ! attribute.isBlink() );
  CPPUNIT_ASSERT ( ! attribute.isReverse() );
  CPPUNIT_ASSERT ( ! attribute.isStandout() );
  CPPUNIT_ASSERT ( ! attribute.isInvisible() );
  CPPUNIT_ASSERT ( ! attribute.isProtected() );
  CPPUNIT_ASSERT ( ! attribute.isCrossedOut() );
  CPPUNIT_ASSERT ( ! attribute.isDoubleUnderline() );
  CPPUNIT_ASSERT ( ! attribute.isAltCharset() );
  CPPUNIT_ASSERT ( ! attribute.isPCcharset() );
  CPPUNIT_ASSERT ( ! attribute.isTransparent() );
  CPPUNIT_ASSERT ( ! attribute.isColorOverlay() );
  CPPUNIT_ASSERT ( ! attribute.isInheritBackground() );

  attribute.print(finalcut::FStyle(finalcut::Style::Blink));
  CPPUNIT_ASSERT ( attribute.isBold() );
  CPPUNIT_ASSERT ( attribute.isDim() );
  CPPUNIT_ASSERT ( attribute.isItalic() );
  CPPUNIT_ASSERT ( attribute.isUnderline() );
  CPPUNIT_ASSERT ( attribute.isBlink() );
  CPPUNIT_ASSERT ( ! attribute.isReverse() );
  CPPUNIT_ASSERT ( ! attribute.isStandout() );
  CPPUNIT_ASSERT ( ! attribute.isInvisible() );
  CPPUNIT_ASSERT ( ! attribute.isProtected() );
  CPPUNIT_ASSERT ( ! attribute.isCrossedOut() );
  CPPUNIT_ASSERT ( ! attribute.isDoubleUnderline() );
  CPPUNIT_ASSERT ( ! attribute.isAltCharset() );
  CPPUNIT_ASSERT ( ! attribute.isPCcharset() );
  CPPUNIT_ASSERT ( ! attribute.isTransparent() );
  CPPUNIT_ASSERT ( ! attribute.isColorOverlay() );
  CPPUNIT_ASSERT ( ! attribute.isInheritBackground() );

  attribute.print(finalcut::FStyle(finalcut::Style::Reverse));
  CPPUNIT_ASSERT ( attribute.isBold() );
  CPPUNIT_ASSERT ( attribute.isDim() );
  CPPUNIT_ASSERT ( attribute.isItalic() );
  CPPUNIT_ASSERT ( attribute.isUnderline() );
  CPPUNIT_ASSERT ( attribute.isBlink() );
  CPPUNIT_ASSERT ( attribute.isReverse() );
  CPPUNIT_ASSERT ( ! attribute.isStandout() );
  CPPUNIT_ASSERT ( ! attribute.isInvisible() );
  CPPUNIT_ASSERT ( ! attribute.isProtected() );
  CPPUNIT_ASSERT ( ! attribute.isCrossedOut() );
  CPPUNIT_ASSERT ( ! attribute.isDoubleUnderline() );
  CPPUNIT_ASSERT ( ! attribute.isAltCharset() );
  CPPUNIT_ASSERT ( ! attribute.isPCcharset() );
  CPPUNIT_ASSERT ( ! attribute.isTransparent() );
  CPPUNIT_ASSERT ( ! attribute.isColorOverlay() );
  CPPUNIT_ASSERT ( ! attribute.isInheritBackground() );

  attribute.print(finalcut::FStyle(finalcut::Style::Standout));
  CPPUNIT_ASSERT ( attribute.isBold() );
  CPPUNIT_ASSERT ( attribute.isDim() );
  CPPUNIT_ASSERT ( attribute.isItalic() );
  CPPUNIT_ASSERT ( attribute.isUnderline() );
  CPPUNIT_ASSERT ( attribute.isBlink() );
  CPPUNIT_ASSERT ( attribute.isReverse() );
  CPPUNIT_ASSERT ( attribute.isStandout() );
  CPPUNIT_ASSERT ( ! attribute.isInvisible() );
  CPPUNIT_ASSERT ( ! attribute.isProtected() );
  CPPUNIT_ASSERT ( ! attribute.isCrossedOut() );
  CPPUNIT_ASSERT ( ! attribute.isDoubleUnderline() );
  CPPUNIT_ASSERT ( ! attribute.isAltCharset() );
  CPPUNIT_ASSERT ( ! attribute.isPCcharset() );
  CPPUNIT_ASSERT ( ! attribute.isTransparent() );
  CPPUNIT_ASSERT ( ! attribute.isColorOverlay() );
  CPPUNIT_ASSERT ( ! attribute.isInheritBackground() );

  attribute.print(finalcut::FStyle(finalcut::Style::Invisible));
  CPPUNIT_ASSERT ( attribute.isBold() );
  CPPUNIT_ASSERT ( attribute.isDim() );
  CPPUNIT_ASSERT ( attribute.isItalic() );
  CPPUNIT_ASSERT ( attribute.isUnderline() );
  CPPUNIT_ASSERT ( attribute.isBlink() );
  CPPUNIT_ASSERT ( attribute.isReverse() );
  CPPUNIT_ASSERT ( attribute.isStandout() );
  CPPUNIT_ASSERT ( attribute.isInvisible() );
  CPPUNIT_ASSERT ( ! attribute.isProtected() );
  CPPUNIT_ASSERT ( ! attribute.isCrossedOut() );
  CPPUNIT_ASSERT ( ! attribute.isDoubleUnderline() );
  CPPUNIT_ASSERT ( ! attribute.isAltCharset() );
  CPPUNIT_ASSERT ( ! attribute.isPCcharset() );
  CPPUNIT_ASSERT ( ! attribute.isTransparent() );
  CPPUNIT_ASSERT ( ! attribute.isColorOverlay() );
  CPPUNIT_ASSERT ( ! attribute.isInheritBackground() );

  attribute.print(finalcut::FStyle(finalcut::Style::Protected));
  CPPUNIT_ASSERT ( attribute.isBold() );
  CPPUNIT_ASSERT ( attribute.isDim() );
  CPPUNIT_ASSERT ( attribute.isItalic() );
  CPPUNIT_ASSERT ( attribute.isUnderline() );
  CPPUNIT_ASSERT ( attribute.isBlink() );
  CPPUNIT_ASSERT ( attribute.isReverse() );
  CPPUNIT_ASSERT ( attribute.isStandout() );
  CPPUNIT_ASSERT ( attribute.isInvisible() );
  CPPUNIT_ASSERT ( attribute.isProtected() );
  CPPUNIT_ASSERT ( ! attribute.isCrossedOut() );
  CPPUNIT_ASSERT ( ! attribute.isDoubleUnderline() );
  CPPUNIT_ASSERT ( ! attribute.isAltCharset() );
  CPPUNIT_ASSERT ( ! attribute.isPCcharset() );
  CPPUNIT_ASSERT ( ! attribute.isTransparent() );
  CPPUNIT_ASSERT ( ! attribute.isColorOverlay() );
  CPPUNIT_ASSERT ( ! attribute.isInheritBackground() );

  attribute.print(finalcut::FStyle(finalcut::Style::CrossedOut));
  CPPUNIT_ASSERT ( attribute.isBold() );
  CPPUNIT_ASSERT ( attribute.isDim() );
  CPPUNIT_ASSERT ( attribute.isItalic() );
  CPPUNIT_ASSERT ( attribute.isUnderline() );
  CPPUNIT_ASSERT ( attribute.isBlink() );
  CPPUNIT_ASSERT ( attribute.isReverse() );
  CPPUNIT_ASSERT ( attribute.isStandout() );
  CPPUNIT_ASSERT ( attribute.isInvisible() );
  CPPUNIT_ASSERT ( attribute.isProtected() );
  CPPUNIT_ASSERT ( attribute.isCrossedOut() );
  CPPUNIT_ASSERT ( ! attribute.isDoubleUnderline() );
  CPPUNIT_ASSERT ( ! attribute.isAltCharset() );
  CPPUNIT_ASSERT ( ! attribute.isPCcharset() );
  CPPUNIT_ASSERT ( ! attribute.isTransparent() );
  CPPUNIT_ASSERT ( ! attribute.isColorOverlay() );
  CPPUNIT_ASSERT ( ! attribute.isInheritBackground() );

  attribute.print(finalcut::FStyle(finalcut::Style::DoubleUnderline));
  CPPUNIT_ASSERT ( attribute.isBold() );
  CPPUNIT_ASSERT ( attribute.isDim() );
  CPPUNIT_ASSERT ( attribute.isItalic() );
  CPPUNIT_ASSERT ( attribute.isUnderline() );
  CPPUNIT_ASSERT ( attribute.isBlink() );
  CPPUNIT_ASSERT ( attribute.isReverse() );
  CPPUNIT_ASSERT ( attribute.isStandout() );
  CPPUNIT_ASSERT ( attribute.isInvisible() );
  CPPUNIT_ASSERT ( attribute.isProtected() );
  CPPUNIT_ASSERT ( attribute.isCrossedOut() );
  CPPUNIT_ASSERT ( attribute.isDoubleUnderline() );
  CPPUNIT_ASSERT ( ! attribute.isAltCharset() );
  CPPUNIT_ASSERT ( ! attribute.isPCcharset() );
  CPPUNIT_ASSERT ( ! attribute.isTransparent() );
  CPPUNIT_ASSERT ( ! attribute.isColorOverlay() );
  CPPUNIT_ASSERT ( ! attribute.isInheritBackground() );

  attribute.print(finalcut::FStyle(finalcut::Style::Transparent));
  CPPUNIT_ASSERT ( attribute.isBold() );
  CPPUNIT_ASSERT ( attribute.isDim() );
  CPPUNIT_ASSERT ( attribute.isItalic() );
  CPPUNIT_ASSERT ( attribute.isUnderline() );
  CPPUNIT_ASSERT ( attribute.isBlink() );
  CPPUNIT_ASSERT ( attribute.isReverse() );
  CPPUNIT_ASSERT ( attribute.isStandout() );
  CPPUNIT_ASSERT ( attribute.isInvisible() );
  CPPUNIT_ASSERT ( attribute.isProtected() );
  CPPUNIT_ASSERT ( attribute.isCrossedOut() );
  CPPUNIT_ASSERT ( attribute.isDoubleUnderline() );
  CPPUNIT_ASSERT ( ! attribute.isAltCharset() );
  CPPUNIT_ASSERT ( ! attribute.isPCcharset() );
  CPPUNIT_ASSERT ( attribute.isTransparent() );
  CPPUNIT_ASSERT ( ! attribute.isColorOverlay() );
  CPPUNIT_ASSERT ( ! attribute.isInheritBackground() );

  attribute.print(finalcut::FStyle(finalcut::Style::ColorOverlay));
  CPPUNIT_ASSERT ( attribute.isBold() );
  CPPUNIT_ASSERT ( attribute.isDim() );
  CPPUNIT_ASSERT ( attribute.isItalic() );
  CPPUNIT_ASSERT ( attribute.isUnderline() );
  CPPUNIT_ASSERT ( attribute.isBlink() );
  CPPUNIT_ASSERT ( attribute.isReverse() );
  CPPUNIT_ASSERT ( attribute.isStandout() );
  CPPUNIT_ASSERT ( attribute.isInvisible() );
  CPPUNIT_ASSERT ( attribute.isProtected() );
  CPPUNIT_ASSERT ( attribute.isCrossedOut() );
  CPPUNIT_ASSERT ( attribute.isDoubleUnderline() );
  CPPUNIT_ASSERT ( ! attribute.isAltCharset() );
  CPPUNIT_ASSERT ( ! attribute.isPCcharset() );
  CPPUNIT_ASSERT ( attribute.isTransparent() );
  CPPUNIT_ASSERT ( attribute.isColorOverlay() );
  CPPUNIT_ASSERT ( ! attribute.isInheritBackground() );

  attribute.print(finalcut::FStyle(finalcut::Style::InheritBackground));
  CPPUNIT_ASSERT ( attribute.isBold() );
  CPPUNIT_ASSERT ( attribute.isDim() );
  CPPUNIT_ASSERT ( attribute.isItalic() );
  CPPUNIT_ASSERT ( attribute.isUnderline() );
  CPPUNIT_ASSERT ( attribute.isBlink() );
  CPPUNIT_ASSERT ( attribute.isReverse() );
  CPPUNIT_ASSERT ( attribute.isStandout() );
  CPPUNIT_ASSERT ( attribute.isInvisible() );
  CPPUNIT_ASSERT ( attribute.isProtected() );
  CPPUNIT_ASSERT ( attribute.isCrossedOut() );
  CPPUNIT_ASSERT ( attribute.isDoubleUnderline() );
  CPPUNIT_ASSERT ( ! attribute.isAltCharset() );
  CPPUNIT_ASSERT ( ! attribute.isPCcharset() );
  CPPUNIT_ASSERT ( attribute.isTransparent() );
  CPPUNIT_ASSERT ( attribute.isColorOverlay() );
  CPPUNIT_ASSERT ( attribute.isInheritBackground() );

  attribute.print(finalcut::FStyle(finalcut::Style::None));
  CPPUNIT_ASSERT ( ! attribute.isBold() );
  CPPUNIT_ASSERT ( ! attribute.isDim() );
  CPPUNIT_ASSERT ( ! attribute.isItalic() );
  CPPUNIT_ASSERT ( ! attribute.isUnderline() );
  CPPUNIT_ASSERT ( ! attribute.isBlink() );
  CPPUNIT_ASSERT ( ! attribute.isReverse() );
  CPPUNIT_ASSERT ( ! attribute.isStandout() );
  CPPUNIT_ASSERT ( ! attribute.isInvisible() );
  CPPUNIT_ASSERT ( ! attribute.isProtected() );
  CPPUNIT_ASSERT ( ! attribute.isCrossedOut() );
  CPPUNIT_ASSERT ( ! attribute.isDoubleUnderline() );
  CPPUNIT_ASSERT ( ! attribute.isAltCharset() );
  CPPUNIT_ASSERT ( ! attribute.isPCcharset() );
  CPPUNIT_ASSERT ( ! attribute.isTransparent() );
  CPPUNIT_ASSERT ( ! attribute.isColorOverlay() );
  CPPUNIT_ASSERT ( ! attribute.isInheritBackground() );
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FVTermAttributeTest);

// The general unit test main part
#include <main-test.inc>
