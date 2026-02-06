/***********************************************************************
* fvterm.h - Virtual terminal implementation                           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2016-2026 Markus Gans                                      *
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

    // Using-declarations
    using FVTermAttribute::print;
    using FCharVector = std::vector<FChar>;
    using FPreprocessingHandler = void (FVTerm::*)();
    using FPreprocessingFunction = std::function<void()>;
    using FPreprocVector = std::vector<std::unique_ptr<FVTermPreprocessing>>;
    using FVTermList = std::vector<FVTerm*>;
    using FTermAreaList = std::vector<const FTermArea*>;

    // Enumeration
    enum class TerminalUpdate : uInt8
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
    static auto  getWindowList() noexcept -> FVTermList*;

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
    virtual void  clearArea (wchar_t = L' ');
    void  createVDesktop (const FSize& size) noexcept;
    void  createVTerm (const FSize&) noexcept;
    void  resizeVTerm (const FSize&) const noexcept;
    void  putVTerm() const;
    auto  updateTerminal() const -> bool;
    static void reduceTerminalLineUpdates (uInt);
    virtual void addPreprocessingHandler ( const FVTerm*
                                         , FPreprocessingFunction&& );
    virtual void delPreprocessingHandler (const FVTerm*);
    template <typename... Args>
    auto  printf (const FString&, Args&&...) -> int;
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
    virtual void  print (const FPoint&);
    auto  print() noexcept -> FVTerm&;
    void  flush() const;

  protected:
    // Using-declaration
    using FChar_iterator = std::vector<FChar>::iterator;
    using FChar_const_iterator = std::vector<FChar>::const_iterator;

    struct FShadowBox
    {
      FRect box{};
      FSize shadow{};
    };

    // Accessor
    virtual auto getPrintArea() -> FTermArea*;
    auto  getChildPrintArea() const noexcept -> FTermArea*;
    auto  getCurrentPrintArea() const noexcept -> FTermArea*;
    auto  getVirtualDesktop() const noexcept -> FTermArea*;
    auto  getVirtualTerminal() const noexcept -> FTermArea*;

    // Mutators
    void  setPrintArea (FTermArea*) noexcept;
    void  setChildPrintArea (FTermArea*) noexcept;
    void  setActiveArea (FTermArea*) const noexcept;

    // Inquiries
    auto  isActive (const FTermArea*) const noexcept -> bool;
    auto  hasPrintArea() const noexcept -> bool;
    auto  hasChildPrintArea() const noexcept -> bool;
    auto  isVirtualWindow() const noexcept -> bool;
    auto  isCursorHideable() const noexcept -> bool;

    // Methods
    auto  createArea (const FShadowBox&) -> std::unique_ptr<FTermArea>;
    auto  createArea (const FRect&) -> std::unique_ptr<FTermArea>;
    void  resizeArea (const FShadowBox&, FTermArea*) const;
    void  resizeArea (const FRect&, FTermArea*) const;
    void  restoreVTerm (const FRect&) const noexcept;
    auto  updateVTermCursor (const FTermArea*) const noexcept -> bool;
    void  hideVTermCursor() const noexcept;
    void  setAreaCursor (const FPoint&, bool, FTermArea*) const noexcept;
    void  getArea (const FPoint&, FTermArea*) const noexcept;
    void  getArea (const FRect&, FTermArea*) const noexcept;
    void  addLayer (FTermArea*) const noexcept;
    void  putArea (const FPoint&, FTermArea*) const noexcept;
    void  copyArea (FTermArea*, const FPoint&, FTermArea*)  const noexcept;
    static auto  getLayer (FVTerm&) noexcept -> int;
    static void  determineWindowLayers() noexcept;
    void  scrollAreaForward (FTermArea*);
    void  scrollAreaReverse (FTermArea*);
    void  clearArea (FTermArea*, wchar_t = L' ') noexcept;
    void  forceTerminalUpdate() const;
    auto  processTerminalUpdate() const -> bool;
    static void  startDrawing() noexcept;
    static void  finishDrawing() noexcept;
    virtual void  initTerminal();

  private:
    struct LayerGeometry
    {
      int area_x;
      int area_y;
      int ol;                  // Outside-left
      unsigned vterm_width;
      unsigned vterm_height;
      unsigned ax;             // Clipped area x (>=0)
      int width;               // Full area width
      int height;              // Effective area height (minimized or full)
      int xmax_inside_vterm;
      int y_start;
      int y_end;
    };

    struct AreaLine
    {
      const FChar* data;       // Source drawing area line
      unsigned     offset;     // Source data offset
      std::size_t  start_idx;  // Start index
      std::size_t  end_idx;    // End index
    };

    enum class NoTrans : sInt8
    {
      Undefined = -1,
      Set,
      Unset
    };

    struct LineChanges
    {
      int  count;
      int  ypos;
      int  xmin;
      int  xmax;
      NoTrans  has_no_transparency;
    };

    // Constants
    static constexpr int DEFAULT_MINIMIZED_HEIGHT = 1;

    // Enumeration
    enum class CoveredState : uInt8
    {
      None,
      Half,
      Full
    };

    enum class SearchState : uInt8  // Character search state
    {
      start,       // The overlaid characters are not parsed
      printable,   // Search for a printable character
      overlay,     // Search for the next printable character for color overlay
      background,  // Search for the next printable character to inherit the background color
      ready        // Overlaid character was fully printed
    };

    // Using-declaration
    using FOverlaySearchBuffer = std::vector<SearchState>;
    using FOverlayLineBuffer = std::vector<AreaLine>;
    using FLineChangesBatch = std::vector<LineChanges>;

    // Methods
    static void setGlobalFVTermInstance (FVTerm*) noexcept;
    static auto getGlobalFVTermInstance() noexcept -> FVTerm*&;
    static auto isInitialized() noexcept -> bool;
    void  resetAreaEncoding() const;
    void  resetTextAreaToDefault (FTermArea*, const FSize&) const noexcept;
    auto  resizeTextArea (FTermArea*, std::size_t, std::size_t ) const -> bool;
    auto  resizeTextArea (FTermArea*, std::size_t) const -> bool;
    auto  isCovered (const FPoint&, const FTermArea*) const noexcept -> CoveredState;
    auto  isAreaValid (const FShadowBox&) const noexcept -> bool;
    auto  isSizeEqual (const FTermArea*, const FShadowBox&) const noexcept -> bool;
    constexpr auto  needsHeightResize (const FTermArea*, const std::size_t) const noexcept -> bool;
    constexpr auto  needsWidthResize (const FTermArea*, const std::size_t) const noexcept -> bool;
    auto  tryResizeArea (FTermArea*, const std::size_t, const std::size_t) const -> bool;
    void  updateAreaProperties (FTermArea*, const FShadowBox&) const noexcept;
    constexpr auto  getFullAreaWidth (const FTermArea*) const noexcept -> int;
    constexpr auto  getFullAreaHeight (const FTermArea*) const noexcept -> int;
    void  passChangesToOverlap (const FTermArea*) const;
    void  processOverlappingWindows (const FTermArea*, const FVTermList&) const;
    void  passChangesToOverlappingWindow (FTermArea*, const FTermArea*) const noexcept;
    void  passChangesToOverlappingWindowLine (FTermArea*, int, const FTermArea*) const noexcept;
    auto  calculateStartCoordinate (int, int) const noexcept -> int;
    auto  calculateEndCoordinate (int, int, int, int) const noexcept -> int;
    void  updateVTerm() const;
    void  updateVTermDesktop() const;
    void  updateVTermWindow (FTermArea*) const;
    auto  computeLayerGeometry (const FTermArea*) const noexcept -> LayerGeometry;
    auto  isLayerOutsideVTerm (const LayerGeometry&) const noexcept -> bool;
    void  buildLineChangeBatch (const FTermArea*, const LayerGeometry&) const noexcept;
    void  applyLineBatch (FTermArea*, const LayerGeometry&) const noexcept;
    void  updateVTermChangesFromBatch (const LayerGeometry&) const noexcept;
    void  scrollTerminalForward() const;
    void  scrollTerminalReverse() const;
    void  callPreprocessingHandler (const FTermArea*) const;
    auto  hasChildAreaChanges (const FTermArea*) const -> bool;
    void  clearChildAreaChanges (const FTermArea*) const;
    static constexpr auto  isInsideArea (const FPoint&, const FTermArea*) noexcept -> bool;
    auto  isInsideTerminal (const FPoint&) const noexcept -> bool;
    static constexpr auto  isFCharTransparent (const FChar&) noexcept -> bool;
    auto  isTransparentInvisible (const FChar&) const noexcept -> bool;
    template <typename FOutputType>
    void  init();
    void  initFromGlobalInstance();
    void  initSettings();
    void  finish() const;
    void  saveCurrentVTerm() const noexcept;
    void  putAreaLine (const FChar&, FChar&, const std::size_t) const noexcept;
    void  applyColorOverlay (const FChar&, FChar&) const;
    void  inheritBackground (const FChar&, FChar&) const;
    void  putMultiLayerAreaLine (FChar*, const std::size_t, const FPoint&) const noexcept;
    void  putAreaLineWithTransparency (const FChar*, FChar*, const int, FPoint, bool) const;
    void  addAreaLineWithTransparency (const FChar*, FChar*, const std::size_t) const;
    void  addTransparentAreaLine (const FChar&, FChar&, const std::size_t) const;
    void  addTransparentAreaChar (const FChar&, FChar&) const;
    auto  clearFullArea (FTermArea*, FChar&) const -> bool;
    void  clearAreaWithShadow (FTermArea*, const FChar&) const noexcept;
    auto  printWrap (FTermArea*, FChar_iterator&) const -> bool;
    auto  interpretControlCodes (FTermArea*, FChar_iterator&, const FChar&) const noexcept -> bool;
    auto  printCharacter (FTermArea*, FChar_iterator&, const FChar&) const noexcept -> int;
    auto  printCharacterOnCoordinate ( FTermArea*
                                     , FChar_iterator&
                                     , const FChar&) const noexcept -> std::size_t;
    void  printPaddingCharacter (FTermArea*, const FChar&) const;
    void  putNonTransparent (const FChar*, FChar*&, std::size_t&) const;
    void  addTransparent (const FChar*, FChar*&, std::size_t&) const;
    void  addVDesktopToListIfExists (FTermAreaList&) const;
    void  determineCoveredAreas (FTermArea*) const;
    void  resetLineCoveredState (FTermArea*) const;
    void  determineLineCoveredState (const FTermArea* const, FTermArea*) const noexcept;
    auto  canUpdateTerminalNow() const -> bool;
    static auto hasPendingUpdates (const FTermArea*) noexcept -> bool;

    // Data members
    FTermArea*                   print_area{nullptr};        // Print area for this object
    FTermArea*                   child_print_area{nullptr};  // Print area for children
    FVTermBuffer                 vterm_buffer{};             // Print buffer
    mutable FLineChangesBatch    line_changes_batch{};       // All line changes to an area
    mutable FOverlayLineBuffer   overlay_line_buffer{};      // Overlay area line buffer
    mutable FOverlaySearchBuffer overlay_search_buffer{};    // Overlay search state buffer
    mutable FTermAreaList        covered_areas_buffer{};     // Covered overlay areas buffer
    FChar                        nc{};                       // Next character
    std::unique_ptr<FTermArea>   vwin{};                     // Virtual window
    std::shared_ptr<FOutput>     foutput{};                  // Terminal output class
    std::shared_ptr<FVTermList>  vterm_window_list{};        // List of all window owner in z-order
    std::shared_ptr<FTermArea>   vterm{};                    // Virtual terminal
    std::shared_ptr<FTermArea>   vterm_old{};                // Last virtual terminal
    std::shared_ptr<FTermArea>   vdesktop{};                 // Virtual desktop
    static FTermArea*            active_area;                // Active area
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
  struct FLineChanges
  {
    uInt xmin;         // X-position with the first change
    uInt xmax;         // X-position with the last change
    uInt trans_count;  // Number of transparent characters
    bool covered;      // Line cover state
  };

  struct FRowChanges
  {
    uInt ymin;         // Y-position with the first change
    uInt ymax;         // Y-position with the last change
  };

  // Using-declaration
  using FDataAccessPtr        = std::shared_ptr<FDataAccess>;
  using FLineChangesVec       = std::vector<FLineChanges>;
  using FCharVec              = std::vector<FChar>;
  using FChar_reference       = FCharVec::reference;
  using FChar_const_reference = FCharVec::const_reference;
  using FChar_iterator        = FCharVec::iterator;
  using FChar_const_iterator  = FCharVec::const_iterator;

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

  constexpr auto contains (int, int) const noexcept -> bool;

  constexpr auto contains (const FPoint& pos) const noexcept -> bool
  {
    return contains(pos.getX(), pos.getY());
  }

  constexpr auto isOverlapped (const FRect&) const noexcept -> bool;
  constexpr auto isOverlapped (const FTermArea*) const noexcept -> bool;
  constexpr auto isPrintPositionInsideArea() const noexcept -> bool;
  auto reprint (const FRect&, const FSize&) noexcept -> bool;



  inline auto getFChar (int x, int y) const noexcept -> FChar_const_reference
  {
    return data[unsigned(y) * unsigned(size.width + shadow.width) + unsigned(x)];
  }

  inline auto getFChar (int x, int y) noexcept -> FChar_reference
  {
    return data[unsigned(y) * unsigned(size.width + shadow.width) + unsigned(x)];
  }

  inline auto getFChar (const FPoint& pos) const noexcept -> FChar_const_reference
  {
    return getFChar(pos.getX(), pos.getY());
  }

  inline auto getFChar (const FPoint& pos) noexcept -> FChar_reference
  {
    return getFChar(pos.getX(), pos.getY());
  }

  inline auto getFCharIterator (int x, int y) const noexcept -> FChar_const_iterator
  {
    return data.cbegin() + (unsigned(y) * unsigned(size.width + shadow.width) + unsigned(x));
  }

  inline auto getFCharIterator (int x, int y) noexcept -> FChar_iterator
  {
    return data.begin() + (unsigned(y) * unsigned(size.width + shadow.width) + unsigned(x));
  }

  constexpr void setCursorPos (int x, int y) noexcept
  {
    cursor.x = x;
    cursor.y = y;
  }

  constexpr void setInputCursorPos (int x, int y) noexcept
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

  void updateAreaChanges (uInt, uInt, uInt8) noexcept;

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
  FRowChanges     changes_in_row{};
  FLineChangesVec changes_in_line{};
  FCharVec        data{};                // FChar data of the drawing area
};

//----------------------------------------------------------------------
constexpr auto FVTerm::FTermArea::contains (int x, int y) const noexcept -> bool
{
  // Is the terminal position (pos) located on my area?

  const int current_height = minimized ? min_size.height
                                       : size.height + shadow.height;
  const int x_min = position.x;
  const int x_max = x_min + size.width + shadow.width;
  const int y_min = position.y;
  const int y_max = y_min + current_height;
  return x >= x_min
      && x < x_max
      && y >= y_min
      && y < y_max;
}

//----------------------------------------------------------------------
constexpr auto FVTerm::FTermArea::isOverlapped (const FRect& box) const noexcept -> bool
{
  const int x1 = position.x;
  const int x2 = x1 + size.width + shadow.width;

  if ( x2 < box.getX1() || x1 >= box.getX2()  )
    return false;

  const int y1 = position.y;
  const int height = minimized ? min_size.height
                               : size.height + shadow.height;
  const int y2 = y1 + height;
  return ! ( y2 < box.getY1() || y1 >= box.getY2() );
}

//----------------------------------------------------------------------
constexpr auto FVTerm::FTermArea::isOverlapped (const FTermArea* area) const noexcept -> bool
{
  const int area_x1 = area->position.x;
  const int area_x2 = area_x1 + area->size.width + area->shadow.width;
  const int x1 = position.x;
  const int x2 = x1 + size.width + shadow.width;

  if ( x2 <= area_x1 || x1 >= area_x2 )
    return false;

  const int area_height = area->minimized ? area->min_size.height
                                          : area->size.height + area->shadow.height;
  const int area_y1 = area->position.y;
  const int area_y2 = area_y1 + area_height;
  const int height = minimized ? min_size.height
                               : size.height + shadow.height;
  const int y1 = position.y;
  const int y2 = y1 + height;
  return ! ( y2 <= area_y1 || y1 >= area_y2 );
}

//----------------------------------------------------------------------
constexpr auto FVTerm::FTermArea::isPrintPositionInsideArea() const noexcept -> bool
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

  const int box_x1 = box.getX1() - 1;
  const int box_y1 = box.getY1() - 1;
  const int box_x2 = box.getX2() - 1;
  const int box_y2 = box.getY2() - 1;

  if ( box_x1 > box_x2 || box_y1 > box_y2 )
    return false;

  has_changes = true;

  // Compute geometry
  const int term_w = int(term_size.getWidth()) - 1;
  const int term_h = int(term_size.getHeight()) - 1;
  const int self_h = minimized ? min_size.height
                               : size.height + shadow.height;
  const int self_w  = size.width + shadow.width;
  const int self_x1 = position.x;
  const int self_y1 = position.y;
  const int self_x2 = self_x1 + self_w - 1;
  const int self_y2 = self_y1 + self_h - 1;

  // Vertical boundaries
  const int y_start = std::max(std::max(0, box_y1), self_y1) - self_y1;
  const int y_end   = std::min(std::min(term_h, box_y2), self_y2) - self_y1;

  // Horizontal boundaries
  const int x_start = std::max(std::max(0, box_x1), self_x1) - self_x1;
  const int x_end   = std::min(std::min(term_w, box_x2), self_x2) - self_x1;

  if ( y_end < y_start || x_end < x_start )  // Nothing visible
    return false;

  auto first_row = unsigned(y_start);
  auto last_row  = unsigned(std::max(0, y_end));
  auto* line_changes = &changes_in_line[first_row];
  const auto* const line_changes_end = &changes_in_line[last_row];

  while  ( line_changes <= line_changes_end )  // Line loop
  {
    line_changes->xmin = std::min(line_changes->xmin, uInt(x_start));
    line_changes->xmax = std::max(line_changes->xmax, uInt(x_end));
    ++line_changes;
  }

  changes_in_row.ymin = std::min(changes_in_row.ymin, first_row);
  changes_in_row.ymax = std::max(changes_in_row.ymax, last_row);
  return true;
}

//----------------------------------------------------------------------
inline void FVTerm::FTermArea::updateAreaChanges ( uInt x, uInt y
                                                 , uInt8 char_width ) noexcept
{
  auto& line_changes = changes_in_line[y];
  const uInt x_end = x + uInt( char_width >> 1 );

  line_changes.xmin = std::min(line_changes.xmin, x);
  line_changes.xmax = std::max(line_changes.xmax, x_end);

  changes_in_row.ymin = std::min(changes_in_row.ymin, y);
  changes_in_row.ymax = std::max(changes_in_row.ymax, y);
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
  print (wide_string);
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
  print (string);
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
inline auto FVTerm::getWindowList() noexcept -> FVTermList*
{
  static const auto& init_object = getGlobalFVTermInstance();
  return ( isInitialized() && init_object->vterm_window_list )
        ? init_object->vterm_window_list.get()
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
auto FVTerm::printf (const FString& format, Args&&... args) -> int
{
  FString str{};
  str.sprintf (format, std::forward<Args>(args)...);
  return print(str);
}

//----------------------------------------------------------------------
inline auto FVTerm::print() noexcept -> FVTerm&
{ return *this; }

//----------------------------------------------------------------------
inline void FVTerm::print (const FPoint& p)
{
  setCursor (p);
}

//----------------------------------------------------------------------
inline auto FVTerm::getChildPrintArea() const noexcept -> FTermArea*
{ return child_print_area; }

//----------------------------------------------------------------------
inline auto FVTerm::getCurrentPrintArea() const noexcept -> FTermArea*
{ return print_area; }

//----------------------------------------------------------------------
inline auto FVTerm::getVirtualDesktop() const noexcept -> FTermArea*
{ return vdesktop.get(); }

//----------------------------------------------------------------------
inline auto FVTerm::getVirtualTerminal() const noexcept -> FTermArea*
{ return vterm.get(); }

//----------------------------------------------------------------------
inline void FVTerm::setPrintArea (FTermArea* area) noexcept
{ print_area = area; }

//----------------------------------------------------------------------
inline void FVTerm::setChildPrintArea (FTermArea* area) noexcept
{ child_print_area = area; }

//----------------------------------------------------------------------
inline void FVTerm::setActiveArea (FTermArea* area) const noexcept
{ active_area = area; }

//----------------------------------------------------------------------
inline auto FVTerm::isActive (const FTermArea* area) const noexcept -> bool
{ return area == active_area; }

//----------------------------------------------------------------------
inline auto FVTerm::hasPrintArea() const noexcept -> bool
{ return print_area; }

//----------------------------------------------------------------------
inline auto FVTerm::hasChildPrintArea() const noexcept -> bool
{ return child_print_area; }

//----------------------------------------------------------------------
inline auto FVTerm::isVirtualWindow() const noexcept -> bool
{ return vwin.get(); }

//----------------------------------------------------------------------
inline void FVTerm::hideVTermCursor() const noexcept
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
  if ( isInitialized() )
  {
    initFromGlobalInstance();
  }
  else
  {
    setGlobalFVTermInstance(this);
    foutput           = std::make_shared<FOutputType>(*this);
    vterm_window_list = std::make_shared<FVTermList>();
    initSettings();
  }
}

//----------------------------------------------------------------------
inline void FVTerm::initFromGlobalInstance()
{
  const auto& init_object = getGlobalFVTermInstance();

  if ( ! init_object )
    return;

  foutput           = std::shared_ptr<FOutput>(init_object->foutput);
  vterm_window_list = std::shared_ptr<FVTermList>(init_object->vterm_window_list);
  vterm             = std::shared_ptr<FTermArea>(init_object->vterm);
  vterm_old         = std::shared_ptr<FTermArea>(init_object->vterm_old);
  vdesktop          = std::shared_ptr<FTermArea>(init_object->vdesktop);
}

}  // namespace finalcut

#endif  // FVTERM_H
