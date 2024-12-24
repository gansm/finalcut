/***********************************************************************
* fwidget_functions.cpp - FWidget helper functions                     *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2024 Markus Gans                                      *
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

constexpr auto initByte1PrintTransMask() -> uInt8
{
  // Set bits that must not be reset
  FCharAttribute mask{};
  mask.transparent = true;
  mask.color_overlay = true;
  mask.inherit_background = true;
  mask.no_changes = true;
  mask.printed = true;
  return getFAttributeByte(mask, 1);
}

struct var
{
  static constexpr auto b1_print_trans_mask = initByte1PrintTransMask();
};

constexpr uInt8 var::b1_print_trans_mask;

}  // namespace internal

//----------------------------------------------------------------------
struct TransparentShadowData
{
  // Using-declaration
  using FTermArea = FVTerm::FTermArea;

  // Data members
  FTermArea& area;
  uInt       width{};
  uInt       height{};
  uInt       shadow_width{};
  uInt       shadow_height{};
  FChar      transparent_char{};
  FChar      color_overlay_char{};
  FChar*     area_ptr{};
};


// Function forward declarations
//----------------------------------------------------------------------
void drawRightShadow (TransparentShadowData&);
void drawBottomShadow (TransparentShadowData&);


// FWidget non-member functions
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
auto isExpandComboBoxKey (const FKey key) -> bool
{
  return ( key == FKey::F4
        || key == FKey::Meta_down
        || key == FKey::Ctrl_down );
}

//----------------------------------------------------------------------
auto isCollapseComboBoxKey (const FKey key) -> bool
{
  return ( key == FKey::Meta_up
        || key == FKey::Ctrl_up
        || isEscapeKey(key) );
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
void setWidgetShadow (FWidget* w, bool enable)
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
  const auto& wc_shadow = FWidget::getColorTheme()->shadow;

  TransparentShadowData data
  {
    area,
    uInt(area.size.width),
    uInt(area.size.height),
    uInt(area.shadow.width),
    uInt(area.shadow.height),
    {
      { { L'\0',  L'\0', L'\0', L'\0', L'\0' } },
      { { L'\0', L'\0', L'\0', L'\0', L'\0' } },
      { { FColor::Default, FColor::Default } },
      { { 0x00, 0x20, 0x00, 0x00} }  // byte 0..3 (byte 1 = 0x32 = transparent)
    },
    {
      { { L'\0', L'\0', L'\0', L'\0', L'\0' } },
      { { L'\0', L'\0', L'\0', L'\0', L'\0' } },
      { { wc_shadow.fg, wc_shadow.bg } },
      { { 0x00, 0x40, 0x00, 0x00} }  // byte 0..3 (byte 1 = 0x64 = color_overlay)
    },
    &area.getFChar(area.size.width, 0)
  };

  drawRightShadow(data);
  drawBottomShadow(data);
  area.has_changes = true;

  if ( FVTerm::getFOutput()->isMonochron() )
    w->setReverse(false);
}

//----------------------------------------------------------------------
inline void drawRightShadow (TransparentShadowData& d)
{
  if ( d.shadow_width > 0 )  // Draw right shadow
  {
    std::fill (d.area_ptr, d.area_ptr + d.shadow_width, d.transparent_char);
    d.area.changes[0].xmin = std::min(d.area.changes[0].xmin, d.width);
    d.area.changes[0].xmax = d.width + d.shadow_width - 1;
    d.area.changes[0].trans_count += d.shadow_width;

    for (std::size_t y{1}; y < d.height; y++)
    {
      d.area_ptr += d.shadow_width + d.width;
      d.area.changes[y].xmin = std::min(d.area.changes[y].xmin, d.width);
      d.area.changes[y].xmax = d.width + d.shadow_width - 1;
      d.area.changes[y].trans_count += d.shadow_width;
      std::fill (d.area_ptr, d.area_ptr + d.shadow_width, d.color_overlay_char);
    }

    d.area_ptr += d.shadow_width;
  }
}

//----------------------------------------------------------------------
inline void drawBottomShadow (TransparentShadowData& d)
{
  for (std::size_t y{d.height}; y < d.height + d.shadow_height; y++)  // Draw bottom shadow
  {
    d.area.changes[y].xmin = 0;
    d.area.changes[y].xmax = d.width + d.shadow_width - 1;
    d.area.changes[y].trans_count += d.width + d.shadow_width;
    std::fill (d.area_ptr, d.area_ptr + d.shadow_width, d.transparent_char);
    d.area_ptr += d.shadow_width;
    std::fill (d.area_ptr, d.area_ptr + d.width, d.color_overlay_char);
    d.area_ptr += d.width;
  }
}

//----------------------------------------------------------------------
void drawBlockShadow (FWidget* w)
{
  // non-transparent shadow

  if ( ! w || ! FVTerm::getFOutput()->hasShadowCharacter() )
    return;

  const auto& wc_shadow = FWidget::getColorTheme()->shadow;

  std::array<FChar, 4> shadow_char
  {{
    {
      { { wchar_t(UniChar::LowerHalfBlock),  L'\0', L'\0', L'\0', L'\0' } },  // ▄
      { { L'\0', L'\0', L'\0', L'\0', L'\0' } },
      { { wc_shadow.bg, FColor::Default } },
      { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3 (byte 2 = 0x08 = char_width 1)
    },
    {
      { { wchar_t(UniChar::FullBlock),  L'\0', L'\0', L'\0', L'\0' } },  // █
      { { L'\0', L'\0', L'\0', L'\0', L'\0' } },
      { { wc_shadow.bg, FColor::Default } },
      { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3 (byte 2 = 0x08 = char_width 1)
    },
    {
      { { L' ',  L'\0', L'\0', L'\0', L'\0' } },  // ' '
      { { L'\0', L'\0', L'\0', L'\0', L'\0' } },
      { { FColor::Default, FColor::Default } },
      { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3 (byte 2 = 0x08 = char_width 1)
    },
    {
      { { wchar_t(UniChar::UpperHalfBlock),  L'\0', L'\0', L'\0', L'\0' } },  // ▄
      { { L'\0', L'\0', L'\0', L'\0', L'\0' } },
      { { wc_shadow.bg, FColor::Default } },
      { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3 (byte 2 = 0x08 = char_width 1)
    }
  }};

  if ( w->isWindowWidget() )
  {
    shadow_char[0].attr.bit.inherit_background = true;
    shadow_char[1].attr.bit.inherit_background = true;
    shadow_char[2].attr.bit.transparent = true;
    shadow_char[3].attr.bit.inherit_background = true;
  }
  else if ( auto p = w->getParentWidget() )
  {
    shadow_char[0].color.pair.bg = p->getBackgroundColor();
    shadow_char[1].color.pair.bg = p->getBackgroundColor();
    shadow_char[2].color.pair.bg = p->getBackgroundColor();
    shadow_char[3].color.pair.bg = p->getBackgroundColor();
  }

  drawGenericBlockShadow (w, shadow_char);
}

//----------------------------------------------------------------------
void clearBlockShadow (FWidget* w)
{
  if ( ! w || ! FVTerm::getFOutput()->hasShadowCharacter() )
    return;

  FChar spacer_char
  {
    { { L' ',  L'\0', L'\0', L'\0', L'\0' } },  // ' '
    { { L'\0', L'\0', L'\0', L'\0', L'\0' } },
    { { FColor::Default, FColor::Default } },
    { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3 (byte 2 = 0x08 = char_width 1)
  };

  if ( w->isWindowWidget() )
    spacer_char.attr.bit.transparent = true;
  else if ( auto p = w->getParentWidget() )
    spacer_char.color.pair.bg = p->getBackgroundColor();

  const std::array<FChar, 4> shadow_char
  {
    {spacer_char, spacer_char, spacer_char, spacer_char}
  };

  drawGenericBlockShadow (w, shadow_char);
}

//----------------------------------------------------------------------
struct GenericBlockShadowData
{
  // Constructor
  GenericBlockShadowData (FWidget* w, const std::array<FChar, 4>& chars)
    : shadow_char{chars}
    , area{*w->getPrintArea()}
    , is_window{w->isWindowWidget()}
    , width{is_window ? uInt(area.size.width) : uInt(w->getWidth())}
    , height{is_window ? uInt(area.size.height) : uInt(w->getHeight())}
    , shadow_width{uInt(area.shadow.width)}
    , shadow_height{uInt(area.shadow.height)}
    , x_offset{uInt(w->woffset.getX1() + w->getX() - area.position.x - 1)}
    , y_offset{uInt(w->woffset.getY1() + w->getY() - area.position.y - 1)}
    , shadow_x{x_offset + width}
    , shadow_y{y_offset + height}
  { }

  inline void updateChanges (uInt y, uInt xmin, uInt xmax, uInt tc = 0)
  {
    auto& changes = area.changes[y];
    changes.xmin = std::min(changes.xmin, xmin);
    changes.xmax = std::max(changes.xmax, xmax);
    changes.trans_count += tc + 1;
  }

  // Data members
  const std::array<FChar, 4>& shadow_char{};
  FVTerm::FTermArea& area;
  const bool is_window{false};
  const uInt width{0};
  const uInt height{0};
  const uInt shadow_width{0};
  const uInt shadow_height{0};
  const uInt x_offset{0};
  const uInt y_offset{0};
  const uInt shadow_x{0};  // Shadow column (right)
  const uInt shadow_y{0};  // Shadow row (bottom)
};

//----------------------------------------------------------------------
void drawGenericBlockShadow ( FWidget* w
                            , const std::array<FChar, 4>& shadow_char )
{
  if ( ! w
    || ! w->getPrintArea()
    || ! FVTerm::getFOutput()->hasShadowCharacter() )
    return;

  GenericBlockShadowData bsd(w, shadow_char);

  if ( bsd.is_window && (bsd.shadow_width < 1 || bsd.shadow_height < 1) )
    return;


  // Draw the top-right shadow
  auto* area_ptr = &bsd.area.getFChar(int(bsd.shadow_x), int(bsd.y_offset));
  *area_ptr = bsd.shadow_char[0];  // ▄ (top-right corner)
  bsd.updateChanges(bsd.y_offset, bsd.shadow_x, bsd.shadow_x);

  // Draw the right-side shadow
  for (uInt y = bsd.y_offset + 1; y < bsd.shadow_y; y++)
  {
    area_ptr = &bsd.area.getFChar(int(bsd.shadow_x), int(y));
    *area_ptr = bsd.shadow_char[1];  // █
    bsd.updateChanges(y, bsd.shadow_x, bsd.shadow_x);
  }

  // Draw the bottom shadow
  area_ptr = &bsd.area.getFChar(int(bsd.x_offset), int(bsd.shadow_y));
  *area_ptr = bsd.shadow_char[2];  // ' ' (bottom-left corner)
  ++area_ptr;
  // Fill the horizontal shadow
  std::fill (area_ptr, area_ptr + bsd.width, bsd.shadow_char[3]);  // ▀
  bsd.updateChanges(bsd.shadow_y, bsd.x_offset, bsd.shadow_x, bsd.width);
}

//----------------------------------------------------------------------
inline void setWidgetColors (const FWidget* w)
{
  const auto& wc_dialog = FWidget::getColorTheme()->dialog;

  if ( auto p = w->getParentWidget() )
    w->setColor (wc_dialog.fg, p->getBackgroundColor());
  else
    w->setColor (wc_dialog.fg, wc_dialog.bg);
}

//----------------------------------------------------------------------
inline void drawVerticalFlatBorder (FWidget* w)
{
  const std::size_t width = w->getWidth();
  const std::size_t height = w->getHeight();

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
}

//----------------------------------------------------------------------
inline void drawHorizontalFlatBorder (FWidget* w)
{
  const std::size_t width = w->getWidth();
  const std::size_t height = w->getHeight();
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
void drawFlatBorder (FWidget* w)
{
  if ( ! FVTerm::getFOutput()->isNewFont() )
    return;

  setWidgetColors(w);
  drawVerticalFlatBorder(w);
  drawHorizontalFlatBorder(w);
}

//----------------------------------------------------------------------
inline void clearVerticalFlatBorder (FWidget* w)
{
  const std::size_t width = w->getWidth();
  const std::size_t height = w->getHeight();

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
}

//----------------------------------------------------------------------
inline void clearHorizontalFlatBorder (FWidget* w)
{
  const std::size_t width = w->getWidth();
  const std::size_t height = w->getHeight();

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
void clearFlatBorder (FWidget* w)
{
  if ( ! FVTerm::getFOutput()->isNewFont() )
    return;

  setWidgetColors(w);
  clearVerticalFlatBorder(w);
  clearHorizontalFlatBorder(w);
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
struct GenericBoxData
{
  // Constructor
  GenericBoxData ( FWidget* w, const FRect& r
                 , const std::array<wchar_t, 8>& chars )
    : box_char{chars}
    , area{*w->getPrintArea()}
    , x_offset{w->woffset.getX1() + w->getX() - area.position.x - 1}
    , y_offset{w->woffset.getY1() + w->getY() - area.position.y - 1}
    , x1{int(x_offset) + r.getX1() - 1}
    , y1{int(y_offset) + r.getY1() - 1}
    , x2{int(x_offset) + r.getX2() - 1}
    , y2{int(y_offset) + r.getY2() - 1}
    , max_width{uInt(area.size.width + area.shadow.width - 1)}
    , width{uInt(r.getWidth())}
    , line_length{width - 2}
    , is_transparent{(fchar.attr.byte[1] & internal::var::b1_print_trans_mask) != 0}
    , trans_count_increment{uInt(is_transparent) * width}
  {
    // Prepare the first character to draw the box
    fchar.attr.bit.char_width = 1;
    fchar.ch[1] = L'\0';
  }

  inline void updateChanges (uInt y, uInt xmin, uInt xmax)
  {
    auto& changes = area.changes[y];
    changes.xmin = std::min(changes.xmin, xmin);
    changes.xmax = std::min(max_width, std::max(changes.xmax, xmax));
    changes.trans_count += trans_count_increment;
  }

  // Data members
  const std::array<wchar_t, 8>& box_char{};
  FChar& fchar{FVTermAttribute::getAttribute()};
  FVTerm::FTermArea& area;
  const int  x_offset{0};
  const int  y_offset{0};
  const int  x1{0};
  const int  y1{0};
  const int  x2{-1};
  const int  y2{-1};
  const uInt max_width{0};
  const uInt width{0};
  const uInt line_length{0};
  const bool is_transparent{false};
  const uInt trans_count_increment{0};
};

//----------------------------------------------------------------------
inline void drawBoxTopLine (GenericBoxData& bd)
{
  // Draw the horizontal top line of the box

  auto* area_ptr = &bd.area.getFChar(bd.x1, bd.y1);

  bd.fchar.ch[0] = bd.box_char[0];  // Upper left corner
  *area_ptr = bd.fchar;

  bd.fchar.ch[0] = bd.box_char[1];  // Upper line
  area_ptr = std::fill_n(area_ptr + 1, bd.line_length, bd.fchar);

  bd.fchar.ch[0] = bd.box_char[2];  // Upper right corner
  *area_ptr = bd.fchar;

  // Update area_changes for the top line
  bd.updateChanges(uInt(bd.y1), uInt(bd.x1), uInt(bd.x2));
}

//----------------------------------------------------------------------
inline void drawBoxSides (GenericBoxData& bd)
{
  // Draw the vertical sides of the box

  for (auto y = bd.y1 + 1; y < bd.y2; y++)
  {
    auto* left = &bd.area.getFChar(bd.x1, y);
    auto* right = left + bd.width - 1;

    bd.fchar.ch[0] = bd.box_char[3];  // Left line
    *left = bd.fchar;

    bd.fchar.ch[0] = bd.box_char[4];  // Right line
    *right = bd.fchar;

    // Update area_changes for the sides
    bd.updateChanges(uInt(y), uInt(bd.x1), uInt(bd.x2));
  }
}

//----------------------------------------------------------------------
inline void drawBoxBottomLine (GenericBoxData& bd)
{
  // Draw the horizontal bottom line of the box

  auto* area_ptr = &bd.area.getFChar(bd.x1, bd.y2);

  bd.fchar.ch[0] = bd.box_char[5];  // Upper left corner
  *area_ptr = bd.fchar;

  bd.fchar.ch[0] = bd.box_char[6];  // Bottom line
  area_ptr = std::fill_n(area_ptr + 1, bd.line_length, bd.fchar);

  bd.fchar.ch[0] = bd.box_char[7];  // Bottom left corner
  *area_ptr = bd.fchar;

  // Update area_changes for the bottom line
  bd.updateChanges(uInt(bd.y2), uInt(bd.x1), uInt(bd.x2));
}

//----------------------------------------------------------------------
void drawGenericBox ( FWidget* w, const FRect& r
                    , const std::array<wchar_t, 8>& box_char )
{
  if ( ! w || ! w->getPrintArea() || r.getWidth() < 3 )
    return;

  GenericBoxData box_data(w, r, box_char);

  drawBoxTopLine (box_data);
  drawBoxSides (box_data);
  drawBoxBottomLine (box_data);

  box_data.area.has_changes = true;  // Mark area as having changes
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
