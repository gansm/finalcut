/***********************************************************************
* calculator.cpp - A simple calculator with trigonometric functions    *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2016-2018 Markus Gans                                      *
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

class Button : public finalcut::FButton
{
  public:
    // Constructor
    explicit Button (FWidget* = 0);

    // Method
    void setChecked(bool);

    // Event handler
    virtual void onKeyPress (finalcut::FKeyEvent*);

  private:
    // Data Member
    bool checked;
};
#pragma pack(pop)

//----------------------------------------------------------------------
Button::Button (finalcut::FWidget* parent)
  : finalcut::FButton(parent)
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
    setBackgroundColor(finalcut::fc::Cyan);
    setFocusForegroundColor(finalcut::fc::White);
    setFocusBackgroundColor(finalcut::fc::Cyan);
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
void Button::onKeyPress (finalcut::FKeyEvent* ev)
{
  FKey key = ev->key();

  // catch the enter key
  if ( key == finalcut::fc::Fkey_return
    || key == finalcut::fc::Fkey_enter )
    return;

  finalcut::FButton::onKeyPress(ev);
}


//----------------------------------------------------------------------
// class Calc
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class Calc : public finalcut::FDialog
{
  public:
    // Constructor
    explicit Calc (finalcut::FWidget* parent = 0);

    // Destructor
    ~Calc();

    // Event handlers
    virtual void   onKeyPress (finalcut::FKeyEvent*);
    virtual void   onAccel (finalcut::FAccelEvent*);
    virtual void   onClose (finalcut::FCloseEvent*);

    // Callback method
    void           cb_buttonClicked (finalcut::FWidget*, data_ptr);

  private:
    // Typedef and Enumeration
    typedef void (Calc::*keyFunction)(lDouble&);  // Method pointer

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
    void           drawDispay();
    virtual void   draw();
    void           clear (lDouble&);
    void           zero (lDouble&);
    void           one (lDouble&);
    void           two (lDouble&);
    void           three (lDouble&);
    void           four (lDouble&);
    void           five (lDouble&);
    void           six (lDouble&);
    void           seven (lDouble&);
    void           eight (lDouble&);
    void           nine (lDouble&);
    void           add (lDouble&);
    void           subtract (lDouble&);
    void           multiply (lDouble&);
    void           divide (lDouble&);
    void           equals (lDouble&);
    void           change_sign (lDouble&);
    void           radix_point(lDouble&);
    void           reciprocal (lDouble&);
    void           percent (lDouble&);
    void           pi (lDouble&);
    void           open_bracket (lDouble&);
    void           close_bracket (lDouble&);
    void           log_e (lDouble&);
    void           power_e (lDouble&);
    void           log_10 (lDouble&);
    void           power_10 (lDouble&);
    void           power (lDouble&);
    void           square_root (lDouble&);
    void           hyperbolic (lDouble&);
    void           arcus (lDouble&);
    void           sine (lDouble&);
    void           cosine (lDouble&);
    void           tangent (lDouble&);
    bool           isDataEntryKey (int);
    bool           isOperatorKey (int);
    lDouble&       getValue();
    void           setDisplay (lDouble);
    void           setInfixOperator (char);
    void           clearInfixOperator();
    void           calcInfixOperator();
    virtual void   adjustSize();
    const wchar_t* getButtonText (int);
    void           mapKeyFunctions();

    // Data Members
    bool              error;
    bool              arcus_mode;
    bool              hyperbolic_mode;
    lDouble           a, b;
    lDouble           infinity;
    uInt              max_char;
    int               last_key;
    char              infix_operator;
    char              last_infix_operator;
    finalcut::FString input;
    int               button_no[Calc::NUM_OF_BUTTONS];

    struct stack_data
    {
      lDouble term;
      char infix_operator;
    };

    std::stack<stack_data> bracket_stack;
    std::map<Calc::button, Button*> calculator_buttons;
    std::map<Calc::button, keyFunction> key_map;
};
#pragma pack(pop)

//----------------------------------------------------------------------
Calc::Calc (FWidget* parent)
  : finalcut::FDialog(parent)
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
  , key_map()
{
  mapKeyFunctions();
  clearInfixOperator();
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
    btn->setText(getButtonText(key));
    btn->setDoubleFlatLine(finalcut::fc::top);
    btn->setDoubleFlatLine(finalcut::fc::bottom);

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

  calculator_buttons[On]->addAccelerator(finalcut::fc::Fkey_dc);  // Del key
  calculator_buttons[On]->setFocus();
  calculator_buttons[Pi]->addAccelerator('p');
  calculator_buttons[Power]->addAccelerator('^');
  calculator_buttons[Divide]->addAccelerator('/');
  calculator_buttons[Powers_of_ten]->addAccelerator('d');
  calculator_buttons[Multiply]->addAccelerator('*');
  calculator_buttons[Decimal_point]->addAccelerator(',');
  calculator_buttons[Change_sign]->addAccelerator('#');
  calculator_buttons[Equals]->addAccelerator(finalcut::fc::Fkey_return);
  calculator_buttons[Equals]->addAccelerator(finalcut::fc::Fkey_enter);
}

//----------------------------------------------------------------------
Calc::~Calc()
{ }

//----------------------------------------------------------------------
void Calc::drawDispay()
{
  finalcut::FString display = input;

  if ( display.isNull() || display.isEmpty()  )
    display = L'0';

  if ( display.right(3) == L"-0." )
    display = L'0';

  if ( display.back() == L'.' && display.getLength() > 1 )
    display = display.left(display.getLength() - 1);

  if ( ! display.isEmpty() && display.getLength() < max_char )
    display.insert(finalcut::FString(max_char - display.getLength(), L' '), 0);

  if ( display.getLength() > max_char )
    display = display.left(max_char);

  if ( infix_operator )
    display[1] = infix_operator;

  if ( error )
    display = " Error                          ";

  setColor(finalcut::fc::Black, finalcut::fc::LightGray);

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
    wchar_t bottom_line     = wchar_t(finalcut::fc::NF_border_line_bottom);
    wchar_t top_bottom_line = wchar_t(finalcut::fc::NF_border_line_up_and_down);
    wchar_t top_line        = wchar_t(finalcut::fc::NF_border_line_upper);
    wchar_t right_line      = wchar_t(finalcut::fc::NF_rev_border_line_right);
    wchar_t left_line       = wchar_t(finalcut::fc::NF_border_line_left);
    setPrintPos (3, 2);
    print (finalcut::FString(33, bottom_line));
    setPrintPos (2, 3);
    print (right_line);
    setPrintPos (36, 3);
    print (left_line);
    setPrintPos (3, 4);
    finalcut::FString top_bottom_line_5 (5, top_bottom_line);
    finalcut::FString top_line_2 (2, top_line);
    print ( top_bottom_line_5 + top_line_2
          + top_bottom_line_5 + top_line_2
          + top_bottom_line_5 + top_line_2
          + top_bottom_line_5 + top_line_2
          + top_bottom_line_5 );
  }
  else
  {
    wchar_t vertical_and_right  = wchar_t(finalcut::fc::BoxDrawingsVerticalAndRight);
    wchar_t horizontal          = wchar_t(finalcut::fc::BoxDrawingsHorizontal);
    wchar_t vertical_and_left   = wchar_t(finalcut::fc::BoxDrawingsVerticalAndLeft);
    finalcut::FString separator = finalcut::FString(vertical_and_right)
                                + finalcut::FString(35, horizontal)
                                + finalcut::FString(vertical_and_left);
    setPrintPos (1, 4);
    print(separator);
  }
}

//----------------------------------------------------------------------
void Calc::clear (lDouble&)
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
void Calc::zero (lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '0';
  else
    input = '0';
}

//----------------------------------------------------------------------
void Calc::one (lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '1';
  else
    input = '1';
}

//----------------------------------------------------------------------
void Calc::two (lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '2';
  else
    input = '2';
}

//----------------------------------------------------------------------
void Calc::three (lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '3';
  else
    input = '3';
}

//----------------------------------------------------------------------
void Calc::four (lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '4';
  else
    input = '4';
}

//----------------------------------------------------------------------
void Calc::five (lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '5';
  else
    input = '5';
}

//----------------------------------------------------------------------
void Calc::six (lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '6';
  else
    input = '6';
}

//----------------------------------------------------------------------
void Calc::seven (lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '7';
  else
    input = '7';
}

//----------------------------------------------------------------------
void Calc::eight (lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '8';
  else
    input = '8';
}

//----------------------------------------------------------------------
void Calc::nine (lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '9';
  else
    input = '9';
}

//----------------------------------------------------------------------
void Calc::add (lDouble&)
{
  if ( ! isOperatorKey(last_key) )
    calcInfixOperator();

  setDisplay(a);
  setInfixOperator('+');
}

//----------------------------------------------------------------------
void Calc::subtract (lDouble&)
{
  if ( ! isOperatorKey(last_key) )
    calcInfixOperator();

  setDisplay(a);
  setInfixOperator('-');
}

//----------------------------------------------------------------------
void Calc::multiply (lDouble&)
{
  if ( ! isOperatorKey(last_key) )
    calcInfixOperator();

  setDisplay(a);
  setInfixOperator('*');
}

//----------------------------------------------------------------------
void Calc::divide (lDouble&)
{
  if ( ! isOperatorKey(last_key) )
    calcInfixOperator();

  setDisplay(a);
  setInfixOperator('/');
}

//----------------------------------------------------------------------
void Calc::equals (lDouble&)
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
void Calc::radix_point (lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key)
    && ! input.isNull()
    && ! input.isEmpty()
    && ! input.includes('.') )
    input += '.';
  else
    input = "0.";
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
void Calc::open_bracket (lDouble&)
{
  stack_data d = { a, infix_operator };
  bracket_stack.push(d);
  clearInfixOperator();
  input = "";
  a = b = 0.0L;
  setDisplay(a);
}

//----------------------------------------------------------------------
void Calc::close_bracket (lDouble&)
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
    {
      // Test if (x / 180) != 0 and x / 90 == 0
      if ( std::fabs(std::fmod(x, 180.0L)) > LDBL_EPSILON
        && std::fabs(std::fmod(x, 90.0L)) < LDBL_EPSILON )
        error = true;
      else if ( std::fabs(std::fmod(x, 180.0L)) < LDBL_EPSILON )  // x / 180 == 0
        x = 0.0L;
      else
        x = std::tan(x * PI / 180.0L);
    }
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
  setColor (finalcut::fc::Blue, finalcut::fc::Cyan);
  clearArea (vdesktop, finalcut::fc::MediumShade);
  unsetBold();
  finalcut::FDialog::draw();
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
void Calc::onKeyPress (finalcut::FKeyEvent* ev)
{
  std::size_t len = input.getLength();
  FKey key = ev->key();

  switch ( key )
  {
    case finalcut::fc::Fkey_erase:
    case finalcut::fc::Fkey_backspace:
      if ( len > 0 )
      {
        lDouble& x = getValue();

        if ( len == 1 )
        {
          input = "";
          x = 0.0L;
        }
        else
        {
          input = input.left(input.getLength() - 1);
          x = lDouble(std::atof(input.c_str()));
        }

        drawDispay();
        updateTerminal();
      }

      ev->accept();
      break;

    case finalcut::fc::Fkey_escape:
    case finalcut::fc::Fkey_escape_mintty:
      {
        finalcut::FAccelEvent a_ev( finalcut::fc::Accelerator_Event
                                  , getFocusWidget() );
        calculator_buttons[On]->onAccel(&a_ev);
      }
      ev->accept();
      break;

    default:
      finalcut::FDialog::onKeyPress(ev);
      break;
  }
}

//----------------------------------------------------------------------
void Calc::onAccel (finalcut::FAccelEvent* ev)
{
  close();
  ev->accept();
}

//----------------------------------------------------------------------
void Calc::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void Calc::cb_buttonClicked (finalcut::FWidget*, data_ptr data)
{
  lDouble& x = getValue();
  Calc::button key = *(static_cast<Calc::button*>(data));

  // Call the key function
  (this->*key_map[key])(x);

  if ( ! input.isEmpty() )
  {
    if ( isDataEntryKey(key) )
      x = lDouble(input.toDouble());
    else
    {
      // Remove trailing zeros
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
  std::size_t pw = getParentWidget()->getWidth();
  std::size_t ph = getParentWidget()->getHeight();
  setX (1 + int(pw - getWidth()) / 2, false);
  setY (1 + int(ph - getHeight()) / 2, false);
  finalcut::FDialog::adjustSize();
}

//----------------------------------------------------------------------
const wchar_t* Calc::getButtonText (int key)
{
  static const wchar_t* const button_text[Calc::NUM_OF_BUTTONS] =
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

  return button_text[key];
}

//----------------------------------------------------------------------
void Calc::mapKeyFunctions()
{
  key_map[Sine] = &Calc::sine;                   // sin
  key_map[Cosine] = &Calc::cosine;               // cos
  key_map[Tangent] = &Calc::tangent;             // tan
  key_map[Reciprocal] = &Calc::reciprocal;       // 1/x
  key_map[On] = &Calc::clear;                    // On
  key_map[Natural_logarithm] = &Calc::log_e;     // ln
  key_map[Powers_of_e] = &Calc::power_e;         // eˣ
  key_map[Power] = &Calc::power;                 // yˣ
  key_map[Square_root] = &Calc::square_root;     // sqrt
  key_map[Divide] = &Calc::divide;               // ÷
  key_map[Common_logarithm] = &Calc::log_10;     // lg
  key_map[Powers_of_ten] = &Calc::power_10;      // 10ˣ
  key_map[Parenthese_l] = &Calc::open_bracket;   // (
  key_map[Parenthese_r] = &Calc::close_bracket;  // )
  key_map[Multiply] = &Calc::multiply;           // *
  key_map[Hyperbolic] = &Calc::hyperbolic;       // hyp
  key_map[Seven] = &Calc::seven;                 // 7
  key_map[Eight] = &Calc::eight;                 // 8
  key_map[Nine] = &Calc::nine;                   // 9
  key_map[Subtract] = &Calc::subtract;           // -
  key_map[Arcus] = &Calc::arcus;                 // arc
  key_map[Four] = &Calc::four;                   // 4
  key_map[Five] = &Calc::five;                   // 5
  key_map[Six] = &Calc::six;                     // 6
  key_map[Add] = &Calc::add;                     // +
  key_map[Pi] = &Calc::pi;                       // π
  key_map[One] = &Calc::one;                     // 1
  key_map[Two] = &Calc::two;                     // 2
  key_map[Three] = &Calc::three;                 // 3
  key_map[Percent] = &Calc::percent;             // %
  key_map[Zero] = &Calc::zero;                   // 0
  key_map[Decimal_point] = &Calc::radix_point;   // .
  key_map[Change_sign] = &Calc::change_sign;     // ±
  key_map[Equals] = &Calc::equals;               // =
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  // Create the application object
  finalcut::FApplication app(argc, argv);

  // Create a calculator object
  Calc calculator(&app);

  // Set calculator object as main widget
  app.setMainWidget(&calculator);

  // Show and start the application
  calculator.show();
  return app.exec();
}
