// File: calculator.cpp

//----------------------------------------------------------------------
// A simple calculator with trigonometric functions
//----------------------------------------------------------------------

#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <stack>

#include "fapp.h"
#include "fdialog.h"
#include "fmessagebox.h"

const lDouble PI = 3.141592653589793238L;

//----------------------------------------------------------------------
// class Button
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class Button : public FButton
{
 public:
   // Constructor
   explicit Button (FWidget* = 0);

   // Method
   void setChecked(bool);

   // Event handler
   void onKeyPress (FKeyEvent*);

 private:
   // Data Member
   bool checked;
};
#pragma pack(pop)

//----------------------------------------------------------------------
Button::Button (FWidget* parent)
  : FButton(parent)
  , checked(false)
{ }

//----------------------------------------------------------------------
void Button::setChecked (bool on)
{
  if ( checked == on )
    return;

  checked = on;

  if ( checked )
  {
    setBackgroundColor(fc::Cyan);
    setFocusForegroundColor(fc::White);
    setFocusBackgroundColor(fc::Cyan);
  }
  else
  {
    setBackgroundColor(wc.button_active_bg);
    setFocusForegroundColor(wc.button_active_focus_fg);
    setFocusBackgroundColor(wc.button_active_focus_bg);
  }

  redraw();
}

//----------------------------------------------------------------------
void Button::onKeyPress (FKeyEvent* ev)
{
  int key = ev->key();

  // catch the enter key
  if ( key == fc::Fkey_return || key == fc::Fkey_enter )
    return;

  FButton::onKeyPress(ev);
}


//----------------------------------------------------------------------
// class Calc
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class Calc : public FDialog
{
 public:
   // Constructor
   explicit Calc (FWidget* parent=0);

   // Destructor
  ~Calc();

   // Event handlers
   void onKeyPress (FKeyEvent*);
   void onAccel (FAccelEvent*);
   void onClose (FCloseEvent*);

   // Callback method
   void cb_buttonClicked (FWidget*, void*);

 private:
   // Methods
   void drawDispay();
   virtual void draw();
   bool isDataEntryKey(int);
   bool isOperatorKey(int);
   void setDisplay (lDouble);
   void setInfixOperator(char);
   void clearInfixOperator();
   void calcInfixOperator();
   void adjustSize();

   // Data Members
   bool error;
   bool arcus_mode;
   bool hyperbolic_mode;

   enum button
   {
     Sine,
     Cosine,
     Tangent,
     Reciprocal,
     On,
     Natural_logarithm,
     Powers_of_e,
     Power,
     Square_root,
     Divide,
     Common_logarithm,
     Powers_of_ten,
     Parenthese_l,
     Parenthese_r,
     Multiply,
     Hyperbolic,
     Seven,
     Eight,
     Nine,
     Subtract,
     Arcus,
     Four,
     Five,
     Six,
     Add,
     Pi,
     One,
     Two,
     Three,
     Percent,
     Zero,
     Decimal_point,
     Change_sign,
     Equals,
     NUM_OF_BUTTONS
   };

   lDouble a, b;
   uInt    max_char;
   int     last_key;
   char    infix_operator;
   char    last_infix_operator;
   FString input;
   int     button_no[Calc::NUM_OF_BUTTONS];

   struct stack_data
   {
     lDouble term;
     char infix_operator;
   };

   std::stack<stack_data> bracket_stack;
   std::map<Calc::button, Button*> calculator_buttons;
};
#pragma pack(pop)

