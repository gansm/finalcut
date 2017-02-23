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


// class forward declaration
class FStatusBar;

//----------------------------------------------------------------------
// class FStatusKey
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FStatusKey : public FWidget
{
 public:
   // Constructors
   explicit FStatusKey (FWidget* = 0);
   FStatusKey (int, FString&, FWidget* = 0);
   FStatusKey (int, const std::string&, FWidget* = 0);
   FStatusKey (int, const char*, FWidget* = 0);

   // Destructor
   virtual ~FStatusKey();

   // Accessors
   virtual const char* getClassName() const;
   virtual int         getKey() const;
   virtual FString     getText() const;

   // Mutators
   void                setActive();
   void                unsetActive();
   bool                setMouseFocus(bool);
   bool                setMouseFocus();
   bool                unsetMouseFocus();

   // Inquiry
   bool                isActivated() const;
   bool                hasMouseFocus() const;

   // Event handler
   void                onAccel (FAccelEvent*);

 protected:
   // Mutators
   void                setKey (int);
   void                setText (FString&);
   void                setText (const std::string&);
   void                setText (const char*);

 private:
   // Disable copy constructor
   FStatusKey (const FStatusKey&);

   // Disable assignment operator (=)
   FStatusKey& operator = (const FStatusKey&);

   // Methods
   void                init (FWidget*);
   void                processActivate();
   FStatusBar*         getConnectedStatusbar() const;
   void                setConnectedStatusbar (FStatusBar*);

   // Friend class
   friend class FStatusBar;

   // Data Members
   int         key;
   FString     text;
   bool        active;
   bool        mouse_focus;
   FStatusBar* bar;
};
#pragma pack(pop)


// FStatusKey inline functions
//----------------------------------------------------------------------
inline const char* FStatusKey::getClassName() const
{ return "FStatusKey"; }

//----------------------------------------------------------------------
inline int FStatusKey::getKey() const
{ return key; }

//----------------------------------------------------------------------
inline FString FStatusKey::getText() const
{ return text; }

//----------------------------------------------------------------------
inline void FStatusKey::unsetActive()
{ active = false; }

//----------------------------------------------------------------------
inline bool FStatusKey::setMouseFocus()
{ return setMouseFocus(true); }

//----------------------------------------------------------------------
inline bool FStatusKey::unsetMouseFocus()
{ return setMouseFocus(false); }

//----------------------------------------------------------------------
inline bool FStatusKey::isActivated() const
{ return active; }

//----------------------------------------------------------------------
inline bool FStatusKey::hasMouseFocus() const
{ return mouse_focus; }

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
inline FStatusBar* FStatusKey::getConnectedStatusbar() const
{ return bar; }

//----------------------------------------------------------------------
inline void FStatusKey::setConnectedStatusbar (FStatusBar* sb)
{ bar = sb; }


//----------------------------------------------------------------------
// class FStatusBar
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FStatusBar : public FWindow
{
 public:
   // Constructor
   explicit FStatusBar (FWidget* = 0);

   // Destructor
   virtual ~FStatusBar();

   // Accessors
   virtual const char* getClassName() const;
   FStatusKey*         getStatusKey (int) const;
   FString             getMessage() const;
   uInt                getCount() const;

   // Mutators
   void                activateKey (int);
   void                deactivateKey (int);
   void                setMessage (FString&);
   void                setMessage (const std::string&);
   void                setMessage (const char*);

   // Inquiries
   bool                isActivated (int) const;
   bool                hasActivatedKey();

   // Methods
   void                hide();
   void                drawMessage();
   void                clearMessage();
   void                insert (FStatusKey*);
   void                remove (FStatusKey*);
   void                remove (int);
   void                clear();
   void                adjustSize();

   // Event handlers
   void                onMouseDown (FMouseEvent*);
   void                onMouseUp (FMouseEvent*);
   void                onMouseMove (FMouseEvent*);

   // Callback method
   void                cb_statuskey_activated (FWidget*, data_ptr);

 private:
   // Typedef
   typedef std::vector<FStatusKey*> keyList;

   // Disable copy constructor
   FStatusBar (const FStatusBar&);

   // Disable assignment operator (=)
   FStatusBar& operator = (const FStatusBar&);

   // Methods
   void                init();
   void                draw();
   void                drawKeys();

   // Data Members
   keyList             key_list;
   FString             text;
   bool                mouse_down;
   int                 x;
   int                 x_msg;
};
#pragma pack(pop)


// FStatusBar inline functions
//----------------------------------------------------------------------
inline const char* FStatusBar::getClassName() const
{ return "FStatusBar"; }

//----------------------------------------------------------------------
inline FStatusKey* FStatusBar::getStatusKey (int index) const
{ return key_list[uInt(index-1)]; }

//----------------------------------------------------------------------
inline uInt FStatusBar::getCount() const
{ return uInt(key_list.size()); }

//----------------------------------------------------------------------
inline void FStatusBar::activateKey (int index)
{ key_list[uInt(index-1)]->setActive(); }

//----------------------------------------------------------------------
inline void FStatusBar::deactivateKey (int index)
{ key_list[uInt(index-1)]->unsetActive(); }

//----------------------------------------------------------------------
inline bool FStatusBar::isActivated(int index) const
{ return key_list[uInt(index-1)]->isActivated(); }

//----------------------------------------------------------------------
inline FString FStatusBar::getMessage() const
{ return text; }

//----------------------------------------------------------------------
inline void FStatusBar::clearMessage()
{ text.clear(); }

#endif  // _FSTATUSBAR_H
