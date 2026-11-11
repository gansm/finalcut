/***********************************************************************
* fdatedialog.h - Widget FDateDialog (a calendar date chooser)         *
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

/*  Inheritance diagram
 *  ═══════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▏
 * ▕ FVTerm  ▏ ▕ FObject ▏
 * ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▏
 *      ▲           ▲
 *      │           │
 *      └─────┬─────┘
 *            │
 *       ▕▔▔▔▔▔▔▔▔▔▏
 *       ▕ FWidget ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏
 *            ▲
 *            │
 *       ▕▔▔▔▔▔▔▔▔▔▏
 *       ▕ FWindow ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏
 *            ▲
 *            │
 *       ▕▔▔▔▔▔▔▔▔▔▏
 *       ▕ FDialog ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏
 *            ▲
 *            │
 *     ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *     ▕ FDateDialog ▏
 *     ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FDATEDIALOG_H
#define FDATEDIALOG_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <memory>

#include "final/dialog/fdialog.h"
#include "final/fwidgetcolors.h"
#include "final/widget/fbutton.h"
#include "final/widget/fspinbox.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FDateDialog
//----------------------------------------------------------------------

class FDateDialog : public FDialog
{
  public:
    // Enumeration
    enum class Layout : uInt8
    {
      Calendar = 0,  // month grid with month and year chooser
      Wheels   = 1   // day, month and year wheels
    };

    // Constructors
    explicit FDateDialog (FWidget* = nullptr);
    explicit FDateDialog (FString&&, FWidget* = nullptr);

    // Destructor
    ~FDateDialog() noexcept override;

    // Accessors
    auto getClassName() const -> FString override;
    auto getYear() const noexcept -> int;
    auto getMonth() const noexcept -> int;
    auto getDay() const noexcept -> int;
    auto getDate() const -> FString;  // "YYYY-MM-DD"
    auto getLayout() const noexcept -> Layout;

    // Mutators
    auto setDate (int, int, int) -> bool;
    void setLayout (Layout);

    // Methods
    static auto isLeapYear (int) noexcept -> bool;
    static auto daysInMonth (int, int) noexcept -> int;

  protected:
    // Methods
    void adjustSize() override;
    void onKeyPress (FKeyEvent*) override;
    void onMouseDown (FMouseEvent*) override;
    void onMouseDoubleClick (FMouseEvent*) override;
    void onWheel (FWheelEvent*) override;

  private:
    // Constants
    static constexpr int MIN_YEAR = 1;
    static constexpr int MAX_YEAR = 9999;
    static constexpr int GRID_X = 2;
    static constexpr int CELL_WIDTH = 4;
    static constexpr int DAYS_PER_WEEK = 7;
    static constexpr int WEEK_ROWS = 6;
    static constexpr int DIALOG_WIDTH = 32;
    static constexpr int DIALOG_HEIGHT = 16;
    static constexpr int ROW_OFFSET = 2;  // title bar and border rows
    static constexpr int HEADER_ROW = 1;
    static constexpr int YEAR_ROW = 2;
    static constexpr int WEEKDAY_ROW = 4;
    static constexpr int FIRST_WEEK_ROW = 5;
    static constexpr int BUTTON_ROW = 12;
    static constexpr int WHEEL_LABEL_ROW = 1;
    static constexpr int WHEEL_TOP_ROW = 2;
    // Wheel rows: 3 values, box top, selected value, box bottom, 3 values
    static constexpr int WHEEL_ROWS = 9;
    static constexpr int WHEEL_BOX_TOP = 3;
    static constexpr int WHEEL_VALUE_ROW = 4;
    static constexpr int WHEEL_BOX_BOTTOM = 5;
    static constexpr int WHEEL_WIDTH = 8;
    static constexpr int WHEEL_X = 3;
    static constexpr int WHEEL_SPACING = 10;
    static constexpr int WHEEL_COUNT = 3;

    // Methods
    void init();
    void applyLayout();
    void createCalendarWidgets();
    void destroyCalendarWidgets();
    void draw() override;
    void drawHeader();
    void drawWeekdays();
    void drawDays();
    void drawWheels();
    void drawWheel (int);
    void syncYearSpinBox();
    void selectionChanged();
    auto firstWeekdayOffset() const noexcept -> int;
    auto dayAt (int, int) const noexcept -> int;
    auto wheelValue (int) const noexcept -> int;
    auto wheelMax (int) const noexcept -> int;
    auto wheelAt (int) const noexcept -> int;
    void adjustWheel (int);
    void moveWheel (int);
    void moveDays (int);
    void moveMonths (int);

    // Callback methods
    void cb_processYearChange();
    void cb_processPrevMonth();
    void cb_processNextMonth();
    void cb_processOk();
    void cb_processCancel();

    // Data members
    std::unique_ptr<FSpinBox> year_spinbox{};  // calendar layout only
    std::unique_ptr<FButton>  prev_button{};   // calendar layout only
    std::unique_ptr<FButton>  next_button{};   // calendar layout only
    FButton   ok_button{"&OK", this};
    FButton   cancel_button{"&Cancel", this};
    FColor    accent_color{getColorTheme()->dialog.emphasis_fg};
    int       sel_year{2000};
    int       sel_month{1};
    int       sel_day{1};
    int       active_wheel{0};      // 0 = day, 1 = month, 2 = year
    Layout    layout{Layout::Calendar};
};


// FDateDialog inline functions
//----------------------------------------------------------------------
inline auto FDateDialog::getClassName() const -> FString
{ return "FDateDialog"; }

//----------------------------------------------------------------------
inline auto FDateDialog::getYear() const noexcept -> int
{ return sel_year; }

//----------------------------------------------------------------------
inline auto FDateDialog::getMonth() const noexcept -> int
{ return sel_month; }

//----------------------------------------------------------------------
inline auto FDateDialog::getDay() const noexcept -> int
{ return sel_day; }

//----------------------------------------------------------------------
inline auto FDateDialog::getLayout() const noexcept -> Layout
{ return layout; }

//----------------------------------------------------------------------
inline auto FDateDialog::isLeapYear (int year) noexcept -> bool
{ return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0; }

}  // namespace finalcut

#endif  // FDATEDIALOG_H
