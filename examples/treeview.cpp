/***********************************************************************
* treeview.cpp - Example of a FListView widget with a tree hierarchy   *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2017-2020 Markus Gans                                      *
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

namespace fc = finalcut::fc;
using finalcut::FPoint;
using finalcut::FSize;


// Function prototypes
sInt64 StringToNumber (const finalcut::FString&);
bool sortAscending (const finalcut::FObject*, const finalcut::FObject*);
bool sortDescending (const finalcut::FObject*, const finalcut::FObject*);
bool isLessThanInteger (const finalcut::FString&, const finalcut::FString&);
bool isLessThanDouble (const finalcut::FString&, const finalcut::FString&);
bool isGreaterThanInteger (const finalcut::FString&, const finalcut::FString&);
bool isGreaterThanDouble (const finalcut::FString&, const finalcut::FString&);


// non-member functions
//----------------------------------------------------------------------
sInt64 StringToNumber (const finalcut::FString& str)
{
  // Cut off one character (because LONG_MAX = 2147483647)
  auto NumString = str.left(str.getLength() - 1);
  NumString = NumString.replace(",", "");
  NumString = NumString.replace('.', "");
  sInt64 number = sInt64(NumString.toLong());
  return number;
}

//----------------------------------------------------------------------
inline bool isLessThanInteger ( const finalcut::FString& lhs
                              , const finalcut::FString& rhs )
{
  const sInt64 l_number = StringToNumber(lhs);
  const sInt64 r_number = StringToNumber(rhs);
  return bool( l_number < r_number );  // lhs < rhs
}

//----------------------------------------------------------------------
inline bool isLessThanDouble ( const finalcut::FString& lhs
                             , const finalcut::FString& rhs )
{
  std::setlocale(LC_NUMERIC, "C");
  const double l_number = lhs.toDouble();
  const double r_number = rhs.toDouble();
  return bool( l_number < r_number );  // lhs < rhs
}

//----------------------------------------------------------------------
inline bool isGreaterThanInteger ( const finalcut::FString& lhs
                                 , const finalcut::FString& rhs )
{
  const sInt64 l_number = StringToNumber(lhs);
  const sInt64 r_number = StringToNumber(rhs);
  return bool( l_number > r_number );  // lhs > rhs
}

//----------------------------------------------------------------------
inline bool isGreaterThanDouble ( const finalcut::FString& lhs
                         , const finalcut::FString& rhs )
{
  std::setlocale(LC_NUMERIC, "C");
  const double l_number = lhs.toDouble();
  const double r_number = rhs.toDouble();
  return bool( l_number > r_number );  // lhs > rhs
}

//----------------------------------------------------------------------
bool sortAscending ( const finalcut::FObject* lhs
                   , const finalcut::FObject* rhs )
{
  const auto& l_item = static_cast<const finalcut::FListViewItem*>(lhs);
  const auto& r_item = static_cast<const finalcut::FListViewItem*>(rhs);
  const int column = l_item->getSortColumn();
  const auto& l_str = l_item->getText(column);
  const auto& r_str = r_item->getText(column);

  if ( column == 2 )
  {
    return isGreaterThanInteger(l_str, r_str);
  }
  else if ( column == 3 )
  {
    return isGreaterThanDouble(l_str, r_str);
  }

  return false;
}

//----------------------------------------------------------------------
bool sortDescending ( const finalcut::FObject* lhs
                    , const finalcut::FObject* rhs )
{
  const auto& l_item = static_cast<const finalcut::FListViewItem*>(lhs);
  const auto& r_item = static_cast<const finalcut::FListViewItem*>(rhs);
  const int column = l_item->getSortColumn();
  const auto& l_str = l_item->getText(column);
  const auto& r_str = r_item->getText(column);

  if ( column == 2 )
  {
    return isLessThanInteger(l_str, r_str);
  }
  else if ( column == 3 )
  {
    return isLessThanDouble(l_str, r_str);
  }

  return false;
}


//----------------------------------------------------------------------
// class Treeview
//----------------------------------------------------------------------

class Treeview final : public finalcut::FDialog
{
  public:
    // Constructor
    explicit Treeview (finalcut::FWidget* = nullptr);

    // Disable copy constructor
    Treeview (const Treeview&) = delete;

    // Destructor
    ~Treeview() override;

    // Disable copy assignment operator (=)
    Treeview& operator = (const Treeview&) = delete;

  private:
    // Typedefs
    struct TreeItem;  // forward declaration

    // Methods
    void adjustSize() override;

    // Event handler
    void onClose (finalcut::FCloseEvent*) override;

    // Data members
    bool                initialized{false};
    finalcut::FListView listView{this};
    finalcut::FButton   Quit{this};
    static TreeItem     africa[];
    static TreeItem     asia[];
    static TreeItem     europe[];
    static TreeItem     north_america[];
    static TreeItem     south_america[];
    static TreeItem     oceania[];
};


//----------------------------------------------------------------------
// struct Treeview::TreeItem
//----------------------------------------------------------------------

struct Treeview::TreeItem
{
  const char* const* begin() const
  { return &name; }

  const char* const* end() const
  { return reinterpret_cast<const char* const*>(&child_element); }

  // Data members
  const char* name;
  const char* population;
  const char* density;
  TreeItem*   child_element;
};

//----------------------------------------------------------------------
// class Treeview - array data
//----------------------------------------------------------------------
Treeview::TreeItem Treeview::africa[] =
{
  { "Algeria", "40,400,000", "15.9", nullptr },
  { "Angola", "25,789,024", "20.69", nullptr },
  { "Botswana", "2,250,260", "3.7", nullptr },
  { "Cameroon", "22,534,532", "39.7", nullptr },
  { "Chad", "13,670,084", "8.6", nullptr },
  { "Egypt", "94,666,000", "87", nullptr },
  { "Ethiopia", "102,374,044", "92.7", nullptr },
  { "Ivory Coast", "23,740,424", "63.9", nullptr },
  { "Libya", "6,541,948", "3.55", nullptr },
  { "Madagascar", "24,430,325", "35.2", nullptr },
  { "Mali", "14,517,176", "11.7", nullptr },
  { "Mauritania", "4,301,018", "3.4", nullptr },
  { "Mozambique", "24,692,144", "28.7", nullptr },
  { "Namibia", "2,113,077", "2.54", nullptr },
  { "Niger", "20,672,987", "12.1", nullptr },
  { "Nigeria", "185,989,640", "197.2", nullptr },
  { "Somalia", "14,317,996", "19.31", nullptr },
  { "South Africa", "54,956,900", "42.4", nullptr },
  { "South Sudan", "12,340,000", "13.33", nullptr },
  { "Sudan", "39,578,828", "21.3", nullptr },
  { "Tanzania", "51,820,00", "47.5", nullptr },
  { "Zambia", "16,212,000", "17.2", nullptr },
  { nullptr, nullptr, nullptr, nullptr }
};

Treeview::TreeItem Treeview::asia[] =
{
  { "Afghanistan", "34,656,032", "49.88", nullptr },
  { "China", "1,403,500,365", "145.0", nullptr },
  { "India", "1,324,171,354", "393.9", nullptr },
  { "Indonesia", "261,115,456", "124.66", nullptr },
  { "Iran", "80,829,192", "48.0", nullptr },
  { "Iraq", "37,202,572", "82.7", nullptr },
  { "Japan", "126,740,000", "336.0", nullptr },
  { "Kazakhstan", "17,987,736", "6.49", nullptr },
  { "Mongolia", "3,081,677", "1.97", nullptr },
  { "Myanmar", "51,486,253", "76.0", nullptr },
  { "Pakistan", "207,774,520", "244.4", nullptr },
  { "Russia", "144,463,451", "8.4", nullptr },
  { "Saudi Arabia", "33,000,000", "15.0", nullptr },
  { "Thailand", "68,863,514", "132.1", nullptr },
  { "Turkey", "79,814,871", "102.0", nullptr },
  { "Turkmenistan", "5,662,544", "10.5", nullptr },
  { "Uzbekistan", "32,979,000", "70.5", nullptr },
  { "Vietnam", "94,569,072", "276.03", nullptr },
  { "Yemen", "27,584,213", "44.7", nullptr },
  { nullptr, nullptr, nullptr, nullptr }
};

Treeview::TreeItem Treeview::europe[] =
{
  { "Austria", "8,794,267", "104.0", nullptr },
  { "Belarus", "9,498,700", "45.8", nullptr },
  { "Bulgaria", "7,101,859", "64.9", nullptr },
  { "Czech Republic", "10,610,947", "134.0", nullptr },
  { "Finland", "5,506,312", "16.0", nullptr },
  { "France", "66,991,000", "103.0", nullptr },
  { "Germany", "82,175,700", "227.0", nullptr },
  { "Greece", "11,183,716", "82.0", nullptr },
  { "Hungary", "9,797,561", "105.3", nullptr },
  { "Iceland", "332,529", "3.2", nullptr },
  { "Italy", "60,589,445", "201.3", nullptr },
  { "Norway", "5,267,146", "15.8", nullptr },
  { "Poland", "38,634,007", "123.0", nullptr },
  { "Portugal", "10,309,573", "115.0", nullptr },
  { "Romania", "19,638,000", "84.4", nullptr },
  { "Serbia", "7,058,322", "91.1", nullptr },
  { "Spain", "46,468,102", "92.0", nullptr },
  { "Sweden", "10,065,389", "22.0", nullptr },
  { "United Kingdom", "65,648,000", "270.7", nullptr },
  { nullptr, nullptr, nullptr, nullptr }
};


Treeview::TreeItem Treeview::north_america[] =
{
  { "Canada", "35,151,728", "3.92", nullptr },
  { "Cuba", "11,239,224", "102.3", nullptr },
  { "Greenland", "56,483", "0.028", nullptr },
  { "Guatemala", "16,582,469", "129.0", nullptr },
  { "Honduras", "9,112,867", "64.0", nullptr },
  { "Mexico", "119,530,753", "61.0", nullptr },
  { "Nicaragua", "6,167,237", "51.0", nullptr },
  { "USA", "325,365,189", "35.0", nullptr },
  { nullptr, nullptr, nullptr, nullptr }
};

Treeview::TreeItem Treeview::south_america[] =
{
  { "Argentina", "43,847,430", "14.4", nullptr },
  { "Bolivia", "11,410,651", "10.4", nullptr },
  { "Brazil", "208,064,000", "24.35", nullptr },
  { "Chile", "18,006,407", "24.0", nullptr },
  { "Colombia", "49,364,592", "40.74", nullptr },
  { "Ecuador", "16,385,068", "58.95", nullptr },
  { "Guyana", "773,303", "3.502", nullptr },
  { "Paraguay", "6,725,308", "17.2", nullptr },
  { "Peru", "31,826,018", "23.0", nullptr },
  { "Venezuela", "31,568,179", "33.75", nullptr },
  { nullptr, nullptr, nullptr, nullptr }
};

Treeview::TreeItem Treeview::oceania[] =
{
  { "Australia", "24,675,900", "3.2", nullptr },
  { "Papua New Guinea", "7,059,653", "15.0", nullptr },
  { "Papua", "3,486,432", "11.0", nullptr },
  { "New Zealand", "4,823,090", "17.5", nullptr },
  { "West Papua", "877,437", "6.3", nullptr },
  { "Solomon Islands", "599,419", "18.1", nullptr },
  { "New Caledonia", "268,767", "14.5", nullptr },
  { "Fiji", "898,76", "46.4", nullptr },
  { "Hawaii", "1,428,557", "82.6", nullptr },
  { "Vanuatu", "270,402", "19.7", nullptr },
  { "French Polynesia", "280,208", "76.0", nullptr },
  { "Samoa", "192,342", "68.0", nullptr },
  { "Kiribati", "110,136", "152.0", nullptr },
  { nullptr, nullptr, nullptr, nullptr }
};

// constructors and destructor
//----------------------------------------------------------------------
Treeview::Treeview (finalcut::FWidget* parent)
  : finalcut::FDialog(parent)
{
  // Set FListView geometry
  listView.setGeometry(FPoint{2, 1}, FSize{53, 14});

  // Add columns to the view
  listView.addColumn ("Name", 23);
  listView.addColumn ("Population");
  listView.addColumn ("Density/km²");

  // Set right alignment for the second and third column
  listView.setColumnAlignment (2, fc::alignRight);
  listView.setColumnAlignment (3, fc::alignRight);

  // Set the type of sorting
  listView.setColumnSortType (1, fc::by_name);
  listView.setColumnSortType (2, fc::user_defined);
  listView.setColumnSortType (3, fc::user_defined);
  listView.setUserAscendingCompare(sortAscending);
  listView.setUserDescendingCompare(sortDescending);

  // Activate tree view
  listView.setTreeView();

  // Populate FListView with a list of items
  static TreeItem continent_list[] =
  {
    { "Africa", "944,000,000", "31.2", africa },
    { "Asia", "4,010,000,000", "90.3", asia },
    { "Europe", "733,000,000", "69.9", europe },
    { "North America", "523,000,000", "21", north_america },
    { "South America", "381,000,000", "21.4", south_america },
    { "Antarctica", "1000", "0", nullptr },
    { "Australia/Oceania", "34,000,000", "4", oceania }
  };

  for (const auto& continent : continent_list)
  {
    const TreeItem* country_list = continent.child_element;
    finalcut::FStringList continent_line ( continent.begin()
                                         , continent.end() );
    auto iter = listView.insert (continent_line);

    while ( country_list && country_list->name )
    {
      finalcut::FStringList country_line ( country_list->begin()
                                         , country_list->end() );
      listView.insert (country_line, iter);
      country_list++;
    }
  }

  // Quit button
  Quit.setGeometry(FPoint{24, 16}, FSize{10, 1});
  Quit.setText (L"&Quit");

  // Callback function
  Quit.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &finalcut::FApplication::cb_exitApp)
  );

  initialized = true;
}

//----------------------------------------------------------------------
Treeview::~Treeview()  // destructor
{ }

//----------------------------------------------------------------------
void Treeview::adjustSize()
{
  std::size_t h = getDesktopHeight() - 4;
  setHeight (h, false);
  int X = int((getDesktopWidth() - getWidth()) / 2);

  if ( X < 1 )
    X = 1;

  setX (X, false);

  if ( initialized )
  {
    listView.setHeight (getHeight() - 6, false);
    Quit.setY(int(getHeight()) - 4);
  }

  finalcut::FDialog::adjustSize();
}

//----------------------------------------------------------------------
void Treeview::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

int main (int argc, char* argv[])
{
  // Create the application object
  finalcut::FApplication app{argc, argv};

  // Create main dialog object
  Treeview d{&app};
  d.setText (L"Continents");
  d.setGeometry ( FPoint{int(1 + (app.getWidth() - 57) / 2), 3}
                , FSize{57, 20} );
  d.setShadow();

  // Set dialog d as main widget
  finalcut::FWidget::setMainWidget(&d);

  // Show and start the application
  d.show();
  return app.exec();
}
