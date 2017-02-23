// File: ffiledialog.h
// Provides: class FFileDialog
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


//----------------------------------------------------------------------
// class FFileDialog
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FFileDialog : public FDialog
{
 public:
   // Enumeration
   enum DialogType
   {
     Open = 0,
     Save = 1
   };

   // Constructors
   explicit FFileDialog (FWidget* = 0);
   FFileDialog (const FFileDialog&);  // copy constructor
   FFileDialog ( const FString&
               , const FString&
               , DialogType = FFileDialog::Open
               , FWidget* = 0 );
   // Destructor
  ~FFileDialog();

   // Assignment operator (=)
   FFileDialog& operator = (const FFileDialog&);

   // Accessors
   const char*   getClassName() const;
   const FString getPath() const;
   const FString getFilter() const;
   const FString getSelectedFile() const;
   bool          getShowHiddenFiles();

   // Mutators
   void          setPath (const FString&);
   void          setFilter (const FString&);
   bool          setShowHiddenFiles(bool);
   bool          setShowHiddenFiles();
   bool          unsetShowHiddenFiles();

   // Event handler
   void          onKeyPress (FKeyEvent*);

   // Methods
   int           readDir();
   static FString fileOpenChooser ( FWidget*
                                  , const FString& = FString()
                                  , const FString& = FString() );
   static FString fileSaveChooser ( FWidget*
                                  , const FString& = FString()
                                  , const FString& = FString() );

 protected:
   // Method
   void adjustSize();

 private:
   // Typedef
   struct dir_entry
   {
     char* name;
     uChar type;
   };

   typedef std::vector<dir_entry> dirEntries;

   // Method
   void          init();
   static char*  getHomeDir();
   inline bool   pattern_match (const char*, const char*);
   void          clear();
   int           numOfDirs();
   int           changeDir (const FString&);
   void          printPath (const FString&);

   // Callback methods
   void          cb_processActivate (FWidget*, data_ptr);
   void          cb_processRowChanged (FWidget*, data_ptr);
   void          cb_processClicked (FWidget*, data_ptr);
   void          cb_processCancel (FWidget*, data_ptr);
   void          cb_processOpen (FWidget*, data_ptr);
   void          cb_processShowHidden (FWidget*, data_ptr);

   // Data Members
   DIR*          directory_stream;
   dirEntries    dir_entries;
   FString       directory;
   FString       filter_pattern;
   FListBox*     filebrowser;
   FLineEdit*    filename;
   FCheckBox*    hidden;
   FButton*      cancel;
   FButton*      open;
   DialogType    dlg_type;
   bool          show_hidden;

   // Friend functions
   friend bool sortByName ( const FFileDialog::dir_entry&
                          , const FFileDialog::dir_entry& );
   friend bool sortDirFirst ( const FFileDialog::dir_entry&
                            , const FFileDialog::dir_entry& );
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

