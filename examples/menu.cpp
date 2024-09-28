/***********************************************************************
* menu.cpp - A menu example                                            *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2023 Markus Gans                                      *
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

#include <final/final.h>

using FKey = finalcut::FKey;
using finalcut::FPoint;
using finalcut::FSize;


//----------------------------------------------------------------------
// class Menu
//----------------------------------------------------------------------

class Menu final : public finalcut::FDialog
{
  public:
    // Constructor
    explicit Menu (finalcut::FWidget* = nullptr);

  private:
    struct FileMenu
    {
      explicit FileMenu (finalcut::FMenuBar& menubar)
        : File{"&File", &menubar}
      { }

      finalcut::FMenu      File{};
      finalcut::FMenuItem  New{"&New", &File};
      finalcut::FMenuItem  Open{"&Open...", &File};
      finalcut::FMenuItem  Save{"&Save", &File};
      finalcut::FMenuItem  SaveAs{"&Save as...", &File};
      finalcut::FMenuItem  Close{"&Close", &File};
      finalcut::FMenuItem  Line1{&File};
      finalcut::FMenuItem  Print{"&Print", &File};
      finalcut::FMenuItem  Line2{&File};
      finalcut::FMenuItem  Quit{"&Quit", &File};
    };

    struct EditMenu
    {
      explicit EditMenu (finalcut::FMenuBar& menubar)
        : Edit{"&Edit", &menubar}
      { }

      finalcut::FMenu      Edit{};
      finalcut::FMenuItem  Undo{FKey::Ctrl_z, "&Undo", &Edit};
      finalcut::FMenuItem  Redo{FKey::Ctrl_y, "&Redo", &Edit};
      finalcut::FMenuItem  Line3{&Edit};
      finalcut::FMenuItem  Cut{FKey::Ctrl_x, "Cu&t", &Edit};
      finalcut::FMenuItem  Copy{FKey::Ctrl_c, "&Copy", &Edit};
      finalcut::FMenuItem  Paste{FKey::Ctrl_v, "&Paste", &Edit};
      finalcut::FMenuItem  Line4{&Edit};
      finalcut::FMenuItem  Search{FKey::Ctrl_f, "&Search", &Edit};
      finalcut::FMenuItem  Next{FKey::F3, "Search &next", &Edit};
      finalcut::FMenuItem  Line5{&Edit};
      finalcut::FMenuItem  SelectAll{FKey::Ctrl_a, "Select &all", &Edit};
    };

    struct ChoiceMenu
    {
      explicit ChoiceMenu (finalcut::FMenuBar& menubar)
        : Choice{"&Choice", &menubar}
      { }

      finalcut::FMenu  Choice{};
      finalcut::FMenu  Color{"&Color", &Choice};
      finalcut::FMenu  Style{"&Style", &Choice};
      finalcut::FMenu  Border{"&Border", &Choice};
    };

    struct ColorMenu
    {
      explicit ColorMenu (finalcut::FMenu& Color)
        : Color1{"Red", &Color}
        , Color2{"Green", &Color}
        , Color3{"Yellow", &Color}
        , Color4{"Brue", &Color}
        , Color5{"Black", &Color}
      { }

      finalcut::FRadioMenuItem Color1{};
      finalcut::FRadioMenuItem Color2{};
      finalcut::FRadioMenuItem Color3{};
      finalcut::FRadioMenuItem Color4{};
      finalcut::FRadioMenuItem Color5{};
    };

    struct StyleMenu
    {
      explicit StyleMenu (finalcut::FMenu& Style)
        : Bold{"Bold", &Style}
        , Italic{"Italic", &Style}
      { }

      finalcut::FCheckMenuItem Bold{};
      finalcut::FCheckMenuItem Italic{};
    };

    struct BorderMenu
    {
      explicit BorderMenu (finalcut::FMenu& Border)
        : BColor{"&Color", &Border}
        , BStyle{"&Style", &Border}
        , BColor1{"Red", &BColor}
        , BColor2{"Blue", &BColor}
        , BStyle1{std::move(line), &BStyle}
        , BStyle2{"-------------", &BStyle}
        , BStyle3{"- - - - - - -", &BStyle}
        , BStyle4{"-  -  -  -  -", &BStyle}
      { }

      finalcut::FString        line{13, finalcut::UniChar::BoxDrawingsHorizontal};
      finalcut::FMenu          BColor{};
      finalcut::FMenu          BStyle{};
      finalcut::FRadioMenuItem BColor1{};
      finalcut::FRadioMenuItem BColor2{};
      finalcut::FRadioMenuItem BStyle1{};
      finalcut::FRadioMenuItem BStyle2{};
      finalcut::FRadioMenuItem BStyle3{};
      finalcut::FRadioMenuItem BStyle4{};
    };

    // Methods
    void configureFileMenuItems();
    void configureEditMenuItems();
    void configureChoiceMenuItems();
    void configureColorMenuItems();
    void configureStyleMenuItems();
    void configureBorderMenuItems();
    void defaultCallback (const finalcut::FMenuList*);
    void initLayout() override;
    void adjustSize() override;

    // Event handler
    void onClose (finalcut::FCloseEvent*) override;

    // Callback method
    void cb_message (const finalcut::FMenuItem*);

    // Data members
    finalcut::FMenuBar    Menubar{this};
    FileMenu              file_menu{Menubar};
    EditMenu              edit_menu{Menubar};
    ChoiceMenu            choice_menu{Menubar};
    ColorMenu             color_menu{choice_menu.Color};
    StyleMenu             style_menu{choice_menu.Style};
    BorderMenu            border_menu{choice_menu.Border};
    finalcut::FMenuItem   Window{"&Window", &Menubar};
    finalcut::FMenuItem   Help{"&Help", &Menubar};
    finalcut::FStatusBar  Statusbar{this};
    finalcut::FLabel      Headline1{this};
    finalcut::FLabel      Headline2{this};
    finalcut::FLabel      Info{this};
};

//----------------------------------------------------------------------
Menu::Menu (finalcut::FWidget* parent)
  : finalcut::FDialog{parent}
{
  // Menu bar itms
  file_menu.File.setStatusbarMessage ("File management commands");
  edit_menu.Edit.setStatusbarMessage ("Cut-and-paste editing commands");
  choice_menu.Choice.setStatusbarMessage ("Choice menu");
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
  Headline1.setEmphasis();

  Headline2 << " Function ";
  Headline2.ignorePadding();
  Headline2.setEmphasis();

  // Info label
  Info << "<F10>            Activate menu bar\n"
       << "<Ctrl>+<Space>   Activate menu bar\n"
       << "<Menu>           Activate menu bar\n"
       << "<Shift>+<Menu>   Open dialog menu\n"
       << "<Meta>+<X>       Exit";
}

//----------------------------------------------------------------------
void Menu::configureFileMenuItems()
{
  // "File" menu items
  file_menu.New.addAccelerator (FKey::Ctrl_n);  // Ctrl + N
  file_menu.New.setStatusbarMessage ("Create a new file");
  file_menu.Open.addAccelerator (FKey::Ctrl_o);  // Ctrl + O
  file_menu.Open.setStatusbarMessage ("Locate and open a text file");
  file_menu.Save.addAccelerator (FKey::Ctrl_s);  // Ctrl + S
  file_menu.Save.setStatusbarMessage ("Save the file");
  file_menu.SaveAs.setStatusbarMessage ("Save the current file under a different name");
  file_menu.Close.addAccelerator (FKey::Ctrl_w);  // Ctrl + W
  file_menu.Close.setStatusbarMessage ("Close the current file");
  file_menu.Line1.setSeparator();
  file_menu.Print.addAccelerator (FKey::Ctrl_p);  // Ctrl + P
  file_menu.Print.setStatusbarMessage ("Print the current file");
  file_menu.Line2.setSeparator();
  file_menu.Quit.addAccelerator (FKey::Meta_x);  // Meta/Alt + X
  file_menu.Quit.setStatusbarMessage ("Exit the program");

  // Add quit menu item callback
  file_menu.Quit.addCallback
  (
    "clicked",
    finalcut::getFApplication(),
    &finalcut::FApplication::cb_exitApp,
    this
  );
}

//----------------------------------------------------------------------
void Menu::configureEditMenuItems()
{
  // "Edit" menu items
  edit_menu.Undo.setStatusbarMessage ("Undo the previous operation");
  edit_menu.Redo.setDisable();
  edit_menu.Line3.setSeparator();
  edit_menu.Cut.setStatusbarMessage ("Remove the input text "
                                     "and put it in the clipboard");
  edit_menu.Copy.setStatusbarMessage ("Copy the input text into the clipboad");
  edit_menu.Paste.setStatusbarMessage ("Insert text form clipboard");
  edit_menu.Line4.setSeparator();
  edit_menu.Search.setStatusbarMessage ("Search for text");
  edit_menu.Next.setStatusbarMessage ("Repeat the last search command");
  edit_menu.Line5.setSeparator();
  edit_menu.SelectAll.setStatusbarMessage ("Select the whole text");
}

//----------------------------------------------------------------------
void Menu::configureChoiceMenuItems()
{
  // "Choice" menu items
  choice_menu.Color.setStatusbarMessage ("Choose a color");
  choice_menu.Style.setStatusbarMessage ("Choose a Style");
  choice_menu.Border.setStatusbarMessage ("Choose Border");

  configureColorMenuItems();
  configureStyleMenuItems();
  configureBorderMenuItems();
}

//----------------------------------------------------------------------
void Menu::configureColorMenuItems()
{
  // "Color" menu items
  color_menu.Color1.setStatusbarMessage ("Set text red");
  color_menu.Color2.setStatusbarMessage ("Set text green");
  color_menu.Color3.setStatusbarMessage ("Set text yellow");
  color_menu.Color4.setStatusbarMessage ("Set text brue");
  color_menu.Color5.setStatusbarMessage ("Set text black");
  color_menu.Color5.setChecked();
}

//----------------------------------------------------------------------
void Menu::configureStyleMenuItems()
{
  // "Style" menu items
  style_menu.Bold.setStatusbarMessage ("Set text bold");
  style_menu.Italic.setStatusbarMessage ("Set text italic");
}

//----------------------------------------------------------------------
void Menu::configureBorderMenuItems()
{
  // "Border" menu items
  border_menu.BColor.setStatusbarMessage ("Choose the border color");
  border_menu.BStyle.setStatusbarMessage ("Choose the border Style");

  // "BColor" menu items
  border_menu.BColor1.setStatusbarMessage ("Set red border");
  border_menu.BColor2.setStatusbarMessage ("Set blue border");

  // "BStyle" menu items
  border_menu.BStyle1.setChecked();
  border_menu.BStyle1.setStatusbarMessage ("Set border 1");
  border_menu.BStyle2.setStatusbarMessage ("Set border 2");
  border_menu.BStyle3.setStatusbarMessage ("Set border 3");
  border_menu.BStyle4.setStatusbarMessage ("Set border 4");
}

//----------------------------------------------------------------------
void Menu::defaultCallback (const finalcut::FMenuList* mb)
{
  for (std::size_t i{1}; i <= mb->getCount(); i++)
  {
    auto item = mb->getItem(int(i));

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
        this, &Menu::cb_message,
        item
      );

      // Call sub-menu
      if ( item->hasMenu() )
        defaultCallback (item->getMenu());
    }
  }
}

//----------------------------------------------------------------------
void Menu::initLayout()
{
  Headline1.setGeometry (FPoint{3, 2}, FSize{5, 1});
  Headline2.setGeometry (FPoint{19, 2}, FSize{10, 1});
  Info.setGeometry(FPoint{2, 1}, FSize{36, 5});
  FDialog::initLayout();
}

//----------------------------------------------------------------------
void Menu::adjustSize()
{
  const auto pw = int(getDesktopWidth());
  const auto ph = int(getDesktopHeight());
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
void Menu::cb_message (const finalcut::FMenuItem* menuitem)
{
  auto text = menuitem->getText();
  text = text.replace('&', "");
  finalcut::FMessageBox::info ( this
                              , "Info"
                              , "You have chosen \"" + text + "\"" );
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

auto main (int argc, char* argv[]) -> int
{
  // Create the application object
  finalcut::FApplication app {argc, argv};

  // Create main dialog object
  Menu main_dlg {&app};
  main_dlg.setText ("Menu example");
  main_dlg.setSize ({40, 8});
  main_dlg.setShadow();

  // Set dialog main_dlg as main widget
  finalcut::FWidget::setMainWidget (&main_dlg);

  // Show and start the application
  main_dlg.show();
  return app.exec();
}
