/***********************************************************************
* highlight-text.cpp - Example of FTextView with highlighted text      *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2022 Markus Gans                                           *
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

#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include <final/final.h>

using finalcut::FColor;
using finalcut::FColorPair;
using finalcut::FPoint;
using finalcut::FRect;
using finalcut::FSize;
using finalcut::FStyle;
using FTextHighlight = finalcut::FTextView::FTextHighlight;

constexpr char lgpl3[] = \
R"(                   GNU LESSER GENERAL PUBLIC LICENSE
                       Version 3, 29 June 2007

 Copyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.


  This version of the GNU Lesser General Public License incorporates
the terms and conditions of version 3 of the GNU General Public
License, supplemented by the additional permissions listed below.

  0. Additional Definitions.

  As used herein, "this License" refers to version 3 of the GNU Lesser
General Public License, and the "GNU GPL" refers to version 3 of the GNU
General Public License.

  "The Library" refers to a covered work governed by this License,
other than an Application or a Combined Work as defined below.

  An "Application" is any work that makes use of an interface provided
by the Library, but which is not otherwise based on the Library.
Defining a subclass of a class defined by the Library is deemed a mode
of using an interface provided by the Library.

  A "Combined Work" is a work produced by combining or linking an
Application with the Library.  The particular version of the Library
with which the Combined Work was made is also called the "Linked
Version".

  The "Minimal Corresponding Source" for a Combined Work means the
Corresponding Source for the Combined Work, excluding any source code
for portions of the Combined Work that, considered in isolation, are
based on the Application, and not on the Linked Version.

  The "Corresponding Application Code" for a Combined Work means the
object code and/or source code for the Application, including any data
and utility programs needed for reproducing the Combined Work from the
Application, but excluding the System Libraries of the Combined Work.

  1. Exception to Section 3 of the GNU GPL.

  You may convey a covered work under sections 3 and 4 of this License
without being bound by section 3 of the GNU GPL.

  2. Conveying Modified Versions.

  If you modify a copy of the Library, and, in your modifications, a
facility refers to a function or data to be supplied by an Application
that uses the facility (other than as an argument passed when the
facility is invoked), then you may convey a copy of the modified
version:

   a) under this License, provided that you make a good faith effort to
   ensure that, in the event an Application does not supply the
   function or data, the facility still operates, and performs
   whatever part of its purpose remains meaningful, or

   b) under the GNU GPL, with none of the additional permissions of
   this License applicable to that copy.

  3. Object Code Incorporating Material from Library Header Files.

  The object code form of an Application may incorporate material from
a header file that is part of the Library.  You may convey such object
code under terms of your choice, provided that, if the incorporated
material is not limited to numerical parameters, data structure
layouts and accessors, or small macros, inline functions and templates
(ten or fewer lines in length), you do both of the following:

   a) Give prominent notice with each copy of the object code that the
   Library is used in it and that the Library and its use are
   covered by this License.

   b) Accompany the object code with a copy of the GNU GPL and this license
   document.

  4. Combined Works.

  You may convey a Combined Work under terms of your choice that,
taken together, effectively do not restrict modification of the
portions of the Library contained in the Combined Work and reverse
engineering for debugging such modifications, if you also do each of
the following:

   a) Give prominent notice with each copy of the Combined Work that
   the Library is used in it and that the Library and its use are
   covered by this License.

   b) Accompany the Combined Work with a copy of the GNU GPL and this license
   document.

   c) For a Combined Work that displays copyright notices during
   execution, include the copyright notice for the Library among
   these notices, as well as a reference directing the user to the
   copies of the GNU GPL and this license document.

   d) Do one of the following:

       0) Convey the Minimal Corresponding Source under the terms of this
       License, and the Corresponding Application Code in a form
       suitable for, and under terms that permit, the user to
       recombine or relink the Application with a modified version of
       the Linked Version to produce a modified Combined Work, in the
       manner specified by section 6 of the GNU GPL for conveying
       Corresponding Source.

       1) Use a suitable shared library mechanism for linking with the
       Library.  A suitable mechanism is one that (a) uses at run time
       a copy of the Library already present on the user's computer
       system, and (b) will operate properly with a modified version
       of the Library that is interface-compatible with the Linked
       Version.

   e) Provide Installation Information, but only if you would otherwise
   be required to provide such information under section 6 of the
   GNU GPL, and only to the extent that such information is
   necessary to install and execute a modified version of the
   Combined Work produced by recombining or relinking the
   Application with a modified version of the Linked Version. (If
   you use option 4d0, the Installation Information must accompany
   the Minimal Corresponding Source and Corresponding Application
   Code. If you use option 4d1, you must provide the Installation
   Information in the manner specified by section 6 of the GNU GPL
   for conveying Corresponding Source.)

  5. Combined Libraries.

  You may place library facilities that are a work based on the
Library side by side in a single library together with other library
facilities that are not Applications and are not covered by this
License, and convey such a combined library under terms of your
choice, if you do both of the following:

   a) Accompany the combined library with a copy of the same work based
   on the Library, uncombined with any other library facilities,
   conveyed under the terms of this License.

   b) Give prominent notice with the combined library that part of it
   is a work based on the Library, and explaining where to find the
   accompanying uncombined form of the same work.

  6. Revised Versions of the GNU Lesser General Public License.

  The Free Software Foundation may publish revised and/or new versions
of the GNU Lesser General Public License from time to time. Such new
versions will be similar in spirit to the present version, but may
differ in detail to address new problems or concerns.

  Each version is given a distinguishing version number. If the
Library as you received it specifies that a certain numbered version
of the GNU Lesser General Public License "or any later version"
applies to it, you have the option of following the terms and
conditions either of that published version or of any later version
published by the Free Software Foundation. If the Library as you
received it does not specify a version number of the GNU Lesser
General Public License, you may choose any version of the GNU Lesser
General Public License ever published by the Free Software Foundation.

  If the Library as you received it specifies that a proxy can decide
whether future versions of the GNU Lesser General Public License shall
apply, that proxy's public statement of acceptance of any version is
permanent authorization for you to choose that version for the
Library.)";


//----------------------------------------------------------------------
// class TextWindow
//----------------------------------------------------------------------

class TextWindow final : public finalcut::FDialog
{
  public:
    // Constructor
    explicit TextWindow (finalcut::FWidget* = nullptr);

  private:
    // Using-declarations
    using MatchList = std::vector<std::size_t>;
    using StringPos = std::wstring::size_type;

    // Method
    void initLayout() override;
    void adjustSize() override;
    MatchList find_matches ( const finalcut::FString&
                           , const finalcut::FString& ) const;
    template <typename... Args>
    void highlight ( std::size_t
                   , const finalcut::FString&
                   , const finalcut::FString&
                   , Args&&... );
    void highlightText();

    // Event handlers
    void onAccel (finalcut::FAccelEvent*) override;
    void onClose (finalcut::FCloseEvent*) override;

    // Data members
    finalcut::FTextView scrolltext{this};
};

//----------------------------------------------------------------------
TextWindow::TextWindow (finalcut::FWidget* parent)
  : finalcut::FDialog{parent}
{
  setResizeable();
  scrolltext.ignorePadding();
  scrolltext.setFocus();
  scrolltext.append(lgpl3);
  highlightText();
}

//----------------------------------------------------------------------
void TextWindow::initLayout()
{
  FDialog::setText("LGPLv3");
  setMinimumSize (FSize{51, 6});
  auto x = 1 + int((getDesktopWidth() - 72) / 2);
  auto y = int(getDesktopHeight() / 11);
  auto window_size = FSize{72, getDesktopHeight() * 7 / 8};
  FDialog::setGeometry(FPoint{x, y}, window_size);
  scrolltext.setGeometry (FPoint{1, 2}, FSize{getWidth(), getHeight() - 1});
  FDialog::initLayout();
}

//----------------------------------------------------------------------
void TextWindow::adjustSize()
{
  finalcut::FDialog::adjustSize();

  if ( ! isZoomed() )
  {
    auto desktop_size = FSize{getDesktopWidth(), getDesktopHeight()};
    FRect screen(FPoint{1, 1}, desktop_size);
    bool center = false;

    if ( getWidth() > getDesktopWidth() )
    {
      setWidth(getDesktopWidth() * 9 / 10);
      center = true;
    }

    if ( getHeight() > getDesktopHeight() )
    {
      setHeight(getDesktopHeight() * 7 / 8);
      center = true;
    }

    // Centering the window when it is no longer
    // in the visible area of the terminal
    if ( ! screen.contains(getPos()) || center )
    {
      int x = 1 + int((getDesktopWidth() - getWidth()) / 2);
      int y = 1 + int((getDesktopHeight() - getHeight()) / 2);
      setPos(FPoint{x, y});
    }
  }

  scrolltext.setGeometry (FPoint{1, 2}, FSize(getWidth(), getHeight() - 1));
}

//----------------------------------------------------------------------
TextWindow::MatchList TextWindow::find_matches ( const finalcut::FString& string
                                               , const finalcut::FString& search ) const
{
  MatchList matches{};
  const auto search_length = search.getLength();
  StringPos end = std::wstring::npos;
  StringPos first = 0;
  StringPos last;

  while ( (last = string.toWString().find(search.toWString(), first)) != end )
  {
    matches.emplace_back(last);
    first = last + search_length;
  }

  return matches;
}

//----------------------------------------------------------------------
template <typename... Args>
void TextWindow::highlight ( std::size_t line_number
                           , const finalcut::FString& string
                           , const finalcut::FString& search
                           , Args&&... args )
{
  const auto len = search.getLength();

  for (const auto& found_pos : find_matches(string, search))
  {
    FTextHighlight hgl{found_pos, len, std::forward<Args>(args)...};
    scrolltext.addHighlight (line_number, hgl);
  }
}

//----------------------------------------------------------------------
void TextWindow::highlightText()
{
  std::size_t num{0};  // Line number

  for (const auto& line : scrolltext.getLines())
  {
    highlight (num, line.text, L"GNU", FColorPair{FColor::White, FColor::Red});
    highlight (num, line.text, L"Free Software Foundation", FColor::Green);
    FStyle style;
    style.setStyle (finalcut::Style::Underline + finalcut::Style::Italic);
    highlight (num, line.text, L"GPL", style);
    num++;
  }

  scrolltext.addHighlight
  (
    0,
    FTextHighlight { 19, 33, FColorPair{FColor::Blue, FColor::LightGray}
                           , FStyle(finalcut::Style::Underline) }
  );
}

//----------------------------------------------------------------------
void TextWindow::onAccel (finalcut::FAccelEvent* ev)
{
  close();
  ev->accept();
}

//----------------------------------------------------------------------
void TextWindow::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

int main (int argc, char* argv[])
{
  // Create the application object app
  finalcut::FApplication app{argc, argv};

  // Force initialization of the terminal without calling show() so that
  // the color theme is already available at the initialization time
  app.initTerminal();

  // Create main dialog object text_window
  TextWindow text_window{&app};
  text_window.addAccelerator(finalcut::FKey('q'));

  // Show the dialog text_window
  text_window.show();

  // Set dialog text_window as main widget
  finalcut::FWidget::setMainWidget(&text_window);

  // Show and start the application
  text_window.show();
  return app.exec();
}
