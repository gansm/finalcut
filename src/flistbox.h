// File: flistbox.h
// Provides: class FListBoxItem
//           class FListBox
//
//  Inheritance diagram
//  ═══════════════════
//
// ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▏
// ▕ FObject ▏ ▕  FTerm  ▏
// ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▏
//      ▲           ▲
//      │           │
//      └─────┬─────┘
//            │
//       ▕▔▔▔▔▔▔▔▔▏
//       ▕ FVTerm ▏
//       ▕▁▁▁▁▁▁▁▁▏
//            ▲
//            │
//       ▕▔▔▔▔▔▔▔▔▔▏
//       ▕ FWidget ▏
//       ▕▁▁▁▁▁▁▁▁▁▏
//            ▲
//            │
//       ▕▔▔▔▔▔▔▔▔▔▔▏1     *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//       ▕ FListBox ▏- - - -▕ FListBoxItem ▏
//       ▕▁▁▁▁▁▁▁▁▁▁▏       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
//

#ifndef _FLISTBOX_H
#define _FLISTBOX_H

#include "fscrollbar.h"
#include "fstring.h"
#include "fwidget.h"


//----------------------------------------------------------------------
// class FListBoxItem
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FListBoxItem
{
 public:
   // Constructors
   FListBoxItem ();
   FListBoxItem (const FListBoxItem&);  // copy constructor
   explicit FListBoxItem (FString&, FWidget::data_ptr = 0);
   explicit FListBoxItem (const std::string&, FWidget::data_ptr = 0);
   explicit FListBoxItem (const char*, FWidget::data_ptr = 0);

   // Destructor
   virtual ~FListBoxItem();

   // Assignment operator (=)
   FListBoxItem& operator = (const FListBoxItem&);

   // Accessors
   virtual FString getText() const;
   virtual FWidget::data_ptr getData() const;

 protected:
   // Mutators
   void setText (FString&);
   void setText (const std::string&);
   void setText (const char*);
   void setData (FWidget::data_ptr);

 private:
   // Friend classes
   friend class FListBox;

   // Data Members
   FString           text;
   FWidget::data_ptr data_pointer;
   fc::brackets_type brackets;
   bool              selected;
};
#pragma pack(pop)


// FListBoxItem inline functions
//----------------------------------------------------------------------
inline FString FListBoxItem::getText() const
{ return text; }

//----------------------------------------------------------------------
inline FWidget::data_ptr FListBoxItem::getData() const
{ return data_pointer; }

//----------------------------------------------------------------------
inline void FListBoxItem::setText (FString& txt)
{ text = txt; }

//----------------------------------------------------------------------
inline void FListBoxItem::setText (const std::string& txt)
{ text = txt; }

//----------------------------------------------------------------------
inline void FListBoxItem::setText (const char* txt)
{ text = txt; }

//----------------------------------------------------------------------
inline void FListBoxItem::setData (FWidget::data_ptr data)
{ data_pointer = data; }


