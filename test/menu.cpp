// File: menu.cpp

#include "fapp.h"
#include "fcheckmenuitem.h"
#include "fdialog.h"
#include "flabel.h"
#include "fmenubar.h"
#include "fmenu.h"
#include "fmessagebox.h"
#include "fradiomenuitem.h"
#include "fstatusbar.h"


//----------------------------------------------------------------------
// class Menu
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class Menu : public FDialog
{
 public:
   // Constructor
   explicit Menu (FWidget* = 0);

   // Destructor
  ~Menu();

 private:
   // Disable copy constructor
   Menu (const Menu&);

   // Disable assignment operator (=)
   Menu& operator = (const Menu&);

   // Methods
   void defaultCallback (FMenuList*);
   void adjustSize();

   // Event handler
   void onClose (FCloseEvent*);

   // Callback methods
   void cb_message (FWidget*, data_ptr);
   void cb_exitApp (FWidget*, data_ptr);
};
#pragma pack(pop)

//----------------------------------------------------------------------
Menu::Menu (FWidget* parent)
  : FDialog(parent)
{
  // menu bar
  FMenuBar* Menubar  = new FMenuBar (this);

  // menu bar items
  FMenu* File        = new FMenu ("&File", Menubar);
  File->setStatusbarMessage ("File management commands");
  FMenu* Edit        = new FMenu ("&Edit", Menubar);
  Edit->setStatusbarMessage ("Cut-and-paste editing commands");
  FMenu* Choice      = new FMenu ("&Choice", Menubar);
  Choice->setStatusbarMessage ("Choice menu");
  FMenuItem* Window = new FMenuItem ("&Window", Menubar);
  Window->setDisable();
  FMenuItem* Help    = new FMenuItem ("&Help", Menubar);
  Help->setStatusbarMessage ("Show version and copyright information");

  // "File" menu items
  FMenuItem* New     = new FMenuItem ("&New", File);
  New->addAccelerator (fc::Fckey_n); // Ctrl + N
  New->setStatusbarMessage ("Create a new file");
  FMenuItem* Open    = new FMenuItem ("&Open...", File);
  Open->addAccelerator (fc::Fckey_o); // Ctrl + O
  Open->setStatusbarMessage ("Locate and open a text file");
  FMenuItem* Save    = new FMenuItem ("&Save", File);
  Save->addAccelerator (fc::Fckey_s); // Ctrl + S
  Save->setStatusbarMessage ("Save the file");
  FMenuItem* SaveAs  = new FMenuItem ("&Save as...", File);
  SaveAs->setStatusbarMessage ("Save the current file under a different name");
  FMenuItem* Close   = new FMenuItem ("&Close", File);
  Close->addAccelerator (fc::Fckey_w); // Ctrl + W
  Close->setStatusbarMessage ("Close the current file");
  FMenuItem* Line1   = new FMenuItem (File);
  Line1->setSeparator();
  FMenuItem* Print   = new FMenuItem ("&Print", File);
  Print->addAccelerator (fc::Fckey_p); // Ctrl + P
  Print->setStatusbarMessage ("Print the current file");
  FMenuItem* Line2   = new FMenuItem (File);
  Line2->setSeparator();
  FMenuItem* Quit    = new FMenuItem ("&Quit", File);
  Quit->addAccelerator (fc::Fmkey_x); // Meta/Alt + X
  Quit->setStatusbarMessage ("Exit the program");

  // "Edit" menu items
  FMenuItem* Undo    = new FMenuItem (fc::Fckey_z, "&Undo", Edit);
  Undo->setStatusbarMessage ("Undo the previous operation");
  FMenuItem* Redo    = new FMenuItem (fc::Fckey_y, "&Redo", Edit);
  Redo->setDisable();
  FMenuItem* Line3   = new FMenuItem (Edit);
  Line3->setSeparator();
  FMenuItem* Cut     = new FMenuItem (fc::Fckey_x, "Cu&t", Edit);
  Cut->setStatusbarMessage ("Remove the input text and put it in the clipboard");
  FMenuItem* Copy    = new FMenuItem (fc::Fckey_c, "&Copy", Edit);
  Copy->setStatusbarMessage ("Copy the input text into the clipboad");
  FMenuItem* Paste   = new FMenuItem (fc::Fckey_v, "&Paste", Edit);
  Paste->setStatusbarMessage ("Insert text form clipboard");
  FMenuItem* Line4   = new FMenuItem (Edit);
  Line4->setSeparator();
  FMenuItem* Search  = new FMenuItem (fc::Fckey_f, "&Search", Edit);
  Search->setStatusbarMessage ("Search for text");
  FMenuItem* Next    = new FMenuItem (fc::Fkey_f3, "Search &next", Edit);
  Next->setStatusbarMessage ("Repeat the last search command");
  FMenuItem* Line5   = new FMenuItem (Edit);
  Line5->setSeparator();
  FMenuItem* SelectAll = new FMenuItem (fc::Fckey_a, "Select &all", Edit);
  SelectAll->setStatusbarMessage ("Select the whole text");

  // "Choice" menu items
  FMenu* Color  = new FMenu ("&Color", Choice);
  Color->setStatusbarMessage ("Choose a color");
  FMenu* Style  = new FMenu ("&Style", Choice);
  Style->setStatusbarMessage ("Choose a Style");
  FMenu* Border = new FMenu ("&Border", Choice);
  Border->setStatusbarMessage ("Choose Border");

  // "Color" menu items
  FRadioMenuItem* Color1 = new FRadioMenuItem ("Red", Color);
  Color1->setStatusbarMessage ("Set text red");
  FRadioMenuItem* Color2 = new FRadioMenuItem ("Green", Color);
  Color2->setStatusbarMessage ("Set text green");
  FRadioMenuItem* Color3 = new FRadioMenuItem ("Yellow", Color);
  Color3->setStatusbarMessage ("Set text yellow");
  FRadioMenuItem* Color4 = new FRadioMenuItem ("Brue", Color);
  Color4->setStatusbarMessage ("Set text brue");
  FRadioMenuItem* Color5 = new FRadioMenuItem ("Black", Color);
  Color5->setStatusbarMessage ("Set text black");
  Color5->setChecked();

  // "Style" menu items
  FCheckMenuItem* Bold = new FCheckMenuItem ("Bold", Style);
  Bold->setStatusbarMessage ("Set text bold");
  FCheckMenuItem* Italic = new FCheckMenuItem ("Italic", Style);
  Italic->setStatusbarMessage ("Set text italic");

  // "Border" menu items
  FMenu* BColor  = new FMenu ("&Color", Border);
  BColor->setStatusbarMessage ("Choose the border color");
  FMenu* BStyle  = new FMenu ("&Style", Border);
  BStyle->setStatusbarMessage ("Choose the border Style");

  // "BColor" menu items
  FRadioMenuItem* BColor1 = new FRadioMenuItem ("Red", BColor);
  BColor1->setStatusbarMessage ("Set red border");
  FRadioMenuItem* BColor2 = new FRadioMenuItem ("Blue", BColor);
  BColor2->setStatusbarMessage ("Set blue border");

  // "BStyle" menu items
  FString line(13, wchar_t(fc::BoxDrawingsHorizontal));
  FRadioMenuItem* BStyle1 = new FRadioMenuItem (line, BStyle);
  BStyle1->setChecked();
  BStyle1->setStatusbarMessage ("Set border 1");
  FRadioMenuItem* BStyle2 = new FRadioMenuItem ("-------------", BStyle);
  BStyle2->setStatusbarMessage ("Set border 2");
  FRadioMenuItem* BStyle3 = new FRadioMenuItem ("- - - - - - -", BStyle);
  BStyle3->setStatusbarMessage ("Set border 3");
  FRadioMenuItem* BStyle4 = new FRadioMenuItem ("-  -  -  -  -", BStyle);
  BStyle4->setStatusbarMessage ("Set border 4");

  // Add default menu item callback
  defaultCallback (Menubar);

  // Add quit menu item callback
  Quit->addCallback
  (
    "clicked",
    _METHOD_CALLBACK (this, &Menu::cb_exitApp)
  );

  // Statusbar at the bottom
  FStatusBar* statusbar = new FStatusBar (this);
  statusbar->setMessage("Status bar message");

  // Headline labels
  FLabel* Headline1 = new FLabel(" Key ", this);
  Headline1->ignorePadding();
  Headline1->setGeometry(3,2,5,1);
  Headline1->setEmphasis();

  FLabel* Headline2 = new FLabel(" Function ", this);
  Headline2->ignorePadding();
  Headline2->setGeometry(19,2,10,1);
  Headline2->setEmphasis();

  // Info label
  FLabel* Info = new FLabel("<F10>            Activate menu bar\n"
                            "<Ctrl>+<Space>   Activate menu bar\n"
                            "<Meta>+<X>       Exit", this);
  Info->setGeometry(2,1,36,3);
}

