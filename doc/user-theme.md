User Themes
===========

FINAL CUT supports color themes. That makes it possible to change the color 
of all elements of a widget in the program. Also, you can adjust the color 
palette to your preferences. If you want to switch back to the default 
themes, you can always call the method `FApplication::setDefaultTheme()` 
or `FApplication::setDarkTheme()` for the dark theme.


Widget Color Theme
------------------

FINAL CUT uses a default color scheme that the user can override in a 
derived class of `FWidgetColors`. All widget colors are redefined in the 
constructor by the method `setColorTheme()`.

```cpp
class myWidgetColors final : public finalcut::FWidgetColors
{
  public:
    myWidgetColors()
    {
      myWidgetColors::setColorTheme();
    }

    ~myWidgetColors() override
    { }

    const finalcut::FString getClassName() const override
    {
      return "myWidgetColors";
    }

    void myWidgetColors() override
    {
      ...  // Color definitions
    }
};
```

For setting the widget colors, it is recommended to call the method 
`FWidget::setColorTheme()` via the `FApplication` object to create a 
new instance of the theme and assign it to the application.

```cpp
finalcut::FApplication app(argc, argv);
app.setColorTheme<myWidgetColors>();
```

In the following example, we will create the `BeeColorTheme`. For this 
purpose, we will first create an include file that can be easily included 
later in your application.