//----------------------------------------------------------------------
Calc::Calc (FWidget* parent)
  : FDialog(parent)
  , error(false)
  , arcus_mode(false)
  , hyperbolic_mode(false)
  , a(0.0L)
  , b(0.0L)
  , max_char(32)
  , last_key(-1)
  , infix_operator('\0')
  , last_infix_operator('\0')
  , input("")
  , bracket_stack()
  , calculator_buttons()
{
  clearInfixOperator();

  const wchar_t* button_text[Calc::NUM_OF_BUTTONS] =
  {
    L"&Sin",
    L"&Cos",
    L"&Tan",
    L"1/&x",
    L"&On",
    L"L&n",
    L"&e\x02e3",
    L"&y\x02e3",
    L"Sq&r",
    L"&\xf7",
    L"&Lg",
    L"10&\x02e3",
    L"&(",
    L"&)",
    L"&\xd7",
    L"&Hyp",
    L"&7",
    L"&8",
    L"&9",
    L"&-",
    L"&Arc",
    L"&4",
    L"&5",
    L"&6",
    L"&+",
    L"&\x03c0",
    L"&1",
    L"&2",
    L"&3",
    L"&%",
    L"&0",
    L"&.",
    L"&±",
    L"&="
  };

  std::setlocale(LC_NUMERIC, "C");
  setText ("calculator");
  setGeometry (19, 6, 37, 18);
  addAccelerator('q');  // press 'q' to quit
  setShadow();

  for (int key=0; key < Calc::NUM_OF_BUTTONS; key++)
  {
    Button* btn = new Button(this);
    button_no[key] = key;

    if ( key == Equals )
      btn->setGeometry(30, 15, 5, 3);
    else
    {
      int x, y, n;
      (key <= Three) ? n=0 : n=1;
      x = (key+n)%5*7 + 2;
      y = (key+n)/5*2 + 3;
      btn->setGeometry(x, y, 5, 1);
    }

    btn->setFlat();
    btn->setNoUnderline();
    btn->setText(button_text[key]);
    btn->setDoubleFlatLine(fc::top);
    btn->setDoubleFlatLine(fc::bottom);

    if ( isNewFont() )
      btn->unsetClickAnimation();

    btn->addCallback
    (
      "clicked",
      _METHOD_CALLBACK (this, &Calc::cb_buttonClicked),
      &button_no[key]
    );

    calculator_buttons[button(key)] = btn;

  }

  calculator_buttons[On]->addAccelerator(fc::Fkey_dc); // del key
  calculator_buttons[On]->setFocus();
  calculator_buttons[Pi]->addAccelerator('p');
  calculator_buttons[Power]->addAccelerator('^');
  calculator_buttons[Divide]->addAccelerator('/');
  calculator_buttons[Powers_of_ten]->addAccelerator('d');
  calculator_buttons[Multiply]->addAccelerator('*');
  calculator_buttons[Decimal_point]->addAccelerator(',');
  calculator_buttons[Change_sign]->addAccelerator('#');
  calculator_buttons[Equals]->addAccelerator(fc::Fkey_return);
  calculator_buttons[Equals]->addAccelerator(fc::Fkey_enter);
}

//----------------------------------------------------------------------
Calc::~Calc()
{
}

//----------------------------------------------------------------------
void Calc::drawDispay()
{
  FString display = input;
  updateVTerm(false);

  if ( display.isNull() || display.isEmpty()  )
    display = L'0';

  if ( display.right(3) == L"-0." )
    display = L'0';

  if ( display.back() == L'.' && display.getLength() > 1 )
    display = display.left(display.getLength() - 1);

  if ( ! display.isEmpty() && display.getLength() < max_char )
    display.insert(FString(max_char - display.getLength(), L' '), 0);

  if ( display.getLength() > max_char )
    display = display.left(max_char);

  if ( infix_operator )
    display[1] = infix_operator;

  if ( error )
    display = " Error                          ";

  setColor(fc::Black, fc::LightGray);

  if ( isMonochron() )
    setReverse(false);

  setPrintPos (3,3);
  print(display);
  print(L' ');
  setColor(wc.dialog_fg, wc.dialog_bg);

  if ( isMonochron() )
    setReverse(true);

  if ( isNewFont() )
  {
    FString bottom_line(33, wchar_t(fc::NF_border_line_bottom));
    setPrintPos (3,2);
    print (bottom_line);
    setPrintPos (2,3);
    print (wchar_t(fc::NF_rev_border_line_right));
    setPrintPos (36,3);
    print (wchar_t(fc::NF_border_line_left));
    FString top_bottom_line_5(5, wchar_t(fc::NF_border_line_up_and_down));
    FString top_line_2(2, wchar_t(fc::NF_border_line_upper));
    setPrintPos (3,4);
    print ( top_bottom_line_5 + top_line_2
          + top_bottom_line_5 + top_line_2
          + top_bottom_line_5 + top_line_2
          + top_bottom_line_5 + top_line_2
          + top_bottom_line_5);
  }
  else
  {
    FString separator = FString(wchar_t(fc::BoxDrawingsVerticalAndRight))
                      + FString(35, wchar_t(fc::BoxDrawingsHorizontal))
                      + FString(wchar_t(fc::BoxDrawingsVerticalAndLeft));
    setPrintPos (1,4);
    print(separator);
  }

  updateVTerm(true);
}

//----------------------------------------------------------------------
void Calc::draw()
{
  FDialog::draw();
  drawDispay();
}