//----------------------------------------------------------------------
Menu::~Menu()
{ }

//----------------------------------------------------------------------
void Menu::defaultCallback (FMenuList* mb)
{
  for (uInt i=1; i <= mb->getCount(); i++)
  {
    FMenuItem* item = mb->getItem(int(i));

    if ( item
        && item->isEnabled()
        && item->acceptFocus()
        && item->isVisible()
        && ! item->isSeparator()
        && item->getText() != "&Quit" )
    {
      // Add the callback function
      item->addCallback
      (
        "clicked",
        _METHOD_CALLBACK (this, &Menu::cb_message)
      );

      // Call sub-menu
      if ( item->hasMenu() )
        defaultCallback (item->getMenu());
    }
  }
}

//----------------------------------------------------------------------
void Menu::adjustSize()
{
  int pw = getParentWidget()->getWidth();
  int ph = getParentWidget()->getHeight();
  setX (1 + (pw - getWidth()) / 2, false);
  setY (1 + (ph - getHeight()) / 4, false);
  FDialog::adjustSize();
}

//----------------------------------------------------------------------
void Menu::onClose (FCloseEvent* ev)
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
void Menu::cb_message (FWidget* widget, data_ptr)
{
  FMenuItem* menuitem = static_cast<FMenuItem*>(widget);
  FString text = menuitem->getText();
  text = text.replace('&', "");
  FMessageBox::info (this, "Info", "You have chosen \"" + text + "\"");
}

//----------------------------------------------------------------------
void Menu::cb_exitApp (FWidget*, data_ptr)
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

  FApplication app (argc, argv);

  Menu main_dlg (&app);
  main_dlg.setText ("Menu example");
  main_dlg.setGeometry (int(1+(app.getWidth()-40)/2), 2, 40, 6);
  main_dlg.setShadow();

  app.setMainWidget (&main_dlg);
  main_dlg.show();
  return app.exec();
}
