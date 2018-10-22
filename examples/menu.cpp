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
    void configureFileMenuItems();
    void configureEditMenuItems();
    void configureChoiceMenuItems();
    void configureColorMenuItems();
    void configureStyleMenuItems();
    void configureBorderMenuItems();
    void defaultCallback (finalcut::FMenuList*);
    virtual void adjustSize();

    // Event handler
    virtual void onClose (finalcut::FCloseEvent*);

    // Callback method
    void cb_message (finalcut::FWidget*, data_ptr);

    // Data Members
    finalcut::FString        line;
    finalcut::FMenuBar       Menubar;
    finalcut::FMenu          File;
    finalcut::FMenu          Edit;
    finalcut::FMenu          Choice;
    finalcut::FMenuItem      Window;
    finalcut::FMenuItem      Help;
    finalcut::FMenuItem      New;
    finalcut::FMenuItem      Open;
    finalcut::FMenuItem      Save;
    finalcut::FMenuItem      SaveAs;
    finalcut::FMenuItem      Close;
    finalcut::FMenuItem      Line1;
    finalcut::FMenuItem      Print;
    finalcut::FMenuItem      Line2;
    finalcut::FMenuItem      Quit;
    finalcut::FMenuItem      Undo;
    finalcut::FMenuItem      Redo;
    finalcut::FMenuItem      Line3;
    finalcut::FMenuItem      Cut;
    finalcut::FMenuItem      Copy;
    finalcut::FMenuItem      Paste;
    finalcut::FMenuItem      Line4;
    finalcut::FMenuItem      Search;
    finalcut::FMenuItem      Next;
    finalcut::FMenuItem      Line5;
    finalcut::FMenuItem      SelectAll;
    finalcut::FMenu          Color;
    finalcut::FMenu          Style;
    finalcut::FMenu          Border;
    finalcut::FRadioMenuItem Color1;
    finalcut::FRadioMenuItem Color2;
    finalcut::FRadioMenuItem Color3;
    finalcut::FRadioMenuItem Color4;
    finalcut::FRadioMenuItem Color5;
    finalcut::FCheckMenuItem Bold;
    finalcut::FCheckMenuItem Italic;
    finalcut::FMenu          BColor;
    finalcut::FMenu          BStyle;
    finalcut::FRadioMenuItem BColor1;
    finalcut::FRadioMenuItem BColor2;
    finalcut::FRadioMenuItem BStyle1;
    finalcut::FRadioMenuItem BStyle2;
    finalcut::FRadioMenuItem BStyle3;
    finalcut::FRadioMenuItem BStyle4;
    finalcut::FStatusBar     Statusbar;
    finalcut::FLabel         Headline1;
    finalcut::FLabel         Headline2;
    finalcut::FLabel         Info;
};
#pragma pack(pop)

//----------------------------------------------------------------------
Menu::Menu (finalcut::FWidget* parent)
  : finalcut::FDialog(parent)
  , line(13, wchar_t(finalcut::fc::BoxDrawingsHorizontal))
  , Menubar(this)
  , File("&File", &Menubar)
  , Edit("&Edit", &Menubar)
  , Choice("&Choice", &Menubar)
  , Window("&Window", &Menubar)
  , Help("&Help", &Menubar)
  , New("&New", &File)
  , Open("&Open...", &File)
  , Save("&Save", &File)
  , SaveAs("&Save as...", &File)
  , Close("&Close", &File)
  , Line1(&File)
  , Print("&Print", &File)
  , Line2(&File)
  , Quit("&Quit", &File)
  , Undo(finalcut::fc::Fckey_z, "&Undo", &Edit)
  , Redo(finalcut::fc::Fckey_y, "&Redo", &Edit)
  , Line3(&Edit)
  , Cut(finalcut::fc::Fckey_x, "Cu&t", &Edit)
  , Copy(finalcut::fc::Fckey_c, "&Copy", &Edit)
  , Paste(finalcut::fc::Fckey_v, "&Paste", &Edit)
  , Line4(&Edit)
  , Search(finalcut::fc::Fckey_f, "&Search", &Edit)
  , Next(finalcut::fc::Fkey_f3, "Search &next", &Edit)
  , Line5(&Edit)
  , SelectAll(finalcut::fc::Fckey_a, "Select &all", &Edit)
  , Color("&Color", &Choice)
  , Style("&Style", &Choice)
  , Border("&Border", &Choice)
  , Color1("Red", &Color)
  , Color2("Green", &Color)
  , Color3("Yellow", &Color)
  , Color4("Brue", &Color)
  , Color5("Black", &Color)
  , Bold("Bold", &Style)
  , Italic("Italic", &Style)
  , BColor("&Color", &Border)
  , BStyle("&Style", &Border)
  , BColor1("Red", &BColor)
  , BColor2("Blue", &BColor)
  , BStyle1(line, &BStyle)
  , BStyle2("-------------", &BStyle)
  , BStyle3("- - - - - - -", &BStyle)
  , BStyle4("-  -  -  -  -", &BStyle)
  , Statusbar(this)
  , Headline1(this)
  , Headline2(this)
  , Info(this)
{
  // Menu bar itms
  File.setStatusbarMessage ("File management commands");
  Edit.setStatusbarMessage ("Cut-and-paste editing commands");
  Choice.setStatusbarMessage ("Choice menu");
  Window.setDisable();
  Help.setStatusbarMessage ("Show version and copyright information");

  // Menu items
  configureFileMenuItems();
  configureEditMenuItems();
  configureChoiceMenuItems();

  // Add default menu item callback
  defaultCallback (&Menubar);

  // Statusbar at the bottom
  Statusbar.setMessage("Status bar message");

  // Headline labels
  Headline1 << " Key ";
  Headline1.ignorePadding();
  Headline1.setGeometry(3, 2, 5, 1);
  Headline1.setEmphasis();

  Headline2 << " Function ";
  Headline2.ignorePadding();
  Headline2.setGeometry(19, 2, 10, 1);
  Headline2.setEmphasis();

  // Info label
  Info << "<F10>            Activate menu bar\n"
       << "<Ctrl>+<Space>   Activate menu bar\n"
       << "<Meta>+<X>       Exit";
  Info.setGeometry(2, 1, 36, 3);
}

