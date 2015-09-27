// File: fstatusbar.h
// Provides: class FStatusKey
//           class FStatusBar
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
//       ▕▔▔▔▔▔▔▔▔▔▏
//       ▕ FWidget ▏
//       ▕▁▁▁▁▁▁▁▁▁▏
//            ▲
//            │
//            ├────────────────────┐
//            │                    │
//       ▕▔▔▔▔▔▔▔▔▔▏               │
//       ▕ FWindow ▏               │
//       ▕▁▁▁▁▁▁▁▁▁▏               │
//            ▲                    │
//            │                    │
//     ▕▔▔▔▔▔▔▔▔▔▔▔▔▏1     *▕▔▔▔▔▔▔▔▔▔▔▔▔▏
//     ▕ FStatusBar ▏- - - -▕ FStatusKey ▏
//     ▕▁▁▁▁▁▁▁▁▁▁▁▁▏       ▕▁▁▁▁▁▁▁▁▁▁▁▁▏

#ifndef _FSTATUSBAR_H
#define _FSTATUSBAR_H

#include "fwindow.h"


class FStatusBar;

//----------------------------------------------------------------------
// class FStatusKey
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FStatusKey : public FWidget
{
 private:
   int         key;
   FString     text;
   bool        active;
   bool        mouse_focus;
   FStatusBar* bar;

 private:
   FStatusKey (const FStatusKey&);
   FStatusKey& operator = (const FStatusKey&);
   void init (FWidget*);
   void processActivate();
   FStatusBar* statusbar() const;
   void setStatusbar (FStatusBar*);

 public:
   explicit FStatusKey (FWidget* = 0);
   FStatusKey (int, FString&, FWidget* = 0);
   FStatusKey (int, const std::string&, FWidget* = 0);
   FStatusKey (int, const char*, FWidget* = 0);
   virtual ~FStatusKey();

   void onAccel (FAccelEvent*);
   void setActive();
   void unsetActive();
   bool isActivated() const;
   bool setMouseFocus(bool);
   bool setMouseFocus();
   bool unsetMouseFocus();
   bool hasMouseFocus() const;
   virtual int getKey() const;
   virtual FString getText() const;

 protected:
   void setKey (int);
   void setText (FString&);
   void setText (const std::string&);
   void setText (const char*);

 private:
   friend class FStatusBar;
};
#pragma pack(pop)


// FStatusKey inline functions
//----------------------------------------------------------------------
inline void FStatusKey::unsetActive()
{ active = false; }

//----------------------------------------------------------------------
inline bool FStatusKey::isActivated() const
{ return active; }

//----------------------------------------------------------------------
inline bool FStatusKey::setMouseFocus()
{ return setMouseFocus(true); }

//----------------------------------------------------------------------
inline bool FStatusKey::unsetMouseFocus()
{ return setMouseFocus(false); }

//----------------------------------------------------------------------
inline bool FStatusKey::hasMouseFocus() const
{ return mouse_focus; }

//----------------------------------------------------------------------
inline int FStatusKey::getKey() const
{ return key; }

//----------------------------------------------------------------------
inline FString FStatusKey::getText() const
{ return text; }

//----------------------------------------------------------------------
inline void FStatusKey::setKey (int k)
{ key = k; }

//----------------------------------------------------------------------
inline void FStatusKey::setText (FString& txt)
{ text = txt; }

//----------------------------------------------------------------------
inline void FStatusKey::setText (const std::string& txt)
{ text = txt; }

//----------------------------------------------------------------------
inline void FStatusKey::setText (const char* txt)
{ text = txt; }


//----------------------------------------------------------------------
// class FStatusBar
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FStatusBar : public FWindow
{
 private:
   std::vector<FStatusKey*> keylist;
   FString text;
   bool    mouse_down;
   int     x;
   int     x_msg;

 private:
   FStatusBar (const FStatusBar&);
   FStatusBar& operator = (const FStatusBar&);
   void init();
   void draw();
   void drawKeys();
   void adjustSize();

 public:
   explicit FStatusBar (FWidget* = 0);  // constructor
   virtual ~FStatusBar();  // destructor
   virtual const char* getClassName() const;

   void onMouseDown (FMouseEvent*);
   void onMouseUp (FMouseEvent*);
   void onMouseMove (FMouseEvent*);
   void hide();
   // make every setGeometry from FWidget available
   using FWidget::setGeometry;
   void setGeometry (int, int, int, int, bool = true);

   uInt count() const;
   FStatusKey* key (int) const;
   void activateKey (int);
   void deactivateKey (int);
   bool isActivated (int) const;
   bool hasActivatedKey();

   void drawMessage();
   void setMessage (const FString);
   FString getMessage() const;
   void clearMessage();

   void insert (FStatusKey*);
   void remove (FStatusKey*);
   void remove (int);
   void clear();

   void cb_statuskey_activated (FWidget*, void*);
};
#pragma pack(pop)


// FStatusBar inline functions
//----------------------------------------------------------------------
inline const char* FStatusBar::getClassName() const
{ return "FStatusBar"; }

//----------------------------------------------------------------------
inline uInt FStatusBar::count() const
{ return uInt(keylist.size()); }

//----------------------------------------------------------------------
inline FStatusKey* FStatusBar::key(int index) const
{ return keylist[uInt(index-1)]; }

//----------------------------------------------------------------------
inline void FStatusBar::activateKey (int index)
{ keylist[uInt(index-1)]->setActive(); }

//----------------------------------------------------------------------
inline void FStatusBar::deactivateKey (int index)
{ keylist[uInt(index-1)]->unsetActive(); }

//----------------------------------------------------------------------
inline bool FStatusBar::isActivated(int index) const
{ return keylist[uInt(index-1)]->isActivated(); }

//----------------------------------------------------------------------
inline void FStatusBar::setMessage (FString mgs)
{ text = mgs; }

//----------------------------------------------------------------------
inline FString FStatusBar::getMessage() const
{ return text; }

//----------------------------------------------------------------------
inline void FStatusBar::clearMessage()
{ text.clear(); }

#endif  // _FSTATUSBAR_H
