// File: ffiledialog.h
// Provides: class FFileDialog.h
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
//       ▕▔▔▔▔▔▔▔▔▔▏
//       ▕ FWindow ▏
//       ▕▁▁▁▁▁▁▁▁▁▏
//            ▲
//            │
//       ▕▔▔▔▔▔▔▔▔▔▏
//       ▕ FDialog ▏
//       ▕▁▁▁▁▁▁▁▁▁▏
//            ▲
//            │
//     ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//     ▕ FFileDialog ▏
//     ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏

#ifndef _FFILEDIALOG_H
#define _FFILEDIALOG_H

#include <sys/param.h>

#include <dirent.h>
#include <fnmatch.h>
#include <libgen.h>
#include <pwd.h>

#include <string>

#include "fbutton.h"
#include "fcheckbox.h"
#include "fdialog.h"
#include "flineedit.h"
#include "flistbox.h"
#include "fmessagebox.h"
#include "fstatusbar.h"
#include "fterm.h"


#pragma pack(push)
#pragma pack(1)

struct dir_entry
{
  char* name;
  uChar type;
};
#pragma pack(pop)


//----------------------------------------------------------------------
// class FFileDialog
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FFileDialog : public FDialog
{
 public:
   enum DialogType
   {
     Open = 0,
     Save = 1
   };
 private:
   DIR* directory_stream;
   std::vector<dir_entry> dir_entries;

   FString    directory;
   FString    filter_pattern;
   FListBox*  filebrowser;
   FLineEdit* filename;
   FCheckBox* hidden;
   FButton*   cancel;
   FButton*   open;
   DialogType dlg_type;
   bool       show_hidden;

 private:
   void          init();
   static char*  getHomeDir();
   inline bool   pattern_match (const char*, const char*);
   void          clear();
   int           numOfDirs();
   int           changeDir (const FString&);
   void          printPath (const FString&);
   void          cb_processActivate (FWidget*, void*);
   void          cb_processRowChanged (FWidget*, void*);
   void          cb_processClicked (FWidget*, void*);
   void          cb_processCancel (FWidget*, void*);
   void          cb_processOpen (FWidget*, void*);
   void          cb_processShowHidden (FWidget*, void*);

 protected:
   void adjustSize();

 public:
   explicit FFileDialog (FWidget* = 0);
   FFileDialog (const FFileDialog&);       // copy constructor
   FFileDialog ( const FString&
               , const FString&
               , DialogType = FFileDialog::Open
               , FWidget* = 0 );
  ~FFileDialog();
   FFileDialog& operator = (const FFileDialog&); // assignment
   const char*   getClassName() const;

   void          onKeyPress (FKeyEvent*);

   const FString getPath() const;
   void          setPath (const FString&);
   const FString getFilter() const;
   void          setFilter (const FString&);
   const FString getSelectedFile() const;
   int           readDir();
   bool          setShowHiddenFiles(bool);
   bool          setShowHiddenFiles();
   bool          unsetShowHiddenFiles();
   bool          getShowHiddenFiles();

   static FString fileOpenChooser ( FWidget*
                                  , const FString& = FString()
                                  , const FString& = FString() );
   static FString fileSaveChooser ( FWidget*
                                  , const FString& = FString()
                                  , const FString& = FString() );
};
#pragma pack(pop)


// FMessageBox inline functions
//----------------------------------------------------------------------
inline const char* FFileDialog::getClassName() const
{ return "FFileDialog"; }

//----------------------------------------------------------------------
inline const FString FFileDialog::getPath() const
{ return directory; }

//----------------------------------------------------------------------
inline const FString FFileDialog::getFilter() const
{ return filter_pattern; }

//----------------------------------------------------------------------
inline bool FFileDialog::setShowHiddenFiles()
{ return setShowHiddenFiles(true); }

//----------------------------------------------------------------------
inline bool FFileDialog::unsetShowHiddenFiles()
{ return setShowHiddenFiles(false); }

//----------------------------------------------------------------------
inline bool FFileDialog::getShowHiddenFiles()
{ return show_hidden; }

#endif  // _FFILEDIALOG_H

