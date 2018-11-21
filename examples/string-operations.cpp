/***********************************************************************
* string-operations.cpp - Demonstrates the functionality of FString    *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2012-2018 Markus Gans                                      *
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

// function prototype
void init();
void inputStreamExample();
void outputStreamExample();
void streamingIntoFStringExample();
void streamingFromFStringExample();
void streamToInterger();
void streamToUnsignedInterger();
void streamToDouble();
void streamToFloat();
void CStringOutputExample();
void copyIntoFString();
void utf8StringOutputExample();
void letterCaseExample();
void stringConcatenationExample();
void stringCompareExample();
void stringSplittingExample();
void fromatStringExample();
void convertToNumberExample();
void convertNumberToStringExample();
void formatedNumberExample();
void trimExample();
void substringExample();
void insertExample();
void indexExample();
void iteratorExample();
void overwriteExample();
void removeExample();
void substringIncludeExample();
void replaceExample();
void tabToSpaceExample();
void backspaceControlCharacterExample();
void deleteControlCharacterExample();

//----------------------------------------------------------------------
// functions
//----------------------------------------------------------------------
void init()
{
  std::cout << "----------------[ terminal ]-------------------"
            << std::endl;

  // init current locale
  std::cout << "    Locale: " << std::setlocale(LC_CTYPE, "")
            << std::endl;

  if ( isatty(1) && ! std::strcmp(nl_langinfo(CODESET), "ANSI_X3.4-1968") )
  {
    // locale C -> switch from 7bit ascii -> latin1
    std::setlocale(LC_ALL, "C");
  }

  std::cout << "   Codeset: " << nl_langinfo(CODESET) << std::endl;
  std::cout << "--------------[ FString test ]-----------------"
            << std::endl;
}

//----------------------------------------------------------------------
void inputStreamExample()
{
  // Test: input stream (operator >>)
  finalcut::FString in;
  std::cout << "     Input: ";
  std::cin >> in;
  std::cout << "  instream >> " << in << std::endl;
}

//----------------------------------------------------------------------
void outputStreamExample()
{
  // Test: output stream (operator <<)
  const finalcut::FString& out = L"A test string for 0 \x20ac";
  std::cout << " outstream << " << out << std::endl;
}

//----------------------------------------------------------------------
void streamingIntoFStringExample()
{
  // Test: Streaming into a FString (operator <<)...

  // ...from FStrings
  finalcut::FString streamer1;
  streamer1 << finalcut::FString("FStr") << finalcut::FString("ing");
  std::cout << " stream in: " << streamer1 << std::endl;

  // ...from c++ wide string
  finalcut::FString streamer2;
  streamer2 << std::wstring(L"std::wstring");
  std::cout << " stream in: " << streamer2 << std::endl;

  // ...from wide string
  finalcut::FString streamer3;
  streamer3 << const_cast<wchar_t*>(L"wchar_t*");
  std::cout << " stream in: " << streamer3 << std::endl;

  // ...from c++ string
  finalcut::FString streamer4;
  streamer4 << std::string("std::string");
  std::cout << " stream in: " << streamer4 << std::endl;

  // ...from c-string
  finalcut::FString streamer5;
  streamer5 << const_cast<char*>("char*");
  std::cout << " stream in: " << streamer5 << std::endl;

  // ...from wide character
  finalcut::FString streamer6;
  streamer6 << wchar_t(L'w');
  std::cout << " stream in: " << streamer6 << std::endl;

  // ...from character
  finalcut::FString streamer7;
  streamer7 << char('c');
  std::cout << " stream in: " << streamer7 << std::endl;

  // ...from interger
  finalcut::FString streamer8;
  streamer8 << int(-12345);
  std::cout << " stream in: " << streamer8 << std::endl;

  // ...from unsigned interger
  finalcut::FString streamer9;
  streamer9 << uInt(54321);
  std::cout << " stream in: " << streamer9 << std::endl;

  // ...from long double
  finalcut::FString streamer10;
  streamer10 << lDouble(0.333333333333333333L);
  std::cout << " stream in: " << streamer10 << std::endl;

  // ...from double
  finalcut::FString streamer11;
  streamer11 << double(0.11111111111);
  std::cout << " stream in: " << streamer11 << std::endl;

  // ...from float
  finalcut::FString streamer12;
  streamer12 << float(0.22222222);
  std::cout << " stream in: " << streamer12 << std::endl;
}

//----------------------------------------------------------------------
void streamingFromFStringExample()
{
  // Test: Streaming from a FString (operator >>)...

  // ...to FStrings
  finalcut::FString stream_fstring;
  finalcut::FString("FString") >> stream_fstring;
  std::cout << "stream out: " << stream_fstring << std::endl;

  // ...to c++ wide string
  std::wstring stream_wstring;
  finalcut::FString("std::wstring") >> stream_wstring;
  std::wcout << "stream out: " << stream_wstring << std::endl;

  // ...to wide character
  wchar_t stream_wchar_t = 0;
  finalcut::FString("w") >> stream_wchar_t;
  std::wcout << "stream out: " << stream_wchar_t << std::endl;

  // ...to character
  char stream_char;
  finalcut::FString('c') >> stream_char;
  std::cout << "stream out: " << stream_char << std::endl;

  // ...to interger
  streamToInterger();

  // ...to unsigned interger
  streamToUnsignedInterger();

  // ...to double
  streamToDouble();

  // ...to float
  streamToFloat();
}

//----------------------------------------------------------------------
void streamToInterger()
{
  // Streaming from a FString to interger

  try
  {
    int stream_int;
    finalcut::FString("-321") >> stream_int;
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
}

//----------------------------------------------------------------------
void streamToUnsignedInterger()
{
  // Streaming from a FString to unsigned interger
  try
  {
    uInt stream_uint;
    finalcut::FString("123") >> stream_uint;
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
}

//----------------------------------------------------------------------
void streamToDouble()
{
  // Streaming from a FString to double
  try
  {
    double stream_double;
    finalcut::FString("0.123456e+2") >> stream_double;
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
}

//----------------------------------------------------------------------
void streamToFloat()
{
  // Streaming from a FString to float
  try
  {
    float stream_float;
    finalcut::FString("0.123e-3") >> stream_float;
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
}

//----------------------------------------------------------------------
void CStringOutputExample()
{
  // Test: c-string output
  const finalcut::FString& out = L"A test string for 0 \x20ac";
  printf ("     c_str:  \"%s\"\n", out.c_str());
}

//----------------------------------------------------------------------
void copyIntoFString()
{
  // Test: copy a c++ string
  const finalcut::FString& cpp_str( std::string("c++ String") );
  std::cout << "   cpp_str: \"" << cpp_str << "\"" << std::endl;

  // Test: copy a character
  const finalcut::FString ch('c');
  std::cout << "      char: '" << ch << "'" << std::endl;

  // Test: copy a wide character
  const finalcut::FString wch(L'w');
  std::cout << "   wchar_t: '" << wch << "'" << std::endl;
}

//----------------------------------------------------------------------
void utf8StringOutputExample()
{
  // Test: utf-8 string
  const finalcut::FString& len = "длина́";
  std::cout << "    length: \"" << len << "\" has "
            << len.getLength() << " characters" << std::endl;
}

//----------------------------------------------------------------------
void letterCaseExample()
{
  // Test: convert uppercase letter to lowercase
  const finalcut::FString& lower = finalcut::FString(L"InPut").toLower();
  std::wcout << L"   toLower: " << lower << std::endl;

  // Test: convert lowercase letter to uppercase
  const finalcut::FString& upper = finalcut::FString("inPut").toUpper();
  std::cout << "   toUpper: " << upper << std::endl;
}

//----------------------------------------------------------------------
void stringConcatenationExample()
{
  // Test: concatenate two FStrings (operator +)
  const finalcut::FString& add1 = finalcut::FString("FString + ")
                                + finalcut::FString("FString");
  std::cout << "       add: " << add1 << std::endl;

  // Test: concatenate a FString and a c++ wide string (operator +)
  const finalcut::FString& add2 = finalcut::FString("FString + ")
                                + std::wstring(L"std::wstring");
  std::cout << "       add: " << add2 << std::endl;

  // Test: concatenate a FString and a wide string (operator +)
  const finalcut::FString& add3 = finalcut::FString("FString + ")
                                + const_cast<wchar_t*>(L"wchar_t*");
  std::cout << "       add: " << add3 << std::endl;

  // Test: concatenate a FString and a c++ string (operator +)
  const finalcut::FString& add4 = finalcut::FString("FString + ")
                                + std::string("std::string");
  std::cout << "       add: " << add4 << std::endl;

  // Test: concatenate a FString and a c-string (operator +)
  const finalcut::FString& add5 = finalcut::FString("FString + ")
                                + const_cast<char*>("char*");
  std::cout << "       add: " << add5 << std::endl;

  // Test: concatenate a FString and a wide character (operator +)
  const finalcut::FString& add6 = finalcut::FString("FString + ")
                                + wchar_t(L'w');
  std::cout << "       add: " << add6 << std::endl;

  // Test: concatenate a FString and a character (operator +)
  const finalcut::FString& add7 = finalcut::FString("FString + ")
                                + char('c');
  std::cout << "       add: " << add7 << std::endl;

  // Test: concatenate a character and a FString (operator +)
  const finalcut::FString& add8 = 'c' + finalcut::FString(" + FString");
  std::cout << "       add: " << add8 << std::endl;

  // Test: concatenate a wide character and a FString (operator +)
  const finalcut::FString& add9 = L'w' + finalcut::FString(" + FString");
  std::cout << "       add: " << add9 << std::endl;

  // Test: concatenate a c-string and a FString (operator +)
  const finalcut::FString& add10 = const_cast<char*>("char*")
                                 + finalcut::FString(" + FString");
  std::cout << "       add: " << add10 << std::endl;

  // Test: concatenate a c++ string and a FString (operator +)
  const finalcut::FString& add11 = std::string("std::string")
                                 + finalcut::FString(" + FString");
  std::cout << "       add: " << add11 << std::endl;

  // Test: concatenate a wide string and a FString (operator +)
  const finalcut::FString& add12 = const_cast<wchar_t*>(L"wchar_t*")
                                 + finalcut::FString(" + FString");
  std::cout << "       add: " << add12 << std::endl;

  // Test: concatenate a c++ wide string and a FString (operator +)
  const finalcut::FString& add13 = std::wstring(L"std::wstring")
                                 + finalcut::FString(" + FString");
  std::cout << "       add: " << add13 << std::endl;
}

//----------------------------------------------------------------------
void stringCompareExample()
{
  // Test: compare operators ==, <=, <, >=, >, !=
  const finalcut::FString& cmp = "compare";

  if ( cmp == finalcut::FString("compare") )
    std::cout << "       cmp: == Ok" << std::endl;
  else
    std::cout << "       cmp: == Not Ok" << std::endl;

  if ( cmp <= finalcut::FString("d_compare") )
    std::cout << "       cmp: <= Ok" << std::endl;
  else
    std::cout << "       cmp: <= Not Ok" << std::endl;

  if ( cmp < finalcut::FString("e_compare") )
    std::cout << "       cmp: <  Ok" << std::endl;
  else
    std::cout << "       cmp: <  Not Ok" << std::endl;

  if ( cmp >= finalcut::FString("b_compare") )
    std::cout << "       cmp: >= Ok" << std::endl;
  else
    std::cout << "       cmp: >= Not Ok" << std::endl;

  if ( cmp > finalcut::FString("a_compare") )
    std::cout << "       cmp: >  Ok" << std::endl;
  else
    std::cout << "       cmp: >  Not Ok" << std::endl;

  if ( cmp != finalcut::FString("equal") )
    std::cout << "       cmp: != Ok" << std::endl;
  else
    std::cout << "       cmp: != Not Ok" << std::endl;
}

//----------------------------------------------------------------------
void stringSplittingExample()
{
  // Test: split a string with a delimiter and returns a vector (array)
  finalcut::FString split_str = "a,b,c,d";
  std::cout << "     split: \""
            << split_str << "\" into substrings ->";
  finalcut::FStringList parts = split_str.split(",");
  finalcut::FStringList::iterator it, end;
  end = parts.end();

  for (it = parts.begin(); it != end; ++it)
    std::cout << " \"" << (*it) << "\"";

  std::cout << std::endl;
}

//----------------------------------------------------------------------
void fromatStringExample()
{
  // Test: format a string with sprintf
  finalcut::FString formatStr = "";
  std::cout << " formatted: "
            << formatStr.sprintf("sqrt(%d) = %d", 16, 4)
            << std::endl;
}

//----------------------------------------------------------------------
void convertToNumberExample()
{
  // Test: convert a string to a unsigned long interger
  try
  {
    const uLong ulong_num = finalcut::FString("123456789").toULong();
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
    const long long_num = finalcut::FString("-9876543210").toLong();
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
    const double double_num = \
        finalcut::FString("2.7182818284590452353").toDouble();
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
}

//----------------------------------------------------------------------
void convertNumberToStringExample()
{
  // Test: convert integer and double value to a string
  finalcut::FString num1, num2, num3;
  num1.setNumber(137);
  num2.setNumber(-512);
  num3.setNumber(3.141592653589793238L, 12);
  std::cout << " setNumber:  "
            << num1 << " (unsigned)"    << std::endl;
  std::cout << " setNumber: "
            << num2 << " (signed)"      << std::endl;
  std::cout << " setNumber:  "
            << num3 << " (long double)" << std::endl;
}

//----------------------------------------------------------------------
void formatedNumberExample()
{
  // Test: convert and format a integer number with thousand separator
  std::setlocale (LC_NUMERIC, "");
  finalcut::FString fnum1, fnum2;
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
}

//----------------------------------------------------------------------
void trimExample()
{
  // Test: remove whitespace from the end of a string
  const finalcut::FString& trim_str = "  A string \t";
  std::wcout << "    rtrim: \""
             << trim_str.rtrim() << "\"" << std::endl;

  // Test: remove whitespace from the beginning of a string
  std::cout << "    ltrim: \""
            << trim_str.ltrim() << "\"" << std::endl;

  // Test: remove whitespace from the beginning and end of a string
  std::cout << "     trim: \""
            << trim_str.trim()  << "\"" << std::endl;
}

//----------------------------------------------------------------------
void substringExample()
{
  // Test: 11 characters from the left of the string
  const finalcut::FString& alphabet = "a b c d e f g h i j k l m "
                                      "n o p q r s t u v w x y z";
  std::cout << "     left: \""
            << alphabet.left(11)   << "\"" << std::endl;

  // Test: extract a substring of 27 characters from position 12
  std::cout << "      mid: \""
            << alphabet.mid(13, 27) << "\"" << std::endl;

  // Test: 11 characters from the right of the string
  std::cout << "    right: \""
            << alphabet.right(11)  << "\"" << std::endl;
}

//----------------------------------------------------------------------
void insertExample()
{
  // Test: insert a string at index position 7
  finalcut::FString insert_str = "I am a string";

  try
  {
    std::cout << "   insert: "
              << insert_str.insert("changed ", 7) << std::endl;
  }
  catch (const std::out_of_range& ex)
  {
    std::cerr << "Out of Range error: " << ex.what() << std::endl;
  }
}

//----------------------------------------------------------------------
void indexExample()
{
  // Test: get character access at a specified index position
  finalcut::FString index(5);  // string with five characters
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
}

//----------------------------------------------------------------------
void iteratorExample()
{
  // Test: character access with std::iterator
  const finalcut::FString& stringIterator = "iterator";
  finalcut::FString::iterator iter;
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
}

//----------------------------------------------------------------------
void overwriteExample()
{
  // Test: overwrite string at position 10 with "for t"
  finalcut::FString overwrite_std = "Overwrite the rest";
  std::cout << "overwrite: "
            << overwrite_std.overwrite("for t", 10) << std::endl;
}

//----------------------------------------------------------------------
void removeExample()
{
  // Test: remove 2 characters at position 7
  finalcut::FString remove_std = "A fast remove";
  std::cout << "   remove: "
            << remove_std.remove(7, 2) << std::endl;
}

//----------------------------------------------------------------------
void substringIncludeExample()
{
  // Test: includes a substring (positive test)
  finalcut::FString include_std = "string";

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
}

//----------------------------------------------------------------------
void replaceExample()
{
  // Test: find and replace a substring
  finalcut::FString source_str = "computer and software";
  const finalcut::FString& replace_str = \
      source_str.replace("computer", "hard-");
  std::cout << "  replace: "
            << replace_str << std::endl;
}

//----------------------------------------------------------------------
void tabToSpaceExample()
{
  // Test: convert tabs to spaces
  const finalcut::FString& tab_str = "1234\t5678";
  std::cout << "      tab: "
            << tab_str.expandTabs() << std::endl;
}

//----------------------------------------------------------------------
void backspaceControlCharacterExample()
{
  // Test: backspaces remove characters in the string
  const finalcut::FString& bs_str = "t\b\bTesT\bt";
  std::cout << "backspace: "
            << bs_str.removeBackspaces() << std::endl;
}

//----------------------------------------------------------------------
void deleteControlCharacterExample()
{
  // Test: delete characters remove characters in the string
  const finalcut::FString& del_str = "apple \177\177\177pietree";
  std::cout << "   delete: "
            << del_str.removeDel() << std::endl;
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int, char**)
{
  init();

  // Some FString examples
  inputStreamExample();
  outputStreamExample();
  streamingIntoFStringExample();
  streamingFromFStringExample();
  CStringOutputExample();
  copyIntoFString();
  utf8StringOutputExample();
  letterCaseExample();
  stringConcatenationExample();
  stringCompareExample();
  stringSplittingExample();
  fromatStringExample();
  convertToNumberExample();
  convertNumberToStringExample();
  formatedNumberExample();
  trimExample();
  substringExample();
  insertExample();
  indexExample();
  iteratorExample();
  overwriteExample();
  removeExample();
  substringIncludeExample();
  replaceExample();
  tabToSpaceExample();
  backspaceControlCharacterExample();
  deleteControlCharacterExample();
}
