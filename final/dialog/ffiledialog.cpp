/***********************************************************************
* ffiledialog.cpp - Widget FFileDialog (a file chooser dialog)         *
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

#if defined(__CYGWIN__)
  #define _BSD_SOURCE  // for D_TYPE in readdir
#endif

#include <algorithm>
#include <array>
#include <utility>
#include <vector>

#include "final/dialog/ffiledialog.h"
#include "final/fevent.h"
#include "final/util/fsystem.h"

#if defined(__GNU__)
  #define MAXPATHLEN 4096  // Necessity at GNU Hurd
#endif

namespace finalcut
{

// non-member functions
//----------------------------------------------------------------------
auto sortFileByName ( const FFileDialog::FDirEntry& lhs
                    , const FFileDialog::FDirEntry& rhs ) -> bool
{
  // lhs < rhs
  return FStringCaseCompare(lhs.name, rhs.name) < 0;
}

//----------------------------------------------------------------------
auto sortDirByName ( const FFileDialog::FDirEntry& lhs
                   , const FFileDialog::FDirEntry& rhs ) -> bool
{
  // lhs < rhs

  if ( lhs.name == rhs.name )
    return false;

  if ( lhs.name == ".." )  // ".." always comes first
    return true;

  if ( rhs.name == ".." )
    return false;

  return FStringCaseCompare(lhs.name, rhs.name) < 0;
}

//----------------------------------------------------------------------
auto sortDirFirst (const FFileDialog::FDirEntry& entry) -> bool
{
  // sort directories first
  return entry.directory;
}

//----------------------------------------------------------------------
auto isDirectory (const std::string& path) noexcept -> bool
{
  struct stat sb{};
  return ::stat(path.data(), &sb) == 0 && S_ISDIR(sb.st_mode);
}

//----------------------------------------------------------------------
auto isRegularFile (const std::string& path) noexcept -> bool
{
  struct stat sb{};
  return ::stat(path.data(), &sb) == 0 && S_ISREG(sb.st_mode);
}

//----------------------------------------------------------------------
auto isFileAccessible (const std::string& path) noexcept -> bool
{
  if ( ! path.empty() )
    return ::access( path.data(), F_OK ) == 0;

  return false;
}

//----------------------------------------------------------------------
void checkPath (const std::string& path, bool& exists, bool& is_dir) noexcept
{
  struct stat sb{};

  if ( ::stat(path.data(), &sb) == 0)
  {
    exists = true;
    is_dir = S_ISDIR(sb.st_mode);
  }
  else
  {
    exists = false;
    is_dir = false;
  }
}

//----------------------------------------------------------------------
auto getParentPath (const std::string& path) -> std::string
{
  if ( path.empty() )
    return {};

  auto end_pos = path.size();

  while ( end_pos > 1 && path[end_pos - 1] == '/' )
    end_pos--;

  const auto last_slash = path.find_last_of('/', end_pos - 1);

  if ( last_slash == std::string::npos )
    return {};

  if ( last_slash == 0 )
    return "/";

  return path.substr(0, last_slash);
}

//----------------------------------------------------------------------
auto getWorkingDirectory (const std::string& path) -> std::string
{
  bool exists = false;
  bool is_dir = false;

  checkPath(path, exists, is_dir);

  if ( exists )
  {
    if ( is_dir )
      return path;

    return getParentPath(path);
  }

  auto parent_path = getParentPath(path);

  if ( ! parent_path.empty() )
  {
    checkPath(parent_path, exists, is_dir);
    return ( exists && is_dir ) ? parent_path : "/";
  }

  return "/";
}

//----------------------------------------------------------------------
std::string getFilenameComponent (const std::string& path)
{
  const auto last_sep = path.find_last_of('/');

  if ( last_sep == std::string::npos )
    return path;

  return path.substr(last_sep + 1);
}

//----------------------------------------------------------------------
auto getFilename ( const std::string& path
                 , FFileDialog::DialogType type) -> std::string
{
  if ( type == FFileDialog::DialogType::Save )
    return getFilenameComponent(path);

  bool exists = false;
  bool is_dir = false;

  checkPath(path, exists, is_dir);

  if ( exists && ! is_dir )
    return getFilenameComponent(path);

  return {};
}

//----------------------------------------------------------------------
void setAndSelectFilename ( FFileDialog& file_dialog
                          , const FString& default_name )
{
  if ( file_dialog.getDialogType() != FFileDialog::DialogType::Save
    || default_name.isEmpty() )
    return;

  // Set and select the file name
  file_dialog.setFilename(default_name);
  file_dialog.selectDirectoryEntry(default_name.toString());
}

//----------------------------------------------------------------------
auto fileChooser ( FWidget* parent
                 , const FString& dir_nane
                 , const FString& filter
                 , FFileDialog::DialogType type ) -> FString
{
  auto full_path = dir_nane.toString();
  FString path = getWorkingDirectory(full_path);
  const FString& default_name = getFilename(full_path, type);

  if ( dir_nane.isEmpty() )
  {
    path.setString(FFileDialog::getHomeDir());

    if ( path.isEmpty() )
      path.setString("/");
  }

  auto file_filter = filter.isEmpty() ? FString("*") : filter;

  // Create a new FFileDialog object
  FFileDialog file_dialog ( path
                          , std::move(file_filter)
                          , type
                          , parent );
  setAndSelectFilename (file_dialog, default_name);

  if ( file_dialog.exec() != FDialog::ResultCode::Accept )
    return {};  // Cancelled: no error message

  auto name = file_dialog.file_name.getText().trim();

  if ( name.isEmpty() )
  {
    name = file_dialog.getSelectedFilename();

    if ( name.isEmpty() )
      return {};
  }

  const auto chosen_file = file_dialog.getAbsolutePath(name);
  const auto chosen_str = chosen_file.toString();

  if ( type == FFileDialog::DialogType::Save )
  {
    return chosen_file;
  }

  if ( type == FFileDialog::DialogType::Open )
  {
    if ( isFileAccessible(chosen_str) )
      return chosen_file;

    return {};
  }

  if ( ! isRegularFile(chosen_str) )
  {
    FMessageBox::error (&file_dialog, chosen_file + "\nis not a file");
    return {};
  }

  return chosen_file;
}


//----------------------------------------------------------------------
// class FFileDialog
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FFileDialog::FFileDialog (FWidget* parent)
  : FDialog{parent}
{
  init();
}

//----------------------------------------------------------------------
FFileDialog::FFileDialog ( const FString& dir_name
                         , FString&& filter
                         , DialogType type
                         , FWidget* parent )
  : FDialog{parent}
  , filter_pattern{std::move(filter)}
  , dlg_type{type}
{
  if ( ! dir_name.isEmpty() )
    setPath(dir_name);

  init();
}

//----------------------------------------------------------------------
FFileDialog::~FFileDialog() noexcept = default;  // destructor


// public methods of FFileDialog
//----------------------------------------------------------------------
auto FFileDialog::getSelectedFile() const -> FString
{
  return {getSelectedFilename()};
}

//----------------------------------------------------------------------
void FFileDialog::setFilename (const FString& name)
{
  file_name.setText(name);
}

//----------------------------------------------------------------------
void FFileDialog::setPath (const FString& dir)
{
  const auto& dir_name = dir.c_str();
  std::array<char, MAXPATHLEN> resolved_path{};
  FString r_dir{};
  struct stat sb{};

  if ( ::stat(dir_name, &sb) != 0 )
  {
    directory = '/';
    return;
  }

  if ( S_ISLNK(sb.st_mode) && ::lstat(dir_name, &sb) != 0 )
  {
    directory = '/';
    return;
  }

  if ( ! S_ISDIR(sb.st_mode) )
  {
    directory = '/';
    return;
  }

  const auto& fsystem = FSystem::getInstance();

  if ( fsystem->realpath(dir.c_str(), resolved_path.data()) != nullptr )
    r_dir.setString(resolved_path.data());
  else
    r_dir.setString(dir);

  if ( r_dir[r_dir.getLength() - 1] != '/' )
    directory = r_dir + "/";
  else
    directory = std::move(r_dir);
}

//----------------------------------------------------------------------
void FFileDialog::setFilter (const FString& filter)
{
  filter_pattern = filter;
  setTitelbarText();
}

//----------------------------------------------------------------------
void FFileDialog::setShowHiddenFiles (bool state)
{
  if ( show_hidden == state )
    return;

  show_hidden = state;
  readDir();
  file_browser.redraw();
}

//----------------------------------------------------------------------
void FFileDialog::onKeyPress (FKeyEvent* ev)
{
  if ( ! isEnabled() )
    return;

  FDialog::onKeyPress (ev);

  if ( ! file_browser.hasFocus() )
    return;

  const auto& key = ev->key();

  if ( key == FKey::Erase || key == FKey::Backspace )
  {
    changeDir("..");
    ev->accept();
  }
}

//----------------------------------------------------------------------
auto FFileDialog::fileOpenChooser ( FWidget* parent
                                  , const FString& dir_name
                                  , const FString& filter ) -> FString
{
  return fileChooser (parent, dir_name, filter, DialogType::Open);
}

//----------------------------------------------------------------------
auto FFileDialog::fileSaveChooser ( FWidget* parent
                                  , const FString& dir_name
                                  , const FString& filter ) -> FString
{
  return fileChooser (parent, dir_name, filter, DialogType::Save);
}


// protected methods of FFileDialog
//----------------------------------------------------------------------
void FFileDialog::adjustSize()
{
  const auto& root_widget = getRootWidget();
  // Set maximum values with fallback to xterm default size
  auto max_width = root_widget ? root_widget->getClientWidth() : 80;
  auto max_height = root_widget ? root_widget->getClientHeight() : 24;
  std::size_t h = max_height - 6;
  h = std::max(h, std::size_t(15));  // minimum 15
  h = std::min(h, std::size_t(30));  // maximum 30
  setHeight (h, false);
  const int X = 1 + int((max_width - getWidth()) / 2);
  const int Y = 1 + int((max_height - getHeight()) / 3);
  setPos(FPoint{X, Y}, false);
  file_browser.setHeight (h - 8, false);
  hidden_check.setY (int(h) - 4, false);
  cancel_btn.setY (int(h) - 4, false);
  open_btn.setY (int(h) - 4, false);
  FDialog::adjustSize();
  printPath(directory);
}


// private methods of FFileDialog
//----------------------------------------------------------------------
void FFileDialog::init()
{
  static constexpr std::size_t w = 42;
  static constexpr std::size_t h = 15;
  FWindow::setGeometry(FPoint{1, 1}, FSize{w, h}, false);
  const auto& parent_widget = getParentWidget();
  int x = parent_widget ? 1 + int((parent_widget->getWidth() - w) / 2) : 1;
  int y = parent_widget ? 1 + int((parent_widget->getHeight() - h) / 3) : 1;
  setTitelbarText();
  widgetSettings (FPoint{x, y});  // Create widgets
  initCallbacks();
  setModal();
  readDir();
}

//----------------------------------------------------------------------
inline void FFileDialog::widgetSettings (const FPoint& pos)
{
  if ( dlg_type != DialogType::Save )
    file_name.setText (filter_pattern);

  file_name.setLabelText ("File&name");
  file_name.setGeometry (FPoint{11, 1}, FSize{28, 1});
  file_name.setFocus();

  file_browser.setGeometry (FPoint{2, 3}, FSize{38, 6});
  printPath (directory);

  hidden_check.setText ("&hidden files");
  hidden_check.setGeometry (FPoint{2, 10}, FSize{16, 1});

  cancel_btn.setText ("&Cancel");
  cancel_btn.setGeometry(FPoint{19, 10}, FSize{9, 1});

  if ( dlg_type == DialogType::Save )
    open_btn.setText ("&Save");
  else
    open_btn.setText ("&Open");

  open_btn.setGeometry(FPoint{30, 10}, FSize{9, 1});
  FWindow::setGeometry (pos, getSize());
}

//----------------------------------------------------------------------
void FFileDialog::initCallbacks()
{
  file_name.addCallback
  (
    "activate",
    this, &FFileDialog::cb_processActivate
  );

  file_browser.addCallback
  (
    "row-changed",
    this, &FFileDialog::cb_processRowChanged
  );

  file_browser.addCallback
  (
    "clicked",
    this, &FFileDialog::cb_processClicked
  );

  hidden_check.addCallback
  (
    "toggled",
    this, &FFileDialog::cb_processShowHidden
  );

  cancel_btn.addCallback
  (
    "clicked",
    this, &FFileDialog::cb_processCancel
  );

  open_btn.addCallback
  (
    "clicked",
    this, &FFileDialog::cb_processOpen
  );
}

//----------------------------------------------------------------------
inline auto FFileDialog::patternMatch ( const std::string& pattern
                                      , const std::string& fname ) const -> bool
{
  std::string search{};
  search.reserve(128);

  if ( show_hidden && fname[0] == '.' && fname[1] != '\0' )  // hidden files
  {
    search = ".";
    search.append(pattern);
  }
  else
    search = pattern;

  return ( fnmatch(search.data(), fname.data(), FNM_PERIOD) == 0 );
}

//----------------------------------------------------------------------
void FFileDialog::clear()
{
  if ( dir_entries.empty() )
    return;

  dir_entries.clear();
  dir_entries.shrink_to_fit();
}

//----------------------------------------------------------------------
auto FFileDialog::numOfDirs() -> sInt64
{
  if ( dir_entries.empty() )
    return 0;

  const sInt64 n = std::count_if ( std::begin(dir_entries)
                                 , std::end(dir_entries)
                                 , [] (const auto& entry)
                                   {
                                     return entry.directory
                                         && entry.name != ".";
                                   }
                                 );
  return n;
}

//----------------------------------------------------------------------
void FFileDialog::sortDir()
{
  const sInt64 dir_num = numOfDirs();

  // Directories first
  std::partition ( dir_entries.begin()
                 , dir_entries.end()
                 , sortDirFirst );

  // Sort directories by name
  std::sort ( dir_entries.begin()
            , dir_entries.begin() + dir_num
            , sortDirByName );

  // Sort files by name
  std::sort ( dir_entries.begin() + dir_num
            , dir_entries.end()
            , sortFileByName );
}

//----------------------------------------------------------------------
auto FFileDialog::readDir() -> int
{
  auto directory_stream = openDirectory();

  if ( ! directory_stream )
    return -1;

  clear();
  readDirEntries (directory_stream);

  if ( closeDirectory (directory_stream) == CloseDir::error )
    return -2;

  sortDir();

  // Insert directory entries into the list
  dirEntriesToList();
  return 0;
}

//----------------------------------------------------------------------
void FFileDialog::getEntry (const std::string& dir, const struct dirent* d_entry)
{
  const auto& filter = filter_pattern.toString();
  FDirEntry entry{};

  entry.name = d_entry->d_name;

#if defined _DIRENT_HAVE_D_TYPE || defined HAVE_STRUCT_DIRENT_D_TYPE
  entry.fifo             = (d_entry->d_type & DT_FIFO) == DT_FIFO;
  entry.character_device = (d_entry->d_type & DT_CHR ) == DT_CHR;
  entry.directory        = (d_entry->d_type & DT_DIR ) == DT_DIR;
  entry.block_device     = (d_entry->d_type & DT_BLK ) == DT_BLK;
  entry.regular_file     = (d_entry->d_type & DT_REG ) == DT_REG;
  entry.symbolic_link    = (d_entry->d_type & DT_LNK ) == DT_LNK;
  entry.socket           = (d_entry->d_type & DT_SOCK) == DT_SOCK;
#else
  struct stat s{};
  ::stat (d_entry->d_name, &s);
  entry.fifo             = S_ISFIFO (s.st_mode);
  entry.character_device = S_ISCHR (s.st_mode);
  entry.directory        = S_ISDIR (s.st_mode);
  entry.block_device     = S_ISBLK (s.st_mode);
  entry.regular_file     = S_ISREG (s.st_mode);
  entry.symbolic_link    = S_ISLNK (s.st_mode);
  entry.socket           = S_ISSOCK (s.st_mode);
#endif

  followSymLink (dir, entry);

  if ( entry.directory || patternMatch(filter, entry.name) )
    dir_entries.push_back (std::move(entry));
  else
    entry.name.clear();
}

//----------------------------------------------------------------------
void FFileDialog::followSymLink (const std::string& dir, FDirEntry& entry) const
{
  if ( ! entry.symbolic_link )
    return;  // No symbolic link

  std::array<char, MAXPATHLEN> resolved_path{};
  std::string symLink{};
  symLink.reserve(MAXPATHLEN);
  struct stat sb{};
  const auto& fsystem = FSystem::getInstance();
  symLink = dir + entry.name;

  if ( fsystem->realpath(symLink.data(), resolved_path.data()) == nullptr )
    return;  // Cannot follow the symlink

  if ( ::lstat(resolved_path.data(), &sb) == -1 )
    return;  // Cannot get file status

  entry.directory = S_ISDIR(sb.st_mode);
}

//----------------------------------------------------------------------
auto FFileDialog::openDirectory() -> DIR*
{
  const auto& dir = directory.c_str();
  auto directory_stream = opendir(dir);

  if ( ! directory_stream )
  {
    FMessageBox::error (this, "Can't open directory\n" + directory);
  }

  return directory_stream;
}

//----------------------------------------------------------------------
auto FFileDialog::closeDirectory (DIR* directory_stream) -> CloseDir
{
  auto ret = closedir(directory_stream);

  if ( ret )
  {
    FMessageBox::error (this, "Closing directory\n" + directory);
    return CloseDir::error;
  }

  return CloseDir::success;
}

//----------------------------------------------------------------------
void FFileDialog::readDirEntries (DIR* directory_stream)
{
  const auto& dir_name = directory.toString();

  while ( true )
  {
    errno = 0;
    const struct dirent* next = readdir(directory_stream);

    if ( next )
    {
      if ( isCurrentDirectory(next) )
        continue;  // Skip name = "."

      if ( ! show_hidden && isHiddenEntry(next) )
        continue;  // Skip hidden entries

      if ( isRootDirectory(dir_name) && isParentDirectory(next) )
        continue;  // Skip ".." for the root directory

      getEntry(dir_name, next);
    }
    else
    {
      if ( errno != 0 )
        FMessageBox::error (this, "Reading directory\n" + directory);

      break;
    }
  }  // end while
}

//----------------------------------------------------------------------
auto FFileDialog::getSelectedFilename() const -> std::string
{
  // In save mode the typed name counts, not the highlighted list item

  const auto item = file_browser.currentItem();

  if ( item == 0 || item > dir_entries.size() )
    return {};

  const auto n = uLong(item - 1);

  if ( dir_entries[n].directory )
    return {""};

  return dir_entries[n].name;
}

//----------------------------------------------------------------------
auto FFileDialog::interpretInputType() const -> InputType
{
  const auto input = file_name.getText().trim();

  if ( input.isEmpty() )
    return InputType::Undefined;

  if ( isFilterInput() )
    return InputType::Filter;

  if ( isDirectoryInput() )
  {
    FString dir_name{getAbsolutePath(input)};

    if ( isDirectory(dir_name.toString()) )
      return InputType::Directory;

    if ( isFileAccessible(dir_name.toString()) )
      return InputType::File;
  }

  const auto name = input.toString();
  const auto iter = \
      std::find_if ( std::begin(dir_entries)
                   , std::end(dir_entries)
                   , [&name] (const auto& entry)
                     {
                       return ! entry.name.empty()
                           && ! name.empty()
                           && entry.name == name;
                     }
                   );

  if ( iter == std::end(dir_entries) )
    return InputType::Undefined;

  if ( iter->directory )
    return InputType::Directory;

  return InputType::File;
}

//----------------------------------------------------------------------
auto FFileDialog::isCurrentDirectory (const struct dirent* entry) const -> bool
{
  // name = "." (current directory)
  return entry->d_name[0] == '.'
      && entry->d_name[1] == '\0';
}

//----------------------------------------------------------------------
auto FFileDialog::isParentDirectory (const struct dirent* entry) const -> bool
{
  // name = ".." (parent directory)
  return entry->d_name[0] == '.'
      && entry->d_name[1] == '.'
      && entry->d_name[3] == '\0';
}

//----------------------------------------------------------------------
auto FFileDialog::isHiddenEntry (const struct dirent* entry) const -> bool
{
  // name = "." + one or more character
  return entry->d_name[0] == '.'
      && entry->d_name[1] != '\0'
      && entry->d_name[1] != '.';
}

//----------------------------------------------------------------------
auto FFileDialog::isRootDirectory (const std::string& dir) const -> bool
{
  return dir.size() > 0
      && dir[0] == '/'
      && dir[1] == '\0';
}

//----------------------------------------------------------------------
void FFileDialog::dirEntriesToList()
{
  // Fill list with directory entries

  file_browser.clear();

  if ( dir_entries.empty() )
    return;

  std::for_each ( dir_entries.cbegin()
                , dir_entries.cend()
                , [this] (const auto& entry)
                  {
                    if ( entry.directory )
                      file_browser.insert(FString{entry.name}, BracketType::Brackets);
                    else
                      file_browser.insert(FString{entry.name});
                  }
                );
}

//----------------------------------------------------------------------
void FFileDialog::selectDirectoryEntry (const std::string& name)
{
  if ( dir_entries.empty() )
    return;

  std::size_t i{1};

  for (const auto& entry : dir_entries)
  {
    if ( entry.name == name )
    {
      file_browser.setCurrentItem(i);

      if ( dlg_type == DialogType::Save )
        break;

      if ( entry.directory )
        file_name.setText(name + '/');
      else
        file_name.setText(name);

      break;
    }

    i++;
  }
}

//----------------------------------------------------------------------
auto FFileDialog::getAbsolutePath (const FString& dir_name) const -> FString
{
  FString newdir{dir_name};

  // Path Resolution
  if ( newdir.includes('~') )
    newdir = newdir.replace('~', getHomeDir());

  if ( newdir[0] == '/' )
    return newdir;
  else
    return directory + newdir;
}

//----------------------------------------------------------------------
void FFileDialog::handleParentDirectoryChange ( const FString& lastdir
                                              , bool is_non_save_dlg )
{
  if ( lastdir == FString{'/'} )
  {
    if ( is_non_save_dlg )
      file_name.setText('/');

    return;
  }

  auto base_name = getFilenameComponent(lastdir.toString());
  selectDirectoryEntry(base_name);
}

//----------------------------------------------------------------------
void FFileDialog::handleDefaultDirectorySelection()
{
  if ( dir_entries.empty() )
    return;

  FString firstname{dir_entries[0].name};

  if ( dir_entries[0].directory )
    file_name.setText(firstname + '/');
  else
    file_name.setText(firstname);
}

//----------------------------------------------------------------------
auto FFileDialog::changeDir (const FString& dir_name) -> int
{
  const auto is_non_save_dlg = dlg_type != DialogType::Save;
  const auto lastdir = directory;
  const auto newdir = getAbsolutePath(dir_name);
  setPath(newdir);

  switch ( readDir() )  // Read the directory and handle errors
  {
    case -1:
      setPath(lastdir);
      return -1;

    case -2:
      setPath(lastdir);
      readDir();
      return -2;

    case 0:
      if ( newdir == FString{".."} )
      {
        handleParentDirectoryChange (lastdir, is_non_save_dlg);
      }
      else if ( is_non_save_dlg )
      {
        handleDefaultDirectorySelection();
      }

      printPath(directory);
      file_name.redraw();
      file_browser.redraw();
      // fall through
    default:
      return 0;
  }
}

//----------------------------------------------------------------------
void FFileDialog::printPath (const FString& txt)
{
  const auto& path = txt;
  const std::size_t max_width = file_browser.getWidth() - 4;
  const auto column_width = getColumnWidth(path);

  if ( column_width > max_width )
  {
    const std::size_t width = max_width - 2;
    const std::size_t first = column_width + 1 - width;
    const FString sub_str{getColumnSubString (path, first, width)};
    file_browser.setText(".." + sub_str);
  }
  else
    file_browser.setText(path);
}

//----------------------------------------------------------------------
void FFileDialog::setTitelbarText()
{
  const auto suffix = [this] ()
  {
    if ( ! filter_pattern.isEmpty() && filter_pattern != L"*" )
      return FString(L" (") + filter_pattern + L")";

    return FString{};
  };

  if ( dlg_type == DialogType::Save )
    FDialog::setText("Save file" + suffix());
  else
    FDialog::setText("Open file" + suffix());
}

//----------------------------------------------------------------------
auto FFileDialog::getHomeDir() -> FString
{
  struct passwd pwd{};
  struct passwd* pwd_ptr{};
  std::array<char, 1024> buf{};

  const auto& fsystem = FSystem::getInstance();
  const uid_t euid = fsystem->geteuid();

  if ( fsystem->getpwuid_r(euid, &pwd, buf.data(), buf.size(), &pwd_ptr) )
    return {""};

  return {pwd.pw_dir};
}

//----------------------------------------------------------------------
inline auto FFileDialog::isFilterInput() const -> bool
{
  return file_name.getText().includes('*')
      || file_name.getText().includes('?');
}

//----------------------------------------------------------------------
inline auto FFileDialog::isDirectoryInput() const -> bool
{
  return file_name.getText().trim() == FString{".."}
      || file_name.getText().includes('/')
      || file_name.getText().includes('~');
}

//----------------------------------------------------------------------
inline void FFileDialog::activateNewFilter()
{
  setFilter(file_name.getText());
  redraw();  // Show new filter in title bar
  readDir();
  file_browser.redraw();
}

//----------------------------------------------------------------------
inline void FFileDialog::activateDefaultFilter()
{
  setFilter("*");
  redraw();  // Delete filter from title bar
  readDir();
  file_browser.redraw();
}

//----------------------------------------------------------------------
inline void FFileDialog::findItem (const FString& search_text)
{
  auto iter = file_browser.findItem(search_text);

  if ( iter != file_browser.getData().end() )
  {
    file_browser.setCurrentItem(iter);
    done (ResultCode::Accept);
  }
  else
    done (ResultCode::Reject);
}

//----------------------------------------------------------------------
inline void FFileDialog::changeIntoSubDir()
{
  const auto& input = file_name.getText().trim();

  if ( dir_entries.empty() )
    done (ResultCode::Reject);

  const auto name = input.toString();
  const auto found = \
      std::any_of ( std::begin(dir_entries)
                  , std::end(dir_entries)
                  , [&name] (auto& entry)
                    {
                      return ! entry.name.empty()
                          && ! name.empty()
                          && entry.name == name
                          && entry.directory;
                    }
                  );

  if ( found )
  {
    changeDir(input);
    return;
  }
}

//----------------------------------------------------------------------
auto FFileDialog::confirmOverwrite (const std::string& name) -> FMessageBox::ButtonType
{
  FMessageBox mbox ( "Replace File"
                   , "Do you want to replace it?"
                   , FMessageBox::ButtonType::Yes
                   , FMessageBox::ButtonType::No
                   , FMessageBox::ButtonType::Cancel
                   , this );
  mbox.setHeadline("\"" + FString{name} + "\" " + "already exists.");
  mbox.setCenterText();
  return mbox.exec();
}

//----------------------------------------------------------------------
void FFileDialog::processSaveSelection()
{
  const auto name = file_name.getText().trim().toString();
  const auto path = getAbsolutePath(name);

  if ( ! isFileAccessible(path.toString()) )
  {
    // New non-existent file
    done (ResultCode::Accept);
    return;
  }

  // The file already exists
  const auto reply = confirmOverwrite(name);

  if ( reply == FMessageBox::ButtonType::Yes )
    done (ResultCode::Accept);
  else if ( reply == FMessageBox::ButtonType::Cancel )
    done (ResultCode::Reject);
}

//----------------------------------------------------------------------
void FFileDialog::cb_processActivate()
{
  const auto input_type = interpretInputType();

  if ( input_type == InputType::Filter )
  {
    activateNewFilter();
    return;
  }

  if ( file_name.getText().isEmpty() )
  {
    activateDefaultFilter();
    return;
  }

  if ( input_type == InputType::Directory )
  {
    if ( isDirectoryInput() )
    {
      changeDir(file_name.getText().trim());
      return;
    }

    changeIntoSubDir();
    return;
  }

  if ( dlg_type == DialogType::Save )
  {
    processSaveSelection();
    return;
  }

  if ( input_type == InputType::File )
  {
    const auto input = file_name.getText().trim();

    if ( isDirectoryInput() && isFileAccessible(input.toString()) )
    {
      done (ResultCode::Accept);
      return;
    }

    findItem(input);
  }
}

//----------------------------------------------------------------------
void FFileDialog::cb_processRowChanged()
{
  const std::size_t n = file_browser.currentItem();

  if ( n == 0 )
    return;

  const auto& name = FString{dir_entries[n - 1].name};

  if ( dir_entries[n - 1].directory )
  {
    if ( dlg_type != DialogType::Save )
      file_name.setText(name + '/');
  }
  else
    file_name.setText(name);

  file_name.redraw();
}

//----------------------------------------------------------------------
void FFileDialog::cb_processClicked()
{
  const auto n = uLong(file_browser.currentItem() - 1);

  if ( dir_entries[n].directory )
  {
    changeDir(dir_entries[n].name);
    return;
  }

  if ( dlg_type != DialogType::Save )
  {
    done (ResultCode::Accept);
    return;
  }

  processSaveSelection();
}

//----------------------------------------------------------------------
void FFileDialog::cb_processCancel()
{
  done (ResultCode::Reject);
}

//----------------------------------------------------------------------
void FFileDialog::cb_processOpen()
{
  if ( dlg_type != DialogType::Save )
  {
    done (ResultCode::Accept);
    return;
  }

  processSaveSelection();
}

//----------------------------------------------------------------------
void FFileDialog::cb_processShowHidden()
{
  setShowHiddenFiles(! show_hidden);
}

}  // namespace finalcut
