/***********************************************************************
* fprogressbar.cpp - Widget FProgressbar                               *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2024 Markus Gans                                      *
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

#include "final/fevent.h"
#include "final/fwidgetcolors.h"
#include "final/util/fstring.h"
#include "final/vterm/fcolorpair.h"
#include "final/widget/fprogressbar.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FProgressbar
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FProgressbar::FProgressbar(FWidget* parent)
  : FWidget{parent}
{
  init();
}

//----------------------------------------------------------------------
FProgressbar::~FProgressbar() noexcept = default;  // destructor


// public methods of FProgressbar
//----------------------------------------------------------------------
void FProgressbar::setPercentage (std::size_t percentage_value)
{
  if ( percentage_value <= percentage && percentage != NOT_SET )
    return;

  percentage = std::min(percentage_value, std::size_t(100));

  if ( isShown() )
  {
    drawProgressLabel();
    drawProgressBar();
  }
}

//----------------------------------------------------------------------
void FProgressbar::setSize (const FSize& size, bool adjust)
{
  // Sets the progress bar size

  FWidget::setSize (size, adjust);
  bar_length = size.getWidth();
}

//----------------------------------------------------------------------
void FProgressbar::setGeometry ( const FPoint& pos, const FSize& size
                               , bool adjust )
{
  // Sets the progress bar geometry

  FWidget::setGeometry (pos, size, adjust);
  bar_length = size.getWidth();
}

//----------------------------------------------------------------------
void FProgressbar::setShadow (bool enable)
{
  setWidgetShadow(this, enable);
}

//----------------------------------------------------------------------
void FProgressbar::hide()
{
  FWidget::hide();
  const auto& shadow = hasShadow() ? FSize(1, 1) : FSize(0, 0);
  hideArea (getSize() + shadow);
  print() << FPoint{int(getWidth()) - 4, 0}
          << "      ";  // hide percentage
}

//----------------------------------------------------------------------
void FProgressbar::reset()
{
  percentage = NOT_SET;

  if ( isShown() )
  {
    drawProgressLabel();
    drawProgressBar();
  }
}


// private methods of FProgressbar
//----------------------------------------------------------------------
void FProgressbar::init()
{
  unsetFocusable();
  setShadow();
}

//----------------------------------------------------------------------
void FProgressbar::draw()
{
  drawProgressLabel();
  drawProgressBar();

  if ( getFlags().shadow.shadow )
    drawShadow(this);

  forceTerminalUpdate();
}

//----------------------------------------------------------------------
void FProgressbar::drawProgressLabel()
{
  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  useParentWidgetColor();
  print() << FPoint{int(getWidth()) - 3, 0};

  if ( percentage > 100 )
    print ("--- %");
  else
    printf ("%3zu %%", percentage);

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
void FProgressbar::drawProgressBar()
{
  std::size_t len{0};
  print() << FPoint{1, 1};

  if ( percentage > 0 && percentage <= 100 )
    len = drawProgressIndicator();

  drawProgressBackground(len);

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
auto FProgressbar::drawProgressIndicator() -> std::size_t
{
  // Draw the progress indicator

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  const auto& wc_progressbar = getColorTheme()->progressbar;
  const double length = double(bar_length * percentage) / 100;
  auto len = std::size_t(trunc(length));
  print() << FColorPair {wc_progressbar.fg, wc_progressbar.fg}
          << FString {len, UniChar::FullBlock};  // █

  if ( len >= bar_length )
    return len;

  if ( std::size_t(round(length)) > len || FVTerm::getFOutput()->getMaxColor() < 16 )
  {
    if ( FVTerm::getFOutput()->isMonochron() )
      setReverse(false);

    print(' ');

    if ( FVTerm::getFOutput()->isMonochron() )
      setReverse(true);
  }
  else
  {
    print() << FColorPair{wc_progressbar.fg, wc_progressbar.bg}
            << UniChar::LeftHalfBlock;  // ▌
  }

  len++;
  return len;
}

//----------------------------------------------------------------------
void FProgressbar::drawProgressBackground (std::size_t len)
{
  // Draw the progress background

  const std::size_t bg_len = bar_length - len;
  const auto& wc_progressbar = getColorTheme()->progressbar;
  setColor (wc_progressbar.fg, wc_progressbar.bg);

  if ( FVTerm::getFOutput()->getMaxColor() < 16 )
    print() << FString {bg_len, UniChar::MediumShade};  // ▒
  else
    print() << FString {bg_len, L' '};
}

}  // namespace finalcut
