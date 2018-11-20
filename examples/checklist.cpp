/***********************************************************************
* checklist.cpp - Example for FListView widget with checkboxes         *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2017-2018 Markus Gans                                      *
*                                                                      *
* The Final Cut is free software; you can redistribute it and/or       *
* modify it under the terms of the GNU Lesser General Public License   *
* as published by the Free Software Foundation; either version 3 of    *
* the License, or (at your option) any later version.                  *
*                                                                      *
* The Final Cut is distributed in the hope that it will be useful,     *
* but WITHOUT ANY WARRANTY; without even the implied warranty of       *
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


//----------------------------------------------------------------------
// class CheckList
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class CheckList : public finalcut::FDialog
{
  public:
    // Constructor
    explicit CheckList (finalcut::FWidget* = 0);
    // Destructor
    ~CheckList();

  private:
    // Disable copy constructor
    CheckList (const CheckList&);
    // Disable assignment operator (=)
    CheckList& operator = (const CheckList&);

    // Method
    void populate();

    // Event handlers
    virtual void onKeyPress (finalcut::FKeyEvent*);
    virtual void onClose (finalcut::FCloseEvent*);

    // Callback method
    void cb_showList (finalcut::FWidget*, data_ptr);

    // Data Members
    finalcut::FListView  listView;
    finalcut::FStatusBar status_bar;
};
#pragma pack(pop)

//----------------------------------------------------------------------
CheckList::CheckList (finalcut::FWidget* parent)
  : finalcut::FDialog(parent)
  , listView(this)
  , status_bar(this)
{
  setText (L"Shopping list");
  setShadow();
  setGeometry (int(1 + (parent->getWidth() - 30) / 2), 5, 30, 13);
  listView.ignorePadding();
  listView.setGeometry (1, 2, getWidth(), getHeight() - 1);

  // Add columns to the view
  listView.addColumn ("Item");
  listView.addColumn ("Priority", 12);

  // Set the type of sorting
  listView.setColumnSortType (1, finalcut::fc::by_name);
  listView.setColumnSortType (2, finalcut::fc::by_name);

  // Statusbar at the bottom
  finalcut::FString separator;
  separator << ' ' << wchar_t(finalcut::fc::BoxDrawingsVertical) << ' ';
  listView.setStatusbarMessage ( finalcut::FString()
                                 << "<Q> exit" << separator
                                 << "<Space> select an item" << separator
                                 << "<Enter> see your pick list");

  // Populate FListView with a list of items
  populate();

  // Add callback method
  listView.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &CheckList::cb_showList)
  );
}

//----------------------------------------------------------------------
CheckList::~CheckList()  // destructor
{ }

//----------------------------------------------------------------------
void CheckList::populate()
{
  std::string list[][2] =
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

  const int lastItem = int(sizeof(list) / sizeof(list[0])) - 1;

  for (int i = 0; i <= lastItem; i++)
  {
    const finalcut::FStringList line (&list[i][0], &list[i][0] + 2);
    finalcut::FObject::FObjectIterator iter = listView.insert (line);
    finalcut::FListViewItem* item = \
        static_cast<finalcut::FListViewItem*>(*iter);
    item->setCheckable(true);
  }
}

//----------------------------------------------------------------------
void CheckList::onKeyPress (finalcut::FKeyEvent* ev)
{
  if ( ! ev )
    return;

  if ( ev->key() == 'q'
    || ev->key() == finalcut::fc::Fkey_escape
    || ev->key() == finalcut::fc::Fkey_escape_mintty )
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
void CheckList::cb_showList (finalcut::FWidget*, data_ptr)
{
  finalcut::FListViewIterator iter = listView.beginOfList();
  finalcut::FString shopping_list;

  while ( iter != listView.endOfList() )
  {
    const finalcut::FListViewItem* item = \
        static_cast<finalcut::FListViewItem*>(*iter);

    if ( item->isChecked() )
      shopping_list << wchar_t(finalcut::fc::Bullet) << ' '
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
  app.setMainWidget(&d);

  // Show and start the application
  d.show();
  return app.exec();
}
