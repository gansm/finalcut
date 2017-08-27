// File: listview.cpp

#include <iostream>
#include <fstream>

#include "fapplication.h"
#include "fdialog.h"
#include "flistview.h"
#include "fmessagebox.h"


//----------------------------------------------------------------------
// class Listview
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class Listview : public FDialog
{
 public:
   // Constructor
   explicit Listview (FWidget* = 0);
   // Destructor
  ~Listview();

 private:
   // Disable copy constructor
   Listview (const Listview&);
   // Disable assignment operator (=)
   Listview& operator = (const Listview&);

   // Event handlers
   void onClose (FCloseEvent*);

   // Callback methods
   void cb_exitApp (FWidget*, data_ptr);
   void cb_showInMessagebox (FWidget*, data_ptr);
};
#pragma pack(pop)

//----------------------------------------------------------------------
Listview::Listview (FWidget* parent)
  : FDialog(parent)
{
  // Create FListView object
  FListView* listView = new FListView (this);
  listView->setGeometry(2, 1, 33, 14);

  // Add columns to the view
  listView->addColumn ("City");
  listView->addColumn ("Condition");
  listView->addColumn ("Temp.");
  listView->addColumn ("Humidity");
  listView->addColumn ("Pressure", 10);

  // Set right alignment for the third, fourth, and fifth column
  listView->setColumnAlignment (3, fc::alignRight);
  listView->setColumnAlignment (4, fc::alignRight);
  listView->setColumnAlignment (5, fc::alignRight);

  // Populate FListView with a list of items
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
    std::vector<FString> line (&weather[i][0], &weather[i][0] + 5);
    listView->insert (line);
  }

  // Quit button
  FButton* Quit = new FButton (this);
  Quit->setGeometry(24, 16, 10, 1);
  Quit->setText (L"&Quit");

  // Add some function callbacks
  Quit->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &Listview::cb_exitApp)
  );

  listView->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &Listview::cb_showInMessagebox)
  );
}

//----------------------------------------------------------------------
Listview::~Listview()
{ }

//----------------------------------------------------------------------
void Listview::onClose (FCloseEvent* ev)
{
  int ret = FMessageBox::info ( this, "Quit"
                              , "Do you really want\n"
                                "to quit the program ?"
                              , FMessageBox::Yes
                              , FMessageBox::No );
  if ( ret == FMessageBox::Yes )
    ev->accept();
  else
    ev->ignore();
}

//----------------------------------------------------------------------
void Listview::cb_exitApp (FWidget*, data_ptr)
{
  close();
}

//----------------------------------------------------------------------
void Listview::cb_showInMessagebox (FWidget* widget, data_ptr)
{
  FListView* listView = static_cast<FListView*>(widget);
  FListViewItem* item = listView->getCurrentItem();
  FMessageBox info ( "Weather in " + item->getText(0)
                   , "  Condition: " + item->getText(1) + "\n"
                     "Temperature: " + item->getText(2) + "\n"
                     "   Humidity: " + item->getText(3) + "\n"
                     "   Pressure: " + item->getText(4)
                   , FMessageBox::Ok, 0, 0, this );
  info.show();
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

int main (int argc, char* argv[])
{
  if ( argv[1] && ( std::strcmp(argv[1], "--help") == 0
                   || std::strcmp(argv[1], "-h") == 0 ) )
  {
    std::cout << "Generic options:" << std::endl
              << "  -h, --help                  "
              << "Display this help and exit" << std::endl;
    FApplication::print_cmd_Options();
    std::exit(EXIT_SUCCESS);
  }

  FApplication app(argc, argv);

  Listview d(&app);
  d.setText (L"Weather data");
  d.setGeometry (int(1 + (app.getWidth() - 37) / 2), 3, 37, 20);
  d.setShadow();

  app.setMainWidget(&d);
  d.show();
  return app.exec();
}
