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
#include <queue>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "final/fc.h"
#include "final/output/tty/fterm_functions.h"
#include "final/util/fdata.h"
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
class FPoint;
class FRect;
class FSize;
class FString;
class FStyle;
class FVTermBuffer;

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
    FVTerm& operator = (const FVTerm&) = delete;

    // Disable move assignment operator (=)
    FVTerm& operator = (FVTerm&&) noexcept = delete;

    // Destructor
    ~FVTerm() override;

    // Overloaded operators
    template <typename NumT
            , enable_if_arithmetic_without_char_t<NumT> = nullptr>
    FVTerm& operator << (const NumT&);

    template <typename CharT
            , enable_if_CString_t<CharT> = nullptr>
    FVTerm& operator << (const CharT&);
    FVTerm& operator << (char);
    FVTerm& operator << (wchar_t);
    FVTerm& operator << (const wchar_t*);
    FVTerm& operator << (const UniChar&);
    FVTerm& operator << (const std::string&);
    FVTerm& operator << (const std::wstring&);
    FVTerm& operator << (const FString&);
    FVTerm& operator << (const FVTermBuffer&);
    FVTerm& operator << (const FChar&);
    FVTerm& operator << (const FCharVector&);
    FVTerm& operator << (const FPoint&);
    FVTerm& operator << (const FStyle&);
    FVTerm& operator << (const FColorPair&);

    // Accessors
    FString               getClassName() const override;
    static auto           getFOutput() -> std::shared_ptr<FOutput>;
    FTermArea*&           getVWin();
    const FTermArea*      getVWin() const;
    FPoint                getPrintCursor();
    static FVTermList*    getWindowList();

    // Mutators
    void                  setTerminalUpdates (TerminalUpdate) const;
    void                  setCursor (const FPoint&);
    static void           setNonBlockingRead (bool = true);
    static void           unsetNonBlockingRead();

    // Inquiries
    static bool           isDrawingFinished() noexcept;
    static bool           isTerminalUpdateForced() noexcept;
    static bool           areTerminalUpdatesPaused() noexcept;
    static bool           hasPendingTerminalUpdates();

    // Methods
    virtual void          clearArea (wchar_t = L' ');
    void                  createVTerm (const FSize&);
    void                  resizeVTerm (const FSize&) const;
    void                  putVTerm() const;
    bool                  updateTerminal() const;
    virtual void          addPreprocessingHandler ( const FVTerm*
                                                  , FPreprocessingFunction&& );
    virtual void          delPreprocessingHandler (const FVTerm*);
    bool                  interpretControlCodes (FTermArea*, const FChar&) const;
    template <typename... Args>
    int                   printf (const FString&, Args&&...);
    int                   print (const FString&);
    int                   print (FTermArea*, const FString&);
    int                   print (const std::vector<FChar>&);
    int                   print (FTermArea*, const std::vector<FChar>&);
    int                   print (const FVTermBuffer&);
    int                   print (FTermArea*, const FVTermBuffer&);
    int                   print (wchar_t);
    int                   print (FTermArea*, wchar_t);
    int                   print (const FChar&);
    int                   print (FTermArea*, const FChar&);
    virtual void          print (const FPoint&);
    FVTerm&               print() &;
    void                  flush() const;

  protected:
    // Accessor
    virtual FTermArea*    getPrintArea();
    FTermArea*            getChildPrintArea() const;
    FTermArea*            getCurrentPrintArea() const;
    FTermArea*            getVirtualDesktop() const;
    FTermArea*            getVirtualTerminal() const;

    // Mutators
    void                  setPrintArea (FTermArea*);
    void                  setChildPrintArea (FTermArea*);
    void                  setActiveArea (FTermArea*) const;

    // Inquiries
    bool                  isActive (const FTermArea*) const;
    bool                  hasPrintArea() const;
    bool                  hasChildPrintArea() const;
    bool                  isVirtualWindow() const;
    bool                  isCursorHideable() const;

    // Methods
    void                  createArea ( const FRect&
                                     , const FSize&
                                     , FTermArea*& );
    void                  createArea ( const FRect&, FTermArea*&);
    void                  resizeArea ( const FRect&
                                     , const FSize&
                                     , FTermArea* ) const;
    void                  resizeArea (const FRect&, FTermArea*) const;
    static void           removeArea (FTermArea*&);
    static void           restoreVTerm (const FRect&);
    bool                  updateVTermCursor (const FTermArea*) const;
    void                  hideVTermCursor() const;
    static void           setAreaCursor ( const FPoint&
                                        , bool, FTermArea* );
    static void           getArea (const FPoint&, const FTermArea*);
    static void           getArea (const FRect&, const FTermArea*);
    void                  putArea (const FTermArea*) const;
    static void           putArea (const FPoint&, const FTermArea*);
    static void           copyArea (FTermArea*, const FPoint&, const FTermArea*);
    static int            getLayer (FVTerm&);
    static void           determineWindowLayers();
    void                  scrollAreaForward (FTermArea*) const;
    void                  scrollAreaReverse (FTermArea*) const;
    void                  clearArea (FTermArea*, wchar_t = L' ') const;
    void                  forceTerminalUpdate() const;
    bool                  processTerminalUpdate() const;
    static void           startDrawing();
    static void           finishDrawing();
    virtual void          initTerminal();

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
    void                  resetTextAreaToDefault ( const FTermArea*
                                                 , const FSize&) const;
    static bool           reallocateTextArea ( FTermArea*
                                             , std::size_t
                                             , std::size_t );
    static bool           reallocateTextArea ( FTermArea*
                                             , std::size_t );
    static CoveredState   isCovered (const FPoint&, const FTermArea*);
    static constexpr int  getFullAreaWidth (const FTermArea*);
    static constexpr int  getFullAreaHeight (const FTermArea*);
    static void           updateOverlappedColor (const FChar&, const FChar&, FChar&);
    static void           updateOverlappedCharacter (FChar&, FChar&);
    static void           updateShadedCharacter (const FChar&, FChar&, FChar&);
    static void           updateInheritBackground (const FChar&, const FChar&, FChar&);
    static void           updateCharacter (const FChar&, FChar&);
    static bool           updateVTermCharacter ( const FTermArea*
                                               , const FPoint&
                                               , const FPoint& );
    void                  updateVTerm() const;
    void                  scrollTerminalForward() const;
    void                  scrollTerminalReverse() const;
    static void           callPreprocessingHandler (const FTermArea*);
    bool                  hasChildAreaChanges (const FTermArea*) const;
    void                  clearChildAreaChanges (const FTermArea*) const;
    static bool           isInsideArea (const FPoint&, const FTermArea*);
    static bool           isTransparentInvisible (const FChar&);
    static FChar          generateCharacter (const FPoint&);
    static FChar          getCharacter ( CharacterType
                                       , const FPoint&
                                       , const FTermArea* );
    static FChar          getCoveredCharacter (const FPoint&, const FTermArea*);
    static FChar          getOverlappedCharacter (const FPoint&, const FTermArea*);
    template <typename FOutputType>
    void                  init();
    void                  initSettings();
    void                  finish() const;
    static void           putAreaLine (const FChar&, FChar&, std::size_t);
    static void           putAreaCharacter ( const FPoint&, const FTermArea*
                                           , const FChar&, FChar& );
    static void           getAreaCharacter ( const FPoint&, const FTermArea*
                                           , FChar*& );
    bool                  clearFullArea (const FTermArea*, FChar&) const;
    static void           clearAreaWithShadow (const FTermArea*, const FChar&);
    bool                  printWrap (FTermArea*) const;
    static uInt8          getByte1TransMask();
    bool                  changedToTransparency (const FChar&, const FChar&) const;
    bool                  changedFromTransparency (const FChar&, const FChar&) const;
    std::size_t           printCharacterOnCoordinate ( FTermArea*
                                                     , const FChar&) const;
    void                  printPaddingCharacter (FTermArea*, const FChar&);
    bool                  isInsideTerminal (const FPoint&) const noexcept;
    static bool           hasPendingUpdates (const FTermArea*) noexcept;

    // Data members
    FTermArea*                    print_area{nullptr};        // print area for this object
    FTermArea*                    child_print_area{nullptr};  // print area for children
    FTermArea*                    vwin{nullptr};              // virtual window
    std::shared_ptr<FOutput>      foutput{};                  // Terminal output class
    std::shared_ptr<FVTermList>   window_list{};              // List of all window owner
    static const FVTerm*          init_object;                // Global FVTerm object
    static FTermArea*             vterm;                      // virtual terminal
    static FTermArea*             vdesktop;                   // virtual desktop
    static FTermArea*             active_area;                // active area
    static FChar                  s_ch;                       // shadow character
    static FChar                  i_ch;                       // inherit background character
    static uInt8                  b1_trans_mask;              // Transparency mask
    static int                    tabstop;
    static bool                   draw_completed;
    static bool                   no_terminal_updates;
    static bool                   force_terminal_update;
};


