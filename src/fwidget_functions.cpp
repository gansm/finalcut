/***********************************************************************
* fwidget_functions.cpp - FWidget helper functions                     *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2019 Markus Gans                                           *
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

#include "final/fwidget.h"
#include "final/fwidgetcolors.h"

namespace finalcut
{

// FWidget non-member functions
//----------------------------------------------------------------------
FKey getHotkey (const FString& text)
{
  // Returns the hotkey character from a string
  // e.g. "E&xit" returns 'x'

  if ( text.isEmpty() )
    return 0;

  std::size_t length = text.getLength();

  for (std::size_t i{0}; i < length; i++)
  {
    try
    {
      if ( i + 1 < length && text[i] == '&' )
        return FKey(text[++i]);
    }
    catch (const std::out_of_range&)
    {
      return 0;
    }
  }
  return 0;
}

//----------------------------------------------------------------------
std::size_t getHotkeyPos (const FString& src, FString& dest)
{
  // Find hotkey position in string
  // + generate a new string without the '&'-sign

  constexpr std::size_t NOT_SET = static_cast<std::size_t>(-1);
  std::size_t hotkeypos{NOT_SET};
  std::size_t i{0};

  for (auto&& ch : src)
  {
    if ( ch == L'&' && hotkeypos == NOT_SET && src.getLength() != i + 1 )
      hotkeypos = i;
    else
      dest += ch;

    i++;
  }

  return hotkeypos;
}
//----------------------------------------------------------------------
void setHotkeyViaString (FWidget* w, const FString& text)
{
  // Set hotkey accelerator via string

  if ( ! w )
    return;

  FKey hotkey = getHotkey(text);

  if ( hotkey > 0xff00 && hotkey < 0xff5f )  // full-width character
    hotkey -= 0xfee0;

  if ( hotkey )
  {
    if ( std::isalpha(int(hotkey)) || std::isdigit(int(hotkey)) )
    {
      w->addAccelerator (FKey(std::tolower(int(hotkey))));
      w->addAccelerator (FKey(std::toupper(int(hotkey))));
      // Meta + hotkey
      w->addAccelerator (fc::Fmkey_meta + FKey(std::tolower(int(hotkey))));
    }
    else
      w->addAccelerator (hotkey);
  }
  else
    w->delAccelerator();
}

//----------------------------------------------------------------------
void drawShadow (FWidget* w)
{
  if ( w->isMonochron() && ! w->flags.trans_shadow )
    return;

  if ( (w->getEncoding() == fc::VT100 && ! w->flags.trans_shadow)
    || (w->getEncoding() == fc::ASCII && ! w->flags.trans_shadow) )
  {
    clearShadow(w);
    return;
  }

  if ( w->flags.trans_shadow )
    drawTransparentShadow (w);  // transparent shadow
  else
    drawBlockShadow (w);        // non-transparent shadow
}

//----------------------------------------------------------------------
void drawTransparentShadow (FWidget* w)
{
  // transparent shadow

  std::size_t width = w->getWidth();
  std::size_t height = w->getHeight();
  w->setTransparent();
  w->print() << FPoint(int(width) + 1, 1) << "  ";
  w->unsetTransparent();
  w->setColor (w->wcolors.shadow_bg, w->wcolors.shadow_fg);
  w->setTransShadow();

  for (std::size_t y{1}; y < height; y++)
  {
    w->print() << FPoint(int(width) + 1, int(y) + 1) << "  ";
  }

  w->unsetTransShadow();
  w->setTransparent();
  w->print() << FPoint(1, int(height) + 1) << "  ";
  w->unsetTransparent();
  w->setColor (w->wcolors.shadow_bg, w->wcolors.shadow_fg);
  w->setTransShadow();
  w->print() << FString(width, L' ');

  w->unsetTransShadow();

  if ( w->isMonochron() )
    w->setReverse(false);
}

//----------------------------------------------------------------------
void drawBlockShadow (FWidget* w)
{
  // non-transparent shadow

  if ( ! w->hasShadowCharacter() )
    return;

  std::size_t width = w->getWidth();
  std::size_t height = w->getHeight();
  w->print() << FPoint(int(width) + 1, 1);

  if ( w->isWindowWidget() )
  {
    w->setColor (w->wcolors.shadow_fg, w->wcolors.shadow_bg);
    w->setInheritBackground();  // current background color will be ignored
  }
  else if ( auto p = w->getParentWidget() )
    w->setColor (w->wcolors.shadow_fg, p->getBackgroundColor());

  w->print (fc::LowerHalfBlock);  // ▄

  if ( w->isWindowWidget() )
    w->unsetInheritBackground();

  for (std::size_t y{1}; y < height; y++)
  {
    w->print() << FPoint(int(width) + 1, int(y) + 1) << fc::FullBlock;  // █
  }

  w->print() << FPoint(2, int(height) + 1);

  if ( w->isWindowWidget() )
    w->setInheritBackground();

  w->print() << FString(width, fc::UpperHalfBlock);  // ▀

  if ( w->isWindowWidget() )
    w->unsetInheritBackground();
}

//----------------------------------------------------------------------
void clearShadow (FWidget* w)
{
  if ( w->isMonochron() )
    return;

  std::size_t width = w->getWidth();
  std::size_t height = w->getHeight();

  if ( w->isWindowWidget() )
  {
    w->setColor (w->wcolors.shadow_fg, w->wcolors.shadow_bg);
    w->setInheritBackground();  // current background color will be ignored
  }
  else if ( auto p = w->getParentWidget() )
    w->setColor (w->wcolors.shadow_fg, p->getBackgroundColor());

  if ( int(width) <= w->woffset.getX2() )
  {
    for (std::size_t y{1}; y <= height; y++)
    {
      w->print() << FPoint(int(width) + 1, int(y)) << ' ';  // clear █
    }
  }

  if ( int(height) <= w->woffset.getY2() )
  {
    w->print() << FPoint(2, int(height) + 1)
               << FString(width, L' ');  // clear ▀
  }

  if ( w->isWindowWidget() )
    w->unsetInheritBackground();
}

//----------------------------------------------------------------------
void drawFlatBorder (FWidget* w)
{
  if ( ! w->isNewFont() )
    return;

  if ( auto p = w->getParentWidget() )
    w->setColor (w->wcolors.dialog_fg, p->getBackgroundColor());
  else
    w->setColor (w->wcolors.dialog_fg, w->wcolors.dialog_bg);

  std::size_t width = w->getWidth();
  std::size_t height = w->getHeight();

  for (std::size_t y{0}; y < height; y++)
  {
    w->print() << FPoint(0, int(y) + 1);

    if ( w->double_flatline_mask.left[uLong(y)] )
      // left+right line (on left side)
      w->print (fc::NF_rev_border_line_right_and_left);
    else
      // right line (on left side)
      w->print (fc::NF_rev_border_line_right);

    w->print() << FPoint(int(width) + 1, int(y) + 1);

    if ( w->double_flatline_mask.right[y] )
      // left+right line (on right side)
      w->print (fc::NF_rev_border_line_right_and_left);
    else
      // left line (on right side)
      w->print (fc::NF_border_line_left);
  }

  w->print() << FPoint(1, 0);

  for (std::size_t x{0}; x < width; x++)
  {
    if ( w->double_flatline_mask.top[x] )
      // top+bottom line (at top)
      w->print (fc::NF_border_line_up_and_down);
    else
      // bottom line (at top)
      w->print (fc::NF_border_line_bottom);
  }

  w->print() << FPoint(1, int(height) + 1);

  for (std::size_t x{0}; x < width; x++)
  {
    if ( w->double_flatline_mask.bottom[x] )
      // top+bottom line (at bottom)
      w->print (fc::NF_border_line_up_and_down);
    else
      // top line (at bottom)
      w->print (fc::NF_border_line_upper);
  }
}

//----------------------------------------------------------------------
void clearFlatBorder (FWidget* w)
{
  if ( ! w->isNewFont() )
    return;

  if ( auto p = w->getParentWidget() )
    w->setColor (w->wcolors.dialog_fg, p->getBackgroundColor());
  else
    w->setColor (w->wcolors.dialog_fg, w->wcolors.dialog_bg);

  std::size_t width = w->getWidth();
  std::size_t height = w->getHeight();

  for (std::size_t y{0}; y < height; y++)
  {
    // clear on left side
    w->print() << FPoint(0, int(y) + 1);

    if ( w->double_flatline_mask.left[y] )
      w->print (fc::NF_border_line_left);
    else
      w->print (' ');

    // clear on right side
    w->print() << FPoint(int(width) + 1, int(y) + 1);

    if ( w->double_flatline_mask.right[y] )
      w->print (fc::NF_rev_border_line_right);
    else
      w->print (' ');
  }

  // clear at top
  w->print() << FPoint(1, 0);

  for (std::size_t x{0}; x < width; x++)
  {
    if ( w->double_flatline_mask.top[x] )
      w->print (fc::NF_border_line_upper);
    else
      w->print (' ');
  }

  // clear at bottom
  w->print() << FPoint(1, int(height) + 1);

  for (std::size_t x{0}; x < width; x++)
  {
    if ( w->double_flatline_mask.bottom[x] )
      w->print (fc::NF_border_line_bottom);
    else
      w->print (' ');
  }
}

//----------------------------------------------------------------------
void drawBorder (FWidget* w, FRect r)
{
  if ( r.x1_ref() > r.x2_ref() )
    std::swap (r.x1_ref(), r.x2_ref());

  if ( r.y1_ref() > r.y2_ref() )
    std::swap (r.y1_ref(), r.y2_ref());

  if ( r.x1_ref() < 1 )
    r.x1_ref() = 1;

  if ( r.y1_ref() < 1 )
    r.y1_ref() = 1;

  if ( r.x2_ref() > int(w->getWidth()) )
    r.x2_ref() = int(w->getWidth());

  if ( r.y2_ref() > int(w->getHeight()) )
    r.y2_ref() = int(w->getHeight());

  if ( w->isNewFont() )
    drawNewFontBox (w, r);
  else
    drawBox (w, r);
}

//----------------------------------------------------------------------
inline void drawBox (FWidget* w, const FRect& r)
{
  // Use box-drawing characters to draw a border

  if ( ! w )
    return;

  w->print() << r.getUpperLeftPos()
             << fc::BoxDrawingsDownAndRight  // ┌
             << FString(r.getWidth() - 2, fc::BoxDrawingsHorizontal)  // ─
             << fc::BoxDrawingsDownAndLeft;   // ┐

  for (int y = r.getY1() + 1; y < r.getY2(); y++)
  {
    w->print() << FPoint(r.getX1(), y)
               << fc::BoxDrawingsVertical     // │
               << FPoint(r.getX2(), y)
               << fc::BoxDrawingsVertical;    // │
  }

  w->print() << r.getLowerLeftPos()
             << fc::BoxDrawingsUpAndRight  // └
             << FString(r.getWidth() - 2, fc::BoxDrawingsHorizontal)  // ─
             << fc::BoxDrawingsUpAndLeft;     // ┘
}

//----------------------------------------------------------------------
inline void drawNewFontBox (FWidget* w, const FRect& r)
{
  // Use new graphical font characters to draw a border

  w->print() << r.getUpperLeftPos()
             << fc::NF_border_corner_middle_upper_left    // ┌
             << FString(r.getWidth() - 2, fc::BoxDrawingsHorizontal)  // ─
             << fc::NF_border_corner_middle_upper_right;  // ┐

  for (int y = r.getY1() + 1; y < r.getY2(); y++)
  {
    w->print() << FPoint(r.getX1(), y)
               << fc::NF_border_line_left        // border left ⎸
               << FPoint(r.getX2(), y)
               << fc::NF_rev_border_line_right;  // border right⎹
  }

  w->print() << r.getLowerLeftPos()
             << fc::NF_border_corner_middle_lower_left    // └
             << FString(r.getWidth() - 2, fc::BoxDrawingsHorizontal)  // ─
             << fc::NF_border_corner_middle_lower_right;  // ┘
}

}  // namespace finalcut
