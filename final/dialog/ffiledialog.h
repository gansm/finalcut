/***********************************************************************
* ffiledialog.h - Widget FFileDialog (a file chooser dialog)           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2022 Markus Gans                                      *
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

#include "final/dialog/fdialog.h"
#include "final/dialog/fmessagebox.h"
#include "final/output/tty/fterm.h"
#include "final/widget/fbutton.h"
#include "final/widget/fcheckbox.h"
#include "final/widget/flineedit.h"
#include "final/widget/flistbox.h"
#include "final/widget/fstatusbar.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FFileDialog
//----------------------------------------------------------------------

class FFileDialog : public FDialog
{
  public:
    // Enumeration
    enum class DialogType
    {
      Open = 0,
      Save = 1
    };

    // Constructors
    explicit FFileDialog (FWidget* = nullptr);

    FFileDialog ( const FString&
                , FString&&
                , DialogType = DialogType::Open
                , FWidget* = nullptr );

    // Destructor
    ~FFileDialog() noexcept override;

    // Accessors
    FString              getClassName() const override;
    FString              getPath() const;
    FString              getFilter() const;
    FString              getSelectedFile() const;
    bool                 getShowHiddenFiles() const noexcept;

    // Mutators
    void                 setPath (const FString&);
    void                 setFilter (const FString&);
    bool                 setShowHiddenFiles (bool = true);
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
    bool                 patternMatch ( const std::string&
                                      , const std::string& ) const;
    void                 clear();
    sInt64               numOfDirs();
    void                 sortDir();
    int                  readDir();
    void                 getEntry (const char* const, const struct dirent*);
    void                 followSymLink (const char* const, FDirEntry&) const;
    void                 dirEntriesToList();
    void                 selectDirectoryEntry (const std::string&);
    int                  changeDir (const FString&);
    void                 printPath (const FString&);
    void                 setTitelbarText();
    static FString       getHomeDir();

    // Callback methods
    void                 cb_processActivate();
    void                 cb_processRowChanged();
    void                 cb_processClicked();
    void                 cb_processCancel();
    void                 cb_processOpen();
    void                 cb_processShowHidden();

    // Data members
    DirEntries       dir_entries{};
    FString          directory{};
    FString          filter_pattern{};
    FLineEdit        filename{this};
    FListBox         filebrowser{this};
    FCheckBox        hidden_check{this};
    FButton          cancel_btn{this};
    FButton          open_btn{this};
    DialogType       dlg_type{DialogType::Open};
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
inline bool FFileDialog::unsetShowHiddenFiles()
{ return setShowHiddenFiles(false); }

//----------------------------------------------------------------------
inline bool FFileDialog::getShowHiddenFiles() const noexcept
{ return show_hidden; }

}  // namespace finalcut

#endif  // FFILEDIALOG_H
