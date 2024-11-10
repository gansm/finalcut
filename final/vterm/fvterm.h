/***********************************************************************
* fvterm.h - Virtual terminal implementation                           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2016-2024 Markus Gans                                      *
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
 *  ▕▔▔▔▔▔▔▔▔▔▏1         *▕▔▔▔▔▔▔▔▏
 *  ▕ FVTerm  ▏- - -┬- - -▕ FTerm ▏
 *  ▕▁▁▁▁▁▁▁▁▁▏     :     ▕▁▁▁▁▁▁▁▏
 *                  :
 *                  :    *▕▔▔▔▔▔▔▔▔▔▏
 *                  :- - -▕ FString ▏
 *                  :     ▕▁▁▁▁▁▁▁▁▁▏
 *                  :
 *                  :    *▕▔▔▔▔▔▔▔▔▏
 *                  └- - -▕ FPoint ▏
 *                  :     ▕▁▁▁▁▁▁▁▁▏
 *                  :
 *                  :    *▕▔▔▔▔▔▔▔▏
 *                  └- - -▕ FRect ▏
 *                        ▕▁▁▁▁▁▁▁▏
 */

#ifndef FVTERM_H
#define FVTERM_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <sys/time.h>  // need for timeval (cygwin)

#include <algorithm>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "final/fc.h"
#include "final/output/tty/fterm_functions.h"
#include "final/util/fdata.h"
#include "final/util/fpoint.h"
#include "final/util/frect.h"
#include "final/util/fsize.h"
#include "final/util/fstringstream.h"
#include "final/vterm/fvtermattribute.h"
#include "final/vterm/fvtermbuffer.h"

#define F_PREPROC_HANDLER(i,h) \
    reinterpret_cast<FVTerm*>((i)), \
    std::bind ( reinterpret_cast<FVTerm::FPreprocessingHandler>((h)) \
              , reinterpret_cast<FVTerm*>((i)) )

namespace finalcut
{

// class forward declaration
class FColorPair;
class FOutput;
class FSize;
class FString;
class FStyle;
class FVTermBuffer;
class FWidget;

template <typename FOutputType>
struct outputClass
{
  using type = std::remove_cv_t<FOutputType>;
};

//----------------------------------------------------------------------
// class FVTerm
//----------------------------------------------------------------------

class FVTerm : public FVTermAttribute
{
  public:
    struct FTermArea;             // forward declaration
    struct FVTermPreprocessing;   // forward declaration

    struct FLineChanges
    {
      uInt xmin;           // X-position with the first change
      uInt xmax;           // X-position with the last change
      uInt trans_count;    // Number of transparent characters
    };

    // Using-declarations
    using FVTermAttribute::print;
    using FCharVector = std::vector<FChar>;
    using FPreprocessingHandler = void (FVTerm::*)();
    using FPreprocessingFunction = std::function<void()>;
    using FPreprocVector = std::vector<std::unique_ptr<FVTermPreprocessing>>;
    using FVTermList = std::vector<FVTerm*>;

    // Enumeration
    enum class TerminalUpdate
    {
      Stop,      // No terminal refresh
      Continue,  // Resuming terminal refresh
      Start      // Allowing terminal refresh
    };

    // Disable copy constructor
    FVTerm (const FVTerm&) = delete;

    // Disable move constructor
    FVTerm (FVTerm&&) noexcept = delete;

    // Constructor
    FVTerm();

    template <typename FOutputType>
    explicit FVTerm (outputClass<FOutputType>);

    // Disable copy assignment operator (=)
    auto operator = (const FVTerm&) -> FVTerm& = delete;

    // Disable move assignment operator (=)
    auto operator = (FVTerm&&) noexcept -> FVTerm& = delete;

    // Destructor
    ~FVTerm() override;

    // Overloaded operators
    template <typename NumT
            , enable_if_arithmetic_without_char_t<NumT> = nullptr>
    auto operator << (const NumT&) noexcept -> FVTerm&;

