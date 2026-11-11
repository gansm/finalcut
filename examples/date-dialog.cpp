/***********************************************************************
* date-dialog.cpp - A calendar date chooser example                    *
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

#include <final/final.h>

using finalcut::FPoint;
using finalcut::FSize;
using finalcut::FDateDialog;

// function prototypes
void cb_quit (const finalcut::FApplication&);
void cb_pickDate ( finalcut::FDialog&
                 , finalcut::FLabel&
                 , FDateDialog::Layout );

//----------------------------------------------------------------------
// callback functions
//----------------------------------------------------------------------
void cb_quit (const finalcut::FApplication& app)
{
  app.quit();
}

//----------------------------------------------------------------------
void cb_pickDate ( finalcut::FDialog& main_dialog
                 , finalcut::FLabel& result
                 , FDateDialog::Layout layout )
{
  // Open the date chooser as a modal dialog on the heap
  auto* date_dialog = new FDateDialog{"Choose a date", &main_dialog};
  date_dialog->setDate (2026, 11, 11);  // preselect a date
  date_dialog->setLayout (layout);

  const bool accepted = date_dialog->exec() == finalcut::FDialog::ResultCode::Accept;
  finalcut::FString date{};

  if ( accepted )
    date = date_dialog->getDate();

  // Destroy the date chooser before the result is shown
  delete date_dialog;
  date_dialog = nullptr;

  if ( accepted )
  {
    result.setText (finalcut::FString{"Selected date: "} + date);

    // Show the chosen date in a popup message
    finalcut::FMessageBox::info ( &main_dialog
                                , "Date selected"
                                , finalcut::FString{"You selected: "} + date );
  }
  else
  {
    result.setText ("Selection cancelled");
  }

  result.redraw();
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
auto main (int argc, char* argv[]) -> int
{
  // Create the application object
  finalcut::FApplication app{argc, argv};

  // Create the main dialog box
  finalcut::FDialog dgl{&app};
  dgl.setText ("Date demo");
  dgl.setGeometry (FPoint{4, 2}, FSize{40, 10});
  dgl.setShadow();

  // Label that shows the result
  finalcut::FLabel result {"No date selected yet", &dgl};
  result.setGeometry (FPoint{2, 2}, FSize{36, 1});

  // Button to open the calendar layout
  finalcut::FButton pick_btn {"&Pick a date...", &dgl};
  pick_btn.setGeometry (FPoint{2, 4}, FSize{18, 1});

  // Button to quit
  finalcut::FButton quit_btn {"&Quit", &dgl};
  quit_btn.setGeometry (FPoint{24, 4}, FSize{10, 1});

  // Button to open the wheel layout
  finalcut::FButton pick_wheel_btn {"Pick with &wheels...", &dgl};
  pick_wheel_btn.setGeometry (FPoint{2, 6}, FSize{24, 1});

  // Connect the buttons with the callback functions
  pick_btn.addCallback
  (
    "clicked",
    &cb_pickDate,
    std::ref(dgl),
    std::ref(result),
    FDateDialog::Layout::Calendar
  );

  pick_wheel_btn.addCallback
  (
    "clicked",
    &cb_pickDate,
    std::ref(dgl),
    std::ref(result),
    FDateDialog::Layout::Wheels
  );

  quit_btn.addCallback
  (
    "clicked",
    &cb_quit,
    std::ref(app)
  );

  // Set dialog object as main widget
  finalcut::FWidget::setMainWidget(&dgl);

  // Show and start the application
  dgl.show();
  return app.exec();
}
