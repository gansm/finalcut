// File: fvterm.cpp
// Provides: class FVTerm

#include "fapp.h"
#include "fvterm.h"
#include "fwidget.h"
#include "fwindow.h"

// global FVTerm object
static FVTerm* init_object = 0;

// static class attributes
bool                 FVTerm::hidden_cursor;
bool                 FVTerm::terminal_update_complete;
bool                 FVTerm::terminal_update_pending;
bool                 FVTerm::force_terminal_update;
bool                 FVTerm::stop_terminal_updates;
bool                 FVTerm::vterm_updates;
int                  FVTerm::skipped_terminal_update = 0;
std::queue<int>*     FVTerm::output_buffer           = 0;
FPoint*              FVTerm::term_pos                = 0;
FPoint*              FVTerm::cursor                  = 0;
FVTerm::term_area*   FVTerm::vterm                   = 0;
FVTerm::term_area*   FVTerm::vdesktop                = 0;
FVTerm::term_area*   FVTerm::last_area               = 0;
FVTerm::term_area*   FVTerm::active_area             = 0;
FTermcap::tcap_map*  FVTerm::tcap                    = 0;
FOptiAttr::char_data FVTerm::term_attribute;
FOptiAttr::char_data FVTerm::next_attribute;


//----------------------------------------------------------------------
// class FVTerm
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FVTerm::FVTerm (FVTerm* parent)
  : FObject(parent)
  , print_area(0)
  , vwin(0)
{
  terminal_update_complete = false;
  vterm_updates    = true;

  if ( ! parent )
  {
    init();
  }
}

//----------------------------------------------------------------------
FVTerm::~FVTerm()  // destructor
{
  if ( init_object == this )
  {
    finish();
  }
}

//----------------------------------------------------------------------
void FVTerm::init()
{
  init_object   = this;
  vterm         =  0;
  vdesktop      =  0;
  last_area     =  0;
  term_pos      = new FPoint(-1,-1);
  cursor        = new FPoint(0,0);
  output_buffer = new std::queue<int>;

  // Preset to false
  hidden_cursor           = \
  terminal_update_pending = \
  force_terminal_update   = \
  stop_terminal_updates   = false;

  // term_attribute stores the current state of the terminal
  term_attribute.code          = '\0';
  term_attribute.fg_color      = fc::Default;
  term_attribute.bg_color      = fc::Default;
  term_attribute.bold          = \
  term_attribute.dim           = \
  term_attribute.italic        = \
  term_attribute.underline     = \
  term_attribute.blink         = \
  term_attribute.reverse       = \
  term_attribute.standout      = \
  term_attribute.invisible     = \
  term_attribute.protect       = \
  term_attribute.crossed_out   = \
  term_attribute.dbl_underline = \
  term_attribute.alt_charset   = \
  term_attribute.pc_charset    = \
  term_attribute.transparent   = \
  term_attribute.trans_shadow  = \
  term_attribute.inherit_bg    = false;

  // next_attribute contains the state of the next printed character
  std::memcpy (&next_attribute, &term_attribute, sizeof(FOptiAttr::char_data));

  // receive the terminal capabilities
  tcap = FTermcap().getTermcapMap();

  // create virtual terminal
  FRect term_geometry (0, 0, getColumnNumber(), getLineNumber());
  createVTerm (term_geometry);

  // create virtual desktop area
  FPoint shadow_size(0,0);
  createArea (term_geometry, shadow_size, vdesktop);
  vdesktop->visible = true;
  active_area = vdesktop;

  // Hide the input cursor
  hideCursor();
}

//----------------------------------------------------------------------
void FVTerm::finish()
{
  // Show the input cursor
  showCursor();

  if ( output_buffer )
    delete output_buffer;

  // remove virtual terminal + virtual desktop area
  removeArea (vdesktop);
  removeArea (vterm);

  if ( cursor )
    delete cursor;

  if ( term_pos )
    delete term_pos;
}

//----------------------------------------------------------------------
FVTerm::term_area* FVTerm::getPrintArea()
{
  // returns the print area of this object
  if ( print_area )
    return print_area;
  else
  {
    FVTerm* obj = static_cast<FVTerm*>(this);
    FVTerm* p_obj = static_cast<FVTerm*>(obj->getParent());

    while ( ! obj->vwin && p_obj )
    {
      obj = p_obj;
      p_obj = static_cast<FVTerm*>(p_obj->getParent());
    }

    if ( obj->vwin )
    {
      print_area = obj->vwin;
      return print_area;
    }
  }

  return vdesktop;
}

//----------------------------------------------------------------------
void FVTerm::createArea ( const FRect& r
                        , const FPoint& p
                        , FVTerm::term_area*& area )
{
  createArea ( r.getX()
             , r.getY()
             , r.getWidth()
             , r.getHeight()
             , p.getX()
             , p.getY()
             , area );
}

//----------------------------------------------------------------------
void FVTerm::createArea ( int x_offset, int y_offset
                        , int width, int height
                        , int rsw, int bsh
                        , term_area*& area )
{
  // initialize virtual window
  area = new term_area;

  area->x_offset             = 0;
  area->y_offset             = 0;
  area->width                = -1;
  area->height               = -1;
  area->right_shadow         = 0;
  area->bottom_shadow        = 0;
  area->input_cursor_x       = -1;
  area->input_cursor_y       = -1;
  area->input_cursor_visible = false;
  area->changes              = 0;
  area->text                 = 0;
  area->visible              = false;
  area->widget               = static_cast<FWidget*>(this);

  resizeArea (x_offset, y_offset, width, height, rsw, bsh, area);
}

//----------------------------------------------------------------------
void FVTerm::resizeArea ( const FRect& r
                        , const FPoint& p
                        , FVTerm::term_area* area )
{
  resizeArea ( r.getX()
             , r.getY()
             , r.getWidth()
             , r.getHeight()
             , p.getX()
             , p.getY()
             , area );
}

//----------------------------------------------------------------------
void FVTerm::resizeArea ( int x_offset, int y_offset
                        , int width, int height
                        , int rsw, int bsh
                        , term_area* area )
{
  int area_size;
  FOptiAttr::char_data default_char;
  line_changes unchanged;

  if ( ! area )
    return;

  area_size = (width+rsw) * (height+bsh);

  if ( area->height + area->bottom_shadow != height + bsh )
  {
    if ( area->changes != 0 )
      delete[] area->changes;

    if ( area->text != 0 )
      delete[] area->text;

    area->changes = new line_changes[height + bsh];
    area->text    = new FOptiAttr::char_data[area_size];
  }
  else if ( area->width + area->right_shadow != width + rsw )
  {
    if ( area->text != 0 )
      delete[] area->text;

    area->text = new FOptiAttr::char_data[area_size];
  }
  else
    return;

  area->x_offset = x_offset;
  area->y_offset = y_offset;
  area->width = width;
  area->height = height;
  area->right_shadow = rsw;
  area->bottom_shadow = bsh;

  default_char.code          = ' ';
  default_char.fg_color      = fc::Default;
  default_char.bg_color      = fc::Default;
  default_char.bold          = 0;
  default_char.dim           = 0;
  default_char.italic        = 0;
  default_char.underline     = 0;
  default_char.blink         = 0;
  default_char.reverse       = 0;
  default_char.standout      = 0;
  default_char.invisible     = 0;
  default_char.protect       = 0;
  default_char.crossed_out   = 0;
  default_char.dbl_underline = 0;
  default_char.alt_charset   = 0;
  default_char.pc_charset    = 0;
  default_char.transparent   = 0;
  default_char.trans_shadow  = 0;
  default_char.inherit_bg    = 0;

  std::fill_n (area->text, area_size, default_char);

  unchanged.xmin = uInt(width+rsw);
  unchanged.xmax = 0;
  unchanged.trans_count = 0;

  std::fill_n (area->changes, height+bsh, unchanged);
}

