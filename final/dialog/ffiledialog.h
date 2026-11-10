/***********************************************************************
* ffiledialog.h - Widget FFileDialog (a file chooser dialog)           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2026 Markus Gans                                      *
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

#include <ctime>
#include <string>
#include <vector>

#include "final/dialog/fdialog.h"
#include "final/dialog/fmessagebox.h"
#include "final/output/tty/fterm.h"
#include "final/widget/fbutton.h"
#include "final/widget/fbuttongroup.h"
#include "final/widget/fcheckbox.h"
#include "final/widget/flineedit.h"
#include "final/widget/flistbox.h"
#include "final/widget/fradiobutton.h"
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
    enum class DialogType : uInt8
    {
      Open = 0,
      Save = 1,
      Directory = 2
    };

    // The order of the files in the list (directories always come first).
    // Each order can be reversed with setSortReverse()
    enum class SortOrder : uInt8
    {
      Name = 0,             // by name, case insensitive, A-Z (default)
      Date = 1,             // by modification time, the newest file first
      Size = 2,             // by size, the largest file first
      DateNewestFirst = Date  // (old name)
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
    auto getClassName() const -> FString override;
    auto getPath() const -> FString;
    auto getFilter() const -> FString;
    auto getSelectedFile() const -> FString;
    auto getShowHiddenFiles() const noexcept -> bool;
    auto getDialogType() const noexcept -> DialogType;
    auto getSortOrder() const noexcept -> SortOrder;
    auto getSortReverse() const noexcept -> bool;

    // Mutators
    void setPath (const FString&);
    void setFilter (const FString&);
    void setShowHiddenFiles (bool = true);
    void setSortOrder (SortOrder);
    void setSortReverse (bool = true);
    void setFilename (const FString&);
    void unsetShowHiddenFiles();

    // Event handler
    void onKeyPress (FKeyEvent*) override;

    // Methods
    static auto fileOpenChooser ( FWidget*
                                , const FString& = FString()
                                , const FString& = FString()
                                , SortOrder = SortOrder::Name
                                , bool = false ) -> FString;
    static auto fileSaveChooser ( FWidget*
                                , const FString& = FString()
                                , const FString& = FString() ) -> FString;
    static auto directoryChooser ( FWidget*
                                 , const FString& = FString() ) -> FString;

  protected:
    // Method
    void adjustSize() override;

  private:
    // Enumeration
    enum class CloseDir : uInt8 { success, error };

    enum class InputType : uInt8
    {
      Undefined,
      Filter,
      Directory,
      File
    };

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
      std::time_t  mtime{0};  // modification time (only read when sorting by date)
      off_t        size{0};   // file size (only read when sorting by size)
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
    void init();
    void widgetSettings (const FPoint&);
    void initCallbacks();
    auto patternMatch ( const std::string&
                      , const std::string& ) const -> bool;
    void clear();
    auto numOfDirs() -> sInt64;
    void sortDir();
    auto readDir() -> int;
    void getEntry (const std::string&, const struct dirent*);
    void followSymLink (const std::string&, FDirEntry&) const;
    auto openDirectory() -> DIR*;
    auto closeDirectory (DIR*) -> CloseDir;
    bool shouldSkipEntry (const struct dirent*, const std::string&) const;
    void readDirEntries (DIR*);
    auto getSelectedFilename() const -> std::string;
    auto interpretInputType() const -> InputType;
    auto isCurrentDirectory (const struct dirent*) const -> bool;
    auto isParentDirectory (const struct dirent*) const -> bool;
    auto isHiddenEntry (const struct dirent*) const -> bool;
    auto isRootDirectory (const std::string&) const -> bool;
    void dirEntriesToList();
    void selectDirectoryEntry (const std::string&);
    auto getAbsolutePath (const FString&) const -> FString;
    void handleParentDirectoryChange (const FString&, bool);
    void handleDefaultDirectorySelection();
    auto changeDir (const FString&) -> int;
    void printPath (const FString&);
    void setTitelbarText();
    auto isFilterInput() const -> bool;
    auto isDirectoryInput() const -> bool;
    void activateNewFilter();
    void activateDefaultFilter();
    void findItem (const FString&);
    void changeIntoSubDir();
    auto confirmOverwrite (const std::string&) -> FMessageBox::ButtonType;
    void processSaveSelection();

    // Callback methods
    void cb_processActivate();
    void cb_processRowChanged();
    void cb_processClicked();
    void cb_processCancel();
    void cb_processOpen();
    void cb_processShowHidden();
    void cb_processSort();

    // Data members
    DirEntries  dir_entries{};
    FString     directory{};
    FString     filter_pattern{};
    FLineEdit   file_name{this};
    FListBox    file_browser{this};
    FCheckBox   hidden_check{this};
    FButtonGroup  sort_group{"sort by", this};
    FRadioButton  sort_name{"n&ame", &sort_group};
    FRadioButton  sort_date{"&date", &sort_group};
    FRadioButton  sort_size{"si&ze", &sort_group};
    FCheckBox     sort_reverse{"&rev", &sort_group};
    FButton     cancel_btn{this};
    FButton     open_btn{this};
    DialogType  dlg_type{DialogType::Open};
    bool        show_hidden{false};
    SortOrder   sort_order{SortOrder::Name};
    bool        sort_rev{false};

    // Friend functions
    friend auto sortFileByName ( const FFileDialog::FDirEntry&
                               , const FFileDialog::FDirEntry& ) -> bool;
    friend auto sortFileByDate ( const FFileDialog::FDirEntry&
                               , const FFileDialog::FDirEntry& ) -> bool;
    friend auto sortFileBySize ( const FFileDialog::FDirEntry&
                               , const FFileDialog::FDirEntry& ) -> bool;
    friend auto sortDirByName ( const FFileDialog::FDirEntry&
                              , const FFileDialog::FDirEntry& ) -> bool;
    friend auto sortDirFirst (const FFileDialog::FDirEntry&) -> bool;
    friend auto isRegularFile (const std::string&) noexcept -> bool;
    friend void setAndSelectFilename (FFileDialog&, const FString&);
    friend auto Chooser ( FWidget*
                        , const FString&
                        , const FString&
                        , FFileDialog::DialogType
                        , FFileDialog::SortOrder
                        , bool ) -> FString;
};

// FMessageBox inline functions
//----------------------------------------------------------------------
inline auto FFileDialog::getClassName() const -> FString
{ return "FFileDialog"; }

//----------------------------------------------------------------------
inline auto FFileDialog::getPath() const -> FString
{ return directory; }

//----------------------------------------------------------------------
inline auto FFileDialog::getFilter() const -> FString
{ return filter_pattern; }

//----------------------------------------------------------------------
inline void FFileDialog::unsetShowHiddenFiles()
{ setShowHiddenFiles(false); }

//----------------------------------------------------------------------
inline auto FFileDialog::getShowHiddenFiles() const noexcept -> bool
{ return show_hidden; }

//----------------------------------------------------------------------
inline auto FFileDialog::getDialogType() const noexcept -> DialogType
{ return dlg_type; }

//----------------------------------------------------------------------
inline auto FFileDialog::getSortOrder() const noexcept -> SortOrder
{ return sort_order; }

//----------------------------------------------------------------------
inline auto FFileDialog::getSortReverse() const noexcept -> bool
{ return sort_rev; }

}  // namespace finalcut

#endif  // FFILEDIALOG_H