**File:** *widget-color-theme.h*
```cpp
#ifndef WIDGETCOLORTHEME_H
#define WIDGETCOLORTHEME_H

class BeeColorTheme final : public finalcut::FWidgetColors
{
  public:
    BeeColorTheme()
    {
      BeeColorTheme::setColorTheme();
    }

    ~BeeColorTheme() override
    { }

    const finalcut::FString getClassName() const override
    {
      return "BeeColorTheme";
    }

    void setColorTheme() override
    {
      term_fg                           = finalcut::fc::Black;
      term_bg                           = finalcut::fc::LightBlue;
      list_fg                           = finalcut::fc::Black;
      list_bg                           = finalcut::fc::LightGray;
      selected_list_fg                  = finalcut::fc::LightRed;
      selected_list_bg                  = finalcut::fc::LightGray;
      dialog_fg                         = finalcut::fc::Black;
      dialog_resize_fg                  = finalcut::fc::Red;
      dialog_emphasis_fg                = finalcut::fc::Blue;
      dialog_bg                         = finalcut::fc::LightGray;
      error_box_fg                      = finalcut::fc::Black;
      error_box_emphasis_fg             = finalcut::fc::Red;
      error_box_bg                      = finalcut::fc::Yellow;
      tooltip_fg                        = finalcut::fc::Black;
      tooltip_bg                        = finalcut::fc::Yellow;
      shadow_fg                         = finalcut::fc::Black;
      shadow_bg                         = finalcut::fc::LightGray;
      current_element_focus_fg          = finalcut::fc::White;
      current_element_focus_bg          = finalcut::fc::Green;
      current_element_fg                = finalcut::fc::LightGray;
      current_element_bg                = finalcut::fc::DarkGray;
      current_inc_search_element_fg     = finalcut::fc::Brown;
      selected_current_element_focus_fg = finalcut::fc::LightRed;
      selected_current_element_focus_bg = finalcut::fc::Green;
      selected_current_element_fg       = finalcut::fc::LightRed;
      selected_current_element_bg       = finalcut::fc::DarkGray;
      label_fg                          = finalcut::fc::Black;
      label_bg                          = finalcut::fc::LightGray;
      label_inactive_fg                 = finalcut::fc::LightGray;
      label_inactive_bg                 = finalcut::fc::DarkGray;
      label_hotkey_fg                   = finalcut::fc::Red;
      label_hotkey_bg                   = finalcut::fc::LightGray;
      label_emphasis_fg                 = finalcut::fc::Blue;
      label_ellipsis_fg                 = finalcut::fc::DarkGray;
      inputfield_active_focus_fg        = finalcut::fc::LightGray;
      inputfield_active_focus_bg        = finalcut::fc::Green;
      inputfield_active_fg              = finalcut::fc::Black;
      inputfield_active_bg              = finalcut::fc::Cyan ;
      inputfield_inactive_fg            = finalcut::fc::Black;
      inputfield_inactive_bg            = finalcut::fc::LightGray;
      toggle_button_active_focus_fg     = finalcut::fc::White;
      toggle_button_active_focus_bg     = finalcut::fc::Green;
      toggle_button_active_fg           = finalcut::fc::Black;
      toggle_button_active_bg           = finalcut::fc::LightGray;
      toggle_button_inactive_fg         = finalcut::fc::DarkGray;
      toggle_button_inactive_bg         = finalcut::fc::LightGray;
      button_active_focus_fg            = finalcut::fc::White;
      button_active_focus_bg            = finalcut::fc::Green;
      button_active_fg                  = finalcut::fc::Black;
      button_active_bg                  = finalcut::fc::Cyan;
      button_inactive_fg                = finalcut::fc::Cyan;
      button_inactive_bg                = finalcut::fc::LightGray;
      button_hotkey_fg                  = finalcut::fc::Red;
      titlebar_active_fg                = finalcut::fc::White;
      titlebar_active_bg                = finalcut::fc::Blue;
      titlebar_inactive_fg              = finalcut::fc::LightGray;
      titlebar_inactive_bg              = finalcut::fc::DarkGray;
      titlebar_button_fg                = finalcut::fc::Black;
      titlebar_button_bg                = finalcut::fc::LightGray;
      titlebar_button_focus_fg          = finalcut::fc::LightGray;
      titlebar_button_focus_bg          = finalcut::fc::Black;
      menu_active_focus_fg              = finalcut::fc::White;
      menu_active_focus_bg              = finalcut::fc::Blue;
      menu_active_fg                    = finalcut::fc::Black;
      menu_active_bg                    = finalcut::fc::Yellow;
      menu_inactive_fg                  = finalcut::fc::Cyan;
      menu_inactive_bg                  = finalcut::fc::Yellow;
      menu_hotkey_fg                    = finalcut::fc::Red;
      menu_hotkey_bg                    = finalcut::fc::Yellow;
      statusbar_fg                      = finalcut::fc::White;
      statusbar_bg                      = finalcut::fc::DarkGray;
      statusbar_hotkey_fg               = finalcut::fc::LightRed;
      statusbar_hotkey_bg               = finalcut::fc::DarkGray;
      statusbar_separator_fg            = finalcut::fc::Black;
      statusbar_active_fg               = finalcut::fc::White;
      statusbar_active_bg               = finalcut::fc::Green;
      statusbar_active_hotkey_fg        = finalcut::fc::LightRed;
      statusbar_active_hotkey_bg        = finalcut::fc::Green;
      scrollbar_fg                      = finalcut::fc::Black;
      scrollbar_bg                      = finalcut::fc::Green;
      scrollbar_button_fg               = finalcut::fc::Black;
      scrollbar_button_bg               = finalcut::fc::Green;
      scrollbar_button_inactive_fg      = finalcut::fc::Cyan;
      scrollbar_button_inactive_bg      = finalcut::fc::LightGray;
      progressbar_fg                    = finalcut::fc::Green;
      progressbar_bg                    = finalcut::fc::DarkGray;
    }
};

#endif  // WIDGETCOLORTHEME_H
```


Color Palette Theme
-------------------

FINAL CUT has four color tables for the 16 standard colors in the terminal. 
These are a redefinition of the 16 ANSI colors. You can address the colors 
via indexes values from 0 to 15. They correspond to the following colors:

| Index  | Color name                 |
|:------:|:---------------------------|
|    0   | finalcut::fc::Black        |
|    1   | finalcut::fc::Blue         |
|    2   | finalcut::fc::Green        |
|    3   | finalcut::fc::Cyan         |
|    4   | finalcut::fc::Red          |
|    5   | finalcut::fc::Magenta      |
|    6   | finalcut::fc::Brown        |
|    7   | finalcut::fc::LightGray    |
|    8   | finalcut::fc::DarkGray     |
|    9   | finalcut::fc::LightBlue    |
|   10   | finalcut::fc::LightGreen   |
|   11   | finalcut::fc::LightCyan    |
|   12   | finalcut::fc::LightRed     |
|   13   | finalcut::fc::LightMagenta |
|   14   | finalcut::fc::Yellow       |
|   15   | finalcut::fc::White        |

