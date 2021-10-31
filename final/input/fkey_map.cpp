/***********************************************************************
* fkey_map.cpp - Key name mapping                                      *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2021 Markus Gans                                      *
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

#include <array>

#include "final/fc.h"
#include "final/input/fkey_map.h"

namespace finalcut
{

//----------------------------------------------------------------------
auto FKeyMap::getInstance() -> FKeyMap&
{
  static const auto& key_map = make_unique<FKeyMap>();
  return *key_map;
}

//----------------------------------------------------------------------
FKeyMap::KeyCapMapType& FKeyMap::getKeyCapMap()
{
  return fkey_cap_table;
}

//----------------------------------------------------------------------
FKeyMap::KeyMapType& FKeyMap::getKeyMap()
{
  return fkey_table;
}

//----------------------------------------------------------------------
const FKeyMap::KeyNameType& FKeyMap::getKeyName()
{
  return fkeyname;
}

//----------------------------------------------------------------------
FKeyMap::KeyCapMapType FKeyMap::fkey_cap_table
{{
  { FKey::Backspace       , nullptr,   0, "kb" },  // Backspace key
  { FKey::Clear_all_tabs  , nullptr,   0, "ka" },  // Clear-all-tabs key
  { FKey::Clear           , nullptr,   0, "kC" },  // Clear-screen or erase key
  { FKey::Clear_tab       , nullptr,   0, "kt" },  // Clear-tab key
  { FKey::Del_char        , nullptr,   0, "kD" },  // Delete-character key
  { FKey::Del_line        , nullptr,   0, "kL" },  // Delete-line key
  { FKey::Down            , nullptr,   0, "kd" },  // Down-arrow key
  { FKey::Exit_insert     , nullptr,   0, "kM" },  // Sent by rmir or smir in insert mode
  { FKey::Clear_eol       , nullptr,   0, "kE" },  // Clear-to-end-of-line key
  { FKey::Clear_eos       , nullptr,   0, "kS" },  // Clear-to-end-of-screen key
  { FKey::F0              , nullptr,   0, "k0" },  // F0 function key
  { FKey::F1              , nullptr,   0, "k1" },  // F1 function key
  { FKey::F2              , nullptr,   0, "k2" },  // F2 function key
  { FKey::F3              , nullptr,   0, "k3" },  // F3 function key
  { FKey::F4              , nullptr,   0, "k4" },  // F4 function key
  { FKey::F5              , nullptr,   0, "k5" },  // F5 function key
  { FKey::F6              , nullptr,   0, "k6" },  // F6 function key
  { FKey::F7              , nullptr,   0, "k7" },  // F7 function key
  { FKey::F8              , nullptr,   0, "k8" },  // F8 fucntion key
  { FKey::F9              , nullptr,   0, "k9" },  // F9 function key
  { FKey::F10             , nullptr,   0, "k;" },  // F10 function key
  { FKey::Home            , nullptr,   0, "kh" },  // Home key
  { FKey::Insert          , nullptr,   0, "kI" },  // Insert-character key
  { FKey::Insert_line     , nullptr,   0, "kA" },  // Insert-line key
  { FKey::Left            , nullptr,   0, "kl" },  // Left-arrow key
  { FKey::Home_down       , nullptr,   0, "kH" },  // Last-line key
  { FKey::Page_down       , nullptr,   0, "kN" },  // Page-down key
  { FKey::Page_up         , nullptr,   0, "kP" },  // Page-up key
  { FKey::Right           , nullptr,   0, "kr" },  // Right-arrow key
  { FKey::Scroll_forward  , nullptr,   0, "kF" },  // Scroll-forward key (shift-up)
  { FKey::Scroll_backward , nullptr,   0, "kR" },  // Scroll-backward key (shift-down)
  { FKey::Set_tab         , nullptr,   0, "kT" },  // Set-tab key
  { FKey::Up              , nullptr,   0, "ku" },  // Up-arrow key
  { FKey::Upper_left      , nullptr,   0, "K1" },  // Upper left of keypad
  { FKey::Upper_right     , nullptr,   0, "K3" },  // Upper right of keypad
  { FKey::Center          , nullptr,   0, "K2" },  // Center of keypad
  { FKey::Lower_left      , nullptr,   0, "K4" },  // Lower left of keypad
  { FKey::Lower_right     , nullptr,   0, "K5" },  // Lower right of keypad
  { FKey::Back_tab        , nullptr,   0, "kB" },  // Back-tab key
  { FKey::Begin           , nullptr,   0, "@1" },  // Begin key
  { FKey::Cancel          , nullptr,   0, "@2" },  // Cancel key
  { FKey::Close           , nullptr,   0, "@3" },  // Close key
  { FKey::Command         , nullptr,   0, "@4" },  // Command key
  { FKey::Copy            , nullptr,   0, "@5" },  // Copy key
  { FKey::Create          , nullptr,   0, "@6" },  // Create key
  { FKey::End             , nullptr,   0, "@7" },  // End key
  { FKey::Enter           , nullptr,   0, "@8" },  // Enter/send key
  { FKey::Exit            , nullptr,   0, "@9" },  // Exit key
  { FKey::Find            , nullptr,   0, "@0" },  // Find key
  { FKey::Help            , nullptr,   0, "%1" },  // Help key
  { FKey::Mark            , nullptr,   0, "%2" },  // Mark key
  { FKey::Message         , nullptr,   0, "%3" },  // Message key
  { FKey::Move            , nullptr,   0, "%4" },  // Move key
  { FKey::Next            , nullptr,   0, "%5" },  // Next key
  { FKey::Open            , nullptr,   0, "%6" },  // Open key
  { FKey::Options         , nullptr,   0, "%7" },  // Options key
  { FKey::Previous        , nullptr,   0, "%8" },  // Previous key
  { FKey::Print           , nullptr,   0, "%9" },  // Print key
  { FKey::Redo            , nullptr,   0, "%0" },  // Redo key
  { FKey::Reference       , nullptr,   0, "&1" },  // Reference key
  { FKey::Refresh         , nullptr,   0, "&2" },  // Refresh key
  { FKey::Replace         , nullptr,   0, "&3" },  // Replace key
  { FKey::Restart         , nullptr,   0, "&4" },  // Restart key
  { FKey::Resume          , nullptr,   0, "&5" },  // Resume key
  { FKey::Save            , nullptr,   0, "&6" },  // Save key
  { FKey::Suspend         , nullptr,   0, "&7" },  // Suspend key
  { FKey::Undo            , nullptr,   0, "&8" },  // Undo key
  { FKey::Shift_begin     , nullptr,   0, "&9" },  // Shifted begin key
  { FKey::Shift_cancel    , nullptr,   0, "&0" },  // Shifted cancel key
  { FKey::Shift_command   , nullptr,   0, "*1" },  // Shifted command key
  { FKey::Shift_copy      , nullptr,   0, "*2" },  // Shifted copy key
  { FKey::Shift_create    , nullptr,   0, "*3" },  // Shifted create key
  { FKey::Shift_del_char  , nullptr,   0, "*4" },  // Shifted delete-character key
  { FKey::Shift_dl        , nullptr,   0, "*5" },  // Shifted delete-line key
  { FKey::Select          , nullptr,   0, "*6" },  // Select key
  { FKey::Shift_end       , nullptr,   0, "*7" },  // Shifted End key
  { FKey::Shift_clear_eol , nullptr,   0, "*8" },  // Shifted Clear-to-end-of-line key
  { FKey::Shift_exit      , nullptr,   0, "*9" },  // Shifted Exit key
  { FKey::Shift_find      , nullptr,   0, "*0" },  // Shifted Find key
  { FKey::Shift_help      , nullptr,   0, "#1" },  // Shifted Help key
  { FKey::Shift_home      , nullptr,   0, "#2" },  // Shifted Home key
  { FKey::Shift_insert    , nullptr,   0, "#3" },  // Shifted Insert-character key
  { FKey::Shift_left      , nullptr,   0, "#4" },  // Shifted Left-arrow key
  { FKey::Shift_message   , nullptr,   0, "%a" },  // Shifted Message key
  { FKey::Shift_move      , nullptr,   0, "%b" },  // Shifted Move key
  { FKey::Shift_page_down , nullptr,   0, "%c" },  // Shifted Page-down key
  { FKey::Shift_options   , nullptr,   0, "%d" },  // Shifted Options key
  { FKey::Shift_page_up   , nullptr,   0, "%e" },  // Shifted Page-up key
  { FKey::Shift_print     , nullptr,   0, "%f" },  // Shifted Print key
  { FKey::Shift_redo      , nullptr,   0, "%g" },  // Shifted Redo key
  { FKey::Shift_replace   , nullptr,   0, "%h" },  // Shifted Replace key
  { FKey::Shift_right     , nullptr,   0, "%i" },  // Shifted Right-arrow key
  { FKey::Shift_rsume     , nullptr,   0, "%j" },  // Shifted Resume key
  { FKey::Shift_save      , nullptr,   0, "!1" },  // Shifted Save key
  { FKey::Shift_suspend   , nullptr,   0, "!2" },  // Shifted Suspend key
  { FKey::Shift_undo      , nullptr,   0, "!3" },  // Shifted Undo key
  { FKey::F11             , nullptr,   0, "F1" },  // F11 function key
  { FKey::F12             , nullptr,   0, "F2" },  // F12 function key
  { FKey::F13             , nullptr,   0, "F3" },  // F13 function key
  { FKey::F14             , nullptr,   0, "F4" },  // F14 function key
  { FKey::F15             , nullptr,   0, "F5" },  // F15 function key
  { FKey::F16             , nullptr,   0, "F6" },  // F16 function key
  { FKey::F17             , nullptr,   0, "F7" },  // F17 function key
  { FKey::F18             , nullptr,   0, "F8" },  // F18 function key
  { FKey::F19             , nullptr,   0, "F9" },  // F19 function key
  { FKey::F20             , nullptr,   0, "FA" },  // F20 function key
  { FKey::F21             , nullptr,   0, "FB" },  // F21 function key
  { FKey::F22             , nullptr,   0, "FC" },  // F22 function key
  { FKey::F23             , nullptr,   0, "FD" },  // F23 function key
  { FKey::F24             , nullptr,   0, "FE" },  // F24 function key
  { FKey::F25             , nullptr,   0, "FF" },  // F25 function key
  { FKey::F26             , nullptr,   0, "FG" },  // F26 function key
  { FKey::F27             , nullptr,   0, "FH" },  // F27 function key
  { FKey::F28             , nullptr,   0, "FI" },  // F28 function key
  { FKey::F29             , nullptr,   0, "FJ" },  // F29 function key
  { FKey::F30             , nullptr,   0, "FK" },  // F30 function key
  { FKey::F31             , nullptr,   0, "FL" },  // F31 function key
  { FKey::F32             , nullptr,   0, "FM" },  // F32 function key
  { FKey::F33             , nullptr,   0, "FN" },  // F33 function key
  { FKey::F34             , nullptr,   0, "FO" },  // F34 function key
  { FKey::F35             , nullptr,   0, "FP" },  // F35 function key
  { FKey::F36             , nullptr,   0, "FQ" },  // F36 function key
  { FKey::F37             , nullptr,   0, "FR" },  // F37 function key
  { FKey::F38             , nullptr,   0, "FS" },  // F38 function key
  { FKey::F39             , nullptr,   0, "FT" },  // F39 function key
  { FKey::F40             , nullptr,   0, "FU" },  // F40 function key
  { FKey::F41             , nullptr,   0, "FV" },  // F41 function key
  { FKey::F42             , nullptr,   0, "FW" },  // F42 function key
  { FKey::F43             , nullptr,   0, "FX" },  // F43 function key
  { FKey::F44             , nullptr,   0, "FY" },  // F44 function key
  { FKey::F45             , nullptr,   0, "FZ" },  // F45 function key
  { FKey::F46             , nullptr,   0, "Fa" },  // F46 function key
  { FKey::F47             , nullptr,   0, "Fb" },  // F47 function key
  { FKey::F48             , nullptr,   0, "Fc" },  // F48 function key
  { FKey::F49             , nullptr,   0, "Fd" },  // F49 function key
  { FKey::F50             , nullptr,   0, "Fe" },  // F50 function key
  { FKey::F51             , nullptr,   0, "Ff" },  // F51 function key
  { FKey::F52             , nullptr,   0, "Fg" },  // F52 function key
  { FKey::F53             , nullptr,   0, "Fh" },  // F53 function key
  { FKey::F54             , nullptr,   0, "Fi" },  // F54 function key
  { FKey::F55             , nullptr,   0, "Fj" },  // F55 function key
  { FKey::F56             , nullptr,   0, "Fk" },  // F56 function key
  { FKey::F57             , nullptr,   0, "Fl" },  // F57 function key
  { FKey::F58             , nullptr,   0, "Fm" },  // F58 function key
  { FKey::F59             , nullptr,   0, "Fn" },  // F59 function key
  { FKey::F60             , nullptr,   0, "Fo" },  // F60 function key
  { FKey::F61             , nullptr,   0, "Fp" },  // F61 function key
  { FKey::F62             , nullptr,   0, "Fq" },  // F62 function key
  { FKey::F63             , nullptr,   0, "Fr" },  // F63 function key
  // Some terminals (e.g. PuTTY) send vt100 key codes
  // when the arrow and function keys are pressed
  { FKey::F1              , ESC "OP",  3, "k1x"},  // PF1 (application mode)
  { FKey::F2              , ESC "OQ",  3, "k2x"},  // PF2 (application mode)
  { FKey::F3              , ESC "OR",  3, "k3x"},  // PF3 (application mode)
  { FKey::F4              , ESC "OS",  3, "k4x"},  // PF4 (application mode)
  { FKey::Left            , CSI "D",   3, "klx"},  // Left-arrow key (standard mode)
  { FKey::Left            , ESC "OD",  3, "klX"},  // Left-arrow key (application mode)
  { FKey::Right           , CSI "C",   3, "krx"},  // Right-arrow key (standard mode)
  { FKey::Right           , ESC "OC",  3, "krX"},  // Right-arrow key (application mode)
  { FKey::Up              , CSI "A",   3, "kux"},  // Up-arrow key (standard mode)
  { FKey::Up              , ESC "OA",  3, "kuX"},  // Up-arrow key (application mode)
  { FKey::Down            , CSI "B",   3, "kdx"},  // Down-arrow key (standard mode)
  { FKey::Down            , ESC "OB",  3, "kdX"},  // Down-arrow key (application mode)
  { FKey::Scroll_forward  , CSI "a",   3, "kFx"},  // Scroll-forward key (shift-up)
  { FKey::Scroll_backward , CSI "b",   3, "kRx"},  // Scroll-backward key (shift-down)
  // Fallback for rxvt with TERM=xterm
  { FKey::Home            , CSI "7~",  4, "khx"},  // Home key
  { FKey::End             , CSI "8~",  4, "@7x"},  // End key
  { FKey::F1              , CSI "11~", 5, "k1X"},  // F1 function key
  { FKey::F2              , CSI "12~", 5, "k2X"},  // F2 function key
  { FKey::F3              , CSI "13~", 5, "k3X"},  // F3 function key
  { FKey::F4              , CSI "14~", 5, "k4X"},  // F4 function key
  // Fallback for TERM=ansi
  { FKey::Home            , CSI "H",   3, "khX"},  // Home key
  { FKey::End             , CSI "F",   3, "@7X"},  // End key
  { FKey::End             , CSI "K",   3, "@7y"},  // End key (Microsoft HyperTerminal)
  // Keypad keys
  { FKey::Enter           , ESC "OM",  3, "@8x"},  // Enter key
  { FKey::Slash           , ESC "Oo",  3, "KP1"},  // Keypad Slash
  { FKey::Asterisk        , ESC "Oj",  3, "KP2"},  // Keypad Asterisk
  { FKey::Minus_sign      , ESC "Om",  3, "KP3"},  // Keypad Minus sign
  { FKey::Plus_sign       , ESC "Ok",  3, "KP4"},  // Keypad Plus sign
  { FKey::Insert          , ESC "Op",  3, "kIx"},  // Keypad Insert
  { FKey::Del_char        , ESC "On",  3, "kDx"},  // Keypad Delete
  { FKey::Left            , ESC "Ot",  3, "kly"},  // Keypad Left-arrow
  { FKey::Right           , ESC "Ov",  3, "kry"},  // Keypad Right-arrow
  { FKey::Up              , ESC "Ox",  3, "kuy"},  // Keypad Up-arrow
  { FKey::Down            , ESC "Or",  3, "kdy"},  // Keypad Down-arrow
  { FKey::Upper_left      , ESC "Ow",  3, "K1x"},  // Keypad Upper left
  { FKey::Upper_right     , ESC "Oy",  3, "K3x"},  // Keypad Upper right
  { FKey::Center          , ESC "Ou",  3, "K2x"},  // Keypad Center
  { FKey::Lower_left      , ESC "Oq",  3, "K4x"},  // Keypad Lower left
  { FKey::Lower_right     , ESC "Os",  3, "K5x"}   // Keypad Lower right
}};

//----------------------------------------------------------------------
FKeyMap::KeyMapType FKeyMap::fkey_table =
{{
  { FKey::Meta_insert               , "\033[2;3~"   , 6},  // M-Insert
  { FKey::Meta_insert               , "\033\033[2~" , 5},  // M-Insert
  { FKey::Meta_del_char             , "\033[3;3~"   , 6},  // M-Delete
  { FKey::Meta_del_char             , "\033\033[3~" , 5},  // M-Delete
  { FKey::Meta_home                 , "\033[1;3H"   , 6},  // M-Home
  { FKey::Meta_home                 , "\033\033[1~" , 5},  // M-Home
  { FKey::Meta_end                  , "\033[1;3F"   , 6},  // M-End
  { FKey::Meta_end                  , "\033\033[4~" , 5},  // M-End
  { FKey::Meta_page_up              , "\033[5;3~"   , 6},  // M-Page-up
  { FKey::Meta_page_up              , "\033\033[5~" , 5},  // M-Page-up
  { FKey::Meta_page_down            , "\033[6;3~"   , 6},  // M-Page-down
  { FKey::Meta_page_down            , "\033\033[6~" , 5},  // M-Page-down
  { FKey::Meta_f1                   , "\033[1;3P"   , 6},  // M-F1
  { FKey::Meta_f1                   , "\033\033[11~", 6},  // M-F1
  { FKey::Meta_f2                   , "\033[1;3Q"   , 6},  // M-F2
  { FKey::Meta_f2                   , "\033\033[12~", 6},  // M-F2
  { FKey::Meta_f3                   , "\033[1;3R"   , 6},  // M-F3
  { FKey::Meta_f3                   , "\033\033[13~", 6},  // M-F3
  { FKey::Meta_f4                   , "\033[1;3S"   , 6},  // M-F4
  { FKey::Meta_f4                   , "\033\033[14~", 6},  // M-F4
  { FKey::Meta_f5                   , "\033[15;3~"  , 7},  // M-F5
  { FKey::Meta_f5                   , "\033\033[15~", 6},  // M-F5
  { FKey::Meta_f6                   , "\033[17;3~"  , 7},  // M-F6
  { FKey::Meta_f6                   , "\033\033[17~", 6},  // M-F6
  { FKey::Meta_f7                   , "\033[18;3~"  , 7},  // M-F7
  { FKey::Meta_f7                   , "\033\033[18~", 6},  // M-F7
  { FKey::Meta_f8                   , "\033[19;3~"  , 7},  // M-F8
  { FKey::Meta_f8                   , "\033\033[19~", 6},  // M-F8
  { FKey::Meta_f9                   , "\033[20;3~"  , 7},  // M-F9
  { FKey::Meta_f9                   , "\033\033[20~", 6},  // M-F9
  { FKey::Meta_f10                  , "\033[21;3~"  , 7},  // M-F10
  { FKey::Meta_f10                  , "\033\033[21~", 6},  // M-F10
  { FKey::Meta_f11                  , "\033[23;3~"  , 7},  // M-F11
  { FKey::Meta_f11                  , "\033\033[23~", 6},  // M-F11
  { FKey::Meta_f12                  , "\033[24;3~"  , 7},  // M-F12
  { FKey::Meta_f12                  , "\033\033[24~", 6},  // M-F12
  { FKey::Meta_up                   , "\033[1;3A"   , 6},  // M-Up
  { FKey::Meta_up                   , "\033\033[A"  , 4},  // M-Up
  { FKey::Meta_down                 , "\033[1;3B"   , 6},  // M-Down
  { FKey::Meta_down                 , "\033\033[B"  , 4},  // M-Down
  { FKey::Meta_right                , "\033[1;3C"   , 6},  // M-Right
  { FKey::Meta_right                , "\033\033[C"  , 4},  // M-Right
  { FKey::Meta_left                 , "\033[1;3D"   , 6},  // M-Left
  { FKey::Meta_left                 , "\033\033[D"  , 4},  // M-Left
  { FKey::Shift_Meta_insert         , "\033[2;4~"   , 6},  // Shift-M-Insert
  { FKey::Shift_Meta_del_char       , "\033[3;4~"   , 6},  // Shift-M-Delete
  { FKey::Shift_Meta_home           , "\033[1;4H"   , 6},  // Shift-M-Home
  { FKey::Shift_Meta_end            , "\033[1;4F"   , 6},  // Shift-M-End
  { FKey::Shift_Meta_page_up        , "\033[5;4~"   , 6},  // Shift-M-Page-up
  { FKey::Shift_Meta_page_down      , "\033[6;4~"   , 6},  // Shift-M-Page-down
  { FKey::Shift_Meta_f1             , "\033[1;4P"   , 6},  // Shift-M-F1
  { FKey::Shift_Meta_f2             , "\033[1;4Q"   , 6},  // Shift-M-F2
  { FKey::Shift_Meta_f3             , "\033[1;4R"   , 6},  // Shift-M-F3
  { FKey::Shift_Meta_f4             , "\033[1;4S"   , 6},  // Shift-M-F4
  { FKey::Shift_Meta_f5             , "\033[15;4~"  , 7},  // Shift-M-F5
  { FKey::Shift_Meta_f6             , "\033[17;4~"  , 7},  // Shift-M-F6
  { FKey::Shift_Meta_f7             , "\033[18;4~"  , 7},  // Shift-M-F7
  { FKey::Shift_Meta_f8             , "\033[19;4~"  , 7},  // Shift-M-F8
  { FKey::Shift_Meta_f9             , "\033[20;4~"  , 7},  // Shift-M-F9
  { FKey::Shift_Meta_f10            , "\033[21;4~"  , 7},  // Shift-M-F10
  { FKey::Shift_Meta_f11            , "\033[23;4~"  , 7},  // Shift-M-F11
  { FKey::Shift_Meta_f12            , "\033[24;4~"  , 7},  // Shift-M-F12
  { FKey::Shift_Meta_up             , "\033[1;4A"   , 6},  // Shift-M-Up
  { FKey::Shift_Meta_down           , "\033[1;4B"   , 6},  // Shift-M-Down
  { FKey::Shift_Meta_right          , "\033[1;4C"   , 6},  // Shift-M-Right
  { FKey::Shift_Meta_left           , "\033[1;4D"   , 6},  // Shift-M-Left
  { FKey::Ctrl_insert               , "\033[2;5~"   , 6},  // Ctrl-Insert
  { FKey::Ctrl_del_char             , "\033[3;5~"   , 6},  // Ctrl-Delete
  { FKey::Ctrl_home                 , "\033[1;5H"   , 6},  // Ctrl-Home
  { FKey::Ctrl_end                  , "\033[1;5F"   , 6},  // Ctrl-End
  { FKey::Ctrl_page_up              , "\033[5;5~"   , 6},  // Ctrl-Page-up
  { FKey::Ctrl_page_down            , "\033[6;5~"   , 6},  // Ctrl-Page-down
  { FKey::Ctrl_up                   , "\033[1;5A"   , 6},  // Ctrl-Up
  { FKey::Ctrl_up                   , "\033Oa"      , 3},  // Ctrl-Up
  { FKey::Ctrl_down                 , "\033[1;5B"   , 6},  // Ctrl-Down
  { FKey::Ctrl_down                 , "\033Ob"      , 3},  // Ctrl-Down
  { FKey::Ctrl_right                , "\033[1;5C"   , 6},  // Ctrl-Right
  { FKey::Ctrl_right                , "\033Oc"      , 3},  // Ctrl-Right
  { FKey::Ctrl_left                 , "\033[1;5D"   , 6},  // Ctrl-Left
  { FKey::Ctrl_left                 , "\033Od"      , 3},  // Ctrl-Left
  { FKey::Shift_Ctrl_insert         , "\033[2;6~"   , 6},  // Shift-Ctrl-Insert
  { FKey::Shift_Ctrl_del_char       , "\033[3;6~"   , 6},  // Shift-Ctrl-Delete
  { FKey::Shift_Ctrl_home           , "\033[1;6H"   , 6},  // Shift-Ctrl-Home
  { FKey::Shift_Ctrl_end            , "\033[1;6F"   , 6},  // Shift-Ctrl-End
  { FKey::Shift_Ctrl_page_up        , "\033[5;6~"   , 6},  // Shift-Ctrl-Page-up
  { FKey::Shift_Ctrl_page_down      , "\033[6;6~"   , 6},  // Shift-Ctrl-Page-down
  { FKey::Shift_Ctrl_up             , "\033[1;6A"   , 6},  // Shift-Ctrl-Up
  { FKey::Shift_Ctrl_down           , "\033[1;6B"   , 6},  // Shift-Ctrl-Down
  { FKey::Shift_Ctrl_right          , "\033[1;6C"   , 6},  // Shift-Ctrl-Right
  { FKey::Shift_Ctrl_left           , "\033[1;6D"   , 6},  // Shift-Ctrl-Left
  { FKey::Ctrl_Meta_insert          , "\033[2;7~"   , 6},  // Ctrl-M-Insert
  { FKey::Ctrl_Meta_del_char        , "\033[3;7~"   , 6},  // Ctrl-M-Delete
  { FKey::Ctrl_Meta_home            , "\033[1;7H"   , 6},  // Ctrl-M-Home
  { FKey::Ctrl_Meta_end             , "\033[1;7F"   , 6},  // Ctrl-M-End
  { FKey::Ctrl_Meta_page_up         , "\033[5;7~"   , 6},  // Ctrl-M-Page-up
  { FKey::Ctrl_Meta_page_down       , "\033[6;7~"   , 6},  // Ctrl-M-Page-down
  { FKey::Ctrl_Meta_up              , "\033[1;7A"   , 6},  // Ctrl-M-Up
  { FKey::Ctrl_Meta_down            , "\033[1;7B"   , 6},  // Ctrl-M-Down
  { FKey::Ctrl_Meta_right           , "\033[1;7C"   , 6},  // Ctrl-M-Right
  { FKey::Ctrl_Meta_left            , "\033[1;7D"   , 6},  // Ctrl-M-Left
  { FKey::Shift_Ctrl_Meta_insert    , "\033[2;8~"   , 6},  // Shift-Ctrl-M-Insert
  { FKey::Shift_Ctrl_Meta_del_char  , "\033[3;8~"   , 6},  // Shift-Ctrl-M-Delete
  { FKey::Shift_Ctrl_Meta_home      , "\033[1;8H"   , 6},  // Shift-Ctrl-M-Home
  { FKey::Shift_Ctrl_Meta_end       , "\033[1;8F"   , 6},  // Shift-Ctrl-M-End
  { FKey::Shift_Ctrl_Meta_page_up   , "\033[5;8~"   , 6},  // Shift-Ctrl-M-Page-up
  { FKey::Shift_Ctrl_Meta_page_down , "\033[6;8~"   , 6},  // Shift-Ctrl-M-Page-down
  { FKey::Shift_Ctrl_Meta_f1        , "\033[1;8P"   , 6},  // Shift-Ctrl-M-F1
  { FKey::Shift_Ctrl_Meta_f2        , "\033[1;8Q"   , 6},  // Shift-Ctrl-M-F2
  { FKey::Shift_Ctrl_Meta_f3        , "\033[1;8R"   , 6},  // Shift-Ctrl-M-F3
  { FKey::Shift_Ctrl_Meta_f4        , "\033[1;8S"   , 6},  // Shift-Ctrl-M-F4
  { FKey::Shift_Ctrl_Meta_f5        , "\033[15;8~"  , 7},  // Shift-Ctrl-M-F5
  { FKey::Shift_Ctrl_Meta_f6        , "\033[17;8~"  , 7},  // Shift-Ctrl-M-F6
  { FKey::Shift_Ctrl_Meta_f7        , "\033[18;8~"  , 7},  // Shift-Ctrl-M-F7
  { FKey::Shift_Ctrl_Meta_f8        , "\033[19;8~"  , 7},  // Shift-Ctrl-M-F8
  { FKey::Shift_Ctrl_Meta_f9        , "\033[20;8~"  , 7},  // Shift-Ctrl-M-F9
  { FKey::Shift_Ctrl_Meta_f10       , "\033[21;8~"  , 7},  // Shift-Ctrl-M-F10
  { FKey::Shift_Ctrl_Meta_f11       , "\033[23;8~"  , 7},  // Shift-Ctrl-M-F11
  { FKey::Shift_Ctrl_Meta_f12       , "\033[24;8~"  , 7},  // Shift-Ctrl-M-F12
  { FKey::Shift_Ctrl_Meta_up        , "\033[1;8A"   , 6},  // Shift-Ctrl-M-Up
  { FKey::Shift_Ctrl_Meta_down      , "\033[1;8B"   , 6},  // Shift-Ctrl-M-Down
  { FKey::Shift_Ctrl_Meta_right     , "\033[1;8C"   , 6},  // Shift-Ctrl-M-Right
  { FKey::Shift_Ctrl_Meta_left      , "\033[1;8D"   , 6},  // Shift-Ctrl-M-Left
  { FKey::Menu                      , "\033[29~"    , 5},  // Menu
  { FKey::Shift_menu                , "\033[29$"    , 5},  // Shift-Menu
  { FKey::Shift_menu                , "\033[29;2~"  , 7},  // Shift-Menu
  { FKey::Ctrl_menu                 , "\033[29^"    , 5},  // Ctrl-Menu
  { FKey::Ctrl_menu                 , "\033[29;5~"  , 7},  // Ctrl-Menu
  { FKey::Shift_Ctrl_menu           , "\033[29@"    , 5},  // Shift-Ctrl-Menu
  { FKey::Shift_Ctrl_menu           , "\033[29;6~"  , 7},  // Shift-Ctrl-Menu
  { FKey::Meta_menu                 , "\033[29;3~"  , 7},  // M-Menu
  { FKey::Shift_Meta_menu           , "\033[29;4~"  , 7},  // Shift-M-Menu
  { FKey::Ctrl_Meta_menu            , "\033[29;7~"  , 7},  // Ctrl-M-Menu
  { FKey::Shift_Ctrl_Meta_menu      , "\033[29;8~"  , 7},  // Shift-Ctrl-M-Menu
  { FKey::Escape_mintty             , "\033O[", 3},  // Mintty Esc
  { FKey::Meta_tab                  , "\033\t", 2},  // M-Tab
  { FKey::Meta_enter                , "\033\n", 2},  // M-Enter
  { FKey::Meta_enter                , "\033\r", 2},  // M-Enter
  { FKey::Meta_space                , "\033 " , 2},  // M-' '
  { FKey::Meta_bang                 , "\033!" , 2},  // M-!
  { FKey::Meta_quotes               , "\033\"", 2},  // M-"
  { FKey::Meta_hash                 , "\033#" , 2},  // M-#
  { FKey::Meta_dollar               , "\033$" , 2},  // M-$
  { FKey::Meta_percent              , "\033%" , 2},  // M-%
  { FKey::Meta_ampersand            , "\033&" , 2},  // M-&
  { FKey::Meta_apostrophe           , "\033'" , 2},  // M-'
  { FKey::Meta_left_parenthesis     , "\033(" , 2},  // M-(
  { FKey::Meta_right_parenthesis    , "\033)" , 2},  // M-)
  { FKey::Meta_asterisk             , "\033*" , 2},  // M-*
  { FKey::Meta_plus                 , "\033+" , 2},  // M-+
  { FKey::Meta_comma                , "\033," , 2},  // M-,
  { FKey::Meta_minus                , "\033-" , 2},  // M-'-'
  { FKey::Meta_full_stop            , "\033." , 2},  // M-.
  { FKey::Meta_slash                , "\033/" , 2},  // M-/
  { FKey::Meta_0                    , "\0330" , 2},  // M-0
  { FKey::Meta_1                    , "\0331" , 2},  // M-1
  { FKey::Meta_2                    , "\0332" , 2},  // M-2
  { FKey::Meta_3                    , "\0333" , 2},  // M-3
  { FKey::Meta_4                    , "\0334" , 2},  // M-4
  { FKey::Meta_5                    , "\0335" , 2},  // M-5
  { FKey::Meta_6                    , "\0336" , 2},  // M-6
  { FKey::Meta_7                    , "\0337" , 2},  // M-7
  { FKey::Meta_8                    , "\0338" , 2},  // M-8
  { FKey::Meta_9                    , "\0339" , 2},  // M-9
  { FKey::Meta_colon                , "\033:" , 2},  // M-:
  { FKey::Meta_semicolon            , "\033;" , 2},  // M-;
  { FKey::Meta_less_than            , "\033<" , 2},  // M-<
  { FKey::Meta_equals               , "\033=" , 2},  // M-=
  { FKey::Meta_greater_than         , "\033>" , 2},  // M->
  { FKey::Meta_question_mark        , "\033?" , 2},  // M-?
  { FKey::Meta_at                   , "\033@" , 2},  // M-@
  { FKey::Meta_A                    , "\033A" , 2},  // Shift-M-A
  { FKey::Meta_B                    , "\033B" , 2},  // Shift-M-B
  { FKey::Meta_C                    , "\033C" , 2},  // Shift-M-C
  { FKey::Meta_D                    , "\033D" , 2},  // Shift-M-D
  { FKey::Meta_E                    , "\033E" , 2},  // Shift-M-E
  { FKey::Meta_F                    , "\033F" , 2},  // Shift-M-F
  { FKey::Meta_G                    , "\033G" , 2},  // Shift-M-G
  { FKey::Meta_H                    , "\033H" , 2},  // Shift-M-H
  { FKey::Meta_I                    , "\033I" , 2},  // Shift-M-I
  { FKey::Meta_J                    , "\033J" , 2},  // Shift-M-J
  { FKey::Meta_K                    , "\033K" , 2},  // Shift-M-K
  { FKey::Meta_L                    , "\033L" , 2},  // Shift-M-L
  { FKey::Meta_M                    , "\033M" , 2},  // Shift-M-M
  { FKey::Meta_N                    , "\033N" , 2},  // Shift-M-N
  { FKey::Meta_O                    , "\033O" , 2},  // Shift-M-O
  { FKey::Meta_P                    , "\033P" , 2},  // Shift-M-P
  { FKey::Meta_Q                    , "\033Q" , 2},  // Shift-M-Q
  { FKey::Meta_R                    , "\033R" , 2},  // Shift-M-R
  { FKey::Meta_S                    , "\033S" , 2},  // Shift-M-S
  { FKey::Meta_T                    , "\033T" , 2},  // Shift-M-T
  { FKey::Meta_U                    , "\033U" , 2},  // Shift-M-U
  { FKey::Meta_V                    , "\033V" , 2},  // Shift-M-V
  { FKey::Meta_W                    , "\033W" , 2},  // Shift-M-W
  { FKey::Meta_X                    , "\033X" , 2},  // Shift-M-X
  { FKey::Meta_Y                    , "\033Y" , 2},  // Shift-M-Y
  { FKey::Meta_Z                    , "\033Z" , 2},  // Shift-M-Z
  { FKey::Meta_left_square_bracket  , "\033[" , 2},  // M-[
  { FKey::Meta_backslash            , "\033\\", 2},  // M-'\'
  { FKey::Meta_right_square_bracket , "\033]" , 2},  // M-]
  { FKey::Meta_caret                , "\033^" , 2},  // M-^
  { FKey::Meta_underscore           , "\033_" , 2},  // M-_
  { FKey::Meta_grave_accent         , "\033`" , 2},  // M-`
  { FKey::Meta_a                    , "\033a" , 2},  // M-a
  { FKey::Meta_b                    , "\033b" , 2},  // M-b
  { FKey::Meta_c                    , "\033c" , 2},  // M-c
  { FKey::Meta_d                    , "\033d" , 2},  // M-d
  { FKey::Meta_e                    , "\033e" , 2},  // M-e
  { FKey::Meta_f                    , "\033f" , 2},  // M-f
  { FKey::Meta_g                    , "\033g" , 2},  // M-g
  { FKey::Meta_h                    , "\033h" , 2},  // M-h
  { FKey::Meta_i                    , "\033i" , 2},  // M-i
  { FKey::Meta_j                    , "\033j" , 2},  // M-j
  { FKey::Meta_k                    , "\033k" , 2},  // M-k
  { FKey::Meta_l                    , "\033l" , 2},  // M-l
  { FKey::Meta_m                    , "\033m" , 2},  // M-m
  { FKey::Meta_n                    , "\033n" , 2},  // M-n
  { FKey::Meta_o                    , "\033o" , 2},  // M-o
  { FKey::Meta_p                    , "\033p" , 2},  // M-p
  { FKey::Meta_q                    , "\033q" , 2},  // M-q
  { FKey::Meta_r                    , "\033r" , 2},  // M-r
  { FKey::Meta_s                    , "\033s" , 2},  // M-s
  { FKey::Meta_t                    , "\033t" , 2},  // M-t
  { FKey::Meta_u                    , "\033u" , 2},  // M-u
  { FKey::Meta_v                    , "\033v" , 2},  // M-v
  { FKey::Meta_w                    , "\033w" , 2},  // M-w
  { FKey::Meta_x                    , "\033x" , 2},  // M-x
  { FKey::Meta_y                    , "\033y" , 2},  // M-y
  { FKey::Meta_z                    , "\033z" , 2},  // M-z
  { FKey::Meta_left_curly_bracket   , "\033{" , 2},  // M-{
  { FKey::Meta_vertical_bar         , "\033|" , 2},  // M-|
  { FKey::Meta_right_curly_bracket  , "\033}" , 2},  // M-}
  { FKey::Meta_tilde                , "\033~" , 2}   // M-~
}};

//----------------------------------------------------------------------
constexpr FKeyMap::KeyNameType FKeyMap::fkeyname =
{{
  { FKey::Ctrl_a                    , "Ctrl+A" },
  { FKey::Ctrl_b                    , "Ctrl+B" },
  { FKey::Ctrl_c                    , "Ctrl+C" },
  { FKey::Ctrl_d                    , "Ctrl+D" },
  { FKey::Ctrl_e                    , "Ctrl+E" },
  { FKey::Ctrl_f                    , "Ctrl+F" },
  { FKey::Ctrl_g                    , "Ctrl+G" },
  { FKey::Erase                     , "Backspace" },  // Ctrl+H
  { FKey::Tab                       , "Tab" },        // Ctrl+I
  { FKey::Ctrl_j                    , "Ctrl+J" },
  { FKey::Ctrl_h                    , "Ctrl+K" },
  { FKey::Ctrl_l                    , "Ctrl+L" },
  { FKey::Return                    , "Return" },     // Ctrl+M
  { FKey::Ctrl_n                    , "Ctrl+N" },
  { FKey::Ctrl_o                    , "Ctrl+O" },
  { FKey::Ctrl_p                    , "Ctrl+P" },
  { FKey::Ctrl_q                    , "Ctrl+Q" },
  { FKey::Ctrl_r                    , "Ctrl+R" },
  { FKey::Ctrl_s                    , "Ctrl+S" },
  { FKey::Ctrl_t                    , "Ctrl+T" },
  { FKey::Ctrl_u                    , "Ctrl+U" },
  { FKey::Ctrl_v                    , "Ctrl+V" },
  { FKey::Ctrl_w                    , "Ctrl+W" },
  { FKey::Ctrl_x                    , "Ctrl+X" },
  { FKey::Ctrl_y                    , "Ctrl+Y" },
  { FKey::Ctrl_z                    , "Ctrl+Z" },
  { FKey::Escape                    , "Esc" },        // Ctrl+[
  { FKey::Escape_mintty             , "Esc" },
  { FKey::Ctrl_backslash            , "Ctrl+\\" },
  { FKey::Ctrl_right_square_bracket , "Ctrl+]" },
  { FKey::Ctrl_caret                , "Ctrl+^" },
  { FKey::Ctrl_underscore           , "Ctrl+_" },
  { FKey::Space                     , "Space" },
  { FKey::Ctrl_space                , "Ctrl+Space" },  // Ctrl+(Space or @)
  { FKey::Backspace                 , "Backspace" },
  { FKey::Clear_all_tabs            , "Clear-All-Tabs" },
  { FKey::Clear                     , "Clear-Screen" },
  { FKey::Clear_tab                 , "Clear-Tab" },
  { FKey::Del_char                  , "Del" },
  { FKey::Del_line                  , "Del-line" },
  { FKey::Down                      , "Down" },
  { FKey::Exit_insert               , "Exit-Ins" },
  { FKey::Clear_eol                 , "Clear-End-of-Line" },
  { FKey::Clear_eos                 , "Clear-End-of-Screen" },
  { FKey::F0                        , "F0" },
  { FKey::F1                        , "F1" },
  { FKey::F2                        , "F2" },
  { FKey::F3                        , "F3" },
  { FKey::F4                        , "F4" },
  { FKey::F5                        , "F5" },
  { FKey::F6                        , "F6" },
  { FKey::F7                        , "F7" },
  { FKey::F8                        , "F8" },
  { FKey::F9                        , "F9" },
  { FKey::F10                       , "F10" },
  { FKey::Home                      , "Home" },
  { FKey::Insert                    , "Ins" },
  { FKey::Insert_line               , "Ins-Line" },
  { FKey::Left                      , "Left" },
  { FKey::Home_down                 , "Home-Down" },
  { FKey::Page_down                 , "Page-Down" },
  { FKey::Page_up                   , "Page-Up" },
  { FKey::Right                     , "Right" },
  { FKey::Scroll_forward            , "Scroll-Forward" },
  { FKey::Scroll_backward           , "Scroll-Backward" },
  { FKey::Set_tab                   , "Set-Tab" },
  { FKey::Up                        , "Up" },
  { FKey::Upper_left                , "Upper-Left" },
  { FKey::Upper_right               , "Upper-Right" },
  { FKey::Center                    , "Center" },
  { FKey::Lower_left                , "Lower-Left" },
  { FKey::Lower_right               , "Lower-Right" },
  { FKey::Back_tab                  , "Shift+Tab" },
  { FKey::Begin                     , "Begin" },
  { FKey::Cancel                    , "Cancel" },
  { FKey::Close                     , "Close" },
  { FKey::Command                   , "Command" },
  { FKey::Copy                      , "Copy" },
  { FKey::Create                    , "Create" },
  { FKey::End                       , "End" },
  { FKey::Enter                     , "Enter" },
  { FKey::Exit                      , "Exit" },
  { FKey::Find                      , "Find" },
  { FKey::Help                      , "Help" },
  { FKey::Mark                      , "Mark" },
  { FKey::Message                   , "Message" },
  { FKey::Move                      , "Move" },
  { FKey::Next                      , "Next" },
  { FKey::Open                      , "Open" },
  { FKey::Options                   , "Options" },
  { FKey::Previous                  , "Previous" },
  { FKey::Print                     , "Print" },
  { FKey::Redo                      , "Redo" },
  { FKey::Reference                 , "Reference" },
  { FKey::Refresh                   , "Refresh" },
  { FKey::Replace                   , "Replace" },
  { FKey::Restart                   , "Restart" },
  { FKey::Resume                    , "Resume" },
  { FKey::Save                      , "Save" },
  { FKey::Suspend                   , "Suspend" },
  { FKey::Undo                      , "Undo" },
  { FKey::Shift_begin               , "Shift+Begin" },
  { FKey::Shift_cancel              , "Shift+Cancel" },
  { FKey::Shift_command             , "Shift+Command" },
  { FKey::Shift_copy                , "Shift+Copy" },
  { FKey::Shift_create              , "Shift+Create" },
  { FKey::Shift_del_char            , "Shift+Del" },
  { FKey::Shift_dl                  , "Shift+Del-line" },
  { FKey::Select                    , "Select" },
  { FKey::Shift_end                 , "Shift+End" },
  { FKey::Shift_clear_eol           , "Shift+Clear-End-of-Line" },
  { FKey::Shift_exit                , "Shift+Exit" },
  { FKey::Shift_find                , "Shift+Find" },
  { FKey::Shift_help                , "Shift+Help" },
  { FKey::Shift_home                , "Shift+Home" },
  { FKey::Shift_insert              , "Shift+Ins" },
  { FKey::Shift_left                , "Shift+Left" },
  { FKey::Shift_message             , "Shift+Message" },
  { FKey::Shift_move                , "Shift+Move" },
  { FKey::Shift_page_down           , "Shift+Page-Down" },
  { FKey::Shift_options             , "Shift+Options" },
  { FKey::Shift_page_up             , "Shift+Page-Up" },
  { FKey::Shift_print               , "Shift+Print" },
  { FKey::Shift_redo                , "Shift+Redo" },
  { FKey::Shift_replace             , "Shift+Replace" },
  { FKey::Shift_right               , "Shift+Right" },
  { FKey::Shift_rsume               , "Shift+Resume" },
  { FKey::Shift_save                , "Shift+Save" },
  { FKey::Shift_suspend             , "Shift+Suspend" },
  { FKey::Shift_undo                , "Shift+Undo" },
  { FKey::F11                       , "F11" },
  { FKey::F12                       , "F12" },
  { FKey::F13                       , "Shift+F1" },
  { FKey::F14                       , "Shift+F2" },
  { FKey::F15                       , "Shift+F3" },
  { FKey::F16                       , "Shift+F4" },
  { FKey::F17                       , "Shift+F5" },
  { FKey::F18                       , "Shift+F6" },
  { FKey::F19                       , "Shift+F7" },
  { FKey::F20                       , "Shift+F8" },
  { FKey::F21                       , "Shift+F9" },
  { FKey::F22                       , "Shift+F10" },
  { FKey::F23                       , "Shift+F11" },
  { FKey::F24                       , "Shift+F12" },
  { FKey::F25                       , "Ctrl+F1" },
  { FKey::F26                       , "Ctrl+F2" },
  { FKey::F27                       , "Ctrl+F3" },
  { FKey::F28                       , "Ctrl+F4" },
  { FKey::F29                       , "Ctrl+F5" },
  { FKey::F30                       , "Ctrl+F6" },
  { FKey::F31                       , "Ctrl+F7" },
  { FKey::F32                       , "Ctrl+F8" },
  { FKey::F33                       , "Ctrl+F9" },
  { FKey::F34                       , "Ctrl+F10" },
  { FKey::F35                       , "Ctrl+F11" },
  { FKey::F36                       , "Ctrl+F12" },
  { FKey::F37                       , "Shift+Ctrl+F1" },
  { FKey::F38                       , "Shift+Ctrl+F2" },
  { FKey::F39                       , "Shift+Ctrl+F3" },
  { FKey::F40                       , "Shift+Ctrl+F4" },
  { FKey::F41                       , "Shift+Ctrl+F5" },
  { FKey::F42                       , "Shift+Ctrl+F6" },
  { FKey::F43                       , "Shift+Ctrl+F7" },
  { FKey::F44                       , "Shift+Ctrl+F8" },
  { FKey::F45                       , "Shift+Ctrl+F9" },
  { FKey::F46                       , "Shift+Ctrl+F10" },
  { FKey::F47                       , "Shift+Ctrl+F11" },
  { FKey::F48                       , "Shift+Ctrl+F12" },
  { FKey::F49                       , "Meta+F1" },
  { FKey::F50                       , "Meta+F2" },
  { FKey::F51                       , "Meta+F3" },
  { FKey::F52                       , "Meta+F4" },
  { FKey::F53                       , "Meta+F5" },
  { FKey::F54                       , "Meta+F6" },
  { FKey::F55                       , "Meta+F7" },
  { FKey::F56                       , "Meta+F8" },
  { FKey::F57                       , "Meta+F9" },
  { FKey::F58                       , "Meta+F10" },
  { FKey::F59                       , "Meta+F11" },
  { FKey::F60                       , "Meta+F12" },
  { FKey::F61                       , "Shift+Meta+F1" },
  { FKey::F62                       , "Shift+Meta+F2" },
  { FKey::F63                       , "Shift+Meta+F3" },
  { FKey::Meta_insert               , "Meta+Ins" },
  { FKey::Meta_del_char             , "Meta+Del" },
  { FKey::Meta_home                 , "Meta+Home" },
  { FKey::Meta_end                  , "Meta+End" },
  { FKey::Meta_page_up              , "Meta+Page-Up" },
  { FKey::Meta_page_down            , "Meta+Page-Down" },
  { FKey::Meta_f1                   , "Meta+F1" },
  { FKey::Meta_f2                   , "Meta+F2" },
  { FKey::Meta_f3                   , "Meta+F3" },
  { FKey::Meta_f4                   , "Meta+F4" },
  { FKey::Meta_f5                   , "Meta+F5" },
  { FKey::Meta_f6                   , "Meta+F6" },
  { FKey::Meta_f7                   , "Meta+F7" },
  { FKey::Meta_f8                   , "Meta+F8" },
  { FKey::Meta_f9                   , "Meta+F9" },
  { FKey::Meta_f10                  , "Meta+F10" },
  { FKey::Meta_f11                  , "Meta+F11" },
  { FKey::Meta_f12                  , "Meta+F12" },
  { FKey::Meta_up                   , "Meta+Up" },
  { FKey::Meta_down                 , "Meta+Down" },
  { FKey::Meta_right                , "Meta+Right" },
  { FKey::Meta_left                 , "Meta+Left" },
  { FKey::Shift_Meta_insert         , "Shift+Meta+Ins" },
  { FKey::Shift_Meta_del_char       , "Shift+Meta+Del" },
  { FKey::Shift_Meta_home           , "Shift+Meta+Home" },
  { FKey::Shift_Meta_end            , "Shift+Meta+End" },
  { FKey::Shift_Meta_page_up        , "Shift+Meta+Page-Up" },
  { FKey::Shift_Meta_page_down      , "Shift+Meta+Page-Down" },
  { FKey::Shift_Meta_f1             , "Shift+Meta+F1" },
  { FKey::Shift_Meta_f2             , "Shift+Meta+F2" },
  { FKey::Shift_Meta_f3             , "Shift+Meta+F3" },
  { FKey::Shift_Meta_f4             , "Shift+Meta+F4" },
  { FKey::Shift_Meta_f5             , "Shift+Meta+F5" },
  { FKey::Shift_Meta_f6             , "Shift+Meta+F6" },
  { FKey::Shift_Meta_f7             , "Shift+Meta+F7" },
  { FKey::Shift_Meta_f8             , "Shift+Meta+F8" },
  { FKey::Shift_Meta_f9             , "Shift+Meta+F9" },
  { FKey::Shift_Meta_f10            , "Shift+Meta+F10" },
  { FKey::Shift_Meta_f11            , "Shift+Meta+F11" },
  { FKey::Shift_Meta_f12            , "Shift+Meta+F12" },
  { FKey::Shift_Meta_up             , "Shift+Meta+Up" },
  { FKey::Shift_Meta_down           , "Shift+Meta+Down" },
  { FKey::Shift_Meta_right          , "Shift+Meta+Right" },
  { FKey::Shift_Meta_left           , "Shift+Meta+Left" },
  { FKey::Ctrl_insert               , "Ctrl+Ins" },
  { FKey::Ctrl_del_char             , "Ctrl+Del" },
  { FKey::Ctrl_home                 , "Ctrl+Home" },
  { FKey::Ctrl_end                  , "Ctrl+End" },
  { FKey::Ctrl_page_up              , "Ctrl+PgUp" },
  { FKey::Ctrl_page_down            , "Ctrl+PgDn" },
  { FKey::Ctrl_up                   , "Ctrl+Up" },
  { FKey::Ctrl_down                 , "Ctrl+Down" },
  { FKey::Ctrl_right                , "Ctrl+Right" },
  { FKey::Ctrl_left                 , "Ctrl+Left" },
  { FKey::Shift_Ctrl_insert         , "Shift+Ctrl+Ins" },
  { FKey::Shift_Ctrl_del_char       , "Shift+Ctrl+Del" },
  { FKey::Shift_Ctrl_home           , "Shift+Ctrl+Home" },
  { FKey::Shift_Ctrl_end            , "Shift+Ctrl+End" },
  { FKey::Shift_Ctrl_page_up        , "Shift+Ctrl+Page-Up" },
  { FKey::Shift_Ctrl_page_down      , "Shift+Ctrl+Page-Down" },
  { FKey::Shift_Ctrl_up             , "Shift+Ctrl+Up" },
  { FKey::Shift_Ctrl_down           , "Shift+Ctrl+Down" },
  { FKey::Shift_Ctrl_right          , "Shift+Ctrl+Right" },
  { FKey::Shift_Ctrl_left           , "Shift+Ctrl+Left" },
  { FKey::Ctrl_Meta_insert          , "Ctrl+Meta+Ins" },
  { FKey::Ctrl_Meta_del_char        , "Ctrl+Meta+Del" },
  { FKey::Ctrl_Meta_home            , "Ctrl+Meta+Home" },
  { FKey::Ctrl_Meta_end             , "Ctrl+Meta+End" },
  { FKey::Ctrl_Meta_page_up         , "Ctrl+Meta+Page-Up" },
  { FKey::Ctrl_Meta_page_down       , "Ctrl+Meta+Page-Down" },
  { FKey::Ctrl_Meta_up              , "Ctrl+Meta+Up" },
  { FKey::Ctrl_Meta_down            , "Ctrl+Meta+Down" },
  { FKey::Ctrl_Meta_right           , "Ctrl+Meta+Right" },
  { FKey::Ctrl_Meta_left            , "Ctrl+Meta+Left" },
  { FKey::Shift_Ctrl_Meta_insert    , "Shift+Ctrl+Meta+Ins" },
  { FKey::Shift_Ctrl_Meta_del_char  , "Shift+Ctrl+Meta+Del" },
  { FKey::Shift_Ctrl_Meta_home      , "Shift+Ctrl+Meta+Home" },
  { FKey::Shift_Ctrl_Meta_end       , "Shift+Ctrl+Meta+End" },
  { FKey::Shift_Ctrl_Meta_page_up   , "Shift+Ctrl+Meta+Page-Up" },
  { FKey::Shift_Ctrl_Meta_page_down , "Shift+Ctrl+Meta+Page-Down" },
  { FKey::Shift_Ctrl_Meta_f1        , "Shift+Ctrl+Meta+F1" },
  { FKey::Shift_Ctrl_Meta_f2        , "Shift+Ctrl+Meta+F2" },
  { FKey::Shift_Ctrl_Meta_f3        , "Shift+Ctrl+Meta+F3" },
  { FKey::Shift_Ctrl_Meta_f4        , "Shift+Ctrl+Meta+F4" },
  { FKey::Shift_Ctrl_Meta_f5        , "Shift+Ctrl+Meta+F5" },
  { FKey::Shift_Ctrl_Meta_f6        , "Shift+Ctrl+Meta+F6" },
  { FKey::Shift_Ctrl_Meta_f7        , "Shift+Ctrl+Meta+F7" },
  { FKey::Shift_Ctrl_Meta_f8        , "Shift+Ctrl+Meta+F8" },
  { FKey::Shift_Ctrl_Meta_f9        , "Shift+Ctrl+Meta+F9" },
  { FKey::Shift_Ctrl_Meta_f10       , "Shift+Ctrl+Meta+F10" },
  { FKey::Shift_Ctrl_Meta_f11       , "Shift+Ctrl+Meta+F11" },
  { FKey::Shift_Ctrl_Meta_f12       , "Shift+Ctrl+Meta+F12" },
  { FKey::Shift_Ctrl_Meta_up        , "Shift+Ctrl+Meta+Up" },
  { FKey::Shift_Ctrl_Meta_down      , "Shift+Ctrl+Meta+Down" },
  { FKey::Shift_Ctrl_Meta_right     , "Shift+Ctrl+Meta+Right" },
  { FKey::Shift_Ctrl_Meta_left      , "Shift+Ctrl+Meta+Left" },
  { FKey::Menu                      , "Menu" },
  { FKey::Shift_menu                , "Shift+Menu" },
  { FKey::Ctrl_menu                 , "Ctrl+Menu" },
  { FKey::Shift_Ctrl_menu           , "Shift+Ctrl+Menu" },
  { FKey::Meta_menu                 , "Meta+Menu" },
  { FKey::Shift_Meta_menu           , "Shift+Meta+Menu" },
  { FKey::Ctrl_Meta_menu            , "Ctrl+Meta+Menu" },
  { FKey::Shift_Ctrl_Meta_menu      , "Shift+Ctrl+Meta+Menu" },
  { FKey::Meta_tab                  , "Meta+Tab" },
  { FKey::Meta_enter                , "Meta+Enter" },
  { FKey::Meta_space                , "Meta+Space" },
  { FKey::Meta_bang                 , "Meta+!" },
  { FKey::Meta_quotes               , "Meta+\"" },
  { FKey::Meta_hash                 , "Meta+#" },
  { FKey::Meta_dollar               , "Meta+$" },
  { FKey::Meta_percent              , "Meta+%" },
  { FKey::Meta_ampersand            , "Meta+&" },
  { FKey::Meta_apostrophe           , "Meta+'" },
  { FKey::Meta_left_parenthesis     , "Meta+(" },
  { FKey::Meta_right_parenthesis    , "Meta+)" },
  { FKey::Meta_asterisk             , "Meta+*" },
  { FKey::Meta_plus                 , "Meta++" },
  { FKey::Meta_comma                , "Meta+," },
  { FKey::Meta_minus                , "Meta+-" },
  { FKey::Meta_full_stop            , "Meta+." },
  { FKey::Meta_slash                , "Meta+/" },
  { FKey::Meta_0                    , "Meta+0" },
  { FKey::Meta_1                    , "Meta+1" },
  { FKey::Meta_2                    , "Meta+2" },
  { FKey::Meta_3                    , "Meta+3" },
  { FKey::Meta_4                    , "Meta+4" },
  { FKey::Meta_5                    , "Meta+5" },
  { FKey::Meta_6                    , "Meta+6" },
  { FKey::Meta_7                    , "Meta+7" },
  { FKey::Meta_8                    , "Meta+8" },
  { FKey::Meta_9                    , "Meta+9" },
  { FKey::Meta_colon                , "Meta+:" },
  { FKey::Meta_semicolon            , "Meta+;" },
  { FKey::Meta_less_than            , "Meta+<" },
  { FKey::Meta_equals               , "Meta+=" },
  { FKey::Meta_greater_than         , "Meta+>" },
  { FKey::Meta_question_mark        , "Meta+?" },
  { FKey::Meta_at                   , "Meta+@" },
  { FKey::Meta_A                    , "Shift+Meta+A" },
  { FKey::Meta_B                    , "Shift+Meta+B" },
  { FKey::Meta_C                    , "Shift+Meta+C" },
  { FKey::Meta_D                    , "Shift+Meta+D" },
  { FKey::Meta_E                    , "Shift+Meta+E" },
  { FKey::Meta_F                    , "Shift+Meta+F" },
  { FKey::Meta_G                    , "Shift+Meta+G" },
  { FKey::Meta_H                    , "Shift+Meta+H" },
  { FKey::Meta_I                    , "Shift+Meta+I" },
  { FKey::Meta_J                    , "Shift+Meta+J" },
  { FKey::Meta_K                    , "Shift+Meta+K" },
  { FKey::Meta_L                    , "Shift+Meta+L" },
  { FKey::Meta_M                    , "Shift+Meta+M" },
  { FKey::Meta_N                    , "Shift+Meta+N" },
  { FKey::Meta_O                    , "Shift+Meta+O" },
  { FKey::Meta_P                    , "Shift+Meta+P" },
  { FKey::Meta_Q                    , "Shift+Meta+Q" },
  { FKey::Meta_R                    , "Shift+Meta+R" },
  { FKey::Meta_S                    , "Shift+Meta+S" },
  { FKey::Meta_T                    , "Shift+Meta+T" },
  { FKey::Meta_U                    , "Shift+Meta+U" },
  { FKey::Meta_V                    , "Shift+Meta+V" },
  { FKey::Meta_W                    , "Shift+Meta+W" },
  { FKey::Meta_X                    , "Shift+Meta+X" },
  { FKey::Meta_Y                    , "Shift+Meta+Y" },
  { FKey::Meta_Z                    , "Shift+Meta+Z" },
  { FKey::Meta_left_square_bracket  , "Meta+[" },
  { FKey::Meta_backslash            , "Meta+\\" },
  { FKey::Meta_right_square_bracket , "Meta+]" },
  { FKey::Meta_caret                , "Meta+^" },
  { FKey::Meta_underscore           , "Meta+_" },
  { FKey::Meta_grave_accent         , "Meta+`" },
  { FKey::Meta_a                    , "Meta+A" },
  { FKey::Meta_b                    , "Meta+B" },
  { FKey::Meta_c                    , "Meta+C" },
  { FKey::Meta_d                    , "Meta+D" },
  { FKey::Meta_e                    , "Meta+E" },
  { FKey::Meta_f                    , "Meta+F" },
  { FKey::Meta_g                    , "Meta+G" },
  { FKey::Meta_h                    , "Meta+H" },
  { FKey::Meta_i                    , "Meta+I" },
  { FKey::Meta_j                    , "Meta+J" },
  { FKey::Meta_k                    , "Meta+K" },
  { FKey::Meta_l                    , "Meta+L" },
  { FKey::Meta_m                    , "Meta+M" },
  { FKey::Meta_n                    , "Meta+N" },
  { FKey::Meta_o                    , "Meta+O" },
  { FKey::Meta_p                    , "Meta+P" },
  { FKey::Meta_q                    , "Meta+Q" },
  { FKey::Meta_r                    , "Meta+R" },
  { FKey::Meta_s                    , "Meta+S" },
  { FKey::Meta_t                    , "Meta+T" },
  { FKey::Meta_u                    , "Meta+U" },
  { FKey::Meta_v                    , "Meta+V" },
  { FKey::Meta_w                    , "Meta+W" },
  { FKey::Meta_x                    , "Meta+X" },
  { FKey::Meta_y                    , "Meta+Y" },
  { FKey::Meta_z                    , "Meta+Z" },
  { FKey::Meta_left_curly_bracket   , "Meta+{" },
  { FKey::Meta_vertical_bar         , "Meta+|" },
  { FKey::Meta_right_curly_bracket  , "Meta+}" },
  { FKey::Meta_tilde                , "Meta+~" },
  { FKey::X11mouse                  , "xterm mouse" },
  { FKey::Extended_mouse            , "SGR extended mouse" },
  { FKey::Urxvt_mouse               , "urxvt mouse extension" },
  { FKey::Incomplete                , "incomplete key string" }
}};

}  // namespace finalcut
