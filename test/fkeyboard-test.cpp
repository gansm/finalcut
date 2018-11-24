/***********************************************************************
* fkeyboard-test.cpp - FKeyboard unit tests                            *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2018 Markus Gans                                           *
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

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <final/final.h>

namespace test
{

#pragma pack(push)
#pragma pack(1)
typedef struct
{
  int   num;
  char* string;
  char  tname[4];
}
fkeymap;
#pragma pack(pop)

fkeymap Fkey[] =
{
  { finalcut::fc::Fkey_backspace , C_STR("\177")     , "kb" },  // backspace key
  { finalcut::fc::Fkey_catab     , 0                 , "ka" },  // clear-all-tabs key
  { finalcut::fc::Fkey_clear     , 0                 , "kC" },  // clear-screen or erase key
  { finalcut::fc::Fkey_ctab      , C_STR(CSI "3~")   , "kt" },  // clear-tab key
  { finalcut::fc::Fkey_dc        , 0                 , "kD" },  // delete-character key
  { finalcut::fc::Fkey_dc        , 0                 , "kDx" }, // delete-character key
  { finalcut::fc::Fkey_dl        , 0                 , "kL" },  // delete-line key
  { finalcut::fc::Fkey_down      , C_STR(ESC "OB")   , "kd" },  // down-arrow key
  { finalcut::fc::Fkey_down      , C_STR(CSI "B")    , "kdx"},  // down-arrow key
  { finalcut::fc::Fkey_eic       , 0                 , "kM" },  // sent by rmir or smir in insert mode
  { finalcut::fc::Fkey_eol       , 0                 , "kE" },  // clear-to-end-of-line key
  { finalcut::fc::Fkey_eos       , 0                 , "kS" },  // clear-to-end-of-screen key
  { finalcut::fc::Fkey_f0        , 0                 , "k0" },  // F0 function key
  { finalcut::fc::Fkey_f1        , C_STR(ESC "OP")   , "k1" },  // F1 function key
  { finalcut::fc::Fkey_f1        , C_STR(CSI "11~")  , "k1x"},  // F1 function key
  { finalcut::fc::Fkey_f1        , C_STR(ESC "OP")   , "k1X"},  // F1 function key
  { finalcut::fc::Fkey_f2        , C_STR(ESC "OQ")   , "k2" },  // F2 function key
  { finalcut::fc::Fkey_f2        , C_STR(CSI "12~")  , "k2x"},  // F2 function key
  { finalcut::fc::Fkey_f2        , C_STR(CSI "OQ")   , "k2X"},  // F2 function key
  { finalcut::fc::Fkey_f3        , C_STR(ESC "OR")   , "k3" },  // F3 function key
  { finalcut::fc::Fkey_f3        , C_STR(CSI "13~")  , "k3x"},  // F3 function key
  { finalcut::fc::Fkey_f3        , C_STR(ESC "OR")   , "k3X"},  // F3 function key
  { finalcut::fc::Fkey_f4        , C_STR(ESC "OS")   , "k4" },  // F4 function key
  { finalcut::fc::Fkey_f4        , C_STR(CSI "14~")  , "k4x"},  // F4 function key
  { finalcut::fc::Fkey_f4        , C_STR(ESC "OS")   , "k4X"},  // F4 function key
  { finalcut::fc::Fkey_f5        , C_STR(CSI "15~")  , "k5" },  // F5 function key
  { finalcut::fc::Fkey_f6        , C_STR(CSI "17~")  , "k6" },  // F6 function key
  { finalcut::fc::Fkey_f7        , C_STR(CSI "18~")  , "k7" },  // F7 function key
  { finalcut::fc::Fkey_f8        , C_STR(CSI "19~")  , "k8" },  // F8 fucntion key
  { finalcut::fc::Fkey_f9        , C_STR(CSI "20~")  , "k9" },  // F9 function key
  { finalcut::fc::Fkey_f10       , C_STR(CSI "21~")  , "k;" },  // F10 function key
  { finalcut::fc::Fkey_home      , C_STR(ESC "OH")   , "kh" },  // home key
  { finalcut::fc::Fkey_home      , C_STR(CSI "7~")   , "khx"},  // home key
  { finalcut::fc::Fkey_ic        , C_STR(CSI "2~")   , "kI" },  // insert-character key
  { finalcut::fc::Fkey_il        , 0                 , "kA" },  // insert-line key
  { finalcut::fc::Fkey_left      , C_STR(ESC "OD")   , "kl" },  // left-arrow key
  { finalcut::fc::Fkey_left      , C_STR(CSI "D")    , "klx"},  // left-arrow key
  { finalcut::fc::Fkey_ll        , 0                 , "kH" },  // last-line key
  { finalcut::fc::Fkey_npage     , C_STR(CSI "6~")   , "kN" },  // next-page key
  { finalcut::fc::Fkey_ppage     , C_STR(CSI "5~")   , "kP" },  // prev-page key
  { finalcut::fc::Fkey_right     , C_STR(ESC "OC")   , "kr" },  // right-arrow key
  { finalcut::fc::Fkey_right     , C_STR(CSI "C")    , "krx"},  // right-arrow key
  { finalcut::fc::Fkey_sf        , C_STR(CSI "1;2B") , "kF" },  // scroll-forward key (shift-up)
  { finalcut::fc::Fkey_sr        , C_STR(CSI "1;2A") , "kR" },  // scroll-backward key (shift-down)
  { finalcut::fc::Fkey_stab      , 0                 , "kT" },  // set-tab key
  { finalcut::fc::Fkey_up        , C_STR(ESC "OA")   , "ku" },  // up-arrow key
  { finalcut::fc::Fkey_up        , C_STR(CSI "A")    , "kux"},  // up-arrow key
  { finalcut::fc::Fkey_a1        , 0                 , "K1" },  // upper left of keypad
  { finalcut::fc::Fkey_a3        , 0                 , "K3" },  // upper right of keypad
  { finalcut::fc::Fkey_b2        , C_STR(CSI "E")    , "K2" },  // center of keypad
  { finalcut::fc::Fkey_c1        , 0                 , "K4" },  // lower left of keypad
  { finalcut::fc::Fkey_c3        , 0                 , "K5" },  // lower right of keypad
  { finalcut::fc::Fkey_btab      , C_STR(CSI "Z")    , "kB" },  // back-tab key
  { finalcut::fc::Fkey_beg       , 0                 , "@1" },  // begin key
  { finalcut::fc::Fkey_cancel    , 0                 , "@2" },  // cancel key
  { finalcut::fc::Fkey_close     , 0                 , "@3" },  // close key
  { finalcut::fc::Fkey_command   , 0                 , "@4" },  // command key
  { finalcut::fc::Fkey_copy      , 0                 , "@5" },  // copy key
  { finalcut::fc::Fkey_create    , 0                 , "@6" },  // create key
  { finalcut::fc::Fkey_end       , C_STR(ESC "OF")   , "@7" },  // end key
  { finalcut::fc::Fkey_end       , C_STR(CSI "8~")   , "@7x"},  // end key
  { finalcut::fc::Fkey_end       , C_STR(CSI "K")    , "@7X"},  // end key
  { finalcut::fc::Fkey_enter     , 0                 , "@8" },  // enter/send key
  { finalcut::fc::Fkey_enter     , C_STR(ESC "OM")   , "@8x"},  // enter/send key
  { finalcut::fc::Fkey_exit      , 0                 , "@9" },  // exit key
  { finalcut::fc::Fkey_find      , C_STR(CSI "1~")   , "@0" },  // find key
  { finalcut::fc::Fkey_slash     , C_STR(ESC "Oo")   , "KP1"},  // keypad slash
  { finalcut::fc::Fkey_asterisk  , C_STR(ESC "Oj")   , "KP2"},  // keypad asterisk
  { finalcut::fc::Fkey_minus_sign, C_STR(ESC "Om")   , "KP3"},  // keypad minus sign
  { finalcut::fc::Fkey_plus_sign , C_STR(ESC "Ok")   , "KP4"},  // keypad plus sign
  { finalcut::fc::Fkey_help      , 0                 , "%1" },  // help key
  { finalcut::fc::Fkey_mark      , 0                 , "%2" },  // mark key
  { finalcut::fc::Fkey_message   , 0                 , "%3" },  // message key
  { finalcut::fc::Fkey_move      , 0                 , "%4" },  // move key
  { finalcut::fc::Fkey_next      , 0                 , "%5" },  // next key
  { finalcut::fc::Fkey_open      , 0                 , "%6" },  // open key
  { finalcut::fc::Fkey_options   , 0                 , "%7" },  // options key
  { finalcut::fc::Fkey_previous  , 0                 , "%8" },  // previous key
  { finalcut::fc::Fkey_print     , 0                 , "%9" },  // print key
  { finalcut::fc::Fkey_redo      , 0                 , "%0" },  // redo key
  { finalcut::fc::Fkey_reference , 0                 , "&1" },  // reference key
  { finalcut::fc::Fkey_refresh   , 0                 , "&2" },  // refresh key
  { finalcut::fc::Fkey_replace   , 0                 , "&3" },  // replace key
  { finalcut::fc::Fkey_restart   , 0                 , "&4" },  // restart key
  { finalcut::fc::Fkey_resume    , 0                 , "&5" },  // resume key
  { finalcut::fc::Fkey_save      , 0                 , "&6" },  // save key
  { finalcut::fc::Fkey_suspend   , 0                 , "&7" },  // suspend key
  { finalcut::fc::Fkey_undo      , 0                 , "&8" },  // undo key
  { finalcut::fc::Fkey_sbeg      , 0                 , "&9" },  // shifted begin key
  { finalcut::fc::Fkey_scancel   , 0                 , "&0" },  // shifted cancel key
  { finalcut::fc::Fkey_scommand  , 0                 , "*1" },  // shifted command key
  { finalcut::fc::Fkey_scopy     , 0                 , "*2" },  // shifted copy key
  { finalcut::fc::Fkey_screate   , 0                 , "*3" },  // shifted create key
  { finalcut::fc::Fkey_sdc       , C_STR(CSI "3;2~") , "*4" },  // shifted delete-character key
  { finalcut::fc::Fkey_sdl       , 0                 , "*5" },  // shifted delete-line key
  { finalcut::fc::Fkey_select    , C_STR(CSI "4~")   , "*6" },  // select key
  { finalcut::fc::Fkey_send      , C_STR(CSI "1;2F") , "*7" },  // shifted end key
  { finalcut::fc::Fkey_seol      , 0                 , "*8" },  // shifted clear-to-end-of-line key
  { finalcut::fc::Fkey_sexit     , 0                 , "*9" },  // shifted exit key
  { finalcut::fc::Fkey_sfind     , 0                 , "*0" },  // shifted find key
  { finalcut::fc::Fkey_shelp     , 0                 , "#1" },  // shifted help key
  { finalcut::fc::Fkey_shome     , C_STR(CSI "1;2H") , "#2" },  // shifted home key
  { finalcut::fc::Fkey_sic       , C_STR(CSI "2;2~") , "#3" },  // shifted insert-character key
  { finalcut::fc::Fkey_sleft     , C_STR(CSI "1;2D") , "#4" },  // shifted left-arrow key
  { finalcut::fc::Fkey_smessage  , 0                 , "%a" },  // shifted message key
  { finalcut::fc::Fkey_smove     , 0                 , "%b" },  // shifted move key
  { finalcut::fc::Fkey_snext     , C_STR(CSI "6;2~") , "%c" },  // shifted next key
  { finalcut::fc::Fkey_soptions  , 0                 , "%d" },  // shifted options key
  { finalcut::fc::Fkey_sprevious , C_STR(CSI "5;2~") , "%e" },  // shifted previous key
  { finalcut::fc::Fkey_sprint    , 0                 , "%f" },  // shifted print key
  { finalcut::fc::Fkey_sredo     , 0                 , "%g" },  // shifted redo key
  { finalcut::fc::Fkey_sreplace  , 0                 , "%h" },  // shifted replace key
  { finalcut::fc::Fkey_sright    , C_STR(CSI "1;2C") , "%i" },  // shifted right-arrow key
  { finalcut::fc::Fkey_srsume    , 0                 , "%j" },  // shifted resume key
  { finalcut::fc::Fkey_ssave     , 0                 , "!1" },  // shifted save key
  { finalcut::fc::Fkey_ssuspend  , 0                 , "!2" },  // shifted suspend key
  { finalcut::fc::Fkey_sundo     , 0                 , "!3" },  // shifted undo key
  { finalcut::fc::Fkey_f11       , C_STR(CSI "23~")  , "F1" },  // F11 function key
  { finalcut::fc::Fkey_f12       , C_STR(CSI "24~")  , "F2" },  // F12 function key
  { finalcut::fc::Fkey_f13       , C_STR(ESC "O1;2P"), "F3" },  // F13 function key
  { finalcut::fc::Fkey_f14       , C_STR(ESC "O1;2Q"), "F4" },  // F14 function key
  { finalcut::fc::Fkey_f15       , C_STR(ESC "O1;2R"), "F5" },  // F15 function key
  { finalcut::fc::Fkey_f16       , C_STR(ESC "O1;2S"), "F6" },  // F16 function key
  { finalcut::fc::Fkey_f17       , C_STR(CSI "15;2~"), "F7" },  // F17 function key
  { finalcut::fc::Fkey_f18       , C_STR(CSI "17;2~"), "F8" },  // F18 function key
  { finalcut::fc::Fkey_f19       , C_STR(CSI "18;2~"), "F9" },  // F19 function key
  { finalcut::fc::Fkey_f20       , C_STR(CSI "19;2~"), "FA" },  // F20 function key
  { finalcut::fc::Fkey_f21       , C_STR(CSI "20;2~"), "FB" },  // F21 function key
  { finalcut::fc::Fkey_f22       , C_STR(CSI "21;2~"), "FC" },  // F22 function key
  { finalcut::fc::Fkey_f23       , C_STR(CSI "23;2~"), "FD" },  // F23 function key
  { finalcut::fc::Fkey_f24       , C_STR(CSI "24;2~"), "FE" },  // F24 function key
  { finalcut::fc::Fkey_f25       , C_STR(ESC "O1;5P"), "FF" },  // F25 function key
  { finalcut::fc::Fkey_f26       , C_STR(ESC "O1;5Q"), "FG" },  // F26 function key
  { finalcut::fc::Fkey_f27       , C_STR(ESC "O1;5R"), "FH" },  // F27 function key
  { finalcut::fc::Fkey_f28       , C_STR(ESC "O1;5S"), "FI" },  // F28 function key
  { finalcut::fc::Fkey_f29       , C_STR(CSI "15;5~"), "FJ" },  // F29 function key
  { finalcut::fc::Fkey_f30       , C_STR(CSI "17;5~"), "FK" },  // F30 function key
  { finalcut::fc::Fkey_f31       , C_STR(CSI "18;5~"), "FL" },  // F31 function key
  { finalcut::fc::Fkey_f32       , C_STR(CSI "19;5~"), "FM" },  // F32 function key
  { finalcut::fc::Fkey_f33       , C_STR(CSI "20;5~"), "FN" },  // F33 function key
  { finalcut::fc::Fkey_f34       , C_STR(CSI "21;5~"), "FO" },  // F34 function key
  { finalcut::fc::Fkey_f35       , C_STR(CSI "23;5~"), "FP" },  // F35 function key
  { finalcut::fc::Fkey_f36       , C_STR(CSI "24;5~"), "FQ" },  // F36 function key
  { finalcut::fc::Fkey_f37       , C_STR(ESC "O1;6P"), "FR" },  // F37 function key
  { finalcut::fc::Fkey_f38       , C_STR(ESC "O1;6Q"), "FS" },  // F38 function key
  { finalcut::fc::Fkey_f39       , C_STR(ESC "O1;6R"), "FT" },  // F39 function key
  { finalcut::fc::Fkey_f40       , C_STR(ESC "O1;6S"), "FU" },  // F40 function key
  { finalcut::fc::Fkey_f41       , C_STR(CSI "15;6~"), "FV" },  // F41 function key
  { finalcut::fc::Fkey_f42       , C_STR(CSI "17;6~"), "FW" },  // F42 function key
  { finalcut::fc::Fkey_f43       , C_STR(CSI "18;6~"), "FX" },  // F43 function key
  { finalcut::fc::Fkey_f44       , C_STR(CSI "19;6~"), "FY" },  // F44 function key
  { finalcut::fc::Fkey_f45       , C_STR(CSI "20;6~"), "FZ" },  // F45 function key
  { finalcut::fc::Fkey_f46       , C_STR(CSI "21;6~"), "Fa" },  // F46 function key
  { finalcut::fc::Fkey_f47       , C_STR(CSI "23;6~"), "Fb" },  // F47 function key
  { finalcut::fc::Fkey_f48       , C_STR(CSI "24;6~"), "Fc" },  // F48 function key
  { finalcut::fc::Fkey_f49       , C_STR(ESC "O1;3P"), "Fd" },  // F49 function key
  { finalcut::fc::Fkey_f50       , C_STR(ESC "O1;3Q"), "Fe" },  // F50 function key
  { finalcut::fc::Fkey_f51       , C_STR(ESC "O1;3R"), "Ff" },  // F51 function key
  { finalcut::fc::Fkey_f52       , C_STR(ESC "O1;3S"), "Fg" },  // F52 function key
  { finalcut::fc::Fkey_f53       , C_STR(CSI "15;3~"), "Fh" },  // F53 function key
  { finalcut::fc::Fkey_f54       , C_STR(CSI "17;3~"), "Fi" },  // F54 function key
  { finalcut::fc::Fkey_f55       , C_STR(CSI "18;3~"), "Fj" },  // F55 function key
  { finalcut::fc::Fkey_f56       , C_STR(CSI "19;3~"), "Fk" },  // F56 function key
  { finalcut::fc::Fkey_f57       , C_STR(CSI "20;3~"), "Fl" },  // F57 function key
  { finalcut::fc::Fkey_f58       , C_STR(CSI "21;3~"), "Fm" },  // F58 function key
  { finalcut::fc::Fkey_f59       , C_STR(CSI "23;3~"), "Fn" },  // F59 function key
  { finalcut::fc::Fkey_f60       , C_STR(CSI "24;3~"), "Fo" },  // F60 function key
  { finalcut::fc::Fkey_f61       , C_STR(ESC "O1;4P"), "Fp" },  // F61 function key
  { finalcut::fc::Fkey_f62       , C_STR(ESC "O1;4Q"), "Fq" },  // F62 function key
  { finalcut::fc::Fkey_f63       , C_STR(ESC "O1;4R"), "Fr" },  // F63 function key
  { 0                  , 0                 , "\0" }
};

}  // namespace test


//----------------------------------------------------------------------
// class FKeyboardTest
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FKeyboardTest : public CPPUNIT_NS::TestFixture
{
  public:
    FKeyboardTest();
    ~FKeyboardTest();

  protected:
    void classNameTest();
    void noArgumentTest();
    void escapeKeyTest();
    void characterwiseInputTest();
    void severalKeysTest();
    void functionKeyTest();
    void metaKeyTest();
    void sequencesTest();
    void mouseTest();
    void utf8Test();
    void unknownKeyTest();

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FKeyboardTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (noArgumentTest);
    CPPUNIT_TEST (escapeKeyTest);
    CPPUNIT_TEST (characterwiseInputTest);
    CPPUNIT_TEST (severalKeysTest);
    CPPUNIT_TEST (functionKeyTest);
    CPPUNIT_TEST (metaKeyTest);
    CPPUNIT_TEST (sequencesTest);
    CPPUNIT_TEST (mouseTest);
    CPPUNIT_TEST (utf8Test);
    CPPUNIT_TEST (unknownKeyTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
    void init();
    void input (std::string);
    void processInput();
    void clear();
    void keyPressed();
    void keyReleased();
    void escapeKeyPressed();

    // Data Members
    FKey key_pressed;
    FKey key_released;
    int  number_of_keys;
    finalcut::FKeyboard* keyboard;
};
#pragma pack(pop)

//----------------------------------------------------------------------
FKeyboardTest::FKeyboardTest()
  : key_pressed(0)
  , key_released(0)
  , number_of_keys(0)
  , keyboard(0)
{
  init();
}

//----------------------------------------------------------------------
FKeyboardTest::~FKeyboardTest()
{
  delete keyboard;
}

//----------------------------------------------------------------------
void FKeyboardTest::classNameTest()
{
  finalcut::FKeyboard k;
  const char* const classname = k.getClassName();
  CPPUNIT_ASSERT ( std::strcmp(classname, "FKeyboard") == 0 );
}

//----------------------------------------------------------------------
void FKeyboardTest::noArgumentTest()
{
  CPPUNIT_ASSERT ( keyboard->getKey() == 0 );
  finalcut::FKeyboard::keybuffer& buffer = keyboard->getKeyBuffer();
  std::size_t size = sizeof(buffer);

  CPPUNIT_ASSERT ( size == finalcut::FKeyboard::FIFO_BUF_SIZE );
  CPPUNIT_ASSERT ( buffer[0] == 0 );
  int sum = 0;

  for (std::size_t i = 0; i < size; i++)
    sum += int(buffer[i]);

  CPPUNIT_ASSERT ( sum == 0 );

  timeval* time = keyboard->getKeyPressedTime();
  CPPUNIT_ASSERT ( time->tv_sec == 0);
  CPPUNIT_ASSERT ( time->tv_usec == 0);

  CPPUNIT_ASSERT ( ! keyboard->isInputDataPending() );
  CPPUNIT_ASSERT ( ! keyboard->unprocessedInput() );
  CPPUNIT_ASSERT ( ! keyboard->isKeyPressed() );

  keyboard->clearKeyBufferOnTimeout();

  if ( keyboard->isKeyPressed() )
    keyboard->fetchKeyCode();

  // special case: Esc key
  keyboard->escapeKeyHandling();

  CPPUNIT_ASSERT ( keyboard->getKey() == 0 );
}

//----------------------------------------------------------------------
void FKeyboardTest::escapeKeyTest()
{
  std::cout << std::endl;

  // Mintty application escape key
  input("\033O[");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_escape_mintty );
  clear();

  // Normal escape (needs a timeout)
  input("\033");
  processInput();
  // Wait 100 ms (= 100,000,000 ns)
  nanosleep ((const struct timespec[]){{0, 100000000L}}, NULL);
  keyboard->escapeKeyHandling();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_escape );
  keyboard->clearKeyBufferOnTimeout();
  clear();
}

//----------------------------------------------------------------------
void FKeyboardTest::characterwiseInputTest()
{
  std::cout << std::endl;

  // Cursor down in character by character input (e.g. rxvt-cygwin-native)
  input("\033");
  processInput();
  input("[");
  processInput();
  input("B");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( number_of_keys == 1 );
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_down );
  clear();
}

//----------------------------------------------------------------------
void FKeyboardTest::severalKeysTest()
{
  std::cout << std::endl;

  // Input of two 1 byte characters
  input("A");
  input("B");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( number_of_keys == 2 );
  CPPUNIT_ASSERT ( key_pressed == 'B' );
  CPPUNIT_ASSERT ( key_released == 'B' );
  clear();

  // F1, F2, F3
  input("\033[11~\033[12~\033[13~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( number_of_keys == 3 );
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f3 );
  CPPUNIT_ASSERT ( key_released == finalcut::fc::Fkey_f3 );
  clear();
}

//----------------------------------------------------------------------
void FKeyboardTest::functionKeyTest()
{
  // Function key F1 (numeric keypad PF1)
  input("\033OP");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f1 );
  clear();

  // Function key F1
  input("\033[11~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f1 );
  clear();

  // Function key F2 (numeric keypad PF2)
  input("\033OQ");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f2 );
  clear();

  // Function key F2
  input("\033[12~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f2 );
  clear();

  // Function key F3 (numeric keypad PF3)
  input("\033OR");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f3 );
  clear();

  // Function key F3
  input("\033[13~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f3 );
  clear();

  // Function key F4 (numeric keypad PF3)
  input("\033OS");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f4 );
  clear();

  // Function key F4
  input("\033[14~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f4 );
  clear();

  // Function key F5
  input("\033[15~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f5 );
  clear();

  // Function key F6
  input("\033[17~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f6 );
  clear();

  // Function key F7
  input("\033[18~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f7 );
  clear();

  // Function key F8
  input("\033[19~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f8 );
  clear();

  // Function key F9
  input("\033[20~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f9 );
  clear();

  // Function key F10
  input("\033[21~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f10 );
  clear();

  // Function key F11
  input("\033[23~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f11 );
  clear();

  // Function key F12
  input("\033[24~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f12 );
  clear();

  // Function key F13 (shift + F1)
  input("\033O1;2P");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f13 );
  clear();

  // Function key F14 (shift + F2)
  input("\033O1;2Q");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f14 );
  clear();

  // Function key F15 (shift + F3)
  input("\033O1;2R");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f15 );
  clear();

  // Function key F16 (shift + F4)
  input("\033O1;2S");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f16 );
  clear();

  // Function key F17 (shift + F5)
  input("\033[15;2~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f17 );
  clear();

  // Function key F18 (shift + F6)
  input("\033[17;2~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f18 );
  clear();

  // Function key F19 (shift + F7)
  input("\033[18;2~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f19 );
  clear();

  // Function key F20 (shift + F8)
  input("\033[19;2~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f20 );
  clear();

  // Function key F21 (shift + F9)
  input("\033[20;2~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f21 );
  clear();

  // Function key F22 (shift + F10)
  input("\033[21;2~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f22 );
  clear();

  // Function key F23 (shift + F11)
  input("\033[23;2~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f23 );
  clear();

  // Function key F24 (shift + F12)
  input("\033[24;2~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f24 );
  clear();

  // Function key F25 (ctrl + F1)
  input("\033O1;5P");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f25 );
  clear();

  // Function key F26 (ctrl + F2)
  input("\033O1;5Q");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f26 );
  clear();

  // Function key F27 (ctrl + F3)
  input("\033O1;5R");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f27 );
  clear();

  // Function key F28 (ctrl + F4)
  input("\033O1;5S");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f28 );
  clear();

  // Function key F29 (ctrl + F5)
  input("\033[15;5~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f29 );
  clear();

  // Function key F30 (ctrl + F6)
  input("\033[17;5~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f30 );
  clear();

  // Function key F31 (ctrl + F7)
  input("\033[18;5~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f31 );
  clear();

  // Function key F32 (ctrl + F8)
  input("\033[19;5~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f32 );
  clear();

  // Function key F33 (ctrl + F9)
  input("\033[20;5~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f33 );
  clear();

  // Function key F34 (ctrl + F10)
  input("\033[21;5~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f34 );
  clear();

  // Function key F35 (ctrl + F11)
  input("\033[23;5~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f35 );
  clear();

  // Function key F36 (ctrl + F12)
  input("\033[24;5~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f36 );
  clear();

  // Function key F37 (shift + ctrl + F1)
  input("\033O1;6P");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f37 );
  clear();

  // Function key F38 (shift + ctrl + F2)
  input("\033O1;6Q");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f38 );
  clear();

  // Function key F39 (shift + ctrl + F3)
  input("\033O1;6R");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f39 );
  clear();

  // Function key F40 (shift + ctrl + F4)
  input("\033O1;6S");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f40 );
  clear();

  // Function key F41 (shift + ctrl + F5)
  input("\033[15;6~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f41 );
  clear();

  // Function key F42 (shift + ctrl + F6)
  input("\033[17;6~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f42 );
  clear();

  // Function key F43 (shift + ctrl + F7)
  input("\033[18;6~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f43 );
  clear();

  // Function key F44 (shift + ctrl + F8)
  input("\033[19;6~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f44 );
  clear();

  // Function key F45 (shift + ctrl + F9)
  input("\033[20;6~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f45 );
  clear();

  // Function key F46 (shift + ctrl + F10)
  input("\033[21;6~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f46 );
  clear();

  // Function key F47 (shift + ctrl + F11)
  input("\033[23;6~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f47 );
  clear();

  // Function key F48 (shift + ctrl + F12)
  input("\033[24;6~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f48 );
  clear();

  // Function key F49 (meta + F1)
  input("\033O1;3P");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f49 );
  clear();

  // Function key F50 (meta + F2)
  input("\033O1;3Q");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f50 );
  clear();

  // Function key F51 (meta + F3)
  input("\033O1;3R");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f51 );
  clear();

  // Function key F52 (meta + F4)
  input("\033O1;3S");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f52 );
  clear();

  // Function key F53 (meta + F5)
  input("\033[15;3~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f53 );
  clear();

  // Function key F54 (meta + F6)
  input("\033[17;3~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f54 );
  clear();

  // Function key F55 (meta + F7)
  input("\033[18;3~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f55 );
  clear();

  // Function key F56 (meta + F8)
  input("\033[19;3~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f56 );
  clear();

  // Function key F57 (meta + F9)
  input("\033[20;3~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f57 );
  clear();

  // Function key F58 (meta + F10)
  input("\033[21;3~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f58 );
  clear();

  // Function key F59 (meta + F11)
  input("\033[23;3~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f59 );
  clear();

  // Function key F60 (meta + F12)
  input("\033[24;3~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f60 );
  clear();

  // Function key F61 (shift + meta + F1)
  input("\033O1;4P");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f61 );
  clear();

  // Function key F62 (shift + meta + F2)
  input("\033O1;4Q");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f62 );
  clear();

  // Function key F63 (shift + meta + F3)
  input("\033O1;4R");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_f63 );
  clear();
}

//----------------------------------------------------------------------
void FKeyboardTest::metaKeyTest()
{
  std::cout << std::endl;

  // meta-insert
  input("\033[2;3~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_ic );
  clear();

  // meta-insert
  input("\033\033[2~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_ic );
  clear();

  // meta-delete
  input("\033[3;3~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_dc );
  clear();

  // meta-delete
  input("\033\033[3~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_dc );
  clear();

  // meta-home
  input("\033[1;3H");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_home );
  clear();

  // meta-home
  input("\033\033[1~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_home );
  clear();

  // meta-end
  input("\033[1;3F");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_end );
  clear();

  // meta-end
  input("\033\033[4~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_end );
  clear();

  // meta-prev-page
  input("\033[5;3~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_ppage );
  clear();

  // meta-prev-page
  input("\033\033[5~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_ppage );
  clear();

  // meta-next-page
  input("\033[6;3~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_npage );
  clear();

  // meta-next-page
  input("\033\033[6~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_npage );
  clear();

  // meta-f1
  input("\033[1;3P");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_f1 );
  clear();

  // meta-f1
  input("\033\033[11~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_f1 );
  clear();

  // meta-f2
  input("\033[1;3Q");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_f2 );
  clear();

  // meta-f2
  input("\033\033[12~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_f2 );
  clear();

  // meta-f3
  input("\033[1;3R");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_f3 );
  clear();

  // meta-f3
  input("\033\033[13~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_f3 );
  clear();

  // meta-f4
  input("\033[1;3S");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_f4 );
  clear();

  // meta-f4
  input("\033\033[14~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_f4 );
  clear();

  // meta-f5
  input("\033\033[15~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_f5 );
  clear();

  // meta-f6
  input("\033\033[17~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_f6 );
  clear();

  // meta-f7
  input("\033\033[18~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_f7 );
  clear();

  // meta-f8
  input("\033\033[19~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_f8 );
  clear();

  // meta-f9
  input("\033\033[20~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_f9 );
  clear();

  // meta-f10
  input("\033\033[21~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_f10 );
  clear();

  // meta-f11
  input("\033\033[23~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_f11 );
  clear();

  // meta-f12
  input("\033\033[24~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_f12 );
  clear();

  // meta-up
  input("\033[1;3A");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_up );
  clear();

  // meta-up
  input("\033\033[A");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_up );
  clear();

  // meta-down
  input("\033[1;3B");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_down );
  clear();

  // meta-down
  input("\033\033[B");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_down );
  clear();

  // meta-right
  input("\033[1;3C");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_right );
  clear();

  // meta-right
  input("\033\033[C");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_right );
  clear();

  // meta-left
  input("\033[1;3D");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_left );
  clear();

  // meta-left
  input("\033\033[D");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_left );
  clear();

  // shift-meta-insert
  input("\033[2;4~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_sic );
  clear();

  // shift-meta-delete
  input("\033[3;4~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_sdc );
  clear();

  // shift-meta-home
  input("\033[1;4H");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_shome );
  clear();

  // shift-meta-end
  input("\033[1;4F");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_send );
  clear();

  // shift-meta-prev-page
  input("\033[5;4~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_sppage );
  clear();

  // shift-meta-next-page
  input("\033[6;4~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_snpage );
  clear();

  // shift-meta-f1
  input("\033[1;4P");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_sf1 );
  clear();

  // shift-meta-f2
  input("\033[1;4Q");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_sf2 );
  clear();

  // shift-meta-f3
  input("\033[1;4R");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_sf3 );
  clear();

  // shift-meta-f4
  input("\033[1;4S");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_sf4 );
  clear();

  // shift-meta-f5
  input("\033[15;4~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_sf5 );
  clear();

  // shift-meta-f6
  input("\033[17;4~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_sf6 );
  clear();

  // shift-meta-f7
  input("\033[18;4~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_sf7 );
  clear();

  // shift-meta-f8
  input("\033[19;4~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_sf8 );
  clear();

  // shift-meta-f9
  input("\033[20;4~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_sf9 );
  clear();

  // shift-meta-f10
  input("\033[21;4~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_sf10 );
  clear();

  // shift-meta-f11
  input("\033[23;4~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_sf11 );
  clear();

  // shift-meta-f12
  input("\033[24;4~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_sf12 );
  clear();

  // shift-meta-up
  input("\033[1;4A");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_sup );
  clear();

  // shift-meta-down
  input("\033[1;4B");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_sdown );
  clear();

  // shift-meta-right
  input("\033[1;4C");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_sright );
  clear();

  // shift-meta-left
  input("\033[1;4D");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_sleft );
  clear();

  // ctrl-insert
  input("\033[2;5~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fckey_ic );
  clear();

  // ctrl-delete
  input("\033[3;5~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fckey_dc );
  clear();

  // ctrl-home
  input("\033[1;5H");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fckey_home );
  clear();

  // ctrl-end
  input("\033[1;5F");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fckey_end );
  clear();

  // ctrl-prev-page
  input("\033[5;5~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fckey_ppage );
  clear();

  // ctrl-next-page
  input("\033[6;5~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fckey_npage );
  clear();

  // ctrl-up
  input("\033[1;5A");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fckey_up );
  clear();

  // ctrl-down
  input("\033[1;5B");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fckey_down );
  clear();

  // ctrl-right
  input("\033[1;5C");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fckey_right );
  clear();

  // ctrl-left
  input("\033[1;5D");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fckey_left );
  clear();

  // shift-ctrl-meta-insert
  input("\033[2;6~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fckey_sic );
  clear();

  // shift-ctrl-meta-delete
  input("\033[3;6~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fckey_sdc );
  clear();

  // shift-ctrl-meta-home
  input("\033[1;6H");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fckey_shome );
  clear();

  // shift-ctrl-meta-end
  input("\033[1;6F");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fckey_send );
  clear();

  // shift-ctrl-meta-prev-page
  input("\033[5;6~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fckey_sppage );
  clear();

  // shift-ctrl-meta-next-page
  input("\033[6;6~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fckey_snpage );
  clear();

  // shift-ctrl-meta-up
  input("\033[1;6A");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fckey_sup );
  clear();

  // shift-ctrl-meta-down
  input("\033[1;6B");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fckey_sdown );
  clear();

  // shift-ctrl-meta-right
  input("\033[1;6C");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fckey_sright );
  clear();

  // shift-ctrl-meta-left
  input("\033[1;6D");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fckey_sleft );
  clear();

  // ctrl-meta-insert
  input("\033[2;7~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_ic );
  clear();

  // ctrl-meta-delete
  input("\033[3;7~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_dc );
  clear();

  // ctrl-meta-home
  input("\033[1;7H");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_home );
  clear();

  // ctrl-meta-end
  input("\033[1;7F");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_end );
  clear();

  // ctrl-meta-prev-page
  input("\033[5;7~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_ppage );
  clear();

  // ctrl-meta-next-page
  input("\033[6;7~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_npage );
  clear();

  // ctrl-meta-up
  input("\033[1;7A");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_up );
  clear();

  // ctrl-meta-down
  input("\033[1;7B");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_down );
  clear();

  // ctrl-meta-right
  input("\033[1;7C");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_right );
  clear();

  // ctrl-meta-left
  input("\033[1;7D");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_left );
  clear();

  // shift-ctrl-meta-insert
  input("\033[2;8~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_sic );
  clear();

  // shift-ctrl-meta-delete
  input("\033[3;8~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_sdc );
  clear();

  // shift-ctrl-meta-home
  input("\033[1;8H");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_shome );
  clear();

  // shift-ctrl-meta-end
  input("\033[1;8F");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_send );
  clear();

  // shift-ctrl-meta-prev-page
  input("\033[5;8~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_sppage );
  clear();

  // shift-ctrl-meta-next-page
  input("\033[6;8~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_snpage );
  clear();

  // shift-ctrl-meta-f1
  input("\033[1;8P");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_sf1 );
  clear();

  // shift-ctrl-meta-f2
  input("\033[1;8Q");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_sf2 );
  clear();

  // shift-ctrl-meta-f3
  input("\033[1;8R");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_sf3 );
  clear();

  // shift-ctrl-meta-f4
  input("\033[1;8S");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_sf4 );
  clear();

  // shift-ctrl-meta-f5
  input("\033[15;8~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_sf5 );
  clear();

  // shift-ctrl-meta-f6
  input("\033[17;8~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_sf6 );
  clear();

  // shift-ctrl-meta-f7
  input("\033[18;8~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_sf7 );
  clear();

  // shift-ctrl-meta-f8
  input("\033[19;8~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_sf8 );
  clear();

  // shift-ctrl-meta-f9
  input("\033[20;8~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_sf9 );
  clear();

  // shift-ctrl-meta-f10
  input("\033[21;8~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_sf10 );
  clear();

  // shift-ctrl-meta-f11
  input("\033[23;8~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_sf11 );
  clear();

  // shift-ctrl-meta-f12
  input("\033[24;8~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_sf12 );
  clear();

  // shift-ctrl-meta-up
  input("\033[1;8A");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_sup );
  clear();

  // shift-ctrl-meta-down
  input("\033[1;8B");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_sdown );
  clear();

  // shift-ctrl-meta-right
  input("\033[1;8C");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_sright );
  clear();

  // shift-ctrl-meta-left
  input("\033[1;8D");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_sleft );
  clear();

  // menu
  input("\033[29~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_menu );
  clear();

  // shift-menu
  input("\033[29$");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_smenu );
  clear();

  // shift-menu
  input("\033[29;2~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_smenu );
  clear();

  // ctrl-menu
  input("\033[29^");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fckey_menu );
  clear();

  // ctrl-menu
  input("\033[29;5~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fckey_menu );
  clear();

  // shift-ctrl-menu
  input("\033[29@");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fckey_smenu );
  clear();

  // shift-ctrl-menu
  input("\033[29;6~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fckey_smenu );
  clear();

  // meta-menu
  input("\033[29;3~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_menu );
  clear();

  // shift-meta-menu
  input("\033[29;4~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_smenu );
  clear();

  // ctrl-meta-menu
  input("\033[29;7~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_menu );
  clear();

  // shift-ctrl-meta-menu
  input("\033[29;8~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fcmkey_smenu );
  clear();

  // meta-tab
  input("\033\t");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_tab );
  clear();

  // meta-enter
  input("\033\n");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_enter );
  clear();

  // meta-enter
  input("\033\r");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_enter );
  clear();

  // meta-' '
  input("\033 ");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_space );
  clear();

  // meta-!
  input("\033!");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_bang );
  clear();

  // meta-"
  input("\033\"");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_quotes );
  clear();

  // meta-#
  input("\033#");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_hash );
  clear();

  // meta-$
  input("\033$");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_dollar );
  clear();

  // meta-%
  input("\033%");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_percent );
  clear();

  // meta-&
  input("\033&");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_ampersand );
  clear();

  // meta-'
  input("\033'");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_apostrophe );
  clear();

  // meta-(
  input("\033(");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_left_parenthesis );
  clear();

  // meta-)
  input("\033)");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_right_parenthesis );
  clear();

  // meta-*
  input("\033*");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_asterisk );
  clear();

  // meta-+
  input("\033+");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_plus );
  clear();

  // meta-,
  input("\033,");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_comma );
  clear();

  // meta-'-'
  input("\033-");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_minus );
  clear();

  // meta-.
  input("\033.");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_full_stop );
  clear();

  // meta-/
  input("\033/");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_slash );
  clear();

  // meta-0
  input("\0330");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_0 );
  clear();

  // meta-1
  input("\0331");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_1 );
  clear();

  // meta-2
  input("\0332");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_2 );
  clear();

  // meta-3
  input("\0333");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_3 );
  clear();

  // meta-4
  input("\0334");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_4 );
  clear();

  // meta-5
  input("\0335");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_5 );
  clear();

  // meta-6
  input("\0336");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_6 );
  clear();

  // meta-7
  input("\0337");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_7 );
  clear();

  // meta-8
  input("\0338");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_8 );
  clear();

  // meta-9
  input("\0339");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_9 );
  clear();

  // meta-:
  input("\033:");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_colon );
  clear();

  // meta-;
  input("\033;");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_semicolon );
  clear();

  // meta-<
  input("\033<");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_less_than );
  clear();

  // meta-=
  input("\033=");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_equals );
  clear();

  // meta->
  input("\033>");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_greater_than );
  clear();

  // meta-?
  input("\033?");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_question_mark );
  clear();

  // meta-@
  input("\033@");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_at );
  clear();

  // shifted meta-A
  input("\033A");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_A );
  clear();

  // shifted meta-B
  input("\033B");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_B );
  clear();

  // shifted meta-C
  input("\033C");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_C );
  clear();

  // shifted meta-D
  input("\033D");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_D );
  clear();

  // shifted meta-E
  input("\033E");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_E );
  clear();

  // shifted meta-F
  input("\033F");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_F );
  clear();

  // shifted meta-G
  input("\033G");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_G );
  clear();

  // shifted meta-H
  input("\033H");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_H );
  clear();

  // shifted meta-I
  input("\033I");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_I );
  clear();

  // shifted meta-J
  input("\033J");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_J );
  clear();

  // shifted meta-K
  input("\033K");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_K );
  clear();

  // shifted meta-L
  input("\033L");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_L );
  clear();

  // shifted meta-M
  input("\033M");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_M );
  clear();

  // shifted meta-N
  input("\033N");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_N );
  clear();

  // shifted meta-O
  input("\033O");
  processInput();
  // Wait 100 ms - Substring keys needs a timeout
  nanosleep ((const struct timespec[]){{0, 100000000L}}, NULL);
  keyboard->escapeKeyHandling();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_O );
  clear();

  // shifted meta-P
  input("\033P");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_P );
  clear();

  // shifted meta-Q
  input("\033Q");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_Q );
  clear();

  // shifted meta-R
  input("\033R");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_R );
  clear();

  // shifted meta-S
  input("\033S");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_S );
  clear();

  // shifted meta-T
  input("\033T");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_T );
  clear();

  // shifted meta-U
  input("\033U");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_U );
  clear();

  // shifted meta-V
  input("\033V");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_V );
  clear();

  // shifted meta-W
  input("\033W");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_W );
  clear();

  // shifted meta-X
  input("\033X");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_X );
  clear();

  // shifted meta-Y
  input("\033Y");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_Y );
  clear();

  // shifted meta-Z
  input("\033Z");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_Z );
  clear();

  // meta-[
  input("\033[");
  processInput();
  // Wait 100 ms - Substring keys needs a timeout
  nanosleep ((const struct timespec[]){{0, 100000000L}}, NULL);
  keyboard->escapeKeyHandling();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_left_square_bracket );
  clear();

  // meta-'\'
  input("\033\\");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_backslash );
  clear();

  // meta-]
  input("\033]");
  processInput();
  // Wait 100 ms - Substring keys needs a timeout
  nanosleep ((const struct timespec[]){{0, 100000000L}}, NULL);
  keyboard->escapeKeyHandling();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_right_square_bracket );
  clear();

  // meta-^
  input("\033^");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_caret );
  clear();

  // meta-_
  input("\033_");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_underscore );
  clear();

  // meta-`
  input("\033`");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_grave_accent );
  clear();

  // meta-a
  input("\033a");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_a );
  clear();

  // meta-b
  input("\033b");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_b );
  clear();

  // meta-c
  input("\033c");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_c );
  clear();

  // meta-d
  input("\033d");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_d );
  clear();

  // meta-e
  input("\033e");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_e );
  clear();

  // meta-f
  input("\033f");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_f );
  clear();

  // meta-g
  input("\033g");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_g );
  clear();

  // meta-h
  input("\033h");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_h );
  clear();

  // meta-i
  input("\033i");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_i );
  clear();

  // meta-j
  input("\033j");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_j );
  clear();

  // meta-k
  input("\033k");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_k );
  clear();

  // meta-l
  input("\033l");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_l );
  clear();

  // meta-m
  input("\033m");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_m );
  clear();

  // meta-n
  input("\033n");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_n );
  clear();

  // meta-o
  input("\033o");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_o );
  clear();

  // meta-p
  input("\033p");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_p );
  clear();

  // meta-q
  input("\033q");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_q );
  clear();

  // meta-r
  input("\033r");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_r );
  clear();

  // meta-s
  input("\033s");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_s );
  clear();

  // meta-t
  input("\033t");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_t );
  clear();

  // meta-u
  input("\033u");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_u );
  clear();

  // meta-v
  input("\033v");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_v );
  clear();

  // meta-w
  input("\033w");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_w );
  clear();

  // meta-x
  input("\033x");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_x );
  clear();

  // meta-y
  input("\033y");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_y );
  clear();

  // meta-z
  input("\033z");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_z );
  clear();

  // meta-{
  input("\033{");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_left_curly_bracket );
  clear();

  // meta-|
  input("\033|");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_vertical_bar );
  clear();

  // meta-}
  input("\033}");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_right_curly_bracket );
  clear();

  // meta-~
  input("\033~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fmkey_tilde );
  clear();
}

//----------------------------------------------------------------------
void FKeyboardTest::sequencesTest()
{
  std::cout << std::endl;

  // Clear-tab
  input("\033[3~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_ctab );
  clear();

  // Cursor up key in applications mode
  input("\033OA");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_up );
  clear();

  // Cursor up key in positioning mode
  input("\033[A");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_up );
  clear();

  // Cursor down key in applications mode
  input("\033OB");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_down );
  clear();

  // Cursor down key in positioning mode
  input("\033[B");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_down );
  clear();

  // Cursor right key in applications mode
  input("\033OC");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_right );
  clear();

  // Cursor right key in positioning mode
  input("\033[C");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_right );
  clear();

  // Cursor left key in applications mode
  input("\033OD");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_left );
  clear();

  // Cursor left key in applications mode
  input("\033OD");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_left );
  clear();

  // Home key in positioning mode
  input("\033[7~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_home );
  clear();

  // Home key in applications mode
  input("\033OH");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_home );
  clear();

  // End key in positioning mode
  input("\033[8~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_end );
  clear();

  // End key in applications mode
  input("\033OF");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_end );
  clear();

  // End key (ANSI terminal)
  input("\033[K");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_end );
  clear();

  // Next-page key (Page down)
  input("\033[6~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_npage );
  clear();

  // Previous-page key (Page up)
  input("\033[5~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_ppage );
  clear();

  // Insert key
  input("\033[2~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_ic );
  clear();

  // Scroll-forward key (shift + up-arrow)
  input("\033[1;2B");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_sf );
  clear();

  // Scroll-backward key (shift + down-arrow)
  input("\033[1;2A");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_sr );
  clear();

  // Center of keypad
  input("\033[E");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_b2 );
  clear();

  // back-tab key
  input("\033[Z");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_btab );
  clear();

  // find key
  input("\033[1~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_find );
  clear();

  // select key
  input("\033[4~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_select );
  clear();

  // shifted delete-character key
  input("\033[3;2~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_sdc );
  clear();

  // shifted end  key
  input("\033[1;2F");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_send );
  clear();

  // shifted home key
  input("\033[1;2H");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_shome );
  clear();

  // shifted insert-character key
  input("\033[2;2~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_sic );
  clear();

  // shifted left-arrow key
  input("\033[1;2D");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_sleft );
  clear();

  // shifted next key
  input("\033[6;2~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_snext );
  clear();

  // shifted previous key
  input("\033[5;2~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_sprevious );
  clear();

  // shifted right-arrow  key
  input("\033[1;2C");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_sright );
  clear();

  // Keypad slash (numlock off)
  input("\033Oo");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_slash );
  clear();

  // Keypad asterisk (numlock off)
  input("\033Oj");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_asterisk );
  clear();

  // Keypad minus sign (numlock off)
  input("\033Om");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_minus_sign );
  clear();

  // Keypad plus sign (numlock off)
  input("\033Ok");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_plus_sign );
  clear();
}

//----------------------------------------------------------------------
void FKeyboardTest::mouseTest()
{
  std::cout << std::endl;

  // X11 mouse
  input("\033[M Z2");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_mouse );
  clear();

  // SGR mouse
  input("\033[<0;11;7M");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_extended_mouse );
  clear();

  // URXVT mouse
  input("\033[32;11;7M");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == finalcut::fc::Fkey_urxvt_mouse );
  clear();

  // Without mouse support
  keyboard->disableMouseSequences();
  input("\033[M Z2");
  processInput();
  CPPUNIT_ASSERT ( key_pressed != finalcut::fc::Fkey_mouse );
  clear();
  input("\033[<0;11;7M");
  processInput();
  CPPUNIT_ASSERT ( key_pressed != finalcut::fc::Fkey_extended_mouse );
  clear();
  input("\033[32;11;7M");
  processInput();
  CPPUNIT_ASSERT ( key_pressed != finalcut::fc::Fkey_urxvt_mouse );
  clear();
}

//----------------------------------------------------------------------
void FKeyboardTest::utf8Test()
{
  std::cout << std::endl;

  // UTF-8 encoded character

  // Linear B syllable (4 bytes)
  input("\360\220\200\200");
  processInput();
  std::cout << " - code: " << key_pressed << std::endl;
  CPPUNIT_ASSERT ( key_pressed == 0x10000 );

  // Euro sign (3 bytes)
  input("\342\202\254");
  processInput();
  std::cout << " - code: " << key_pressed << std::endl;
  CPPUNIT_ASSERT ( key_pressed == 0x20ac );

  // u with two Dots (2 bytes)
  input("\303\274");
  processInput();
  std::cout << " - code: " << key_pressed << std::endl;
  CPPUNIT_ASSERT ( key_pressed == 0x00fc );

  // @ (1 bytes)
  input("@");
  processInput();
  std::cout << " - code: " << key_pressed << std::endl;
  CPPUNIT_ASSERT ( key_pressed == 0x0040 );

  // Invalid UTF-8
  key_pressed = 0xffffffff;
  input("\377");
  processInput();
  CPPUNIT_ASSERT ( key_pressed == 0xffffffff );

  // Without UTF-8 support
  keyboard->disableUTF8();
  input("\360\220\200\200");
  processInput();
  CPPUNIT_ASSERT ( key_released == 128 );
  clear();
}

//----------------------------------------------------------------------
void FKeyboardTest::unknownKeyTest()
{
  std::cout << std::endl;

  // Unknown key code
  CPPUNIT_ASSERT ( keyboard->getKeyName(0xf8d0) == "" );

  // Unknown input
  input("\033[_.");
  processInput();
  CPPUNIT_ASSERT ( key_pressed == 0 );
  CPPUNIT_ASSERT ( keyboard->getKeyName(key_pressed) == "" );
}

//----------------------------------------------------------------------
void FKeyboardTest::init()
{
  keyboard = new finalcut::FKeyboard();
  finalcut::FApplication* object = \
      reinterpret_cast<finalcut::FApplication*>(this);
  void (finalcut::FApplication::*method1)()
      = reinterpret_cast<void(finalcut::FApplication::*)()>(&FKeyboardTest::keyPressed);
  void (finalcut::FApplication::*method2)()
      = reinterpret_cast<void(finalcut::FApplication::*)()>(&FKeyboardTest::keyReleased);
  void (finalcut::FApplication::*method3)()
      = reinterpret_cast<void(finalcut::FApplication::*)()>(&FKeyboardTest::escapeKeyPressed);
  finalcut::FKeyboardCommand key_cmd1 (object, method1);
  finalcut::FKeyboardCommand key_cmd2 (object, method2);
  finalcut::FKeyboardCommand key_cmd3 (object, method3);
  keyboard->setPressCommand (key_cmd1);
  keyboard->setReleaseCommand (key_cmd2);
  keyboard->setEscPressedCommand (key_cmd3);
  keyboard->setKeypressTimeout (100000);  // 100 ms
  processInput();
  CPPUNIT_ASSERT ( key_pressed == 0 );
  keyboard->enableUTF8();
  keyboard->enableMouseSequences();
  keyboard->setTermcapMap (reinterpret_cast<finalcut::fc::fkeymap*>(test::Fkey));
}

//----------------------------------------------------------------------
void FKeyboardTest::input (std::string s)
{
  // Simulates keystrokes

  const char EOT = 0x04;  // End of Transmission

  std::string::const_iterator iter;
  iter = s.begin();

  while ( iter != s.end() )
  {
    char c = *iter;

    if ( ioctl (finalcut::FTermios::getStdIn(), TIOCSTI, &c) < 0 )
      break;

    ++iter;
  }

  if ( ioctl (finalcut::FTermios::getStdIn(), TIOCSTI, &EOT) < 0 )
    return;
}

//----------------------------------------------------------------------
void FKeyboardTest::processInput()
{
  keyboard->clearKeyBufferOnTimeout();

  if ( keyboard->isKeyPressed() )
    keyboard->fetchKeyCode();

  // special case: Esc key
  keyboard->escapeKeyHandling();
}

//----------------------------------------------------------------------
void FKeyboardTest::clear()
{
  keyboard->clearKeyBuffer();
  number_of_keys = \
     key_pressed = \
     key_released = 0;
}

//----------------------------------------------------------------------
void FKeyboardTest::keyPressed()
{
  key_pressed = keyboard->getKey();
  number_of_keys++;
}

//----------------------------------------------------------------------
void FKeyboardTest::keyReleased()
{
  key_released = keyboard->getKey();
}

//----------------------------------------------------------------------
void FKeyboardTest::escapeKeyPressed()
{
  key_pressed = finalcut::fc::Fkey_escape;
  key_released = finalcut::fc::Fkey_escape;
  number_of_keys++;
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FKeyboardTest);

// The general unit test main part
#include <main-test.inc>
