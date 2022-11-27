/***********************************************************************
* fvterm.h - Virtual terminal implementation                           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2016-2022 Markus Gans                                      *
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

#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "final/fc.h"
#include "final/output/tty/fterm_functions.h"
#include "final/util/fdata.h"
#include "final/util/fpoint.h"
#include "final/util/fstringstream.h"
#include "final/vterm/fvtermattribute.h"

#define F_PREPROC_HANDLER(i,h) \
    reinterpret_cast<FVTerm*>((i)), \
    std::bind ( reinterpret_cast<FVTerm::FPreprocessingHandler>((h)) \
              , reinterpret_cast<FVTerm*>((i)) )

namespace finalcut
{

// class forward declaration
class FColorPair;
class FOutput;
class FRect;
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
    using FPreprocessing = std::vector<std::unique_ptr<FVTermPreprocessing>>;
    using FVTermList = std::vector<FVTerm*>;

    // Enumerations
    enum class CoveredState
    {
      None,
      Half,
      Full
    };

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
    auto operator << (const FVTermBuffer&) noexcept -> FVTerm&;
    auto operator << (const FChar&) noexcept -> FVTerm&;
    auto operator << (const FCharVector&) noexcept -> FVTerm&;
    auto operator << (const FPoint&) noexcept -> FVTerm&;
    auto operator << (const FStyle&) noexcept -> FVTerm&;
    auto operator << (const FColorPair&) noexcept -> FVTerm&;

    // Accessors
    auto  getClassName() const -> FString override;
    static auto  getFOutput() -> std::shared_ptr<FOutput>;
    auto  getVWin() noexcept -> FTermArea*&;
    auto  getVWin() const noexcept -> const FTermArea*;
    auto  getPrintCursor() -> FPoint;
    static auto  getWindowList() -> FVTermList*;

    // Mutators
    void  setTerminalUpdates (TerminalUpdate) const;
    void  setCursor (const FPoint&) noexcept;
    static void  setNonBlockingRead (bool = true);
    static void  unsetNonBlockingRead();

    // Inquiries
    static auto  isDrawingFinished() noexcept -> bool;
    static auto  isTerminalUpdateForced() noexcept -> bool;
    static auto  areTerminalUpdatesPaused() noexcept -> bool;
    static auto  hasPendingTerminalUpdates() noexcept -> bool;

    // Methods
    virtual void clearArea (wchar_t = L' ');
    void  createVTerm (const FSize&) noexcept;
    void  resizeVTerm (const FSize&) const noexcept;
    void  putVTerm() const;
    auto  updateTerminal() const -> bool;
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
    auto  print (const FVTermBuffer&) noexcept -> int;
    auto  print (FTermArea*, const FVTermBuffer&) noexcept -> int;
    auto  print (wchar_t) noexcept -> int;
    auto  print (FTermArea*, wchar_t) noexcept -> int;
    auto  print (const FChar&) noexcept -> int;
    auto  print (FTermArea*, const FChar&) noexcept -> int;
    virtual void print (const FPoint&);
    auto  print() & -> FVTerm&;
    void  flush() const;

  protected:
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
    void  createArea ( const FRect&
                     , const FSize&
                     , FTermArea*& );
    void  createArea ( const FRect&, FTermArea*&);
    void  resizeArea ( const FRect&
                     , const FSize&
                     , FTermArea* ) const;
    void  resizeArea (const FRect&, FTermArea*) const;
    static void  removeArea (FTermArea*&);
    static void  restoreVTerm (const FRect&) noexcept;
    auto  updateVTermCursor (const FTermArea*) const noexcept -> bool;
    void  hideVTermCursor() const;
    static void  setAreaCursor ( const FPoint&
                               , bool, FTermArea* ) noexcept;
    static void  getArea (const FPoint&, FTermArea*) noexcept;
    static void  getArea (const FRect&, FTermArea*) noexcept;
    void  putArea (FTermArea*) const noexcept;
    static void  putArea (const FPoint&, const FTermArea*) noexcept;
    static void  copyArea (FTermArea*, const FPoint&, const FTermArea*) noexcept;
    static auto  getLayer (FVTerm&) noexcept -> int;
    static void  determineWindowLayers() noexcept;
    void  scrollAreaForward (FTermArea*) const;
    void  scrollAreaReverse (FTermArea*) const;
    void  clearArea (FTermArea*, wchar_t = L' ') const noexcept;
    void  forceTerminalUpdate() const;
    auto  processTerminalUpdate() const -> bool;
    static void  startDrawing() noexcept;
    static void  finishDrawing() noexcept;
    virtual void initTerminal();

  private:
    // Constants
    static constexpr int DEFAULT_MINIMIZED_HEIGHT = 1;

    // Enumerations
    enum class CharacterType
    {
      Overlapped,
      Covered
    };

    // Methods
    void  resetTextAreaToDefault ( FTermArea*
                                 , const FSize&) const noexcept;
    static auto  reallocateTextArea ( FTermArea*
                                    , std::size_t
                                    , std::size_t ) -> bool;
    static auto  reallocateTextArea ( FTermArea*
                                    , std::size_t ) -> bool;
    static auto isCovered (const FPoint&, const FTermArea*) noexcept -> CoveredState;
    static constexpr auto  getFullAreaWidth (const FTermArea*) noexcept -> int;
    static constexpr auto  getFullAreaHeight (const FTermArea*) noexcept -> int;
    static void updateOverlappedColor (const FChar&, const FChar&, FChar&) noexcept;
    static void updateOverlappedCharacter (FChar&, FChar&) noexcept;
    static void updateShadedCharacter (const FChar&, FChar&, FChar&) noexcept;
    static void updateInheritBackground (const FChar&, const FChar&, FChar&) noexcept;
    static void updateCharacter (const FChar&, FChar&) noexcept;
    static auto updateVTermCharacter ( const FTermArea*
                                     , const FPoint&
                                     , const FPoint& ) noexcept -> bool;
    void        updateVTerm() const;
    void        scrollTerminalForward() const;
    void        scrollTerminalReverse() const;
    static void callPreprocessingHandler (const FTermArea*);
    auto        hasChildAreaChanges (const FTermArea*) const -> bool;
    void        clearChildAreaChanges (const FTermArea*) const;
    static auto isInsideArea (const FPoint&, const FTermArea*) -> bool;
    static auto isTransparentInvisible (const FChar&) -> bool;
    static auto generateCharacter (const FPoint&) -> FChar;
    static auto getCharacter ( CharacterType
                             , const FPoint&
                             , const FTermArea* ) -> FChar;
    static auto getCoveredCharacter (const FPoint&, const FTermArea*) -> FChar;
    static auto getOverlappedCharacter (const FPoint&, const FTermArea*) -> FChar;
    template <typename FOutputType>
    void        init();
    void        initSettings();
    void        finish() const;
    static void putAreaLine (const FChar&, FChar&, std::size_t);
    static void putAreaCharacter ( const FPoint&, const FTermArea*
                                 , const FChar&, FChar& );
    static void getAreaCharacter (const FPoint&, FTermArea*, FChar*&);
    auto        clearFullArea (FTermArea*, FChar&) const -> bool;
    static void clearAreaWithShadow (FTermArea*, const FChar&) noexcept;
    auto        printWrap (FTermArea*) const -> bool;
    static auto getByte1TransMask() -> uInt8;
    auto        changedToTransparency (const FChar&, const FChar&) const -> bool;
    auto        changedFromTransparency (const FChar&, const FChar&) const -> bool;
    auto        printCharacterOnCoordinate ( FTermArea*
                                           , const FChar&) const noexcept -> std::size_t;
    void        printPaddingCharacter (FTermArea*, const FChar&);
    auto        isInsideTerminal (const FPoint&) const noexcept -> bool;
    static auto hasPendingUpdates (const FTermArea*) noexcept -> bool;

    // Data members
    FTermArea*                   print_area{nullptr};        // print area for this object
    FTermArea*                   child_print_area{nullptr};  // print area for children
    FTermArea*                   vwin{nullptr};              // virtual window
    std::shared_ptr<FOutput>     foutput{};                  // Terminal output class
    std::shared_ptr<FVTermList>  window_list{};              // List of all window owner
    static const FVTerm*         init_object;                // Global FVTerm object
    static FTermArea*            vterm;                      // virtual terminal
    static FTermArea*            vdesktop;                   // virtual desktop
    static FTermArea*            active_area;                // active area
    static FChar                 s_ch;                       // shadow character
    static FChar                 i_ch;                       // inherit background character
    static uInt8                 b1_trans_mask;              // Transparency mask
    static int                   tabstop;
    static bool                  draw_completed;
    static bool                  no_terminal_updates;
    static bool                  force_terminal_update;

    // friend function
    friend void setPrintArea (FWidget&, FTermArea*);
};


//----------------------------------------------------------------------
// struct FVTerm::FTermArea
//----------------------------------------------------------------------

struct FVTerm::FTermArea  // define virtual terminal character properties
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

  auto contains (const FPoint& pos) const noexcept -> bool;
  auto checkPrintPos() const noexcept -> bool;

  inline auto getFChar (int x, int y) const noexcept -> const FChar&
  {
    return data[y * (width + right_shadow) + x];
  }

  inline auto getFChar (int x, int y) noexcept -> FChar&
  {
    return data[y * (width + right_shadow) + x];
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
    cursor_x = x;
    cursor_y = y;
  }

  inline void setInputCursorPos (int x, int y) noexcept
  {
    input_cursor_x = x;
    input_cursor_y = y;
  }

  template <typename T>
  inline auto print (T&& term_data) -> int
  {
    if ( hasOwner() )
      return getOwner<FVTerm*>()->print (this, std::forward<T>(term_data));

    return -1;
  }

  // Data members
  int             offset_left{0};      // Distance from left terminal side
  int             offset_top{0};       // Distance from top of the terminal
  int             width{-1};           // Window width
  int             height{-1};          // Window height
  int             min_width{-1};       // Minimized window width
  int             min_height{-1};      // Minimized window height
  int             right_shadow{0};     // Right window shadow
  int             bottom_shadow{0};    // Bottom window shadow
  int             cursor_x{0};         // X-position for the next write operation
  int             cursor_y{0};         // Y-position for the next write operation
  int             input_cursor_x{-1};  // X-position input cursor
  int             input_cursor_y{-1};  // Y-position input cursor
  int             layer{-1};
  bool            input_cursor_visible{false};
  bool            has_changes{false};
  bool            visible{false};
  bool            minimized{false};
  FDataAccessPtr  owner{nullptr};      // Object that owns this FTermArea
  FPreprocessing  preproc_list{};
  FLineChangesPtr changes{};
  FCharPtr        data{};              // FChar data of the drawing area
};

//----------------------------------------------------------------------
inline auto FVTerm::FTermArea::contains (const FPoint& pos) const noexcept -> bool
{
  // Is the terminal position (pos) located on my area?

  const int area_height = minimized ? min_height : height + bottom_shadow;
  const int x = pos.getX();
  const int y = pos.getY();
  return x >= offset_left
      && x <= offset_left + width + right_shadow - 1
      && y >= offset_top
      && y <= offset_top + area_height - 1;
}

//----------------------------------------------------------------------
inline auto FVTerm::FTermArea::checkPrintPos() const noexcept -> bool
{
  return cursor_x > 0
      && cursor_y > 0
      && cursor_x <= width + right_shadow
      && cursor_y <= height + bottom_shadow;
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
inline auto FVTerm::getVWin() noexcept -> FTermArea*&
{ return vwin; }

//----------------------------------------------------------------------
inline auto FVTerm::getVWin() const noexcept -> const FTermArea*
{ return vwin; }

//----------------------------------------------------------------------
inline auto FVTerm::getWindowList() -> FVTermList*
{
  return (init_object && init_object->window_list)
        ? init_object->window_list.get()
        : nullptr;
}

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
{ return hasPendingUpdates(vterm); }

//----------------------------------------------------------------------
template <typename... Args>
inline auto FVTerm::printf (const FString& format, Args&&... args) noexcept -> int
{
  FString str{};
  str.sprintf (format, std::forward<Args>(args)...);
  return print(str);
}

//----------------------------------------------------------------------
inline auto FVTerm::print() & -> FVTerm&
{ return *this; }

//----------------------------------------------------------------------
inline auto FVTerm::getChildPrintArea() const -> FTermArea*
{ return child_print_area; }

//----------------------------------------------------------------------
inline auto FVTerm::getCurrentPrintArea() const -> FTermArea*
{ return print_area; }

//----------------------------------------------------------------------
inline auto FVTerm::getVirtualDesktop() const -> FTermArea*
{ return vdesktop; }

//----------------------------------------------------------------------
inline auto FVTerm::getVirtualTerminal() const -> FTermArea*
{ return vterm; }

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
{ return vwin; }

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
  if ( ! init_object )
  {
    init_object   = this;
    vterm         = nullptr;
    vdesktop      = nullptr;
    b1_trans_mask = getByte1TransMask();
    foutput       = std::make_shared<FOutputType>(*this);
    window_list   = std::make_shared<FVTermList>();
    initSettings();
  }
  else
  {
    foutput = std::shared_ptr<FOutput>(init_object->foutput);
    window_list = std::shared_ptr<FVTermList>(init_object->window_list);
  }
}

}  // namespace finalcut

#endif  // FVTERM_H
