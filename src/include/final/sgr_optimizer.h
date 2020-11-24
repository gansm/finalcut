/***********************************************************************
* sgr_optimizer.h - Combines SGR (Select Graphic Rendition) attributes *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2020 Markus Gans                                      *
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
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ SGRoptimizer ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef SGR_OPTIMIZER_H
#define SGR_OPTIMIZER_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <array>
#include <vector>

namespace finalcut
{

//----------------------------------------------------------------------
// class SGRoptimizer
//----------------------------------------------------------------------

class SGRoptimizer final
{
  public:
    // Constants
    static constexpr std::size_t ATTR_BUF_SIZE{8192};

    // Typedefs
    typedef std::array<char, ATTR_BUF_SIZE> AttributeBuffer;

    // Constructors
    explicit SGRoptimizer (AttributeBuffer&);

    // Disable copy constructor
    SGRoptimizer (const SGRoptimizer&) = delete;

    // Destructor
    ~SGRoptimizer() noexcept = default;

    // Disable copy assignment operator (=)
    SGRoptimizer& operator = (const SGRoptimizer&) = delete;

    // Method
    void optimize();

  private:
    // Constants
    static constexpr auto NOT_SET = static_cast<std::size_t>(-1);

    // Methods
    void findParameter();
    void combineParameter();

    // Data member
    AttributeBuffer& seq;

    struct parameter
    {
      std::size_t start;
      std::size_t end;
    };

    std::vector<parameter> csi_parameter{};
};

}  // namespace finalcut

#endif  // SGR_OPTIMIZER_H
