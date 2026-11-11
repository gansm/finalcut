/***********************************************************************
* fdatedialog.cpp - Widget FDateDialog (a calendar date chooser)       *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2026 Wim Stockman <wim@thinkerwim.org>                     *
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
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>

#include "final/dialog/fdatedialog.h"
#include "final/fapplication.h"
#include "final/fwidget_functions.h"

namespace finalcut
{

namespace
{

constexpr std::array<const char* const, 12> month_names =
{{
  "January", "February", "March",     "April",
  "May",     "June",     "July",      "August",
  "September", "October", "November", "December"
}};

constexpr std::array<const char* const, 7> weekday_names =
{{ "Mo", "Tu", "We", "Th", "Fr", "Sa", "Su" }};

constexpr std::array<const char* const, 3> wheel_labels =
{{ "Day", "Month", "Year" }};

// Sakamoto's table for computing the weekday (0 = Sunday)
constexpr std::array<int, 12> sakamoto_table =
{{ 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 }};

// Clamp by value, so no constexpr member has to be odr-used
auto clampInt (int value, int low, int high) noexcept -> int
{
  if ( value < low )
    return low;

  if ( value > high )
    return high;

  return value;
}

}  // namespace


// constructors and destructor
//----------------------------------------------------------------------
FDateDialog::FDateDialog (FWidget* parent)
  : FDialog{parent}
{
  setText("Select date");
  init();
}

//----------------------------------------------------------------------
FDateDialog::FDateDialog (FString&& title, FWidget* parent)
  : FDialog{std::move(title), parent}
{
  init();
}

//----------------------------------------------------------------------
FDateDialog::~FDateDialog() noexcept = default;


// public methods of FDateDialog
//----------------------------------------------------------------------
auto FDateDialog::getDate() const -> FString
{
  char buffer[32]{};
  std::snprintf ( buffer, sizeof(buffer), "%04d-%02d-%02d"
                , sel_year, sel_month, sel_day );
  return FString{buffer};
}

//----------------------------------------------------------------------
auto FDateDialog::setDate (int year, int month, int day) -> bool
{
  if ( year < MIN_YEAR || year > MAX_YEAR
    || month < 1 || month > 12
    || day < 1 || day > daysInMonth(year, month) )
  {
    return false;
  }

  sel_year = year;
  sel_month = month;
  sel_day = day;
  selectionChanged();
  return true;
}

//----------------------------------------------------------------------
void FDateDialog::setLayout (Layout new_layout)
{
  if ( new_layout == layout )
    return;

  layout = new_layout;
  applyLayout();

  if ( isShown() )
    redraw();
}

//----------------------------------------------------------------------
auto FDateDialog::daysInMonth (int year, int month) noexcept -> int
{
  constexpr std::array<int, 12> days_per_month =
  {{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }};

  if ( month == 2 && isLeapYear(year) )
    return 29;

  return days_per_month[std::size_t(month - 1)];
}


// protected methods of FDateDialog
//----------------------------------------------------------------------
void FDateDialog::adjustSize()
{
  int max_width = 80;   // fallback to xterm default size
  int max_height = 24;
  const auto& root_widget = getRootWidget();

  if ( root_widget )
  {
    max_width = int(root_widget->getClientWidth());
    max_height = int(root_widget->getClientHeight());
  }

  const int x = 1 + (max_width - int(getWidth())) / 2;
  const int y = 1 + (max_height - int(getHeight())) / 3;
  FWindow::setPos (FPoint{x, y}, false);
  FDialog::adjustSize();
}

//----------------------------------------------------------------------
void FDateDialog::onKeyPress (FKeyEvent* ev)
{
  const auto key = ev->key();

  if ( isEnterKey(key) )
  {
    ev->accept();
    done (ResultCode::Accept);
    return;
  }

  if ( layout == Layout::Wheels )
  {
    if ( key == FKey::Left )
      moveWheel(-1);
    else if ( key == FKey::Right )
      moveWheel(1);
    else if ( key == FKey::Up )
      adjustWheel(1);
    else if ( key == FKey::Down )
      adjustWheel(-1);
    else
    {
      FDialog::onKeyPress(ev);
      return;
    }

    ev->accept();
    return;
  }

  if ( key == FKey::Left )
    moveDays(-1);
  else if ( key == FKey::Right )
    moveDays(1);
  else if ( key == FKey::Up )
    moveDays(-DAYS_PER_WEEK);
  else if ( key == FKey::Down )
    moveDays(DAYS_PER_WEEK);
  else if ( key == FKey::Page_up )
    moveMonths(-1);
  else if ( key == FKey::Page_down )
    moveMonths(1);
  else
  {
    FDialog::onKeyPress(ev);
    return;
  }

  ev->accept();
}

//----------------------------------------------------------------------
void FDateDialog::onMouseDown (FMouseEvent* ev)
{
  if ( layout == Layout::Calendar )
  {
    const int day = dayAt(ev->getX(), ev->getY());

    if ( day > 0 )
    {
      if ( day != sel_day )
      {
        sel_day = day;
        selectionChanged();
      }

      return;
    }
  }

  FDialog::onMouseDown(ev);
}

//----------------------------------------------------------------------
void FDateDialog::onMouseDoubleClick (FMouseEvent* ev)
{
  if ( layout == Layout::Calendar )
  {
    const int day = dayAt(ev->getX(), ev->getY());

    if ( day > 0 )
    {
      sel_day = day;
      done (ResultCode::Accept);
      return;
    }
  }

  FDialog::onMouseDoubleClick(ev);
}

//----------------------------------------------------------------------
void FDateDialog::onWheel (FWheelEvent* ev)
{
  const int wheel = wheelAt(ev->getX());

  if ( layout != Layout::Wheels || wheel < 0 )
  {
    FDialog::onWheel(ev);
    return;
  }

  // Scrolling over a wheel changes its value
  const auto direction = ev->getWheel();

  if ( direction == MouseWheel::Up )
  {
    active_wheel = wheel;
    adjustWheel(1);
  }
  else if ( direction == MouseWheel::Down )
  {
    active_wheel = wheel;
    adjustWheel(-1);
  }
  else
    FDialog::onWheel(ev);
}


// private methods of FDateDialog
//----------------------------------------------------------------------
void FDateDialog::init()
{
  const std::time_t now = std::time(nullptr);
  const std::tm* today = std::localtime(&now);

  if ( today )
  {
    sel_year = today->tm_year + 1900;
    sel_month = today->tm_mon + 1;
    sel_day = today->tm_mday;
  }

  setModal();
  setSize (FSize{DIALOG_WIDTH, DIALOG_HEIGHT}, false);

  // Dialog buttons
  ok_button.setGeometry (FPoint{8, BUTTON_ROW}, FSize{10, 1});
  cancel_button.setGeometry (FPoint{19, BUTTON_ROW}, FSize{10, 1});

  ok_button.unsetShadow();
  cancel_button.unsetShadow();

  ok_button.addCallback
  (
    "clicked",
    this, &FDateDialog::cb_processOk
  );

  cancel_button.addCallback
  (
    "clicked",
    this, &FDateDialog::cb_processCancel
  );

  applyLayout();
}

//----------------------------------------------------------------------
void FDateDialog::applyLayout()
{
  // The calendar widgets exist only in the calendar layout, so the
  // wheel layout has no hidden children that could still be drawn
  if ( layout == Layout::Calendar )
    createCalendarWidgets();
  else
    destroyCalendarWidgets();
}

//----------------------------------------------------------------------
void FDateDialog::createCalendarWidgets()
{
  if ( prev_button )
    return;

  prev_button = std::make_unique<FButton>("<", this);
  next_button = std::make_unique<FButton>(">", this);
  year_spinbox = std::make_unique<FSpinBox>(this);

  // Calendar layout: month navigation and year chooser
  prev_button->setGeometry (FPoint{GRID_X, HEADER_ROW}, FSize{5, 1});
  next_button->setGeometry ( FPoint{DIALOG_WIDTH - 2 - 5, HEADER_ROW}
                           , FSize{5, 1} );
  year_spinbox->setRange (MIN_YEAR, MAX_YEAR);
  year_spinbox->setValue (sel_year);
  year_spinbox->setGeometry (FPoint{9, YEAR_ROW}, FSize{12, 1});

  prev_button->unsetShadow();
  next_button->unsetShadow();
  year_spinbox->unsetShadow();

  prev_button->addCallback
  (
    "clicked",
    this, &FDateDialog::cb_processPrevMonth
  );

  next_button->addCallback
  (
    "clicked",
    this, &FDateDialog::cb_processNextMonth
  );

  year_spinbox->addCallback
  (
    "changed",
    this, &FDateDialog::cb_processYearChange
  );

  // A widget created on a shown dialog must be drawn now
  if ( isShown() )
  {
    prev_button->show();
    next_button->show();
    year_spinbox->show();
  }
}

//----------------------------------------------------------------------
void FDateDialog::destroyCalendarWidgets()
{
  prev_button.reset();
  next_button.reset();
  year_spinbox.reset();
}

//----------------------------------------------------------------------
void FDateDialog::draw()
{
  FDialog::draw();
  setColor();

  if ( layout == Layout::Calendar )
  {
    drawHeader();
    drawWeekdays();
    drawDays();
  }
  else
  {
    drawWheels();
  }
}

//----------------------------------------------------------------------
void FDateDialog::drawHeader()
{
  // Center the month name between the two navigation buttons
  const char* const name = month_names[std::size_t(sel_month - 1)];
  const int name_len = int(std::strlen(name));
  const int free_begin = GRID_X + 5;
  const int free_width = DIALOG_WIDTH - 2 - 5 - free_begin;
  const int x = free_begin + (free_width - name_len) / 2;
  print() << FPoint{x, HEADER_ROW + ROW_OFFSET} << FString{name};
}

//----------------------------------------------------------------------
void FDateDialog::drawWeekdays()
{
  for ( std::size_t i = 0; i < weekday_names.size(); i++ )
  {
    const int x = GRID_X + int(i) * CELL_WIDTH;
    print() << FPoint{x, WEEKDAY_ROW + ROW_OFFSET}
            << FString{weekday_names[i]};
  }
}

//----------------------------------------------------------------------
void FDateDialog::drawDays()
{
  const int offset = firstWeekdayOffset();
  const int days = daysInMonth(sel_year, sel_month);

  for ( int week = 0; week < WEEK_ROWS; week++ )
  {
    for ( int col = 0; col < DAYS_PER_WEEK; col++ )
    {
      const int day = week * DAYS_PER_WEEK + col - offset + 1;

      if ( day < 1 || day > days )
        continue;

      char text[16]{};
      std::snprintf (text, sizeof(text), "%2d", day);
      setReverse (day == sel_day);
      const int x = GRID_X + col * CELL_WIDTH;
      print() << FPoint{x, FIRST_WEEK_ROW + week + ROW_OFFSET}
              << FString{text};
    }
  }

  unsetReverse();
}

//----------------------------------------------------------------------
void FDateDialog::drawWheels()
{
  for ( int wheel = 0; wheel < WHEEL_COUNT; wheel++ )
    drawWheel(wheel);

  unsetReverse();
  setColor();
}

//----------------------------------------------------------------------
void FDateDialog::drawWheel (int wheel)
{
  const int x = WHEEL_X + wheel * WHEEL_SPACING;
  const char* const label = wheel_labels[std::size_t(wheel)];
  const int label_len = int(std::strlen(label));
  setColor();
  print() << FPoint{ x + (WHEEL_WIDTH - label_len) / 2
                   , WHEEL_LABEL_ROW + ROW_OFFSET }
          << FString{label};

  const int value = wheelValue(wheel);

  const int lowest = wheel == 2 ? MIN_YEAR : 1;
  const int highest = wheelMax(wheel);

  for ( int row = 0; row < WHEEL_ROWS; row++ )
  {
    const int y = WHEEL_TOP_ROW + row + ROW_OFFSET;

    // The box lines around the selected value
    if ( row == WHEEL_BOX_TOP || row == WHEEL_BOX_BOTTOM )
    {
      // Eight columns wide, the same as WHEEL_WIDTH
      const char* const box_line = row == WHEEL_BOX_TOP
                                 ? "┌──────┐"
                                 : "└──────┘";
      setColor (accent_color, getBackgroundColor());
      unsetReverse();
      print() << FPoint{x, y} << FString{box_line};
      continue;
    }

    // Values above and below the selected one, three on each side
    int offset = 0;

    if ( row < WHEEL_BOX_TOP )
      offset = row - WHEEL_BOX_TOP;
    else if ( row > WHEEL_BOX_BOTTOM )
      offset = row - WHEEL_BOX_BOTTOM;

    const int number = value + offset;
    const bool selected = (row == WHEEL_VALUE_ROW);
    const bool in_range = number >= lowest && number <= highest;

    if ( selected )
    {
      // The accent color marks the selected value
      setColor (accent_color, getBackgroundColor());
      setReverse (wheel == active_wheel);
    }
    else
    {
      setColor();
      unsetReverse();
    }

    std::string cell (std::size_t(WHEEL_WIDTH), ' ');

    if ( in_range )
    {
      char digits[16]{};
      const int len = std::snprintf (digits, sizeof(digits), "%d", number);
      const int pad = (WHEEL_WIDTH - len) / 2;
      cell.replace (std::size_t(pad), std::size_t(len), digits);
    }

    print() << FPoint{x, y} << FString{cell.c_str()};
  }
}

//----------------------------------------------------------------------
void FDateDialog::syncYearSpinBox()
{
  if ( year_spinbox && year_spinbox->getValue() != sel_year )
    year_spinbox->setValue(sel_year);
}

//----------------------------------------------------------------------
void FDateDialog::selectionChanged()
{
  syncYearSpinBox();
  redraw();
  emitCallback("changed");
}

//----------------------------------------------------------------------
auto FDateDialog::firstWeekdayOffset() const noexcept -> int
{
  // Weekday of the 1st of the selected month, Monday = 0
  const int y = sel_month < 3 ? sel_year - 1 : sel_year;
  const int sunday_based = ( y + y / 4 - y / 100 + y / 400
                           + sakamoto_table[std::size_t(sel_month - 1)]
                           + 1 ) % DAYS_PER_WEEK;
  return (sunday_based + DAYS_PER_WEEK - 1) % DAYS_PER_WEEK;
}

//----------------------------------------------------------------------
auto FDateDialog::dayAt (int x, int y) const noexcept -> int
{
  const int row = y - FIRST_WEEK_ROW - ROW_OFFSET;

  if ( row < 0 || row >= WEEK_ROWS )
    return 0;

  const int rel_x = x - GRID_X;

  if ( rel_x < 0 )
    return 0;

  const int col = rel_x / CELL_WIDTH;

  // The day number occupies the first two columns of each cell
  if ( col >= DAYS_PER_WEEK || rel_x % CELL_WIDTH >= 2 )
    return 0;

  const int day = row * DAYS_PER_WEEK + col - firstWeekdayOffset() + 1;

  if ( day < 1 || day > daysInMonth(sel_year, sel_month) )
    return 0;

  return day;
}

//----------------------------------------------------------------------
auto FDateDialog::wheelValue (int wheel) const noexcept -> int
{
  if ( wheel == 0 )
    return sel_day;

  if ( wheel == 1 )
    return sel_month;

  return sel_year;
}

//----------------------------------------------------------------------
auto FDateDialog::wheelMax (int wheel) const noexcept -> int
{
  if ( wheel == 0 )
    return daysInMonth(sel_year, sel_month);

  if ( wheel == 1 )
    return 12;

  return MAX_YEAR;
}

//----------------------------------------------------------------------
auto FDateDialog::wheelAt (int x) const noexcept -> int
{
  // Return the wheel whose columns contain x, or -1 outside the wheels
  for ( int wheel = 0; wheel < WHEEL_COUNT; wheel++ )
  {
    const int left = WHEEL_X + wheel * WHEEL_SPACING;

    if ( x >= left && x < left + WHEEL_WIDTH )
      return wheel;
  }

  return -1;
}

//----------------------------------------------------------------------
void FDateDialog::adjustWheel (int delta)
{
  const int low = active_wheel == 2 ? MIN_YEAR : 1;
  const int old_value = wheelValue(active_wheel);
  const int new_value = clampInt ( old_value + delta
                                 , low, wheelMax(active_wheel) );

  if ( new_value == old_value )
    return;

  if ( active_wheel == 0 )
    sel_day = new_value;
  else if ( active_wheel == 1 )
    sel_month = new_value;
  else
    sel_year = new_value;

  // Keep the day inside the selected month
  sel_day = clampInt(sel_day, 1, daysInMonth(sel_year, sel_month));
  selectionChanged();
}

//----------------------------------------------------------------------
void FDateDialog::moveWheel (int delta)
{
  const int new_wheel = clampInt(active_wheel + delta, 0, WHEEL_COUNT - 1);

  if ( new_wheel != active_wheel )
  {
    active_wheel = new_wheel;
    redraw();
  }
}

//----------------------------------------------------------------------
void FDateDialog::moveDays (int n)
{
  for ( int i = 0; i < std::abs(n); i++ )
  {
    if ( n > 0 )
    {
      if ( sel_day < daysInMonth(sel_year, sel_month) )
      {
        sel_day++;
      }
      else if ( sel_year < MAX_YEAR || sel_month < 12 )
      {
        sel_day = 1;

        if ( sel_month == 12 )
        {
          sel_month = 1;
          sel_year++;
        }
        else
        {
          sel_month++;
        }
      }
      else
      {
        break;  // end of supported range
      }
    }
    else
    {
      if ( sel_day > 1 )
      {
        sel_day--;
      }
      else if ( sel_year > MIN_YEAR || sel_month > 1 )
      {
        if ( sel_month == 1 )
        {
          sel_month = 12;
          sel_year--;
        }
        else
        {
          sel_month--;
        }

        sel_day = daysInMonth(sel_year, sel_month);
      }
      else
      {
        break;  // start of supported range
      }
    }
  }

  selectionChanged();
}

//----------------------------------------------------------------------
void FDateDialog::moveMonths (int n)
{
  const int total = sel_year * 12 + (sel_month - 1) + n;
  const int year = total / 12;
  const int month = total % 12 + 1;

  if ( year < MIN_YEAR || year > MAX_YEAR )
    return;

  sel_year = year;
  sel_month = month;
  sel_day = std::min(sel_day, daysInMonth(year, month));
  selectionChanged();
}


// private callback methods of FDateDialog
//----------------------------------------------------------------------
void FDateDialog::cb_processYearChange()
{
  const auto value = year_spinbox->getValue();

  if ( value < MIN_YEAR || value > MAX_YEAR || int(value) == sel_year )
    return;

  sel_year = int(value);
  sel_day = std::min(sel_day, daysInMonth(sel_year, sel_month));
  selectionChanged();
}

//----------------------------------------------------------------------
void FDateDialog::cb_processPrevMonth()
{
  moveMonths(-1);
}

//----------------------------------------------------------------------
void FDateDialog::cb_processNextMonth()
{
  moveMonths(1);
}

//----------------------------------------------------------------------
void FDateDialog::cb_processOk()
{
  done (ResultCode::Accept);
}

//----------------------------------------------------------------------
void FDateDialog::cb_processCancel()
{
  done (ResultCode::Reject);
}

}  // namespace finalcut
