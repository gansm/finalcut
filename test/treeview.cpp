// File: treeview.cpp

#include <iostream>
#include <fstream>

#include "fapplication.h"
#include "fdialog.h"
#include "flistview.h"
#include "fmessagebox.h"


//----------------------------------------------------------------------
// class Treeview
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class Treeview : public FDialog
{
 public:
   // Constructor
   explicit Treeview (FWidget* = 0);
   // Destructor
  ~Treeview();

 private:
   // Disable copy constructor
   Treeview (const Treeview&);
   // Disable assignment operator (=)
   Treeview& operator = (const Treeview&);

   // Event handlers
   void onClose (FCloseEvent*);

   // Callback methods
   void cb_exitApp (FWidget*, data_ptr);
};
#pragma pack(pop)

//----------------------------------------------------------------------
Treeview::Treeview (FWidget* parent)
  : FDialog(parent)
{
  // Create FListView object
  FListView* listView = new FListView (this);
  listView->setGeometry(2, 1, 33, 14);

  // Add columns to the view
  listView->addColumn ("Name");
  listView->addColumn ("Population");
  listView->addColumn ("Density/km²");

  // Set right alignment for the second and third column
  listView->setColumnAlignment (2, fc::alignRight);
  listView->setColumnAlignment (3, fc::alignRight);

  // Populate FListView with a list of items
  std::string continent[][3] =
  {
    { "Africa", "944,000,000", "31.2" },
    { "Asia", "4,010,000,000", "90.3" },
    { "Europe", "733,000,000", "69.9" },
    { "North America", "523,000,000", "21" },
    { "South America", "381,000,000", "21.4" },
    { "Antarctica", "1000", "0" },
    { "Australia/Oceania", "34,000,000", "4" }
  };

  const int lastItem = int(sizeof(continent) / sizeof(continent[0])) - 1;

  for (int i = 0; i <= lastItem; i++)
  {
    std::vector<FString> line (&continent[i][0], &continent[i][0] + 3);
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
    F_METHOD_CALLBACK (this, &Treeview::cb_exitApp)
  );
}

//----------------------------------------------------------------------
Treeview::~Treeview()
{ }

//----------------------------------------------------------------------
void Treeview::onClose (FCloseEvent* ev)
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
void Treeview::cb_exitApp (FWidget*, data_ptr)
{
  close();
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

  Treeview d(&app);
  d.setText (L"Continents");
  d.setGeometry (int(1 + (app.getWidth() - 37) / 2), 3, 37, 20);
  d.setShadow();

  app.setMainWidget(&d);
  d.show();
  return app.exec();
}
