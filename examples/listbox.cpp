// File: listbox.cpp

#include <iostream>
#include <list>
#include <map>
#include <fstream>

#include <final/fapplication.h>
#include <final/fdialog.h>
#include <final/flistbox.h>
#include <final/fmessagebox.h>


// Global application object
static FString* temp_str = 0;

// Function prototypes
void doubleToItem (FListBoxItem&, FWidget::data_ptr container, int index);
FString& doubleToString (std::list<double>::const_iterator iter);
FString& mapToString (std::map<FString, FString>::const_iterator iter);

// Lazy conversion import function
void doubleToItem (FListBoxItem& item, FWidget::data_ptr container, int index)
{
  typedef std::list<double>* double_list_ptr;
  double_list_ptr dbllist = static_cast<double_list_ptr>(container);
  std::list<double>::iterator iter = dbllist->begin();
  std::advance (iter, index);
  item.setText (FString().setNumber(*iter));
  item.setData (FWidget::data_ptr(&(*iter)));
}

// Import converter functions
FString& doubleToString (std::list<double>::const_iterator iter)
{
  return temp_str->setNumber(*iter);
}

FString& mapToString (std::map<FString, FString>::const_iterator iter)
{
  return *temp_str = iter->first + ": " + iter->second;
}


//----------------------------------------------------------------------
// class Listbox
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class Listbox : public FDialog
{
  public:
    // Constructor
    explicit Listbox (FWidget* = 0);
    // Destructor
    ~Listbox();

  private:
    // Disable copy constructor
    Listbox (const Listbox&);
    // Disable assignment operator (=)
    Listbox& operator = (const Listbox&);

    // Event handlers
    void onClose (FCloseEvent*);

    // Callback method
    void cb_exitApp (FWidget*, data_ptr);

    // Data Member
    std::list<double>* double_list;
};
#pragma pack(pop)

//----------------------------------------------------------------------
Listbox::Listbox (FWidget* parent)
  : FDialog(parent)
  , double_list()
{
  temp_str = new FString;

  // listbox 1
  FListBox* list1 = new FListBox (this);
  list1->setGeometry(2, 1, 18, 10);
  list1->setText ("FListBoxItem");

  for (int i=1; i < 30; i++)
    list1->insert (L"----- " + FString().setNumber(i) + L" -----");

  // listbox 2
  double_list = new std::list<double>;

  for (double i=1; i<=15; i++)
    double_list->push_back(2 * i + (i / 100));

  FListBox* list2 = new FListBox (this);
  list2->setGeometry(21, 1, 10, 10);
  list2->setText ("double");

  //
  // Import via lazy conversion on print
  //
  list2->insert (double_list, doubleToItem);

  //
  // Direct import of the complete list
  //
  //list2->insert (double_list->begin(), double_list->end(), doubleToString);

  // listbox 3
  std::map<FString, FString> TLD;
  TLD["com"] = "Commercial";
  TLD["org"] = "Organization";
  TLD["net"] = "Network";
  TLD["edu"] = "Education";
  TLD["gov"] = "Government";

  FListBox* list3;
  list3 = new FListBox (TLD.begin(), TLD.end(), mapToString, this);
  list3->setGeometry(32, 1, 21, 10);
  list3->setText ("key: value");

  // Quit button
  FButton* Quit = new FButton (this);
  Quit->setGeometry(42, 12, 10, 1);
  Quit->setText (L"&Quit");

  // Add quit button function callback
  Quit->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &Listbox::cb_exitApp)
  );
}

//----------------------------------------------------------------------
Listbox::~Listbox()  // destructor
{
  delete temp_str;
  delete double_list;
}

//----------------------------------------------------------------------
void Listbox::onClose (FCloseEvent* ev)
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
void Listbox::cb_exitApp (FWidget*, data_ptr)
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

  Listbox d(&app);
  d.setText (L"Listbox");
  d.setGeometry (int(1 + (app.getWidth() - 56) / 2), 5, 56, 16);
  d.setShadow();

  app.setMainWidget(&d);
  d.show();
  return app.exec();
}
