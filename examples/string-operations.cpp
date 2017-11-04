/***********************************************************************
* string-operations.cpp - Demonstrates the functionality of FString    *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2012-2017 Markus Gans                                      *
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

#include <langinfo.h>
#include <unistd.h>

#include <clocale>
#include <iomanip>
#include <string>
#include <vector>

#include <final/final.h>


int main (int, char**)
{
  printf ("----------------[ terminal ]-------------------\n");

  // init current locale
  printf ("    Locale: %s\n", std::setlocale(LC_CTYPE, "") );

  if ( isatty(1) && ! std::strcmp(nl_langinfo(CODESET), "ANSI_X3.4-1968") )
  {
    // locale C -> switch from 7bit ascii -> latin1
    std::setlocale(LC_ALL, "C");
  }

  printf ("   Codeset: %s\n", nl_langinfo(CODESET));

  std::cout << "--------------[ FString test ]-----------------"
            << std::endl;

  // Test: input stream (operator >>)
  FString in;
  std::cout << "     Input: ";
  std::cin >> in;
  std::cout << "  instream >> " << in << std::endl;

  // Test: output stream (operator <<)
  const FString& out = L"A test string for 0 \x20ac";
  std::cout << " outstream << " << out << std::endl;

  // Test: Streaming into a FString (operator <<)...

  // ...from FStrings
  FString streamer1;
  streamer1 << FString("FStr") << FString("ing");
  std::cout << " stream in: " << streamer1 << std::endl;

  // ...from c++ wide string
  FString streamer2;
  streamer2 << std::wstring(L"std::wstring");
  std::cout << " stream in: " << streamer2 << std::endl;

  // ...from wide string
  FString streamer3;
  streamer3 << const_cast<wchar_t*>(L"wchar_t*");
  std::cout << " stream in: " << streamer3 << std::endl;

  // ...from c++ string
  FString streamer4;
  streamer4 << std::string("std::string");
  std::cout << " stream in: " << streamer4 << std::endl;

  // ...from c-string
  FString streamer5;
  streamer5 << const_cast<char*>("char*");
  std::cout << " stream in: " << streamer5 << std::endl;

  // ...from wide character
  FString streamer6;
  streamer6 << wchar_t(L'w');
  std::cout << " stream in: " << streamer6 << std::endl;

  // ...from character
  FString streamer7;
  streamer7 << char('c');
  std::cout << " stream in: " << streamer7 << std::endl;

  // ...from interger
  FString streamer8;
  streamer8 << int(-12345);
  std::cout << " stream in: " << streamer8 << std::endl;

  // ...from unsigned interger
  FString streamer9;
  streamer9 << uInt(54321);
  std::cout << " stream in: " << streamer9 << std::endl;

  // ...from long double
  FString streamer10;
  streamer10 << lDouble(0.333333333333333333L);
  std::cout << " stream in: " << streamer10 << std::endl;

  // ...from double
  FString streamer11;
  streamer11 << double(0.11111111111);
  std::cout << " stream in: " << streamer11 << std::endl;

  // ...from float
  FString streamer12;
  streamer12 << float(0.22222222);
  std::cout << " stream in: " << streamer12 << std::endl;

  // Test: Streaming from a FString (operator >>)...

  // ...to FStrings
  FString stream_fstring;
  FString("FString") >> stream_fstring;
  std::cout << "stream out: " << stream_fstring << std::endl;

  // ...to c++ wide string
  std::wstring stream_wstring;
  FString("std::wstring") >> stream_wstring;
  std::wcout << "stream out: " << stream_wstring << std::endl;

  // ...to wide character
  wchar_t stream_wchar_t = 0;
  FString("w") >> stream_wchar_t;
  std::wcout << "stream out: " << stream_wchar_t << std::endl;

  // ...to character
  char stream_char;
  FString('c') >> stream_char;
  std::cout << "stream out: " << stream_char << std::endl;

  // ...to interger
  try
  {
    int stream_int;
    FString("-321") >> stream_int;
    std::cout << "stream out: " << stream_int << std::endl;
  }
  catch (const std::invalid_argument& ex)
  {
    std::cerr << "Invalid argument: " << ex.what() << std::endl;
  }
  catch (const std::overflow_error& ex)
  {
    std::cerr << "overflow: " << ex.what() << std::endl;
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Arithmetic error: " << ex.what() << std::endl;
  }

  // ...to unsigned interger
  try
  {
    uInt stream_uint;
    FString("123") >> stream_uint;
    std::cout << "stream out: " << stream_uint << std::endl;
  }
  catch (const std::invalid_argument& ex)
  {
    std::cerr << "Invalid argument: " << ex.what() << std::endl;
  }
  catch (const std::overflow_error& ex)
  {
    std::cerr << "overflow: " << ex.what() << std::endl;
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Arithmetic error: " << ex.what() << std::endl;
  }

  // ...to double
  try
  {
    double stream_double;
    FString("0.123456e+2") >> stream_double;
    std::cout << "stream out: " << stream_double << std::endl;
  }
  catch (const std::invalid_argument& ex)
  {
    std::cerr << "Invalid argument: " << ex.what() << std::endl;
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Arithmetic error: " << ex.what() << std::endl;
  }

  // ...to float
  try
  {
    float stream_float;
    FString("0.123e-3") >> stream_float;
    std::cout << "stream out: " << stream_float << std::endl;
  }
  catch (const std::invalid_argument& ex)
  {
    std::cerr << "Invalid argument: " << ex.what() << std::endl;
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Arithmetic error: " << ex.what() << std::endl;
  }

  // Test: c-string output
  printf ("     c_str:  \"%s\"\n", out.c_str());

  // Test: copy a c++ string
  const FString& cpp_str( std::string("c++ String") );
  std::cout << "   cpp_str: \"" << cpp_str << "\"" << std::endl;

  // Test: copy a character
  const FString ch('c');
  std::cout << "      char: '" << ch << "'" << std::endl;

  // Test: copy a wide character
  const FString wch(L'w');
  std::cout << "   wchar_t: '" << wch << "'" << std::endl;

  // Test: utf-8 string
  const FString& len = "длина́";
  std::cout << "    length: \"" << len << "\" has "
            << len.getLength() << " characters" << std::endl;

  // Test: convert uppercase letter to lowercase
  const FString& lower = FString(L"InPut").toLower();
  std::wcout << L"   toLower: " << lower << std::endl;

  // Test: convert lowercase letter to uppercase
  const FString& upper = FString("inPut").toUpper();
  std::cout << "   toUpper: " << upper << std::endl;

  // Test: concatenate two FStrings (operator +)
  const FString& add1 = FString("FString + ") + FString("FString");
  std::cout << "       add: " << add1 << std::endl;

  // Test: concatenate a FString and a c++ wide string (operator +)
  const FString& add2 = FString("FString + ")
                      + std::wstring(L"std::wstring");
  std::cout << "       add: " << add2 << std::endl;

  // Test: concatenate a FString and a wide string (operator +)
  const FString& add3 = FString("FString + ")
                      + const_cast<wchar_t*>(L"wchar_t*");
  std::cout << "       add: " << add3 << std::endl;

  // Test: concatenate a FString and a c++ string (operator +)
  const FString& add4 = FString("FString + ")
                      + std::string("std::string");
  std::cout << "       add: " << add4 << std::endl;

  // Test: concatenate a FString and a c-string (operator +)
  const FString& add5 = FString("FString + ")
                      + const_cast<char*>("char*");
  std::cout << "       add: " << add5 << std::endl;

  // Test: concatenate a FString and a wide character (operator +)
  const FString& add6 = FString("FString + ") + wchar_t(L'w');
  std::cout << "       add: " << add6 << std::endl;

  // Test: concatenate a FString and a character (operator +)
  const FString& add7 = FString("FString + ") + char('c');
  std::cout << "       add: " << add7 << std::endl;

  // Test: concatenate a character and a FString (operator +)
  const FString& add8 = 'c' + FString(" + FString");
  std::cout << "       add: " << add8 << std::endl;

  // Test: concatenate a wide character and a FString (operator +)
  const FString& add9 = L'w' + FString(" + FString");
  std::cout << "       add: " << add9 << std::endl;

  // Test: concatenate a c-string and a FString (operator +)
  const FString& add10 = const_cast<char*>("char*")
                       + FString(" + FString");
  std::cout << "       add: " << add10 << std::endl;

  // Test: concatenate a c++ string and a FString (operator +)
  const FString& add11 = std::string("std::string")
                       + FString(" + FString");
  std::cout << "       add: " << add11 << std::endl;

  // Test: concatenate a wide string and a FString (operator +)
  const FString& add12 = const_cast<wchar_t*>(L"wchar_t*")
                       + FString(" + FString");
  std::cout << "       add: " << add12 << std::endl;

  // Test: concatenate a c++ wide string and a FString (operator +)
  const FString& add13 = std::wstring(L"std::wstring")
                       + FString(" + FString");
  std::cout << "       add: " << add13 << std::endl;

  // Test: compare operators ==, <=, <, >=, >, !=
  const FString& cmp = "compare";

  if ( cmp == FString("compare") )
    std::cout << "       cmp: == Ok" << std::endl;
  else
    std::cout << "       cmp: == Not Ok" << std::endl;

  if ( cmp <= FString("d_compare") )
    std::cout << "       cmp: <= Ok" << std::endl;
  else
    std::cout << "       cmp: <= Not Ok" << std::endl;

  if ( cmp < FString("e_compare") )
    std::cout << "       cmp: <  Ok" << std::endl;
  else
    std::cout << "       cmp: <  Not Ok" << std::endl;

  if ( cmp >= FString("b_compare") )
    std::cout << "       cmp: >= Ok" << std::endl;
  else
    std::cout << "       cmp: >= Not Ok" << std::endl;

  if ( cmp > FString("a_compare") )
    std::cout << "       cmp: >  Ok" << std::endl;
  else
    std::cout << "       cmp: >  Not Ok" << std::endl;

  if ( cmp != FString("equal") )
    std::cout << "       cmp: != Ok" << std::endl;
  else
    std::cout << "       cmp: != Not Ok" << std::endl;

  // Test: split a string with a delimiter and returns a vector (array)
  FString split_str = "a,b,c,d";
  std::cout << "     split: \""
            << split_str << "\" into substrings ->";
  FStringList parts = split_str.split(",");
  FStringList::iterator it, end;
  end = parts.end();

  for (it = parts.begin(); it != end; ++it)
    std::cout << " \"" << (*it) << "\"";

  std::cout << std::endl;

  // Test: format a string with sprintf
  FString formatStr = "";
  std::cout << " formatted: "
            << formatStr.sprintf("sqrt(%d) = %d", 16, 4)
            << std::endl;

  // Test: convert a string to a unsigned long interger
  try
  {
    const uLong ulong_num = FString("123456789").toULong();
    std::cout << "   toULong:  " << ulong_num << std::endl;
  }
  catch (const std::invalid_argument& ex)
  {
    std::cerr << "Invalid argument: " << ex.what() << std::endl;
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Arithmetic error: " << ex.what() << std::endl;
  }

  // Test: convert a string to a signed long interger
  try
  {
    const long long_num = FString("-9876543210").toLong();
    std::cout << "    toLong:  " << long_num << std::endl;
  }
  catch (const std::invalid_argument& ex)
  {
    std::cerr << "Invalid argument: " << ex.what() << std::endl;
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Arithmetic error: " << ex.what() << std::endl;
  }

  // Test: convert a string to a double value
  std::setlocale(LC_NUMERIC, "C");

  try
  {
    const double double_num = FString("2.7182818284590452353").toDouble();
    std::ios_base::fmtflags save_flags = std::cout.flags();
    std::cout << "  toDouble:  " << std::setprecision(11)
                                 << double_num << std::endl;
    std::cout.flags(save_flags);
  }
  catch (const std::invalid_argument& ex)
  {
    std::cerr << "Invalid argument: " << ex.what() << std::endl;
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Arithmetic error: " << ex.what() << std::endl;
  }

  // Test: convert integer and double value to a string
  FString num1, num2, num3;
  num1.setNumber(137);
  num2.setNumber(-512);
  num3.setNumber(3.141592653589793238L, 12);
  std::cout << " setNumber:  "
            << num1 << " (unsigned)"    << std::endl;
  std::cout << " setNumber: "
            << num2 << " (signed)"      << std::endl;
  std::cout << " setNumber:  "
            << num3 << " (long double)" << std::endl;

  // Test: convert and format a integer number with thousand separator
  std::setlocale (LC_NUMERIC, "");
  FString fnum1, fnum2;
#if defined(__LP64__) || defined(_LP64)
  // 64-bit architecture
  fnum1.setFormatedNumber(0xffffffffffffffff, '\'');
  fnum2.setFormatedNumber(-9223372036854775807);
#else
  // 32-bit architecture
  fnum1.setFormatedNumber(0xffffffff, '\'');
  fnum2.setFormatedNumber(-2147483647);
#endif
  std::cout << "setFormatedNumber: "
            << fnum1 << " (unsigned)" << std::endl;
  std::cout << "setFormatedNumber: "
            << fnum2 << " (signed)"   << std::endl;

  // Test: remove whitespace from the end of a string
  const FString& trim_str = "  A string \t";
  std::wcout << "    rtrim: \""
             << trim_str.rtrim() << "\"" << std::endl;

  // Test: remove whitespace from the beginning of a string
  std::cout << "    ltrim: \""
            << trim_str.ltrim() << "\"" << std::endl;

  // Test: remove whitespace from the beginning and end of a string
  std::cout << "     trim: \""
            << trim_str.trim()  << "\"" << std::endl;

  // Test: 11 characters from the left of the string
  const FString& alphabet = "a b c d e f g h i j k l m n o p q r s t u v w x y z";
  std::cout << "     left: \""
            << alphabet.left(11)   << "\"" << std::endl;

  // Test: extract a substring of 27 characters from position 12
  std::cout << "      mid: \""
            << alphabet.mid(13, 27) << "\"" << std::endl;

  // Test: 11 characters from the right of the string
  std::cout << "    right: \""
            << alphabet.right(11)  << "\"" << std::endl;

  // Test: insert a string at index position 7
  FString insert_str = "I am a string";

  try
  {
    std::cout << "   insert: "
              << insert_str.insert("changed ", 7) << std::endl;
  }
  catch (const std::out_of_range& ex)
  {
    std::cerr << "Out of Range error: " << ex.what() << std::endl;
  }

  // Test: get character access at a specified index position
  FString index(5);  // string with five characters
  index = "index";

  try
  {
    index[0] = L'I';  // write a wide character at position 0
    printf ( "    index: [0] = %c ; [4] = %c\n"
           , char(index[0])
           , char(index[4]) );
  }
  catch (const std::out_of_range& ex)
  {
    std::cerr << "Out of Range error: " << ex.what() << std::endl;
  }

  // Test: character access with std::iterator
  const FString& stringIterator = "iterator";
  FString::iterator iter;
  iter = stringIterator.begin();
  std::cout << " " << stringIterator << ": ";

  while ( iter != stringIterator.end() )
  {
    std::cout << char(*iter) << "|";
    ++iter;
  }

  std::cout << "  (front='" << char(stringIterator.front())
            << "', back='" << char(stringIterator.back())
            << "')" << std::endl;

  // Test: overwrite string at position 10 with "for t"
  FString overwrite_std = "Overwrite the rest";
  std::cout << "overwrite: "
            << overwrite_std.overwrite("for t", 10) << std::endl;

  // Test: remove 2 characters at position 7
  FString remove_std = "A fast remove";
  std::cout << "   remove: "
            << remove_std.remove(7, 2) << std::endl;

  // Test: includes a substring (positive test)
  FString include_std = "string";

  if ( include_std.includes("ring") )
    std::cout << " includes: \""
              << include_std << "\" includes \"ring\" "
              << std::endl;
  else
    std::cout << " includes: \""
              << include_std << "\" includes no \"ring\" "
              << std::endl;

  // Test: includes a substring (negativ test)
  if ( include_std.includes("data") )
    std::cout << " includes: \""
              << include_std << "\" includes \"data\" "
              << std::endl;
  else
    std::cout << " includes: \""
              << include_std << "\" includes no \"data\" "
              << std::endl;

  // Test: find and replace a substring
  FString source_str = "computer and software";
  const FString& replace_str = source_str.replace("computer", "hard-");
  std::cout << "  replace: "
            << replace_str << std::endl;

  // Test: convert tabs to spaces
  const FString& tab_str = "1234\t5678";
  std::cout << "      tab: "
            << tab_str.expandTabs() << std::endl;

  // Test: backspaces remove characters in the string
  const FString& bs_str = "t\b\bTesT\bt";
  std::cout << "backspace: "
            << bs_str.removeBackspaces() << std::endl;

  // Test: delete characters remove characters in the string
  const FString& del_str = "apple \177\177\177pietree";
  std::cout << "   delete: "
            << del_str.removeDel() << std::endl;
}