//----------------------------------------------------------------------
// struct FVTerm::FTermArea
//----------------------------------------------------------------------

struct FVTerm::FTermArea  // define virtual terminal character properties
{
  // Using-declaration
  using FDataAccessPtr = std::shared_ptr<FDataAccess>;

  // Constructor
  FTermArea() = default;

  // Disable copy constructor
  FTermArea (const FTermArea&) = delete;

  // Destructor
  ~FTermArea() = default;

  // Disable copy assignment operator (=)
  FTermArea& operator = (const FTermArea&) = delete;

  template <typename T>
  clean_fdata_t<T>& getOwner() const
  {
    return static_cast<FData<clean_fdata_t<T>>&>(*owner).get();
  }

  template <typename T>
  void setOwner (T&& obj)
  {
    owner.reset(makeFData(std::forward<T>(obj)));
  }

  bool hasOwner() const
  {
    return owner.get() != nullptr;
  }

  bool contains (const FPoint& pos) const noexcept;
  bool checkPrintPos() const noexcept;

  void setCursorPos (int x, int y)
  {
    cursor_x = x;
    cursor_y = y;
  }

  void setInputCursorPos (int x, int y)
  {
    input_cursor_x = x;
    input_cursor_y = y;
  }

  template <typename T>
  int print (T&& term_data)
  {
    if ( hasOwner() )
      return getOwner<FVTerm*>()->print (this, std::forward<T>(term_data));

    return -1;
  }