    template <typename CharT
            , enable_if_CString_t<CharT> = nullptr>
    auto operator << (const CharT&) noexcept -> FVTerm&;
    auto operator << (char) noexcept -> FVTerm&;
    auto operator << (wchar_t) noexcept -> FVTerm&;
    auto operator << (const wchar_t*) noexcept -> FVTerm&;
    auto operator << (const UniChar&) noexcept -> FVTerm&;
    auto operator << (const std::string&) noexcept -> FVTerm&;
    auto operator << (const std::wstring&) noexcept -> FVTerm&;
    auto operator << (const FString&) noexcept -> FVTerm&;
    auto operator << (FVTermBuffer&) noexcept -> FVTerm&;
    auto operator << (const FVTermBuffer&) noexcept -> FVTerm&;
    auto operator << (const FChar&) noexcept -> FVTerm&;
    auto operator << (const FCharVector&) noexcept -> FVTerm&;
    auto operator << (const FPoint&) noexcept -> FVTerm&;
    auto operator << (const FStyle&) noexcept -> FVTerm&;
    auto operator << (const FColorPair&) noexcept -> FVTerm&;

    // Accessors
    auto  getClassName() const -> FString override;
    static auto  getFOutput() -> std::shared_ptr<FOutput>;
    auto  getVWin() noexcept -> FTermArea*;
    auto  getVWin() const noexcept -> const FTermArea*;
    auto  getPrintCursor() -> FPoint;
    static auto  getWindowList() -> FVTermList*;

    // Mutators
    void  setTerminalUpdates (TerminalUpdate) const;
    void  setCursor (const FPoint&) noexcept;
    void  setVWin (std::unique_ptr<FTermArea>&&) noexcept;
    static void  setNonBlockingRead (bool = true);
    static void  unsetNonBlockingRead();

    // Inquiries
    static auto  isDrawingFinished() noexcept -> bool;
    static auto  isTerminalUpdateForced() noexcept -> bool;
    static auto  areTerminalUpdatesPaused() noexcept -> bool;
    static auto  hasPendingTerminalUpdates() noexcept -> bool;
    auto hasPreprocessingHandler (const FVTerm*) noexcept -> bool;

    // Methods
    virtual void clearArea (wchar_t = L' ');
    void  createVDesktop (const FSize& size) noexcept;
    void  createVTerm (const FSize&) noexcept;
    void  resizeVTerm (const FSize&) const noexcept;
    void  putVTerm() const;
    auto  updateTerminal() const -> bool;
    static void reduceTerminalLineUpdates (uInt);
    virtual void addPreprocessingHandler ( const FVTerm*
                                         , FPreprocessingFunction&& );
    virtual void delPreprocessingHandler (const FVTerm*);
    auto  interpretControlCodes (FTermArea*, const FChar&) const noexcept -> bool;
    template <typename... Args>
    auto  printf (const FString&, Args&&...) noexcept -> int;
    auto  print (const FString&) noexcept -> int;
    auto  print (FTermArea*, const FString&) noexcept -> int;
    auto  print (const std::vector<FChar>&) noexcept -> int;
    auto  print (FTermArea*, const std::vector<FChar>&) noexcept -> int;
    auto  print (FVTermBuffer&) noexcept -> int;
    auto  print (const FVTermBuffer&) noexcept -> int;
    auto  print (FTermArea*, FVTermBuffer&) const noexcept -> int;
    auto  print (FTermArea*, const FVTermBuffer&) const noexcept -> int;
    auto  print (wchar_t) noexcept -> int;
    auto  print (FTermArea*, wchar_t) noexcept -> int;
    auto  print (const FChar&) noexcept -> int;
    auto  print (FTermArea*, const FChar&) const noexcept -> int;
    virtual void print (const FPoint&);
    auto  print() & -> FVTerm&;
    void  flush() const;

  protected:
    struct FShadowBox
    {
      FRect box{};
      FSize shadow{};
    };

