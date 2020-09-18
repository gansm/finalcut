/***********************************************************************
* ftextview.h - Widget FTextView (a multiline text viewer)             *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2020 Markus Gans                                      *
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

/*  Inheritance diagram
 *  ═══════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▏
 * ▕ FVTerm  ▏ ▕ FObject ▏
 * ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▏
 *      ▲           ▲
 *      │           │
 *      └─────┬─────┘
 *            │
 *       ▕▔▔▔▔▔▔▔▔▔▏
 *       ▕ FWidget ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏
 *            ▲
 *            │
 *      ▕▔▔▔▔▔▔▔▔▔▔▔▏
 *      ▕ FTextView ▏
 *      ▕▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FTEXTVIEW_H
#define FTEXTVIEW_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <string>
#include <unordered_map>
#include <vector>

#include "final/fstringstream.h"
#include "final/fwidget.h"

namespace finalcut
{

// class forward declaration
class FString;

//----------------------------------------------------------------------
// class FTextView
//----------------------------------------------------------------------

class FTextView : public FWidget
{
  public:
    // Using-declarations
    using FWidget::setGeometry;

    // Constructor
    explicit FTextView (FWidget* = nullptr);

    // Disable copy constructor
    FTextView (const FTextView&) = delete;

    // Destructor
    ~FTextView() override;

    // Disable copy assignment operator (=)
    FTextView& operator = (const FTextView&) = delete;

    // Overloaded operators
    FTextView& operator = (const FString&);
    template <typename typeT>
    FTextView& operator << (const typeT&);
    FTextView& operator << (fc::SpecialCharacter);
    FTextView& operator << (const std::string&);

    // Accessors
    const FString       getClassName() const override;
    std::size_t         getColumns() const;
    std::size_t         getRows() const;
    const FString       getText() const;
    const FStringList&  getLines() const;

    // Mutators
    void                setSize (const FSize&, bool = true) override;
    void                setGeometry ( const FPoint&, const FSize&
                                    , bool = true ) override;
    void                resetColors() override;
    void                setText (const FString&);
    void                scrollToX (int);
    void                scrollToY (int);
    void                scrollTo (const FPoint&);
    void                scrollTo (int, int);
    void                scrollToBegin();
    void                scrollToEnd();
    void                scrollBy (int, int);

    // Methods
    void                hide() override;
    template <typename T>
    void                append (const std::initializer_list<T>&);
    void                append (const FString&);
    template <typename T>
    void                insert (const std::initializer_list<T>&, int);
    void                insert (const FString&, int);
    void                replaceRange (const FString&, int, int);
    void                deleteRange (int, int);
    void                deleteLine (int);
    void                clear();

    // Event handlers
    void                onKeyPress (FKeyEvent*) override;
    void                onMouseDown (FMouseEvent*) override;
    void                onMouseUp (FMouseEvent*) override;
    void                onMouseMove (FMouseEvent*) override;
    void                onWheel (FWheelEvent*) override;
    void                onFocusIn (FFocusEvent*) override;
    void                onFocusOut (FFocusEvent*) override;

  protected:
    // Method
    void                adjustSize() override;

  private:
    // Typedefs
    typedef std::unordered_map<int, std::function<void()>> keyMap;

    // Accessors
    std::size_t         getTextHeight() const;
    std::size_t         getTextWidth() const;

    // Inquiry
    bool                isHorizontallyScrollable() const;
    bool                isVerticallyScrollable() const;

    // Methods
    void                init();
    void                mapKeyFunctions();
    void                draw() override;
    void                drawBorder() override;
    void                drawScrollbars() const;
    void                drawText();
    bool                useFDialogBorder() const;
    bool                isPrintable (wchar_t) const;
    void                processChanged() const;
    void                changeOnResize() const;

    // Callback methods
    void                cb_vbarChange (const FWidget*);
    void                cb_hbarChange (const FWidget*);

    // Data members
    FStringList        data{};
    FScrollbarPtr      vbar{nullptr};
    FScrollbarPtr      hbar{nullptr};
    keyMap             key_map{};
    bool               update_scrollbar{true};
    int                xoffset{0};
    int                yoffset{0};
    int                nf_offset{0};
    std::size_t        max_line_width{0};
};

// FListBox inline functions
//----------------------------------------------------------------------
inline FTextView& FTextView::operator = (const FString& s)
{
  setText(s);
  return *this;
}

//----------------------------------------------------------------------
template <typename typeT>
inline FTextView& FTextView::operator << (const typeT& s)
{
  FStringStream outstream{std::ios_base::out};
  outstream << s;

  if ( ! outstream.str().isEmpty() )
    append (outstream.str());

  return *this;
}

//----------------------------------------------------------------------
inline FTextView& FTextView::operator << (fc::SpecialCharacter c)
{
  append (static_cast<wchar_t>(c));  // Required under Solaris
  return *this;
}

//----------------------------------------------------------------------
inline FTextView& FTextView::operator << (const std::string& string)
{
  append (string);
  return *this;
}

//----------------------------------------------------------------------
inline const FString FTextView::getClassName() const
{ return "FTextView"; }

//----------------------------------------------------------------------
inline std::size_t FTextView::getColumns() const
{ return max_line_width; }

//----------------------------------------------------------------------
inline std::size_t FTextView::getRows() const
{ return std::size_t(data.size()); }

//----------------------------------------------------------------------
inline const FStringList& FTextView::getLines() const
{ return data; }

//----------------------------------------------------------------------
inline void FTextView::scrollTo (const FPoint& pos)
{ scrollTo(pos.getX(), pos.getY()); }

//----------------------------------------------------------------------
template <typename T>
void FTextView::append (const std::initializer_list<T>& list)
{
  for (auto& str : list)
    insert(str, -1);
}

//----------------------------------------------------------------------
template <typename T>
void FTextView::insert (const std::initializer_list<T>& list, int pos)
{
  for (auto& str : list)
  {
    insert(str, pos);
    pos++;
  }
}

//----------------------------------------------------------------------
inline void FTextView::deleteRange (int from, int to)
{ replaceRange (FString(), from, to); }

//----------------------------------------------------------------------
inline void FTextView::deleteLine (int pos)
{ deleteRange (pos, pos); }

//----------------------------------------------------------------------
inline bool FTextView::isHorizontallyScrollable() const
{ return bool( max_line_width > getTextWidth() ); }

//----------------------------------------------------------------------
inline bool FTextView::isVerticallyScrollable() const
{ return bool( getRows() > getTextHeight() ); }

}  // namespace finalcut

#endif  // FTEXTVIEW_H
