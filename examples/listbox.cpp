/***********************************************************************
* listbox.cpp - Example for using a FListBox widget                    *
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

#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <fstream>

#include <final/final.h>

using namespace finalcut;
using finalcut::FPoint;
using finalcut::FSize;

// Global application object
static std::weak_ptr<FString> temp_str;


// Function prototypes
void doubleToItem ( FListBoxItem&
                  , FDataAccess* container
                  , std::size_t index);
FString& doubleToString (std::list<double>::const_iterator iter);
FString& mapToString ( std::map<FString
                     , FString>::const_iterator iter );


// Lazy conversion insert function
void doubleToItem ( FListBoxItem& item
                  , FDataAccess* container
                  , std::size_t index )
{
  using DblList = std::list<double>;
  DblList& dbl_list = flistboxhelper::getContainer<DblList>(container);
  auto iter = dbl_list.begin();
  std::advance (iter, index);
  item.setText (FString() << *iter);
  item.setData (*iter);
}

// Insert converter functions
FString& doubleToString (std::list<double>::const_iterator iter)
{
  auto temp = temp_str.lock();
  return temp->setNumber(*iter);
}

FString& mapToString ( std::map<FString
                              , FString>::const_iterator iter )
{
  auto temp = temp_str.lock();
  return *temp = iter->first + ": " + iter->second;
}


//----------------------------------------------------------------------
// class Listbox
//----------------------------------------------------------------------

class Listbox final : public FDialog
{
  public:
    // Constructor
    explicit Listbox (FWidget* = nullptr);

  private:
    // Method
    void initLayout() override;

    // Event handlers
    void onClose (FCloseEvent*) override;

    // Data member
    std::list<double>  double_list{};
    FListBox list1{this};
    FListBox list2{this};
    FListBox list3{this};
    FButton  quit{this};
};

//----------------------------------------------------------------------
Listbox::Listbox (FWidget* parent)
  : FDialog{parent}
{
  auto temp = std::make_shared<FString>();
  temp_str = temp;

  // listbox 1
  //----------
  list1.setText ("FListBoxItem");

  for (auto i{1}; i < 30; i++)
    list1.insert (L"----- " + (FString{} << i) + L" -----");

  // listbox 2
  //----------
  for (auto i{1}; i <= 15; i++)
    double_list.push_back(2 * double(i) + (double(i) / 100));

  list2.setText ("double");

  //
  // Insert via lazy conversion on print
  //
  list2.insert (double_list, doubleToItem);  // (container, converter)

  //
  // Direct insert of the complete list
  //
  //list2.insert (double_list.cbegin(), double_list.cend(), doubleToString);

  // listbox 3
  //----------
  std::map<FString, FString> TLD =
  {
    { "com", "Commercial" },
    { "org", "Organization" },
    { "net", "Network" },
    { "edu", "Education" },
    { "gov", "Government" }
  };

  list3.insert (TLD.cbegin(), TLD.cend(), mapToString);
  list3.setText ("key: value");

  // Quit button
  quit.setText (L"&Quit");

  // Add quit button function callback
  quit.addCallback
  (
    "clicked",
    finalcut::getFApplication(),
    &finalcut::FApplication::cb_exitApp,
    this
  );
}

//----------------------------------------------------------------------
void Listbox::initLayout()
{
  list1.setGeometry(FPoint{2, 1}, FSize{18, 10});
  list2.setGeometry(FPoint{21, 1}, FSize{10, 10});
  list3.setGeometry(FPoint{32, 1}, FSize{21, 10});
  quit.setGeometry(FPoint{42, 12}, FSize{10, 1});
  FDialog::initLayout();
}

//----------------------------------------------------------------------
void Listbox::onClose (FCloseEvent* ev)
{
  FApplication::closeConfirmationDialog (this, ev);
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

int main (int argc, char* argv[])
{
  // Create the application object
  FApplication app(argc, argv);

  // Create main dialog object
  Listbox d(&app);
  d.setText (L"Listbox");
  d.setGeometry ( FPoint{int(1 + (app.getWidth() - 56) / 2), 5}
                , FSize{56, 16} );
  d.setShadow();

  // Set dialog d as main widget
  finalcut::FWidget::setMainWidget(&d);

  // Show and start the application
  d.show();
  return app.exec();
}