    // Accessor
    virtual auto getPrintArea() -> FTermArea*;
    auto  getChildPrintArea() const -> FTermArea*;
    auto  getCurrentPrintArea() const -> FTermArea*;
    auto  getVirtualDesktop() const -> FTermArea*;
    auto  getVirtualTerminal() const -> FTermArea*;

    // Mutators
    void  setPrintArea (FTermArea*);
    void  setChildPrintArea (FTermArea*);
    void  setActiveArea (FTermArea*) const;

    // Inquiries
    auto  isActive (const FTermArea*) const -> bool;
    auto  hasPrintArea() const -> bool;
    auto  hasChildPrintArea() const -> bool;
    auto  isVirtualWindow() const -> bool;
    auto  isCursorHideable() const -> bool;

    // Methods
    auto  createArea (const FShadowBox&) -> std::unique_ptr<FTermArea>;
    auto  createArea (const FRect&) -> std::unique_ptr<FTermArea>;
    void  resizeArea (const FShadowBox&, FTermArea*) const;
    void  resizeArea (const FRect&, FTermArea*) const;
    void  restoreVTerm (const FRect&) const noexcept;
    auto  updateVTermCursor (const FTermArea*) const noexcept -> bool;
    void  hideVTermCursor() const;
    void  setAreaCursor (const FPoint&, bool, FTermArea*) const noexcept;
    void  getArea (const FPoint&, FTermArea*) const noexcept;
    void  getArea (const FRect&, FTermArea*) const noexcept;
    void  addLayer (FTermArea*) const noexcept;
    void  putArea (const FPoint&, const FTermArea*) const noexcept;
    void  copyArea (FTermArea*, const FPoint&, const FTermArea* const)  const noexcept;
    static auto  getLayer (FVTerm&) noexcept -> int;
    static void  determineWindowLayers() noexcept;
    void  scrollAreaForward (FTermArea*);
    void  scrollAreaReverse (FTermArea*);
    void  clearArea (FTermArea*, wchar_t = L' ') noexcept;
    void  forceTerminalUpdate() const;
    auto  processTerminalUpdate() const -> bool;
    static void  startDrawing() noexcept;
    static void  finishDrawing() noexcept;
    virtual void initTerminal();

  private:
    // Constants
    static constexpr int DEFAULT_MINIMIZED_HEIGHT = 1;

    // Enumeration
    enum class CoveredState
    {
      None,
      Half,
      Full
    };

