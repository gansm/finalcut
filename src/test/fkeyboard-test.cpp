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
  { fc::Fkey_backspace, C_STR("\177")     , "kb" },  // backspace key
  { fc::Fkey_catab    , 0                 , "ka" },  // clear-all-tabs key
  { fc::Fkey_clear    , 0                 , "kC" },  // clear-screen or erase key
  { fc::Fkey_ctab     , C_STR(CSI "3~")   , "kt" },  // clear-tab key
  { fc::Fkey_dc       , 0                 , "kD" },  // delete-character key
  { fc::Fkey_dl       , 0                 , "kL" },  // delete-line key
  { fc::Fkey_down     , C_STR(ESC "OB")   , "kd" },  // down-arrow key
  { fc::Fkey_down     , C_STR(CSI "B")    , "kdx"},  // down-arrow key
  { fc::Fkey_eic      , 0                 , "kM" },  // sent by rmir or smir in insert mode
  { fc::Fkey_eol      , 0                 , "kE" },  // clear-to-end-of-line key
  { fc::Fkey_eos      , 0                 , "kS" },  // clear-to-end-of-screen key
  { fc::Fkey_f0       , 0                 , "k0" },  // F0 function key
  { fc::Fkey_f1       , C_STR(ESC "OP")   , "k1" },  // F1 function key
  { fc::Fkey_f1       , C_STR(CSI "11~")  , "k1x"},  // F1 function key
  { fc::Fkey_f1       , C_STR(ESC "OP")   , "k1X"},  // F1 function key
  { fc::Fkey_f2       , C_STR(ESC "OQ")   , "k2" },  // F2 function key
  { fc::Fkey_f2       , C_STR(CSI "12~")  , "k2x"},  // F2 function key
  { fc::Fkey_f2       , C_STR(ESC "OQ")   , "k2X"},  // F2 function key
  { fc::Fkey_f3       , C_STR(ESC "OR")   , "k3" },  // F3 function key
  { fc::Fkey_f3       , C_STR(CSI "13~")  , "k3x"},  // F3 function key
  { fc::Fkey_f3       , C_STR(ESC "OR")   , "k3X"},  // F3 function key
  { fc::Fkey_f4       , C_STR(ESC "OS")   , "k4" },  // F4 function key
  { fc::Fkey_f4       , C_STR(CSI "14~")  , "k4x"},  // F4 function key
  { fc::Fkey_f4       , C_STR(ESC "OS")   , "k4X"},  // F4 function key
  { fc::Fkey_f5       , C_STR(CSI "15~")  , "k5" },  // F5 function key
  { fc::Fkey_f6       , C_STR(CSI "17~")  , "k6" },  // F6 function key
  { fc::Fkey_f7       , C_STR(CSI "18~")  , "k7" },  // F7 function key
  { fc::Fkey_f8       , C_STR(CSI "19~")  , "k8" },  // F8 fucntion key
  { fc::Fkey_f9       , C_STR(CSI "20~")  , "k9" },  // F9 function key
  { fc::Fkey_f10      , C_STR(CSI "21~")  , "k;" },  // F10 function key
  { fc::Fkey_home     , C_STR(ESC "OH")   , "kh" },  // home key
  { fc::Fkey_home     , C_STR(CSI "7~")   , "khx"},  // home key
  { fc::Fkey_ic       , C_STR(CSI "2~")   , "kI" },  // insert-character key
  { fc::Fkey_il       , 0                 , "kA" },  // insert-line key
  { fc::Fkey_left     , C_STR(ESC "OD")   , "kl" },  // left-arrow key
  { fc::Fkey_left     , C_STR(CSI "D")    , "klx"},  // left-arrow key
  { fc::Fkey_ll       , 0                 , "kH" },  // last-line key
  { fc::Fkey_npage    , C_STR(CSI "6~")   , "kN" },  // next-page key
  { fc::Fkey_ppage    , C_STR(CSI "5~")   , "kP" },  // prev-page key
  { fc::Fkey_right    , C_STR(ESC "OC")   , "kr" },  // right-arrow key
  { fc::Fkey_right    , C_STR(CSI "C")    , "krx"},  // right-arrow key
  { fc::Fkey_sf       , C_STR(CSI "1;2B") , "kF" },  // scroll-forward key
  { fc::Fkey_sr       , C_STR(CSI "1;2A") , "kR" },  // scroll-backward key
  { fc::Fkey_stab     , 0                 , "kT" },  // set-tab key
  { fc::Fkey_up       , C_STR(ESC "OA")   , "ku" },  // up-arrow key
  { fc::Fkey_up       , C_STR(CSI "A")    , "kux"},  // up-arrow key
  { fc::Fkey_a1       , 0                 , "K1" },  // upper left of keypad
  { fc::Fkey_a3       , 0                 , "K3" },  // upper right of keypad
  { fc::Fkey_b2       , C_STR(CSI "E")    , "K2" },  // center of keypad
  { fc::Fkey_c1       , 0                 , "K4" },  // lower left of keypad
  { fc::Fkey_c3       , 0                 , "K5" },  // lower right of keypad
  { fc::Fkey_btab     , C_STR(CSI "Z")    , "kB" },  // back-tab key
  { fc::Fkey_beg      , 0                 , "@1" },  // begin key
  { fc::Fkey_cancel   , 0                 , "@2" },  // cancel key
  { fc::Fkey_close    , 0                 , "@3" },  // close key
  { fc::Fkey_command  , 0                 , "@4" },  // command key
  { fc::Fkey_copy     , 0                 , "@5" },  // copy key
  { fc::Fkey_create   , 0                 , "@6" },  // create key
  { fc::Fkey_end      , C_STR(ESC "OF")   , "@7" },  // end key
  { fc::Fkey_end      , C_STR(CSI "8~")   , "@7x"},  // end key
  { fc::Fkey_end      , C_STR(CSI "K")    , "@7X"},  // end key
  { fc::Fkey_enter    , 0                 , "@8" },  // enter/send key
  { fc::Fkey_exit     , 0                 , "@9" },  // exit key
  { fc::Fkey_find     , C_STR(CSI "1~")   , "@0" },  // find key
  { fc::Fkey_help     , 0                 , "%1" },  // help key
  { fc::Fkey_mark     , 0                 , "%2" },  // mark key
  { fc::Fkey_message  , 0                 , "%3" },  // message key
  { fc::Fkey_move     , 0                 , "%4" },  // move key
  { fc::Fkey_next     , 0                 , "%5" },  // next key
  { fc::Fkey_open     , 0                 , "%6" },  // open key
  { fc::Fkey_options  , 0                 , "%7" },  // options key
  { fc::Fkey_previous , 0                 , "%8" },  // previous key
  { fc::Fkey_print    , 0                 , "%9" },  // print key
  { fc::Fkey_redo     , 0                 , "%0" },  // redo key
  { fc::Fkey_reference, 0                 , "&1" },  // reference key
  { fc::Fkey_refresh  , 0                 , "&2" },  // refresh key
  { fc::Fkey_replace  , 0                 , "&3" },  // replace key
  { fc::Fkey_restart  , 0                 , "&4" },  // restart key
  { fc::Fkey_resume   , 0                 , "&5" },  // resume key
  { fc::Fkey_save     , 0                 , "&6" },  // save key
  { fc::Fkey_suspend  , 0                 , "&7" },  // suspend key
  { fc::Fkey_undo     , 0                 , "&8" },  // undo key
  { fc::Fkey_sbeg     , 0                 , "&9" },  // shifted key
  { fc::Fkey_scancel  , 0                 , "&0" },  // shifted key
  { fc::Fkey_scommand , 0                 , "*1" },  // shifted key
  { fc::Fkey_scopy    , 0                 , "*2" },  // shifted key
  { fc::Fkey_screate  , 0                 , "*3" },  // shifted key
  { fc::Fkey_sdc      , C_STR(CSI "3;2~") , "*4" },  // shifted key
  { fc::Fkey_sdl      , 0                 , "*5" },  // shifted key
  { fc::Fkey_select   , C_STR(CSI "4~")   , "*6" },  // select key
  { fc::Fkey_send     , C_STR(CSI "1;2F") , "*7" },  // shifted key
  { fc::Fkey_seol     , 0                 , "*8" },  // shifted key
  { fc::Fkey_sexit    , 0                 , "*9" },  // shifted key
  { fc::Fkey_sfind    , 0                 , "*0" },  // shifted key
  { fc::Fkey_shelp    , 0                 , "#1" },  // shifted key
  { fc::Fkey_shome    , C_STR(CSI "1;2H") , "#2" },  // shifted key
  { fc::Fkey_sic      , C_STR(CSI "2;2~") , "#3" },  // shifted key
  { fc::Fkey_sleft    , C_STR(CSI "1;2D") , "#4" },  // shifted key
  { fc::Fkey_smessage , 0                 , "%a" },  // shifted key
  { fc::Fkey_smove    , 0                 , "%b" },  // shifted key
  { fc::Fkey_snext    , C_STR(CSI "6;2~") , "%c" },  // shifted key
  { fc::Fkey_soptions , 0                 , "%d" },  // shifted key
  { fc::Fkey_sprevious, C_STR(CSI "5;2~") , "%e" },  // shifted key
  { fc::Fkey_sprint   , 0                 , "%f" },  // shifted key
  { fc::Fkey_sredo    , 0                 , "%g" },  // shifted key
  { fc::Fkey_sreplace , 0                 , "%h" },  // shifted key
  { fc::Fkey_sright   , C_STR(CSI "1;2C") , "%i" },  // shifted key
  { fc::Fkey_srsume   , 0                 , "%j" },  // shifted key
  { fc::Fkey_ssave    , 0                 , "!1" },  // shifted key
  { fc::Fkey_ssuspend , 0                 , "!2" },  // shifted key
  { fc::Fkey_sundo    , 0                 , "!3" },  // shifted key
  { fc::Fkey_f11      , C_STR(CSI "23~")  , "F1" },  // F11 function key
  { fc::Fkey_f12      , C_STR(CSI "24~")  , "F2" },  // F12 function key
  { fc::Fkey_f13      , C_STR(ESC "O1;2P"), "F3" },  // F13 function key
  { fc::Fkey_f14      , C_STR(ESC "O1;2Q"), "F4" },  // F14 function key
  { fc::Fkey_f15      , C_STR(ESC "O1;2R"), "F5" },  // F15 function key
  { fc::Fkey_f16      , C_STR(ESC "O1;2S"), "F6" },  // F16 function key
  { fc::Fkey_f17      , C_STR(CSI "15;2~"), "F7" },  // F17 function key
  { fc::Fkey_f18      , C_STR(CSI "17;2~"), "F8" },  // F18 function key
  { fc::Fkey_f19      , C_STR(CSI "18;2~"), "F9" },  // F19 function key
  { fc::Fkey_f20      , C_STR(CSI "19;2~"), "FA" },  // F20 function key
  { fc::Fkey_f21      , C_STR(CSI "20;2~"), "FB" },  // F21 function key
  { fc::Fkey_f22      , C_STR(CSI "21;2~"), "FC" },  // F22 function key
  { fc::Fkey_f23      , C_STR(CSI "23;2~"), "FD" },  // F23 function key
  { fc::Fkey_f24      , C_STR(CSI "24;2~"), "FE" },  // F24 function key
  { fc::Fkey_f25      , C_STR(ESC "O1;5P"), "FF" },  // F25 function key
  { fc::Fkey_f26      , C_STR(ESC "O1;5Q"), "FG" },  // F26 function key
  { fc::Fkey_f27      , C_STR(ESC "O1;5R"), "FH" },  // F27 function key
  { fc::Fkey_f28      , C_STR(ESC "O1;5S"), "FI" },  // F28 function key
  { fc::Fkey_f29      , C_STR(CSI "15;5~"), "FJ" },  // F29 function key
  { fc::Fkey_f30      , C_STR(CSI "17;5~"), "FK" },  // F30 function key
  { fc::Fkey_f31      , C_STR(CSI "18;5~"), "FL" },  // F31 function key
  { fc::Fkey_f32      , C_STR(CSI "19;5~"), "FM" },  // F32 function key
  { fc::Fkey_f33      , C_STR(CSI "20;5~"), "FN" },  // F33 function key
  { fc::Fkey_f34      , C_STR(CSI "21;5~"), "FO" },  // F34 function key
  { fc::Fkey_f35      , C_STR(CSI "23;5~"), "FP" },  // F35 function key
  { fc::Fkey_f36      , C_STR(CSI "24;5~"), "FQ" },  // F36 function key
  { fc::Fkey_f37      , C_STR(ESC "O1;6P"), "FR" },  // F37 function key
  { fc::Fkey_f38      , C_STR(ESC "O1;6Q"), "FS" },  // F38 function key
  { fc::Fkey_f39      , C_STR(ESC "O1;6R"), "FT" },  // F39 function key
  { fc::Fkey_f40      , C_STR(ESC "O1;6S"), "FU" },  // F40 function key
  { fc::Fkey_f41      , C_STR(CSI "15;6~"), "FV" },  // F41 function key
  { fc::Fkey_f42      , C_STR(CSI "17;6~"), "FW" },  // F42 function key
  { fc::Fkey_f43      , C_STR(CSI "18;6~"), "FX" },  // F43 function key
  { fc::Fkey_f44      , C_STR(CSI "19;6~"), "FY" },  // F44 function key
  { fc::Fkey_f45      , C_STR(CSI "20;6~"), "FZ" },  // F45 function key
  { fc::Fkey_f46      , C_STR(CSI "21;6~"), "Fa" },  // F46 function key
  { fc::Fkey_f47      , C_STR(CSI "23;6~"), "Fb" },  // F47 function key
  { fc::Fkey_f48      , C_STR(CSI "24;6~"), "Fc" },  // F48 function key
  { fc::Fkey_f49      , C_STR(ESC "O1;3P"), "Fd" },  // F49 function key
  { fc::Fkey_f50      , C_STR(ESC "O1;3Q"), "Fe" },  // F50 function key
  { fc::Fkey_f51      , C_STR(ESC "O1;3R"), "Ff" },  // F51 function key
  { fc::Fkey_f52      , C_STR(ESC "O1;3S"), "Fg" },  // F52 function key
  { fc::Fkey_f53      , C_STR(CSI "15;3~"), "Fh" },  // F53 function key
  { fc::Fkey_f54      , C_STR(CSI "17;3~"), "Fi" },  // F54 function key
  { fc::Fkey_f55      , C_STR(CSI "18;3~"), "Fj" },  // F55 function key
  { fc::Fkey_f56      , C_STR(CSI "19;3~"), "Fk" },  // F56 function key
  { fc::Fkey_f57      , C_STR(CSI "20;3~"), "Fl" },  // F57 function key
  { fc::Fkey_f58      , C_STR(CSI "21;3~"), "Fm" },  // F58 function key
  { fc::Fkey_f59      , C_STR(CSI "23;3~"), "Fn" },  // F59 function key
  { fc::Fkey_f60      , C_STR(CSI "24;3~"), "Fo" },  // F60 function key
  { fc::Fkey_f61      , C_STR(ESC "O1;4P"), "Fp" },  // F61 function key
  { fc::Fkey_f62      , C_STR(ESC "O1;4Q"), "Fq" },  // F62 function key
  { fc::Fkey_f63      , C_STR(ESC "O1;4R"), "Fr" },  // F63 function key
  { 0                 , 0                 , "\0" }
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

  protected:
    void classNameTest();
    void noArgumentTest();
    void inputTest();

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FKeyboardTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (noArgumentTest);
    CPPUNIT_TEST (inputTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
    void input (std::string);
    void processInput();
    void clear();
    void keyPressed();
    void keyReleased();
    void escapeKeyPressed();

    // Data Members
    int key_pressed;
    int key_released;
    FKeyboard* keyboard;
    //FTerm t;
};
#pragma pack(pop)

