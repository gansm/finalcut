/***********************************************************************
* ffiledialog.cpp - Widget FFileDialog (a file chooser dialog)         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2024 Markus Gans                                      *
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

#include <array>
#include <utility>
#include <vector>

#if defined(__CYGWIN__)
  #define _BSD_SOURCE  // for D_TYPE in readdir
#endif

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
auto sortByName ( const FFileDialog::FDirEntry& lhs
                , const FFileDialog::FDirEntry& rhs ) -> bool
{
  // lhs < rhs
  return FStringCaseCompare(lhs.name, rhs.name) < 0;
}

//----------------------------------------------------------------------
auto sortDirFirst (const FFileDialog::FDirEntry& entry) -> bool
{
  // sort directories first
  return entry.directory;
}

//----------------------------------------------------------------------
auto fileChooser ( FWidget* parent
                 , const FString& dirname
                 , const FString& filter
                 , FFileDialog::DialogType type ) -> FString
{
  FString ret{};
  FString path{dirname};
  FString file_filter{filter};

  if ( path.isEmpty() )
  {
    path.setString(FFileDialog::getHomeDir());

    if ( path.isEmpty() )
      path.setString("/");
  }

  if ( file_filter.isEmpty() )
    file_filter.setString("*");

  FFileDialog fileopen ( path
                       , std::move(file_filter)
                       , type
                       , parent );

  if ( fileopen.exec() == FDialog::ResultCode::Accept )
    ret = fileopen.getPath() + fileopen.getSelectedFile();
  else
    ret = FString{};

  return ret;
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
FFileDialog::FFileDialog ( const FString& dirname
                         , FString&& filter
                         , DialogType type
                         , FWidget* parent )
  : FDialog{parent}
  , filter_pattern{std::move(filter)}
  , dlg_type{type}
{
  if ( ! dirname.isEmpty() )
    setPath(dirname);

  init();
}

//----------------------------------------------------------------------
FFileDialog::~FFileDialog() noexcept = default;  // destructor


// public methods of FFileDialog
//----------------------------------------------------------------------
auto FFileDialog::getSelectedFile() const -> FString
{
  const auto n = uLong(filebrowser.currentItem() - 1);

  if ( dir_entries[n].directory )
    return {""};

  return {dir_entries[n].name};
}

//----------------------------------------------------------------------
void FFileDialog::setPath (const FString& dir)
{
  const auto& dirname = dir.c_str();
  std::array<char, MAXPATHLEN> resolved_path{};
  FString r_dir{};
  struct stat sb{};

  if ( stat(dirname, &sb) != 0 )
  {
    directory = '/';
    return;
  }

  if ( S_ISLNK(sb.st_mode) && lstat(dirname, &sb) != 0 )
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
void FFileDialog::setShowHiddenFiles (bool enable)
{
  if ( show_hidden == enable )
    return;

  show_hidden = enable;
  readDir();
  filebrowser.redraw();
}

//----------------------------------------------------------------------
void FFileDialog::onKeyPress (FKeyEvent* ev)
{
  if ( ! isEnabled() )
    return;

  FDialog::onKeyPress (ev);

  if ( ! filebrowser.hasFocus() )
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
                                  , const FString& dirname
                                  , const FString& filter ) -> FString
{
  return fileChooser (parent, dirname, filter, DialogType::Open);
}

//----------------------------------------------------------------------
auto FFileDialog::fileSaveChooser ( FWidget* parent
                                  , const FString& dirname
                                  , const FString& filter ) -> FString
{
  return fileChooser (parent, dirname, filter, DialogType::Save);
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
  filebrowser.setHeight (h - 8, false);
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
  filename.setLabelText ("File&name");
  filename.setText (filter_pattern);
  filename.setGeometry (FPoint{11, 1}, FSize{28, 1});
  filename.setFocus();

  filebrowser.setGeometry (FPoint{2, 3}, FSize{38, 6});
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
  filename.addCallback
  (
    "activate",
    this, &FFileDialog::cb_processActivate
  );

  filebrowser.addCallback
  (
    "row-changed",
    this, &FFileDialog::cb_processRowChanged
  );

  filebrowser.addCallback
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
  const sInt64 start = dir_entries.cbegin()->name == ".." ? 1 : 0;
  const sInt64 dir_num = numOfDirs();
  // directories first
  std::partition ( dir_entries.begin() + start
                 , dir_entries.end()
                 , sortDirFirst );
  // sort directories by name
  std::sort ( dir_entries.begin() + start
            , dir_entries.begin() + dir_num
            , sortByName );
  // sort files by name
  std::sort ( dir_entries.begin() + dir_num
            , dir_entries.end()
            , sortByName );
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
void FFileDialog::getEntry (const char* const dir, const struct dirent* d_entry)
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
  stat (entry.name.c_str(), &s);
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
    dir_entries.push_back (entry);
  else
    entry.name.clear();
}

//----------------------------------------------------------------------
void FFileDialog::followSymLink (const char* const dir, FDirEntry& entry) const
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

  if ( lstat(resolved_path.data(), &sb) == -1 )
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
  const auto& dir = directory.c_str();

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

      if ( isRootDirectory(dir) && isParentDirectory(next) )
        continue;  // Skip ".." for the root directory

      getEntry(dir, next);
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
auto FFileDialog::isRootDirectory (const char* const dir) const -> bool
{
  return dir[0] == '/'
      && dir[1] == '\0';
}

//----------------------------------------------------------------------
void FFileDialog::dirEntriesToList()
{
  // Fill list with directory entries

  filebrowser.clear();

  if ( dir_entries.empty() )
    return;

  std::for_each ( dir_entries.cbegin()
                , dir_entries.cend()
                , [this] (const auto& entry)
                  {
                    if ( entry.directory )
                      filebrowser.insert(FString{entry.name}, BracketType::Brackets);
                    else
                      filebrowser.insert(FString{entry.name});
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
      filebrowser.setCurrentItem(i);
      filename.setText(name + '/');
      break;
    }

    i++;
  }
}

//----------------------------------------------------------------------
auto FFileDialog::changeDir (const FString& dirname) -> int
{
  FString lastdir{directory};
  FString newdir{dirname};

  if ( newdir.includes('~') )
    newdir = newdir.replace('~', getHomeDir());

  if ( newdir[0] == '/' )
    setPath(newdir);
  else
    setPath(directory + newdir);

  switch ( readDir() )
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
        if ( lastdir == FString{'/'} )
          filename.setText('/');
        else
        {
          auto baseName = std::string(basename(lastdir.c_str()));
          selectDirectoryEntry (baseName);
        }
      }
      else
      {
        FString firstname{dir_entries[0].name};

        if ( dir_entries[0].directory )
          filename.setText(firstname + '/');
        else
          filename.setText(firstname);
      }

      printPath(directory);
      filename.redraw();
      filebrowser.redraw();
      // fall through
    default:
      return 0;
  }
}

//----------------------------------------------------------------------
void FFileDialog::printPath (const FString& txt)
{
  const auto& path = txt;
  const std::size_t max_width = filebrowser.getWidth() - 4;
  const auto column_width = getColumnWidth(path);

  if ( column_width > max_width )
  {
    const std::size_t width = max_width - 2;
    const std::size_t first = column_width + 1 - width;
    const FString sub_str{getColumnSubString (path, first, width)};
    filebrowser.setText(".." + sub_str);
  }
  else
    filebrowser.setText(path);
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
  return filename.getText().includes('*')
      || filename.getText().includes('?');
}

//----------------------------------------------------------------------
inline auto FFileDialog::isDirectoryInput() const -> bool
{
  return filename.getText().trim() == FString{".."}
      || filename.getText().includes('/')
      || filename.getText().includes('~');
}

//----------------------------------------------------------------------
inline void FFileDialog::activateNewFilter()
{
  setFilter(filename.getText());
  redraw();  // Show new filter in title bar
  readDir();
  filebrowser.redraw();
}

//----------------------------------------------------------------------
inline void FFileDialog::activateDefaultFilter()
{
  setFilter("*");
  redraw();  // Delete filter from title bar
  readDir();
  filebrowser.redraw();
}

//----------------------------------------------------------------------
inline void FFileDialog::findItem (const FString& search_text)
{
  auto iter = filebrowser.findItem(search_text);

  if ( iter != filebrowser.getData().end() )
  {
    filebrowser.setCurrentItem(iter);
    done (ResultCode::Accept);
  }
  else
    done (ResultCode::Reject);
}

//----------------------------------------------------------------------
inline void FFileDialog::changeIntoSubDir()
{
  bool found{false};
  const auto& input = filename.getText().trim();

  if ( dir_entries.empty() )
    done (ResultCode::Reject);

  found = std::any_of ( std::begin(dir_entries)
                      , std::end(dir_entries)
                      , [&input] (auto& entry)
                        {
                          return ! entry.name.empty()
                              && input
                              && ! input.isEmpty()
                              && entry.name == input.toString()
                              && entry.directory;
                        }
                      );

  if ( found )
  {
    changeDir(input);
    return;
  }

  findItem(input);
}

//----------------------------------------------------------------------
void FFileDialog::cb_processActivate()
{
  if ( isFilterInput() )
    activateNewFilter();
  else if ( filename.getText().isEmpty() )
    activateDefaultFilter();
  else if ( isDirectoryInput() )
    changeDir(filename.getText().trim());
  else
    changeIntoSubDir();
}

//----------------------------------------------------------------------
void FFileDialog::cb_processRowChanged()
{
  const std::size_t n = filebrowser.currentItem();

  if ( n == 0 )
    return;

  const auto& name = FString{dir_entries[n - 1].name};

  if ( dir_entries[n - 1].directory )
    filename.setText(name + '/');
  else
    filename.setText(name);

  filename.redraw();
}

//----------------------------------------------------------------------
void FFileDialog::cb_processClicked()
{
  const auto n = uLong(filebrowser.currentItem() - 1);

  if ( dir_entries[n].directory )
    changeDir(dir_entries[n].name);
  else
    done (ResultCode::Accept);
}

//----------------------------------------------------------------------
void FFileDialog::cb_processCancel()
{
  done (ResultCode::Reject);
}

//----------------------------------------------------------------------
void FFileDialog::cb_processOpen()
{
  done (ResultCode::Accept);
}

//----------------------------------------------------------------------
void FFileDialog::cb_processShowHidden()
{
  setShowHiddenFiles(! show_hidden);
}

}  // namespace finalcut
