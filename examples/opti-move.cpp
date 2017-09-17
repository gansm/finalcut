// File: opti-move.cpp

#include <iomanip>
#include <string>

#include <final/fapplication.h>
#include <final/fvterm.h>


// global FVTerm object
static FVTerm* terminal;

// function prototype
bool keyPressed();
void term_boundaries (int&, int&);
void move (int, int, int, int);


//----------------------------------------------------------------------
// functions
//----------------------------------------------------------------------
bool keyPressed()
{
  // Waiting for keypress
  struct termios save, t;
  bool ret;
  std::cout << "\nPress any key to continue...";
  fflush(stdout);
  tcgetattr (STDIN_FILENO, &save);
  t = save;
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
  int term_width  = terminal->getColumnNumber();
  int term_height = terminal->getLineNumber();

  if ( x < 0 )
    x = 0;

  if ( y < 0 )
    y = 0;

  if ( x >= term_width )
  {
    y += x / term_width;
    x %= term_width;
  }

  if ( y >= term_height )
    y = term_height - 1;
}

//----------------------------------------------------------------------
void move (int xold, int yold, int xnew, int ynew)
{
  // prints the cursor move escape sequence
  std::string sequence;
  char* buffer;
  char  from[10], to[10], byte[20];
  uInt  len;

  term_boundaries(xold, yold);
  term_boundaries(xnew, ynew);
  snprintf (from, sizeof(from), "(%d;%d)", xold, yold);
  snprintf (to, sizeof(to), "(%d;%d)", xnew, ynew);
  std::cout << std::right << std::setw(10) << from
            << " -> "
            << std::left << std::setw(10) << to
            << " ";
  // get the move string
  buffer = terminal->moveCursor (xold, yold, xnew, ynew);
  len    = uInt(std::strlen(buffer));

  for (uInt i = 0; i < len; i++)
  {
    switch ( buffer[i] )
    {
      case 0x08:
        sequence += "BS ";
        break;

      case 0x09:
        sequence += "TAB ";
        break;

      case 0x0a:
        sequence += "LF ";
        break;

      case 0x0d:
        sequence += "CR ";
        break;

      case 0x1b:
        sequence += "Esc ";
        break;

      default:
        sequence += buffer[i];
        sequence += ' ';
    }
  }

  std::cout << std::setw(21) << sequence << " ";

  if ( len <= 1 )
    snprintf (byte, sizeof(byte), "%d byte ", len);
  else
    snprintf (byte, sizeof(byte), "%d bytes", len);

  std::cout << std::right << std::setw(10) << byte << "\r\n";
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  int xmax, ymax;

  FApplication app(argc, argv);
  terminal = new FVTerm(&app);
  xmax = terminal->getColumnNumber() - 1;
  ymax = terminal->getLineNumber() - 1;
  FString line(xmax + 1, '-');

  terminal->setTermXY(0,0);
  terminal->setNormal();
  terminal->clearArea();

  std::cout << "Terminal: " << terminal->getTermType() << "\r\n";
  std::cout << " Columns: 0.." << xmax << "\r\n";
  std::cout << "   Lines: 0.." << ymax << "\r\n";

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

  keyPressed();

  std::cout << "\r" << line;
  terminal->printMoveDurations();

  keyPressed();
}
