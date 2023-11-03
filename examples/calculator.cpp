/***********************************************************************
* calculator.cpp - A simple calculator with trigonometric functions    *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2016-2023 Markus Gans                                      *
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

#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <array>
#include <limits>
#include <map>
#include <memory>
#include <stack>

#include <final/final.h>

using FKey = finalcut::FKey;
using finalcut::FColorPair;
using finalcut::FColor;
using finalcut::FPoint;
using finalcut::FRect;
using finalcut::FSize;

template <typename T>
constexpr T pi_value = T{3.141'592'653'589'793'238L};


//----------------------------------------------------------------------
// class Button
//----------------------------------------------------------------------

class Button final : public finalcut::FButton
{
  public:
    // Constructor
    explicit Button (FWidget* = nullptr);

    // Method
    void setChecked(bool);

    // Event handler
    void onKeyPress (finalcut::FKeyEvent*) override;

  private:
    // Data member
    bool checked{false};
};

//----------------------------------------------------------------------
Button::Button (finalcut::FWidget* parent)
  : finalcut::FButton{parent}
{ }

//----------------------------------------------------------------------
void Button::setChecked (bool enable)
{
  if ( checked == enable )
    return;

  checked = enable;

  if ( checked )
  {
    setBackgroundColor(FColor::Cyan);
    setFocusForegroundColor(FColor::White);
    setFocusBackgroundColor(FColor::Cyan);
  }
  else
  {
    const auto& wc = getColorTheme();
    setBackgroundColor(wc->button.bg);
    setFocusForegroundColor(wc->button.focus_fg);
    setFocusBackgroundColor(wc->button.focus_bg);
  }

  redraw();
}

//----------------------------------------------------------------------
void Button::onKeyPress (finalcut::FKeyEvent* ev)
{
  const FKey key = ev->key();

  // catch the enter key
  if ( isEnterKey(key) )
    return;

  finalcut::FButton::onKeyPress(ev);
}


//----------------------------------------------------------------------
// class Calc
//----------------------------------------------------------------------

class Calc final : public finalcut::FDialog
{
  public:
    // Constructor
    explicit Calc (finalcut::FWidget* parent = nullptr);

  private:
    // Typedef and Enumeration
    using keyFunction = std::function<void(lDouble&)>;  // Member function

    enum class ButtonName
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
    void drawDispay();
    void draw() override;
    void sendOnButtonAccelerator();
    void clear (const lDouble&);
    void zero (const lDouble&);
    void one (const lDouble&);
    void two (const lDouble&);
    void three (const lDouble&);
    void four (const lDouble&);
    void five (const lDouble&);
    void six (const lDouble&);
    void seven (const lDouble&);
    void eight (const lDouble&);
    void nine (const lDouble&);
    void add (const lDouble&);
    void subtract (const lDouble&);
    void multiply (const lDouble&);
    void divide (const lDouble&);
    void equals (const lDouble&);
    void change_sign (lDouble&);
    void radix_point(const lDouble&);
    void reciprocal (lDouble&);
    void percent (lDouble&);
    void pi (lDouble&);
    void open_bracket (const lDouble&);
    void close_bracket (const lDouble&);
    void log_e (lDouble&);
    void power_e (lDouble&);
    void log_10 (lDouble&);
    void power_10 (lDouble&);
    void power (const lDouble&);
    void square_root (lDouble&);
    void hyperbolic (const lDouble&);
    void arcus (const lDouble&);
    void sine (lDouble&);
    void cosine (lDouble&);
    void tangent (lDouble&);
    auto isDataEntryKey (const ButtonName&) const -> bool;
    auto isOperatorKey (const ButtonName&) const -> bool;
    auto getValue() -> lDouble&;
    void setDisplay (lDouble);
    void setInfixOperator (char);
    void clearInfixOperator();
    void calcMultiplication();
    void calcDivision();
    void calcAddition();
    void calcSubtraction();
    void calcExponentiation();
    void calcInfixOperator();
    void initLayout() override;
    void adjustSize() override;
    auto getButtonText (const ButtonName&) const -> const wchar_t*;
    void mapKeyFunctions();

    // Event handlers
    void onKeyPress (finalcut::FKeyEvent*) override;
    void onShow (finalcut::FShowEvent*) override;
    void onClose (finalcut::FCloseEvent*) override;

    // Callback method
    void           cb_buttonClicked (ButtonName);

    // Overloaded operators
    friend auto operator <  (const ButtonName& c, const int n) noexcept -> bool;
    friend auto operator <= (const ButtonName& c, const int n) noexcept -> bool;
    friend auto operator +  (const ButtonName& c, const int n) noexcept -> ButtonName;
    friend auto operator ++ (ButtonName& c) noexcept -> ButtonName&;  // prefix
    friend auto operator ++ (ButtonName& c, int) noexcept -> ButtonName;  // postfix

    // Data members
    bool              error{false};
    bool              arcus_mode{false};
    bool              hyperbolic_mode{false};
    lDouble           a{0.0L};
    lDouble           b{0.0L};
    lDouble           infinity{std::numeric_limits<lDouble>::infinity()};
    uInt              max_char{33};
    ButtonName        last_key{ButtonName(-1)};
    char              infix_operator{'\0'};
    char              last_infix_operator{'\0'};
    finalcut::FString input{""};
    std::array<ButtonName, std::size_t(ButtonName::NUM_OF_BUTTONS)> button_no{};

    struct StackData
    {
      lDouble term;
      char infix_operator;
    };

    std::stack<StackData> bracket_stack{};
    std::map<ButtonName, std::shared_ptr<Button> > calculator_buttons{};
    std::map<ButtonName, keyFunction> key_map{};
};

//----------------------------------------------------------------------
Calc::Calc (FWidget* parent)
  : finalcut::FDialog{parent}
{
  mapKeyFunctions();
  clearInfixOperator();

  for (ButtonName key{ButtonName::Sine}; key < ButtonName::NUM_OF_BUTTONS; key++)
  {
    auto btn = std::make_shared<Button>(this);
    auto index = std::size_t(key);
    button_no[index] = key;
    btn->setFlat();
    btn->setNoUnderline();
    btn->setText(getButtonText(key));

    if ( finalcut::FVTerm::getFOutput()->isNewFont() )
      btn->unsetClickAnimation();

    btn->addCallback
    (
      "clicked",
      this, &Calc::cb_buttonClicked,
      button_no[index]
    );

    calculator_buttons[ButtonName(key)] = btn;
  }

  calculator_buttons[ButtonName::On]->addAccelerator(FKey::Del_char);
  calculator_buttons[ButtonName::On]->setFocus();
  calculator_buttons[ButtonName::Pi]->addAccelerator(FKey('p'));
  calculator_buttons[ButtonName::Power]->addAccelerator(FKey('^'));
  calculator_buttons[ButtonName::Divide]->addAccelerator(FKey('/'));
  calculator_buttons[ButtonName::Powers_of_ten]->addAccelerator(FKey('d'));
  calculator_buttons[ButtonName::Multiply]->addAccelerator(FKey('*'));
  calculator_buttons[ButtonName::Decimal_point]->addAccelerator(FKey(','));
  calculator_buttons[ButtonName::Change_sign]->addAccelerator(FKey('#'));
  calculator_buttons[ButtonName::Equals]->addAccelerator(FKey::Return);
  calculator_buttons[ButtonName::Equals]->addAccelerator(FKey::Enter);
}

//----------------------------------------------------------------------
void Calc::onKeyPress (finalcut::FKeyEvent* ev)
{
  const std::size_t len = input.getLength();
  const FKey key = ev->key();

  if ( key == FKey::Erase || key == FKey::Backspace )
  {
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
        x = std::strtold(input.c_str(), nullptr);
      }

      drawDispay();
    }

    ev->accept();
  }
  else if ( isEscapeKey(key) )
  {
    sendOnButtonAccelerator();
    ev->accept();
  }
  else if ( key == FKey('q') )
  {
    close();
    ev->accept();
  }
  else
    finalcut::FDialog::onKeyPress(ev);
}

//----------------------------------------------------------------------
void Calc::onShow (finalcut::FShowEvent*)
{
  // Overwrites the initialized value of LC_NUMERIC
  std::setlocale(LC_NUMERIC, "C");
}

//----------------------------------------------------------------------
void Calc::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void Calc::cb_buttonClicked (ButtonName key)
{
  lDouble& x = getValue();

  // Call the key function
  key_map[key](x);

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

  if ( infix_operator && ! isDataEntryKey(key) )
    input = "";

  last_key = key;
}

//----------------------------------------------------------------------
void Calc::drawDispay()
{
  finalcut::FString display{input};

  if ( display.isEmpty() )
    display = L'0';

  if ( display.right(3) == L"-0." )
    display = L'0';

  if ( display.back() == L'.' && display.getLength() > 1 )
    display = display.left(display.getLength() - 1);

  if ( ! display.isEmpty() && display.getLength() < max_char )
    display.insert(finalcut::FString{max_char - display.getLength(), L' '}, 0);

  if ( display.getLength() > max_char )
    display = display.left(max_char);

  if ( infix_operator )
    display[1] = infix_operator;

  if ( error )
    display = " Error                          ";

  if ( finalcut::FVTerm::getFOutput()->isMonochron() )
    setReverse(false);

  const auto& wc = getColorTheme();
  print() << FColorPair{FColor::Black, FColor::LightGray}
          << FPoint{3, 3} << display << ' '
          << FColorPair{wc->dialog.fg, wc->dialog.bg};

  if ( finalcut::FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  if ( finalcut::FVTerm::getFOutput()->isNewFont() )
  {
    const auto bottom_line     {finalcut::UniChar::NF_border_line_bottom};
    const auto top_bottom_line {finalcut::UniChar::NF_border_line_up_and_down};
    const auto top_line        {finalcut::UniChar::NF_border_line_upper};
    const auto right_line      {finalcut::UniChar::NF_rev_border_line_right};
    const auto left_line       {finalcut::UniChar::NF_border_line_left};
    print() << FPoint{3, 2} << finalcut::FString{33, bottom_line};
    print() << FPoint{2, 3} << right_line;
    print() << FPoint{36, 3} << left_line;
    print() << FPoint{3, 4};
    const finalcut::FString top_bottom_line_5 {5, top_bottom_line};
    const finalcut::FString top_line_2 {2, top_line};
    print ( top_bottom_line_5 + top_line_2
          + top_bottom_line_5 + top_line_2
          + top_bottom_line_5 + top_line_2
          + top_bottom_line_5 + top_line_2
          + top_bottom_line_5 );
  }
  else
  {
    const auto vertical_and_right {finalcut::UniChar::BoxDrawingsVerticalAndRight};
    const auto horizontal         {finalcut::UniChar::BoxDrawingsHorizontal};
    const auto vertical_and_left  {finalcut::UniChar::BoxDrawingsVerticalAndLeft};
    finalcut::FString separator ( finalcut::FString{vertical_and_right}
                                + finalcut::FString{35, horizontal}
                                + finalcut::FString{vertical_and_left} );
    print() << FPoint{1, 4} << separator;
  }
}

//----------------------------------------------------------------------
inline void Calc::sendOnButtonAccelerator()
{
  finalcut::FAccelEvent a_ev(finalcut::Event::Accelerator, getFocusWidget());
  calculator_buttons[ButtonName::On]->onAccel(&a_ev);
}

//----------------------------------------------------------------------
void Calc::clear (const lDouble&)
{
  error = false;
  arcus_mode = false;
  hyperbolic_mode = false;
  calculator_buttons[ButtonName::Arcus]->setChecked(false);
  calculator_buttons[ButtonName::Hyperbolic]->setChecked(false);
  input = "";
  clearInfixOperator();
  last_infix_operator = '\0';
  a = b = 0.0L;
}

//----------------------------------------------------------------------
void Calc::zero (const lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '0';
  else
    input = '0';
}

//----------------------------------------------------------------------
void Calc::one (const lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '1';
  else
    input = '1';
}

//----------------------------------------------------------------------
void Calc::two (const lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '2';
  else
    input = '2';
}

//----------------------------------------------------------------------
void Calc::three (const lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '3';
  else
    input = '3';
}

//----------------------------------------------------------------------
void Calc::four (const lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '4';
  else
    input = '4';
}

//----------------------------------------------------------------------
void Calc::five (const lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '5';
  else
    input = '5';
}

//----------------------------------------------------------------------
void Calc::six (const lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '6';
  else
    input = '6';
}

//----------------------------------------------------------------------
void Calc::seven (const lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '7';
  else
    input = '7';
}

//----------------------------------------------------------------------
void Calc::eight (const lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '8';
  else
    input = '8';
}

//----------------------------------------------------------------------
void Calc::nine (const lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '9';
  else
    input = '9';
}

//----------------------------------------------------------------------
void Calc::add (const lDouble&)
{
  if ( ! isOperatorKey(last_key) )
    calcInfixOperator();

  setDisplay(a);
  setInfixOperator('+');
}

//----------------------------------------------------------------------
void Calc::subtract (const lDouble&)
{
  if ( ! isOperatorKey(last_key) )
    calcInfixOperator();

  setDisplay(a);
  setInfixOperator('-');
}

//----------------------------------------------------------------------
void Calc::multiply (const lDouble&)
{
  if ( ! isOperatorKey(last_key) )
    calcInfixOperator();

  setDisplay(a);
  setInfixOperator('*');
}

//----------------------------------------------------------------------
void Calc::divide (const lDouble&)
{
  if ( ! isOperatorKey(last_key) )
    calcInfixOperator();

  setDisplay(a);
  setInfixOperator('/');
}

//----------------------------------------------------------------------
void Calc::equals (const lDouble&)
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
void Calc::radix_point (const lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key)
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
  x = pi_value<lDouble>;
  setDisplay(x);
}

//----------------------------------------------------------------------
void Calc::open_bracket (const lDouble&)
{
  const StackData d{ a, infix_operator };
  bracket_stack.push(d);
  clearInfixOperator();
  input = "";
  a = b = 0.0L;
  setDisplay(a);
}

//----------------------------------------------------------------------
void Calc::close_bracket (const lDouble&)
{
  if ( bracket_stack.empty() )
    return;

  calcInfixOperator();
  setDisplay(a);
  const StackData d = bracket_stack.top();
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
void Calc::power (const lDouble& x)
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
void Calc::hyperbolic (const lDouble& x)
{
  hyperbolic_mode = ! hyperbolic_mode;
  calculator_buttons[ButtonName::Hyperbolic]->setChecked(hyperbolic_mode);
  setDisplay(x);
}

//----------------------------------------------------------------------
void Calc::arcus (const lDouble& x)
{
  arcus_mode = ! arcus_mode;
  calculator_buttons[ButtonName::Arcus]->setChecked(arcus_mode);
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
      x = std::asin(x) * 180.0L / pi_value<lDouble>;
    else if ( std::fabs(std::fmod(x, 180.0L)) < LDBL_EPSILON )  // x / 180 = 0
      x = 0.0L;
    else
      x = std::sin(x * pi_value<lDouble> / 180.0L);
  }

  if ( errno == EDOM )
    error = true;

  setDisplay(x);
  arcus_mode = false;
  hyperbolic_mode = false;
  calculator_buttons[ButtonName::Arcus]->setChecked(false);
  calculator_buttons[ButtonName::Hyperbolic]->setChecked(false);
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
      x = std::acos(x) * 180.0L / pi_value<lDouble>;
    else if ( std::fabs(std::fmod(x - 90.0L, 180.0L)) < LDBL_EPSILON )  // (x - 90) / 180 == 0
      x = 0.0L;
    else
      x = std::cos(x * pi_value<lDouble> / 180.0L);
  }

  if ( errno == EDOM )
    error = true;

  setDisplay(x);
  arcus_mode = false;
  hyperbolic_mode = false;
  calculator_buttons[ButtonName::Arcus]->setChecked(false);
  calculator_buttons[ButtonName::Hyperbolic]->setChecked(false);
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
      }
      else
        error = true;
    else
      x = std::tanh(x);
  }
  else
  {
    if ( arcus_mode )
      x = std::atan(x) * 180.0L / pi_value<lDouble>;
    else
    {
      // Test if (x / 180) != 0 and x / 90 == 0
      if ( std::fabs(std::fmod(x, 180.0L)) > LDBL_EPSILON
        && std::fabs(std::fmod(x, 90.0L)) < LDBL_EPSILON )
        error = true;
      else if ( std::fabs(std::fmod(x, 180.0L)) < LDBL_EPSILON )  // x / 180 == 0
        x = 0.0L;
      else
        x = std::tan(x * pi_value<lDouble> / 180.0L);
    }
  }

  if ( errno == EDOM || errno == ERANGE )
    error = true;

  setDisplay(x);
  arcus_mode = false;
  hyperbolic_mode = false;
  calculator_buttons[ButtonName::Arcus]->setChecked(false);
  calculator_buttons[ButtonName::Hyperbolic]->setChecked(false);
}

//----------------------------------------------------------------------
void Calc::draw()
{
  setBold();
  setColor (FColor::LightBlue, FColor::Cyan);
  clearArea (getVirtualDesktop(), wchar_t(finalcut::UniChar::MediumShade));
  unsetBold();
  finalcut::FDialog::draw();
  drawDispay();
}

//----------------------------------------------------------------------
auto Calc::isDataEntryKey (const ButtonName& key) const -> bool
{
  // Test if key is in {'.', '0'..'9'}
  constexpr std::array<ButtonName, 11> key_list =
  {{
    ButtonName::Decimal_point,
    ButtonName::Zero,
    ButtonName::One,
    ButtonName::Two,
    ButtonName::Three,
    ButtonName::Four,
    ButtonName::Five,
    ButtonName::Six,
    ButtonName::Seven,
    ButtonName::Eight,
    ButtonName::Nine
  }};

  const auto& iter = std::find (key_list.begin(), key_list.end(), key);

  if ( iter != key_list.end() )
    return true;

  return false;
}

//----------------------------------------------------------------------
auto Calc::isOperatorKey(const ButtonName& key) const -> bool
{
  // Test if key is in {'*', '/', '+', '-', '^', '='}
  constexpr std::array<ButtonName, 6> operators =
  {{
    ButtonName::Multiply,
    ButtonName::Divide,
    ButtonName::Add,
    ButtonName::Subtract,
    ButtonName::Power,
    ButtonName::Equals
  }};

  const auto& iter = std::find (operators.begin(), operators.end(), key);

  if ( iter != operators.end() )
    return true;

  return false;
}

//----------------------------------------------------------------------
auto Calc::getValue() -> lDouble&
{
  if ( infix_operator )
    return b;

  return a;
}

//----------------------------------------------------------------------
void Calc::setDisplay (lDouble d)
{
  input.sprintf("%32.11Lg", d);
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
inline void Calc::calcMultiplication()
{
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
}

//----------------------------------------------------------------------
inline void Calc::calcDivision()
{
  if ( std::fabs(b) > LDBL_EPSILON )  // b != 0.0L
    a /= b;
  else
    error = true;
}

//----------------------------------------------------------------------
inline void Calc::calcAddition()
{
  if ( std::fabs(a) > LDBL_EPSILON )  // a != 0.0L
  {
    if ( std::log(std::abs(a)) + std::log(std::abs(1 + b / a)) <= std::log(LDBL_MAX) )
      a += b;
    else
      error = true;
  }
  else
    a = b;
}

//----------------------------------------------------------------------
inline void Calc::calcSubtraction()
{
  if ( std::fabs(b) > LDBL_EPSILON )  // b != 0.0L
  {
    if ( std::log(std::abs(a)) + std::log(std::abs(1 - b / a)) <= std::log(LDBL_MAX) )
      a -= b;
    else
      error = true;
  }
  else
    a = b * (-1.0L);
}

//----------------------------------------------------------------------
inline void Calc::calcExponentiation()
{
  a = std::pow(a, b);

  if ( errno == EDOM || errno == ERANGE )
    error = true;
}

//----------------------------------------------------------------------
void Calc::calcInfixOperator()
{
  switch ( infix_operator )
  {
    case '\0':  // The infix operator has not been set yet
      return;

    case '*':
      calcMultiplication();
      break;

    case '/':
      calcDivision();
      break;

    case '+':
      calcAddition();
      break;

    case '-':
      calcSubtraction();
      break;

    case '^':
      calcExponentiation();
      break;

    default:
      throw std::invalid_argument{"Invalid infix operator"};
  }

  clearInfixOperator();
}

//----------------------------------------------------------------------
void Calc::initLayout()
{
  // Dialog settings
  FDialog::setText ("Calculator");
  FDialog::setGeometry (FPoint{19, 6}, FSize{37, 18});

  for (ButtonName key{ButtonName::Sine}; key < ButtonName::NUM_OF_BUTTONS; key++)
  {
    auto btn = calculator_buttons[ButtonName(key)];

    if ( key == ButtonName::Equals )
      btn->setGeometry(FPoint{30, 15}, FSize{5, 3});
    else
    {
      const int n = ( key <= ButtonName::Three ) ? 0 : 1;
      const int x = (int(key) + n) % 5 * 7 + 2;
      const int y = (int(key) + n) / 5 * 2 + 3;
      btn->setGeometry(FPoint{x, y}, FSize{5, 1});
    }

    btn->setDoubleFlatLine(finalcut::Side::Top);
    btn->setDoubleFlatLine(finalcut::Side::Bottom);
  }

  FDialog::initLayout();
}

//----------------------------------------------------------------------
void Calc::adjustSize()
{
  const std::size_t pw = getDesktopWidth();
  const std::size_t ph = getDesktopHeight();
  setX (1 + int(pw - getWidth()) / 2, false);
  setY (1 + int(ph - getHeight()) / 2, false);
  finalcut::FDialog::adjustSize();
}

//----------------------------------------------------------------------
auto Calc::getButtonText (const ButtonName& key) const -> const wchar_t*
{
  constexpr auto num_of_buttons = std::size_t(ButtonName::NUM_OF_BUTTONS);
  constexpr std::array<const wchar_t*, num_of_buttons> button_text =
  {{
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
  }};

  return button_text[std::size_t(key)];
}

//----------------------------------------------------------------------
void Calc::mapKeyFunctions()
{
  #define B(f) std::bind((f), this, std::placeholders::_1)  // Bind macro
  key_map =
  {
    { ButtonName::Sine, B(&Calc::sine) },                   // sin
    { ButtonName::Cosine, B(&Calc::cosine) },               // cos
    { ButtonName::Tangent, B(&Calc::tangent) },             // tan
    { ButtonName::Reciprocal, B(&Calc::reciprocal) },       // 1/x
    { ButtonName::On, B(&Calc::clear) },                    // On
    { ButtonName::Natural_logarithm, B(&Calc::log_e) },     // ln
    { ButtonName::Powers_of_e, B(&Calc::power_e) },         // eˣ
    { ButtonName::Power, B(&Calc::power) },                 // yˣ
    { ButtonName::Square_root, B(&Calc::square_root) },     // sqrt
    { ButtonName::Divide, B(&Calc::divide) },               // ÷
    { ButtonName::Common_logarithm, B(&Calc::log_10) },     // lg
    { ButtonName::Powers_of_ten, B(&Calc::power_10) },      // 10ˣ
    { ButtonName::Parenthese_l, B(&Calc::open_bracket) },   // (
    { ButtonName::Parenthese_r, B(&Calc::close_bracket) },  // )
    { ButtonName::Multiply, B(&Calc::multiply) },           // *
    { ButtonName::Hyperbolic, B(&Calc::hyperbolic) },       // hyp
    { ButtonName::Seven, B(&Calc::seven) },                 // 7
    { ButtonName::Eight, B(&Calc::eight) },                 // 8
    { ButtonName::Nine, B(&Calc::nine) },                   // 9
    { ButtonName::Subtract, B(&Calc::subtract) },           // -
    { ButtonName::Arcus, B(&Calc::arcus) },                 // arc
    { ButtonName::Four, B(&Calc::four) },                   // 4
    { ButtonName::Five, B(&Calc::five) },                   // 5
    { ButtonName::Six, B(&Calc::six) },                     // 6
    { ButtonName::Add, B(&Calc::add) },                     // +
    { ButtonName::Pi, B(&Calc::pi) },                       // π
    { ButtonName::One, B(&Calc::one) },                     // 1
    { ButtonName::Two, B(&Calc::two) },                     // 2
    { ButtonName::Three, B(&Calc::three) },                 // 3
    { ButtonName::Percent, B(&Calc::percent) },             // %
    { ButtonName::Zero, B(&Calc::zero) },                   // 0
    { ButtonName::Decimal_point, B(&Calc::radix_point) },   // .
    { ButtonName::Change_sign, B(&Calc::change_sign) },     // ±
    { ButtonName::Equals, B(&Calc::equals) }                // =
  };
}

// Overloaded operators
//----------------------------------------------------------------------
inline auto operator < (const Calc::ButtonName& c, const int n) noexcept -> bool
{
  return int(c) < n;
}

//----------------------------------------------------------------------
inline auto operator <= (const Calc::ButtonName& c, const int n) noexcept -> bool
{
  return int(c) <= n;
}

//----------------------------------------------------------------------
inline auto operator + (const Calc::ButtonName& c, const int n) noexcept -> Calc::ButtonName
{
  return Calc::ButtonName(int(c) + n);
}

//----------------------------------------------------------------------
inline auto operator ++ (Calc::ButtonName& c) noexcept -> Calc::ButtonName&  // prefix
{
  c = ( c < Calc::ButtonName::NUM_OF_BUTTONS ) ? Calc::ButtonName(int(c) + 1) : Calc::ButtonName::Equals;
  return c;
}

//----------------------------------------------------------------------
inline auto operator ++ (Calc::ButtonName& c, int) noexcept -> Calc::ButtonName  // postfix
{
  Calc::ButtonName tmp = c;
  ++c;
  return tmp;
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
auto main (int argc, char* argv[]) -> int
{
  // Create the application object
  finalcut::FApplication app(argc, argv);

  // Create a calculator object
  Calc calculator(&app);

  // Set calculator object as main widget
  finalcut::FWidget::setMainWidget(&calculator);

  // Show and start the application
  calculator.show();
  return app.exec();
}
