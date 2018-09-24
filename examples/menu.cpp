/***********************************************************************
* menu.cpp - A menu example                                            *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2015-2018 Markus Gans                                      *
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

#include <final/final.h>


//----------------------------------------------------------------------
// class Menu
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class Menu : public finalcut::FDialog
{
  public:
    // Constructor
    explicit Menu (finalcut::FWidget* = 0);

    // Destructor
    ~Menu();

  private:
    // Disable copy constructor
    Menu (const Menu&);

    // Disable assignment operator (=)
    Menu& operator = (const Menu&);

    // Methods
    void createFileMenuItems (finalcut::FMenu*);
    void createEditMenuItems (finalcut::FMenu*);
    void createChoiceMenuItems (finalcut::FMenu*);
    void createColorMenuItems (finalcut::FMenu*);
    void createStyleMenuItems (finalcut::FMenu*);
    void createBorderMenuItems (finalcut::FMenu*);
    void createBorderColorMenuItems (finalcut::FMenu*);
    void createBorderStyleMenuItems (finalcut::FMenu*);
    void defaultCallback (finalcut::FMenuList*);
    virtual void adjustSize();

    // Event handler
    virtual void onClose (finalcut::FCloseEvent*);

    // Callback method
    void cb_message (finalcut::FWidget*, data_ptr);
};
#pragma pack(pop)

//----------------------------------------------------------------------
Menu::Menu (finalcut::FWidget* parent)
  : finalcut::FDialog(parent)
{
  // Menu bar
  finalcut::FMenuBar* Menubar = new finalcut::FMenuBar(this);

  // Menu bar items
  finalcut::FMenu* File = \
      new finalcut::FMenu ("&File", Menubar);
  File->setStatusbarMessage ("File management commands");
  finalcut::FMenu* Edit = \
      new finalcut::FMenu ("&Edit", Menubar);
  Edit->setStatusbarMessage ("Cut-and-paste editing commands");
  finalcut::FMenu* Choice = \
      new finalcut::FMenu ("&Choice", Menubar);
  Choice->setStatusbarMessage ("Choice menu");
  finalcut::FMenuItem* Window = \
      new finalcut::FMenuItem ("&Window", Menubar);
  Window->setDisable();
  finalcut::FMenuItem* Help = \
      new finalcut::FMenuItem ("&Help", Menubar);
  Help->setStatusbarMessage ("Show version and copyright information");

  // Menu items
  createFileMenuItems (File);
  createEditMenuItems (Edit);
  createChoiceMenuItems (Choice);

  // Add default menu item callback
  defaultCallback (Menubar);

  // Statusbar at the bottom
  finalcut::FStatusBar* Statusbar = \
      new finalcut::FStatusBar (this);
  Statusbar->setMessage("Status bar message");

  // Headline labels
  finalcut::FLabel* Headline1 = \
      new finalcut::FLabel(" Key ", this);
  Headline1->ignorePadding();
  Headline1->setGeometry(3, 2, 5, 1);
  Headline1->setEmphasis();

  finalcut::FLabel* Headline2 = \
      new finalcut::FLabel(" Function ", this);
  Headline2->ignorePadding();
  Headline2->setGeometry(19, 2, 10, 1);
  Headline2->setEmphasis();

  // Info label
  finalcut::FLabel* Info = \
      new finalcut::FLabel( "<F10>            Activate menu bar\n"
                            "<Ctrl>+<Space>   Activate menu bar\n"
                            "<Meta>+<X>       Exit", this );
  Info->setGeometry(2, 1, 36, 3);
}

//----------------------------------------------------------------------
Menu::~Menu()
{ }

//----------------------------------------------------------------------
void Menu::createFileMenuItems (finalcut::FMenu* File)
{
  // "File" menu items
  finalcut::FMenuItem* New = \
      new finalcut::FMenuItem ("&New", File);
  New->addAccelerator (finalcut::fc::Fckey_n);  // Ctrl + N
  New->setStatusbarMessage ("Create a new file");
  finalcut::FMenuItem* Open = \
      new finalcut::FMenuItem ("&Open...", File);
  Open->addAccelerator (finalcut::fc::Fckey_o);  // Ctrl + O
  Open->setStatusbarMessage ("Locate and open a text file");
  finalcut::FMenuItem* Save = \
      new finalcut::FMenuItem ("&Save", File);
  Save->addAccelerator (finalcut::fc::Fckey_s);  // Ctrl + S
  Save->setStatusbarMessage ("Save the file");
  finalcut::FMenuItem* SaveAs = \
      new finalcut::FMenuItem ("&Save as...", File);
  SaveAs->setStatusbarMessage ("Save the current file under a different name");
  finalcut::FMenuItem* Close = \
      new finalcut::FMenuItem ("&Close", File);
  Close->addAccelerator (finalcut::fc::Fckey_w);  // Ctrl + W
  Close->setStatusbarMessage ("Close the current file");
  finalcut::FMenuItem* Line1 = \
      new finalcut::FMenuItem (File);
  Line1->setSeparator();
  finalcut::FMenuItem* Print = \
      new finalcut::FMenuItem ("&Print", File);
  Print->addAccelerator (finalcut::fc::Fckey_p);  // Ctrl + P
  Print->setStatusbarMessage ("Print the current file");
  finalcut::FMenuItem* Line2 = \
      new finalcut::FMenuItem (File);
  Line2->setSeparator();
  finalcut::FMenuItem* Quit = \
      new finalcut::FMenuItem ("&Quit", File);
  Quit->addAccelerator (finalcut::fc::Fmkey_x);  // Meta/Alt + X
  Quit->setStatusbarMessage ("Exit the program");

  // Add quit menu item callback
  Quit->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &finalcut::FApplication::cb_exitApp)
  );
}

//----------------------------------------------------------------------
void Menu::createEditMenuItems (finalcut::FMenu* Edit)
{
  // "Edit" menu items
  finalcut::FMenuItem* Undo = \
      new finalcut::FMenuItem (finalcut::fc::Fckey_z, "&Undo", Edit);
  Undo->setStatusbarMessage ("Undo the previous operation");
  finalcut::FMenuItem* Redo = \
      new finalcut::FMenuItem (finalcut::fc::Fckey_y, "&Redo", Edit);
  Redo->setDisable();
  finalcut::FMenuItem* Line3 = \
      new finalcut::FMenuItem (Edit);
  Line3->setSeparator();
  finalcut::FMenuItem* Cut = \
      new finalcut::FMenuItem (finalcut::fc::Fckey_x, "Cu&t", Edit);
  Cut->setStatusbarMessage ( "Remove the input text "
                             "and put it in the clipboard" );
  finalcut::FMenuItem* Copy = \
      new finalcut::FMenuItem (finalcut::fc::Fckey_c, "&Copy", Edit);
  Copy->setStatusbarMessage ("Copy the input text into the clipboad");
  finalcut::FMenuItem* Paste = \
      new finalcut::FMenuItem (finalcut::fc::Fckey_v, "&Paste", Edit);
  Paste->setStatusbarMessage ("Insert text form clipboard");
  finalcut::FMenuItem* Line4 = \
      new finalcut::FMenuItem (Edit);
  Line4->setSeparator();
  finalcut::FMenuItem* Search = \
      new finalcut::FMenuItem (finalcut::fc::Fckey_f, "&Search", Edit);
  Search->setStatusbarMessage ("Search for text");
  finalcut::FMenuItem* Next = \
      new finalcut::FMenuItem (finalcut::fc::Fkey_f3, "Search &next", Edit);
  Next->setStatusbarMessage ("Repeat the last search command");
  finalcut::FMenuItem* Line5 = \
      new finalcut::FMenuItem (Edit);
  Line5->setSeparator();
  finalcut::FMenuItem* SelectAll = \
      new finalcut::FMenuItem (finalcut::fc::Fckey_a, "Select &all", Edit);
  SelectAll->setStatusbarMessage ("Select the whole text");
}

//----------------------------------------------------------------------
void Menu::createChoiceMenuItems (finalcut::FMenu* Choice)
{
  // "Choice" menu items
  finalcut::FMenu* Color = new finalcut::FMenu ("&Color", Choice);
  Color->setStatusbarMessage ("Choose a color");
  finalcut::FMenu* Style = new finalcut::FMenu ("&Style", Choice);
  Style->setStatusbarMessage ("Choose a Style");
  finalcut::FMenu* Border = new finalcut::FMenu ("&Border", Choice);
  Border->setStatusbarMessage ("Choose Border");

  createColorMenuItems (Color);
  createStyleMenuItems (Style);
  createBorderMenuItems (Border);
}

//----------------------------------------------------------------------
void Menu::createColorMenuItems (finalcut::FMenu* Color)
{
  // "Color" menu items
  finalcut::FRadioMenuItem* Color1 = \
      new finalcut::FRadioMenuItem ("Red", Color);
  Color1->setStatusbarMessage ("Set text red");
  finalcut::FRadioMenuItem* Color2 = \
      new finalcut::FRadioMenuItem ("Green", Color);
  Color2->setStatusbarMessage ("Set text green");
  finalcut::FRadioMenuItem* Color3 = \
      new finalcut::FRadioMenuItem ("Yellow", Color);
  Color3->setStatusbarMessage ("Set text yellow");
  finalcut::FRadioMenuItem* Color4 = \
      new finalcut::FRadioMenuItem ("Brue", Color);
  Color4->setStatusbarMessage ("Set text brue");
  finalcut::FRadioMenuItem* Color5 = \
      new finalcut::FRadioMenuItem ("Black", Color);
  Color5->setStatusbarMessage ("Set text black");
  Color5->setChecked();
}

//----------------------------------------------------------------------
void Menu::createStyleMenuItems (finalcut::FMenu* Style)
{
  // "Style" menu items
  finalcut::FCheckMenuItem* Bold = \
      new finalcut::FCheckMenuItem ("Bold", Style);
  Bold->setStatusbarMessage ("Set text bold");
  finalcut::FCheckMenuItem* Italic = \
      new finalcut::FCheckMenuItem ("Italic", Style);
  Italic->setStatusbarMessage ("Set text italic");
}

//----------------------------------------------------------------------
void Menu::createBorderMenuItems (finalcut::FMenu* Border)
{
  // "Border" menu items
  finalcut::FMenu* BColor = new finalcut::FMenu ("&Color", Border);
  BColor->setStatusbarMessage ("Choose the border color");
  finalcut::FMenu* BStyle = new finalcut::FMenu ("&Style", Border);
  BStyle->setStatusbarMessage ("Choose the border Style");

  createBorderColorMenuItems (BColor);
  createBorderStyleMenuItems (BStyle);
}

//----------------------------------------------------------------------
void Menu::createBorderColorMenuItems (finalcut::FMenu* BColor)
{
  // "BColor" menu items
  finalcut::FRadioMenuItem* BColor1 = \
      new finalcut::FRadioMenuItem ("Red", BColor);
  BColor1->setStatusbarMessage ("Set red border");
  finalcut::FRadioMenuItem* BColor2 = \
      new finalcut::FRadioMenuItem ("Blue", BColor);
  BColor2->setStatusbarMessage ("Set blue border");
}

//----------------------------------------------------------------------
void Menu::createBorderStyleMenuItems (finalcut::FMenu* BStyle)
{
  // "BStyle" menu items
  finalcut::FString line(13, wchar_t(finalcut::fc::BoxDrawingsHorizontal));
  finalcut::FRadioMenuItem* BStyle1 = \
      new finalcut::FRadioMenuItem (line, BStyle);
  BStyle1->setChecked();
  BStyle1->setStatusbarMessage ("Set border 1");
  finalcut::FRadioMenuItem* BStyle2 = \
      new finalcut::FRadioMenuItem ("-------------", BStyle);
  BStyle2->setStatusbarMessage ("Set border 2");
  finalcut::FRadioMenuItem* BStyle3 = \
      new finalcut::FRadioMenuItem ("- - - - - - -", BStyle);
  BStyle3->setStatusbarMessage ("Set border 3");
  finalcut::FRadioMenuItem* BStyle4 = \
      new finalcut::FRadioMenuItem ("-  -  -  -  -", BStyle);
  BStyle4->setStatusbarMessage ("Set border 4");
}

//----------------------------------------------------------------------
void Menu::defaultCallback (finalcut::FMenuList* mb)
{
  for (uInt i = 1; i <= mb->getCount(); i++)
  {
    finalcut::FMenuItem* item = mb->getItem(int(i));

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
        F_METHOD_CALLBACK (this, &Menu::cb_message)
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
  finalcut::FDialog::adjustSize();
}

//----------------------------------------------------------------------
void Menu::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void Menu::cb_message (finalcut::FWidget* widget, data_ptr)
{
  finalcut::FMenuItem* menuitem = \
      static_cast<finalcut::FMenuItem*>(widget);
  finalcut::FString text = menuitem->getText();
  text = text.replace('&', "");
  finalcut::FMessageBox::info ( this
                              , "Info"
                              , "You have chosen \"" + text + "\"" );
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

int main (int argc, char* argv[])
{
  // Create the application object
  finalcut::FApplication app (argc, argv);

  // Create main dialog object
  Menu main_dlg (&app);
  main_dlg.setText ("Menu example");
  main_dlg.setGeometry (int(1 + (app.getWidth() - 40) / 2), 2, 40, 6);
  main_dlg.setShadow();

  // Set dialog main_dlg as main widget
  app.setMainWidget (&main_dlg);

  // Show and start the application
  main_dlg.show();
  return app.exec();
}
