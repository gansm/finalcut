// flistbox.h
// class FListBoxItem
// class FListBox

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
 private:
   FString text;
   fc::brackets_type brackets;
   bool selected;
 public:
   FListBoxItem ();
   explicit FListBoxItem (FString&);
   explicit FListBoxItem (const std::string&);
   explicit FListBoxItem (const char*);
   virtual ~FListBoxItem();
   virtual FString getText() const;
 protected:
   void setText (FString&);
   void setText (const std::string&);
   void setText (const char*);
 private:
   friend class FListBox;
};
#pragma pack(pop)

// FListBoxItem inline functions
//----------------------------------------------------------------------
inline FString FListBoxItem::getText() const
{ return this->text; }

//----------------------------------------------------------------------
inline void FListBoxItem::setText (FString& txt)
{ this->text = txt; }

//----------------------------------------------------------------------
inline void FListBoxItem::setText (const std::string& txt)
{ this->text = txt; }

//----------------------------------------------------------------------
inline void FListBoxItem::setText (const char* txt)
{ this->text = txt; }


//----------------------------------------------------------------------
// class FListBox
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FListBox : public FWidget
{
 private:
   std::vector<FListBoxItem> data;
   FScrollbar* VBar;
   FScrollbar* HBar;
   FString text;
   FString inc_search;
   bool multiSelect;
   bool mouseSelect;
   enum drag_scroll
   {
     noScroll         = 0,
     scrollUp         = 1,
     scrollDown       = 2,
     scrollUpSelect   = 3,
     scrollDownSelect = 4
   };
   int  dragScroll;
   bool scrollTimer;
   int  scrollRepeat;
   int  scrollDistance;
   int  current;
   int  last_current;
   int  secect_from_item;
   int  xoffset;
   int  yoffset;
   int  last_yoffset;
   int  nf_offset;
   int  maxLineWidth;

 private:
   FListBox (const FListBox&);
   FListBox& operator = (const FListBox&);
   void init();
   void draw();
   void drawLabel();
   void drawList();
   void processClick();
   void processSelect();
   void processChanged();

 protected:
   void adjustYOffset();
   void adjustSize();

 public:
   explicit FListBox (FWidget* parent=0);  // constructor
  ~FListBox();  // destructor
   const char* getClassName() const;

   void hide();

   void onKeyPress (FKeyEvent*);
   void onMouseDown (FMouseEvent*);
   void onMouseUp (FMouseEvent*);
   void onMouseMove (FMouseEvent*);
   void onMouseDoubleClick (FMouseEvent*);
   void onWheel (FWheelEvent*);
   void onTimer (FTimerEvent*);
   void onFocusIn (FFocusEvent*);
   void onFocusOut (FFocusEvent*);

   void cb_VBarChange (FWidget*, void*);
   void cb_HBarChange (FWidget*, void*);
   uInt count() const;
   FListBoxItem Item(int) const;
   int  currentItem() const;
   void setCurrentItem(int);
   void selectItem(int);
   void unselectItem(int);
   bool isSelected(int) const;
   void showInsideBrackets(int, fc::brackets_type);
   void showNoBrackets(int);
   bool hasBrackets(int) const;
   void setGeometry (int, int, int, int, bool adjust=true);

   void setMultiSelection (bool);
   void setMultiSelection ();
   void unsetMultiSelection ();
   bool isMultiSelection() const;
   bool setEnable(bool);
   bool setEnable();
   bool unsetEnable();
   bool setDisable();
   bool setFocus(bool);
   bool setFocus();
   bool unsetFocus();
   bool setShadow(bool);
   bool setShadow();
   bool unsetShadow();
   bool hasShadow();

   void insert ( FString,
                 fc::brackets_type b = fc::NoBrackets,
                 bool s = false );
   void insert ( long,
                 fc::brackets_type b = fc::NoBrackets,
                 bool s = false );
   void remove ( int);
   void clear();

   void setText (const FString);
   FString& getText();
};
#pragma pack(pop)


// FListBox inline functions
//----------------------------------------------------------------------
inline const char* FListBox::getClassName() const
{ return "FListBox"; }

//----------------------------------------------------------------------
inline uInt FListBox::count() const
{ return uInt(data.size()); }

//----------------------------------------------------------------------
inline FListBoxItem FListBox::Item(int index) const
{ return data[uInt(index-1)]; }

//----------------------------------------------------------------------
inline int FListBox::currentItem() const
{ return current; }

//----------------------------------------------------------------------
inline void FListBox::selectItem (int index)
{ data[uInt(index-1)].selected = true; }

//----------------------------------------------------------------------
inline void FListBox::unselectItem (int index)
{ data[uInt(index-1)].selected = false; }

//----------------------------------------------------------------------
inline bool FListBox::isSelected(int index) const
{ return data[uInt(index-1)].selected; }

//----------------------------------------------------------------------
inline void FListBox::showNoBrackets(int index)
{ data[uInt(index-1)].brackets = fc::NoBrackets; }

//----------------------------------------------------------------------
inline bool FListBox::hasBrackets(int index) const
{ return bool(data[uInt(index-1)].brackets > 0); }

//----------------------------------------------------------------------
inline void FListBox::setMultiSelection (bool on)
{ this->multiSelect = on; }

//----------------------------------------------------------------------
inline void FListBox::setMultiSelection()
{ setMultiSelection(true); }

//----------------------------------------------------------------------
inline void FListBox::unsetMultiSelection()
{ setMultiSelection(false); }

//----------------------------------------------------------------------
inline bool FListBox::isMultiSelection() const
{ return multiSelect; }

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
inline bool FListBox::setShadow()
{ return setShadow(true); }

//----------------------------------------------------------------------
inline bool FListBox::unsetShadow()
{ return setShadow(false); }

//----------------------------------------------------------------------
inline bool FListBox::hasShadow()
{ return ((flags & SHADOW) != 0); }

//----------------------------------------------------------------------
inline FString& FListBox::getText()
{ return this->text; }

#endif  // _FLISTBOX_H