    // Methods
    static void setGlobalFVTermInstance (FVTerm*);
    static auto getGlobalFVTermInstance() -> FVTerm*&;
    static auto isInitialized() -> bool;
    void  resetAreaEncoding() const;
    void  resetTextAreaToDefault (FTermArea*, const FSize&) const noexcept;
    auto  resizeTextArea (FTermArea*, std::size_t, std::size_t ) const -> bool;
    auto  resizeTextArea (FTermArea*, std::size_t) const -> bool;
    auto  isCovered (const FPoint&, const FTermArea*) const noexcept -> CoveredState;
    auto  isAreaValid (const FShadowBox&) const -> bool;
    auto  isSizeEqual (const FTermArea*, const FShadowBox&) const -> bool;
    constexpr auto  needsHeightResize (const FTermArea*, const std::size_t) const noexcept -> bool;
    constexpr auto  needsWidthResize (const FTermArea*, const std::size_t) const noexcept -> bool;
    auto  tryResizeArea (FTermArea*, const std::size_t, const std::size_t) const -> bool;
    void  updateAreaProperties (FTermArea*, const FShadowBox&) const;
    constexpr auto  getFullAreaWidth (const FTermArea*) const noexcept -> int;
    constexpr auto  getFullAreaHeight (const FTermArea*) const noexcept -> int;
    void  passChangesToOverlap (const FTermArea*) const;
    void  processOverlappingWindows (const FTermArea*, const FVTermList&) const;
    void  passChangesToOverlappingWindow (FTermArea*, const FTermArea*) const;
    void  passChangesToOverlappingWindowLine (FTermArea*, int, const FTermArea*) const;
    int   calculateStartCoordinate (int, int) const noexcept;
    int   calculateEndCoordinate (int, int, int, int) const noexcept;
    void  restoreOverlaidWindows (const FTermArea*) const noexcept;
    void  updateVTerm() const;
    void  updateVTermDesktop() const;
    void  updateVTermWindow (FTermArea*) const;
    void  scrollTerminalForward() const;
    void  scrollTerminalReverse() const;
    void  callPreprocessingHandler (const FTermArea*) const;
    auto  hasChildAreaChanges (const FTermArea*) const -> bool;
    void  clearChildAreaChanges (const FTermArea*) const;
    auto  isInsideArea (const FPoint&, const FTermArea*) const -> bool;
    auto  isFCharTransparent (const FChar&) const -> bool;
    auto  isTransparentInvisible (const FChar&) const -> bool;
    static void defineByte1TransparentMask();
    template <typename FOutputType>
    void  init();
    void  initSettings();
    void  finish() const;
    void  saveCurrentVTerm() const;
    void  putAreaLine (const FChar&, FChar&, const std::size_t) const;
    void  putAreaLineWithTransparency (const FChar*, FChar*, const int, FPoint) const;
    void  putTransparentAreaLine (const FPoint&, const std::size_t) const;
    void  addAreaLineWithTransparency (const FChar*, FChar*, const std::size_t) const;
    void  addTransparentAreaLine (const FChar&, FChar&, const std::size_t) const;
    void  addTransparentAreaChar (const FChar&, FChar&) const;
    auto  clearFullArea (FTermArea*, FChar&) const -> bool;
    void  clearAreaWithShadow (FTermArea*, const FChar&) const noexcept;
    auto  printWrap (FTermArea*) const -> bool;
    auto  getByte1PrintTransMask() const -> uInt8;
    auto  changedToTransparency (const FChar&, const FChar&) const -> bool;
    auto  changedFromTransparency (const FChar&, const FChar&) const -> bool;
    auto  printCharacterOnCoordinate ( FTermArea*
                                     , const FChar&) const noexcept -> std::size_t;
    void  printPaddingCharacter (FTermArea*, const FChar&) const;
    void  putNonTransparent (std::size_t&, const FChar*, FChar*&) const;
    void  addTransparent (std::size_t&, const FChar*, FChar*&) const;
    void  putTransparent (std::size_t&, const FPoint&, FChar*&) const;
    auto  isInsideTerminal (const FPoint&) const noexcept -> bool;
    auto  canUpdateTerminalNow() const -> bool;
    static auto hasPendingUpdates (const FTermArea*) noexcept -> bool;

    // Data members
    FTermArea*                   print_area{nullptr};        // Print area for this object
    FTermArea*                   child_print_area{nullptr};  // Print area for children
    FVTermBuffer                 vterm_buffer{};             // Print buffer
    FChar                        nc{};                       // next character
    std::unique_ptr<FTermArea>   vwin{};                     // Virtual window
    std::shared_ptr<FOutput>     foutput{};                  // Terminal output class
    std::shared_ptr<FVTermList>  window_list{};              // List of all window owner
    std::shared_ptr<FTermArea>   vterm{};                    // Virtual terminal
    std::shared_ptr<FTermArea>   vterm_old{};                // Last virtual terminal
    std::shared_ptr<FTermArea>   vdesktop{};                 // Virtual desktop
    static FTermArea*            active_area;                // Active area
    static uInt8                 b1_print_trans_mask;        // Transparency mask
    static int                   tabstop;
    static bool                  draw_completed;
    static bool                  skip_one_vterm_update;
    static bool                  no_terminal_updates;
    static bool                  force_terminal_update;

