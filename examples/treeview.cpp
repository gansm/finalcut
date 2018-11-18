/***********************************************************************
* treeview.cpp - Example of a FListView widget with a tree hierarchy   *
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


// Function prototypes
long StringToLong (const finalcut::FString&);
bool sortAscending (const finalcut::FObject*, const finalcut::FObject*);
bool sortDescending (const finalcut::FObject*, const finalcut::FObject*);


// non-member functions
//----------------------------------------------------------------------
long StringToLong (const finalcut::FString& str)
{
  finalcut::FString NumString = str;
  NumString = NumString.replace(",", "");
  NumString = NumString.replace('.', "");
  long number = NumString.toLong();
  return number;
}

//----------------------------------------------------------------------
bool sortAscending ( const finalcut::FObject* lhs
                   , const finalcut::FObject* rhs )
{
  const finalcut::FListViewItem* l_item = static_cast<const finalcut::FListViewItem*>(lhs);
  const finalcut::FListViewItem* r_item = static_cast<const finalcut::FListViewItem*>(rhs);
  const int column = l_item->getSortColumn();

  switch ( column )
  {
    case 2:
    {
      const long l_number = StringToLong(l_item->getText(column));
      const long r_number = StringToLong(r_item->getText(column));
      return bool( l_number < r_number );  // lhs < rhs
    }
    case 3:
    {
      std::setlocale(LC_NUMERIC, "C");
      const double l_number = l_item->getText(column).toDouble();
      const double r_number = r_item->getText(column).toDouble();
      return bool( l_number < r_number );  // lhs < rhs
    }
  }

  return false;
}

//----------------------------------------------------------------------
bool sortDescending ( const finalcut::FObject* lhs
                    , const finalcut::FObject* rhs )
{
  const finalcut::FListViewItem* l_item = static_cast<const finalcut::FListViewItem*>(lhs);
  const finalcut::FListViewItem* r_item = static_cast<const finalcut::FListViewItem*>(rhs);
  const int column = l_item->getSortColumn();

  switch ( column )
  {
    case 2:
    {
      const long l_number = StringToLong(l_item->getText(column));
      const long r_number = StringToLong(r_item->getText(column));
      return bool( l_number > r_number );  // lhs > rhs
    }

    case 3:
    {
      std::setlocale(LC_NUMERIC, "C");
      const double l_number = l_item->getText(column).toDouble();
      const double r_number = r_item->getText(column).toDouble();
      return bool( l_number > r_number );  // lhs > rhs
    }
  }

  return false;
}


//----------------------------------------------------------------------
// class Treeview
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class Treeview : public finalcut::FDialog
{
  public:
    // Constructor
    explicit Treeview (finalcut::FWidget* = 0);
    // Destructor
    ~Treeview();

  private:
    // Typedefs
    struct TreeItem;  // forward declaration

    // Disable copy constructor
    Treeview (const Treeview&);
    // Disable assignment operator (=)
    Treeview& operator = (const Treeview&);

    // Methods
    virtual void adjustSize();

    // Event handler
    void onClose (finalcut::FCloseEvent*);

    // Data Members
    bool                initialized;
    finalcut::FListView listView;
    finalcut::FButton   Quit;
    static TreeItem     africa[];
    static TreeItem     asia[];
    static TreeItem     europe[];
    static TreeItem     north_america[];
    static TreeItem     south_america[];
    static TreeItem     oceania[];
};
#pragma pack(pop)


//----------------------------------------------------------------------
// struct Treeview::TreeItem
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)
struct Treeview::TreeItem
{
  const char* const* begin() const
  { return &name; }

  const char* const* end() const
  { return reinterpret_cast<const char* const*>(&child_element); }

  // Data Members
  const char* name;
  const char* population;
  const char* density;
  TreeItem*   child_element;
};
#pragma pack(pop)

//----------------------------------------------------------------------
// class Treeview - array data
//----------------------------------------------------------------------
Treeview::TreeItem Treeview::africa[] =
{
  { "Algeria", "40,400,000", "15.9", 0 },
  { "Angola", "25,789,024", "20.69", 0 },
  { "Botswana", "2,250,260", "3.7", 0 },
  { "Cameroon", "22,534,532", "39.7", 0 },
  { "Chad", "13,670,084", "8.6", 0 },
  { "Egypt", "94,666,000", "87", 0 },
  { "Ethiopia", "102,374,044", "92.7", 0 },
  { "Ivory Coast", "23,740,424", "63.9", 0 },
  { "Libya", "6,541,948", "3.55", 0 },
  { "Madagascar", "24,430,325", "35.2", 0 },
  { "Mali", "14,517,176", "11.7", 0 },
  { "Mauritania", "4,301,018", "3.4", 0 },
  { "Mozambique", "24,692,144", "28.7", 0 },
  { "Namibia", "2,113,077", "2.54", 0 },
  { "Niger", "20,672,987", "12.1", 0 },
  { "Nigeria", "185,989,640", "197.2", 0 },
  { "Somalia", "14,317,996", "19.31", 0 },
  { "South Africa", "54,956,900", "42.4", 0 },
  { "South Sudan", "12,340,000", "13.33", 0 },
  { "Sudan", "39,578,828", "21.3", 0 },
  { "Tanzania", "51,820,00", "47.5", 0 },
  { "Zambia", "16,212,000", "17.2", 0 },
  { 0, 0, 0, 0 }
};

Treeview::TreeItem Treeview::asia[] =
{
  { "Afghanistan", "34,656,032", "49.88", 0 },
  { "China", "1,403,500,365", "145.0", 0 },
  { "India", "1,324,171,354", "393.9", 0 },
  { "Indonesia", "261,115,456", "124.66", 0 },
  { "Iran", "80,829,192", "48.0", 0 },
  { "Iraq", "37,202,572", "82.7", 0 },
  { "Japan", "126,740,000", "336.0", 0 },
  { "Kazakhstan", "17,987,736", "6.49", 0 },
  { "Mongolia", "3,081,677", "1.97", 0 },
  { "Myanmar", "51,486,253", "76.0", 0 },
  { "Pakistan", "207,774,520", "244.4", 0 },
  { "Russia", "144,463,451", "8.4", 0 },
  { "Saudi Arabia", "33,000,000", "15.0", 0 },
  { "Thailand", "68,863,514", "132.1", 0 },
  { "Turkey", "79,814,871", "102.0", 0 },
  { "Turkmenistan", "5,662,544", "10.5", 0 },
  { "Uzbekistan", "32,979,000", "70.5", 0 },
  { "Vietnam", "94,569,072", "276.03", 0 },
  { "Yemen", "27,584,213", "44.7", 0 },
  { 0, 0, 0, 0 }
};

Treeview::TreeItem Treeview::europe[] =
{
  { "Austria", "8,794,267", "104.0", 0 },
  { "Belarus", "9,498,700", "45.8", 0 },
  { "Bulgaria", "7,101,859", "64.9", 0 },
  { "Czech Republic", "10,610,947", "134.0", 0 },
  { "Finland", "5,506,312", "16.0", 0 },
  { "France", "66,991,000", "103.0", 0 },
  { "Germany", "82,175,700", "227.0", 0 },
  { "Greece", "11,183,716", "82.0", 0 },
  { "Hungary", "9,797,561", "105.3", 0 },
  { "Iceland", "332,529", "3.2", 0 },
  { "Italy", "60,589,445", "201.3", 0 },
  { "Norway", "5,267,146", "15.8", 0 },
  { "Poland", "38,634,007", "123.0", 0 },
  { "Portugal", "10,309,573", "115.0", 0 },
  { "Romania", "19,638,000", "84.4", 0 },
  { "Serbia", "7,058,322", "91.1", 0 },
  { "Spain", "46,468,102", "92.0", 0 },
  { "Sweden", "10,065,389", "22.0", 0 },
  { "United Kingdom", "65,648,000", "270.7", 0 },
  { 0, 0, 0, 0 }
};


Treeview::TreeItem Treeview::north_america[] =
{
  { "Canada", "35,151,728", "3.92", 0 },
  { "Cuba", "11,239,224", "102.3", 0 },
  { "Greenland", "56,483", "0.028", 0 },
  { "Guatemala", "16,582,469", "129.0", 0 },
  { "Honduras", "9,112,867", "64.0", 0 },
  { "Mexico", "119,530,753", "61.0", 0 },
  { "Nicaragua", "6,167,237", "51.0", 0 },
  { "USA", "325,365,189", "35.0", 0 },
  { 0, 0, 0, 0 }
};

Treeview::TreeItem Treeview::south_america[] =
{
  { "Argentina", "43,847,430", "14.4", 0 },
  { "Bolivia", "11,410,651", "10.4", 0 },
  { "Brazil", "208,064,000", "24.35", 0 },
  { "Chile", "18,006,407", "24.0", 0 },
  { "Colombia", "49,364,592", "40.74", 0 },
  { "Ecuador", "16,385,068", "58.95", 0 },
  { "Guyana", "773,303", "3.502", 0 },
  { "Paraguay", "6,725,308", "17.2", 0 },
  { "Peru", "31,826,018", "23.0", 0 },
  { "Venezuela", "31,568,179", "33.75", 0 },
  { 0, 0, 0, 0 }
};

Treeview::TreeItem Treeview::oceania[] =
{
  { "Australia", "24,675,900", "3.2", 0 },
  { "Papua New Guinea", "7,059,653", "15.0", 0 },
  { "Papua", "3,486,432", "11.0", 0 },
  { "New Zealand", "4,823,090", "17.5", 0 },
  { "West Papua", "877,437", "6.3", 0 },
  { "Solomon Islands", "599,419", "18.1", 0 },
  { "New Caledonia", "268,767", "14.5", 0 },
  { "Fiji", "898,76", "46.4", 0 },
  { "Hawaii", "1,428,557", "82.6", 0 },
  { "Vanuatu", "270,402", "19.7", 0 },
  { "French Polynesia", "280,208", "76.0", 0 },
  { "Samoa", "192,342", "68.0", 0 },
  { "Kiribati", "110,136", "152.0", 0 },
  { 0, 0, 0, 0 }
};

// constructors and destructor
//----------------------------------------------------------------------
Treeview::Treeview (finalcut::FWidget* parent)
  : finalcut::FDialog(parent)
  , initialized(false)
  , listView(this)
  , Quit(this)
{
  // Set FListView geometry
  listView.setGeometry(2, 1, 53, 14);

  // Add columns to the view
  listView.addColumn ("Name", 23);
  listView.addColumn ("Population");
  listView.addColumn ("Density/km²");

  // Set right alignment for the second and third column
  listView.setColumnAlignment (2, finalcut::fc::alignRight);
  listView.setColumnAlignment (3, finalcut::fc::alignRight);

  // Set the type of sorting
  listView.setColumnSortType (1, finalcut::fc::by_name);
  listView.setColumnSortType (2, finalcut::fc::user_defined);
  listView.setColumnSortType (3, finalcut::fc::user_defined);
  listView.setUserAscendingCompare(sortAscending);
  listView.setUserDescendingCompare(sortDescending);

  // Activate tree view
  listView.setTreeView();

  // Populate FListView with a list of items
  static TreeItem continent[] =
  {
    { "Africa", "944,000,000", "31.2", africa },
    { "Asia", "4,010,000,000", "90.3", asia },
    { "Europe", "733,000,000", "69.9", europe },
    { "North America", "523,000,000", "21", north_america },
    { "South America", "381,000,000", "21.4", south_america },
    { "Antarctica", "1000", "0", 0 },
    { "Australia/Oceania", "34,000,000", "4", oceania },
    { 0, 0, 0, 0 }
  };

  TreeItem* continent_list = continent;

  while ( continent_list->name )
  {
    TreeItem* country_list = continent_list->child_element;
    finalcut::FStringList continent_line ( continent_list->begin()
                                         , continent_list->end() );
    finalcut::FListViewIterator::FObjectIterator iter = \
        listView.insert (continent_line);

    while ( country_list && country_list->name )
    {
      finalcut::FStringList country_line ( country_list->begin()
                                         , country_list->end() );
      listView.insert (country_line, iter);
      country_list++;
    }

    continent_list++;
  }

  // Quit button
  Quit.setGeometry(24, 16, 10, 1);
  Quit.setText (L"&Quit");

  // Add some function callbacks
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
  std::size_t h = getParentWidget()->getHeight() - 4;
  setHeight (h, false);
  int X = int((getParentWidget()->getWidth() - getWidth()) / 2);

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
  finalcut::FApplication app(argc, argv);

  // Create main dialog object
  Treeview d(&app);
  d.setText (L"Continents");
  d.setGeometry (int(1 + (app.getWidth() - 57) / 2), 3, 57, 20);
  d.setShadow();

  // Set dialog d as main widget
  app.setMainWidget(&d);

  // Show and start the application
  d.show();
  return app.exec();
}
