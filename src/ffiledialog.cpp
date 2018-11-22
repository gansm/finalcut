/***********************************************************************
* ffiledialog.cpp - Widget FFileDialog (a file chooser dialog)         *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2014-2018 Markus Gans                                      *
*                                                                      *
* The Final Cut is free software; you can redistribute it and/or       *
* modify it under the terms of the GNU Lesser General Public License   *
* as published by the Free Software Foundation; either version 3 of    *
* the License, or (at your option) any later version.                  *
*                                                                      *
* The Final Cut is distributed in the hope that it will be useful,     *
* but WITHOUT ANY WARRANTY; without even the implied warranty of       *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU Lesser General Public License for more details.                  *
*                                                                      *
* You should have received a copy of the GNU Lesser General Public     *
* License along with this program.  If not, see                        *
* <http://www.gnu.org/licenses/>.                                      *
***********************************************************************/

#include <vector>

#include "final/ffiledialog.h"

namespace finalcut
{

// non-member functions
//----------------------------------------------------------------------
bool sortByName ( const FFileDialog::dir_entry& lhs
                , const FFileDialog::dir_entry& rhs )
{
  // lhs < rhs
  return bool( strcasecmp(lhs.name, rhs.name) < 0 );
}

//----------------------------------------------------------------------
bool sortDirFirst ( const FFileDialog::dir_entry& lhs
                  , const FFileDialog::dir_entry& rhs )
{
  // sort directories first
  if ( lhs.directory && ! rhs.directory )
    return true;
  else
    return false;
}


//----------------------------------------------------------------------
// class FFileDialog
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FFileDialog::FFileDialog (FWidget* parent)
  : FDialog(parent)
  , directory_stream(0)
  , dir_entries()
  , directory()
  , filter_pattern()
  , filename()
  , filebrowser()
  , hidden()
  , cancel()
  , open()
  , dlg_type(FFileDialog::Open)
  , show_hidden(false)
{
  init();
}

//----------------------------------------------------------------------
FFileDialog::FFileDialog (const FFileDialog& fdlg)
  : FDialog(fdlg.getParentWidget())
  , directory_stream(0)
  , dir_entries()
  , directory(fdlg.directory)
  , filter_pattern(fdlg.filter_pattern)
  , filename()
  , filebrowser()
  , hidden()
  , cancel()
  , open()
  , dlg_type(fdlg.dlg_type)
  , show_hidden(fdlg.show_hidden)
{
  if ( directory )
    setPath(directory);

  init();
}

//----------------------------------------------------------------------
FFileDialog::FFileDialog ( const FString& dirname
                         , const FString& filter
                         , DialogType type
                         , FWidget* parent )
  : FDialog(parent)
  , directory_stream(0)
  , dir_entries()
  , directory()
  , filter_pattern(filter)
  , filename(this)
  , filebrowser(this)
  , hidden(this)
  , cancel(this)
  , open(this)
  , dlg_type(type)
  , show_hidden(false)
{
  if ( ! dirname.isNull() )
    setPath(dirname);

  init();
}

//----------------------------------------------------------------------
FFileDialog::~FFileDialog()  // destructor
{
  clear();
}


// public methods of FFileDialog
//----------------------------------------------------------------------
FFileDialog& FFileDialog::operator = (const FFileDialog& fdlg)
{
  if ( &fdlg == this )
  {
    return *this;
  }
  else
  {
    clear();

    if ( fdlg.getParentWidget() )
      fdlg.getParentWidget()->addChild (this);

    directory = fdlg.directory;
    filter_pattern = fdlg.filter_pattern;
    dlg_type = fdlg.dlg_type;
    show_hidden = fdlg.show_hidden;

    if ( directory )
      setPath(directory);

    init();
    return *this;
  }
}

//----------------------------------------------------------------------
const FString FFileDialog::getSelectedFile() const
{
  uLong n = uLong(filebrowser.currentItem() - 1);

  if ( dir_entries[n].directory )
    return FString("");
  else
    return FString(dir_entries[n].name);
}

//----------------------------------------------------------------------
void FFileDialog::setPath (const FString& dir)
{
  const char* const dirname = dir.c_str();
  char resolved_path[MAXPATHLEN];
  FString r_dir;
  struct stat sb;

  if ( stat(dirname, &sb) != 0 )
  {
    directory = '/';
    return;
  }

  if ( S_ISLNK(sb.st_mode) )
  {
    if ( lstat(dirname, &sb) != 0 )
    {
      directory = '/';
      return;
    }
  }

  if ( ! S_ISDIR(sb.st_mode) )
  {
    directory = '/';
    return;
  }

  if ( realpath(dir.c_str(), resolved_path) != 0 )
    r_dir = resolved_path;
  else
    r_dir = dir;

  if ( r_dir[r_dir.getLength() - 1] != '/' )
    directory = r_dir + "/";
  else
    directory = r_dir;
}

//----------------------------------------------------------------------
void FFileDialog::setFilter (const FString& filter)
{
  filter_pattern = filter;
}

//----------------------------------------------------------------------
bool FFileDialog::setShowHiddenFiles (bool on)
{
  if ( on == show_hidden )
    return show_hidden;

  show_hidden = on;
  readDir();
  filebrowser.redraw();
  return show_hidden;
}

//----------------------------------------------------------------------
void FFileDialog::onKeyPress (FKeyEvent* ev)
{
  if ( ! isEnabled() )
    return;

  FDialog::onKeyPress (ev);

  if ( ! filebrowser.hasFocus() )
    return;

  FKey key = ev->key();

  switch ( key )
  {
    case fc::Fkey_erase:
    case fc::Fkey_backspace:
      changeDir("..");
      ev->accept();
      break;

    default:
      break;
  }
}

//----------------------------------------------------------------------
const FString FFileDialog::fileOpenChooser ( FWidget* parent
                                           , const FString& dirname
                                           , const FString& filter )
{
  FFileDialog* fileopen;
  FString ret;
  FString path = dirname;
  FString file_filter = filter;

  if ( path.isNull() || path.isEmpty() )
  {
    path = getHomeDir();

    if ( path.isNull() || path.isEmpty() )
      path = FString("/");
  }

  if ( file_filter.isNull() || file_filter.isEmpty() )
    file_filter = FString("*");

  try
  {
    fileopen = new FFileDialog ( path
                               , file_filter
                               , FFileDialog::Open
                               , parent );
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    return FString();
  }

  if ( fileopen->exec() == FDialog::Accept )
    ret = fileopen->getPath() + fileopen->getSelectedFile();
  else
    ret = FString();

  delete fileopen;
  return ret;
}

//----------------------------------------------------------------------
const FString FFileDialog::fileSaveChooser ( FWidget* parent
                                           , const FString& dirname
                                           , const FString& filter )
{
  FFileDialog* fileopen;
  FString ret;
  FString path = dirname;
  FString file_filter = filter;

  if ( path.isNull() || path.isEmpty() )
  {
    path = getHomeDir();

    if ( path.isNull() || path.isEmpty() )
      path = FString("/");
  }

  if ( file_filter.isNull() || file_filter.isEmpty() )
    file_filter = FString("*");

  try
  {
    fileopen = new FFileDialog ( path
                               , file_filter
                               , FFileDialog::Save
                               , parent );
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    return FString();
  }

  if ( fileopen->exec() == FDialog::Accept )
    ret = fileopen->getPath() + fileopen->getSelectedFile();
  else
    ret = FString();

  delete fileopen;
  return ret;
}


// protected methods of FFileDialog
//----------------------------------------------------------------------
void FFileDialog::adjustSize()
{
  int X, Y;
  std::size_t max_width;
  std::size_t max_height;
  std::size_t h;
  FWidget* root_widget = getRootWidget();

  if ( root_widget )
  {
    max_width = root_widget->getClientWidth();
    max_height = root_widget->getClientHeight();
  }
  else
  {
    // fallback to xterm default size
    max_width = 80;
    max_height = 24;
  }

  h = max_height - 6;

  if ( h < 15 )  // minimum
    h = 15;

  if ( h > 30 )  // maximum
    h = 30;

  setHeight (h, false);
  X = 1 + int((max_width - getWidth()) / 2);
  Y = 1 + int((max_height - getHeight()) / 3);
  setPos(X, Y, false);
  filebrowser.setHeight (h - 8, false);
  hidden.setY (int(h) - 4, false);
  cancel.setY (int(h) - 4, false);
  open.setY (int(h) - 4, false);
  FDialog::adjustSize();
  printPath(directory);
}


// private methods of FFileDialog
//----------------------------------------------------------------------
void FFileDialog::init()
{
  static const std::size_t w = 42;
  static const std::size_t h = 15;
  int x, y;
  FWidget* parent_widget;

  setGeometry(1, 1, w, h, false);
  parent_widget = getParentWidget();

  if ( parent_widget )
  {
    x = 1 + int((parent_widget->getWidth() - w) / 2);
    y = 1 + int((parent_widget->getHeight() - h) / 3);
  }
  else
    x = y = 1;

  if ( dlg_type == FFileDialog::Save )
    FDialog::setText("Save file");
  else
    FDialog::setText("Open file");

  widgetSettings (x, y);  // Create widgets
  initCallbacks();
  setModal();
  readDir();
}

//----------------------------------------------------------------------
inline void FFileDialog::widgetSettings (int x, int y)
{
  filename.setLabelText ("File&name");
  filename.setText (filter_pattern);
  filename.setGeometry (11, 1, 28, 1);
  filename.setFocus();

  filebrowser.setGeometry (2, 3, 38, 6);
  printPath (directory);

  hidden.setText ("&hidden files");
  hidden.setGeometry (2, 10, 16, 1);

  cancel.setText ("&Cancel");
  cancel.setGeometry(19, 10, 9, 1);

  if ( dlg_type == FFileDialog::Save )
    open.setText ("&Save");
  else
    open.setText ("&Open");

  open.setGeometry(30, 10, 9, 1);
  setGeometry (x, y, getWidth(), getHeight());
}

//----------------------------------------------------------------------
void FFileDialog::initCallbacks()
{
  filename.addCallback
  (
    "activate",
    F_METHOD_CALLBACK (this, &FFileDialog::cb_processActivate)
  );

  filebrowser.addCallback
  (
    "row-changed",
    F_METHOD_CALLBACK (this, &FFileDialog::cb_processRowChanged)
  );

  filebrowser.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &FFileDialog::cb_processClicked)
  );

  hidden.addCallback
  (
    "toggled",
    F_METHOD_CALLBACK (this, &FFileDialog::cb_processShowHidden)
  );

  cancel.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &FFileDialog::cb_processCancel)
  );

  open.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &FFileDialog::cb_processOpen)
  );
}

