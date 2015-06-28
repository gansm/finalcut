// fstring.cpp

#include <iomanip>

#include <langinfo.h>
#include <term.h>
#include <unistd.h>

#include "fstring.h"


int main (int, char**)
{
  printf ("----------------[ terminal ]-------------------\n");

  // init current locale
  printf ("    Locale: %s\n", setlocale(LC_ALL, "") );
  if ( isatty(1) && ! strcmp(nl_langinfo(CODESET), "ANSI_X3.4-1968") )
  {
    // locale C -> switch from 7bit ascii -> latin1
    setlocale(LC_ALL, "en_US");
  }
  printf ("   Codeset: %s\n", nl_langinfo(CODESET));

  std::cout << "--------------[ FString test ]-----------------"
            << std::endl;

  FString in;
  std::cout << "     Input: "; std::cin >> in;
  std::cout << "  instream >> " << in << std::endl;

  FString out = L"A test string for 0 \x20ac";
  std::cout << " outstream << " << out.c_str() << std::endl;
  printf ("     c_str:  \"%s\"\n", out.c_str());

  FString cpp_str( std::string("c++ String") );
  std::cout << "   cpp_str: \"" << cpp_str << "\"" << std::endl;
  FString ch('c');
  std::cout << "      char: '" << ch << "'" << std::endl;
  FString wch(L'w');
  std::cout << "   wchar_t: '" << wch << "'" << std::endl;

  FString len = "длина́";
  std::cout << "    length: \"" << len << "\" has "
            << len.getLength() << " characters" << std::endl;

  FString lower = FString(L"InPut").toLower();
  std::wcout << L"   toLower: " << lower << std::endl;
  FString upper = FString("inPut").toUpper();
  std::cout << "   toUpper: " << upper << std::endl;

  FString add1 = FString("FString + ") + FString("FString");
  std::cout << "       add: " << add1 << std::endl;
  FString add2 = FString("FString + ") + std::wstring(L"std::wstring");
  std::cout << "       add: " << add2 << std::endl;
  FString add3 = FString("FString + ") + const_cast<wchar_t*>(L"wchar_t*");
  std::cout << "       add: " << add3 << std::endl;
  FString add4 = FString("FString + ") + std::string("std::string");
  std::cout << "       add: " << add4 << std::endl;
  FString add5 = FString("FString + ") + const_cast<char*>("char*");
  std::cout << "       add: " << add5 << std::endl;
  FString add6 = FString("FString + ") + wchar_t(L'w');
  std::cout << "       add: " << add6 << std::endl;
  FString add7 = FString("FString + ") + char('c');
  std::cout << "       add: " << add7 << std::endl;

  FString add8 = 'c' + FString(" + FString");
  std::cout << "       add: " << add8 << std::endl;
  FString add9 = L'w' + FString(" + FString");
  std::cout << "       add: " << add9 << std::endl;
  FString add10 = const_cast<char*>("char*") + FString(" + FString");
  std::cout << "       add: " << add10 << std::endl;
  FString add11 = std::string("std::string") + FString(" + FString");
  std::cout << "       add: " << add11 << std::endl;
  FString add12 = const_cast<wchar_t*>(L"wchar_t*") + FString(" + FString");
  std::cout << "       add: " << add12 << std::endl;
  FString add13 = std::wstring(L"std::wstring") + FString(" + FString");
  std::cout << "       add: " << add13 << std::endl;

  FString cmp = "compare";
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

  FString split_str = "a,b,c,d";
  std::cout << "     split: \""
            << split_str << "\" into substrings ->";
  std::vector <FString> parts = split_str.split(",");
  std::vector<FString>::iterator it, end;
  end = parts.end();
  for (it = parts.begin(); it != end; ++it)
    std::cout << " \"" << (*it) << "\"";
  std::cout << std::endl;

  FString formatStr = "";
  std::cout << " formatted: "
            << formatStr.sprintf("sqrt(%d) = %d", 16, 4)
            << std::endl;
  try
  {
    uLong ulong_num = FString("123456789").toULong();
    std::cout << "   toULong:  " << ulong_num << std::endl;
  }
  catch (const std::invalid_argument& ex)
  {
    std::cerr << "Invalid argument: " << ex.what() << std::endl;
  }
  catch (const std::out_of_range& ex)
  {
    std::cerr << "Out of range: " << ex.what() << std::endl;
  }

  try
  {
    long long_num = FString("-9876543210").toLong();
    std::cout << "    toLong: " << long_num << std::endl;
  }
  catch (const std::invalid_argument& ex)
  {
    std::cerr << "Invalid argument: " << ex.what() << std::endl;
  }
  catch (const std::out_of_range& ex)
  {
    std::cerr << "Out of range: " << ex.what() << std::endl;
  }

  setlocale(LC_NUMERIC, "C");
  try
  {
    double double_num = FString("2.7182818284590452353").toDouble();
    std::cout << "  toDouble:  " << std::setprecision(11)
                                 << double_num << std::endl;
  }
  catch (const std::invalid_argument& ex)
  {
    std::cerr << "Invalid argument: " << ex.what() << std::endl;
  }
  catch (const std::out_of_range& ex)
  {
    std::cerr << "Out of range: " << ex.what() << std::endl;
  }
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

  FString fnum1, fnum2;
  fnum1.setFormatedNumber(0xffffffffffffffff, '\'');
  fnum2.setFormatedNumber(-9223372036854775807);
  std::cout << "setFormatedNumber: "
            << fnum1 << " (unsigned)" << std::endl;
  std::cout << "setFormatedNumber: "
            << fnum2 << " (signed)"   << std::endl;

  FString trim_str = "  A string \t";
  std::wcout << "    rtrim: \""
             << trim_str.rtrim() << "\"" << std::endl;
  std::cout << "    ltrim: \""
            << trim_str.ltrim() << "\"" << std::endl;
  std::cout << "     trim: \""
            << trim_str.trim()  << "\"" << std::endl;

  FString alphabet = "a b c d e f g h i j k l m n o p q r s t u v w x y z";
  std::cout << "     left: \""
            << alphabet.left(11)   << "\"" << std::endl;
  std::cout << "      mid: \""
            << alphabet.mid(13,27) << "\"" << std::endl;
  std::cout << "    right: \""
            << alphabet.right(11)  << "\"" << std::endl;

  FString insert_str = "I am a string";
  std::cout << "   insert: "
            << insert_str.insert("changed ", 7) << std::endl;

  FString index(5); // a string with five characters
  index = "index";
  index[0] = L'I'; // write a wide character at position 0
  printf ( "    index: [0] = %c ; [4] = %c\n",
           char(index[0]),
           char(index[4]) );

  FString stringIterator = "iterator";
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

  FString overwrite_std = "Overwrite the rest";
  std::cout << "overwrite: "
            << overwrite_std.overwrite("for t", 10) << std::endl;

  FString remove_std = "A fast remove";
  std::cout << "   remove: "
            << remove_std.remove(7,2) << std::endl;

  FString include_std = "string";
  if ( include_std.includes("ring") )
    std::cout << " includes: \""
              << include_std << "\" includes \"ring\" "
              << std::endl;
  else
    std::cout << " includes: \""
              << include_std << "\" includes no \"ring\" "
              << std::endl;
  if ( include_std.includes("data") )
    std::cout << " includes: \""
              << include_std << "\" includes \"data\" "
              << std::endl;
  else
    std::cout << " includes: \""
              << include_std << "\" includes no \"data\" "
              << std::endl;

  FString source_str = "computer and software";
  FString replace_str = source_str.replace("computer", "hard-");
  std::cout << "  replace: "
            << replace_str << std::endl;

  FString tab_str = "1234\t5678";
  std::cout << "      tab: "
            << tab_str.expandTabs() << std::endl;

  FString bs_str = "t\b\bTesT\bt";
  std::cout << "backspace: "
            << bs_str.removeBackspaces() << std::endl;

  FString del_str = "apple \177\177\177pietree";
  std::cout << "   delete: "
            << del_str.removeDel() << std::endl;
}