  // Data members
  int            offset_left{0};      // Distance from left terminal side
  int            offset_top{0};       // Distance from top of the terminal
  int            width{-1};           // Window width
  int            height{-1};          // Window height
  int            min_width{-1};       // Minimized window width
  int            min_height{-1};      // Minimized window height
  int            right_shadow{0};     // Right window shadow
  int            bottom_shadow{0};    // Bottom window shadow
  int            cursor_x{0};         // X-position for the next write operation
  int            cursor_y{0};         // Y-position for the next write operation
  int            input_cursor_x{-1};  // X-position input cursor
  int            input_cursor_y{-1};  // Y-position input cursor
  int            layer{-1};
  bool           input_cursor_visible{false};
  bool           has_changes{false};
  bool           visible{false};
  bool           minimized{false};
  FDataAccessPtr owner{nullptr};      // Object that owns this FTermArea
  FPreprocessing preproc_list{};
  FLineChanges*  changes{nullptr};
  FChar*         data{nullptr};       // FChar data of the drawing area
};

struct D
{
  void operator () (const FVTerm*) const
  {
    // No deleting of pointer objects when exiting the std::unique_ptr
  }
};

//----------------------------------------------------------------------
// struct FVTerm::FVTermPreprocessing
//----------------------------------------------------------------------

struct FVTerm::FVTermPreprocessing
{
  // Constructor
  FVTermPreprocessing (const FVTerm* i, FPreprocessingFunction&& f)
    : instance(std::unique_ptr<const FVTerm, D>(i))
    , function(std::move(f))
  { }