//----------------------------------------------------------------------
void FVTerm::removeArea (term_area*& area)
{
  // remove the virtual window
  if ( area != 0 )
  {
    if ( area->changes != 0 )
    {
      delete[] area->changes;
      area->changes = 0;
    }

    if ( area->text != 0 )
    {
      delete[] area->text;
      area->text = 0;
    }

    delete area;
    area = 0;
  }
}

//----------------------------------------------------------------------
void FVTerm::restoreVTerm (const FRect& box)
{
  restoreVTerm ( box.getX()
               , box.getY()
               , box.getWidth()
               , box.getHeight() );
}

//----------------------------------------------------------------------
void FVTerm::restoreVTerm (int x, int y, int w, int h)
{
  FOptiAttr::char_data* tc;   // terminal character
  FOptiAttr::char_data* sc;   // shown character
  FOptiAttr::char_data  s_ch; // shadow character
  FOptiAttr::char_data  i_ch; // inherit background character
  FWidget* widget;

  x--;
  y--;

  if ( x < 0 )
    x = 0;

  if ( y < 0 )
    y = 0;

  if ( w < 0 || h < 0 )
    return;

  if ( x+w > vterm->width )
    w = vterm->width - x;

  if ( w < 0 )
    return;

  if ( y+h > vterm->height )
    h = vterm->height - y;

  if ( h < 0 )
    return;

  widget = static_cast<FWidget*>(vterm->widget);

  for (register int ty=0; ty < h; ty++)
  {
    for (register int tx=0; tx < w; tx++)
    {
      tc = &vterm->text[(y+ty) * vterm->width + (x+tx)];
      sc = &vdesktop->text[(y+ty) * vdesktop->width + (x+tx)];

      if ( widget->window_list && ! widget->window_list->empty() )
      {
        FWidget::widgetList::const_iterator iter, end;
        iter = widget->window_list->begin();
        end  = widget->window_list->end();

        while ( iter != end )
        {
          term_area* win = (*iter)->getVWin();
          int win_x = win->x_offset;
          int win_y = win->y_offset;
          FRect geometry ( win_x
                         , win_y
                         , win->width + win->right_shadow
                         , win->height + win->bottom_shadow );

          // window visible and contains current character
          if ( win && win->visible && geometry.contains(tx+x, ty+y) )
          {
            FOptiAttr::char_data* tmp;
            int line_len = win->width + win->right_shadow;
            tmp = &win->text[(ty+y-win_y) * line_len + (tx+x-win_x)];

            if ( ! tmp->transparent )   // current character not transparent
            {
              if ( tmp->trans_shadow )  // transparent shadow
              {
                // keep the current vterm character
                std::memcpy (&s_ch, sc, sizeof(FOptiAttr::char_data));
                s_ch.fg_color = tmp->fg_color;
                s_ch.bg_color = tmp->bg_color;
                s_ch.reverse  = false;
                s_ch.standout = false;

                if (  s_ch.code == fc::LowerHalfBlock
                   || s_ch.code == fc::UpperHalfBlock
                   || s_ch.code == fc::LeftHalfBlock
                   || s_ch.code == fc::RightHalfBlock
                   || s_ch.code == fc::FullBlock )
                  s_ch.code = ' ';

                sc = &s_ch;
              }
              else if ( tmp->inherit_bg )
              {
                // add the covered background to this character
                std::memcpy (&i_ch, tmp, sizeof(FOptiAttr::char_data));
                i_ch.bg_color = sc->bg_color;  // last background color;
                sc = &i_ch;
              }
              else  // default
                sc = tmp;
            }
          }

          ++iter;
        }
      }

      std::memcpy (tc, sc, sizeof(FOptiAttr::char_data));

      if ( short(vterm->changes[y+ty].xmin) > x )
        vterm->changes[y+ty].xmin = uInt(x);

      if ( short(vterm->changes[y+ty].xmax) < x+w-1 )
        vterm->changes[y+ty].xmax = uInt(x+w-1);
    }
  }
}

//----------------------------------------------------------------------
FVTerm::covered_state FVTerm::isCovered ( const FPoint& pos
                                        , FVTerm::term_area* area )
{
  return isCovered (pos.getX(), pos.getY(), area);
}

//----------------------------------------------------------------------
FVTerm::covered_state FVTerm::isCovered ( int x, int y
                                        , FVTerm::term_area* area )
{
  bool found;
  FVTerm::covered_state is_covered;
  FWidget* w;

  if ( ! area )
    return non_covered;

  is_covered = non_covered;
  found = bool(area == vdesktop);

  w = static_cast<FWidget*>(area->widget);

  if ( w->window_list && ! w->window_list->empty() )
  {
    FWidget::widgetList::const_iterator iter, end;
    iter = w->window_list->begin();
    end  = w->window_list->end();

    while ( iter != end )
    {
      term_area* win = (*iter)->getVWin();
      int win_x = win->x_offset;
      int win_y = win->y_offset;
      FRect geometry ( win_x
                     , win_y
                     , win->width + win->right_shadow
                     , win->height + win->bottom_shadow );

      if (  win && found
         && win->visible
         && geometry.contains(x,y) )
      {
        FOptiAttr::char_data* tmp;
        int line_len = win->width + win->right_shadow;
        tmp = &win->text[(y-win_y) * line_len + (x-win_x)];

        if ( tmp->trans_shadow )
        {
          is_covered = half_covered;
        }
        else if ( ! tmp->transparent )
        {
          is_covered = fully_covered;
          break;
        }
      }

      if ( area == win )
        found = true;

      ++iter;
    }
  }

  return is_covered;
}

//----------------------------------------------------------------------
void FVTerm::updateVTerm (bool on)
{
  vterm_updates = on;

  if ( on )
    updateVTerm (last_area);
}

