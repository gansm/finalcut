/***********************************************************************
* checklist.cpp - Example for FListView widget with checkboxes         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2020 Markus Gans                                      *
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

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <final/final.h>

namespace fc = finalcut::fc;
using finalcut::FPoint;
using finalcut::FRect;
using finalcut::FSize;


//----------------------------------------------------------------------
// class CheckList
//----------------------------------------------------------------------

class CheckList final : public finalcut::FDialog
{
  public:
    // Using-declaration
    using FDialog::setGeometry;

    // Constructor
    explicit CheckList (finalcut::FWidget* = nullptr);

    // Disable copy constructor
    CheckList (const CheckList&) = delete;

    // Destructor
    ~CheckList() override;

    // Disable copy assignment operator (=)
    CheckList& operator = (const CheckList&) = delete;

  private:
    // Method
    void populate();

    // Event handlers
    void onKeyPress (finalcut::FKeyEvent*) override;
    void onClose (finalcut::FCloseEvent*) override;

    // Callback method
    void cb_showList();

    // Data members
    finalcut::FListView  listview{this};
    finalcut::FStatusBar status_bar{this};
};

//----------------------------------------------------------------------
CheckList::CheckList (finalcut::FWidget* parent)
  : finalcut::FDialog{parent}
{
  // Dialog settings
  //   Avoids calling a virtual function from the constructor
  //   (CERT, OOP50-CPP)
  FDialog::setText (L"Shopping list");
  const std::size_t nf_offset = ( finalcut::FTerm::isNewFont() ) ? 1 : 0;
  FDialog::setGeometry ( FPoint{int(1 + (parent->getWidth() - 28) / 2), 5}
                       , FSize{28 + nf_offset, 13} );
  setShadow();
  listview.ignorePadding();
  listview.setGeometry ( FPoint{1 + int(nf_offset), 2}
                       , FSize{getWidth() - nf_offset, getHeight() - 1} );

  // Add columns to the view
  listview.addColumn ("Item");
  listview.addColumn ("Priority", 9);

  // Set the type of sorting
  listview.setColumnSortType (1, fc::by_name);
  listview.setColumnSortType (2, fc::by_name);

  // Statusbar at the bottom
  finalcut::FString separator{};
  separator << ' ' << fc::BoxDrawingsVertical << ' ';
  listview.setStatusbarMessage ( finalcut::FString{}
                                 << "<Q> exit" << separator
                                 << "<Space> select an item" << separator
                                 << "<Enter> see your pick list");

  // Populate FListView with a list of items
  populate();

  // Add callback method
  listview.addCallback
  (
    "clicked",
    this, &CheckList::cb_showList
  );
}

//----------------------------------------------------------------------
CheckList::~CheckList()  // destructor
{ }

//----------------------------------------------------------------------
void CheckList::populate()
{
  const std::string list[][2] =
  {
    { "Milk", "Highest" },
    { "Cheese", "High" },
    { "Yoghurt", "Medium" },
    { "Bread", "Low" },
    { "Eggs", "High" },
    { "Toothpaste", "Medium" },
    { "Apples", "Lowest" },
    { "Bananas", "Medium" },
    { "Fish", "Medium" },
    { "Lemons", "Low" }
  };

  for (const auto& line : list)
  {
    const finalcut::FStringList string_line (&line[0], &line[0] + 2);
    auto iter = listview.insert (string_line);
    auto item = static_cast<finalcut::FListViewItem*>(*iter);
    item->setCheckable(true);
  }
}

//----------------------------------------------------------------------
void CheckList::onKeyPress (finalcut::FKeyEvent* ev)
{
  if ( ! ev )
    return;

  if ( ev->key() == 'q'
    || ev->key() == fc::Fkey_escape
    || ev->key() == fc::Fkey_escape_mintty )
  {
    close();
    ev->accept();
  }
  else
    finalcut::FDialog::onKeyPress(ev);
}

//----------------------------------------------------------------------
void CheckList::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void CheckList::cb_showList()
{
  auto iter = listview.beginOfList();
  finalcut::FString shopping_list{};

  while ( iter != listview.endOfList() )
  {
    const auto item = static_cast<finalcut::FListViewItem*>(*iter);

    if ( item->isChecked() )
      shopping_list << fc::Bullet << ' '
                    << item->getText(1) << '\n';

    ++iter;
  }

  if ( shopping_list.isEmpty() )
    return;

  // Create and show tooltip for two seconds
  finalcut::FToolTip picklist(this);
  picklist.setText (shopping_list);
  picklist.show();
  sleep(2);
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

int main (int argc, char* argv[])
{
  // Create the application object
  finalcut::FApplication app(argc, argv);

  // Create main dialog object
  CheckList d(&app);

  // Set dialog d as main widget
  finalcut::FWidget::setMainWidget(&d);

  // Show and start the application
  d.show();
  return app.exec();
}