    // Friend function
    friend void setPrintArea (FWidget&, FTermArea*);
    friend auto getColumnWidth (const wchar_t wchar) -> std::size_t;
};


//----------------------------------------------------------------------
// struct FVTerm::FTermArea
//----------------------------------------------------------------------

struct FVTerm::FTermArea  // Define virtual terminal character properties
{
  // Using-declaration
  using FDataAccessPtr  = std::shared_ptr<FDataAccess>;
  using FLineChangesPtr = std::vector<FLineChanges>;
  using FCharPtr        = std::vector<FChar>;

  // Constructor
  FTermArea() = default;

  // Disable copy constructor
  FTermArea (const FTermArea&) = delete;

  // Destructor
  ~FTermArea() = default;

  // Disable copy assignment operator (=)
  auto operator = (const FTermArea&) -> FTermArea& = delete;

  template <typename T>
  inline auto getOwner() const -> clean_fdata_t<T>&
  {
    return static_cast<FData<clean_fdata_t<T>>&>(*owner).get();
  }

  template <typename T>
  inline void setOwner (T&& obj)
  {
    owner.reset(makeFData(std::forward<T>(obj)));
  }

  inline auto hasOwner() const -> bool
  {
    return owner.get() != nullptr;
  }

  auto contains (const FPoint&) const noexcept -> bool;
  auto isOverlapped (const FRect&) const noexcept -> bool;
  auto isOverlapped (const FTermArea*) const noexcept -> bool;
  auto checkPrintPos() const noexcept -> bool;
  auto reprint (const FRect&, const FSize&) noexcept -> bool;

  inline auto getFChar (int x, int y) const noexcept -> const FChar&
  {
    return data[unsigned(y) * unsigned(size.width + shadow.width) + unsigned(x)];
  }

  inline auto getFChar (int x, int y) noexcept -> FChar&
  {
    return data[unsigned(y) * unsigned(size.width + shadow.width) + unsigned(x)];
  }

  inline auto getFChar (const FPoint& pos) const noexcept -> const FChar&
  {
    return getFChar(pos.getX(), pos.getY());
  }

  inline auto getFChar (const FPoint& pos) noexcept -> FChar&
  {
    return getFChar(pos.getX(), pos.getY());
  }

  inline void setCursorPos (int x, int y) noexcept
  {
    cursor.x = x;
    cursor.y = y;
  }

  inline void setInputCursorPos (int x, int y) noexcept
  {
    input_cursor.x = x;
    input_cursor.y = y;
  }

  template <typename T>
  inline auto print (T&& term_data) -> int
  {
    if ( hasOwner() )
      return getOwner<FVTerm*>()->print (this, std::forward<T>(term_data));

    return -1;
  }

  // Data members
  struct Coordinate
  {
    int x{};
    int y{};
  };

  struct Dimension
  {
    int width{};
    int height{};
  };

