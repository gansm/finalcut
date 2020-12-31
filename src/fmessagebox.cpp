/***********************************************************************
* fmessagebox.cpp - Widget FMessageBox (a text message window)         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2020 Markus Gans                                      *
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

#include "final/fapplication.h"
#include "final/fbutton.h"
#include "final/flog.h"
#include "final/fmessagebox.h"

namespace finalcut
{

constexpr std::array<const char* const, 8> button_text =
{{
  nullptr,
  "&OK",
  "&Cancel",
  "&Yes",
  "&No",
  "&Abort",
  "&Retry",
  "&Ignore"
}};

//----------------------------------------------------------------------
// class FMessageBox
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FMessageBox::FMessageBox (FWidget* parent)
  : FDialog{parent}
  , button_digit{{ButtonType::Ok, ButtonType::Reject, ButtonType::Reject}}
{
  setTitlebarText("Message for you");
  init();
}

//----------------------------------------------------------------------
FMessageBox::FMessageBox ( const FString& caption
                         , const FString& message
                         , ButtonType button0
                         , ButtonType button1
                         , ButtonType button2
                         , FWidget* parent )
  : FDialog{parent}
  , text{message}
  , button_digit{{button0, button1, button2}}
{
  setTitlebarText(caption);
  init();
}

//----------------------------------------------------------------------
FMessageBox::~FMessageBox() noexcept = default;  // destructor


// public methods of FMessageBox
//----------------------------------------------------------------------
void FMessageBox::setHeadline (const FString& headline)
{
  headline_text.setString(headline);
  setHeight(getHeight() + 2, true);

  for (std::size_t n{0}; n < num_buttons && n < MAX_BUTTONS; n++)
    if ( button[n] )
      button[n]->setY (int(getHeight()) - 4, false);

  const std::size_t column_width = getColumnWidth(headline_text);

  if ( column_width > max_line_width )
    max_line_width = column_width;
}

//----------------------------------------------------------------------
void FMessageBox::setText (const FString& txt)
{
  text.setString(txt);
  calculateDimensions();

  if ( button[0] )
    button[0]->setY (int(getHeight()) - 4, false);

  if ( button[1] && button_digit[1] != ButtonType::Reject )
    button[1]->setY (int(getHeight()) - 4, false);

  if ( button[2] && button_digit[2] != ButtonType::Reject )
    button[2]->setY (int(getHeight()) - 4, false);

  adjustButtons();
}

//----------------------------------------------------------------------
FMessageBox::ButtonType FMessageBox::exec()
{
  result_code = ButtonType::Reject;
  show();
  return result_code;
}

// protected methods of FMessageBox
//----------------------------------------------------------------------
void FMessageBox::adjustSize()
{
  std::size_t max_width{};
  std::size_t max_height{};
  const auto& root_widget = getRootWidget();

  if ( root_widget )
  {
    max_width = root_widget->getClientWidth();
    max_height = root_widget->getClientHeight();
  }
  else
  {
    // fallback to xterm default size
    max_width = 80;
    max_height = 24;
  }

  const int x = 1 + int((max_width - getWidth()) / 2);
  const int y = 1 + int((max_height - getHeight()) / 3);
  setPos(FPoint{x, y}, false);
  FDialog::adjustSize();
}

//----------------------------------------------------------------------
void FMessageBox::done (ButtonType result)
{
  hide();
  result_code = result;
}

//----------------------------------------------------------------------
void FMessageBox::cb_processClick (ButtonType reply)
{
  done(reply);
}


// private methods of FMessageBox
//----------------------------------------------------------------------
void FMessageBox::init()
{
  calculateDimensions();

  if ( (button_digit[2] != ButtonType::Reject && button_digit[1] == ButtonType::Reject)
    || (button_digit[1] != ButtonType::Reject && button_digit[0] == ButtonType::Reject) )
  {
    button_digit[0] = button_digit[1] \
                    = button_digit[2] \
                    = ButtonType::Reject;
  }

  if ( button_digit[0] == ButtonType::Reject )
    button_digit[0] = ButtonType::Ok;

  if ( button_digit[1] == ButtonType::Reject
    && button_digit[2] == ButtonType::Reject )
    num_buttons = 1;
  else if ( button_digit[2] == ButtonType::Reject )
    num_buttons = 2;
  else
    num_buttons = 3;

  allocation();
  resizeButtons();
  adjustButtons();
  initCallbacks();
  setModal();
}

//----------------------------------------------------------------------
inline void FMessageBox::allocation()
{
  try
  {
    button[0].reset(new FButton (this));
    button[0]->setText(button_text[std::size_t(button_digit[0])]);
    button[0]->setPos(FPoint{3, int(getHeight()) - 4}, false);
    button[0]->setWidth(1, false);
    button[0]->setHeight(1, false);
    button[0]->setFocus();

    if ( button_digit[1] > ButtonType::Reject )
    {
      button[1].reset(new FButton(this));
      button[1]->setText(button_text[std::size_t(button_digit[1])]);
      button[1]->setPos(FPoint{17, int(getHeight()) - 4}, false);
      button[1]->setWidth(0, false);
      button[1]->setHeight(1, false);
    }

    if ( button_digit[2] > ButtonType::Reject )
    {
      button[2].reset(new FButton(this));
      button[2]->setText(button_text[std::size_t(button_digit[2])]);
      button[2]->setPos(FPoint{32, int(getHeight()) - 4}, false);
      button[2]->setWidth(0, false);
      button[2]->setHeight(1, false);
    }
  }
  catch (const std::bad_alloc&)
  {
    badAllocOutput ("FButton");
    return;
  }
}

//----------------------------------------------------------------------
inline void FMessageBox::initCallbacks()
{
  if ( button[0] && button_digit[0] != ButtonType::Reject )
  {
    button[0]->addCallback
    (
      "clicked",
      this, &FMessageBox::cb_processClick,
      button_digit[0]
    );
  }

  if ( button[1] && button_digit[1] != ButtonType::Reject )
  {
    button[1]->addCallback
    (
      "clicked",
      this, &FMessageBox::cb_processClick,
      button_digit[1]
    );
  }

  if ( button[2] && button_digit[2] != ButtonType::Reject )
  {
    button[2]->addCallback
    (
      "clicked",
      this, &FMessageBox::cb_processClick,
      button_digit[2]
    );
  }
}

//----------------------------------------------------------------------
void FMessageBox::calculateDimensions()
{
  FSize size{};
  std::size_t headline_height{0};
  text_components = text.split("\n");
  max_line_width = 0;
  text_num_lines = std::size_t(text_components.size());

  if ( text_num_lines == 0 )
    return;

  if ( ! headline_text.isNull() )
    headline_height = 2;

  for (auto&& line : text_components)
  {
    const std::size_t column_width = getColumnWidth(line);

    if ( column_width > max_line_width )
      max_line_width = column_width;
  }

  size.setHeight (text_num_lines + 8 + headline_height);
  size.setWidth (max_line_width + 4);

  if ( size.getWidth() < 20 )
    size.setWidth(20);

  setSize(size);
}

//----------------------------------------------------------------------
void FMessageBox::draw()
{
  FDialog::draw();

  int y{0};
  int head_offset{0};
  int center_x{0};
  // center the whole block
  const auto msg_x = int((getWidth() - max_line_width) / 2);

  if ( FTerm::isMonochron() )
    setReverse(true);

  if ( ! headline_text.isEmpty() )
  {
    setColor(emphasis_color, getBackgroundColor());
    const std::size_t headline_width = getColumnWidth(headline_text);

    if ( center_text )  // center one line
      center_x = int((max_line_width - headline_width) / 2);

    print() << FPoint{1 + msg_x + center_x, 4} << headline_text;
    head_offset = 2;
  }

  setColor();

  for (auto&& line : text_components)
  {
    const std::size_t line_width = getColumnWidth(line);

    if ( center_text )  // center one line
      center_x = int((max_line_width - line_width) / 2);

    print() << FPoint{1 + msg_x + center_x, 4 + head_offset + y}
            << line;
    y++;
  }

  if ( FTerm::isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
void FMessageBox::resizeButtons() const
{
  std::array<std::size_t, MAX_BUTTONS> len{};
  std::size_t max_size{};

  for (std::size_t n{0}; n < num_buttons && n < MAX_BUTTONS; n++)
  {
    if ( ! button[n] )
      continue;

    len[n] = button[n]->getText().getLength();

    if ( button[n]->getText().includes('&') )
      len[n]--;
  }

  if ( num_buttons == 1 )
    max_size = len[0];
  else
  {
    assert ( num_buttons > 1 );
    max_size = std::max(len[0], len[1]);

    if ( num_buttons == 3 )
      max_size = std::max(max_size, len[2]);
  }

  if ( max_size < 7 )
    max_size = 7;

  for (std::size_t n{0}; n < num_buttons && n < MAX_BUTTONS; n++)
    if ( button[n] )
      button[n]->setWidth(max_size + 3, false);
}

//----------------------------------------------------------------------
void FMessageBox::adjustButtons()
{
  static constexpr std::size_t gap = 4;
  std::size_t btn_width{0};

  for (std::size_t n{0}; n < num_buttons && n < MAX_BUTTONS; n++)
  {
    if ( ! button[n] )
      continue;

    if ( n == num_buttons - 1 )
      btn_width += button[n]->getWidth();
    else
      btn_width += button[n]->getWidth() + gap;
  }

  if ( btn_width + 4 >= getWidth() )
  {
    std::size_t max_width;
    const auto& root_widget = getRootWidget();
    setWidth(btn_width + 5);
    max_width = ( root_widget ) ? root_widget->getClientWidth() : 80;
    setX (int((max_width - getWidth()) / 2));
  }

  const auto btn_x = int((getWidth() - btn_width) / 2);

  for (std::size_t n{0}; n < num_buttons && n < MAX_BUTTONS; n++)
  {
    if ( ! button[n] )
      continue;

    if ( n == 0 )
      button[n]->setX(btn_x);
    else
    {
      const auto btn_size = int(button[n]->getWidth());
      button[n]->setX(btn_x + int(n) * (btn_size + int(gap)));
    }
  }
}

}  // namespace finalcut