//----------------------------------------------------------------------
void FVTerm::updateVTerm (FVTerm::term_area* area)
{
  int ax, ay, aw, ah, rsh, bsh, y_end, ol;
  FOptiAttr::char_data* tc; // terminal character
  FOptiAttr::char_data* ac; // area character
  bool modified = false;

  if ( ! vterm_updates )
  {
    last_area = area;
    return;
  }

  if ( ! area )
    return;

  if ( ! area->visible )
    return;

  ax  = area->x_offset;
  ay  = area->y_offset;
  aw  = area->width;
  ah  = area->height;
  rsh = area->right_shadow;
  bsh = area->bottom_shadow;
  ol  = 0;  // outside left

  if ( ax < 0 )
  {
    ol = std::abs(ax);
    ax = 0;
  }

  if ( ah + bsh + ay > vterm->height )
    y_end = vterm->height - ay;
  else
    y_end = ah + bsh;

  for (register int y=0; y < y_end; y++)  // line loop
  {
    int line_xmin = int(area->changes[y].xmin);
    int line_xmax = int(area->changes[y].xmax);

    if ( line_xmin <= line_xmax )
    {
      int _xmin, _xmax;

      if ( ax == 0 )
        line_xmin = ol;

      if ( aw + rsh + ax - ol >= vterm->width )
        line_xmax = vterm->width + ol - ax - 1;

      if ( ax + line_xmin >= vterm->width )
        continue;

      for (register int x=line_xmin; x <= line_xmax; x++)  // column loop
      {
        int gx, gy, line_len;
        FVTerm::covered_state is_covered;
        // global terminal positions
        gx = ax + x;
        gy = ay + y;

        if ( gx < 0 || gy < 0 )
          continue;

        line_len = aw + rsh;
        ac = &area->text[y * line_len + x];
        tc = &vterm->text[gy * vterm->width + gx - ol];
        is_covered = isCovered(gx-ol, gy, area);  // get covered state

        if ( is_covered != fully_covered )
        {
          if ( is_covered == half_covered )
          {
            // add the overlapping color to this character
            FOptiAttr::char_data ch, oc;
            std::memcpy (&ch, ac, sizeof(FOptiAttr::char_data));
            oc = getOverlappedCharacter (gx+1 - ol, gy+1, area->widget);
            ch.fg_color = oc.fg_color;
            ch.bg_color = oc.bg_color;
            ch.reverse  = false;
            ch.standout = false;

            if (  ch.code == fc::LowerHalfBlock
               || ch.code == fc::UpperHalfBlock
               || ch.code == fc::LeftHalfBlock
               || ch.code == fc::RightHalfBlock
               || ch.code == fc::FullBlock )
              ch.code = ' ';

            std::memcpy (tc, &ch, sizeof(FOptiAttr::char_data));
          }
          else if ( ac->transparent )   // transparent
          {
            // restore one character on vterm
            FOptiAttr::char_data ch;
            ch = getCoveredCharacter (gx+1 - ol, gy+1, area->widget);
            std::memcpy (tc, &ch, sizeof(FOptiAttr::char_data));
          }
          else   // not transparent
          {
            if ( ac->trans_shadow )  // transparent shadow
            {
              // get covered character + add the current color
              FOptiAttr::char_data ch;
              ch = getCoveredCharacter (gx+1 - ol, gy+1, area->widget);
              ch.fg_color = ac->fg_color;
              ch.bg_color = ac->bg_color;
              ch.reverse  = false;
              ch.standout = false;

              if (  ch.code == fc::LowerHalfBlock
                 || ch.code == fc::UpperHalfBlock
                 || ch.code == fc::LeftHalfBlock
                 || ch.code == fc::RightHalfBlock
                 || ch.code == fc::FullBlock )
                ch.code = ' ';

              std::memcpy (tc, &ch, sizeof(FOptiAttr::char_data));
            }
            else if ( ac->inherit_bg )
            {
              // add the covered background to this character
              FOptiAttr::char_data ch, cc;
              std::memcpy (&ch, ac, sizeof(FOptiAttr::char_data));
              cc = getCoveredCharacter (gx+1 - ol, gy+1, area->widget);
              ch.bg_color = cc.bg_color;
              std::memcpy (tc, &ch, sizeof(FOptiAttr::char_data));
            }
            else  // default
              std::memcpy (tc, ac, sizeof(FOptiAttr::char_data));
          }

          modified = true;
        }
        else if ( ! modified )
          line_xmin++;  // don't update covered character
      }

      _xmin = ax + line_xmin - ol;
      _xmax = ax + line_xmax;

      if ( _xmin < short(vterm->changes[ay+y].xmin) )
        vterm->changes[ay+y].xmin = uInt(_xmin);

      if ( _xmax >= vterm->width )
        _xmax = vterm->width - 1;

      if ( _xmax > short(vterm->changes[ay+y].xmax) )
        vterm->changes[ay+y].xmax = uInt(_xmax);

      area->changes[y].xmin = uInt(aw + rsh);
      area->changes[y].xmax = 0;
    }
  }

  updateVTermCursor(area);
}

//----------------------------------------------------------------------
bool FVTerm::updateVTermCursor (FVTerm::term_area* area)
{
  if ( ! area )
    return false;

  if ( area != active_area )
    return false;

  if ( ! area->visible )
    return false;

  if ( area->input_cursor_visible )
  {
    int cx, cy, ax, ay, x, y;
    // area offset
    ax  = area->widget->getTermX() - 1;
    ay  = area->widget->getTermY() - 1;
    // area cursor position
    cx = area->input_cursor_x;
    cy = area->input_cursor_y;
    // terminal position
    x  = ax + cx;
    y  = ay + cy;

    if ( isInsideArea(cx, cy, area)
       && isInsideTerminal(x, y)
       && isCovered(x, y, area) == non_covered )
    {
      vterm->input_cursor_x = x;
      vterm->input_cursor_y = y;
      vterm->input_cursor_visible = true;
      return true;
    }
  }

  vterm->input_cursor_visible = false;
  return false;
}

