/***********************************************************************
* fwidget_functions.cpp - FWidget helper functions                     *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2023 Markus Gans                                      *
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
#include <array>
#include <memory>

#include "final/dialog/fdialog.h"
#include "final/fapplication.h"
#include "final/fevent.h"
#include "final/fwidgetcolors.h"
#include "final/fwidget_functions.h"
#include "final/fwidget.h"
#include "final/vterm/fcolorpair.h"
#include "final/vterm/fstyle.h"
#include "final/widget/fstatusbar.h"

namespace finalcut
{

namespace internal
{

struct var
{
  static uInt8 b1_print_trans_mask;
};

uInt8 var::b1_print_trans_mask{};

}  // namespace internal

// FWidget non-member functions
//----------------------------------------------------------------------
void initByte1PrintTransMask()
{
  // Set bits that must not be reset
  FCharAttribute mask{};
  mask.transparent = true;
  mask.color_overlay = true;
  mask.inherit_background = true;
  mask.no_changes = true;
  mask.printed = true;
  internal::var::b1_print_trans_mask = getFAttributeByte(mask, 1);
}

//----------------------------------------------------------------------
auto isFocusNextKey (const FKey key) -> bool
{
  return ( key == FKey::Tab
        || key == FKey::Right
        || key == FKey::Down );
}

//----------------------------------------------------------------------
auto isFocusPrevKey (const FKey key) -> bool
{
  return ( key == FKey::Back_tab
        || key == FKey::Left
        || key == FKey::Up );
}

//----------------------------------------------------------------------
auto isDialogMenuKey (const FKey key) -> bool
{
  return ( key == FKey::Ctrl_caret     // Ctrl+^ (Ctrl+6)
        || key == FKey::F22            // Shift+F10
        || key == FKey::Shift_menu );  // Shift+Menu
}

//----------------------------------------------------------------------
auto isEnterKey (const FKey key) -> bool
{
  return ( key == FKey::Return
        || key == FKey::Enter );
}

//----------------------------------------------------------------------
auto isEscapeKey (const FKey key) -> bool
{
  return ( key == FKey::Escape
        || key == FKey::Escape_mintty );
}

//----------------------------------------------------------------------
auto getFirstFocusableWidget (const FObject::FObjectList& list) -> FWidget*
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
auto getLastFocusableWidget (const FObject::FObjectList& list) -> FWidget*
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
auto isInFWidgetList (const FWidget::FWidgetList* list, const FWidget* obj) -> bool
{
  if ( ! list || ! obj )
    return false;

  return std::any_of ( list->cbegin(), list->cend()
                     , [&obj] (const auto& w) { return w == obj; } );
}

//----------------------------------------------------------------------
auto getFApplication() -> FApplication*
{
  return FApplication::getApplicationObject();
}

//----------------------------------------------------------------------
auto getAlignOffset ( Align alignment
                    , const std::size_t width
                    , const std::size_t length ) -> std::size_t
{
  if ( alignment == Align::Center )
  {
    if ( length < width )
      return (width - length) / 2;
  }
  else if ( alignment == Align::Right && length < width )
  {
    return width - length;
  }

  return 0;
}

//----------------------------------------------------------------------
auto getHotkey (const FString& text) -> FKey
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
auto getHotkeyPos (const FString& src, FString& dest) -> std::size_t
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
  drawStatusBarMessage();
}

//----------------------------------------------------------------------
auto setWidgetShadow (FWidget* w, bool enable) -> bool
{
  if ( enable
    && FVTerm::getFOutput()->getEncoding() != Encoding::VT100
    && FVTerm::getFOutput()->getEncoding() != Encoding::ASCII )
  {
    w->setFlags().shadow.shadow = true;
    w->setShadowSize(FSize{1, 1});
  }
  else
  {
    w->setFlags().shadow.shadow = false;
    w->setShadowSize(FSize{0, 0});
  }

  return w->getFlags().shadow.shadow;
}

//----------------------------------------------------------------------
void passResizeCornerEventToDialog (const FWidget* w, const FMouseEvent& ev)
{
  // Pass mouse event to the parent widget

  auto parent = w->getParentWidget();

  if ( ! parent || ! parent->isDialogWidget() )
    return;

  const auto& dialog = static_cast<FDialog*>(parent);

  if ( ! dialog->isResizeable() || dialog->isZoomed() )
    return;

  const auto& type = ev.getType();
  const auto& tpos = ev.getTermPos();
  const auto& par = parent->termToWidgetPos(tpos);
  const auto btn = ev.getButton();
  const auto& new_ev = \
      std::make_shared<FMouseEvent>(type, par, tpos, btn);
  FApplication::sendEvent (parent, new_ev.get());
}

//----------------------------------------------------------------------
void drawShadow (FWidget* w)
{
  if ( FVTerm::getFOutput()->isMonochron() && ! w->flags.shadow.trans_shadow )
    return;

  if ( (FVTerm::getFOutput()->getEncoding() == Encoding::VT100 && ! w->flags.shadow.trans_shadow)
    || (FVTerm::getFOutput()->getEncoding() == Encoding::ASCII && ! w->flags.shadow.trans_shadow) )
  {
    clearBlockShadow(w);
    return;
  }

  if ( w->flags.shadow.trans_shadow )
    drawTransparentShadow (w);  // transparent shadow
  else if ( w->flags.shadow.shadow )
    drawBlockShadow (w);        // non-transparent shadow
}

//----------------------------------------------------------------------
void drawTransparentShadow (FWidget* w)
{
  // transparent shadow

  if ( ! w || ! w->getPrintArea() )
    return;

  auto& area = *w->getPrintArea();
  const auto width = uInt(area.width);
  const auto height = uInt(area.height);
  const auto shadow_width = uInt(area.right_shadow);
  const auto shadow_height = uInt(area.bottom_shadow);
  const auto& wc = FWidget::getColorTheme();

  const FChar transparent_char
  {
    { { L'\0',  L'\0', L'\0', L'\0', L'\0' } },
    { { L'\0', L'\0', L'\0', L'\0', L'\0' } },
    FColor::Default,
    FColor::Default,
    { { 0x00, 0x20, 0x00, 0x00} }  // byte 0..3 (byte 1 = 0x32 = transparent)
  };

  const FChar color_overlay_char
  {
    { { L'\0', L'\0', L'\0', L'\0', L'\0' } },
    { { L'\0', L'\0', L'\0', L'\0', L'\0' } },
    wc->shadow_bg,
    wc->shadow_fg,
    { { 0x00, 0x40, 0x00, 0x00} }  // byte 0..3 (byte 1 = 0x64 = color_overlay)
  };

  auto* area_pos = &area.getFChar(int(width), 0);
  auto& area_changes = area.changes;

  if ( shadow_width > 0 )  // Draw right shadow
  {
    std::fill (area_pos, area_pos + shadow_width, transparent_char);
    area_changes[0].xmin = std::min(area_changes[0].xmin, width);
    area_changes[0].xmax = width + shadow_width - 1;
    area_changes[0].trans_count += shadow_width;

    for (std::size_t y{1}; y < height; y++)
    {
      area_pos += shadow_width + width;
      area_changes[y].xmin = std::min(area_changes[y].xmin, width);
      area_changes[y].xmax = width + shadow_width - 1;
      area_changes[y].trans_count += shadow_width;
      std::fill (area_pos, area_pos + shadow_width, color_overlay_char);
    }

    area_pos += shadow_width;
  }

  for (std::size_t y{1}; y <= shadow_height; y++)  // Draw bottom shadow
  {
    area_changes[y].xmin = 0;
    area_changes[y].xmax = width + shadow_width - 1;
    area_changes[y].trans_count += width + shadow_width;
    std::fill (area_pos, area_pos + shadow_width, transparent_char);
    area_pos += shadow_width;
    std::fill (area_pos, area_pos + width, color_overlay_char);
    area_pos += width;
  }

  area.has_changes = true;

  if ( FVTerm::getFOutput()->isMonochron() )
    w->setReverse(false);
}

//----------------------------------------------------------------------
void drawBlockShadow (FWidget* w)
{
  // non-transparent shadow

  if ( ! w || ! FVTerm::getFOutput()->hasShadowCharacter() )
    return;

  const auto& wc = FWidget::getColorTheme();

  std::array<FChar, 4> shadow_char
  {{
    {
      { { wchar_t(UniChar::LowerHalfBlock),  L'\0', L'\0', L'\0', L'\0' } },  // ▄
      { { L'\0', L'\0', L'\0', L'\0', L'\0' } },
      wc->shadow_fg,
      wc->shadow_bg,
      { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3 (byte 2 = 0x08 = char_width 1)
    },
    {
      { { wchar_t(UniChar::FullBlock),  L'\0', L'\0', L'\0', L'\0' } },  // █
      { { L'\0', L'\0', L'\0', L'\0', L'\0' } },
      wc->shadow_fg,
      wc->shadow_bg,
      { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3 (byte 2 = 0x08 = char_width 1)
    },
    {
      { { L' ',  L'\0', L'\0', L'\0', L'\0' } },  // ' '
      { { L'\0', L'\0', L'\0', L'\0', L'\0' } },
      wc->shadow_fg,
      wc->shadow_bg,
      { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3 (byte 2 = 0x08 = char_width 1)
    },
    {
      { { wchar_t(UniChar::UpperHalfBlock),  L'\0', L'\0', L'\0', L'\0' } },  // ▄
      { { L'\0', L'\0', L'\0', L'\0', L'\0' } },
      wc->shadow_fg,
      wc->shadow_bg,
      { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3 (byte 2 = 0x08 = char_width 1)
    }
  }};

  if (  w->isWindowWidget() )
  {
    shadow_char[0].attr.bit.inherit_background = true;
    shadow_char[1].attr.bit.inherit_background = true;
    shadow_char[2].attr.bit.transparent = true;
    shadow_char[3].attr.bit.inherit_background = true;
  }
  else if ( auto p = w->getParentWidget() )
  {
    shadow_char[0].bg_color = p->getBackgroundColor();
    shadow_char[1].bg_color = p->getBackgroundColor();
    shadow_char[2].bg_color = p->getBackgroundColor();
    shadow_char[3].bg_color = p->getBackgroundColor();
  }

  drawGenericBlockShadow (w, shadow_char);
}

//----------------------------------------------------------------------
void clearBlockShadow (FWidget* w)
{
  if ( ! w || ! FVTerm::getFOutput()->hasShadowCharacter() )
    return;

  const auto& wc = FWidget::getColorTheme();

  FChar spacer_char
  {
    { { L' ',  L'\0', L'\0', L'\0', L'\0' } },  // ' '
    { { L'\0', L'\0', L'\0', L'\0', L'\0' } },
    wc->shadow_fg,
    wc->shadow_bg,
    { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3 (byte 2 = 0x08 = char_width 1)
  };

  if (  w->isWindowWidget() )
    spacer_char.attr.bit.transparent = true;
  else if ( auto p = w->getParentWidget() )
    spacer_char.bg_color = p->getBackgroundColor();

  const std::array<FChar, 4> shadow_char
  {
    {spacer_char, spacer_char, spacer_char, spacer_char}
  };

  drawGenericBlockShadow (w, shadow_char);
}

//----------------------------------------------------------------------
void drawGenericBlockShadow ( FWidget* w
                            , const std::array<FChar, 4>& shadow_char )
{
  if ( ! w
    || ! w->getPrintArea()
    || ! FVTerm::getFOutput()->hasShadowCharacter() )
    return;

  auto& area = *w->getPrintArea();
  const bool is_window = w->isWindowWidget();
  const auto width = is_window ? uInt(area.width) : uInt(w->getWidth());
  const auto height = is_window ? uInt(area.height) : uInt(w->getHeight());
  const auto shadow_width = uInt(area.right_shadow);
  const auto shadow_height = uInt(area.bottom_shadow);
  const auto x_offset = uInt(w->woffset.getX1() + w->getX() - area.offset_left - 1);
  const auto y_offset = uInt(w->woffset.getY1() + w->getY() - area.offset_top - 1);

  if ( is_window && (shadow_width < 1 || shadow_height < 1) )
    return;

  auto y = y_offset;
  auto& area_changes = area.changes;
  auto* area_pos = &area.getFChar(int(x_offset + width), int(y_offset));
  *area_pos = shadow_char[0];  // ▄
  area_changes[y].xmin = std::min(area_changes[y].xmin, x_offset + width);
  area_changes[y].xmax = std::max(area_changes[y].xmax, x_offset + width);
  area_changes[y].trans_count++;

  for (y = y_offset + 1; y < y_offset + height; y++)
  {
    area_pos = &area.getFChar(int(x_offset + width), int(y));
    *area_pos = shadow_char[1];  // █
    area_changes[y].xmin = std::min(area_changes[y].xmin, x_offset + width);
    area_changes[y].xmax = std::max(area_changes[y].xmax, x_offset + width);
    area_changes[y].trans_count++;
  }

  area_pos = &area.getFChar(int(x_offset), int(y_offset + height));
  *area_pos = shadow_char[2];  // ' '
  ++area_pos;
  std::fill (area_pos, area_pos + width, shadow_char[3]);  // ▀
  area_changes[y].xmin = std::min(area_changes[y].xmin, uInt(x_offset));
  area_changes[y].xmax = std::max(area_changes[y].xmax, x_offset + width);
  area_changes[y].trans_count += width + 1;
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

  r.x1_ref() = std::max(r.x1_ref(), 1);
  r.y1_ref() = std::max(r.y1_ref(), 1);
  r.x2_ref() = std::min(r.x2_ref(), r.x1_ref() + int(w->getWidth()) - 1);
  r.y2_ref() = std::min(r.y2_ref(), r.y1_ref() + int(w->getHeight()) - 1);
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

  constexpr std::array<wchar_t, 8> box_char
  {{
    static_cast<wchar_t>(UniChar::BoxDrawingsDownAndRight),  // ┌
    static_cast<wchar_t>(UniChar::BoxDrawingsHorizontal),    // ─
    static_cast<wchar_t>(UniChar::BoxDrawingsDownAndLeft),   // ┐
    static_cast<wchar_t>(UniChar::BoxDrawingsVertical),      // │
    static_cast<wchar_t>(UniChar::BoxDrawingsVertical),      // │
    static_cast<wchar_t>(UniChar::BoxDrawingsUpAndRight),    // └
    static_cast<wchar_t>(UniChar::BoxDrawingsHorizontal),    // ─
    static_cast<wchar_t>(UniChar::BoxDrawingsUpAndLeft)      // ┘
  }};

  drawGenericBox (w, r, box_char);
}

//----------------------------------------------------------------------
inline void drawNewFontBox (FWidget* w, const FRect& r)
{
  // Use new graphical font characters to draw a border

  constexpr std::array<wchar_t, 8> box_char
  {{
    static_cast<wchar_t>(UniChar::NF_border_corner_middle_upper_left),   // ┌
    static_cast<wchar_t>(UniChar::NF_border_line_horizontal),            // ─
    static_cast<wchar_t>(UniChar::NF_border_corner_middle_upper_right),  // ┐
    static_cast<wchar_t>(UniChar::NF_border_line_vertical),              // │
    static_cast<wchar_t>(UniChar::NF_border_line_vertical),              // │
    static_cast<wchar_t>(UniChar::NF_border_corner_middle_lower_left),   // └
    static_cast<wchar_t>(UniChar::NF_border_line_horizontal),            // ─
    static_cast<wchar_t>(UniChar::NF_border_corner_middle_lower_right)   // ┘
  }};

  drawGenericBox (w, r, box_char);
}

//----------------------------------------------------------------------
void drawNewFontUShapedBox (FWidget* w, const FRect& r)
{
  // Draw a new graphical font U-shaped frame

  constexpr std::array<wchar_t, 8> box_char
  {{
    static_cast<wchar_t>(UniChar::NF_border_line_left),              // border left ⎸
    L' ',                                                            // space ' '
    static_cast<wchar_t>(UniChar::NF_rev_border_line_right),         // border right⎹
    static_cast<wchar_t>(UniChar::NF_border_line_left),              // border left ⎸
    static_cast<wchar_t>(UniChar::NF_rev_border_line_right),         // border right⎹
    static_cast<wchar_t>(UniChar::NF_border_corner_lower_left),      // ⎣
    static_cast<wchar_t>(UniChar::NF_border_line_bottom),            // _
    static_cast<wchar_t>(UniChar::NF_rev_border_corner_lower_right)  // ⎦
  }};

  drawGenericBox (w, r, box_char);
}

//----------------------------------------------------------------------
inline void drawNewFontListBox (FWidget* w, const FRect& r)
{
  constexpr std::array<wchar_t, 8> box_char
  {{
    static_cast<wchar_t>(UniChar::NF_border_line_middle_left_down),  // ┌
    static_cast<wchar_t>(UniChar::NF_border_line_horizontal),        // ─
    static_cast<wchar_t>(UniChar::NF_border_line_left_down),         // ╷
    static_cast<wchar_t>(UniChar::NF_border_line_left),              // border left ⎸
    static_cast<wchar_t>(UniChar::NF_border_line_left),              // border left ⎸
    static_cast<wchar_t>(UniChar::NF_border_line_middle_right_up),   // └
    static_cast<wchar_t>(UniChar::NF_border_line_horizontal),        // ─
    static_cast<wchar_t>(UniChar::NF_border_line_left_up)            // ╵
  }};

  drawGenericBox (w, r, box_char);
}

//----------------------------------------------------------------------
void drawGenericBox ( FWidget* w, const FRect& r
                    , const std::array<wchar_t, 8>& box_char )
{
  if ( ! w || ! w->getPrintArea() || r.getWidth() < 3 )
    return;

  // Get references to frequently accessed variables
  auto& area = *w->getPrintArea();
  auto& area_changes = area.changes;
  auto fchar = FVTermAttribute::getAttribute();

  // Prepare the first character to draw the box
  fchar.attr.bit.char_width = 1;
  fchar.ch[0] = box_char[0];
  fchar.ch[1] = L'\0';
  const auto is_transparent = (fchar.attr.byte[1] & internal::var::b1_print_trans_mask) != 0;

  // Adjust box position to match print area
  auto box = r;
  box.move (-1, -1);
  const auto x_offset = uInt(w->woffset.getX1() + w->getX() - area.offset_left - 1);
  const auto y_offset = uInt(w->woffset.getY1() + w->getY() - area.offset_top - 1);

  // Draw the top line of the box
  auto* area_pos = &area.getFChar(int(x_offset) + box.getX1(), int(y_offset) + box.getY1());
  *area_pos = fchar;
  ++area_pos;
  fchar.ch[0] = box_char[1];
  const auto line_length = box.getWidth() - 2;
  std::fill (area_pos, area_pos + line_length, fchar);
  area_pos += line_length;
  fchar.ch[0] = box_char[2];
  *area_pos = fchar;

  // Update area_changes for the top line
  auto y = y_offset + uInt(box.getY1());
  area_changes[y].xmin = std::min(area_changes[y].xmin, x_offset + uInt(box.getX1()));
  area_changes[y].xmax = std::max(area_changes[y].xmax, x_offset + uInt(box.getX2()));
  area_changes[y].trans_count += uInt(is_transparent) * box.getWidth();

  // Draw the sides of the box
  for (y = y_offset + uInt(box.getY1()) + 1; y < y_offset + uInt(box.getY2()); y++)
  {
    area_pos = &area.getFChar(int(x_offset) + box.getX1(), int(y));
    fchar.ch[0] = box_char[3];
    *area_pos = fchar;
    area_pos += box.getWidth() - 1;
    fchar.ch[0] = box_char[4];
    *area_pos = fchar;
    // Update area_changes for the sides
    area_changes[y].xmin = std::min(area_changes[y].xmin, x_offset + uInt(box.getX1()));
    area_changes[y].xmax = std::max(area_changes[y].xmax, x_offset + uInt(box.getX2()));
    area_changes[y].trans_count += uInt(is_transparent) * box.getWidth();
  }

  // Draw the bottom line of the box
  area_pos = &area.getFChar(int(x_offset) + box.getX1(), int(y));
  fchar.ch[0] = box_char[5];
  *area_pos = fchar;
  ++area_pos;
  fchar.ch[0] = box_char[6];
  std::fill (area_pos, area_pos + line_length, fchar);
  area_pos += line_length;
  fchar.ch[0] = box_char[7];
  *area_pos = fchar;

  // Update area_changes for the bottom line
  y = y_offset + uInt(box.getY2());
  area_changes[y].xmin = std::min(area_changes[y].xmin, x_offset + uInt(box.getX1()));
  area_changes[y].xmax = std::max(area_changes[y].xmax, x_offset + uInt(box.getX2()));
  area_changes[y].trans_count += uInt(is_transparent) * box.getWidth();
  area.has_changes = true;
}

//----------------------------------------------------------------------
void updateStatusbar (const FWidget* w, bool need_focus)
{
  if ( w->hasFocus() != need_focus )
    return;

  if ( auto sbar = w->getStatusBar() )
  {
    const auto& msg = w->getStatusbarMessage();
    const auto& curMsg = sbar->getMessage();

    if ( curMsg != msg )
    {
      sbar->setMessage(msg);
      sbar->drawMessage();
    }
  }
}

//----------------------------------------------------------------------
void drawStatusBarMessage()
{
  if ( FWidget::getStatusBar() )
    FWidget::getStatusBar()->drawMessage();
}

}  // namespace finalcut
