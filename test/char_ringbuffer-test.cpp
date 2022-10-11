/***********************************************************************
* char_ringbuffer-test.cpp - FVTerm unit tests                         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2022 Markus Gans                                           *
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

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <final/final.h>

//----------------------------------------------------------------------
// class CharRingBufferTest
//----------------------------------------------------------------------

class CharRingBufferTest : public CPPUNIT_NS::TestFixture
{
  public:
    CharRingBufferTest() = default;

  protected:
    void classNameTest();
    void noArgumentTest();
    void BaseTest();
    void IteratorTest();
    void KeyStringTest();

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (CharRingBufferTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (noArgumentTest);
    CPPUNIT_TEST (BaseTest);
    CPPUNIT_TEST (IteratorTest);
    CPPUNIT_TEST (KeyStringTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};


//----------------------------------------------------------------------
void CharRingBufferTest::classNameTest()
{
  class finalcut::FRingBuffer<int, 0> int_rbuf;
  const finalcut::FString& classname1 = int_rbuf.getClassName();
  CPPUNIT_ASSERT ( classname1 == "FRingBuffer" );

  const finalcut::CharRingBuffer<0> char_rbuf;
  const finalcut::FString& classname2 = char_rbuf.getClassName();
  CPPUNIT_ASSERT ( classname2 == "CharRingBuffer" );
}

//----------------------------------------------------------------------
void CharRingBufferTest::noArgumentTest()
{
  const finalcut::CharRingBuffer<0> char_rbuf;
  CPPUNIT_ASSERT ( char_rbuf.isEmpty() );
  CPPUNIT_ASSERT ( ! char_rbuf.hasData() );
  CPPUNIT_ASSERT ( char_rbuf.isFull() );
  CPPUNIT_ASSERT ( char_rbuf.getSize() == 0 );
  CPPUNIT_ASSERT ( char_rbuf.getCapacity() == 0 );
  CPPUNIT_ASSERT ( char_rbuf.front() == '\0' );
  CPPUNIT_ASSERT ( char_rbuf.back() == '\0' );
  CPPUNIT_ASSERT ( char_rbuf.begin() == char_rbuf.end() );
}

//----------------------------------------------------------------------
void CharRingBufferTest::BaseTest()
{
  finalcut::CharRingBuffer<3> char_rbuf;
  char* physical_buffer = &char_rbuf[0];
  CPPUNIT_ASSERT ( char_rbuf.isEmpty() );
  CPPUNIT_ASSERT ( ! char_rbuf.hasData() );
  CPPUNIT_ASSERT ( ! char_rbuf.isFull() );
  CPPUNIT_ASSERT ( char_rbuf.getSize() == 0 );
  CPPUNIT_ASSERT ( char_rbuf.getCapacity() == 3 );
  CPPUNIT_ASSERT ( char_rbuf.front() == '\0' );
  CPPUNIT_ASSERT ( char_rbuf.back() == '\0' );
  CPPUNIT_ASSERT ( char_rbuf.begin() == char_rbuf.end() );
  CPPUNIT_ASSERT ( physical_buffer[0] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[1] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[2] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[0] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[1] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[2] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("", 0) );
  CPPUNIT_ASSERT ( ! char_rbuf.strncmp_front("!", 1) );

  char_rbuf.push('A');
  CPPUNIT_ASSERT ( ! char_rbuf.isEmpty() );
  CPPUNIT_ASSERT ( char_rbuf.hasData() );
  CPPUNIT_ASSERT ( ! char_rbuf.isFull() );
  CPPUNIT_ASSERT ( char_rbuf.getSize() == 1 );
  CPPUNIT_ASSERT ( char_rbuf.getCapacity() == 3 );
  CPPUNIT_ASSERT ( char_rbuf.front() == 'A' );
  CPPUNIT_ASSERT ( char_rbuf.back() == 'A' );
  CPPUNIT_ASSERT ( char_rbuf.begin() != char_rbuf.end() );
  CPPUNIT_ASSERT ( char_rbuf.begin() + 1 == char_rbuf.end() );
  CPPUNIT_ASSERT ( std::string(char_rbuf.begin(), char_rbuf.end()).compare("A") == 0 );
  CPPUNIT_ASSERT ( physical_buffer[0] == 'A' );
  CPPUNIT_ASSERT ( physical_buffer[1] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[2] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[0] == 'A' );
  CPPUNIT_ASSERT ( char_rbuf[1] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[2] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("A", 1) );
  CPPUNIT_ASSERT ( ! char_rbuf.strncmp_front("!", 1) );

  char_rbuf.push('B');
  CPPUNIT_ASSERT ( ! char_rbuf.isEmpty() );
  CPPUNIT_ASSERT ( char_rbuf.hasData() );
  CPPUNIT_ASSERT ( ! char_rbuf.isFull() );
  CPPUNIT_ASSERT ( char_rbuf.getSize() == 2 );
  CPPUNIT_ASSERT ( char_rbuf.getCapacity() == 3 );
  CPPUNIT_ASSERT ( char_rbuf.front() == 'A' );
  CPPUNIT_ASSERT ( char_rbuf.back() == 'B' );
  CPPUNIT_ASSERT ( char_rbuf.begin() != char_rbuf.end() );
  CPPUNIT_ASSERT ( char_rbuf.begin() + 2 == char_rbuf.end() );
  CPPUNIT_ASSERT ( std::string(char_rbuf.begin(), char_rbuf.end()).compare("AB") == 0 );
  CPPUNIT_ASSERT ( physical_buffer[0] == 'A' );
  CPPUNIT_ASSERT ( physical_buffer[1] == 'B' );
  CPPUNIT_ASSERT ( physical_buffer[2] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[0] == 'A' );
  CPPUNIT_ASSERT ( char_rbuf[1] == 'B' );
  CPPUNIT_ASSERT ( char_rbuf[2] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("A", 1) );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("AB", 2) );
  CPPUNIT_ASSERT ( ! char_rbuf.strncmp_front("A!", 2) );

  char_rbuf.push('C');
  CPPUNIT_ASSERT ( ! char_rbuf.isEmpty() );
  CPPUNIT_ASSERT ( char_rbuf.hasData() );
  CPPUNIT_ASSERT ( char_rbuf.isFull() );
  CPPUNIT_ASSERT ( char_rbuf.getSize() == 3 );
  CPPUNIT_ASSERT ( char_rbuf.getCapacity() == 3 );
  CPPUNIT_ASSERT ( char_rbuf.front() == 'A' );
  CPPUNIT_ASSERT ( char_rbuf.back() == 'C' );
  CPPUNIT_ASSERT ( char_rbuf.begin() != char_rbuf.end() );
  CPPUNIT_ASSERT ( char_rbuf.begin() + 3 == char_rbuf.end() );
  CPPUNIT_ASSERT ( std::string(char_rbuf.begin(), char_rbuf.end()).compare("ABC") == 0 );
  CPPUNIT_ASSERT ( physical_buffer[0] == 'A' );
  CPPUNIT_ASSERT ( physical_buffer[1] == 'B' );
  CPPUNIT_ASSERT ( physical_buffer[2] == 'C' );
  CPPUNIT_ASSERT ( char_rbuf[0] == 'A' );
  CPPUNIT_ASSERT ( char_rbuf[1] == 'B' );
  CPPUNIT_ASSERT ( char_rbuf[2] == 'C' );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("A", 1) );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("AB", 2) );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("ABC", 3) );
  CPPUNIT_ASSERT ( ! char_rbuf.strncmp_front("AB!", 3) );

  char_rbuf.pop(2);
  CPPUNIT_ASSERT ( ! char_rbuf.isEmpty() );
  CPPUNIT_ASSERT ( char_rbuf.hasData() );
  CPPUNIT_ASSERT ( ! char_rbuf.isFull() );
  CPPUNIT_ASSERT ( char_rbuf.getSize() == 1 );
  CPPUNIT_ASSERT ( char_rbuf.getCapacity() == 3 );
  CPPUNIT_ASSERT ( char_rbuf.front() == 'C' );
  CPPUNIT_ASSERT ( char_rbuf.back() == 'C' );
  CPPUNIT_ASSERT ( char_rbuf.begin() != char_rbuf.end() );
  CPPUNIT_ASSERT ( char_rbuf.begin() + 1 == char_rbuf.end() );
  CPPUNIT_ASSERT ( std::string(char_rbuf.begin(), char_rbuf.end()).compare("C") == 0 );
  CPPUNIT_ASSERT ( physical_buffer[0] == 'A' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[1] == 'B' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[2] == 'C' );
  CPPUNIT_ASSERT ( char_rbuf[0] == 'C' );
  CPPUNIT_ASSERT ( char_rbuf[1] == 'A' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[2] == 'B' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("C", 1) );
  CPPUNIT_ASSERT ( ! char_rbuf.strncmp_front("!", 1) );

  char_rbuf.push('D');
  CPPUNIT_ASSERT ( ! char_rbuf.isEmpty() );
  CPPUNIT_ASSERT ( char_rbuf.hasData() );
  CPPUNIT_ASSERT ( ! char_rbuf.isFull() );
  CPPUNIT_ASSERT ( char_rbuf.getSize() == 2 );
  CPPUNIT_ASSERT ( char_rbuf.getCapacity() == 3 );
  CPPUNIT_ASSERT ( char_rbuf.front() == 'C' );
  CPPUNIT_ASSERT ( char_rbuf.back() == 'D' );
  CPPUNIT_ASSERT ( char_rbuf.begin() != char_rbuf.end() );
  CPPUNIT_ASSERT ( char_rbuf.begin() + 2 == char_rbuf.end() );
  CPPUNIT_ASSERT ( std::string(char_rbuf.begin(), char_rbuf.end()).compare("CD") == 0 );
  CPPUNIT_ASSERT ( physical_buffer[0] == 'D' );
  CPPUNIT_ASSERT ( physical_buffer[1] == 'B' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[2] == 'C' );
  CPPUNIT_ASSERT ( char_rbuf[0] == 'C' );
  CPPUNIT_ASSERT ( char_rbuf[1] == 'D' );
  CPPUNIT_ASSERT ( char_rbuf[2] == 'B' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("C", 1) );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("CD", 2) );
  CPPUNIT_ASSERT ( ! char_rbuf.strncmp_front("C!", 2) );

  CPPUNIT_ASSERT ( *char_rbuf.begin() == 'C' );
  *char_rbuf.begin() = '+';
  CPPUNIT_ASSERT ( *char_rbuf.begin() == '+' );
  *char_rbuf.begin() = 'C';
  CPPUNIT_ASSERT ( *char_rbuf.end()  == 'B' );  // Currently not in use
  *char_rbuf.end() = '#';
  CPPUNIT_ASSERT ( *char_rbuf.end()  == '#' );  // Currently not in use

  char_rbuf.push('E');
  CPPUNIT_ASSERT ( ! char_rbuf.isEmpty() );
  CPPUNIT_ASSERT ( char_rbuf.hasData() );
  CPPUNIT_ASSERT ( char_rbuf.isFull() );
  CPPUNIT_ASSERT ( char_rbuf.getSize() == 3 );
  CPPUNIT_ASSERT ( char_rbuf.getCapacity() == 3 );
  CPPUNIT_ASSERT ( char_rbuf.front() == 'C' );
  CPPUNIT_ASSERT ( char_rbuf.back() == 'E' );
  CPPUNIT_ASSERT ( char_rbuf.begin() != char_rbuf.end() );
  CPPUNIT_ASSERT ( char_rbuf.begin() + 3 == char_rbuf.end() );
  CPPUNIT_ASSERT ( std::string(char_rbuf.begin(), char_rbuf.end()).compare("CDE") == 0 );
  CPPUNIT_ASSERT ( physical_buffer[0] == 'D' );
  CPPUNIT_ASSERT ( physical_buffer[1] == 'E' );
  CPPUNIT_ASSERT ( physical_buffer[2] == 'C' );
  CPPUNIT_ASSERT ( char_rbuf[0] == 'C' );
  CPPUNIT_ASSERT ( char_rbuf[1] == 'D' );
  CPPUNIT_ASSERT ( char_rbuf[2] == 'E' );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("C", 1) );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("CD", 2) );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("CDE", 3) );
  CPPUNIT_ASSERT ( ! char_rbuf.strncmp_front("CD!", 3) );

  char_rbuf.pop(1);
  CPPUNIT_ASSERT ( ! char_rbuf.isEmpty() );
  CPPUNIT_ASSERT ( char_rbuf.hasData() );
  CPPUNIT_ASSERT ( ! char_rbuf.isFull() );
  CPPUNIT_ASSERT ( char_rbuf.getSize() == 2 );
  CPPUNIT_ASSERT ( char_rbuf.getCapacity() == 3 );
  CPPUNIT_ASSERT ( char_rbuf.front() == 'D' );
  CPPUNIT_ASSERT ( char_rbuf.back() == 'E' );
  CPPUNIT_ASSERT ( char_rbuf.begin() != char_rbuf.end() );
  CPPUNIT_ASSERT ( char_rbuf.begin() + 2 == char_rbuf.end() );
  CPPUNIT_ASSERT ( std::string(char_rbuf.begin(), char_rbuf.end()).compare("DE") == 0 );
  CPPUNIT_ASSERT ( physical_buffer[0] == 'D' );
  CPPUNIT_ASSERT ( physical_buffer[1] == 'E' );
  CPPUNIT_ASSERT ( physical_buffer[2] == 'C' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[0] == 'D' );
  CPPUNIT_ASSERT ( char_rbuf[1] == 'E' );
  CPPUNIT_ASSERT ( char_rbuf[2] == 'C' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("D", 1) );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("DE", 2) );
  CPPUNIT_ASSERT ( ! char_rbuf.strncmp_front("D!", 2) );

  char_rbuf.push('F');
  CPPUNIT_ASSERT ( ! char_rbuf.isEmpty() );
  CPPUNIT_ASSERT ( char_rbuf.hasData() );
  CPPUNIT_ASSERT ( char_rbuf.isFull() );
  CPPUNIT_ASSERT ( char_rbuf.getSize() == 3 );
  CPPUNIT_ASSERT ( char_rbuf.getCapacity() == 3 );
  CPPUNIT_ASSERT ( char_rbuf.front() == 'D' );
  CPPUNIT_ASSERT ( char_rbuf.back() == 'F' );
  CPPUNIT_ASSERT ( char_rbuf.begin() != char_rbuf.end() );
  CPPUNIT_ASSERT ( char_rbuf.begin() + 3 == char_rbuf.end() );
  CPPUNIT_ASSERT ( std::string(char_rbuf.begin(), char_rbuf.end()).compare("DEF") == 0 );
  CPPUNIT_ASSERT ( physical_buffer[0] == 'D' );
  CPPUNIT_ASSERT ( physical_buffer[1] == 'E' );
  CPPUNIT_ASSERT ( physical_buffer[2] == 'F' );
  CPPUNIT_ASSERT ( char_rbuf[0] == 'D' );
  CPPUNIT_ASSERT ( char_rbuf[1] == 'E' );
  CPPUNIT_ASSERT ( char_rbuf[2] == 'F' );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("D", 1) );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("DE", 2) );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("DEF", 3) );
  CPPUNIT_ASSERT ( ! char_rbuf.strncmp_front("DE!", 3) );

  char_rbuf.pop();
  CPPUNIT_ASSERT ( ! char_rbuf.isEmpty() );
  CPPUNIT_ASSERT ( char_rbuf.hasData() );
  CPPUNIT_ASSERT ( ! char_rbuf.isFull() );
  CPPUNIT_ASSERT ( char_rbuf.getSize() == 2 );
  CPPUNIT_ASSERT ( char_rbuf.getCapacity() == 3 );
  CPPUNIT_ASSERT ( char_rbuf.front() == 'E' );
  CPPUNIT_ASSERT ( char_rbuf.back() == 'F' );
  CPPUNIT_ASSERT ( char_rbuf.begin() != char_rbuf.end() );
  CPPUNIT_ASSERT ( char_rbuf.begin() + 2 == char_rbuf.end() );
  CPPUNIT_ASSERT ( std::string(char_rbuf.begin(), char_rbuf.end()).compare("EF") == 0 );
  CPPUNIT_ASSERT ( physical_buffer[0] == 'D' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[1] == 'E' );
  CPPUNIT_ASSERT ( physical_buffer[2] == 'F' );
  CPPUNIT_ASSERT ( char_rbuf[0] == 'E' );
  CPPUNIT_ASSERT ( char_rbuf[1] == 'F' );
  CPPUNIT_ASSERT ( char_rbuf[2] == 'D' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("E", 1) );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("EF", 2) );
  CPPUNIT_ASSERT ( ! char_rbuf.strncmp_front("E!", 2) );

  char_rbuf.push('*');
  CPPUNIT_ASSERT ( ! char_rbuf.isEmpty() );
  CPPUNIT_ASSERT ( char_rbuf.hasData() );
  CPPUNIT_ASSERT ( char_rbuf.isFull() );
  CPPUNIT_ASSERT ( char_rbuf.getSize() == 3 );
  CPPUNIT_ASSERT ( char_rbuf.getCapacity() == 3 );
  CPPUNIT_ASSERT ( char_rbuf.front() == 'E' );
  CPPUNIT_ASSERT ( char_rbuf.back() == '*' );
  CPPUNIT_ASSERT ( char_rbuf.begin() != char_rbuf.end() );
  CPPUNIT_ASSERT ( char_rbuf.begin() + 3 == char_rbuf.end() );
  CPPUNIT_ASSERT ( std::string(char_rbuf.begin(), char_rbuf.end()).compare("EF*") == 0 );
  CPPUNIT_ASSERT ( physical_buffer[0] == '*' );
  CPPUNIT_ASSERT ( physical_buffer[1] == 'E' );
  CPPUNIT_ASSERT ( physical_buffer[2] == 'F' );
  CPPUNIT_ASSERT ( char_rbuf[0] == 'E' );
  CPPUNIT_ASSERT ( char_rbuf[1] == 'F' );
  CPPUNIT_ASSERT ( char_rbuf[2] == '*' );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("E", 1) );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("EF", 2) );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("EF*", 3) );
  CPPUNIT_ASSERT ( ! char_rbuf.strncmp_front("EF!", 3) );

  char_rbuf[2] = '~';
  CPPUNIT_ASSERT ( physical_buffer[0] == '~' );
  CPPUNIT_ASSERT ( char_rbuf[2] == '~' );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("EF~", 3) );
  CPPUNIT_ASSERT ( char_rbuf.back() == '~' );

  char_rbuf.pop();
  CPPUNIT_ASSERT ( ! char_rbuf.isEmpty() );
  CPPUNIT_ASSERT ( char_rbuf.hasData() );
  CPPUNIT_ASSERT ( ! char_rbuf.isFull() );
  CPPUNIT_ASSERT ( char_rbuf.getSize() == 2 );
  CPPUNIT_ASSERT ( char_rbuf.getCapacity() == 3 );
  CPPUNIT_ASSERT ( char_rbuf.front() == 'F' );
  CPPUNIT_ASSERT ( char_rbuf.back() == '~' );
  CPPUNIT_ASSERT ( char_rbuf.begin() != char_rbuf.end() );
  CPPUNIT_ASSERT ( char_rbuf.begin() + 2 == char_rbuf.end() );
  CPPUNIT_ASSERT ( std::string(char_rbuf.begin(), char_rbuf.end()).compare("F~") == 0 );
  CPPUNIT_ASSERT ( physical_buffer[0] == '~' );
  CPPUNIT_ASSERT ( physical_buffer[1] == 'E' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[2] == 'F' );
  CPPUNIT_ASSERT ( char_rbuf[0] == 'F' );
  CPPUNIT_ASSERT ( char_rbuf[1] == '~' );
  CPPUNIT_ASSERT ( char_rbuf[2] == 'E' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("F", 1) );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("F~", 2) );
  CPPUNIT_ASSERT ( ! char_rbuf.strncmp_front("F!", 2) );

  char_rbuf.clear();
  CPPUNIT_ASSERT ( char_rbuf.isEmpty() );
  CPPUNIT_ASSERT ( ! char_rbuf.hasData() );
  CPPUNIT_ASSERT ( ! char_rbuf.isFull() );
  CPPUNIT_ASSERT ( char_rbuf.getSize() == 0 );
  CPPUNIT_ASSERT ( char_rbuf.getCapacity() == 3 );
  CPPUNIT_ASSERT ( char_rbuf.front() == '\0' );
  CPPUNIT_ASSERT ( char_rbuf.back() == '\0' );
  CPPUNIT_ASSERT ( char_rbuf.begin() == char_rbuf.end() );
  CPPUNIT_ASSERT ( physical_buffer[0] == '~' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[1] == 'E' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[2] == 'F' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[0] == '~' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[1] == 'E' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[2] == 'F' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("", 0) );

  char_rbuf.pop();
  CPPUNIT_ASSERT ( char_rbuf.isEmpty() );
  CPPUNIT_ASSERT ( ! char_rbuf.hasData() );
  CPPUNIT_ASSERT ( ! char_rbuf.isFull() );
  CPPUNIT_ASSERT ( char_rbuf.getSize() == 0 );
  CPPUNIT_ASSERT ( char_rbuf.getCapacity() == 3 );
  CPPUNIT_ASSERT ( char_rbuf.front() == '\0' );
  CPPUNIT_ASSERT ( char_rbuf.back() == '\0' );
  CPPUNIT_ASSERT ( char_rbuf.begin() == char_rbuf.end() );
  CPPUNIT_ASSERT ( physical_buffer[0] == '~' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[1] == 'E' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[2] == 'F' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[0] == '~' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[1] == 'E' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[2] == 'F' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("", 0) );

  char_rbuf.push('H');
  CPPUNIT_ASSERT ( ! char_rbuf.isEmpty() );
  CPPUNIT_ASSERT ( char_rbuf.hasData() );
  CPPUNIT_ASSERT ( ! char_rbuf.isFull() );
  CPPUNIT_ASSERT ( char_rbuf.getSize() == 1 );
  CPPUNIT_ASSERT ( char_rbuf.getCapacity() == 3 );
  CPPUNIT_ASSERT ( char_rbuf.front() == 'H' );
  CPPUNIT_ASSERT ( char_rbuf.back() == 'H' );
  CPPUNIT_ASSERT ( char_rbuf.begin() != char_rbuf.end() );
  CPPUNIT_ASSERT ( char_rbuf.begin() + 1 == char_rbuf.end() );
  CPPUNIT_ASSERT ( std::string(char_rbuf.begin(), char_rbuf.end()).compare("H") == 0 );
  CPPUNIT_ASSERT ( physical_buffer[0] == 'H' );
  CPPUNIT_ASSERT ( physical_buffer[1] == 'E' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[2] == 'F' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[0] == 'H' );
  CPPUNIT_ASSERT ( char_rbuf[1] == 'E' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[2] == 'F' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("H", 1) );
  CPPUNIT_ASSERT ( ! char_rbuf.strncmp_front("!", 1) );

  char_rbuf.push('I');
  CPPUNIT_ASSERT ( ! char_rbuf.isEmpty() );
  CPPUNIT_ASSERT ( char_rbuf.hasData() );
  CPPUNIT_ASSERT ( ! char_rbuf.isFull() );
  CPPUNIT_ASSERT ( char_rbuf.getSize() == 2 );
  CPPUNIT_ASSERT ( char_rbuf.getCapacity() == 3 );
  CPPUNIT_ASSERT ( char_rbuf.front() == 'H' );
  CPPUNIT_ASSERT ( char_rbuf.back() == 'I' );
  CPPUNIT_ASSERT ( char_rbuf.begin() != char_rbuf.end() );
  CPPUNIT_ASSERT ( char_rbuf.begin() + 2 == char_rbuf.end() );
  CPPUNIT_ASSERT ( std::string(char_rbuf.begin(), char_rbuf.end()).compare("HI") == 0 );
  CPPUNIT_ASSERT ( physical_buffer[0] == 'H' );
  CPPUNIT_ASSERT ( physical_buffer[1] == 'I' );
  CPPUNIT_ASSERT ( physical_buffer[2] == 'F' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[0] == 'H' );
  CPPUNIT_ASSERT ( char_rbuf[1] == 'I' );
  CPPUNIT_ASSERT ( char_rbuf[2] == 'F' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("H", 1) );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("HI", 2) );
  CPPUNIT_ASSERT ( ! char_rbuf.strncmp_front("H!", 2) );
}

//----------------------------------------------------------------------
void CharRingBufferTest::IteratorTest()
{
  finalcut::CharRingBuffer<32> char_rbuf;
  const std::initializer_list<char> mouse_data = \
      { 0x1b, '[', '<', '0', ';', '5', '0', ';', '8', 'M'
      , 0x1b, '[', '<', '0', ';', '5', '1', ';', '9', 'M'
      , 0x1b, '[', '<', '0', ';', '5', '2', ';', '9', 'm' };

  for (const char& ch : mouse_data)
    char_rbuf.push(ch);

  auto iter = char_rbuf.begin();
  CPPUNIT_ASSERT ( *iter == '\033' );
  iter++;
  CPPUNIT_ASSERT ( *iter == '[' );
  CPPUNIT_ASSERT ( *(iter++) == '[' );
  CPPUNIT_ASSERT ( *(++iter) == '0' );
  iter++;
  CPPUNIT_ASSERT ( *iter == ';' );
  CPPUNIT_ASSERT ( *(iter + 5) == 'M' );
  CPPUNIT_ASSERT ( *iter == ';' );
  CPPUNIT_ASSERT ( *(iter + 1234) == '<' );
  auto new_iter = iter + 5;
  CPPUNIT_ASSERT ( *(new_iter) == 'M' );
  CPPUNIT_ASSERT ( iter != new_iter );
  auto iter_copy(iter);
  CPPUNIT_ASSERT ( iter == iter_copy );
  auto end_iter = char_rbuf.end();
  CPPUNIT_ASSERT ( *(end_iter) == '\0' );  // Currently not in use
  end_iter++;
  CPPUNIT_ASSERT ( *(end_iter) == '\0' );  // Currently not in use
  end_iter++;
  CPPUNIT_ASSERT ( *(end_iter) == '\033' );
  CPPUNIT_ASSERT ( &*end_iter == &*char_rbuf.begin() );

  char_rbuf.pop(10);
  CPPUNIT_ASSERT ( &*end_iter != &*char_rbuf.begin() );
  CPPUNIT_ASSERT ( &*(end_iter + 10) == &*char_rbuf.begin() );

  const std::initializer_list<char> new_mouse_data = \
      { 0x1b, '[', '<', '6', '4', ';', '6', ';', '4', 'M' };

  for (const char& ch : new_mouse_data)
    char_rbuf.push(ch);

  CPPUNIT_ASSERT ( *(end_iter) == '<' );
  end_iter++;
  CPPUNIT_ASSERT ( *(end_iter) == '6' );
  end_iter++;
  CPPUNIT_ASSERT ( *(end_iter) == '4' );
  CPPUNIT_ASSERT ( *char_rbuf.end() == '8' );  // Currently not in use
}

//----------------------------------------------------------------------
void CharRingBufferTest::KeyStringTest()
{
  finalcut::CharRingBuffer<12> char_rbuf;
  char* physical_buffer = &char_rbuf[0];
  CPPUNIT_ASSERT ( char_rbuf.isEmpty() );
  CPPUNIT_ASSERT ( ! char_rbuf.hasData() );
  CPPUNIT_ASSERT ( ! char_rbuf.isFull() );
  CPPUNIT_ASSERT ( char_rbuf.getSize() == 0 );
  CPPUNIT_ASSERT ( char_rbuf.getCapacity() == 12 );
  CPPUNIT_ASSERT ( char_rbuf.front() == '\0' );
  CPPUNIT_ASSERT ( char_rbuf.back() == '\0' );
  CPPUNIT_ASSERT ( char_rbuf.begin() == char_rbuf.end() );
  CPPUNIT_ASSERT ( physical_buffer[0] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[1] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[2] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[3] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[4] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[5] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[6] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[7] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[8] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[9] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[10] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[11] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[0] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[1] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[2] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[3] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[4] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[5] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[6] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[7] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[8] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[9] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[10] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[11] == '\0' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("", 0) );
  CPPUNIT_ASSERT ( ! char_rbuf.strncmp_front("!", 1) );

  for (const auto& ch : "\033[A\033[C\033[B\033[D")
    char_rbuf.push(ch);

  CPPUNIT_ASSERT ( ! char_rbuf.isEmpty() );
  CPPUNIT_ASSERT ( char_rbuf.hasData() );
  CPPUNIT_ASSERT ( char_rbuf.isFull() );
  CPPUNIT_ASSERT ( char_rbuf.getSize() == 12 );
  CPPUNIT_ASSERT ( char_rbuf.getCapacity() == 12 );
  CPPUNIT_ASSERT ( char_rbuf.front() == '\033' );
  CPPUNIT_ASSERT ( char_rbuf.back() == 'D' );
  CPPUNIT_ASSERT ( char_rbuf.begin() != char_rbuf.end() );
  CPPUNIT_ASSERT ( char_rbuf.begin() + 12 == char_rbuf.end() );
  CPPUNIT_ASSERT ( std::string(char_rbuf.begin(), char_rbuf.end()).compare("\033[A\033[C\033[B\033[D") == 0 );
  CPPUNIT_ASSERT ( physical_buffer[0] == '\033' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[1] == '[' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[2] == 'A' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[3] == '\033' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[4] == '[' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[5] == 'C' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[6] == '\033' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[7] == '[' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[8] == 'B' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[9] == '\033' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[10] == '[' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[11] == 'D' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[0] == '\033' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[1] == '[' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[2] == 'A' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[3] == '\033' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[4] == '[' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[5] == 'C' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[6] == '\033' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[7] == '[' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[8] == 'B' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[9] == '\033' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[10] == '[' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[11] == 'D' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("\033[A", 3) );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("\033[A\033[C", 6) );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("\033[A\033[C\033[B", 9) );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("\033[A\033[C\033[B\033[D", 12) );

  if ( char_rbuf.strncmp_front("\033[A", 3) )
    char_rbuf.pop(3);

  CPPUNIT_ASSERT ( char_rbuf.getSize() == 9 );

  if ( char_rbuf.strncmp_front("\033[C", 3) )
    char_rbuf.pop(3);

  CPPUNIT_ASSERT ( ! char_rbuf.isEmpty() );
  CPPUNIT_ASSERT ( char_rbuf.hasData() );
  CPPUNIT_ASSERT ( ! char_rbuf.isFull() );
  CPPUNIT_ASSERT ( char_rbuf.getSize() == 6 );
  CPPUNIT_ASSERT ( char_rbuf.getCapacity() == 12 );
  CPPUNIT_ASSERT ( char_rbuf.front() == '\033' );
  CPPUNIT_ASSERT ( char_rbuf.back() == 'D' );
  CPPUNIT_ASSERT ( char_rbuf.begin() != char_rbuf.end() );
  CPPUNIT_ASSERT ( char_rbuf.begin() + 6 == char_rbuf.end() );
  CPPUNIT_ASSERT ( std::string(char_rbuf.begin(), char_rbuf.end()).compare("\033[B\033[D") == 0 );
  CPPUNIT_ASSERT ( physical_buffer[0] == '\033' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[1] == '[' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[2] == 'A' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[3] == '\033' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[4] == '[' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[5] == 'C' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[6] == '\033' );
  CPPUNIT_ASSERT ( physical_buffer[7] == '[' );
  CPPUNIT_ASSERT ( physical_buffer[8] == 'B' );
  CPPUNIT_ASSERT ( physical_buffer[9] == '\033' );
  CPPUNIT_ASSERT ( physical_buffer[10] == '[' );
  CPPUNIT_ASSERT ( physical_buffer[11] == 'D' );
  CPPUNIT_ASSERT ( char_rbuf[0] == '\033' );
  CPPUNIT_ASSERT ( char_rbuf[1] == '[' );
  CPPUNIT_ASSERT ( char_rbuf[2] == 'B' );
  CPPUNIT_ASSERT ( char_rbuf[3] == '\033' );
  CPPUNIT_ASSERT ( char_rbuf[4] == '[' );
  CPPUNIT_ASSERT ( char_rbuf[5] == 'D' );
  CPPUNIT_ASSERT ( char_rbuf[6] == '\033' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[7] == '[' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[8] == 'A' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[9] == '\033' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[10] == '[' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[11] == 'C' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("\033[B", 3) );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("\033[B\033[D", 6) );

  if ( char_rbuf.strncmp_front("\033[B", 3) )
    char_rbuf.pop(3);

  CPPUNIT_ASSERT ( char_rbuf.getSize() == 3 );

  for (const auto& ch : "\033[Z\033[1;6P")
    char_rbuf.push(ch);

  CPPUNIT_ASSERT ( ! char_rbuf.isEmpty() );
  CPPUNIT_ASSERT ( char_rbuf.hasData() );
  CPPUNIT_ASSERT ( char_rbuf.isFull() );
  CPPUNIT_ASSERT ( char_rbuf.getSize() == 12 );
  CPPUNIT_ASSERT ( char_rbuf.getCapacity() == 12 );
  CPPUNIT_ASSERT ( char_rbuf.front() == '\033' );
  CPPUNIT_ASSERT ( char_rbuf.back() == 'P' );
  CPPUNIT_ASSERT ( char_rbuf.begin() != char_rbuf.end() );
  CPPUNIT_ASSERT ( char_rbuf.begin() + 12 == char_rbuf.end() );
  CPPUNIT_ASSERT ( std::string(char_rbuf.begin(), char_rbuf.end()).compare("\033[D\033[Z\033[1;6P") == 0 );
  CPPUNIT_ASSERT ( physical_buffer[0] == '\033' );
  CPPUNIT_ASSERT ( physical_buffer[1] == '[' );
  CPPUNIT_ASSERT ( physical_buffer[2] == 'Z' );
  CPPUNIT_ASSERT ( physical_buffer[3] == '\033' );
  CPPUNIT_ASSERT ( physical_buffer[4] == '[' );
  CPPUNIT_ASSERT ( physical_buffer[5] == '1' );
  CPPUNIT_ASSERT ( physical_buffer[6] == ';' );
  CPPUNIT_ASSERT ( physical_buffer[7] == '6' );
  CPPUNIT_ASSERT ( physical_buffer[8] == 'P' );
  CPPUNIT_ASSERT ( physical_buffer[9] == '\033' );
  CPPUNIT_ASSERT ( physical_buffer[10] == '[' );
  CPPUNIT_ASSERT ( physical_buffer[11] == 'D' );
  CPPUNIT_ASSERT ( char_rbuf[0] == '\033' );
  CPPUNIT_ASSERT ( char_rbuf[1] == '[' );
  CPPUNIT_ASSERT ( char_rbuf[2] == 'D' );
  CPPUNIT_ASSERT ( char_rbuf[3] == '\033' );
  CPPUNIT_ASSERT ( char_rbuf[4] == '[' );
  CPPUNIT_ASSERT ( char_rbuf[5] == 'Z' );
  CPPUNIT_ASSERT ( char_rbuf[6] == '\033' );
  CPPUNIT_ASSERT ( char_rbuf[7] == '[' );
  CPPUNIT_ASSERT ( char_rbuf[8] == '1' );
  CPPUNIT_ASSERT ( char_rbuf[9] == ';' );
  CPPUNIT_ASSERT ( char_rbuf[10] == '6' );
  CPPUNIT_ASSERT ( char_rbuf[11] == 'P' );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("\033[D", 3) );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("\033[D\033[Z", 6) );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("\033[D\033[Z\033[1;6P", 12) );

  if ( char_rbuf.strncmp_front("\033[D", 3) )
    char_rbuf.pop(3);

  CPPUNIT_ASSERT ( char_rbuf.getSize() == 9 );
  CPPUNIT_ASSERT ( char_rbuf.begin() + 9 == char_rbuf.end() );

  if ( char_rbuf.strncmp_front("\033[Z", 3) )
    char_rbuf.pop(3);

  CPPUNIT_ASSERT ( char_rbuf.getSize() == 6 );
  CPPUNIT_ASSERT ( char_rbuf.begin() + 6 == char_rbuf.end() );

   for (const auto& ch : {'\033', '[', '5', '~'} )
    char_rbuf.push(ch);

  CPPUNIT_ASSERT ( ! char_rbuf.isEmpty() );
  CPPUNIT_ASSERT ( char_rbuf.hasData() );
  CPPUNIT_ASSERT ( ! char_rbuf.isFull() );
  CPPUNIT_ASSERT ( char_rbuf.getSize() == 10 );
  CPPUNIT_ASSERT ( char_rbuf.getCapacity() == 12 );
  CPPUNIT_ASSERT ( char_rbuf.front() == '\033' );
  CPPUNIT_ASSERT ( char_rbuf.back() == '~' );
  CPPUNIT_ASSERT ( char_rbuf.begin() != char_rbuf.end() );
  CPPUNIT_ASSERT ( char_rbuf.begin() + 10 == char_rbuf.end() );
  CPPUNIT_ASSERT ( std::string(char_rbuf.begin(), char_rbuf.end()).compare("\033[1;6P\033[5~") == 0 );
  CPPUNIT_ASSERT ( physical_buffer[0] == '~' );
  CPPUNIT_ASSERT ( physical_buffer[1] == '[' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[2] == 'Z' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[3] == '\033' );
  CPPUNIT_ASSERT ( physical_buffer[4] == '[' );
  CPPUNIT_ASSERT ( physical_buffer[5] == '1' );
  CPPUNIT_ASSERT ( physical_buffer[6] == ';' );
  CPPUNIT_ASSERT ( physical_buffer[7] == '6' );
  CPPUNIT_ASSERT ( physical_buffer[8] == 'P' );
  CPPUNIT_ASSERT ( physical_buffer[9] == '\033' );
  CPPUNIT_ASSERT ( physical_buffer[10] == '[' );
  CPPUNIT_ASSERT ( physical_buffer[11] == '5' );
  CPPUNIT_ASSERT ( char_rbuf[0] == '\033' );
  CPPUNIT_ASSERT ( char_rbuf[1] == '[' );
  CPPUNIT_ASSERT ( char_rbuf[2] == '1' );
  CPPUNIT_ASSERT ( char_rbuf[3] == ';' );
  CPPUNIT_ASSERT ( char_rbuf[4] == '6' );
  CPPUNIT_ASSERT ( char_rbuf[5] == 'P' );
  CPPUNIT_ASSERT ( char_rbuf[6] == '\033' );
  CPPUNIT_ASSERT ( char_rbuf[7] == '[' );
  CPPUNIT_ASSERT ( char_rbuf[8] == '5' );
  CPPUNIT_ASSERT ( char_rbuf[9] == '~' );
  CPPUNIT_ASSERT ( char_rbuf[10] == '[' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[11] == 'Z' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("\033[1;6P", 6) );
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("\033[1;6P\033[5~", 10) );

  if ( char_rbuf.strncmp_front("\033[1;6P", 6) )
    char_rbuf.pop(6);

  CPPUNIT_ASSERT ( char_rbuf.getSize() == 4 );
  CPPUNIT_ASSERT ( char_rbuf.begin() + 4 == char_rbuf.end() );

  if ( char_rbuf.strncmp_front("\033[5~", 4) )
    char_rbuf.pop(4);

  CPPUNIT_ASSERT ( char_rbuf.isEmpty() );
  CPPUNIT_ASSERT ( ! char_rbuf.hasData() );
  CPPUNIT_ASSERT ( char_rbuf.getSize() == 0 );

   for (const auto& ch : {'\033', '[', '2', '4', ';', '2', '~'} )
    char_rbuf.push(ch);

  CPPUNIT_ASSERT ( ! char_rbuf.isEmpty() );
  CPPUNIT_ASSERT ( char_rbuf.hasData() );
  CPPUNIT_ASSERT ( ! char_rbuf.isFull() );
  CPPUNIT_ASSERT ( char_rbuf.getSize() == 7 );
  CPPUNIT_ASSERT ( char_rbuf.getCapacity() == 12 );
  CPPUNIT_ASSERT ( char_rbuf.front() == '\033' );
  CPPUNIT_ASSERT ( char_rbuf.back() == '~' );
  CPPUNIT_ASSERT ( char_rbuf.begin() != char_rbuf.end() );
  CPPUNIT_ASSERT ( char_rbuf.begin() + 7 == char_rbuf.end() );
  CPPUNIT_ASSERT ( std::string(char_rbuf.begin(), char_rbuf.end()).compare("\033[24;2~") == 0 );
  CPPUNIT_ASSERT ( physical_buffer[0] == '~' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[1] == '\033' );
  CPPUNIT_ASSERT ( physical_buffer[2] == '[' );
  CPPUNIT_ASSERT ( physical_buffer[3] == '2' );
  CPPUNIT_ASSERT ( physical_buffer[4] == '4' );
  CPPUNIT_ASSERT ( physical_buffer[5] == ';' );
  CPPUNIT_ASSERT ( physical_buffer[6] == '2' );
  CPPUNIT_ASSERT ( physical_buffer[7] == '~' );
  CPPUNIT_ASSERT ( physical_buffer[8] == 'P' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[9] == '\033' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[10] == '[' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[11] == '5' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[0] == '\033' );
  CPPUNIT_ASSERT ( char_rbuf[1] == '[' );
  CPPUNIT_ASSERT ( char_rbuf[2] == '2' );
  CPPUNIT_ASSERT ( char_rbuf[3] == '4' );
  CPPUNIT_ASSERT ( char_rbuf[4] == ';' );
  CPPUNIT_ASSERT ( char_rbuf[5] == '2' );
  CPPUNIT_ASSERT ( char_rbuf[6] == '~' );
  CPPUNIT_ASSERT ( char_rbuf[7] == 'P' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[8] == '\033' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[9] == '[' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[10] == '5' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[11] == '~' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("\033[24;2~", 7) );

  if ( char_rbuf.strncmp_front("\033[24;2~", 7) )
    char_rbuf.pop(7);

  CPPUNIT_ASSERT ( char_rbuf.isEmpty() );
  CPPUNIT_ASSERT ( ! char_rbuf.hasData() );
  CPPUNIT_ASSERT ( char_rbuf.getSize() == 0 );

  for (const auto& ch : {'\033', '[', '1', '1', '~'} )
    char_rbuf.push(ch);

  CPPUNIT_ASSERT ( ! char_rbuf.isEmpty() );
  CPPUNIT_ASSERT ( char_rbuf.hasData() );
  CPPUNIT_ASSERT ( ! char_rbuf.isFull() );
  CPPUNIT_ASSERT ( char_rbuf.getSize() == 5 );
  CPPUNIT_ASSERT ( char_rbuf.getCapacity() == 12 );
  CPPUNIT_ASSERT ( char_rbuf.front() == '\033' );
  CPPUNIT_ASSERT ( char_rbuf.back() == '~' );
  CPPUNIT_ASSERT ( char_rbuf.begin() != char_rbuf.end() );
  CPPUNIT_ASSERT ( char_rbuf.begin() + 5 == char_rbuf.end() );
  CPPUNIT_ASSERT ( std::string(char_rbuf.begin(), char_rbuf.end()).compare("\033[11~") == 0 );
  CPPUNIT_ASSERT ( physical_buffer[0] == '~' );
  CPPUNIT_ASSERT ( physical_buffer[1] == '\033' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[2] == '[' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[3] == '2' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[4] == '4' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[5] == ';' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[6] == '2' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[7] == '~' );  // Currently not in use
  CPPUNIT_ASSERT ( physical_buffer[8] == '\033' );
  CPPUNIT_ASSERT ( physical_buffer[9] == '[' );
  CPPUNIT_ASSERT ( physical_buffer[10] == '1' );
  CPPUNIT_ASSERT ( physical_buffer[11] == '1' );
  CPPUNIT_ASSERT ( char_rbuf[0] == '\033' );
  CPPUNIT_ASSERT ( char_rbuf[1] == '[' );
  CPPUNIT_ASSERT ( char_rbuf[2] == '1' );
  CPPUNIT_ASSERT ( char_rbuf[3] == '1' );
  CPPUNIT_ASSERT ( char_rbuf[4] == '~' );
  CPPUNIT_ASSERT ( char_rbuf[5] == '\033' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[6] == '[' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[7] == '2' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[8] == '4' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[9] == ';' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[10] == '2' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf[11] == '~' );  // Currently not in use
  CPPUNIT_ASSERT ( char_rbuf.strncmp_front("\033[11~", 5) );
}


// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (CharRingBufferTest);

// The general unit test main part
#include <main-test.inc>