FKeyboardTest::FKeyboardTest()
  : key_pressed(0)
  , key_released(0)
  , keyboard(0)
{
  keyboard = new FKeyboard();
  FApplication* object = reinterpret_cast<FApplication*>(this);
  void (FApplication::*method1)()
      = reinterpret_cast<void(FApplication::*)()>(&FKeyboardTest::keyPressed);
  void (FApplication::*method2)()
      = reinterpret_cast<void(FApplication::*)()>(&FKeyboardTest::keyReleased);
  void (FApplication::*method3)()
      = reinterpret_cast<void(FApplication::*)()>(&FKeyboardTest::escapeKeyPressed);
  FKeyboardCommand key_cmd1 (object, method1);
  FKeyboardCommand key_cmd2 (object, method2);
  FKeyboardCommand key_cmd3 (object, method3);
  keyboard->setPressCommand (key_cmd1);
  keyboard->setReleaseCommand (key_cmd2);
  keyboard->setEscPressedCommand (key_cmd3);
  keyboard->setKeypressTimeout (100000);  // 100 ms
  keyboard->enableUTF8();
  keyboard->enableMouseSequences();
  keyboard->setTermcapMap (reinterpret_cast<fc::fkeymap*>(test::Fkey));
}

//----------------------------------------------------------------------
void FKeyboardTest::classNameTest()
{
  FKeyboard k;
  const char* const classname = k.getClassName();
  CPPUNIT_ASSERT ( std::strcmp(classname, "FKeyboard") == 0 );
}

