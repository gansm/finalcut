/***********************************************************************
* fclassname.h - [brief description]                                   *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright [year] [Maintainer]                                        *
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

/*  Standalone class
 *  ════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FClassName ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FCLASSNAME_H
#define FCLASSNAME_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

//#include ...

namespace finalcut
{

//----------------------------------------------------------------------
// class FClassName
//----------------------------------------------------------------------

class FClassName
{
  public:
    // Using-declaration

    // Typedefs and Enumerations

    // Constants

    // Constructors
    FClassName();

    // Disable copy constructor
    FClassName (const FClassName&) = delete;

    // Destructor
    ~FClassName();

    // Disable copy assignment operator (=)
    auto operator = (const FClassName&) -> FClassName& = delete;

    // Overloaded operators

    // Accessors

    // Mutators

    // Inquiries

    // Methods

    // Event handlers

    // Callback methods

    // Data members

  protected:
    // Typedefs and Enumerations

    // Constants

    // Accessors

    // Inquiries

    // Methods

    // Event handlers

    // Callback methods

    // Data members

  private:
    // Typedefs and Enumerations

    // Constants

    // Accessors

    // Inquiries

    // Methods

    // Event handlers

    // Callback methods

    // Data members

    // Friend class
};


// FClassName inline functions
//----------------------------------------------------------------------

}  // namespace finalcut

#endif  // FCLASSNAME_H