  Coordinate      position{0, 0};        // Distance from left and top of terminal edge
  Dimension       size{-1, -1};          // Window width and height
  Dimension       shadow{0, 0};          // Right and bottom window shadow
  Dimension       min_size{-1, -1};      // Minimized window width and height
  Coordinate      cursor{0, 0};          // Position for the next write operation
  Coordinate      input_cursor{-1, -1};  // Position of visible input cursor
  int             layer{-1};
  Encoding        encoding{Encoding::Unknown};
  bool            input_cursor_visible{false};
  bool            has_changes{false};
  bool            visible{false};
  bool            minimized{false};
  FDataAccessPtr  owner{nullptr};        // Object that owns this FTermArea
  FPreprocVector  preproc_list{};
  FLineChangesPtr changes{};
  FCharPtr        data{};                // FChar data of the drawing area
};

//----------------------------------------------------------------------
inline auto FVTerm::FTermArea::contains (const FPoint& pos) const noexcept -> bool
{
  // Is the terminal position (pos) located on my area?

  const int current_height = minimized ? min_size.height : size.height + shadow.height;
  const int x = pos.getX();
  const int y = pos.getY();
  return x >= position.x
      && x < position.x + size.width + shadow.width
      && y >= position.y
      && y < position.y + current_height;
}

//----------------------------------------------------------------------
inline auto FVTerm::FTermArea::isOverlapped (const FRect& box) const noexcept -> bool
{
  const int current_height = minimized ? min_size.height
                                       : size.height + shadow.height;
  const int x1 = position.x;
  const int x2 = position.x + size.width + shadow.width - 1;
  const int y1 = position.y;
  const int y2 = position.y + current_height - 1;

  return ( std::max(x1, box.getX1() - 1) <= std::min(x2, box.getX2() - 1)
        && std::max(y1, box.getY1() - 1) <= std::min(y2, box.getY2() - 1) );
}

//----------------------------------------------------------------------
inline auto FVTerm::FTermArea::isOverlapped (const FTermArea* area) const noexcept -> bool
{
  const int current_height = minimized ? min_size.height
                                       : size.height + shadow.height;
  const int x1 = position.x;
  const int x2 = position.x + size.width + shadow.width - 1;
  const int y1 = position.y;
  const int y2 = position.y + current_height - 1;

  const int area_current_height = area->minimized ? area->min_size.height
                                                  : area->size.height + area->shadow.height;
  const int area_x1 = area->position.x;
  const int area_x2 = area->position.x + area->size.width + area->shadow.width - 1;
  const int area_y1 = area->position.y;
  const int area_y2 = area->position.y + area_current_height - 1;

  return ( std::max(x1, area_x1) <= std::min(x2, area_x2)
        && std::max(y1, area_y1) <= std::min(y2, area_y2) );
}

//----------------------------------------------------------------------
inline auto FVTerm::FTermArea::checkPrintPos() const noexcept -> bool
{
  return cursor.x > 0
      && cursor.y > 0
      && cursor.x <= size.width + shadow.width
      && cursor.y <= size.height + shadow.height;
}


//----------------------------------------------------------------------
inline auto FVTerm::FTermArea::reprint (const FRect& box, const FSize& term_size) noexcept -> bool
{
  if ( ! isOverlapped(box) )
    return false;

  const int x_pos = box.getX() - 1;
  const int y_pos = box.getY() - 1;
  const auto w = int(box.getWidth());
  const auto h = int(box.getHeight());

  if ( w == 0 || h == 0 )
    return false;

  has_changes = true;
  const int y_start = std::max(0, std::max(y_pos, position.y)) - position.y;
  const int box_y2 = y_pos + h - 1;
  const int current_height = minimized ? min_size.height : size.height + shadow.height;
  const int y2 = position.y + current_height - 1;
  const int y_end = std::min(int(term_size.getHeight()) - 1, std::min(box_y2, y2)) - position.y;

  for (auto y{y_start}; y <= y_end; y++)  // Line loop
  {
    const int x_start = std::max(0, std::max(x_pos, position.x)) - position.x;
    const int box_x2 = x_pos + w - 1;
    const int x2 = position.x + size.width + shadow.width - 1;
    const int x_end = std::min(int(term_size.getWidth()) - 1 , std::min(box_x2, x2)) - position.x;
    auto& line_changes = changes[std::size_t(y)];
    line_changes.xmin = uInt(std::min(int(line_changes.xmin), x_start));
    line_changes.xmax = uInt(std::max(int(line_changes.xmax), x_end));
  }

  return true;
}


//----------------------------------------------------------------------
// struct FVTerm::FVTermPreprocessing
//----------------------------------------------------------------------
struct D
{
  void operator () (const FVTerm*) const noexcept
  {
    // No deleting of pointer objects when exiting the std::unique_ptr
  }
};

struct FVTerm::FVTermPreprocessing
{
  // Constructor
  FVTermPreprocessing (const FVTerm* i, FPreprocessingFunction&& f)
    : instance(std::unique_ptr<const FVTerm, D>(i))
    , function(std::move(f))
  { }

  FVTermPreprocessing (const FVTermPreprocessing&) = delete;
  FVTermPreprocessing (FVTermPreprocessing&&) noexcept = default;
  auto operator = (const FVTermPreprocessing&) -> FVTermPreprocessing& = delete;
  auto operator = (FVTermPreprocessing&&) noexcept -> FVTermPreprocessing& = default;