//----------------------------------------------------------------------
bool FVTerm::isInsideArea (int x, int y, FVTerm::term_area* area)
{
  // Check whether the coordinates are within the area
  int ax, ay, aw, ah;
  ax  = 0;
  ay  = 0;
  aw  = area->width;
  ah  = area->height;
  FRect area_geometry(ax, ay, aw, ah);

  if ( area_geometry.contains(x,y) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
void FVTerm::setAreaCursor ( const FPoint& pos
                           , bool visible
                           , FVTerm::term_area* area )
{
  setAreaCursor (pos.getX(), pos.getY(), visible, area);
}

//----------------------------------------------------------------------
void FVTerm::setAreaCursor ( int x, int y
                           , bool visible
                           , FVTerm::term_area* area )
{
  if ( ! area )
    return;

  area->input_cursor_x = x - 1;
  area->input_cursor_y = y - 1;
  area->input_cursor_visible = visible;
  updateVTerm (area);
}

//----------------------------------------------------------------------
void FVTerm::getArea (const FPoint& pos, FVTerm::term_area* area)
{
  return getArea (pos.getX(), pos.getY(), area);
}

//----------------------------------------------------------------------
void FVTerm::getArea (int ax, int ay, FVTerm::term_area* area)
{
  // Copies a block from the virtual terminal position to the given area
  int y_end;
  int length;
  FOptiAttr::char_data* tc; // terminal character
  FOptiAttr::char_data* ac; // area character

  if ( ! area )
    return;

  ax--;
  ay--;

  if ( area->height+ay > vterm->height )
    y_end = area->height - ay;
  else
    y_end = area->height;

  if ( area->width+ax > vterm->width )
    length = vterm->width - ax;
  else
    length = area->width;

  for (int y=0; y < y_end; y++)  // line loop
  {
    tc = &vterm->text[(ay+y) * vterm->width + ax];
    ac = &area->text[y * area->width];
    std::memcpy (ac, tc, sizeof(FOptiAttr::char_data) * unsigned(length));

    if ( short(area->changes[y].xmin) > 0 )
      area->changes[y].xmin = 0;

    if ( short(area->changes[y].xmax) < length-1 )
      area->changes[y].xmax = uInt(length-1);
  }
}

//----------------------------------------------------------------------
void FVTerm::getArea (const FRect& box, FVTerm::term_area* area)
{
  getArea ( box.getX()
          , box.getY()
          , box.getWidth()
          , box.getHeight()
          , area );
}

//----------------------------------------------------------------------
void FVTerm::getArea (int x, int y, int w, int h, FVTerm::term_area* area)
{
  // Copies a block from the virtual terminal rectangle to the given area
  int y_end, length, dx, dy;
  FOptiAttr::char_data* tc; // terminal character
  FOptiAttr::char_data* ac; // area character

  if ( ! area )
    return;

  dx = x - area->widget->getTermX();
  dy = y - area->widget->getTermY();

  if ( x < 0 || y < 0 )
    return;

  if ( y-1+h > vterm->height )
    y_end = vterm->height - y + 1;
  else
    y_end = h - 1;

  if ( x-1+w > vterm->width )
    length = vterm->width - x + 1;
  else
    length = w;

  if ( length < 1 )
    return;

  for (int _y=0; _y < y_end; _y++)  // line loop
  {
    int line_len = area->width + area->right_shadow;
    tc = &vterm->text[(y+_y-1) * vterm->width + x-1];
    ac = &area->text[(dy+_y) * line_len + dx];
    std::memcpy (ac, tc, sizeof(FOptiAttr::char_data) * unsigned(length));

    if ( short(area->changes[dy+_y].xmin) > dx )
      area->changes[dy+_y].xmin = uInt(dx);

    if ( short(area->changes[dy+_y].xmax) < dx+length-1 )
      area->changes[dy+_y].xmax = uInt(dx+length-1);
  }
}

//----------------------------------------------------------------------
void FVTerm::putArea (const FPoint& pos, FVTerm::term_area* area)
{
  // Copies the given area block to the virtual terminal position
  if ( ! area )
    return;

  if ( ! area->visible )
    return;

  putArea (pos.getX(), pos.getY(), area);
}

//----------------------------------------------------------------------
void FVTerm::putArea (int ax, int ay, FVTerm::term_area* area)
{
  // Copies the given area block to the virtual terminal position
  int aw, ah, rsh, bsh, y_end, length, ol;
  FOptiAttr::char_data* tc; // terminal character
  FOptiAttr::char_data* ac; // area character

  if ( ! area )
    return;

  if ( ! area->visible )
    return;

  ax--;
  ay--;
  aw   = area->width;
  ah   = area->height;
  rsh  = area->right_shadow;
  bsh  = area->bottom_shadow;
  ol   = 0;  // outside left

  if ( ax < 0 )
  {
    ol = std::abs(ax);
    ax = 0;
  }

  if ( ay + ah + bsh > vterm->height )
    y_end = vterm->height - ay;
  else
    y_end = ah + bsh;

  if ( aw + rsh - ol + ax > vterm->width )
    length = vterm->width - ax;
  else
    length = aw + rsh - ol;

  if ( length < 1 )
    return;

  for (register int y=0; y < y_end; y++)  // line loop
  {
    int line_len = aw + rsh;

    if ( area->changes[y].trans_count == 0 )
    {
      // Line has only covered characters
      tc = &vterm->text[(ay+y) * vterm->width + ax];
      ac = &area->text[y * line_len + ol];
      std::memcpy (tc, ac, sizeof(FOptiAttr::char_data) * unsigned(length));
    }
    else
    {
      // Line has one or more transparent characters
      for (register int x=0; x < length; x++)  // column loop
      {
        tc = &vterm->text[(ay+y) * vterm->width + (ax+x)];
        ac = &area->text[y * line_len + ol + x];

        if ( ac->transparent )  // transparent
        {
          // restore one character on vterm
          FOptiAttr::char_data ch;
          ch = getCoveredCharacter (ax+x+1, ay+y+1, area->widget);
          std::memcpy (tc, &ch, sizeof(FOptiAttr::char_data));
        }
        else  // not transparent
        {
          if ( ac->trans_shadow )  // transparent shadow
          {
            // get covered character + add the current color
            FOptiAttr::char_data ch;
            ch = getCoveredCharacter (ax+x+1, ay+y+1, area->widget);
            ch.fg_color = ac->fg_color;
            ch.bg_color = ac->bg_color;
            ch.reverse  = false;
            ch.standout = false;

            if (  ch.code == fc::LowerHalfBlock
               || ch.code == fc::UpperHalfBlock
               || ch.code == fc::LeftHalfBlock
               || ch.code == fc::RightHalfBlock
               || ch.code == fc::FullBlock )
              ch.code = ' ';

            std::memcpy (tc, &ch, sizeof(FOptiAttr::char_data));
          }
          else if ( ac->inherit_bg )
          {
            // add the covered background to this character
            FOptiAttr::char_data ch, cc;
            std::memcpy (&ch, ac, sizeof(FOptiAttr::char_data));
            cc = getCoveredCharacter (ax+x+1, ay+y+1, area->widget);
            ch.bg_color = cc.bg_color;
            std::memcpy (tc, &ch, sizeof(FOptiAttr::char_data));
          }
          else  // default
            std::memcpy (tc, ac, sizeof(FOptiAttr::char_data));
        }
      }
    }

    if ( ax < short(vterm->changes[ay+y].xmin) )
      vterm->changes[ay+y].xmin = uInt(ax);

    if ( ax+length-1 > short(vterm->changes[ay+y].xmax) )
      vterm->changes[ay+y].xmax = uInt(ax+length-1);
  }
}

//----------------------------------------------------------------------
void FVTerm::scrollAreaForward (FVTerm::term_area* area)
{
  int total_width;
  int length;
  int y_max;
  FOptiAttr::char_data nc;  // next character
  FOptiAttr::char_data* lc; // last character
  FOptiAttr::char_data* sc; // source character
  FOptiAttr::char_data* dc; // destination character

  if ( ! area )
    return;

  if ( area->height <= 1 )
    return;

  length = area->width;
  total_width = area->width + area->right_shadow;
  y_max = area->height - 1;

  for (int y=0; y < y_max; y++)
  {
    int pos1 = y * total_width;
    int pos2 = (y+1) * total_width;
    sc = &area->text[pos2];
    dc = &area->text[pos1];
    std::memcpy (dc, sc, sizeof(FOptiAttr::char_data) * unsigned(length));
    area->changes[y].xmin = 0;
    area->changes[y].xmax = uInt(area->width - 1);
  }

  // insert a new line below
  lc = &area->text[(y_max * total_width) - area->right_shadow - 1];
  std::memcpy (&nc, lc, sizeof(FOptiAttr::char_data));
  nc.code = ' ';
  dc = &area->text[y_max * total_width];
  std::fill_n (dc, area->width, nc);
  area->changes[y_max].xmin = 0;
  area->changes[y_max].xmax = uInt(area->width - 1);
}

//----------------------------------------------------------------------
void FVTerm::scrollAreaReverse (FVTerm::term_area* area)
{
  int total_width;
  int length;
  FOptiAttr::char_data nc;  // next character
  FOptiAttr::char_data* lc; // last character
  FOptiAttr::char_data* sc; // source character
  FOptiAttr::char_data* dc; // destination character

  if ( ! area )
    return;

  if ( area->height <= 1 )
    return;

  length = area->width;
  total_width = area->width + area->right_shadow;

  for (int y=area->height-1; y > 0; y--)
  {
    int pos1 = (y-1) * total_width;
    int pos2 = y * total_width;
    sc = &area->text[pos1];
    dc = &area->text[pos2];
    std::memcpy (dc, sc, sizeof(FOptiAttr::char_data) * unsigned(length));
    area->changes[y].xmin = 0;
    area->changes[y].xmax = uInt(area->width - 1);
  }

  // insert a new line above
  lc = &area->text[total_width];
  std::memcpy (&nc, lc, sizeof(FOptiAttr::char_data));
  nc.code = ' ';
  dc = &area->text[0];
  std::fill_n (dc, area->width, nc);
  area->changes[0].xmin = 0;
  area->changes[0].xmax = uInt(area->width - 1);
}

//----------------------------------------------------------------------
void FVTerm::clearArea (FVTerm::term_area* area)
{
  FOptiAttr::char_data nc;  // next character
  int  total_width;
  uInt w;

  // clear with the current attributes and space characters
  std::memcpy (&nc, &next_attribute, sizeof(FOptiAttr::char_data));
  nc.code = ' ';

  if ( ! area )
    return;

  total_width = area->width + area->right_shadow;
  w = uInt(total_width);

  if ( area->right_shadow == 0 )
  {
    int area_size = area->width * area->height;
    std::fill_n (area->text, area_size, nc);
  }
  else
  {
    FOptiAttr::char_data t_char = nc;
    t_char.transparent = true;

    for (int y=0; y < area->height; y++)
    {
      int pos = y * total_width;
      std::fill_n (&area->text[pos], total_width, nc);
      std::fill_n (&area->text[pos+area->width], area->right_shadow, t_char);
    }

    for (int y=0; y < area->bottom_shadow; y++)
    {
      int pos = total_width * (y + area->height);
      std::fill_n (&area->text[pos], total_width, t_char);
    }
  }

  for (int i=0; i < area->height; i++)
  {

    area->changes[i].xmin = 0;
    area->changes[i].xmax = w - 1;

    if (  nc.transparent
       || nc.trans_shadow
       || nc.inherit_bg )
      area->changes[i].trans_count = w;
    else if ( area->right_shadow != 0 )
      area->changes[i].trans_count = uInt(area->right_shadow);
    else
      area->changes[i].trans_count = 0;
  }

  for (int i=0; i < area->bottom_shadow; i++)
  {
    int y = area->height + i;
    area->changes[y].xmin = 0;
    area->changes[y].xmax = w - 1;
    area->changes[y].trans_count = w;
  }

  updateVTerm (area);
}

//----------------------------------------------------------------------
FOptiAttr::char_data FVTerm::getCharacter ( character_type type
                                          , const FPoint& pos
                                          , FVTerm* obj )
{
  return getCharacter (type, pos.getX(), pos.getY(), obj);
}

//----------------------------------------------------------------------
FOptiAttr::char_data FVTerm::getCharacter ( character_type char_type
                                          , int x
                                          , int y
                                          , FVTerm* obj )
{
  // get the overlapped or the covered character for a position
  int xx,yy;
  FOptiAttr::char_data* cc;   // covered character
  FOptiAttr::char_data  s_ch; // shadow character
  FOptiAttr::char_data  i_ch; // inherit background character
  FWidget* w;

  x--;
  y--;
  xx = x;
  yy = y;

  if ( xx < 0 )
    xx = 0;

  if ( yy < 0 )
    yy = 0;

  if ( xx >= vterm->width )
    xx = vterm->width - 1;

  if ( yy >= vterm->height )
    yy = vterm->height - 1;

  cc = &vdesktop->text[yy * vdesktop->width + xx];
  w = static_cast<FWidget*>(obj);

  if ( w->window_list && ! w->window_list->empty() )
  {
    FWidget::widgetList::const_iterator iter, end;
    // get the window layer of this object
    int layer = FWindow::getWindowLayer(w);
    iter = w->window_list->begin();
    end  = w->window_list->end();

    while ( iter != end )
    {
      bool significant_char;

      // char_type can be "overlapped_character"
      // or "covered_character"
      if ( char_type == covered_character )
        significant_char = bool(layer >= FWindow::getWindowLayer(*iter));
      else
        significant_char = bool(layer < FWindow::getWindowLayer(*iter));

      if ( obj && *iter != obj && significant_char )
      {
        term_area* win = (*iter)->getVWin();
        int win_x = win->x_offset;
        int win_y = win->y_offset;
        FRect geometry ( win_x
                       , win_y
                       , win->width + win->right_shadow
                       , win->height + win->bottom_shadow );

        // window visible and contains current character
        if ( win && win->visible && geometry.contains(x,y) )
        {
          FOptiAttr::char_data* tmp;
          int line_len = win->width + win->right_shadow;
          tmp = &win->text[(y-win_y) * line_len + (x-win_x)];

          // current character not transparent
          if ( ! tmp->transparent )
          {
            if ( tmp->trans_shadow )  // transparent shadow
            {
              // keep the current vterm character
              std::memcpy (&s_ch, cc, sizeof(FOptiAttr::char_data));
              s_ch.fg_color = tmp->fg_color;
              s_ch.bg_color = tmp->bg_color;
              s_ch.reverse  = false;
              s_ch.standout = false;
              cc = &s_ch;
            }
            else if ( tmp->inherit_bg )
            {
              // add the covered background to this character
              std::memcpy (&i_ch, tmp, sizeof(FOptiAttr::char_data));
              i_ch.bg_color = cc->bg_color;  // last background color
              cc = &i_ch;
            }
            else  // default
              cc = tmp;
          }
        }
      }
      else if ( char_type == covered_character )
        break;

      ++iter;
    }
  }

  return *cc;
}

//----------------------------------------------------------------------
FOptiAttr::char_data FVTerm::getCoveredCharacter ( int x
                                                 , int y
                                                 , FVTerm* obj)
{
  return getCharacter (covered_character, x, y, obj);
}

//----------------------------------------------------------------------
FOptiAttr::char_data FVTerm::getCoveredCharacter ( const FPoint& pos
                                                 , FVTerm* obj )
{
  return getCharacter (covered_character, pos.getX(), pos.getY(), obj);
}

//----------------------------------------------------------------------
FOptiAttr::char_data FVTerm::getOverlappedCharacter ( int x
                                                    , int y
                                                    , FVTerm* obj)
{
  return getCharacter (overlapped_character, x, y, obj);
}

//----------------------------------------------------------------------
FOptiAttr::char_data FVTerm::getOverlappedCharacter ( const FPoint& pos
                                                    , FVTerm* obj )
{
  return getCharacter (overlapped_character, pos.getX(), pos.getY(), obj);
}

//----------------------------------------------------------------------
void FVTerm::startTerminalUpdate()
{
  terminal_update_complete = false;
}

//----------------------------------------------------------------------
void FVTerm::finishTerminalUpdate()
{
  terminal_update_complete = true;
}

//----------------------------------------------------------------------
void FVTerm::setTermXY (register int x, register int y)
{
  // sets the hardware cursor to the given (x,y) position
  int term_x, term_y, term_width, term_height;
  char* move_str;

  if ( term_pos->getX() == x && term_pos->getY() == y )
    return;

  term_width = getColumnNumber();
  term_height = getLineNumber();

  if ( x >= term_width )
  {
    y += x / term_width;
    x %= term_width;
  }

  if ( term_pos->getY() >= term_height )
    term_pos->setY(term_height - 1);

  if ( y >= term_height )
    y = term_height - 1;

  term_x = term_pos->getX();
  term_y = term_pos->getY();

  move_str = moveCursor (term_x, term_y, x, y);

  if ( move_str )
    appendOutputBuffer(move_str);

  flush_out();
  term_pos->setPoint(x,y);
}

//----------------------------------------------------------------------
bool FVTerm::hideCursor (bool on)
{
  if ( on == hidden_cursor )
    return hidden_cursor;

  if ( on )
  {
    char* hide_str = disableCursor();

    if ( hide_str )
      appendOutputBuffer (hide_str);

    hidden_cursor = true;  // global
  }
  else
  {
    char* show_str = enableCursor();

    if ( show_str )
      appendOutputBuffer (show_str);

    hidden_cursor = false;
  }

  flush_out();

  if ( ! hidden_cursor && isLinuxTerm() )
    setConsoleCursor (getConsoleCursor(), false);

  return hidden_cursor;
}

//----------------------------------------------------------------------
void FVTerm::createVTerm (const FRect& r)
{
  // initialize virtual terminal
  const FPoint shadow(0,0);
  createArea (r, shadow, vterm);
}

//----------------------------------------------------------------------
void FVTerm::createVTerm (int width, int height)
{
  // initialize virtual terminal
  createArea (0, 0, width, height, 0, 0, vterm);
}

//----------------------------------------------------------------------
void FVTerm::resizeVTerm (const FRect& r)
{
  const FPoint shadow(0,0);
  resizeArea (r, shadow, vterm);
}

//----------------------------------------------------------------------
void FVTerm::resizeVTerm (int width, int height)
{
  resizeArea (0, 0, width, height, 0, 0, vterm);
}

//----------------------------------------------------------------------
void FVTerm::putVTerm()
{
  for (int i=0; i < vterm->height; i++)
  {
    vterm->changes[i].xmin = 0;
    vterm->changes[i].xmax = uInt(vterm->width - 1);
  }

  updateTerminal();
}

//----------------------------------------------------------------------
void FVTerm::updateTerminal (bool on)
{
  stop_terminal_updates = bool(! on);

  if ( on )
    updateTerminal();
}

//----------------------------------------------------------------------
void FVTerm::updateTerminal()
{
  term_area* vt;
  int term_width, term_height;

  if ( stop_terminal_updates
     || static_cast<FApplication*>(init_object)->isQuit() )
    return;

  if ( ! force_terminal_update )
  {
    if ( ! terminal_update_complete )
      return;

    if ( isInputDataPending() )
    {
      terminal_update_pending = true;
      return;
    }
  }

  vt = vterm;
  term_width = vt->width - 1;
  term_height = vt->height - 1;

  for (register uInt y=0; y < uInt(vt->height); y++)
  {
    uInt change_xmax = vt->changes[y].xmax;

    if ( vt->changes[y].xmin <= vt->changes[y].xmax )
    {
      uInt change_xmin = vt->changes[y].xmin;
      setTermXY (int(change_xmin), int(y));

      for ( register uInt x=change_xmin;
            x <= change_xmax;
            x++ )
      {
        FOptiAttr::char_data* print_char;
        print_char = &vt->text[y * uInt(vt->width) + x];

        if ( term_pos->getX() == term_width
           && term_pos->getY() == term_height )
          appendLowerRight (print_char);
        else
          appendCharacter (print_char);

        term_pos->x_ref()++;
      }

      vt->changes[y].xmin = uInt(vt->width);
      vt->changes[y].xmax = 0;
    }

    // cursor wrap
    if ( term_pos->getX() > term_width )
    {
      if ( term_pos->getY() == term_height )
        term_pos->x_ref()--;
      else
      {
        if ( FTermcap::eat_nl_glitch )
        {
          term_pos->setPoint(-1,-1);
        }
        else if ( FTermcap::automatic_right_margin )
        {
          term_pos->setX(0);
          term_pos->y_ref()++;
        }
        else
          term_pos->x_ref()--;
      }
    }
  }

  // sets the new input cursor position
  updateTerminalCursor();
}

//----------------------------------------------------------------------
bool FVTerm::updateTerminalCursor()
{
  // updates the input cursor visibility and the position
  if ( vterm && vterm->input_cursor_visible )
  {
    int x = vterm->input_cursor_x;
    int y = vterm->input_cursor_y;

    if ( isInsideTerminal(x, y) )
    {
      setTermXY (x,y);
      showCursor();
      return true;
    }
  }
  else
    hideCursor();

  return false;
}

//----------------------------------------------------------------------
void FVTerm::processTerminalUpdate()
{
  const int max_skip = 8;

  if ( terminal_update_pending )
  {
    if ( ! unprocessedInput() )
    {
      updateTerminal();
      terminal_update_pending = false;
      skipped_terminal_update = 0;
    }
    else if ( skipped_terminal_update > max_skip )
    {
      force_terminal_update = true;
      updateTerminal();
      force_terminal_update = false;
      terminal_update_pending = false;
      skipped_terminal_update = 0;
    }
    else
      skipped_terminal_update++;
  }
}

//----------------------------------------------------------------------
bool FVTerm::isInsideTerminal (int x, int y)
{
  // Check whether the coordinates are within the virtual terminal
  FRect term_geometry (0, 0, getColumnNumber(), getLineNumber());

  if ( term_geometry.contains(x,y) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
FPoint* FVTerm::getPrintPos() const
{
  return cursor;
}

//----------------------------------------------------------------------
int FVTerm::printf (const wchar_t* format, ...)
{
  assert ( format != 0 );
  const int buf_size = 1024;
  wchar_t buffer[buf_size];
  va_list args;

  va_start (args, format);
  std::vswprintf (buffer, buf_size, format, args);
  va_end (args);

  FString str(buffer);
  return print(str);
}

//----------------------------------------------------------------------
int FVTerm::printf (const char* format, ...)
{
  assert ( format != 0 );
  int   len;
  char  buf[512];
  char* buffer;
  va_list args;

  buffer = buf;
  va_start (args, format);
  len = std::vsnprintf (buffer, sizeof(buf), format, args);
  va_end (args);

  if ( len >= int(sizeof(buf)) )
  {
    buffer = new char[len+1]();
    va_start (args, format);
    std::vsnprintf (buffer, uLong(len+1), format, args);
    va_end (args);
  }

  FString str(buffer);
  int ret = print(str);

  if ( buffer != buf )
    delete[] buffer;

  return ret;
}

//----------------------------------------------------------------------
int FVTerm::print (const std::wstring& s)
{
  assert ( ! s.empty() );
  return print (FString(s));
}

//----------------------------------------------------------------------
int FVTerm::print (FVTerm::term_area* area, const std::wstring& s)
{
  assert ( area != 0 );
  assert ( ! s.empty() );
  return print (area, FString(s));
}

//----------------------------------------------------------------------
int FVTerm::print (const wchar_t* s)
{
  assert ( s != 0 );
  return print (FString(s));
}

//----------------------------------------------------------------------
int FVTerm::print (FVTerm::term_area* area, const wchar_t* s)
{
  assert ( area != 0 );
  assert ( s != 0 );
  return print (area, FString(s));
}

//----------------------------------------------------------------------
int FVTerm::print (const char* s)
{
  assert ( s != 0 );
  FString str(s);
  return print(str);
}

//----------------------------------------------------------------------
int FVTerm::print (FVTerm::term_area* area, const char* s)
{
  assert ( area != 0 );
  assert ( s != 0 );
  FString str(s);
  return print(area, str);
}

//----------------------------------------------------------------------
int FVTerm::print (const std::string& s)
{
  assert ( ! s.empty() );
  return print (FString(s));
}

//----------------------------------------------------------------------
int FVTerm::print (FVTerm::term_area* area, const std::string& s)
{
  assert ( area != 0 );
  assert ( ! s.empty() );
  return print (area, FString(s));
}

//----------------------------------------------------------------------
int FVTerm::print (FString& s)
{
  assert ( ! s.isNull() );
  term_area* area = getPrintArea();

  if ( ! area )
  {
    if ( vdesktop )
      area = vdesktop;
    else
      return -1;
  }

  return print (area, s);
}

//----------------------------------------------------------------------
int FVTerm::print (FVTerm::term_area* area, FString& s)
{
  assert ( ! s.isNull() );
  register int len = 0;
  const wchar_t* p;
  uInt tabstop = getTabstop();

  if ( ! area )
    return -1;

  p = s.wc_str();

  if ( p )
  {
    while ( *p )
    {
      int x_offset, y_offset, width, height, rsh, bsh;
      x_offset = area->x_offset;
      y_offset = area->y_offset;
      width    = area->width;
      height   = area->height;
      rsh      = area->right_shadow;
      bsh      = area->bottom_shadow;

      switch ( *p )
      {
        case '\n':
          cursor->y_ref()++;

        case '\r':
          cursor->x_ref() = 1;
          break;

        case '\t':
          cursor->x_ref() = short ( uInt(cursor->x_ref())
                                  + tabstop
                                  - uInt(cursor->x_ref())
                                  + 1
                                  % tabstop );
          break;

        case '\b':
          cursor->x_ref()--;
          break;

        case '\a':
          beep();
          break;

        default:
        {
          short x = short(cursor->getX() - 1);
          short y = short(cursor->getY() - 1);

          FOptiAttr::char_data  nc; // next character
          nc.code          = *p;
          nc.fg_color      = next_attribute.fg_color;
          nc.bg_color      = next_attribute.bg_color;
          nc.bold          = next_attribute.bold;
          nc.dim           = next_attribute.dim;
          nc.italic        = next_attribute.italic;
          nc.underline     = next_attribute.underline;
          nc.blink         = next_attribute.blink;
          nc.reverse       = next_attribute.reverse;
          nc.standout      = next_attribute.standout;
          nc.invisible     = next_attribute.invisible;
          nc.protect       = next_attribute.protect;
          nc.crossed_out   = next_attribute.crossed_out;
          nc.dbl_underline = next_attribute.dbl_underline;
          nc.alt_charset   = next_attribute.alt_charset;
          nc.pc_charset    = next_attribute.pc_charset;
          nc.transparent   = next_attribute.transparent;
          nc.trans_shadow  = next_attribute.trans_shadow;
          nc.inherit_bg    = next_attribute.inherit_bg;

          int ax = x - x_offset;
          int ay = y - y_offset;

          if (  area
             && ax >= 0 && ay >= 0
             && ax < area->width + area->right_shadow
             && ay < area->height + area->bottom_shadow )
          {
            FOptiAttr::char_data* ac; // area character
            int line_len = area->width + area->right_shadow;
            ac = &area->text[ay * line_len + ax];

            if ( *ac != nc )  // compare with an overloaded operator
            {
              if (  ( ! ac->transparent  && nc.transparent )
                 || ( ! ac->trans_shadow && nc.trans_shadow )
                 || ( ! ac->inherit_bg && nc.inherit_bg ) )
              {
                // add one transparent character form line
                area->changes[ay].trans_count++;
              }
              else if (  ( ac->transparent  && ! nc.transparent )
                      || ( ac->trans_shadow && ! nc.trans_shadow )
                      || ( ac->inherit_bg && ! nc.inherit_bg ) )
              {
                // remove one transparent character from line
                area->changes[ay].trans_count--;
              }

              // copy character to area
              std::memcpy (ac, &nc, sizeof(nc));

              if ( ax < short(area->changes[ay].xmin) )
                area->changes[ay].xmin = uInt(ax);

              if ( ax > short(area->changes[ay].xmax) )
                area->changes[ay].xmax = uInt(ax);
            }
          }

          cursor->x_ref()++;
        }
      }

      if ( cursor->x_ref() > x_offset + width + rsh )
      {
        cursor->x_ref() = short(x_offset + 1);
        cursor->y_ref()++;
      }

      if ( cursor->y_ref() > y_offset + height + bsh )
      {
        cursor->y_ref()--;
        break;
      }

      p++;
      len++;
    } // end of while

    updateVTerm (area);
  }

  return len;
}

//----------------------------------------------------------------------
int FVTerm::print (register int c)
{
  term_area* area = getPrintArea();

  if ( ! area )
  {
    if ( vdesktop )
      area = vdesktop;
    else
      return -1;
  }

  return print (area, c);
}

//----------------------------------------------------------------------
int FVTerm::print (FVTerm::term_area* area, register int c)
{
  FOptiAttr::char_data nc; // next character
  int x_offset, y_offset, width, height, rsh, bsh, ax, ay;
  short x, y;

  if ( ! area )
    return -1;

  x_offset = area->x_offset;
  y_offset = area->y_offset;
  width    = area->width;
  height   = area->height;
  rsh      = area->right_shadow;
  bsh      = area->bottom_shadow;

  nc.code          = c;
  nc.fg_color      = next_attribute.fg_color;
  nc.bg_color      = next_attribute.bg_color;
  nc.bold          = next_attribute.bold;
  nc.dim           = next_attribute.dim;
  nc.italic        = next_attribute.italic;
  nc.underline     = next_attribute.underline;
  nc.blink         = next_attribute.blink;
  nc.reverse       = next_attribute.reverse;
  nc.standout      = next_attribute.standout;
  nc.invisible     = next_attribute.invisible;
  nc.protect       = next_attribute.protect;
  nc.crossed_out   = next_attribute.crossed_out;
  nc.dbl_underline = next_attribute.dbl_underline;
  nc.alt_charset   = next_attribute.alt_charset;
  nc.pc_charset    = next_attribute.pc_charset;
  nc.transparent   = next_attribute.transparent;
  nc.trans_shadow  = next_attribute.trans_shadow;
  nc.inherit_bg    = next_attribute.inherit_bg;

  x = short(cursor->getX() - 1);
  y = short(cursor->getY() - 1);
  ax = x - x_offset;
  ay = y - y_offset;

  if (  ax >= 0 && ay >= 0
     && ax < area->width + area->right_shadow
     && ay < area->height + area->bottom_shadow )
  {
    FOptiAttr::char_data* ac; // area character
    int line_len = area->width + area->right_shadow;
    ac = &area->text[ay * line_len + ax];

    if ( *ac != nc )  // compare with an overloaded operator
    {
      if (  ( ! ac->transparent  && nc.transparent )
         || ( ! ac->trans_shadow && nc.trans_shadow )
         || ( ! ac->inherit_bg && nc.inherit_bg ) )
      {
        // add one transparent character form line
        area->changes[ay].trans_count++;
      }

      if (  ( ac->transparent  && ! nc.transparent )
         || ( ac->trans_shadow && ! nc.trans_shadow )
         || ( ac->inherit_bg && ! nc.inherit_bg ) )
      {
        // remove one transparent character from line
        area->changes[ay].trans_count--;
      }

      // copy character to area
      std::memcpy (ac, &nc, sizeof(nc));

      if ( ax < short(area->changes[ay].xmin) )
        area->changes[ay].xmin = uInt(ax);

      if ( ax > short(area->changes[ay].xmax) )
        area->changes[ay].xmax = uInt(ax);
    }
  }

  cursor->x_ref()++;

  if ( cursor->x_ref() > x_offset + width + rsh )
  {
    cursor->x_ref() = short(x_offset + 1);
    cursor->y_ref()++;
  }

  if ( cursor->y_ref() > y_offset + height + bsh )
  {
    cursor->y_ref()--;
    updateVTerm (area);
    return -1;
  }

  updateVTerm (area);
  return 1;
}

//----------------------------------------------------------------------
inline void FVTerm::newFontChanges (FOptiAttr::char_data*& next_char)
{
  // NewFont special cases
  if ( NewFont )
  {
    switch ( next_char->code )
    {
      case fc::LowerHalfBlock:
        next_char->code = fc::UpperHalfBlock;
        // fall through
      case fc::NF_rev_left_arrow2:
      case fc::NF_rev_right_arrow2:
      case fc::NF_rev_border_corner_upper_right:
      case fc::NF_rev_border_line_right:
      case fc::NF_rev_border_line_vertical_left:
      case fc::NF_rev_border_corner_lower_right:
      case fc::NF_rev_up_arrow2:
      case fc::NF_rev_down_arrow2:
      case fc::NF_rev_up_arrow1:
      case fc::NF_rev_down_arrow1:
      case fc::NF_rev_left_arrow1:
      case fc::NF_rev_right_arrow1:
      case fc::NF_rev_menu_button1:
      case fc::NF_rev_menu_button2:
      case fc::NF_rev_up_pointing_triangle1:
      case fc::NF_rev_down_pointing_triangle1:
      case fc::NF_rev_up_pointing_triangle2:
      case fc::NF_rev_down_pointing_triangle2:
      case fc::NF_rev_menu_button3:
      case fc::NF_rev_border_line_right_and_left:
        // swap foreground and background color
        std::swap (next_char->fg_color, next_char->bg_color);
        break;

      default:
        break;
    }
  }
}

//----------------------------------------------------------------------
inline void FVTerm::charsetChanges (FOptiAttr::char_data*& next_char)
{
  if ( Encoding == fc::UTF8 )
    return;

  uInt code = uInt(next_char->code);
  uInt ch = charEncode(code);

  if ( ch != code )
  {
    if ( ch == 0 )
    {
      next_char->code = int(charEncode(code, fc::ASCII));
      return;
    }

    next_char->code = int(ch);

    if ( Encoding == fc::VT100 )
      next_char->alt_charset = true;
    else if ( Encoding == fc::PC )
    {
      next_char->pc_charset = true;

      if ( isXTerminal() && hasUTF8() && ch < 0x20 )  // Character 0x00..0x1f
        next_char->code = int(charEncode(code, fc::ASCII));
    }
  }
}

//----------------------------------------------------------------------
inline void FVTerm::appendCharacter (FOptiAttr::char_data*& next_char)
{
  newFontChanges (next_char);
  charsetChanges (next_char);

  appendAttributes (next_char);
  appendOutputBuffer (next_char->code);
}

//----------------------------------------------------------------------
inline void FVTerm::appendAttributes (FOptiAttr::char_data*& next_attr)
{
  char* attr_str;
  FOptiAttr::char_data* term_attr = &term_attribute;

  // generate attribute string for the next character
  attr_str = changeAttribute (term_attr, next_attr);

  if ( attr_str )
    appendOutputBuffer (attr_str);
}

//----------------------------------------------------------------------
int FVTerm::appendLowerRight (FOptiAttr::char_data*& screen_char)
{
  char* SA = tcap[fc::t_enter_am_mode].string;
  char* RA = tcap[fc::t_exit_am_mode].string;

  if ( ! FTermcap::automatic_right_margin )
  {
    appendCharacter (screen_char);
  }
  else if ( SA && RA )
  {
    appendOutputBuffer (RA);
    appendCharacter (screen_char);
    appendOutputBuffer (SA);
  }
  else
  {
    int x, y;
    char* IC = tcap[fc::t_parm_ich].string;
    char* im = tcap[fc::t_enter_insert_mode].string;
    char* ei = tcap[fc::t_exit_insert_mode].string;
    char* ip = tcap[fc::t_insert_padding].string;
    char* ic = tcap[fc::t_insert_character].string;

    x = getColumnNumber() - 2;
    y = getLineNumber() - 1;
    setTermXY (x, y);
    appendCharacter (screen_char);
    term_pos->x_ref()++;

    setTermXY (x, y);
    screen_char--;

    if ( IC )
    {
      appendOutputBuffer (tparm(IC, 1));
      appendCharacter (screen_char);
    }
    else if ( im && ei )
    {
      appendOutputBuffer (im);
      appendCharacter (screen_char);

      if ( ip )
        appendOutputBuffer (ip);

      appendOutputBuffer (ei);
    }
    else if ( ic )
    {
      appendOutputBuffer (ic);
      appendCharacter (screen_char);

      if ( ip )
        appendOutputBuffer (ip);
    }
  }

  return screen_char->code;
}

//----------------------------------------------------------------------
inline void FVTerm::appendOutputBuffer (std::string& s)
{
  const char* c_string = s.c_str();
  tputs (c_string, 1, appendOutputBuffer);
}

//----------------------------------------------------------------------
inline void FVTerm::appendOutputBuffer (const char* s)
{
  tputs (s, 1, appendOutputBuffer);
}

//----------------------------------------------------------------------
int FVTerm::appendOutputBuffer (int ch)
{
  output_buffer->push(ch);

  if ( output_buffer->size() >= TERMINAL_OUTPUT_BUFFER_SIZE )
    flush_out();

  return ch;
}

//----------------------------------------------------------------------
void FVTerm::flush_out()
{
  while ( ! output_buffer->empty() )
  {
    Fputchar (output_buffer->front());
    output_buffer->pop();
  }

  std::fflush(stdout);
}