  FVTermPreprocessing (const FVTermPreprocessing&) = delete;
  FVTermPreprocessing (FVTermPreprocessing&&) noexcept = default;
  FVTermPreprocessing& operator = (const FVTermPreprocessing&) = delete;
  FVTermPreprocessing& operator = (FVTermPreprocessing&&) noexcept = default;

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
inline FVTerm& FVTerm::operator << (const NumT& n)
{
  print (FString(std::to_string(n)));
  return *this;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_CString_t<CharT>>
inline FVTerm& FVTerm::operator << (const CharT& s)
{
  print (FString(s));
  return *this;
}

//----------------------------------------------------------------------
inline FVTerm& FVTerm::operator << (char c)
{
  print (wchar_t(uChar(c)));
  return *this;
}

//----------------------------------------------------------------------
inline FVTerm& FVTerm::operator << (wchar_t c)
{
  print (c);
  return *this;
}

//----------------------------------------------------------------------
inline FVTerm& FVTerm::operator << (const wchar_t* wide_string)
{
  print (FString(wide_string));
  return *this;
}

//----------------------------------------------------------------------
inline FVTerm& FVTerm::operator << (const UniChar& c)
{
  print (static_cast<wchar_t>(c));  // Required under Solaris
  return *this;
}

//----------------------------------------------------------------------
inline FVTerm& FVTerm::operator << (const std::string& string)
{
  print (FString(string));
  return *this;
}

//----------------------------------------------------------------------
inline FVTerm& FVTerm::operator << (const std::wstring& wide_string)
{
  print (FString(wide_string));
  return *this;
}

//----------------------------------------------------------------------
inline FVTerm& FVTerm::operator << (const FString& fstring)
{
  print (fstring);
  return *this;
}

//----------------------------------------------------------------------
inline FVTerm& FVTerm::operator << (const FChar& fchar)
{
  print (fchar);
  return *this;
}

//----------------------------------------------------------------------
inline FVTerm& FVTerm::operator << (const FCharVector& term_string)
{
  print (term_string);
  return *this;
}

//----------------------------------------------------------------------
inline FVTerm& FVTerm::operator << (const FPoint& pos)
{
  print (pos);
  return *this;
}

//----------------------------------------------------------------------
inline FVTerm& FVTerm::operator << (const FStyle& t_style)
{
  print (t_style);
  return *this;
}

//----------------------------------------------------------------------
inline FVTerm& FVTerm::operator << (const FColorPair& pair)
{
  print (pair);
  return *this;
}

//----------------------------------------------------------------------
inline FString FVTerm::getClassName() const
{ return "FVTerm"; }

//----------------------------------------------------------------------
inline FVTerm::FTermArea*& FVTerm::getVWin()
{ return vwin; }

//----------------------------------------------------------------------
inline const FVTerm::FTermArea* FVTerm::getVWin() const
{ return vwin; }

//----------------------------------------------------------------------
inline FVTerm::FVTermList* FVTerm::getWindowList()
{
  return (init_object && init_object->window_list)
        ? init_object->window_list.get()
        : nullptr;
}

//----------------------------------------------------------------------
inline void FVTerm::unsetNonBlockingRead()
{ setNonBlockingRead(false); }

//----------------------------------------------------------------------
inline bool FVTerm::isDrawingFinished() noexcept
{ return draw_completed; }

//----------------------------------------------------------------------
inline bool FVTerm::isTerminalUpdateForced() noexcept
{ return force_terminal_update; }

//----------------------------------------------------------------------
inline bool FVTerm::areTerminalUpdatesPaused() noexcept
{ return no_terminal_updates; }

//----------------------------------------------------------------------
inline bool FVTerm::hasPendingTerminalUpdates()
{ return hasPendingUpdates(vterm); }

//----------------------------------------------------------------------
template <typename... Args>
inline int FVTerm::printf (const FString& format, Args&&... args)
{
  FString str{};
  str.sprintf (format, std::forward<Args>(args)...);
  return print(str);
}

//----------------------------------------------------------------------
inline FVTerm& FVTerm::print() &
{ return *this; }

//----------------------------------------------------------------------
inline FVTerm::FTermArea* FVTerm::getChildPrintArea() const
{ return child_print_area; }

//----------------------------------------------------------------------
inline FVTerm::FTermArea* FVTerm::getCurrentPrintArea() const
{ return print_area; }

//----------------------------------------------------------------------
inline FVTerm::FTermArea* FVTerm::getVirtualDesktop() const
{ return vdesktop; }

//----------------------------------------------------------------------
inline FVTerm::FTermArea* FVTerm::getVirtualTerminal() const
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
inline bool FVTerm::isActive (const FTermArea* area) const
{ return area == active_area; }

//----------------------------------------------------------------------
inline bool FVTerm::hasPrintArea() const
{ return print_area; }

//----------------------------------------------------------------------
inline bool FVTerm::hasChildPrintArea() const
{ return child_print_area; }

//----------------------------------------------------------------------
inline bool FVTerm::isVirtualWindow() const
{ return vwin; }

//----------------------------------------------------------------------
inline void FVTerm::hideVTermCursor() const
{ vterm->input_cursor_visible = false; }

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
