// fmenuitem.cpp
// class FMenuItem

#include "fmenuitem.h"
#include "fmenulist.h"

//----------------------------------------------------------------------
// class FMenuItem
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FMenuItem::FMenuItem (FWidget* parent) : FWidget(parent)
{
  init (parent);
}

//----------------------------------------------------------------------
FMenuItem::FMenuItem (FString& txt, FWidget* parent)
{
  setText(txt);
  init (parent);
}

//----------------------------------------------------------------------
FMenuItem::FMenuItem (const std::string& txt, FWidget* parent) : FWidget(parent)
{
  setText(txt);
  init (parent);
}

//----------------------------------------------------------------------
FMenuItem::FMenuItem (const char* txt, FWidget* parent) : FWidget(parent)
{
  setText(txt);
  init (parent);
}

//----------------------------------------------------------------------
FMenuItem::~FMenuItem()  // destructor
{
}


// private methods of FMenuItem
//----------------------------------------------------------------------
void FMenuItem::init (FWidget* parent)
{
  active    = true;
  selected  = false;
  separator = false;
  checked   = false;
  //menu = 0
  setGeometry (1,1,1,1);

  if ( parent && isMenuBar(parent) )
  {
    setSuperMenu( dynamic_cast<FMenuList*>(parent) );
    superMenu()->insert(this);
  }
}

//----------------------------------------------------------------------
bool FMenuItem::isMenuBar (FWidget* w) const
{
  return bool ( strcmp ( w->getClassName(),
                         const_cast<char*>("FMenuBar") ) == 0 );
}


// protected methods of FMenuItem
//----------------------------------------------------------------------
void FMenuItem::processActivate()
{
}

// public methods of FMenuItem
//----------------------------------------------------------------------
void FMenuItem::onAccel (FAccelEvent* event)
{
  if ( isSelected() )
  {
    unsetSelected();
    FWidget* w = reinterpret_cast<FWidget*>(superMenu());
    if ( isMenuBar(w) )
      w->redraw();
    event->accept();
  }
}

//----------------------------------------------------------------------
FMenuList* FMenuItem::superMenu() const
{
  return super_menu;
}

//----------------------------------------------------------------------
void FMenuItem::setSuperMenu (FMenuList* smenu)
{
  super_menu = smenu;
}

//----------------------------------------------------------------------
inline void FMenuItem::setActive()
{
  this->active = true;
  processActivate();
}