//----------------------------------------------------------------------
inline bool FFileDialog::pattern_match ( const char* const pattern
                                       , char fname[] )
{
  char search[128] = { };

  if ( show_hidden && fname[0] == '.' && fname[1] != '\0' )  // hidden files
  {
    search[0] = '.';
    search[1] = '\0';
    std::strncat(search, pattern, sizeof(search) - std::strlen(search) - 1);
  }
  else
    std::strncpy(search, pattern, sizeof(search));

  search[sizeof(search) - 1] = '\0';

  if ( fnmatch (search, fname, FNM_PERIOD) == 0 )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
void FFileDialog::clear()
{
  std::vector<dir_entry>::const_iterator iter, last;

  if ( dir_entries.empty() )
    return;

  // delete all directory entries;
  iter = dir_entries.begin();
  last = dir_entries.end();

  while ( iter != last )
  {
    std::free (iter->name);
    ++iter;
  }

  dir_entries.clear();
}

//----------------------------------------------------------------------
int FFileDialog::numOfDirs()
{
  if ( dir_entries.empty() )
    return 0;

  int n = 0;
  std::vector<dir_entry>::const_iterator iter, last;
  iter = dir_entries.begin();
  last = dir_entries.end();

  while ( iter != last )
  {
    if ( iter->directory && std::strcmp(iter->name, ".") != 0 )
      n++;

    ++iter;
  }

  return n;
}

//----------------------------------------------------------------------
void FFileDialog::sortDir()
{
  int start, dir_num;

  if ( std::strcmp((*dir_entries.begin()).name, "..") == 0 )
    start = 1;
  else
    start = 0;

  dir_num = numOfDirs();
  // directories first
  std::sort ( dir_entries.begin() + start
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
int FFileDialog::readDir()
{
  const char* const dir = directory.c_str();
  directory_stream = opendir(dir);

  if ( ! directory_stream )
  {
    FMessageBox::error (this, "Can't open directory\n" + directory);
    return -1;
  }

  clear();

  while ( true )
  {
    errno = 0;
    struct dirent* next = readdir(directory_stream);

    if ( next )
    {
      // Continue if name = "." (current directory)
      if ( next->d_name[0] == '.' && next->d_name[1] == '\0' )
        continue;

      // Skip hidden entries
      if ( ! show_hidden
        && next->d_name[0] == '.'
        && next->d_name[1] != '\0'
        && next->d_name[1] != '.' )
        continue;

      // Skip ".." for the root directory
      if ( dir[0] == '/' && dir[1] == '\0'
        && std::strcmp(next->d_name, "..") == 0  )
        continue;

      getEntry(dir, next);
    }
    else if ( errno != 0 )
    {
      FMessageBox::error (this, "Reading directory\n" + directory);

      if ( errno == EOVERFLOW )  // Value too large to be stored in data type
        break;
    }
    else
      break;
  }  // end while

  if ( closedir(directory_stream) != 0 )
  {
    FMessageBox::error (this, "Closing directory\n" + directory);
    return -2;
  }

  sortDir();

  // Insert directory entries into the list
  dirEntriesToList();

  return 0;
}

//----------------------------------------------------------------------
void FFileDialog::getEntry (const char* const dir, struct dirent* d_entry)
{
  const char* const filter = filter_pattern.c_str();
  dir_entry entry;

  entry.name = strdup(d_entry->d_name);

#if defined _DIRENT_HAVE_D_TYPE || defined HAVE_STRUCT_DIRENT_D_TYPE
  entry.fifo             = (d_entry->d_type & DT_FIFO) == DT_FIFO;
  entry.character_device = (d_entry->d_type & DT_CHR ) == DT_CHR;
  entry.directory        = (d_entry->d_type & DT_DIR ) == DT_DIR;
  entry.block_device     = (d_entry->d_type & DT_BLK ) == DT_BLK;
  entry.regular_file     = (d_entry->d_type & DT_REG ) == DT_REG;
  entry.symbolic_link    = (d_entry->d_type & DT_LNK ) == DT_LNK;
  entry.socket           = (d_entry->d_type & DT_SOCK) == DT_SOCK;
#else
  struct stat s;
  stat (entry.name, &s);
  entry.fifo             = S_ISFIFO (s.st_mode);
  entry.character_device = S_ISCHR (s.st_mode);
  entry.directory        = S_ISDIR (s.st_mode);
  entry.block_device     = S_ISBLK (s.st_mode);
  entry.regular_file     = S_ISREG (s.st_mode);
  entry.symbolic_link    = S_ISLNK (s.st_mode);
  entry.socket           = S_ISSOCK (s.st_mode);
#endif

  followSymLink (dir, entry);

  if ( entry.directory )
    dir_entries.push_back (entry);
  else if ( pattern_match(filter, entry.name) )
    dir_entries.push_back (entry);
  else
    std::free(entry.name);
}

//----------------------------------------------------------------------
void FFileDialog::followSymLink (const char* const dir, dir_entry& entry)
{
  if ( ! entry.symbolic_link )
    return;  // No symbolic link

  char resolved_path[MAXPATHLEN] = { };
  char symLink[MAXPATHLEN] = { };
  struct stat sb;

  std::strncpy (symLink, dir, sizeof(symLink));
  symLink[sizeof(symLink) - 1] = '\0';
  std::strncat ( symLink
               , entry.name
               , sizeof(symLink) - std::strlen(symLink) - 1);
  symLink[sizeof(symLink) - 1] = '\0';

  if ( realpath(symLink, resolved_path) == 0 )
    return;  // Cannot follow the symlink

  if ( lstat(resolved_path, &sb) == -1 )
    return;  // Cannot get file status

  if ( S_ISDIR(sb.st_mode) )
    entry.directory = true;
}

//----------------------------------------------------------------------
void FFileDialog::dirEntriesToList()
{
  // Fill list with directory entries

  filebrowser.clear();

  if ( dir_entries.empty() )
    return;

  std::vector<dir_entry>::const_iterator iter, last;
  iter = dir_entries.begin();
  last = dir_entries.end();

  while ( iter != last )
  {
    if ( iter->directory )
      filebrowser.insert(FString(iter->name), fc::SquareBrackets);
    else
      filebrowser.insert(FString(iter->name));

    ++iter;
  }
}

//----------------------------------------------------------------------
int FFileDialog::changeDir (const FString& dirname)
{
  FString lastdir = directory;
  FString newdir = dirname;

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
      if ( newdir == FString("..") )
      {
        if ( lastdir == FString('/') )
          filename.setText('/');
        else if ( ! dir_entries.empty() )
        {
          std::size_t i = 1;
          std::vector<dir_entry>::const_iterator iter, last;
          const char* const baseName = \
              basename(C_STR(lastdir.c_str()));
          iter = dir_entries.begin();
          last = dir_entries.end();

          while ( iter != last )
          {
            if ( std::strcmp(iter->name, baseName) == 0 )
            {
              filebrowser.setCurrentItem(i);
              filename.setText(FString(baseName) + '/');
              break;
            }

            i++;
            ++iter;
          }
        }
      }
      else
      {
        FString firstname = dir_entries[0].name;

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
  const FString& path = txt;
  const uInt max_width = uInt(filebrowser.getWidth()) - 4;

  if ( path.getLength() > max_width )
    filebrowser.setText(".." + path.right(max_width - 2));
  else
    filebrowser.setText(path);
}

//----------------------------------------------------------------------
const FString FFileDialog::getHomeDir()
{
  struct passwd pwd;
  struct passwd* pwd_ptr;
  char buf[1024];

  if ( getpwuid_r (geteuid(), &pwd, buf, sizeof(buf), &pwd_ptr) )
    return FString("");
  else
    return FString(pwd.pw_dir);
}

//----------------------------------------------------------------------
void FFileDialog::cb_processActivate (FWidget*, data_ptr)
{
  if ( filename.getText().includes('*')
    || filename.getText().includes('?') )
  {
    setFilter(filename.getText());
    readDir();
    filebrowser.redraw();
  }
  else if ( filename.getText().getLength() == 0 )
  {
    setFilter("*");
    readDir();
    filebrowser.redraw();
  }
  else if ( filename.getText().trim() == FString("..")
         || filename.getText().includes('/')
         || filename.getText().includes('~') )
  {
    changeDir(filename.getText().trim());
  }
  else
  {
    bool found = false;

    if ( ! dir_entries.empty() )
    {
      std::vector<dir_entry>::const_iterator iter, last;
      const FString& input = filename.getText().trim();
      iter = dir_entries.begin();
      last = dir_entries.end();

      while ( iter != last )
      {
        if ( iter->name && input && ! input.isNull()
          && std::strcmp(iter->name, input) == 0
          && iter->directory )
        {
          found = true;
          changeDir(input);
          break;
        }

        ++iter;
      }
    }

    if ( ! found )
      done (FDialog::Accept);
  }
}

//----------------------------------------------------------------------
void FFileDialog::cb_processRowChanged (FWidget*, data_ptr)
{
  const std::size_t n = filebrowser.currentItem();

  if ( n == 0 )
    return;

  const FString& name = dir_entries[n - 1].name;

  if ( dir_entries[n - 1].directory )
    filename.setText( name + '/' );
  else
    filename.setText( name );

  filename.redraw();
}

//----------------------------------------------------------------------
void FFileDialog::cb_processClicked (FWidget*, data_ptr)
{
  const uLong n = uLong(filebrowser.currentItem() - 1);

  if ( dir_entries[n].directory )
    changeDir(dir_entries[n].name);
  else
    done (FDialog::Accept);
}

//----------------------------------------------------------------------
void FFileDialog::cb_processCancel (FWidget*, data_ptr)
{
  done (FDialog::Reject);
}

//----------------------------------------------------------------------
void FFileDialog::cb_processOpen (FWidget*, data_ptr)
{
  done (FDialog::Accept);
}

//----------------------------------------------------------------------
void FFileDialog::cb_processShowHidden (FWidget*, data_ptr)
{
  setShowHiddenFiles(! show_hidden);
}

}  // namespace finalcut
