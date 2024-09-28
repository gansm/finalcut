/***********************************************************************
* sgr_optimizer.h - Combines SGR (Select Graphic Rendition) attributes *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2023 Markus Gans                                      *
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
#include <string>
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
    static constexpr std::string::size_type ATTR_BUF_SIZE{8192u};

    // Constructors
    explicit SGRoptimizer (std::string&);

    // Method
    void optimize();

  private:
    struct parameter
    {
      std::size_t start;
      std::size_t end;
    };

    // Constants
    static constexpr auto NOT_SET = static_cast<std::size_t>(-1);

    // Methods
    void findParameter();
    auto isSGRStart (std::size_t) -> bool;
    void combineParameter();
    void handleSGRterminating ( const std::vector<parameter>::const_iterator, std::size_t&
                              , std::size_t, std::size_t );

    // Data member
    std::string& seq;
    std::vector<parameter> csi_parameter{};
};

}  // namespace finalcut

#endif  // SGR_OPTIMIZER_H