//----------------------------------------------------------------------
Menu::~Menu()
{ }

//----------------------------------------------------------------------
void Menu::configureFileMenuItems()
{
  // "File" menu items
  New.addAccelerator (finalcut::fc::Fckey_n);  // Ctrl + N
  New.setStatusbarMessage ("Create a new file");
  Open.addAccelerator (finalcut::fc::Fckey_o);  // Ctrl + O
  Open.setStatusbarMessage ("Locate and open a text file");
  Save.addAccelerator (finalcut::fc::Fckey_s);  // Ctrl + S
  Save.setStatusbarMessage ("Save the file");
  SaveAs.setStatusbarMessage ("Save the current file under a different name");
  Close.addAccelerator (finalcut::fc::Fckey_w);  // Ctrl + W
  Close.setStatusbarMessage ("Close the current file");
  Line1.setSeparator();
  Print.addAccelerator (finalcut::fc::Fckey_p);  // Ctrl + P
  Print.setStatusbarMessage ("Print the current file");
  Line2.setSeparator();
  Quit.addAccelerator (finalcut::fc::Fmkey_x);  // Meta/Alt + X
  Quit.setStatusbarMessage ("Exit the program");

  // Add quit menu item callback
  Quit.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &finalcut::FApplication::cb_exitApp)
  );
}

//----------------------------------------------------------------------
void Menu::configureEditMenuItems()
{
  // "Edit" menu items
  Undo.setStatusbarMessage ("Undo the previous operation");
  Redo.setDisable();
  Line3.setSeparator();
  Cut.setStatusbarMessage ( "Remove the input text "
                            "and put it in the clipboard" );
  Copy.setStatusbarMessage ("Copy the input text into the clipboad");
  Paste.setStatusbarMessage ("Insert text form clipboard");
  Line4.setSeparator();
  Search.setStatusbarMessage ("Search for text");
  Next.setStatusbarMessage ("Repeat the last search command");
  Line5.setSeparator();
  SelectAll.setStatusbarMessage ("Select the whole text");
}

//----------------------------------------------------------------------
void Menu::configureChoiceMenuItems()
{
  // "Choice" menu items
  Color.setStatusbarMessage ("Choose a color");
  Style.setStatusbarMessage ("Choose a Style");
  Border.setStatusbarMessage ("Choose Border");

  configureColorMenuItems();
  configureStyleMenuItems();
  configureBorderMenuItems();
}

//----------------------------------------------------------------------
void Menu::configureColorMenuItems()
{
  // "Color" menu items
  Color1.setStatusbarMessage ("Set text red");
  Color2.setStatusbarMessage ("Set text green");
  Color3.setStatusbarMessage ("Set text yellow");
  Color4.setStatusbarMessage ("Set text brue");
  Color5.setStatusbarMessage ("Set text black");
  Color5.setChecked();
}

//----------------------------------------------------------------------
void Menu::configureStyleMenuItems()
{
  // "Style" menu items
  Bold.setStatusbarMessage ("Set text bold");
  Italic.setStatusbarMessage ("Set text italic");
}

//----------------------------------------------------------------------
void Menu::configureBorderMenuItems()
{
  // "Border" menu items
  BColor.setStatusbarMessage ("Choose the border color");
  BStyle.setStatusbarMessage ("Choose the border Style");

  // "BColor" menu items
  BColor1.setStatusbarMessage ("Set red border");
  BColor2.setStatusbarMessage ("Set blue border");

  // "BStyle" menu items
  BStyle1.setChecked();
  BStyle1.setStatusbarMessage ("Set border 1");
  BStyle2.setStatusbarMessage ("Set border 2");
  BStyle3.setStatusbarMessage ("Set border 3");
  BStyle4.setStatusbarMessage ("Set border 4");
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
  int pw = int(getParentWidget()->getWidth());
  int ph = int(getParentWidget()->getHeight());
  setX (1 + (pw - int(getWidth())) / 2, false);
  setY (1 + (ph - int(getHeight())) / 4, false);
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
