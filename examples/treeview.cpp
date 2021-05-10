/***********************************************************************
* treeview.cpp - Example of a FListView widget with a tree hierarchy   *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2021 Markus Gans                                      *
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

using finalcut::FPoint;
using finalcut::FSize;


// Function prototypes
sInt64 stringToNumber (const finalcut::FString&);
bool sortAscending (const finalcut::FObject*, const finalcut::FObject*);
bool sortDescending (const finalcut::FObject*, const finalcut::FObject*);
bool isLessThanInteger (const finalcut::FString&, const finalcut::FString&);
bool isLessThanDouble (const finalcut::FString&, const finalcut::FString&);
bool isGreaterThanInteger (const finalcut::FString&, const finalcut::FString&);
bool isGreaterThanDouble (const finalcut::FString&, const finalcut::FString&);


// non-member functions
//----------------------------------------------------------------------
sInt64 stringToNumber (const finalcut::FString& str)
{
  // Cut off one character (because LONG_MAX = 2147483647)
  auto num_string = str.left(str.getLength() - 1);
  num_string = num_string.replace(",", "");
  num_string = num_string.replace('.', "");
  auto number = sInt64(num_string.toLong());
  return number;
}

//----------------------------------------------------------------------
inline bool isLessThanInteger ( const finalcut::FString& lhs
                              , const finalcut::FString& rhs )
{
  const sInt64 l_number = stringToNumber(lhs);
  const sInt64 r_number = stringToNumber(rhs);
  return l_number < r_number;  // lhs < rhs
}

//----------------------------------------------------------------------
inline bool isLessThanDouble ( const finalcut::FString& lhs
                             , const finalcut::FString& rhs )
{
  std::setlocale(LC_NUMERIC, "C");
  const double l_number = lhs.toDouble();
  const double r_number = rhs.toDouble();
  return l_number < r_number;  // lhs < rhs
}

//----------------------------------------------------------------------
inline bool isGreaterThanInteger ( const finalcut::FString& lhs
                                 , const finalcut::FString& rhs )
{
  const sInt64 l_number = stringToNumber(lhs);
  const sInt64 r_number = stringToNumber(rhs);
  return l_number > r_number;  // lhs > rhs
}

//----------------------------------------------------------------------
inline bool isGreaterThanDouble ( const finalcut::FString& lhs
                         , const finalcut::FString& rhs )
{
  std::setlocale(LC_NUMERIC, "C");
  const double l_number = lhs.toDouble();
  const double r_number = rhs.toDouble();
  return l_number > r_number;  // lhs > rhs
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
    ~Treeview() override = default;

    // Disable copy assignment operator (=)
    Treeview& operator = (const Treeview&) = delete;

  private:
    // Typedefs
    struct TreeItem;  // forward declaration

    // Methods
    auto initAfrica() const -> std::initializer_list<TreeItem>;
    auto initAsia() const -> std::initializer_list<TreeItem>;
    auto initEurope() const -> std::initializer_list<TreeItem>;
    auto initNorthAmerica() const -> std::initializer_list<TreeItem>;
    auto initSouthAmerica() const -> std::initializer_list<TreeItem>;
    auto initOceania() const -> std::initializer_list<TreeItem>;
    void initLayout() override;
    void adjustSize() override;

    // Event handler
    void onClose (finalcut::FCloseEvent*) override;

    // Data members
    bool                  initialized{false};
    finalcut::FListView   listview{this};
    finalcut::FButton     quit{this};
    std::vector<TreeItem> africa{initAfrica()};
    std::vector<TreeItem> asia{initAsia()};
    std::vector<TreeItem> europe{initEurope()};
    std::vector<TreeItem> north_america{initNorthAmerica()};
    std::vector<TreeItem> south_america{initSouthAmerica()};
    std::vector<TreeItem> oceania{initOceania()};
};


//----------------------------------------------------------------------
// struct Treeview::TreeItem
//----------------------------------------------------------------------

struct Treeview::TreeItem
{
  using const_iterator = const char* const*;

  const_iterator begin() const noexcept
  { return &name; }

  const_iterator end() const noexcept
  { return std::next(&density); }

  // Data members
  const char* name;
  const char* population;
  const char* density;
  std::vector<TreeItem> child_element;
};


// constructors and destructor
//----------------------------------------------------------------------
Treeview::Treeview (finalcut::FWidget* parent)
  : finalcut::FDialog{parent}
{
  // Add columns to the view
  listview.addColumn ("Name", 23);
  listview.addColumn ("Population");
  listview.addColumn ("Density/km²");

  // Set right alignment for the second and third column
  listview.setColumnAlignment (2, finalcut::Align::Right);
  listview.setColumnAlignment (3, finalcut::Align::Right);

  // Set the type of sorting
  listview.setColumnSortType (1, finalcut::SortType::Name);
  listview.setColumnSortType (2, finalcut::SortType::UserDefined);
  listview.setColumnSortType (3, finalcut::SortType::UserDefined);
  listview.setUserAscendingCompare(sortAscending);
  listview.setUserDescendingCompare(sortDescending);

  // Activate tree view
  listview.setTreeView();

  // Populate FListView with a list of items
  const std::vector<TreeItem> continent_list
  {
    { "Africa", "944,000,000", "31.2", africa },
    { "Asia", "4,010,000,000", "90.3", asia },
    { "Europe", "733,000,000", "69.9", europe },
    { "North America", "523,000,000", "21", north_america },
    { "South America", "381,000,000", "21.4", south_america },
    { "Antarctica", "1000", "0", {} },
    { "Australia/Oceania", "34,000,000", "4", oceania }
  };

  for (const auto& continent : continent_list)
  {
    finalcut::FStringList continent_line ( continent.begin()
                                         , continent.end() );
    auto iter = listview.insert (continent_line);

    for (const auto& country : continent.child_element)
    {
      finalcut::FStringList country_line ( country.begin()
                                         , country.end() );
      listview.insert (country_line, iter);
    }
  }

  // Quit button text
  quit.setText (L"&Quit");

  // Callback function
  quit.addCallback
  (
    "clicked",
    finalcut::getFApplication(),
    &finalcut::FApplication::cb_exitApp,
    this
  );

  initialized = true;
}

//----------------------------------------------------------------------
auto Treeview::initAfrica() const -> std::initializer_list<Treeview::TreeItem>
{
  static const auto list = std::initializer_list<Treeview::TreeItem>
  {
    { "Algeria", "40,400,000", "15.9", {} },
    { "Angola", "25,789,024", "20.69", {} },
    { "Botswana", "2,250,260", "3.7", {} },
    { "Cameroon", "22,534,532", "39.7", {} },
    { "Chad", "13,670,084", "8.6", {} },
    { "Egypt", "94,666,000", "87", {} },
    { "Ethiopia", "102,374,044", "92.7", {} },
    { "Ivory Coast", "23,740,424", "63.9", {} },
    { "Libya", "6,541,948", "3.55", {} },
    { "Madagascar", "24,430,325", "35.2", {} },
    { "Mali", "14,517,176", "11.7", {} },
    { "Mauritania", "4,301,018", "3.4", {} },
    { "Mozambique", "24,692,144", "28.7", {} },
    { "Namibia", "2,113,077", "2.54", {} },
    { "Niger", "20,672,987", "12.1", {} },
    { "Nigeria", "185,989,640", "197.2", {} },
    { "Somalia", "14,317,996", "19.31", {} },
    { "South Africa", "54,956,900", "42.4", {} },
    { "South Sudan", "12,340,000", "13.33", {} },
    { "Sudan", "39,578,828", "21.3", {} },
    { "Tanzania", "51,820,00", "47.5", {} },
    { "Zambia", "16,212,000", "17.2", {} }
  };
  return list;
}

//----------------------------------------------------------------------
auto Treeview::initAsia() const -> std::initializer_list<Treeview::TreeItem>
{
  static const auto list = std::initializer_list<Treeview::TreeItem>
  {
    { "Afghanistan", "34,656,032", "49.88", {} },
    { "China", "1,403,500,365", "145.0", {} },
    { "India", "1,324,171,354", "393.9", {} },
    { "Indonesia", "261,115,456", "124.66", {} },
    { "Iran", "80,829,192", "48.0", {} },
    { "Iraq", "37,202,572", "82.7", {} },
    { "Japan", "126,740,000", "336.0", {} },
    { "Kazakhstan", "17,987,736", "6.49", {} },
    { "Mongolia", "3,081,677", "1.97", {} },
    { "Myanmar", "51,486,253", "76.0", {} },
    { "Pakistan", "207,774,520", "244.4", {} },
    { "Russia", "144,463,451", "8.4", {} },
    { "Saudi Arabia", "33,000,000", "15.0", {} },
    { "Thailand", "68,863,514", "132.1", {} },
    { "Turkey", "79,814,871", "102.0", {} },
    { "Turkmenistan", "5,662,544", "10.5", {} },
    { "Uzbekistan", "32,979,000", "70.5", {} },
    { "Vietnam", "94,569,072", "276.03", {} },
    { "Yemen", "27,584,213", "44.7", {} }
  };
  return list;
}

//----------------------------------------------------------------------
auto Treeview::initEurope() const -> std::initializer_list<Treeview::TreeItem>
{
  static const auto list = std::initializer_list<Treeview::TreeItem>
  {
    { "Austria", "8,794,267", "104.0", {} },
    { "Belarus", "9,498,700", "45.8", {} },
    { "Bulgaria", "7,101,859", "64.9", {} },
    { "Czech Republic", "10,610,947", "134.0", {} },
    { "Finland", "5,506,312", "16.0", {} },
    { "France", "66,991,000", "103.0", {} },
    { "Germany", "82,175,700", "227.0", {} },
    { "Greece", "11,183,716", "82.0", {} },
    { "Hungary", "9,797,561", "105.3", {} },
    { "Iceland", "332,529", "3.2", {} },
    { "Italy", "60,589,445", "201.3", {} },
    { "Norway", "5,267,146", "15.8", {} },
    { "Poland", "38,634,007", "123.0", {} },
    { "Portugal", "10,309,573", "115.0", {} },
    { "Romania", "19,638,000", "84.4", {} },
    { "Serbia", "7,058,322", "91.1", {} },
    { "Spain", "46,468,102", "92.0", {} },
    { "Sweden", "10,065,389", "22.0", {} },
    { "United Kingdom", "65,648,000", "270.7", {} }
  };
  return list;
}

//----------------------------------------------------------------------
auto Treeview::initNorthAmerica() const -> std::initializer_list<Treeview::TreeItem>
{
  static const auto list = std::initializer_list<Treeview::TreeItem>
  {
    { "Canada", "35,151,728", "3.92", {} },
    { "Cuba", "11,239,224", "102.3", {} },
    { "Greenland", "56,483", "0.028", {} },
    { "Guatemala", "16,582,469", "129.0", {} },
    { "Honduras", "9,112,867", "64.0", {} },
    { "Mexico", "119,530,753", "61.0", {} },
    { "Nicaragua", "6,167,237", "51.0", {} },
    { "USA", "325,365,189", "35.0", {} }
  };
  return list;
}

//----------------------------------------------------------------------
auto Treeview::initSouthAmerica() const -> std::initializer_list<Treeview::TreeItem>
{
  static const auto list = std::initializer_list<Treeview::TreeItem>
  {
    { "Argentina", "43,847,430", "14.4", {} },
    { "Bolivia", "11,410,651", "10.4", {} },
    { "Brazil", "208,064,000", "24.35", {} },
    { "Chile", "18,006,407", "24.0", {} },
    { "Colombia", "49,364,592", "40.74", {} },
    { "Ecuador", "16,385,068", "58.95", {} },
    { "Guyana", "773,303", "3.502", {} },
    { "Paraguay", "6,725,308", "17.2", {} },
    { "Peru", "31,826,018", "23.0", {} },
    { "Venezuela", "31,568,179", "33.75", {} }
  };
  return list;
}

//----------------------------------------------------------------------
auto Treeview::initOceania() const -> std::initializer_list<Treeview::TreeItem>
{
  static const auto list = std::initializer_list<Treeview::TreeItem>
  {
    { "Australia", "24,675,900", "3.2", {} },
    { "Papua New Guinea", "7,059,653", "15.0", {} },
    { "Papua", "3,486,432", "11.0", {} },
    { "New Zealand", "4,823,090", "17.5", {} },
    { "West Papua", "877,437", "6.3", {} },
    { "Solomon Islands", "599,419", "18.1", {} },
    { "New Caledonia", "268,767", "14.5", {} },
    { "Fiji", "898,76", "46.4", {} },
    { "Hawaii", "1,428,557", "82.6", {} },
    { "Vanuatu", "270,402", "19.7", {} },
    { "French Polynesia", "280,208", "76.0", {} },
    { "Samoa", "192,342", "68.0", {} },
    { "Kiribati", "110,136", "152.0", {} }
  };
  return list;
}

//----------------------------------------------------------------------
void Treeview::initLayout()
{
  // Set FListView geometry
  listview.setGeometry(FPoint{2, 1}, FSize{53, 14});
  // Set quit button geometry
  quit.setGeometry(FPoint{24, 16}, FSize{10, 1});
  FDialog::initLayout();
}

//----------------------------------------------------------------------
void Treeview::adjustSize()
{
  finalcut::FDialog::adjustSize();

  std::size_t h = getDesktopHeight() - 4;
  setHeight (h, false);
  auto x = int((getDesktopWidth() - getWidth()) / 2);

  if ( x < 1 )
    x = 1;

  setPos (FPoint{x, 3}, false);

  if ( initialized )
  {
    listview.setHeight (getHeight() - 6, true);
    quit.setY(int(getHeight()) - 4);
  }
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
  d.setSize (FSize{57, 20});
  d.setShadow();

  // Set dialog d as main widget
  finalcut::FWidget::setMainWidget(&d);

  // Show and start the application
  d.show();
  return app.exec();
}