You can define your color as an 8-bit value based on its red, green, and 
blue components. To create a color palette, create a derived class of 
`FColorPalette`. The constructor gets as argument the function to set 
a palette color. This function must have the following structure:

```cpp
setPalette(finalcut::FColor index, int red, int green, int blue);
```

A possible implementation could look as follows:

```cpp
class myColorPalette final : public finalcut::FColorPalette
{
  public:
    explicit myColorPalette (const FSetPalette& f)
      : FColorPalette(f)
    { }

    ~myColorPalette()
    { }

    const finalcut::FString getClassName() const override
    {
      return "myColorPalette";
    }

    void setColorPalette() override
    {
      ...  // Palette definitions
    }

    void resetColorPalette() override
    {
      setVGAdefaultPalette();
    }
};
```
To set the colors of a palette theme, you should use the method 
`FTerm::setColorPaletteTheme()`. This method creates a new instance and 
saves it in the `FTerm` object.

```cpp
finalcut::FTerm::setColorPaletteTheme<myColorPalette>();
```
The standard VGA palette is part of the `FColorPalette` class. To set it, 
use the method `setVGAdefaultPalette()`. You can use it to reset the color 
palette of terminals that cannot reset to default values with escape 
sequences.
<figure class="image">
  <img src="user-theme-vga-palette.svg" alt="VGA palette">
  <figcaption>Figure 1.  VGA palette</figcaption>
</figure>

The FINAL CUT eight-color palette `default8ColorPalette` is optimized for 
the eight-color widget theme `default8ColorTheme`. It is for terminals 
that cannot display more than eight colors.
<figure class="image">
  <img src="user-theme-fc8-palette.svg" alt="FINAL CUT 8-color palette">
  <figcaption>Figure 2.  FINAL CUT 8-color palette</figcaption>
</figure>

The FINAL CUT palette `default16ColorPalette` is the default 16-color 
palette. It is optimized for the widget color theme `default16ColorTheme`.
<figure class="image">
  <img src="user-theme-fc16-palette.svg" alt="FINAL CUT 16-color palette">
  <figcaption>Figure 3.  FINAL CUT 16-color palette</figcaption>
</figure>

The second 16-color palette in FINAL CUT is for the dark theme. It was 
adjusted for the widget color themes `default8ColorDarkTheme` and 
`default16ColorDarkTheme`.
<figure class="image">
  <img src="user-theme-fc16-dark-palette.svg" alt="FINAL CUT 16-color dark palette">
  <figcaption>Figure 4.  FINAL CUT 16-color dark palette</figcaption>
</figure>

In the following example, we want to create the palette them 
`BeeColorPalette`. For this purpose, we generate an include file again, 
in which we implement the new palette class.
<figure class="image">
  <img src="user-theme-bee-palette.svg" alt="Bee palette">
  <figcaption>Figure 6.  Bee palette</figcaption>
</figure>

**File:** *color-palette-theme.h*
```cpp
#ifndef BEECOLORPALETTE_H
#define BEECOLORPALETTE_H

class BeeColorPalette final : public finalcut::FColorPalette
{
  public:
    explicit BeeColorPalette (const FSetPalette& f)
      : FColorPalette(f)
    { }

    ~BeeColorPalette()
    { }

    const finalcut::FString getClassName() const override
    {
      return "BeeColorPalette";
    }

    void setColorPalette() override
    {
      setPalette (finalcut::fc::Black, 0x00, 0x00, 0x00);
      setPalette (finalcut::fc::Blue, 0x23, 0x21, 0x2c);
      setPalette (finalcut::fc::Green, 0x26, 0x93, 0x7c);
      setPalette (finalcut::fc::Cyan, 0xcf, 0xb3, 0xa8);
      setPalette (finalcut::fc::Red, 0xba, 0x1a, 0x1a);
      setPalette (finalcut::fc::Magenta, 0xb2, 0x18, 0xb2);
      setPalette (finalcut::fc::Brown, 0xe8, 0x87, 0x1f);
      setPalette (finalcut::fc::LightGray, 0xff, 0xfb, 0xe4);
      setPalette (finalcut::fc::DarkGray, 0x3a, 0x36, 0x37);
      setPalette (finalcut::fc::LightBlue, 0xa5, 0xa5, 0xb1);
      setPalette (finalcut::fc::LightGreen, 0x5e, 0xeb, 0x5c);
      setPalette (finalcut::fc::LightCyan, 0x62, 0xbf, 0xf8);
      setPalette (finalcut::fc::LightRed, 0xee, 0x44, 0x44);
      setPalette (finalcut::fc::LightMagenta, 0xe9, 0xad, 0xff);
      setPalette (finalcut::fc::Yellow, 0xf8, 0xef, 0xa6);
      setPalette (finalcut::fc::White, 0xff, 0xff, 0xff);
    }

    void resetColorPalette() override
    {
      setVGAdefaultPalette();
    }
};

#endif  // BEECOLORPALETTE_H
```


