/***********************************************************************
* widget-colors.cpp - Shows the internal predefined widget colors      *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2024 Markus Gans                                           *
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

#include <final/final.h>

using finalcut::FColor;
using finalcut::FColorPair;
using finalcut::FPoint;
using finalcut::FRect;
using finalcut::FSize;
using finalcut::FStyle;
using FTextHighlight = finalcut::FTextView::FTextHighlight;

//----------------------------------------------------------------------
// class TextWindow
//----------------------------------------------------------------------

class TextWindow final : public finalcut::FDialog
{
  public:
    // Constructor
    explicit TextWindow (finalcut::FWidget* = nullptr);

  private:
    // Using-declarations
    using MatchList = std::vector<std::size_t>;
    using StringPos = std::wstring::size_type;

    // Method
    void initLayout() override;
    void adjustSize() override;
    void addContent();
    void addTerminalColors (const finalcut::FWidgetColors&);
    void addDialogColors (const finalcut::FWidgetColors&);
    void addTextColors (const finalcut::FWidgetColors&);
    void addErrorBoxColors (const finalcut::FWidgetColors&);
    void addTooltipColors (const finalcut::FWidgetColors&);
    void addShadowColors (const finalcut::FWidgetColors&);
    void addCurrentElementColors (const finalcut::FWidgetColors&);
    void addListColors (const finalcut::FWidgetColors&);
    void addLabelColors (const finalcut::FWidgetColors&);
    void addInputFieldColors (const finalcut::FWidgetColors&);
    void addToggleButtonColors (const finalcut::FWidgetColors&);
    void addButtonColors (const finalcut::FWidgetColors&);
    void addTitlebarColors (const finalcut::FWidgetColors&);
    void addMenuColors (const finalcut::FWidgetColors&);
    void addStatusbarColors (const finalcut::FWidgetColors&);
    void addScrollbarColors (const finalcut::FWidgetColors&);
    void addProgressbarColors (const finalcut::FWidgetColors&);
    void appendBlankLine();
    void appendGroupName (const finalcut::FString&);
    void appendColorEntry (finalcut::FColor, const finalcut::FString&);
    template <typename T, typename... Args>
    void appendColorLine (T&&, Args...);

    // Event handlers
    void onAccel (finalcut::FAccelEvent*) override;
    void onClose (finalcut::FCloseEvent*) override;

    // Data members
    finalcut::FTextView scrolltext{this};
};

//----------------------------------------------------------------------
TextWindow::TextWindow (finalcut::FWidget* parent)
  : finalcut::FDialog{parent}
{
  setResizeable();
  scrolltext.ignorePadding();
  scrolltext.setFocus();
  addContent();
}

//----------------------------------------------------------------------
void TextWindow::initLayout()
{
  FDialog::setText("Predefined widget colors");
  setMinimumSize (FSize{42, 6});
  auto x = 1 + int((getDesktopWidth() - 42) / 2);
  auto y = int(getDesktopHeight() / 11);
  auto window_size = FSize{42, getDesktopHeight() * 7 / 8};
  FDialog::setGeometry(FPoint{x, y}, window_size);
  scrolltext.setGeometry (FPoint{1, 2}, FSize{getWidth(), getHeight() - 1});
  FDialog::initLayout();
}

//----------------------------------------------------------------------
void TextWindow::adjustSize()
{
  finalcut::FDialog::adjustSize();

  if ( ! isZoomed() )
  {
    auto desktop_size = FSize{getDesktopWidth(), getDesktopHeight()};
    FRect screen(FPoint{1, 1}, desktop_size);
    bool center = false;

    if ( getWidth() > getDesktopWidth() )
    {
      setWidth(getDesktopWidth() * 9 / 10);
      center = true;
    }

    if ( getHeight() > getDesktopHeight() )
    {
      setHeight(getDesktopHeight() * 7 / 8);
      center = true;
    }

    // Centering the window when it is no longer
    // in the visible area of the terminal
    if ( ! screen.contains(getPos()) || center )
    {
      int x = 1 + int((getDesktopWidth() - getWidth()) / 2);
      int y = 1 + int((getDesktopHeight() - getHeight()) / 2);
      setPos(FPoint{x, y});
    }
  }

  scrolltext.setGeometry (FPoint{1, 2}, FSize(getWidth(), getHeight() - 1));
}

//----------------------------------------------------------------------
void TextWindow::addContent()
{
  const auto& widget_colors = getColorTheme();
  addTerminalColors (*widget_colors);
  addDialogColors (*widget_colors);
  addTextColors (*widget_colors);
  addErrorBoxColors (*widget_colors);
  addTooltipColors (*widget_colors);
  addShadowColors (*widget_colors);
  addCurrentElementColors (*widget_colors);
  addListColors (*widget_colors);
  addLabelColors (*widget_colors);
  addInputFieldColors (*widget_colors);
  addToggleButtonColors (*widget_colors);
  addButtonColors (*widget_colors);
  addTitlebarColors (*widget_colors);
  addMenuColors (*widget_colors);
  addStatusbarColors (*widget_colors);
  addScrollbarColors (*widget_colors);
  addProgressbarColors (*widget_colors);
}

//----------------------------------------------------------------------
void TextWindow::addTerminalColors (const finalcut::FWidgetColors& widget_colors)
{
  appendGroupName (L"Terminal");
  appendBlankLine();
  appendColorEntry (widget_colors.term.fg, L"term.fg");
  appendColorEntry (widget_colors.term.bg, L"term.bg");
  appendBlankLine();

  auto term_color = FColorPair{widget_colors.term.fg, widget_colors.term.bg};
  appendColorLine("  Text Text Text ", FTextHighlight{1, 16, term_color});
  appendBlankLine();
}

//----------------------------------------------------------------------
void TextWindow::addDialogColors (const finalcut::FWidgetColors& widget_colors)
{
  appendBlankLine();
  appendGroupName (L"Dialog");
  appendBlankLine();
  appendColorEntry (widget_colors.dialog.fg, L"dialog.fg");
  appendColorEntry (widget_colors.dialog.bg, L"dialog.bg");
  appendColorEntry (widget_colors.dialog.resize_fg, L"dialog.resize_fg");
  appendColorEntry (widget_colors.dialog.emphasis_fg, L"dialog.emphasis_fg");
  appendBlankLine();

  auto dialog_color = FColorPair{widget_colors.dialog.fg, widget_colors.dialog.bg};
  auto dialog_emphasis_color = FColorPair{widget_colors.dialog.emphasis_fg, widget_colors.dialog.bg};
  auto resize_border_color = FColorPair{widget_colors.dialog.resize_fg, widget_colors.dialog.bg};

  appendColorLine(" ┌─────────────────┐  ┌────────┐"
                 , FTextHighlight{1, 19, dialog_color}
                 , FTextHighlight{22, 10, resize_border_color}
                 );
  appendColorLine(" │ Emphasized text │  │ Resize │"
                 , FTextHighlight{1, 2, dialog_color}
                 , FTextHighlight{3, 15, dialog_emphasis_color}
                 , FTextHighlight{18, 2, dialog_color}
                 , FTextHighlight{22, 1, resize_border_color}
                 , FTextHighlight{31, 1, resize_border_color}
                 );
  appendColorLine(" │   Normal text   │  │ border │"
                 , FTextHighlight{1, 19, dialog_color}
                 , FTextHighlight{22, 1, resize_border_color}
                 , FTextHighlight{31, 1, resize_border_color}
                 );
  appendColorLine(" └─────────────────┘  └────────┘"
                 , FTextHighlight{1, 19, dialog_color}
                 , FTextHighlight{22, 10, resize_border_color}
                 );
  appendBlankLine();
}

//----------------------------------------------------------------------
void TextWindow::addTextColors (const finalcut::FWidgetColors& widget_colors)
{
  appendBlankLine();
  appendGroupName (L"Text");
  appendBlankLine();
  appendColorEntry (widget_colors.text.fg, L"text.fg");
  appendColorEntry (widget_colors.text.bg, L"text.bg");
  appendColorEntry (widget_colors.text.selected_fg, L"text.selected_fg");
  appendColorEntry (widget_colors.text.selected_bg, L"text.selected_bg");
  appendColorEntry (widget_colors.text.selected_focus_fg, L"text.selected_focus_fg");
  appendColorEntry (widget_colors.text.selected_focus_bg, L"text.selected_focus_bg");
  appendBlankLine();

  auto text_color = FColorPair{widget_colors.text.fg, widget_colors.text.bg};
  auto selected_color = FColorPair{widget_colors.text.selected_fg, widget_colors.text.selected_bg};
  auto selected_focus_color = FColorPair{widget_colors.text.selected_focus_fg, widget_colors.text.selected_focus_bg};

  appendColorLine("  Item         Item      "
                 , FTextHighlight{1, 19, text_color}
                 );
  appendColorLine("  Selected     Selected and focused "
                 , FTextHighlight{1, 10, selected_color}
                 , FTextHighlight{14, 22, selected_focus_color}
                 );
  appendColorLine("  Next item    Another item"
                 , FTextHighlight{1, 19, text_color}
                 );

  appendBlankLine();
}

//----------------------------------------------------------------------
void TextWindow::addErrorBoxColors (const finalcut::FWidgetColors& widget_colors)
{
  appendBlankLine();
  appendGroupName (L"Error box");
  appendBlankLine();
  appendColorEntry (widget_colors.error_box.fg, L"error_box.fg");
  appendColorEntry (widget_colors.error_box.bg, L"error_box.bg");
  appendColorEntry (widget_colors.error_box.emphasis_fg, L"error_box.emphasis_fg");
  appendBlankLine();

  auto error_box_color = FColorPair{widget_colors.error_box.fg, widget_colors.error_box.bg};
  auto error_box_emphasis_color = FColorPair{widget_colors.error_box.emphasis_fg, widget_colors.error_box.bg};

  appendColorLine(" ┌─────────────────┐"
                 , FTextHighlight{1, 19, error_box_color}
                 );
  appendColorLine(" │ Emphasized text │"
                 , FTextHighlight{1, 2, error_box_color}
                 , FTextHighlight{3, 15, error_box_emphasis_color}
                 , FTextHighlight{18, 2, error_box_color}
                 );
  appendColorLine(" │   Normal text   │"
                 , FTextHighlight{1, 19, error_box_color}
                 );
  appendColorLine(" └─────────────────┘"
                 , FTextHighlight{1, 19, error_box_color}
                 );
  appendBlankLine();
}

//----------------------------------------------------------------------
void TextWindow::addTooltipColors (const finalcut::FWidgetColors& widget_colors)
{
  appendBlankLine();
  appendGroupName (L"Tooltip");
  appendBlankLine();
  appendColorEntry (widget_colors.tooltip.fg, L"tooltip.fg");
  appendColorEntry (widget_colors.tooltip.bg, L"tooltip.bg");
  appendBlankLine();

  auto toolkit_color = FColorPair{widget_colors.tooltip.fg, widget_colors.tooltip.bg};

  appendColorLine(" ┌─────────────────┐"
                 , FTextHighlight{1, 19, toolkit_color}
                 );
  appendColorLine(" │   Tooltip text  │"
                 , FTextHighlight{1, 19, toolkit_color}
                 );
  appendColorLine(" └─────────────────┘"
                 , FTextHighlight{1, 19, toolkit_color}
                 );
  appendBlankLine();
}

//----------------------------------------------------------------------
void TextWindow::addShadowColors (const finalcut::FWidgetColors& widget_colors)
{
  appendBlankLine();
  appendGroupName (L"Shadow");
  appendBlankLine();
  appendColorEntry (widget_colors.shadow.fg, L"shadow.fg");
  appendColorEntry (widget_colors.shadow.bg, L"shadow.bg");
  appendBlankLine();

  auto shadow_color = FColorPair{widget_colors.shadow.fg, widget_colors.shadow.bg};
  appendColorLine("     │text"
                 , FTextHighlight{6, 2, shadow_color}
                 );
  appendColorLine(" ────┘text"
                 , FTextHighlight{6, 2, shadow_color}
                 );
  appendColorLine(" text text"
                 , FTextHighlight{1, 7, shadow_color}
                 );
  appendBlankLine();
}

//----------------------------------------------------------------------
void TextWindow::addCurrentElementColors (const finalcut::FWidgetColors& widget_colors)
{
  appendBlankLine();
  appendGroupName (L"Current element");
  appendBlankLine();
  appendColorEntry (widget_colors.current_element.fg, L"current_element.fg");
  appendColorEntry (widget_colors.current_element.bg, L"current_element.bg");
  appendColorEntry (widget_colors.current_element.focus_fg, L"current_element.focus_fg");
  appendColorEntry (widget_colors.current_element.focus_bg, L"current_element.focus_bg");
  appendColorEntry (widget_colors.current_element.inc_search_fg, L"current_element.inc_search_fg");
  appendColorEntry (widget_colors.current_element.selected_fg, L"current_element.selected_fg");
  appendColorEntry (widget_colors.current_element.selected_bg, L"current_element.selected_bg");
  appendColorEntry (widget_colors.current_element.selected_focus_fg, L"current_element.selected_focus_fg");
  appendColorEntry (widget_colors.current_element.selected_focus_bg, L"current_element.selected_focus_bg");
  appendBlankLine();

  auto widget_color = FColorPair{getForegroundColor(), getBackgroundColor()};
  auto current_element_color = FColorPair{widget_colors.current_element.fg, widget_colors.current_element.bg};
  auto focus_current_element_color = FColorPair{widget_colors.current_element.focus_fg, widget_colors.current_element.focus_bg};
  auto selected_widget_color = FColorPair{widget_colors.list.selected_fg, getBackgroundColor()};
  auto selected_current_element_color = FColorPair{widget_colors.current_element.selected_fg, widget_colors.current_element.selected_bg};
  auto selected_focus_current_element_color = FColorPair{widget_colors.current_element.selected_focus_fg, widget_colors.current_element.selected_focus_bg};
  auto inc_search_current_element_color = FColorPair{widget_colors.current_element.inc_search_fg, widget_colors.current_element.focus_bg};

  appendColorLine("  Element         Element "
                 , FTextHighlight{1, 13, widget_color}
                 , FTextHighlight{17, 13, widget_color}
                 );
  appendColorLine("  Not focused     Focused "
                 , FTextHighlight{1, 13, current_element_color}
                 , FTextHighlight{17, 13, focus_current_element_color}
                 );
  appendColorLine("  Selected        Selected "
                 , FTextHighlight{1, 13, selected_widget_color}
                 , FTextHighlight{17, 13, selected_widget_color}
                 );
  appendColorLine("  Not focused     Focused "
                 , FTextHighlight{1, 13, selected_current_element_color}
                 , FTextHighlight{17, 13, selected_focus_current_element_color}
                 );
  appendColorLine("  Element     "
                 , FTextHighlight{1, 13, widget_color}
                 , FTextHighlight{17, 13, widget_color}
                 );
  appendColorLine("  Searching   "
                 , FTextHighlight{1, 1, focus_current_element_color}
                 , FTextHighlight{2, 5, inc_search_current_element_color}
                 , FTextHighlight{7, 6, focus_current_element_color}
                 );
  appendBlankLine();
}

//----------------------------------------------------------------------
void TextWindow::addListColors (const finalcut::FWidgetColors& widget_colors)
{
  appendBlankLine();
  appendGroupName (L"List");
  appendBlankLine();
  appendColorEntry (widget_colors.list.fg, L"list.fg");
  appendColorEntry (widget_colors.list.bg, L"list.bg");
  appendColorEntry (widget_colors.list.selected_fg, L"list.selected_fg");
  appendColorEntry (widget_colors.list.selected_bg, L"list.selected_bg");
  appendBlankLine();

  auto list_color = FColorPair{widget_colors.list.fg, widget_colors.list.bg};
  auto selected_list_color = FColorPair{widget_colors.list.selected_fg, widget_colors.list.selected_bg};
  appendColorLine("  Item #1     "
                 , FTextHighlight{1, 13, list_color}
                 );
  appendColorLine("  Item #2     "
                 , FTextHighlight{1, 13, selected_list_color}
                 );
  appendBlankLine();
}

//----------------------------------------------------------------------
void TextWindow::addLabelColors (const finalcut::FWidgetColors& widget_colors)
{
  appendBlankLine();
  appendGroupName (L"Label");
  appendBlankLine();
  appendColorEntry (widget_colors.label.fg, L"label.fg");
  appendColorEntry (widget_colors.label.bg, L"label.bg");
  appendColorEntry (widget_colors.label.inactive_fg, L"label.inactive_fg");
  appendColorEntry (widget_colors.label.inactive_bg, L"label.inactive_bg");
  appendColorEntry (widget_colors.label.hotkey_fg, L"label.hotkey_fg");
  appendColorEntry (widget_colors.label.hotkey_bg, L"label.hotkey_bg");
  appendColorEntry (widget_colors.label.emphasis_fg, L"label.emphasis_fg");
  appendColorEntry (widget_colors.label.ellipsis_fg, L"label.ellipsis_fg");
  appendBlankLine();

  auto label_color = FColorPair{widget_colors.label.fg, widget_colors.label.bg};
  auto inactive_label_color = FColorPair{widget_colors.label.inactive_fg, widget_colors.label.inactive_bg};
  auto hotkey_label_color = FColorPair{widget_colors.label.hotkey_fg, widget_colors.label.hotkey_bg};
  auto emphasis_label_color = FColorPair{widget_colors.label.emphasis_fg, widget_colors.label.bg};
  auto ellipsis_label_color = FColorPair{widget_colors.label.ellipsis_fg, widget_colors.label.bg};

  appendColorLine(" Label"
                 , FTextHighlight{1, 5, label_color}
                 );
  appendColorLine(" Disabled"
                 , FTextHighlight{1, 8, inactive_label_color}
                 );
  appendColorLine(" Hotkey"
                 , FTextHighlight{1, 1, hotkey_label_color}
                 , FTextHighlight{2, 4, label_color}
                 );
  appendColorLine(" Emphasized"
                 , FTextHighlight{1, 10, emphasis_label_color}
                 );
  appendColorLine(" Ellips.."
                 , FTextHighlight{1, 6, label_color}
                 , FTextHighlight{7, 2, ellipsis_label_color}
                 );
  appendBlankLine();
}

//----------------------------------------------------------------------
void TextWindow::addInputFieldColors (const finalcut::FWidgetColors& widget_colors)
{
  appendBlankLine();
  appendGroupName (L"Input field");
  appendBlankLine();
  appendColorEntry (widget_colors.input_field.fg, L"input_field.fg");
  appendColorEntry (widget_colors.input_field.bg, L"input_field.bg");
  appendColorEntry (widget_colors.input_field.focus_fg, L"input_field.focus_fg");
  appendColorEntry (widget_colors.input_field.focus_bg, L"input_field.focus_bg");
  appendColorEntry (widget_colors.input_field.inactive_fg, L"input_field.inactive_fg");
  appendColorEntry (widget_colors.input_field.inactive_bg, L"input_field.inactive_bg");
  appendBlankLine();

  auto input_field_color = FColorPair{widget_colors.input_field.fg, widget_colors.input_field.bg};
  auto focus_input_field_color = FColorPair{widget_colors.input_field.focus_fg, widget_colors.input_field.focus_bg};
  auto inactive_input_field_color = FColorPair{widget_colors.input_field.inactive_fg, widget_colors.input_field.inactive_bg};

  appendColorLine("  Text input     "
                 , FTextHighlight{1, 16, input_field_color}
                 );
  appendBlankLine();
  appendColorLine("  Focus input    "
                 , FTextHighlight{1, 16, focus_input_field_color}
                 );
  appendBlankLine();
  appendColorLine("  Disabled input "
                 , FTextHighlight{1, 16, inactive_input_field_color}
                 );
  appendBlankLine();
}

//----------------------------------------------------------------------
void TextWindow::addToggleButtonColors (const finalcut::FWidgetColors& widget_colors)
{
  appendBlankLine();
  appendGroupName (L"Toggle button");
  appendBlankLine();
  appendColorEntry (widget_colors.toggle_button.fg, L"toggle_button.fg");
  appendColorEntry (widget_colors.toggle_button.bg, L"toggle_button.bg");
  appendColorEntry (widget_colors.toggle_button.focus_fg, L"toggle_button.focus_fg");
  appendColorEntry (widget_colors.toggle_button.focus_bg, L"toggle_button.focus_bg");
  appendColorEntry (widget_colors.toggle_button.inactive_fg, L"toggle_button.inactive_fg");
  appendColorEntry (widget_colors.toggle_button.inactive_bg, L"toggle_button.inactive_bg");
  appendBlankLine();

  auto label_color = FColorPair{widget_colors.label.fg, widget_colors.label.bg};
  auto toggle_button_color = FColorPair{widget_colors.toggle_button.fg, widget_colors.toggle_button.bg};
  auto focus_toggle_button_color = FColorPair{widget_colors.toggle_button.focus_fg, widget_colors.toggle_button.focus_bg};
  auto inactive_toggle_button_color = FColorPair{widget_colors.toggle_button.inactive_fg, widget_colors.toggle_button.inactive_bg};

  appendColorLine(" [ ] Check button"
                 , FTextHighlight{1, 3, toggle_button_color}
                 , FTextHighlight{5, FTextHighlight::EOL, label_color}
                 );
  appendColorLine(" [x] Focused"
                 , FTextHighlight{1, 3, focus_toggle_button_color}
                 , FTextHighlight{5, FTextHighlight::EOL, label_color}
                 );
  appendColorLine(" [ ] Disabled"
                 , FTextHighlight{1, 3, inactive_toggle_button_color}
                 , FTextHighlight{5, FTextHighlight::EOL, label_color}
                 );
  appendBlankLine();
}

//----------------------------------------------------------------------
void TextWindow::addButtonColors (const finalcut::FWidgetColors& widget_colors)
{
  appendBlankLine();
  appendGroupName (L"Button");
  appendBlankLine();
  appendColorEntry (widget_colors.button.fg, L"button.fg");
  appendColorEntry (widget_colors.button.bg, L"button.bg");
  appendColorEntry (widget_colors.button.focus_fg, L"button.focus_fg");
  appendColorEntry (widget_colors.button.focus_bg, L"button.focus_bg");
  appendColorEntry (widget_colors.button.inactive_fg, L"button.inactive_fg");
  appendColorEntry (widget_colors.button.inactive_bg, L"button.inactive_bg");
  appendColorEntry (widget_colors.button.hotkey_fg, L"button.hotkey_fg");
  appendBlankLine();

  auto button_color = FColorPair{widget_colors.button.fg, widget_colors.button.bg};
  auto focus_button_color = FColorPair{widget_colors.button.focus_fg, widget_colors.button.focus_bg};
  auto inactive_button_color = FColorPair{widget_colors.button.inactive_fg, widget_colors.button.inactive_bg};
  auto hotkey_button_color = FColorPair{widget_colors.button.hotkey_fg, widget_colors.button.bg};

  appendColorLine("   Button      Focused  "
                 , FTextHighlight{1, 10, button_color}
                 , FTextHighlight{13, 11, focus_button_color}
                 );
  appendBlankLine();
  appendColorLine("  Disabled     Hotkey   "
                 , FTextHighlight{1, 10, inactive_button_color}
                 , FTextHighlight{13, 2, button_color}
                 , FTextHighlight{15, 1, hotkey_button_color}
                 , FTextHighlight{16, 8, button_color}
                 );
  appendBlankLine();
}

//----------------------------------------------------------------------
void TextWindow::addTitlebarColors (const finalcut::FWidgetColors& widget_colors)
{
  appendBlankLine();
  appendGroupName (L"Titlebar");
  appendBlankLine();
  appendColorEntry (widget_colors.titlebar.fg, L"titlebar.fg");
  appendColorEntry (widget_colors.titlebar.bg, L"titlebar.bg");
  appendColorEntry (widget_colors.titlebar.inactive_fg, L"titlebar.inactive_fg");
  appendColorEntry (widget_colors.titlebar.inactive_bg, L"titlebar.inactive_bg");
  appendColorEntry (widget_colors.titlebar.button_fg, L"titlebar.button_fg");
  appendColorEntry (widget_colors.titlebar.button_bg, L"titlebar.button_bg");
  appendColorEntry (widget_colors.titlebar.button_focus_fg, L"titlebar.button_focus_fg");
  appendColorEntry (widget_colors.titlebar.button_focus_bg, L"titlebar.button_focus_bg");
  appendBlankLine();

  auto titlebar_color = FColorPair{widget_colors.titlebar.fg, widget_colors.titlebar.bg};
  auto titlebar_button_color = FColorPair{widget_colors.titlebar.button_fg, widget_colors.titlebar.button_bg};
  auto focus_titlebar_button_color = FColorPair{widget_colors.titlebar.button_focus_fg, widget_colors.titlebar.button_focus_bg};
  auto inactive_titlebar_color = FColorPair{widget_colors.titlebar.inactive_fg, widget_colors.titlebar.inactive_bg};

  appendColorLine("  -      Active      ▼  ▲ "
                 , FTextHighlight{1, 3, titlebar_button_color}
                 , FTextHighlight{4, 16, titlebar_color}
                 , FTextHighlight{20, 6, titlebar_button_color}
                 );
  appendBlankLine();
  appendColorLine("  -     Inactive     ▼  ▲ "
                 , FTextHighlight{1, 3, titlebar_button_color}
                 , FTextHighlight{4, 16, inactive_titlebar_color}
                 , FTextHighlight{20, 6, titlebar_button_color}
                 );
  appendBlankLine();
  appendColorLine("  -  Focused button  ▼  ▲ "
                 , FTextHighlight{1, 3, titlebar_button_color}
                 , FTextHighlight{4, 16, inactive_titlebar_color}
                 , FTextHighlight{20, 3, focus_titlebar_button_color}
                 , FTextHighlight{23, 3, titlebar_button_color}
                 );
  appendBlankLine();
}

//----------------------------------------------------------------------
void TextWindow::addMenuColors (const finalcut::FWidgetColors& widget_colors)
{
  appendBlankLine();
  appendGroupName (L"Menu");
  appendBlankLine();
  appendColorEntry (widget_colors.menu.fg, L"menu.fg");
  appendColorEntry (widget_colors.menu.bg, L"menu.bg");
  appendColorEntry (widget_colors.menu.focus_fg, L"menu.focus_fg");
  appendColorEntry (widget_colors.menu.focus_bg, L"menu.focus_bg");
  appendColorEntry (widget_colors.menu.inactive_fg, L"menu.inactive_fg");
  appendColorEntry (widget_colors.menu.inactive_bg, L"menu.inactive_bg");
  appendColorEntry (widget_colors.menu.hotkey_fg, L"menu.hotkey_fg");
  appendColorEntry (widget_colors.menu.hotkey_bg, L"menu.hotkey_bg");
  appendBlankLine();

  auto widget_color = FColorPair{getForegroundColor(), getBackgroundColor()};
  auto menu_color = FColorPair{widget_colors.menu.fg, widget_colors.menu.bg};
  auto hotkey_menu_color = FColorPair{widget_colors.menu.hotkey_fg, widget_colors.menu.hotkey_bg};
  auto focus_menu_color = FColorPair{widget_colors.menu.focus_fg, widget_colors.menu.focus_bg};
  auto inactive__menu_color = FColorPair{widget_colors.menu.inactive_fg, widget_colors.menu.inactive_bg};

  appendColorLine("  File  Options "
                 , FTextHighlight{1, 6, focus_menu_color}
                 , FTextHighlight{7, 1, menu_color}
                 , FTextHighlight{8, 1, hotkey_menu_color}
                 , FTextHighlight{9, 7, menu_color}
                 );
  appendColorLine(" ┌─────────────────┐"
                 , FTextHighlight{1, 19, widget_color}
                 );
  appendColorLine(" │ Open...  Ctrl+O │"
                 , FTextHighlight{1, 1, widget_color}
                 , FTextHighlight{2, 17, focus_menu_color}
                 , FTextHighlight{19, 1, widget_color}
                 );
  appendColorLine(" │ Disabled        │"
                 , FTextHighlight{1, 1, widget_color}
                 , FTextHighlight{2, 17, inactive__menu_color}
                 , FTextHighlight{19, 1, widget_color}
                 );
  appendColorLine(" └─────────────────┘"
                 , FTextHighlight{1, 19, widget_color}
                 );
  appendBlankLine();
}

//----------------------------------------------------------------------
void TextWindow::addStatusbarColors (const finalcut::FWidgetColors& widget_colors)
{
  appendBlankLine();
  appendGroupName (L"Statusbar");
  appendBlankLine();
  appendColorEntry (widget_colors.statusbar.fg, L"statusbar.fg");
  appendColorEntry (widget_colors.statusbar.bg, L"statusbar.bg");
  appendColorEntry (widget_colors.statusbar.focus_fg, L"statusbar.focus_fg");
  appendColorEntry (widget_colors.statusbar.focus_bg, L"statusbar.focus_bg");
  appendColorEntry (widget_colors.statusbar.separator_fg, L"statusbar.separator_fg");
  appendColorEntry (widget_colors.statusbar.hotkey_fg, L"statusbar.hotkey_fg");
  appendColorEntry (widget_colors.statusbar.hotkey_bg, L"statusbar.hotkey_bg");
  appendColorEntry (widget_colors.statusbar.focus_hotkey_fg, L"statusbar.focus_hotkey_fg");
  appendColorEntry (widget_colors.statusbar.focus_hotkey_bg, L"statusbar.focus_hotkey_bg");
  appendBlankLine();

  auto statusbar_color = FColorPair{widget_colors.statusbar.fg, widget_colors.statusbar.bg};
  auto hotkey_statusbar_color = FColorPair{widget_colors.statusbar.hotkey_fg, widget_colors.statusbar.hotkey_bg};
  auto hotkey_focus_statusbar_color = FColorPair{widget_colors.statusbar.focus_hotkey_fg, widget_colors.statusbar.focus_hotkey_bg};
  auto focus_statusbar_color = FColorPair{widget_colors.statusbar.focus_fg, widget_colors.statusbar.focus_bg};
  auto separator_statusbar_color = FColorPair{widget_colors.statusbar.separator_fg, widget_colors.statusbar.bg};

  appendColorLine("  F1-Help▌ F2-Cut▐ F3-Copy│ F4-Paste "
                 , FTextHighlight{1, 1, statusbar_color}
                 , FTextHighlight{2, 2, hotkey_statusbar_color}
                 , FTextHighlight{4, 6, statusbar_color}
                 , FTextHighlight{9, 2, focus_statusbar_color}
                 , FTextHighlight{11, 2, hotkey_focus_statusbar_color}
                 , FTextHighlight{13, 5, focus_statusbar_color}
                 , FTextHighlight{18, 1, statusbar_color}
                 , FTextHighlight{19, 2, hotkey_statusbar_color}
                 , FTextHighlight{21, 5, statusbar_color}
                 , FTextHighlight{26, 1, separator_statusbar_color}
                 , FTextHighlight{27, 1, statusbar_color}
                 , FTextHighlight{28, 2, hotkey_statusbar_color}
                 , FTextHighlight{30, 7, statusbar_color}
                 );
  appendBlankLine();
}

//----------------------------------------------------------------------
void TextWindow::addScrollbarColors (const finalcut::FWidgetColors& widget_colors)
{
  appendBlankLine();
  appendGroupName (L"Scrollbar");
  appendBlankLine();
  appendColorEntry (widget_colors.scrollbar.fg, L"scrollbar.fg");
  appendColorEntry (widget_colors.scrollbar.bg, L"scrollbar.bg");
  appendColorEntry (widget_colors.scrollbar.button_fg, L"scrollbar.button_fg");
  appendColorEntry (widget_colors.scrollbar.button_bg, L"scrollbar.button_bg");
  appendColorEntry (widget_colors.scrollbar.button_inactive_fg, L"scrollbar.button_inactive_fg");
  appendColorEntry (widget_colors.scrollbar.button_inactive_bg, L"scrollbar.button_inactive_bg");
  appendBlankLine();

  auto scrollbar_color = FColorPair{widget_colors.scrollbar.bg, widget_colors.scrollbar.bg};
  auto slider_scrollbar_color = FColorPair{widget_colors.scrollbar.fg, widget_colors.scrollbar.fg};
  auto button_scrollbar_color = FColorPair{widget_colors.scrollbar.button_fg, widget_colors.scrollbar.button_bg};
  auto inactive_button_scrollbar_color = FColorPair{widget_colors.scrollbar.button_inactive_fg, widget_colors.scrollbar.button_inactive_bg};

  appendColorLine(" ◄            ►  ▲"
                 , FTextHighlight{1, 1, button_scrollbar_color}
                 , FTextHighlight{2, 7, slider_scrollbar_color}
                 , FTextHighlight{8, 6, scrollbar_color}
                 , FTextHighlight{14, 1, button_scrollbar_color}
                 , FTextHighlight{17, 1, button_scrollbar_color}
                 );
  appendColorLine("                 █"
                 , FTextHighlight{17, 1, slider_scrollbar_color}
                 );
  appendColorLine(" ◄            ►  ▼"
                 , FTextHighlight{1, 1, inactive_button_scrollbar_color}
                 , FTextHighlight{2, 12, slider_scrollbar_color}
                 , FTextHighlight{14, 1, inactive_button_scrollbar_color}
                 , FTextHighlight{17, 1, button_scrollbar_color}
                 );
  appendBlankLine();
}

//----------------------------------------------------------------------
void TextWindow::addProgressbarColors (const finalcut::FWidgetColors& widget_colors)
{
  appendBlankLine();
  appendGroupName (L"Progressbar");
  appendBlankLine();
  appendColorEntry (widget_colors.progressbar.fg, L"progressbar.fg");
  appendColorEntry (widget_colors.progressbar.bg, L"progressbar.bg");
  appendBlankLine();

  auto Progressbar_color = FColorPair{widget_colors.progressbar.fg, widget_colors.progressbar.bg};
  appendColorLine(" ███████████       64%", FTextHighlight{1, 17, Progressbar_color});
  appendBlankLine();
}

//----------------------------------------------------------------------
void TextWindow::appendBlankLine()
{
  scrolltext.append("");
}

//----------------------------------------------------------------------
void TextWindow::appendGroupName (const finalcut::FString& group_name)
{
  auto group_name_length = group_name.getLength();
  auto last_line_index = scrolltext.getRows();
  scrolltext.append(group_name);
  FTextHighlight hgl{0, group_name_length, FStyle{finalcut::Style::Underline} };
  scrolltext.addHighlight (last_line_index, hgl);
}

//----------------------------------------------------------------------
void TextWindow::appendColorEntry (finalcut::FColor color, const finalcut::FString& item_name)
{
  auto color_pair = FColorPair{color, color};
  auto last_line_index = scrolltext.getRows();
  scrolltext.append(" ██ " + item_name);
  FTextHighlight hgl{1, 2, color_pair };
  scrolltext.addHighlight (last_line_index, hgl);
}

//----------------------------------------------------------------------
template <typename T, typename... Args>
void TextWindow::appendColorLine (T&& line, Args... args)
{
  auto last_line_index = scrolltext.getRows();
  scrolltext.append(std::forward<T>(line));

  for (const auto hgl : {args...})
  {
    scrolltext.addHighlight (last_line_index, hgl);
  }
}

//----------------------------------------------------------------------
void TextWindow::onAccel (finalcut::FAccelEvent* ev)
{
  close();
  ev->accept();
}

//----------------------------------------------------------------------
void TextWindow::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

auto main (int argc, char* argv[]) -> int
{
  // Create the application object app
  finalcut::FApplication app{argc, argv};

  // Force initialization of the terminal without calling show() so that
  // the color theme is already available at the initialization time
  app.initTerminal();

  // Create main dialog object text_window
  TextWindow text_window{&app};
  text_window.addAccelerator(finalcut::FKey('q'));

  // Show the dialog text_window
  text_window.show();

  // Set dialog text_window as main widget
  finalcut::FWidget::setMainWidget(&text_window);

  // Show and start the application
  text_window.show();
  return app.exec();
}