//----------------------------------------------------------------------
bool Calc::isDataEntryKey (int key)
{
  // test if key is in {'.', '0'..'9'}
  int data_entry_keys[] =
  {
    Decimal_point,
    Zero,
    One,
    Two,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
    Nine
  };

  int* iter = std::find (data_entry_keys, data_entry_keys+11, key);

  if ( iter != data_entry_keys+11 )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
bool Calc::isOperatorKey(int key)
{
  // test if key is in {'*', '/', '+', '-', '^', '='}
  int operators[] =
  {
    Multiply,
    Divide,
    Add,
    Subtract,
    Power,
    Equals
  };

  int* iter = std::find (operators, operators+6, key);

  if ( iter != operators+6 )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
void Calc::setDisplay (lDouble d)
{
  char buffer[33];
  std::snprintf (buffer, sizeof(buffer), "%32.11Lg", d);
  input = buffer;
}

//----------------------------------------------------------------------
inline void Calc::setInfixOperator(char c)
{
  infix_operator = c;
  last_infix_operator = infix_operator;
}

//----------------------------------------------------------------------
inline void Calc::clearInfixOperator()
{
  infix_operator = '\0';
}

//----------------------------------------------------------------------
void Calc::calcInfixOperator()
{
  using namespace std;

  switch ( infix_operator )
  {
    case '*':
      if ( std::fabs(a) > LDBL_EPSILON )  // a != 0.0L
      {
        // ln(a * b) = ln(a) + ln(b)
        if ( std::log(std::abs(a)) + std::log(std::abs(b)) <= std::log(LDBL_MAX) )
          a *= b;
        else
          error = true;
      }
      else
        b = 0.0L;
      break;

    case '/':
      if ( std::fabs(b) > LDBL_EPSILON )  // b != 0.0L
        a /= b;
      else
        error = true;
      break;

    case '+':
      if ( std::fabs(a) > LDBL_EPSILON )  // a != 0.0L
      {
        if ( std::log(std::abs(a)) + std::log(std::abs(1 + b/a)) <= std::log(LDBL_MAX) )
          a += b;
        else
          error = true;
      }
      else
        a = b;
      break;

    case '-':
      if ( std::fabs(b) > LDBL_EPSILON )  // b != 0.0L
      {
        if ( std::log(std::abs(a)) + std::log(std::abs(1 - b/a)) <= std::log(LDBL_MAX) )
          a -= b;
        else
          error = true;
      }
      else
        a = b * (-1.0L);
      break;

    case '^':
      a = std::pow(a, b);

      if ( errno == EDOM || errno == ERANGE )
        error = true;
      break;

    default:
      break;
  }
  clearInfixOperator();
}

//----------------------------------------------------------------------
void Calc::onKeyPress (FKeyEvent* ev)
{
  int len = int(input.getLength());
  int key = ev->key();

  switch ( key )
  {
    case fc::Fkey_erase:
    case fc::Fkey_backspace:
      if ( len > 0 )
      {
        if ( len == 1 )
          input = "";
        else
          input = input.left(input.getLength() - 1);

        a = std::atof(input.c_str());
        drawDispay();
        updateTerminal();
      }

      ev->accept();
      break;

    case fc::Fkey_escape:
    case fc::Fkey_escape_mintty:
      {
        FAccelEvent a_ev(fc::Accelerator_Event, getFocusWidget());
        calculator_buttons[On]->onAccel(&a_ev);
      }
      ev->accept();
      break;

    default:
      FDialog::onKeyPress(ev);
      break;
  }
}

//----------------------------------------------------------------------
void Calc::onAccel (FAccelEvent* ev)
{
  close();
  ev->accept();
}

//----------------------------------------------------------------------
void Calc::onClose (FCloseEvent* ev)
{
  int ret = FMessageBox::info ( this, "Quit",
                                "Do you really want\n"
                                "to quit the program ?",
                                FMessageBox::Yes,
                                FMessageBox::No );

  (ret == FMessageBox::Yes) ? ev->accept() : ev->ignore();
}

//----------------------------------------------------------------------
void Calc::cb_buttonClicked (FWidget*, void* data_ptr)
{
  int key;
  lDouble* x;

  using namespace std;

  if ( infix_operator )
    x = &b;
  else
    x = &a;

  key = *(static_cast<int*>(data_ptr));

  switch ( key )
  {
    case Sine:  // sin
      if ( hyperbolic_mode )
      {
        if ( arcus_mode )
        {
          *x = std::log(*x + std::sqrt((*x) * (*x) + 1));

          if ( errno == EDOM || errno == ERANGE )
            error = true;

          if ( *x == INFINITY )
            error = true;
        }
        else
          *x = std::sinh(*x);
      }
      else
      {
        if ( arcus_mode )
          *x = std::asin(*x) * 180.0L/PI;
        else if ( std::fabs(std::fmod(*x,180.0L)) < LDBL_EPSILON )  // x/180 = 0
          *x = 0.0L;
        else
          *x = std::sin(*x * PI/180.0L);
      }

      if ( errno == EDOM )
        error = true;

      setDisplay(*x);
      arcus_mode = false;
      hyperbolic_mode = false;
      calculator_buttons[Arcus]->setChecked(false);
      calculator_buttons[Hyperbolic]->setChecked(false);
      break;

    case Cosine:  // cos
      if ( hyperbolic_mode )
      {
        if ( arcus_mode )
        {
          *x = std::log(*x + std::sqrt((*x) * (*x) - 1));

          if ( errno == EDOM || errno == ERANGE )
            error = true;

          if ( *x == INFINITY )
            error = true;
        }
        else
          *x = std::cosh(*x);
      }
      else
      {
        if ( arcus_mode )
          *x = std::acos(*x) * 180.0L/PI;
        else if ( std::fabs(std::fmod(*x - 90.0L,180.0L)) < LDBL_EPSILON )  // (x - 90)/180 == 0
          *x = 0.0L;
        else
          *x = std::cos(*x * PI/180.0L);
      }

      if ( errno == EDOM )
        error = true;

      setDisplay(*x);
      arcus_mode = false;
      hyperbolic_mode = false;
      calculator_buttons[Arcus]->setChecked(false);
      calculator_buttons[Hyperbolic]->setChecked(false);
      break;

    case Tangent:  // tan
      if ( hyperbolic_mode )
      {
        if ( arcus_mode )
          if ( *x < 1 )
          {
            *x = 0.5L * std::log((1+(*x))/(1-(*x)));

            if ( errno == EDOM || errno == ERANGE )
              error = true;
          }
          else
            error = true;
        else
          *x = std::tanh(*x);
      }
      else
      {
        if ( arcus_mode )
          *x = std::atan(*x) * 180.0L/PI;
        else
          // Test if (x/180) != 0 and x/90 == 0
          if ( std::fabs(std::fmod(*x,180.0L)) > LDBL_EPSILON
             && std::fabs(std::fmod(*x,90.0L)) < LDBL_EPSILON )
            error = true;
          else if ( std::fabs(std::fmod(*x,180.0L)) < LDBL_EPSILON )  // x/180 == 0
            *x = 0.0L;
          else
            *x = std::tan(*x * PI/180.0L);
      }

      if ( errno == EDOM )
        error = true;

      setDisplay(*x);
      arcus_mode = false;
      hyperbolic_mode = false;
      calculator_buttons[Arcus]->setChecked(false);
      calculator_buttons[Hyperbolic]->setChecked(false);
      break;

    case Reciprocal:  // 1/x
      if ( std::fabs(*x) < LDBL_EPSILON )  // x == 0
        error = true;
      else
      {
        *x = 1/(*x);
        setDisplay(*x);
      }
      break;

    case On:
      error = false;
      arcus_mode = false;
      hyperbolic_mode = false;
      calculator_buttons[Arcus]->setChecked(false);
      calculator_buttons[Hyperbolic]->setChecked(false);
      input = "";
      clearInfixOperator();
      last_infix_operator = '\0';
      a = b = 0.0L;
      break;

    case Natural_logarithm:  // ln
      *x = std::log(*x);

      if ( errno == EDOM || errno == ERANGE )
        error = true;

      setDisplay(*x);
      break;

    case Powers_of_e:  // eˣ
      *x = std::exp(*x);

      if ( errno == ERANGE )
        error = true;

      setDisplay(*x);
      break;

    case Power:  // yˣ
      if ( ! isOperatorKey(last_key) )
        calcInfixOperator();

      setDisplay(*x);
      setInfixOperator('^');
      break;

    case Square_root:  // sqrt
      *x = std::sqrt(*x);

      if ( errno == EDOM || errno == ERANGE )
        error = true;

      setDisplay(*x);
      break;

    case Divide:  // ÷
      if ( ! isOperatorKey(last_key) )
        calcInfixOperator();

      setDisplay(a);
      setInfixOperator('/');
      break;

    case Common_logarithm:  // lg
      *x = std::log10(*x);

      if ( errno == EDOM || errno == ERANGE )
        error = true;

      setDisplay(*x);
      break;

    case Powers_of_ten:  // 10ˣ
      *x = std::pow(10,*x);

      if ( errno == EDOM || errno == ERANGE )
        error = true;

      setDisplay(*x);
      break;

    case Parenthese_l:  // (
      {
        stack_data d = { a, infix_operator };
        bracket_stack.push(d);
        clearInfixOperator();
        input = "";
        a = b = 0.0L;
        setDisplay(a);
      }
      break;

    case Parenthese_r:  // )
      if ( ! bracket_stack.empty() )
      {
        calcInfixOperator();
        setDisplay(a);
        stack_data d = bracket_stack.top();
        bracket_stack.pop();
        b = d.term;
        infix_operator = d.infix_operator;
        last_infix_operator = infix_operator;
      }
      break;

    case Multiply:  // *
      if ( ! isOperatorKey(last_key) )
        calcInfixOperator();

      setDisplay(a);
      setInfixOperator('*');
      break;

    case Hyperbolic:  // hyp
      hyperbolic_mode = ! hyperbolic_mode;
      calculator_buttons[Hyperbolic]->setChecked(hyperbolic_mode);
      setDisplay(*x);
      break;

    case Seven:  // 7
      if ( input.getLength() < max_char )
      {
        if ( isDataEntryKey(last_key) )
          input += '7';
        else
          input = '7';
      }
      break;

    case Eight:  // 8
      if ( input.getLength() < max_char )
      {
        if ( isDataEntryKey(last_key) )
          input += '8';
        else
          input = '8';
      }
      break;

    case Nine:  // 9
      if ( input.getLength() < max_char )
      {
        if ( isDataEntryKey(last_key) )
          input += '9';
        else
          input = '9';
      }
      break;

    case Subtract:  // -
      if ( ! isOperatorKey(last_key) )
        calcInfixOperator();

      setDisplay(a);
      setInfixOperator('-');
      break;

    case Arcus:  // arc
      arcus_mode = ! arcus_mode;
      calculator_buttons[Arcus]->setChecked(arcus_mode);
      setDisplay(*x);
      break;

    case Four:  // 4
      if ( input.getLength() < max_char )
      {
        if ( isDataEntryKey(last_key) )
          input += '4';
        else
          input = '4';
      }
      break;

    case Five:  // 5
      if ( input.getLength() < max_char )
      {
        if ( isDataEntryKey(last_key) )
          input += '5';
        else
          input = '5';
      }
      break;

    case Six:  // 6
      if ( input.getLength() < max_char )
      {
        if ( isDataEntryKey(last_key) )
          input += '6';
        else
          input = '6';
      }
      break;

    case Add:  // +
      if ( ! isOperatorKey(last_key) )
        calcInfixOperator();

      setDisplay(a);
      setInfixOperator('+');
      break;

    case Pi:  // π
      *x = PI;
      setDisplay(*x);
      break;

    case One:  // 1
      if ( input.getLength() < max_char )
      {
        if ( isDataEntryKey(last_key) )
          input += '1';
        else
          input = '1';
      }
      break;

    case Two:  // 2
      if ( input.getLength() < max_char )
      {
        if ( isDataEntryKey(last_key) )
          input += '2';
        else
          input = '2';
      }
      break;

    case Three:  // 3
      if ( input.getLength() < max_char )
      {
        if ( isDataEntryKey(last_key) )
          input += '3';
        else
          input = '3';
      }
      break;

    case Percent:  // %
      infix_operator = last_infix_operator;
      *x /= 100.0L;
      setDisplay(*x);
      break;

    case Zero:  // 0
      if ( input.getLength() < max_char )
      {
        if ( isDataEntryKey(last_key) )
          input += '0';
        else
          input = '0';
      }
      break;

    case Decimal_point:  // .
      if ( ! input.includes('.') )
        input += '.';
      break;

    case Change_sign:  // ±
      *x *= -1.0L;
      setDisplay(*x);
      break;

    case Equals:  // =
      infix_operator = last_infix_operator;
      calcInfixOperator();
      setDisplay(a);
      break;

    default:
      break;
  } // end of switch

  if ( ! input.isEmpty() )
  {
    if ( isDataEntryKey(key) )
      *x = input.toDouble();
    else
    {
      // remove trailing zeros
      while ( ! input.includes(L'e')
            && input.includes(L'.')
            && input.back() == L'0' )
        input = input.left(input.getLength()-1);
    }
  }

  drawDispay();
  updateTerminal();

  if ( infix_operator && ! isDataEntryKey(key) )
    input = "";

  last_key = key;
}

//----------------------------------------------------------------------
void Calc::adjustSize()
{
  int pw = getParentWidget()->getWidth();
  int ph = getParentWidget()->getHeight();
  setX (1 + (pw - getWidth()) / 2, false);
  setY (1 + (ph - getHeight()) / 2, false);
  FDialog::adjustSize();
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  // Create the application object
  FApplication app(argc, argv);

  // Create a calculator object
  Calc calculator(&app);

  app.setMainWidget(&calculator);
  calculator.show();
  return app.exec();
}