//----------------------------------------------------------------------
// class FListBox
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FListBox : public FWidget
{
 public:
   // Using-declaration
   using FWidget::setGeometry;

   // Constructor
   explicit FListBox (FWidget* = 0);

   // Destructor
  ~FListBox();

   // Accessors
   const char*  getClassName() const;
   uInt         getCount() const;
   FListBoxItem getItem (int) const;
   int          currentItem() const;
   FString&     getText();

   // Mutators
   void         setCurrentItem (int);
   void         selectItem (int);
   void         unselectItem (int);
   void         showInsideBrackets (int, fc::brackets_type);
   void         showNoBrackets (int);
   void         setGeometry (int, int, int, int, bool = true);
   void         setMultiSelection (bool);
   void         setMultiSelection ();
   void         unsetMultiSelection ();
   bool         setEnable (bool);
   bool         setEnable();
   bool         unsetEnable();
   bool         setDisable();
   bool         setFocus (bool);
   bool         setFocus();
   bool         unsetFocus();
   void         setText (const FString);

   // Inquiries
   bool         isSelected (int) const;
   bool         isMultiSelection() const;
   bool         hasBrackets (int) const;

   // Methods
   void         hide();
   void         insert ( FString
                       , fc::brackets_type = fc::NoBrackets
                       , bool = false
                       , data_ptr = 0);
   void         insert ( long
                       , fc::brackets_type = fc::NoBrackets
                       , bool = false
                       , data_ptr = 0 );
   void         remove (int);
   void         clear();

   // Event handlers
   void         onKeyPress (FKeyEvent*);
   void         onMouseDown (FMouseEvent*);
   void         onMouseUp (FMouseEvent*);
   void         onMouseMove (FMouseEvent*);
   void         onMouseDoubleClick (FMouseEvent*);
   void         onWheel (FWheelEvent*);
   void         onTimer (FTimerEvent*);
   void         onFocusIn (FFocusEvent*);
   void         onFocusOut (FFocusEvent*);

   // Callback methods
   void         cb_VBarChange (FWidget*, data_ptr);
   void         cb_HBarChange (FWidget*, data_ptr);

 protected:
   // Methods
   void adjustYOffset();
   void adjustSize();

 private:
   // Typedef
   typedef std::vector<FListBoxItem> listBoxItem;

   // Enumeration
   enum dragScroll
   {
     noScroll         = 0,
     scrollUp         = 1,
     scrollDown       = 2,
     scrollUpSelect   = 3,
     scrollDownSelect = 4
   };

   // Disable copy constructor
   FListBox (const FListBox&);

   // Disable assignment operator (=)
   FListBox& operator = (const FListBox&);

   // Methods
   void        init();
   void        draw();
   void        drawLabel();
   void        drawList();
   void        processClick();
   void        processSelect();
   void        processChanged();

   // Data Members
   listBoxItem data;
   FScrollbar* vbar;
   FScrollbar* hbar;
   FString     text;
   FString     inc_search;
   bool        multi_select;
   bool        mouse_select;
   dragScroll  drag_scroll;
   bool        scroll_timer;
   int         scroll_repeat;
   int         scroll_distance;
   int         current;
   int         last_current;
   int         secect_from_item;
   int         xoffset;
   int         yoffset;
   int         last_yoffset;
   int         nf_offset;
   int         max_line_width;
};
#pragma pack(pop)


// FListBox inline functions
//----------------------------------------------------------------------
inline const char* FListBox::getClassName() const
{ return "FListBox"; }

//----------------------------------------------------------------------
inline uInt FListBox::getCount() const
{ return uInt(data.size()); }

//----------------------------------------------------------------------
inline FListBoxItem FListBox::getItem (int index) const
{ return data[uInt(index-1)]; }

//----------------------------------------------------------------------
inline int FListBox::currentItem() const
{ return current; }

//----------------------------------------------------------------------
inline FString& FListBox::getText()
{ return text; }

//----------------------------------------------------------------------
inline void FListBox::selectItem (int index)
{ data[uInt(index-1)].selected = true; }

//----------------------------------------------------------------------
inline void FListBox::unselectItem (int index)
{ data[uInt(index-1)].selected = false; }

//----------------------------------------------------------------------
inline void FListBox::showNoBrackets(int index)
{ data[uInt(index-1)].brackets = fc::NoBrackets; }

//----------------------------------------------------------------------
inline void FListBox::setMultiSelection (bool on)
{ multi_select = on; }

//----------------------------------------------------------------------
inline void FListBox::setMultiSelection()
{ setMultiSelection(true); }

//----------------------------------------------------------------------
inline void FListBox::unsetMultiSelection()
{ setMultiSelection(false); }

//----------------------------------------------------------------------
inline bool FListBox::setEnable()
{ return setEnable(true); }

//----------------------------------------------------------------------
inline bool FListBox::unsetEnable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FListBox::setDisable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FListBox::setFocus()
{ return setFocus(true); }

//----------------------------------------------------------------------
inline bool FListBox::unsetFocus()
{ return setFocus(false); }

//----------------------------------------------------------------------
inline bool FListBox::isSelected(int index) const
{ return data[uInt(index-1)].selected; }

//----------------------------------------------------------------------
inline bool FListBox::isMultiSelection() const
{ return multi_select; }

//----------------------------------------------------------------------
inline bool FListBox::hasBrackets(int index) const
{ return bool(data[uInt(index-1)].brackets > 0); }

#endif  // _FLISTBOX_H
