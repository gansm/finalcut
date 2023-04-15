/***********************************************************************
* opti-move.cpp - Tests the cursor movement optimization               *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2023 Markus Gans                                      *
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

#include <array>
#include <iomanip>
#include <memory>
#include <string>

#include <final/final.h>

// function prototype
auto keyPressed() -> bool;
void term_boundaries (int&, int&);
void move (int, int, int, int);


//----------------------------------------------------------------------
// functions
//----------------------------------------------------------------------
auto keyPressed() -> bool
{
  // Waiting for keypress

  struct termios save{};
  bool ret{false};
  std::cout << "\nPress any key to continue...";
  fflush(stdout);
  tcgetattr (STDIN_FILENO, &save);
  struct termios t = save;
  t.c_lflag &= uInt(~(ICANON | ECHO));
  tcsetattr (STDIN_FILENO, TCSANOW, &t);

  if ( std::getchar() != EOF )
    ret = true;
  else
    ret = false;

  tcsetattr (STDIN_FILENO, TCSADRAIN, &save);
  return ret;
}

//----------------------------------------------------------------------
void term_boundaries (int& x, int& y)
{
  // checks and corrects the terminal boundaries
  const auto term_width  = int(finalcut::FVTerm::getFOutput()->getColumnNumber());
  const auto term_height = int(finalcut::FVTerm::getFOutput()->getLineNumber());
  x = std::max(0, x);
  y = std::max(0, y);
  y = std::min(term_height - 1, y);

  if ( x >= term_width && term_width > 0 )
  {
    y += x / term_width;
    x %= term_width;
  }
}

//----------------------------------------------------------------------
void move (int xold, int yold, int xnew, int ynew)
{
  // Prints the cursor move escape sequence
  finalcut::FString buffer{};
  finalcut::FString sequence{};
  finalcut::FString from{};
  finalcut::FString to{};
  finalcut::FString byte{};

  constexpr std::array<const char*, 33> ctrl_character =
  {{
    "NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL",
    "BS",  "Tab", "LF",  "VT",  "FF",  "CR",  "SO",  "SI",
    "DLE", "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB",
    "CAN", "EM",  "SUB", "Esc", "FS",  "GS",  "RS",  "US",
    "Space"
  }};

  term_boundaries(xold, yold);
  term_boundaries(xnew, ynew);

  // Get the move string
  buffer = finalcut::FTerm::moveCursorString (xold, yold, xnew, ynew);

  for (const auto& ch : buffer)
  {
    if ( ch < 0x21 )
      sequence += ctrl_character[uChar(ch)];
    else
      sequence += ch;

    sequence += ' ';
  }

  from.sprintf ("(%3d;%3d)", xold, yold);
  to.sprintf ("(%3d;%3d)", xnew, ynew);
  const std::size_t len = buffer.getLength();

  if ( len <= 1 )
    byte.sprintf ("%d byte ", len);
  else
    byte.sprintf ("%d bytes", len);

  std::cout << std::right << std::setw(10) << from << " -> "
            << std::left << std::setw(10) << to << " "
            << std::setw(21) << sequence << " "
            << std::right << std::setw(10) << byte << "\r\n";
}

//----------------------------------------------------------------------
class DirectLogger final : public finalcut::FLog
{
  public:
    // Constructor
    DirectLogger() = default;

    // Destructor
    ~DirectLogger() noexcept override;

    void info (const std::string& entry) override
    {
      output << entry << "\r" << std::endl;
    }

    void warn (const std::string&) override
    {
      // An implementation is not required in this context
    }

    void error (const std::string&) override
    {
      // An implementation is not required in this context
    }


    void debug (const std::string&) override
    {
      // An implementation is not required in this context
    }

    void flush() override
    {
      output.flush();
    }

    void setOutputStream (const std::ostream& os) override
    { output.rdbuf(os.rdbuf()); }

    void setLineEnding (LineEnding) override
    {
      // An implementation is not required in this context
    }


    void enableTimestamp() override
    {
      // An implementation is not required in this context
    }


    void disableTimestamp() override
    {
      // An implementation is not required in this context
    }


  private:
    // Data member
    std::ostream output{std::cerr.rdbuf()};
};

//----------------------------------------------------------------------
DirectLogger::~DirectLogger() noexcept = default;  // destructor


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
auto main (int argc, char* argv[]) -> int
{
  // Disable mouse, color palette changes and terminal data requests
  auto& start_options = finalcut::FStartOptions::getInstance();
  start_options.mouse_support = false;
  start_options.color_change = false;
  start_options.terminal_data_request = false;
  start_options.terminal_focus_events = false;

  // Create the application object
  finalcut::FApplication term_app{argc, argv};

  // Force terminal initialization without calling show()
  term_app.initTerminal();

  if ( finalcut::FApplication::isQuit() )
    return 0;

  // Get screen dimension
  auto xmax = int(term_app.getDesktopWidth() - 1);
  auto ymax = int(term_app.getDesktopHeight() - 1);
  finalcut::FString line{std::size_t(xmax) + 1, '-'};

  // Show the determined terminal name and text resolution
  std::cout << "Terminal: " << finalcut::FTerm::getTermType() << "\r\n";
  std::cout << " Columns: 0.." << xmax << "\r\n";
  std::cout << "   Lines: 0.." << ymax << "\r\n";

  // Show the escape sequences for the following cursor movements
  std::cout << std::setw(38) << "Cursor move\r\n";
  std::cout << "    (From) -> (To)       ";
  std::cout << "escape sequence          ";
  std::cout << "Length\r\n";
  std::cout << line;

  move (5, 12, 0, 0);
  move (5, ymax, 5, 0);
  move (xmax, 1, 0, 1);
  move (xmax, 1, 0, 2);
  move (xmax + 1, 1, 0, 2);
  move (9, 4, 10, 4);
  move (10, 4, 9, 4);
  move (9, 4, 11, 4);
  move (11, 4, 9, 4);
  move (1, 0, 8, 0);
  move (16, 0, 16, 1);
  move (16, 1, 16, 0);
  move (16, 0, 16, 2);
  move (16, 2, 16, 0);
  move (3, 2, xmax, 2);
  move (5, 5, xmax - 5, ymax - 5);

  // Waiting for keypress
  keyPressed();

  // Show terminal speed and milliseconds for all cursor movement sequence
  std::cout << "\r" << line << std::flush;
  // Generation of a logger in a shared_ptr via a pointer
  finalcut::FApplication::setLog(std::make_shared<DirectLogger>());
  const auto& opti_move = finalcut::FOptiMove::getInstance();
  finalcut::printDurations(opti_move);

  // Waiting for keypress
  keyPressed();
  return 0;
}