  // Data members
  std::unique_ptr<const FVTerm, D> instance{};
  FPreprocessingFunction function{};
};


// FVTerm inline functions
//----------------------------------------------------------------------
template <typename FOutputType>
FVTerm::FVTerm (outputClass<FOutputType>)
{
  init<typename outputClass<FOutputType>::type>();
}

//----------------------------------------------------------------------
template <typename NumT
        , enable_if_arithmetic_without_char_t<NumT>>
inline auto FVTerm::operator << (const NumT& n) noexcept -> FVTerm&
{
  print (FString(std::to_string(n)));
  return *this;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_CString_t<CharT>>
inline auto FVTerm::operator << (const CharT& s) noexcept -> FVTerm&
{
  print (FString(s));
  return *this;
}

//----------------------------------------------------------------------
inline auto FVTerm::operator << (char c) noexcept -> FVTerm&
{
  print (wchar_t(uChar(c)));
  return *this;
}

//----------------------------------------------------------------------
inline auto FVTerm::operator << (wchar_t c) noexcept -> FVTerm&
{
  print (c);
  return *this;
}

//----------------------------------------------------------------------
inline auto FVTerm::operator << (const wchar_t* wide_string) noexcept -> FVTerm&
{
  print (FString(wide_string));
  return *this;
}

//----------------------------------------------------------------------
inline auto FVTerm::operator << (const UniChar& c) noexcept -> FVTerm&
{
  print (static_cast<wchar_t>(c));  // Required under Solaris
  return *this;
}

//----------------------------------------------------------------------
inline auto FVTerm::operator << (const std::string& string) noexcept -> FVTerm&
{
  print (FString(string));
  return *this;
}

//----------------------------------------------------------------------
inline auto FVTerm::operator << (const std::wstring& wide_string) noexcept -> FVTerm&
{
  print (FString(wide_string));
  return *this;
}

//----------------------------------------------------------------------
inline auto FVTerm::operator << (const FString& fstring) noexcept -> FVTerm&
{
  print (fstring);
  return *this;
}

//----------------------------------------------------------------------
inline auto FVTerm::operator << (const FChar& fchar) noexcept -> FVTerm&
{
  print (fchar);
  return *this;
}

//----------------------------------------------------------------------
inline auto FVTerm::operator << (const FCharVector& term_string) noexcept -> FVTerm&
{
  print (term_string);
  return *this;
}

//----------------------------------------------------------------------
inline auto FVTerm::operator << (const FPoint& pos) noexcept -> FVTerm&
{
  print (pos);
  return *this;
}

//----------------------------------------------------------------------
inline auto FVTerm::operator << (const FStyle& t_style) noexcept -> FVTerm&
{
  print (t_style);
  return *this;
}

//----------------------------------------------------------------------
inline auto FVTerm::operator << (const FColorPair& pair) noexcept -> FVTerm&
{
  print (pair);
  return *this;
}

//----------------------------------------------------------------------
inline auto FVTerm::getClassName() const -> FString
{ return "FVTerm"; }

//----------------------------------------------------------------------
inline auto FVTerm::getVWin() noexcept -> FTermArea*
{ return vwin.get(); }

//----------------------------------------------------------------------
inline auto FVTerm::getVWin() const noexcept -> const FTermArea*
{ return vwin.get(); }

//----------------------------------------------------------------------
inline auto FVTerm::getWindowList() -> FVTermList*
{
  static const auto& init_object = getGlobalFVTermInstance();
  return (isInitialized() && init_object->window_list)
        ? init_object->window_list.get()
        : nullptr;
}

//----------------------------------------------------------------------
inline void FVTerm::setVWin (std::unique_ptr<FTermArea>&& area) noexcept
{ vwin = std::move(area); }

//----------------------------------------------------------------------
inline void FVTerm::unsetNonBlockingRead()
{ setNonBlockingRead(false); }

//----------------------------------------------------------------------
inline auto FVTerm::isDrawingFinished() noexcept -> bool
{ return draw_completed; }

//----------------------------------------------------------------------
inline auto FVTerm::isTerminalUpdateForced() noexcept -> bool
{ return force_terminal_update; }

//----------------------------------------------------------------------
inline auto FVTerm::areTerminalUpdatesPaused() noexcept -> bool
{ return no_terminal_updates; }

//----------------------------------------------------------------------
inline auto FVTerm::hasPendingTerminalUpdates() noexcept -> bool
{ return hasPendingUpdates(getGlobalFVTermInstance()->vterm.get()); }

//----------------------------------------------------------------------
template <typename... Args>
auto FVTerm::printf (const FString& format, Args&&... args) noexcept -> int
{
  FString str{};
  str.sprintf (format, std::forward<Args>(args)...);
  return print(str);
}

//----------------------------------------------------------------------
inline auto FVTerm::print() & -> FVTerm&
{ return *this; }

//----------------------------------------------------------------------
inline void FVTerm::print (const FPoint& p)
{
  setCursor (p);
}

//----------------------------------------------------------------------
inline auto FVTerm::getChildPrintArea() const -> FTermArea*
{ return child_print_area; }

//----------------------------------------------------------------------
inline auto FVTerm::getCurrentPrintArea() const -> FTermArea*
{ return print_area; }

//----------------------------------------------------------------------
inline auto FVTerm::getVirtualDesktop() const -> FTermArea*
{ return vdesktop.get(); }

//----------------------------------------------------------------------
inline auto FVTerm::getVirtualTerminal() const -> FTermArea*
{ return vterm.get(); }

//----------------------------------------------------------------------
inline void FVTerm::setPrintArea (FTermArea* area)
{ print_area = area; }

//----------------------------------------------------------------------
inline void FVTerm::setChildPrintArea (FTermArea* area)
{ child_print_area = area; }

//----------------------------------------------------------------------
inline void FVTerm::setActiveArea (FTermArea* area) const
{ active_area = area; }

//----------------------------------------------------------------------
inline auto FVTerm::isActive (const FTermArea* area) const -> bool
{ return area == active_area; }

//----------------------------------------------------------------------
inline auto FVTerm::hasPrintArea() const -> bool
{ return print_area; }

//----------------------------------------------------------------------
inline auto FVTerm::hasChildPrintArea() const -> bool
{ return child_print_area; }

//----------------------------------------------------------------------
inline auto FVTerm::isVirtualWindow() const -> bool
{ return vwin.get(); }

//----------------------------------------------------------------------
inline void FVTerm::hideVTermCursor() const
{ vterm->input_cursor_visible = false; }

//----------------------------------------------------------------------
inline auto FVTerm::getLayer (FVTerm& obj) noexcept -> int
{
  // returns the layer from the FVTerm object
  return obj.FVTerm::getPrintArea()->layer;
}

//----------------------------------------------------------------------
template <typename FOutputType>
inline void FVTerm::init()
{
  if ( ! isInitialized() )
  {
    setGlobalFVTermInstance(this);
    defineByte1TransparentMask();
    b1_print_trans_mask = getByte1PrintTransMask();
    foutput     = std::make_shared<FOutputType>(*this);
    window_list = std::make_shared<FVTermList>();
    initSettings();
  }
  else
  {
    static const auto& init_object = getGlobalFVTermInstance();
    foutput     = std::shared_ptr<FOutput>(init_object->foutput);
    window_list = std::shared_ptr<FVTermList>(init_object->window_list);
    vterm       = std::shared_ptr<FTermArea>(init_object->vterm);
    vterm_old   = std::shared_ptr<FTermArea>(init_object->vterm_old);
    vdesktop    = std::shared_ptr<FTermArea>(init_object->vdesktop);
  }
}

}  // namespace finalcut

#endif  // FVTERM_H
