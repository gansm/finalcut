/***********************************************************************
* calculator.cpp - A simple calculator with trigonometric functions    *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2016-2017 Markus Gans                                      *
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

#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <map>
#include <stack>

#include <final/final.h>

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
    explicit Calc (FWidget* parent = 0);

    // Destructor
    ~Calc();

    // Event handlers
    void     onKeyPress (FKeyEvent*);
    void     onAccel (FAccelEvent*);
    void     onClose (FCloseEvent*);

    // Callback method
    void     cb_buttonClicked (FWidget*, data_ptr);

  private:
    // Enumeration
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

    // Methods
    void     drawDispay();
    virtual void draw();
    void     clear();
    void     zero();
    void     one();
    void     two();
    void     three();
    void     four();
    void     five();
    void     six();
    void     seven();
    void     eight();
    void     nine();
    void     add();
    void     subtract();
    void     multiply();
    void     divide();
    void     equals();
    void     change_sign (lDouble&);
    void     radix_point();
    void     reciprocal (lDouble&);
    void     percent (lDouble&);
    void     pi (lDouble&);
    void     open_bracket();
    void     close_bracket();
    void     log_e (lDouble&);
    void     power_e (lDouble&);
    void     log_10 (lDouble&);
    void     power_10 (lDouble&);
    void     power (lDouble&);
    void     square_root (lDouble&);
    void     hyperbolic (lDouble&);
    void     arcus (lDouble&);
    void     sine (lDouble&);
    void     cosine (lDouble&);
    void     tangent (lDouble&);
    bool     isDataEntryKey (int);
    bool     isOperatorKey (int);
    lDouble& getValue();
    void     setDisplay (lDouble);
    void     setInfixOperator (char);
    void     clearInfixOperator();
    void     calcInfixOperator();
    void     adjustSize();

    // Data Members
    bool error;
    bool arcus_mode;
    bool hyperbolic_mode;

    lDouble a, b;
    lDouble infinity;
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
  , infinity(std::numeric_limits<lDouble>::infinity())
  , max_char(33)
  , last_key(-1)
  , infix_operator('\0')
  , last_infix_operator('\0')
  , input("")
  , bracket_stack()
  , calculator_buttons()
{
  clearInfixOperator();

  const wchar_t* const button_text[Calc::NUM_OF_BUTTONS] =
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
  setText ("Calculator");
  setGeometry (19, 6, 37, 18);
  addAccelerator('q');  // Press 'q' to quit

  for (int key = 0; key < Calc::NUM_OF_BUTTONS; key++)
  {
    Button* btn = new Button(this);
    button_no[key] = key;

    if ( key == Equals )
      btn->setGeometry(30, 15, 5, 3);
    else
    {
      int x, y, n;
      ( key <= Three ) ? n = 0 : n = 1;
      x = (key + n) % 5 * 7 + 2;
      y = (key + n) / 5 * 2 + 3;
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
      F_METHOD_CALLBACK (this, &Calc::cb_buttonClicked),
      &button_no[key]
    );

    calculator_buttons[button(key)] = btn;
  }

  calculator_buttons[On]->addAccelerator(fc::Fkey_dc);  // Del key
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

  setPrintPos (3, 3);
  print(display);
  print(L' ');
  setColor(wc.dialog_fg, wc.dialog_bg);

  if ( isMonochron() )
    setReverse(true);

  if ( isNewFont() )
  {
    FString bottom_line (33, wchar_t(fc::NF_border_line_bottom));
    setPrintPos (3, 2);
    print (bottom_line);
    setPrintPos (2, 3);
    print (wchar_t(fc::NF_rev_border_line_right));
    setPrintPos (36, 3);
    print (wchar_t(fc::NF_border_line_left));
    FString top_bottom_line_5 (5, wchar_t(fc::NF_border_line_up_and_down));
    FString top_line_2 (2, wchar_t(fc::NF_border_line_upper));
    setPrintPos (3, 4);
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
    setPrintPos (1, 4);
    print(separator);
  }
}

//----------------------------------------------------------------------
void Calc::clear()
{
  error = false;
  arcus_mode = false;
  hyperbolic_mode = false;
  calculator_buttons[Arcus]->setChecked(false);
  calculator_buttons[Hyperbolic]->setChecked(false);
  input = "";
  clearInfixOperator();
  last_infix_operator = '\0';
  a = b = 0.0L;
}

//----------------------------------------------------------------------
void Calc::zero()
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '0';
  else
    input = '0';

}

//----------------------------------------------------------------------
void Calc::one()
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '1';
  else
    input = '1';
}

//----------------------------------------------------------------------
void Calc::two()
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '2';
  else
    input = '2';
}

//----------------------------------------------------------------------
void Calc::three()
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '3';
  else
    input = '3';
}

//----------------------------------------------------------------------
void Calc::four()
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '4';
  else
    input = '4';
}

//----------------------------------------------------------------------
void Calc::five()
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '5';
  else
    input = '5';
}

//----------------------------------------------------------------------
void Calc::six()
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '6';
  else
    input = '6';
}

//----------------------------------------------------------------------
void Calc::seven()
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '7';
  else
    input = '7';
}

//----------------------------------------------------------------------
void Calc::eight()
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '8';
  else
    input = '8';
}

//----------------------------------------------------------------------
void Calc::nine()
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '9';
  else
    input = '9';
}

//----------------------------------------------------------------------
void Calc::add()
{
  if ( ! isOperatorKey(last_key) )
    calcInfixOperator();

  setDisplay(a);
  setInfixOperator('+');
}

//----------------------------------------------------------------------
void Calc::subtract()
{
  if ( ! isOperatorKey(last_key) )
    calcInfixOperator();

  setDisplay(a);
  setInfixOperator('-');
}

//----------------------------------------------------------------------
void Calc::multiply()
{
 if ( ! isOperatorKey(last_key) )
   calcInfixOperator();

  setDisplay(a);
  setInfixOperator('*');
}

//----------------------------------------------------------------------
void Calc::divide()
{
  if ( ! isOperatorKey(last_key) )
    calcInfixOperator();

  setDisplay(a);
  setInfixOperator('/');
}

//----------------------------------------------------------------------
void Calc::equals()
{
  infix_operator = last_infix_operator;
  calcInfixOperator();
  setDisplay(a);
}

//----------------------------------------------------------------------
void Calc::change_sign (lDouble& x)
{
  x *= -1.0L;
  setDisplay(x);
}

//----------------------------------------------------------------------
void Calc::radix_point()
{
  if ( ! input.includes('.') )
    input += '.';
}

//----------------------------------------------------------------------
void Calc::reciprocal (lDouble& x)
{
  if ( std::fabs(x) < LDBL_EPSILON )  // x == 0
    error = true;
  else
  {
    x = 1 / x;
    setDisplay(x);
  }
}

//----------------------------------------------------------------------
void Calc::percent (lDouble& x)
{
  infix_operator = last_infix_operator;
  x /= 100.0L;
  setDisplay(x);
}

//----------------------------------------------------------------------
void Calc::pi (lDouble& x)
{
  x = PI;
  setDisplay(x);
}

//----------------------------------------------------------------------
void Calc::open_bracket()
{
  stack_data d = { a, infix_operator };
  bracket_stack.push(d);
  clearInfixOperator();
  input = "";
  a = b = 0.0L;
  setDisplay(a);
}

//----------------------------------------------------------------------
void Calc::close_bracket()
{
  if ( bracket_stack.empty() )
    return;

  calcInfixOperator();
  setDisplay(a);
  stack_data d = bracket_stack.top();
  bracket_stack.pop();
  b = d.term;
  infix_operator = d.infix_operator;
  last_infix_operator = infix_operator;
}

//----------------------------------------------------------------------
void Calc::log_e (lDouble& x)
{
  x = std::log(x);

  if ( errno == EDOM || errno == ERANGE )
    error = true;

  setDisplay(x);
}

//----------------------------------------------------------------------
void Calc::power_e (lDouble& x)
{
  x = std::exp(x);

  if ( errno == ERANGE )
    error = true;

  setDisplay(x);
}

//----------------------------------------------------------------------
void Calc::log_10 (lDouble& x)
{
  x = std::log10(x);

  if ( errno == EDOM || errno == ERANGE )
    error = true;

  setDisplay(x);
}

//----------------------------------------------------------------------
void Calc::power_10 (lDouble& x)
{
  x = std::pow(10, x);

  if ( errno == EDOM || errno == ERANGE )
    error = true;

  setDisplay(x);
}

//----------------------------------------------------------------------
void Calc::power (lDouble& x)
{
  if ( ! isOperatorKey(last_key) )
    calcInfixOperator();

  setDisplay(x);
  setInfixOperator('^');
}

//----------------------------------------------------------------------
void Calc::square_root (lDouble& x)
{
  x = std::sqrt(x);

  if ( errno == EDOM || errno == ERANGE )
    error = true;

  setDisplay(x);
}

//----------------------------------------------------------------------
void Calc::hyperbolic (lDouble& x)
{
  hyperbolic_mode = ! hyperbolic_mode;
  calculator_buttons[Hyperbolic]->setChecked(hyperbolic_mode);
  setDisplay(x);
}

//----------------------------------------------------------------------
void Calc::arcus (lDouble& x)
{
  arcus_mode = ! arcus_mode;
  calculator_buttons[Arcus]->setChecked(arcus_mode);
  setDisplay(x);
}

//----------------------------------------------------------------------
void Calc::sine (lDouble& x)
{
 if ( hyperbolic_mode )
  {
    if ( arcus_mode )
    {
      x = std::log(x + std::sqrt(x * x + 1));

      if ( errno == EDOM || errno == ERANGE )
        error = true;

      if ( std::fabs(x - infinity) < LDBL_EPSILON )  // x = ∞
        error = true;
    }
    else
      x = std::sinh(x);
  }
  else
  {
    if ( arcus_mode )
      x = std::asin(x) * 180.0L / PI;
    else if ( std::fabs(std::fmod(x, 180.0L)) < LDBL_EPSILON )  // x / 180 = 0
      x = 0.0L;
    else
      x = std::sin(x * PI / 180.0L);
  }

  if ( errno == EDOM )
    error = true;

  setDisplay(x);
  arcus_mode = false;
  hyperbolic_mode = false;
  calculator_buttons[Arcus]->setChecked(false);
  calculator_buttons[Hyperbolic]->setChecked(false);
}

//----------------------------------------------------------------------
void Calc::cosine (lDouble& x)
{
  if ( hyperbolic_mode )
  {
    if ( arcus_mode )
    {
      x = std::log(x + std::sqrt(x * x - 1));

      if ( errno == EDOM || errno == ERANGE )
        error = true;

      if ( std::fabs(x - infinity) < LDBL_EPSILON )  // x = ∞
        error = true;
    }
    else
      x = std::cosh(x);
  }
  else
  {
    if ( arcus_mode )
      x = std::acos(x) * 180.0L / PI;
    else if ( std::fabs(std::fmod(x - 90.0L, 180.0L)) < LDBL_EPSILON )  // (x - 90) / 180 == 0
      x = 0.0L;
    else
      x = std::cos(x * PI / 180.0L);
  }

  if ( errno == EDOM )
    error = true;

  setDisplay(x);
  arcus_mode = false;
  hyperbolic_mode = false;
  calculator_buttons[Arcus]->setChecked(false);
  calculator_buttons[Hyperbolic]->setChecked(false);
}

//----------------------------------------------------------------------
void Calc::tangent (lDouble& x)
{
  if ( hyperbolic_mode )
  {
    if ( arcus_mode )
      if ( x < 1 )
      {
        x = 0.5L * std::log((1 + x) / (1 - x));

        if ( errno == EDOM || errno == ERANGE )
          error = true;
      }
      else
        error = true;
    else
      x = std::tanh(x);
  }
  else
  {
    if ( arcus_mode )
      x = std::atan(x) * 180.0L / PI;
    else
      // Test if (x / 180) != 0 and x / 90 == 0
      if ( std::fabs(std::fmod(x, 180.0L)) > LDBL_EPSILON
        && std::fabs(std::fmod(x, 90.0L)) < LDBL_EPSILON )
        error = true;
      else if ( std::fabs(std::fmod(x, 180.0L)) < LDBL_EPSILON )  // x / 180 == 0
        x = 0.0L;
      else
        x = std::tan(x * PI / 180.0L);
  }

  if ( errno == EDOM )
    error = true;

  setDisplay(x);
  arcus_mode = false;
  hyperbolic_mode = false;
  calculator_buttons[Arcus]->setChecked(false);
  calculator_buttons[Hyperbolic]->setChecked(false);
}

//----------------------------------------------------------------------
void Calc::draw()
{
  setBold();
  setColor (fc::Blue, fc::Cyan);
  clearArea (vdesktop, fc::MediumShade);
  unsetBold();
  FDialog::draw();
  drawDispay();
}

//----------------------------------------------------------------------
bool Calc::isDataEntryKey (int key)
{
  // Test if key is in {'.', '0'..'9'}
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

  int* iter = std::find (data_entry_keys, data_entry_keys + 11, key);

  if ( iter != data_entry_keys + 11 )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
bool Calc::isOperatorKey(int key)
{
  // Test if key is in {'*', '/', '+', '-', '^', '='}
  int operators[] =
  {
    Multiply,
    Divide,
    Add,
    Subtract,
    Power,
    Equals
  };

  int* iter = std::find (operators, operators + 6, key);

  if ( iter != operators + 6 )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
lDouble& Calc::getValue()
{
  if ( infix_operator )
    return b;
  else
    return a;
}

//----------------------------------------------------------------------
void Calc::setDisplay (lDouble d)
{
  char buffer[33];
  snprintf (buffer, sizeof(buffer), "%32.11Lg", d);
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
        if ( std::log(std::abs(a)) + std::log(std::abs(1 + b / a)) <= std::log(LDBL_MAX) )
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
        if ( std::log(std::abs(a)) + std::log(std::abs(1 - b / a)) <= std::log(LDBL_MAX) )
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

        a = lDouble(std::atof(input.c_str()));
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
  FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void Calc::cb_buttonClicked (FWidget*, data_ptr data)
{
  lDouble& x = getValue();
  int key = *(static_cast<int*>(data));

  switch ( key )
  {
    case Sine:
      sine(x);          // sin
      break;

    case Cosine:
      cosine(x);        // cos
      break;

    case Tangent:
      tangent(x);       // tan
      break;

    case Reciprocal:
      reciprocal(x);    // 1 / x
      break;

    case On:
      clear();
      break;

    case Natural_logarithm:
      log_e(x);         // ln
      break;

    case Powers_of_e:
      power_e(x);       // eˣ
      break;

    case Power:
      power(x);         // yˣ
      break;

    case Square_root:
      square_root(x);   // sqrt
      break;

    case Divide:
      divide();         // ÷
      break;

    case Common_logarithm:
      log_10(x);        // lg
      break;

    case Powers_of_ten:
      power_10(x);      // 10ˣ
      break;

    case Parenthese_l:
      open_bracket();   // (
      break;

    case Parenthese_r:
      close_bracket();  // )
      break;

    case Multiply:
      multiply();       // *
      break;

    case Hyperbolic:
      hyperbolic(x);    // hyp
      break;

    case Seven:
      seven();          // 7
      break;

    case Eight:
      eight();          // 8
      break;

    case Nine:
      nine();           // 9
      break;

    case Subtract:
      subtract();       // -
      break;

    case Arcus:
      arcus(x);         // arc
      break;

    case Four:
      four();           // 4
      break;

    case Five:
      five();           // 5
      break;

    case Six:
      six();            // 6
      break;

    case Add:
      add();            // +
      break;

    case Pi:
      pi(x);            // π
      break;

    case One:
      one();            // 1
      break;

    case Two:
      two();            // 2
      break;

    case Three:
      three();          // 3
      break;

    case Percent:
      percent(x);       // %
      break;

    case Zero:
      zero();           // 0
      break;

    case Decimal_point:
      radix_point();    // .
      break;

    case Change_sign:
      change_sign(x);   // ±
      break;

    case Equals:
      equals();         // =
      break;

    default:
      break;
  }  // end of switch

  if ( ! input.isEmpty() )
  {
    if ( isDataEntryKey(key) )
      x = lDouble(input.toDouble());
    else
    {
      // remove trailing zeros
      while ( ! input.includes(L'e')
           && input.includes(L'.')
           && input.back() == L'0' )
        input = input.left(input.getLength() - 1);
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

  // Set calculator object as main widget
  app.setMainWidget(&calculator);

  // Show and start the application
  calculator.show();
  return app.exec();
}
