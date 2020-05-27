/***********************************************************************
* fcolorpalette.h - Define RGB color value for a palette entry         *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2018-2020 Markus Gans                                      *
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

/*  Standalone class
 *  ════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FColorPalette ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FCOLORPALETTE_H
#define FCOLORPALETTE_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <functional>

#include "final/fstring.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FColorPalette
//----------------------------------------------------------------------

class FColorPalette
{
  public:
    // Typedef
    typedef std::function<void(FColor, int, int, int)> FSetPalette;

    // Constructor
    FColorPalette(FSetPalette);

    // Destructor
    virtual ~FColorPalette();

    // Accessor
    virtual const FString getClassName() const;

    // Methods
    virtual void setColorPalette() = 0;
    virtual void resetColorPalette() = 0;

  protected:
    void setPalette (FColor, int, int, int);

    // Data members
    FSetPalette set_palette;
};

// FColorPalette inline functions
//----------------------------------------------------------------------
inline const FString FColorPalette::getClassName() const
{ return "FColorPalette"; }


/*  Inheritance diagram
 *  ═══════════════════
 *
 *      ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *      ▕ FColorPalette ▏
 *      ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *              ▲
 *              │
 *  ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *  ▕ default8ColorPalette ▏
 *  ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

//----------------------------------------------------------------------
// class default8ColorPalette
//----------------------------------------------------------------------

class default8ColorPalette final : public FColorPalette
{
  public:
    // Constructor
    default8ColorPalette (FSetPalette);

    // Destructor
    ~default8ColorPalette();

    // Accessor
    virtual const FString getClassName() const;

    // Methods
    void setColorPalette();
    void resetColorPalette();
};

// default8ColorPalette inline functions
//----------------------------------------------------------------------
inline const FString default8ColorPalette::getClassName() const
{ return "default8ColorPalette"; }


/*  Inheritance diagram
 *  ═══════════════════
 *
 *      ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *      ▕ FColorPalette ▏
 *      ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *              ▲
 *              │
 *  ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *  ▕ default16ColorPalette ▏
 *  ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

//----------------------------------------------------------------------
// class default16ColorPalette
//----------------------------------------------------------------------

class default16ColorPalette final : public FColorPalette
{
  public:
    // Constructor
    default16ColorPalette (FSetPalette);

    // Destructor
    ~default16ColorPalette();

    // Accessor
    virtual const FString getClassName() const;

    // Methods
    void setColorPalette();
    void resetColorPalette();
};

// default16ColorPalette inline functions
//----------------------------------------------------------------------
inline const FString default16ColorPalette::getClassName() const
{ return "default16ColorPalette"; }

}  // namespace finalcut

#endif  // FCOLORPALETTE_H