//----------------------------------------------------------------------
void FKeyboardTest::noArgumentTest()
{
  CPPUNIT_ASSERT ( keyboard->getKey() == 0 );
  char* buffer = keyboard->getKeyBuffer();
  int size = keyboard->getKeyBufferSize();

  CPPUNIT_ASSERT ( size == 512 );
  CPPUNIT_ASSERT ( buffer[0] == 0 );
  int sum = 0;

  for (int i = 0; i < size; i++)
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
void FKeyboardTest::inputTest()
{
  std::cout << std::endl;
  input("\033[M Z2");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == fc::Fkey_mouse );
  clear();

  input("\033[<0;11;7M");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == fc::Fkey_extended_mouse );
  clear();

  input("\033[32;11;7M");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == fc::Fkey_urxvt_mouse );
  clear();

  input("\033O[");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == fc::Fkey_escape_mintty );
  clear();

  input("\033");
  processInput();
  usleep(100000);
  keyboard->escapeKeyHandling();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == fc::Fkey_escape );
  clear();

  input("\033[3~");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == fc::Fkey_ctab );
  clear();

  input("\033OB");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == fc::Fkey_down );
  clear();

  input("\033[B");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == fc::Fkey_down );
  clear();

  input("aA");
  processInput();
  std::cout << " - Key: " << keyboard->getKeyName(key_pressed) << std::endl;
  CPPUNIT_ASSERT ( key_pressed == 'A' );
  CPPUNIT_ASSERT ( key_released == 'A' );
  clear();

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
    ioctl (FTermios::getStdIn(), TIOCSTI, &c);
    ++iter;
  }

  ioctl (FTermios::getStdIn(), TIOCSTI, &EOT);
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
  key_pressed = key_released = 0;
}

//----------------------------------------------------------------------
void FKeyboardTest::keyPressed()
{
  key_pressed = keyboard->getKey();
}

//----------------------------------------------------------------------
void FKeyboardTest::keyReleased()
{
  key_released = keyboard->getKey();
}

//----------------------------------------------------------------------
void FKeyboardTest::escapeKeyPressed()
{
  key_pressed = fc::Fkey_escape;
  key_released = fc::Fkey_escape;
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FKeyboardTest);

// The general unit test main part
#include <main-test.inc>