Use of Themes
-------------

If you include the two include files above in your application, you can use 
the widget color theme and the color palette theme. In the main function of 
your application, the object instances of both classes are created and set.
<figure class="image">
  <img src="user-theme.png" alt="User theme example">
  <figcaption>Figure 7.  User theme example</figcaption>
</figure>


**File:** *theme.cpp*
```cpp
#include <final/final.h>

#include "widget-color-theme.h"
#include "color-palette-theme.h"

using namespace finalcut;

class dialogWidget final : public FDialog
{
  public:
    explicit dialogWidget (FWidget* parent = nullptr)
      : FDialog{"Theming test application", parent}
    {
      FDialog::setGeometry (FPoint{15, 5}, FSize{50, 9});
      Input.setGeometry (FPoint{2, 2}, FSize{39, 1});
      Input.setLabelText("File name:");
      Input.setLabelOrientation(FLineEdit::label_above);
      Input.setStatusbarMessage("Enter a file name");
      Browse.setGeometry (FPoint{43, 2}, FSize{4, 1});
      Browse.addCallback
      (
        "clicked",
        F_METHOD_CALLBACK (this, &dialogWidget::cb_FileBrowse)
      );
      Apply.setGeometry (FPoint{24, 5}, FSize{10, 1});
      Apply.setStatusbarMessage("Apply settings");
      Quit.setGeometry (FPoint{37, 5}, FSize{10, 1});
      Quit.setStatusbarMessage("Exit the program");
      Quit.addCallback
      (
        "clicked",
        F_METHOD_CALLBACK (this, &finalcut::FApplication::cb_exitApp)
      );
      Open.addCallback
      (
        "clicked",
        F_METHOD_CALLBACK (this, &dialogWidget::cb_FileBrowse)
      );
    }

  private:
    void cb_FileBrowse (finalcut::FWidget*, FDataPtr)
    {
      auto filename = FFileDialog::fileOpenChooser(this);

      if ( ! filename.isEmpty() )
      {
        Input.setText(filename);
        Input.redraw();
      }
    }

    FMenuBar        Menubar{this};
    FMenu           File{"&File", &Menubar};
    FMenuItem       New{"&New", &File};
    FMenuItem       Open{"&Open...", &File};
    FMenu           Edit{"&Edit", &Menubar};
    FMenuItem       Undo{"&Undo", &Edit};
    FMenu           Help{"&Help", &Menubar};
    FMenuItem       About{"&About", &Help};
    FStatusBar      Statusbar{this};
    FLineEdit       Input{"input...", this};
    FButton         Browse{"..", this};
    FButton         Apply{"&Apply", this};
    FButton         Quit{"&Quit", this};
};

int main (int argc, char* argv[])
{
  FApplication app(argc, argv);

  // Set the widget color theme
  app.setColorTheme<BeeColorTheme>();

  // Set the color palette theme
  FTerm::setColorPaletteTheme<BeeColorPalette>();

  dialogWidget dialog(&app);
  FWidget::setMainWidget(&dialog);
  dialog.show();
  return app.exec();
}
```


After entering the source code in *theme.cpp* you can compile
the above program with gcc:
```cpp
g++ -O2 -lfinal -std=c++11 theme.cpp -o theme

```
