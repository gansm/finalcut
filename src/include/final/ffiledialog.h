/***********************************************************************
* ffiledialog.h - Widget FFileDialog (a file chooser dialog)           *
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
 *       ▕▔▔▔▔▔▔▔▔▔▏
 *       ▕ FWindow ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏
 *            ▲
 *            │
 *       ▕▔▔▔▔▔▔▔▔▔▏
 *       ▕ FDialog ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏
 *            ▲
 *            │
 *     ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *     ▕ FFileDialog ▏
 *     ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FFILEDIALOG_H
#define FFILEDIALOG_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <sys/param.h>
#include <sys/types.h>

#include <dirent.h>
#include <fnmatch.h>
#include <libgen.h>
#include <unistd.h>

#include <string>
#include <vector>

#include "final/fbutton.h"
#include "final/fcheckbox.h"
#include "final/fdialog.h"
#include "final/flineedit.h"
#include "final/flistbox.h"
#include "final/fmessagebox.h"
#include "final/fstatusbar.h"
#include "final/fterm.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FFileDialog
//----------------------------------------------------------------------

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
    explicit FFileDialog (FWidget* = nullptr);
    FFileDialog (const FFileDialog&);  // copy constructor
    FFileDialog ( const FString&
                , const FString&
                , DialogType = FFileDialog::Open
                , FWidget* = nullptr );

    // Destructor
    ~FFileDialog() override;

    // copy assignment operator (=)
    FFileDialog& operator = (const FFileDialog&);

    // Accessors
    FString              getClassName() const override;
    FString              getPath() const;
    FString              getFilter() const;
    FString              getSelectedFile() const;
    bool                 getShowHiddenFiles() const;

    // Mutators
    void                 setPath (const FString&);
    void                 setFilter (const FString&);
    bool                 setShowHiddenFiles(bool);
    bool                 setShowHiddenFiles();
    bool                 unsetShowHiddenFiles();

    // Event handler
    void                 onKeyPress (FKeyEvent*) override;

    // Methods
    static FString fileOpenChooser ( FWidget*
                                   , const FString& = FString()
                                   , const FString& = FString() );
    static FString fileSaveChooser ( FWidget*
                                   , const FString& = FString()
                                   , const FString& = FString() );

  protected:
    // Method
    void                 adjustSize() override;

  private:
    struct FDirEntry
    {
      // Constructor
      FDirEntry()
        : fifo{false}
        , character_device{false}
        , directory{false}
        , block_device{false}
        , regular_file{false}
        , symbolic_link{false}
        , socket{false}
      { }

      // Copy constructor
      FDirEntry (const FDirEntry& entry)
        : name{entry.name}
        , fifo{entry.fifo}
        , character_device{entry.character_device}
        , directory{entry.directory}
        , block_device{entry.block_device}
        , regular_file{entry.regular_file}
        , symbolic_link{entry.symbolic_link}
        , socket{entry.socket}
      { }

      // Destructor
      ~FDirEntry() = default;

      // Copy assignment operator (=)
      FDirEntry& operator = (const FDirEntry&) = default;

      // Data members
      std::string  name{};
      // Type of file
      uChar fifo             : 1;
      uChar character_device : 1;
      uChar directory        : 1;
      uChar block_device     : 1;
      uChar regular_file     : 1;
      uChar symbolic_link    : 1;
      uChar socket           : 1;
      uChar                  : 1;  // padding bits
    };

    using DirEntries = std::vector<FDirEntry>;

    // Methods
    void                 init();
    void                 widgetSettings (const FPoint&);
    void                 initCallbacks();
    bool                 patternMatch (const char* const, const char[]) const;
    void                 clear();
    sInt64               numOfDirs();
    void                 sortDir();
    int                  readDir();
    void                 getEntry (const char* const, const struct dirent*);
    void                 followSymLink (const char* const, FDirEntry&) const;
    void                 dirEntriesToList();
    void                 selectDirectoryEntry (const char* const);
    int                  changeDir (const FString&);
    void                 printPath (const FString&);
    static FString       getHomeDir();

    // Callback methods
    void                 cb_processActivate();
    void                 cb_processRowChanged();
    void                 cb_processClicked();
    void                 cb_processCancel();
    void                 cb_processOpen();
    void                 cb_processShowHidden();

    // Data members
    DIR*             directory_stream{nullptr};
    DirEntries       dir_entries{};
    FString          directory{};
    FString          filter_pattern{};
    FLineEdit        filename{this};
    FListBox         filebrowser{this};
    FCheckBox        hidden_check{this};
    FButton          cancel_btn{this};
    FButton          open_btn{this};
    DialogType       dlg_type{FFileDialog::Open};
    bool             show_hidden{false};

    // Friend functions
    friend bool sortByName ( const FFileDialog::FDirEntry&
                           , const FFileDialog::FDirEntry& );
    friend bool sortDirFirst ( const FFileDialog::FDirEntry&
                             , const FFileDialog::FDirEntry& );
    friend FString fileChooser ( FWidget*
                               , const FString&
                               , const FString&
                               , FFileDialog::DialogType);
};

// FMessageBox inline functions
//----------------------------------------------------------------------
inline FString FFileDialog::getClassName() const
{ return "FFileDialog"; }

//----------------------------------------------------------------------
inline FString FFileDialog::getPath() const
{ return directory; }

//----------------------------------------------------------------------
inline FString FFileDialog::getFilter() const
{ return filter_pattern; }

//----------------------------------------------------------------------
inline bool FFileDialog::setShowHiddenFiles()
{ return setShowHiddenFiles(true); }

//----------------------------------------------------------------------
inline bool FFileDialog::unsetShowHiddenFiles()
{ return setShowHiddenFiles(false); }

//----------------------------------------------------------------------
inline bool FFileDialog::getShowHiddenFiles() const
{ return show_hidden; }

}  // namespace finalcut

#endif  // FFILEDIALOG_H
