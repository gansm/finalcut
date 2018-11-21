/***********************************************************************
* listview.cpp - Example for using a multi-column FListView widget     *
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
// class Listview
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class Listview : public finalcut::FDialog
{
  public:
    // Constructor
    explicit Listview (finalcut::FWidget* = 0);
    // Destructor
    ~Listview();

  private:
    // Disable copy constructor
    Listview (const Listview&);
    // Disable assignment operator (=)
    Listview& operator = (const Listview&);

    // Method
    void populate();

    // Event handlers
    virtual void onClose (finalcut::FCloseEvent*);

    // Callback method
    void cb_showInMessagebox (finalcut::FWidget*, data_ptr);

    // Data Members
    finalcut::FListView listView;
    finalcut::FButton   Quit;
};
#pragma pack(pop)

//----------------------------------------------------------------------
Listview::Listview (finalcut::FWidget* parent)
  : finalcut::FDialog(parent)
  , listView(this)
  , Quit(this)
{
  // Set FListView geometry
  listView.setGeometry(2, 1, 33, 14);

  // Add columns to the view
  listView.addColumn ("City");
  listView.addColumn ("Condition");
  listView.addColumn ("Temp.");
  listView.addColumn ("Humidity");
  listView.addColumn ("Pressure", 10);

  // Set right alignment for the third, fourth, and fifth column
  listView.setColumnAlignment (3, finalcut::fc::alignRight);
  listView.setColumnAlignment (4, finalcut::fc::alignRight);
  listView.setColumnAlignment (5, finalcut::fc::alignRight);

  // Set the type of sorting
  listView.setColumnSortType (1, finalcut::fc::by_name);
  listView.setColumnSortType (2, finalcut::fc::by_name);
  listView.setColumnSortType (3, finalcut::fc::by_number);
  listView.setColumnSortType (4, finalcut::fc::by_number);
  listView.setColumnSortType (5, finalcut::fc::by_number);

  // Sort in ascending order by the 1st column
  listView.setColumnSort (1, finalcut::fc::ascending);
  // Sorting follows later automatically on insert().
  // Otherwise you could start the sorting directly with sort()

  // Allways show the sort indicator (▼/▲)
  listView.hideSortIndicator(false);

  // Populate FListView with a list of items
  populate();

  // Quit button
  Quit.setGeometry(24, 16, 10, 1);
  Quit.setText (L"&Quit");

  // Add some function callbacks
  Quit.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &finalcut::FApplication::cb_exitApp)
  );

  listView.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &Listview::cb_showInMessagebox)
  );
}

//----------------------------------------------------------------------
Listview::~Listview()  // destructor
{ }

//----------------------------------------------------------------------
void Listview::populate()
{
  std::string weather[][5] =
  {
    { "Alexandria", "Sunny", "31°C", "61%", "1006.4 mb" },
    { "Amsterdam", "Cloudy", "21°C", "82%", "1021.3 mb" },
    { "Baghdad", "Fair", "47°C", "9%", "1001.0 mb" },
    { "Bangkok", "Partly Cloudy", "30°C", "69%", "1002.0 mb" },
    { "Beijing", "Fair", "31°C", "68%", "1007.1 mb" },
    { "Berlin", "Cloudy", "22°C", "53%", "1022.0 mb" },
    { "Bogotá", "Fair", "9°C", "95%", "1028.5 mb" },
    { "Budapest", "Partly Cloudy", "23°C", "37%", "1020.7 mb" },
    { "Buenos Aires", "Cloudy", "7°C", "73%", "1019.0 mb" },
    { "Cairo", "Fair", "39°C", "22%", "1006.1 mb" },
    { "Cape Town", "Partly Cloudy", "12°C", "45%", "1030.1 mb" },
    { "Chicago", "Mostly Cloudy", "21°C", "81%", "1014.9 mb" },
    { "Delhi", "Haze", "33°C", "68%", "998.0 mb" },
    { "Dhaka", "Haze", "32°C", "64%", "996.3 mb" },
    { "Houston", "Cloudy", "23°C", "100%", "1014.2 mb" },
    { "Istanbul", "Mostly Cloudy", "27°C", "61%", "1011.2 mb" },
    { "Jakarta", "Fair", "28°C", "71%", "1009.1 mb" },
    { "Jerusalem", "Sunny", "35°C", "17%", "1005.8 mb" },
    { "Johannesburg", "Fair", "18°C", "16%", "1020.0 mb" },
    { "Karachi", "Mostly Cloudy", "29°C", "76%", "998.0 mb" },
    { "Lagos", "Mostly Cloudy", "27°C", "86%", "1014.6 mb" },
    { "Lima", "Cloudy", "17°C", "83%", "1017.3 mb" },
    { "London", "Cloudy", "23°C", "71%", "1023.0 mb" },
    { "Los Angeles", "Fair", "21°C", "78%", "1011.9 mb" },
    { "Madrid", "Fair", "32°C", "35%", "1020.0 mb" },
    { "Mexico City", "Partly Cloudy", "14°C", "79%", "1028.5 mb" },
    { "Moscow", "Partly Cloudy", "24°C", "54%", "1014.2 mb" },
    { "Mumbai", "Haze", "28°C", "77%", "1003.0 mb" },
    { "New York City", "Sunny", "21°C", "80%", "1014.2 mb" },
    { "Paris", "Partly Cloudy", "27°C", "57%", "1024.4 mb" },
    { "Reykjavík", "Mostly Cloudy", "11°C", "76%", "998.6 mb" },
    { "Rio de Janeiro", "Fair", "24°C", "64%", "1022.0 mb" },
    { "Rome", "Fair", "32°C", "18%", "1014.2 mb" },
    { "Saint Petersburg", "Mostly Cloudy", "18°C", "55%", "1014.6 mb" },
    { "São Paulo", "Fair", "19°C", "53%", "1024.0 mb" },
    { "Seoul", "Cloudy", "26°C", "87%", "1012.2 mb" },
    { "Shanghai", "Fair", "32°C", "69%", "1009.1 mb" },
    { "Singapore", "Mostly Cloudy", "29°C", "73%", "1009.1 mb" },
    { "Tehran", "Fair", "36°C", "14%", "1013.2 mb" },
    { "Tokyo", "Mostly Cloudy", "28°C", "67%", "1009.1 mb" },
    { "Zurich", "Mostly Cloudy", "23°C", "44%", "1023.7 mb" }
  };

  const int lastItem = int(sizeof(weather) / sizeof(weather[0])) - 1;

  for (int i = 0; i <= lastItem; i++)
  {
    finalcut::FStringList line (&weather[i][0], &weather[i][0] + 5);
    listView.insert (line);
  }
}

//----------------------------------------------------------------------
void Listview::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void Listview::cb_showInMessagebox (finalcut::FWidget*, data_ptr)
{
  finalcut::FListViewItem* item = listView.getCurrentItem();
  finalcut::FMessageBox info ( "Weather in " + item->getText(1)
                             , "  Condition: " + item->getText(2) + "\n"
                               "Temperature: " + item->getText(3) + "\n"
                               "   Humidity: " + item->getText(4) + "\n"
                               "   Pressure: " + item->getText(5)
                             , finalcut::FMessageBox::Ok, 0, 0, this );
  info.show();
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

int main (int argc, char* argv[])
{
  // Create the application object
  finalcut::FApplication app(argc, argv);

  // Create main dialog object
  Listview d(&app);
  d.setText (L"Weather data");
  d.setGeometry (int(1 + (app.getWidth() - 37) / 2), 3, 37, 20);
  d.setShadow();

  // Set dialog d as main widget
  app.setMainWidget(&d);

  // Show and start the application
  d.show();
  return app.exec();
}
