/***********************************************************************
* fwidget_functions.cpp - FWidget helper functions                     *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2021 Markus Gans                                      *
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

#include <algorithm>

#include "final/fapplication.h"
#include "final/fwidgetcolors.h"
#include "final/fwidget_functions.h"
#include "final/fwidget.h"
#include "final/vterm/fcolorpair.h"
#include "final/vterm/fstyle.h"
#include "final/widget/fstatusbar.h"

namespace finalcut
{

// FWidget non-member functions
//----------------------------------------------------------------------
bool isFocusNextKey (const FKey key)
{
  return ( key == FKey::Tab
        || key == FKey::Right
        || key == FKey::Down );
}

//----------------------------------------------------------------------
bool isFocusPrevKey (const FKey key)
{
  return ( key == FKey::Back_tab
        || key == FKey::Left
        || key == FKey::Up );
}

//----------------------------------------------------------------------
bool isDialogMenuKey (const FKey key)
{
  return ( key == FKey::Ctrl_caret     // Ctrl+^ (Ctrl+6)
        || key == FKey::F22            // Shift+F10
        || key == FKey::Shift_menu );  // Shift+Menu
}

//----------------------------------------------------------------------
bool isEnterKey (const FKey key)
{
  return ( key == FKey::Return
        || key == FKey::Enter );
}

//----------------------------------------------------------------------
bool isEscapeKey (const FKey key)
{
  return ( key == FKey::Escape
        || key == FKey::Escape_mintty );
}

//----------------------------------------------------------------------
FWidget* getFirstFocusableWidget (const FObject::FObjectList& list)
{
  if ( list.empty() )
    return nullptr;

  auto iter = list.cbegin();

  while ( iter != list.cend() )
  {
    if ( (*iter)->isWidget() )
    {
      auto child = static_cast<FWidget*>(*iter);

      if ( child->isEnabled() && child->acceptFocus() )
        return child;
    }

    ++iter;
  }

  return nullptr;
}

//----------------------------------------------------------------------
FWidget* getLastFocusableWidget (const FObject::FObjectList& list)
{
  if ( list.empty() )
    return nullptr;

  auto iter  = list.cend();

  do
  {
    --iter;

    if ( ! (*iter)->isWidget() )
      continue;

    auto child = static_cast<FWidget*>(*iter);

    if ( child->isEnabled() && child->acceptFocus() )
      return child;
  }
  while ( iter != list.cbegin() );

  return nullptr;
}

//----------------------------------------------------------------------
bool isInFWidgetList (const FWidget::FWidgetList* list, const FWidget* obj)
{
  if ( ! list || ! obj )
    return false;

  return std::any_of ( list->cbegin(), list->cend()
                     , [&obj] (const auto& w) { return w == obj; } );
}

//----------------------------------------------------------------------
FApplication* getFApplication()
{
  return FApplication::getApplicationObject();
}

//----------------------------------------------------------------------
FKey getHotkey (const FString& text)
{
  // Returns the hotkey character from a string
  // e.g. "E&xit" returns 'x'

  if ( text.isEmpty() )
    return FKey::None;

  std::size_t i{0};
  const std::size_t length = text.getLength();

  while ( i < length )
  {
    try
    {
      if ( i + 1 < length && text[i] == '&' )
      {
        i++;
        return FKey(text[i]);
      }
    }
    catch (const std::out_of_range&)
    {
      return FKey::None;
    }

    i++;
  }

  return FKey::None;
}

//----------------------------------------------------------------------
std::size_t getHotkeyPos (const FString& src, FString& dest)
{
  // Find hotkey position in string
  // + generate a new string without the '&'-sign

  constexpr auto NOT_SET = static_cast<std::size_t>(-1);
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

  if ( hotkey != FKey::None )
  {
    if ( std::isalpha(int(hotkey)) || std::isdigit(int(hotkey)) )
    {
      w->addAccelerator (FKey(std::tolower(int(hotkey))));
      w->addAccelerator (FKey(std::toupper(int(hotkey))));
      // Meta + hotkey
      w->addAccelerator (FKey::Meta_offset + FKey(std::tolower(int(hotkey))));
    }
    else
      w->addAccelerator (hotkey);
  }
  else
    w->delAccelerator();
}

//----------------------------------------------------------------------
void setWidgetFocus (FWidget* widget)
{
  if ( ! widget || widget->hasFocus() )
    return;

  auto focused_widget = FWidget::getFocusWidget();
  widget->setFocus();

  if ( focused_widget && focused_widget->isWidget() )  // old focused widget
    focused_widget->redraw();

  widget->redraw();

  if ( FWidget::getStatusBar() )
    FWidget::getStatusBar()->drawMessage();
}

//----------------------------------------------------------------------
bool setWidgetShadow (FWidget* w, bool enable)
{
  if ( enable
    && FVTerm::getFOutput()->getEncoding() != Encoding::VT100
    && FVTerm::getFOutput()->getEncoding() != Encoding::ASCII )
  {
    w->setFlags().shadow = true;
    w->setShadowSize(FSize{1, 1});
  }
  else
  {
    w->setFlags().shadow = false;
    w->setShadowSize(FSize{0, 0});
  }

  return w->getFlags().shadow;
}

//----------------------------------------------------------------------
void drawShadow (FWidget* w)
{
  if ( FVTerm::getFOutput()->isMonochron() && ! w->flags.trans_shadow )
    return;

  if ( (FVTerm::getFOutput()->getEncoding() == Encoding::VT100 && ! w->flags.trans_shadow)
    || (FVTerm::getFOutput()->getEncoding() == Encoding::ASCII && ! w->flags.trans_shadow) )
  {
    clearShadow(w);
    return;
  }

  if ( w->flags.trans_shadow )
    drawTransparentShadow (w);  // transparent shadow
  else if ( w->flags.shadow )
    drawBlockShadow (w);        // non-transparent shadow
}

//----------------------------------------------------------------------
void drawTransparentShadow (FWidget* w)
{
  // transparent shadow

  const std::size_t width = w->getWidth();
  const std::size_t height = w->getHeight();
  const auto& wc = FWidget::getColorTheme();
  w->print() << FStyle {Style::Transparent}
             << FPoint {int(width) + 1, 1}
             << "  "
             << FStyle {Style::None}
             << FColorPair {wc->shadow_bg, wc->shadow_fg}
             << FStyle {Style::ColorOverlay};

  for (std::size_t y{1}; y < height; y++)
  {
    w->print() << FPoint{int(width) + 1, int(y) + 1} << "  ";
  }

  w->print() << FStyle {Style::None} << FStyle {Style::Transparent}
             << FPoint {1, int(height) + 1}
             << "  "
             << FStyle {Style::None}
             << FColorPair {wc->shadow_bg, wc->shadow_fg}
             << FStyle {Style::ColorOverlay}
             << FString {width, L' '}
             << FStyle {Style::None};

  if ( FVTerm::getFOutput()->isMonochron() )
    w->setReverse(false);
}

//----------------------------------------------------------------------
void drawBlockShadow (FWidget* w)
{
  // non-transparent shadow

  if ( ! FVTerm::getFOutput()->hasShadowCharacter() )
    return;

  const std::size_t width = w->getWidth();
  const std::size_t height = w->getHeight();
  const auto& wc = FWidget::getColorTheme();
  w->print() << FPoint {int(width) + 1, 1};

  if ( w->isWindowWidget() )
  {
    w->print() << FColorPair {wc->shadow_fg, wc->shadow_bg}
               << FStyle {Style::InheritBackground};  // current background color will be ignored
  }
  else if ( auto p = w->getParentWidget() )
    w->print() << FColorPair {wc->shadow_fg, p->getBackgroundColor()};

  w->print (UniChar::LowerHalfBlock);  // ▄

  if ( w->isWindowWidget() )
    w->print() << FStyle {Style::InheritBackground};

  for (std::size_t y{1}; y < height; y++)
  {
    w->print() << FPoint {int(width) + 1, int(y) + 1}
               << UniChar::FullBlock;  // █
  }

  w->print() << FPoint {2, int(height) + 1};

  if ( w->isWindowWidget() )
    w->print() << FStyle {Style::InheritBackground};

  w->print() << FString{width, UniChar::UpperHalfBlock};  // ▀

  if ( w->isWindowWidget() )
    w->print() << FStyle {Style::None};
}

//----------------------------------------------------------------------
void clearShadow (FWidget* w)
{
  if ( FVTerm::getFOutput()->isMonochron() )
    return;

  const std::size_t width = w->getWidth();
  const std::size_t height = w->getHeight();
  const auto& wc = FWidget::getColorTheme();

  if ( w->isWindowWidget() )
  {
    w->print() << FColorPair {wc->shadow_fg, wc->shadow_bg}
               << FStyle {Style::InheritBackground};  // current background color will be ignored
  }
  else if ( auto p = w->getParentWidget() )
    w->print() << FColorPair {wc->shadow_fg, p->getBackgroundColor()};

  if ( int(width) <= w->woffset.getX2() )
  {
    for (std::size_t y{1}; y <= height; y++)
    {
      w->print() << FPoint {int(width) + 1, int(y)}
                 << ' ';  // clear █
    }
  }

  if ( int(height) <= w->woffset.getY2() )
  {
    w->print() << FPoint{2, int(height) + 1}
               << FString{width, L' '};  // clear ▀
  }

  if ( w->isWindowWidget() )
    w->print() << FStyle {Style::None};
}

//----------------------------------------------------------------------
void drawFlatBorder (FWidget* w)
{
  if ( ! FVTerm::getFOutput()->isNewFont() )
    return;

  const std::size_t width = w->getWidth();
  const std::size_t height = w->getHeight();
  const auto& wc = FWidget::getColorTheme();

  if ( auto p = w->getParentWidget() )
    w->setColor (wc->dialog_fg, p->getBackgroundColor());
  else
    w->setColor (wc->dialog_fg, wc->dialog_bg);

  for (std::size_t y{0}; y < height; y++)
  {
    w->print() << FPoint {0, int(y) + 1};

    if ( w->double_flatline_mask.left[uLong(y)] )
      // left+right line (on left side)
      w->print (UniChar::NF_rev_border_line_right_and_left);
    else
      // right line (on left side)
      w->print (UniChar::NF_rev_border_line_right);

    w->print() << FPoint {int(width) + 1, int(y) + 1};

    if ( w->double_flatline_mask.right[y] )
      // left+right line (on right side)
      w->print (UniChar::NF_rev_border_line_right_and_left);
    else
      // left line (on right side)
      w->print (UniChar::NF_border_line_left);
  }

  w->print() << FPoint {1, 0};

  for (std::size_t x{0}; x < width; x++)
  {
    if ( w->double_flatline_mask.top[x] )
      // top+bottom line (at top)
      w->print (UniChar::NF_border_line_up_and_down);
    else
      // bottom line (at top)
      w->print (UniChar::NF_border_line_bottom);
  }

  w->print() << FPoint {1, int(height) + 1};

  for (std::size_t x{0}; x < width; x++)
  {
    if ( w->double_flatline_mask.bottom[x] )
      // top+bottom line (at bottom)
      w->print (UniChar::NF_border_line_up_and_down);
    else
      // top line (at bottom)
      w->print (UniChar::NF_border_line_upper);
  }
}

//----------------------------------------------------------------------
void clearFlatBorder (FWidget* w)
{
  if ( ! FVTerm::getFOutput()->isNewFont() )
    return;

  const std::size_t width = w->getWidth();
  const std::size_t height = w->getHeight();
  const auto& wc = FWidget::getColorTheme();

  if ( auto p = w->getParentWidget() )
    w->setColor (wc->dialog_fg, p->getBackgroundColor());
  else
    w->setColor (wc->dialog_fg, wc->dialog_bg);

  for (std::size_t y{0}; y < height; y++)
  {
    // clear on left side
    w->print() << FPoint {0, int(y) + 1};

    if ( w->double_flatline_mask.left[y] )
      w->print (UniChar::NF_border_line_left);
    else
      w->print (' ');

    // clear on right side
    w->print() << FPoint {int(width) + 1, int(y) + 1};

    if ( w->double_flatline_mask.right[y] )
      w->print (UniChar::NF_rev_border_line_right);
    else
      w->print (' ');
  }

  // clear at top
  w->print() << FPoint {1, 0};

  for (std::size_t x{0}; x < width; x++)
  {
    if ( w->double_flatline_mask.top[x] )
      w->print (UniChar::NF_border_line_upper);
    else
      w->print (' ');
  }

  // clear at bottom
  w->print() << FPoint {1, int(height) + 1};

  for (std::size_t x{0}; x < width; x++)
  {
    if ( w->double_flatline_mask.bottom[x] )
      w->print (UniChar::NF_border_line_bottom);
    else
      w->print (' ');
  }
}

//----------------------------------------------------------------------
inline void checkBorder (const FWidget* w, FRect& r)
{
  if ( r.x1_ref() > r.x2_ref() )
    std::swap (r.x1_ref(), r.x2_ref());

  if ( r.y1_ref() > r.y2_ref() )
    std::swap (r.y1_ref(), r.y2_ref());

  if ( r.x1_ref() < 1 )
    r.x1_ref() = 1;

  if ( r.y1_ref() < 1 )
    r.y1_ref() = 1;

  if ( r.x2_ref() > r.x1_ref() + int(w->getWidth()) - 1 )
    r.x2_ref() = r.x1_ref() + int(w->getWidth()) - 1;

  if ( r.y2_ref() > r.y1_ref() + int(w->getHeight()) - 1 )
    r.y2_ref() = r.y1_ref() + int(w->getHeight()) - 1;
}

//----------------------------------------------------------------------
void drawBorder (FWidget* w, const FRect& r)
{
  FRect rect = r;
  checkBorder (w, rect);

  if ( r.getWidth() < 3 )
    return;

  if ( FVTerm::getFOutput()->isNewFont() )
    drawNewFontBox (w, rect);
  else
    drawBox (w, rect);
}

//----------------------------------------------------------------------
void drawListBorder (FWidget* w, const FRect& r)
{
  FRect rect = r;
  checkBorder (w, rect);

  if ( r.getWidth() < 3 )
    return;

  if ( FVTerm::getFOutput()->isNewFont() )
    drawNewFontListBox (w, rect);
  else
    drawBox (w, rect);
}

//----------------------------------------------------------------------
inline void drawBox (FWidget* w, const FRect& r)
{
  // Use box-drawing characters to draw a border

  if ( ! w || r.getWidth() < 3 )
    return;

  w->print() << r.getUpperLeftPos()
             << UniChar::BoxDrawingsDownAndRight   // ┌
             << FString{r.getWidth() - 2, UniChar::BoxDrawingsHorizontal}  // ─
             << UniChar::BoxDrawingsDownAndLeft;   // ┐

  for (auto y = r.getY1() + 1; y < r.getY2(); y++)
  {
    w->print() << FPoint{r.getX1(), y}
               << UniChar::BoxDrawingsVertical     // │
               << FPoint{r.getX2(), y}
               << UniChar::BoxDrawingsVertical;    // │
  }

  w->print() << r.getLowerLeftPos()
             << UniChar::BoxDrawingsUpAndRight     // └
             << FString{r.getWidth() - 2, UniChar::BoxDrawingsHorizontal}  // ─
             << UniChar::BoxDrawingsUpAndLeft;     // ┘
}

//----------------------------------------------------------------------
inline void drawNewFontBox (FWidget* w, const FRect& r)
{
  // Use new graphical font characters to draw a border

  w->print() << r.getUpperLeftPos()
             << UniChar::NF_border_corner_middle_upper_left    // ┌
             << FString{r.getWidth() - 2, UniChar::NF_border_line_horizontal}  // ─
             << UniChar::NF_border_corner_middle_upper_right;  // ┐

  for (auto y = r.getY1() + 1; y < r.getY2(); y++)
  {
    w->print() << FPoint{r.getX1(), y}
               << UniChar::NF_border_line_vertical   // │
               << FPoint{r.getX2(), y}
               << UniChar::NF_border_line_vertical;  // │
  }

  w->print() << r.getLowerLeftPos()
             << UniChar::NF_border_corner_middle_lower_left    // └
             << FString{r.getWidth() - 2, UniChar::NF_border_line_horizontal}  // ─
             << UniChar::NF_border_corner_middle_lower_right;  // ┘
}

//----------------------------------------------------------------------
inline void drawNewFontListBox (FWidget* w, const FRect& r)
{
  w->print() << r.getUpperLeftPos()
             << UniChar::NF_border_line_middle_left_down  // ┌
             << FString{r.getWidth() - 2, UniChar::NF_border_line_horizontal}  // ─
             << UniChar::NF_border_line_left_down;        // ╷

  for (auto y = r.getY1() + 1; y < r.getY2(); y++)
  {
    w->print() << FPoint{r.getX1(), y}
               << UniChar::NF_border_line_left   // border left ⎸
               << FPoint{r.getX2(), y}
               << UniChar::NF_border_line_left;  // border left ⎸
  }

  w->print() << r.getLowerLeftPos()
             << UniChar::NF_border_line_middle_right_up  // └
             << FString{r.getWidth() - 2, UniChar::NF_border_line_horizontal}  // ─
             << UniChar::NF_border_line_left_up;         // ╵
}

}  // namespace finalcut
