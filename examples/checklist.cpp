/***********************************************************************
* checklist.cpp - Example for FListView widget with checkboxes         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2022 Markus Gans                                      *
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

#include <array>
#include <iostream>
#include <fstream>
#include <string>

#include <final/final.h>

using finalcut::FPoint;
using finalcut::FSize;


//----------------------------------------------------------------------
// class CheckList
//----------------------------------------------------------------------

class CheckList final : public finalcut::FDialog
{
  public:
    // Constructor
    explicit CheckList (finalcut::FWidget* = nullptr);

  private:
    // Method
    void populate();
    void initLayout() override;
    void adjustSize() override;

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
  setShadow();  // Instead of the transparent window shadow
  listview.ignorePadding();

  // Add columns to the view
  listview.addColumn ("Item");
  listview.addColumn ("Priority", 9);

  // Set the type of sorting
  listview.setColumnSortType (1, finalcut::SortType::Name);
  listview.setColumnSortType (2, finalcut::SortType::Name);

  // Statusbar at the bottom
  finalcut::FString separator{};
  separator << ' ' << finalcut::UniChar::BoxDrawingsVertical << ' ';
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
void CheckList::populate()
{
  constexpr std::array<std::array<const char*, 2>, 10> list =
  {{
    {{ "Milk", "Highest" }},
    {{ "Cheese", "High" }},
    {{ "Yoghurt", "Medium" }},
    {{ "Bread", "Low" }},
    {{ "Eggs", "High" }},
    {{ "Toothpaste", "Medium" }},
    {{ "Apples", "Lowest" }},
    {{ "Bananas", "Medium" }},
    {{ "Fish", "Medium" }},
    {{ "Lemons", "Low" }}
  }};

  for (const auto& line : list)
  {
    const finalcut::FStringList string_line (line.cbegin(), line.cend());
    auto iter = listview.insert (string_line);
    auto item = static_cast<finalcut::FListViewItem*>(*iter);
    item->setCheckable(true);
  }
}

//----------------------------------------------------------------------
void CheckList::initLayout()
{
  FDialog::setText (L"Shopping list");
  const std::size_t nf_offset = ( finalcut::FVTerm::getFOutput()->isNewFont() ) ? 1 : 0;
  FDialog::setSize (FSize{28 + nf_offset, 13} );
  listview.setGeometry ( FPoint{1 + int(nf_offset), 2}
                       , FSize{getWidth() - nf_offset, getHeight() - 1} );
  FDialog::initLayout();
}

//----------------------------------------------------------------------
void CheckList::adjustSize()
{
  finalcut::FDialog::adjustSize();
  setPos(FPoint{int(1 + (getDesktopWidth() - getWidth()) / 2), 5});
}

//----------------------------------------------------------------------
void CheckList::onKeyPress (finalcut::FKeyEvent* ev)
{
  if ( ! ev )
    return;

  if ( ev->key() == finalcut::FKey('q') || isEscapeKey(ev->key()) )
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
  finalcut::FString shopping_list{};

  for (const auto& item : listview.getData())
  {
    if ( item->isChecked() )
      shopping_list << finalcut::UniChar::Bullet << ' ' << item->getText(1) << '\n';
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
