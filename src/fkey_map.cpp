/***********************************************************************
* fkey_map.cpp - Key name mapping                                      *
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

#include <final/fc.h>
#include <final/fkey_map.h>

namespace finalcut
{

namespace fc
{

fkeymap Fkey[] =
{
  { fc::Fkey_backspace , 0, "kb" },  // backspace key
  { fc::Fkey_catab     , 0, "ka" },  // clear-all-tabs key
  { fc::Fkey_clear     , 0, "kC" },  // clear-screen or erase key
  { fc::Fkey_ctab      , 0, "kt" },  // clear-tab key
  { fc::Fkey_dc        , 0, "kD" },  // delete-character key
  { fc::Fkey_dc        , 0, "kDx"},  // keypad delete
  { fc::Fkey_dl        , 0, "kL" },  // delete-line key
  { fc::Fkey_down      , 0, "kd" },  // down-arrow key
  { fc::Fkey_down      , 0, "kdx"},  // down-arrow key
  { fc::Fkey_eic       , 0, "kM" },  // sent by rmir or smir in insert mode
  { fc::Fkey_eol       , 0, "kE" },  // clear-to-end-of-line key
  { fc::Fkey_eos       , 0, "kS" },  // clear-to-end-of-screen key
  { fc::Fkey_f0        , 0, "k0" },  // F0 function key
  { fc::Fkey_f1        , 0, "k1" },  // F1 function key
  { fc::Fkey_f1        , 0, "k1x"},  // F1 function key
  { fc::Fkey_f1        , 0, "k1X"},  // F1 function key
  { fc::Fkey_f2        , 0, "k2" },  // F2 function key
  { fc::Fkey_f2        , 0, "k2x"},  // F2 function key
  { fc::Fkey_f2        , 0, "k2X"},  // F2 function key
  { fc::Fkey_f3        , 0, "k3" },  // F3 function key
  { fc::Fkey_f3        , 0, "k3x"},  // F3 function key
  { fc::Fkey_f3        , 0, "k3X"},  // F3 function key
  { fc::Fkey_f4        , 0, "k4" },  // F4 function key
  { fc::Fkey_f4        , 0, "k4x"},  // F4 function key
  { fc::Fkey_f4        , 0, "k4X"},  // F4 function key
  { fc::Fkey_f5        , 0, "k5" },  // F5 function key
  { fc::Fkey_f6        , 0, "k6" },  // F6 function key
  { fc::Fkey_f7        , 0, "k7" },  // F7 function key
  { fc::Fkey_f8        , 0, "k8" },  // F8 fucntion key
  { fc::Fkey_f9        , 0, "k9" },  // F9 function key
  { fc::Fkey_f10       , 0, "k;" },  // F10 function key
  { fc::Fkey_home      , 0, "kh" },  // home key
  { fc::Fkey_home      , 0, "khx"},  // home key
  { fc::Fkey_ic        , 0, "kI" },  // insert-character key
  { fc::Fkey_il        , 0, "kA" },  // insert-line key
  { fc::Fkey_left      , 0, "kl" },  // left-arrow key
  { fc::Fkey_left      , 0, "klx"},  // left-arrow key
  { fc::Fkey_ll        , 0, "kH" },  // last-line key
  { fc::Fkey_npage     , 0, "kN" },  // next-page key
  { fc::Fkey_ppage     , 0, "kP" },  // prev-page key
  { fc::Fkey_right     , 0, "kr" },  // right-arrow key
  { fc::Fkey_right     , 0, "krx"},  // right-arrow key
  { fc::Fkey_sf        , 0, "kF" },  // scroll-forward key (shift-up)
  { fc::Fkey_sr        , 0, "kR" },  // scroll-backward key (shift-down)
  { fc::Fkey_stab      , 0, "kT" },  // set-tab key
  { fc::Fkey_up        , 0, "ku" },  // up-arrow key
  { fc::Fkey_up        , 0, "kux"},  // up-arrow key
  { fc::Fkey_a1        , 0, "K1" },  // upper left of keypad
  { fc::Fkey_a3        , 0, "K3" },  // upper right of keypad
  { fc::Fkey_b2        , 0, "K2" },  // center of keypad
  { fc::Fkey_c1        , 0, "K4" },  // lower left of keypad
  { fc::Fkey_c3        , 0, "K5" },  // lower right of keypad
  { fc::Fkey_btab      , 0, "kB" },  // back-tab key
  { fc::Fkey_beg       , 0, "@1" },  // begin key
  { fc::Fkey_cancel    , 0, "@2" },  // cancel key
  { fc::Fkey_close     , 0, "@3" },  // close key
  { fc::Fkey_command   , 0, "@4" },  // command key
  { fc::Fkey_copy      , 0, "@5" },  // copy key
  { fc::Fkey_create    , 0, "@6" },  // create key
  { fc::Fkey_end       , 0, "@7" },  // end key
  { fc::Fkey_end       , 0, "@7x"},  // end key
  { fc::Fkey_end       , 0, "@7X"},  // end key
  { fc::Fkey_enter     , 0, "@8" },  // enter/send key
  { fc::Fkey_enter     , 0, "@8x"},  // enter/send key
  { fc::Fkey_exit      , 0, "@9" },  // exit key
  { fc::Fkey_find      , 0, "@0" },  // find key
  { fc::Fkey_slash     , 0, "KP1"},  // keypad slash
  { fc::Fkey_asterisk  , 0, "KP2"},  // keypad asterisk
  { fc::Fkey_minus_sign, 0, "KP3"},  // keypad minus sign
  { fc::Fkey_plus_sign , 0, "KP4"},  // keypad plus sign
  { fc::Fkey_help      , 0, "%1" },  // help key
  { fc::Fkey_mark      , 0, "%2" },  // mark key
  { fc::Fkey_message   , 0, "%3" },  // message key
  { fc::Fkey_move      , 0, "%4" },  // move key
  { fc::Fkey_next      , 0, "%5" },  // next key
  { fc::Fkey_open      , 0, "%6" },  // open key
  { fc::Fkey_options   , 0, "%7" },  // options key
  { fc::Fkey_previous  , 0, "%8" },  // previous key
  { fc::Fkey_print     , 0, "%9" },  // print key
  { fc::Fkey_redo      , 0, "%0" },  // redo key
  { fc::Fkey_reference , 0, "&1" },  // reference key
  { fc::Fkey_refresh   , 0, "&2" },  // refresh key
  { fc::Fkey_replace   , 0, "&3" },  // replace key
  { fc::Fkey_restart   , 0, "&4" },  // restart key
  { fc::Fkey_resume    , 0, "&5" },  // resume key
  { fc::Fkey_save      , 0, "&6" },  // save key
  { fc::Fkey_suspend   , 0, "&7" },  // suspend key
  { fc::Fkey_undo      , 0, "&8" },  // undo key
  { fc::Fkey_sbeg      , 0, "&9" },  // shifted begin key
  { fc::Fkey_scancel   , 0, "&0" },  // shifted cancel key
  { fc::Fkey_scommand  , 0, "*1" },  // shifted command key
  { fc::Fkey_scopy     , 0, "*2" },  // shifted copy key
  { fc::Fkey_screate   , 0, "*3" },  // shifted create key
  { fc::Fkey_sdc       , 0, "*4" },  // shifted delete-character key
  { fc::Fkey_sdl       , 0, "*5" },  // shifted delete-line key
  { fc::Fkey_select    , 0, "*6" },  // select key
  { fc::Fkey_send      , 0, "*7" },  // shifted end key
  { fc::Fkey_seol      , 0, "*8" },  // shifted clear-to-end-of-line key
  { fc::Fkey_sexit     , 0, "*9" },  // shifted exit key
  { fc::Fkey_sfind     , 0, "*0" },  // shifted find key
  { fc::Fkey_shelp     , 0, "#1" },  // shifted help key
  { fc::Fkey_shome     , 0, "#2" },  // shifted home key
  { fc::Fkey_sic       , 0, "#3" },  // shifted insert-character key
  { fc::Fkey_sleft     , 0, "#4" },  // shifted left-arrow key
  { fc::Fkey_smessage  , 0, "%a" },  // shifted message key
  { fc::Fkey_smove     , 0, "%b" },  // shifted move key
  { fc::Fkey_snext     , 0, "%c" },  // shifted next key
  { fc::Fkey_soptions  , 0, "%d" },  // shifted options key
  { fc::Fkey_sprevious , 0, "%e" },  // shifted previous key
  { fc::Fkey_sprint    , 0, "%f" },  // shifted print key
  { fc::Fkey_sredo     , 0, "%g" },  // shifted redo key
  { fc::Fkey_sreplace  , 0, "%h" },  // shifted replace key
  { fc::Fkey_sright    , 0, "%i" },  // shifted right-arrow key
  { fc::Fkey_srsume    , 0, "%j" },  // shifted resume key
  { fc::Fkey_ssave     , 0, "!1" },  // shifted save key
  { fc::Fkey_ssuspend  , 0, "!2" },  // shifted suspend key
  { fc::Fkey_sundo     , 0, "!3" },  // shifted undo key
  { fc::Fkey_f11       , 0, "F1" },  // F11 function key
  { fc::Fkey_f12       , 0, "F2" },  // F12 function key
  { fc::Fkey_f13       , 0, "F3" },  // F13 function key
  { fc::Fkey_f14       , 0, "F4" },  // F14 function key
  { fc::Fkey_f15       , 0, "F5" },  // F15 function key
  { fc::Fkey_f16       , 0, "F6" },  // F16 function key
  { fc::Fkey_f17       , 0, "F7" },  // F17 function key
  { fc::Fkey_f18       , 0, "F8" },  // F18 function key
  { fc::Fkey_f19       , 0, "F9" },  // F19 function key
  { fc::Fkey_f20       , 0, "FA" },  // F20 function key
  { fc::Fkey_f21       , 0, "FB" },  // F21 function key
  { fc::Fkey_f22       , 0, "FC" },  // F22 function key
  { fc::Fkey_f23       , 0, "FD" },  // F23 function key
  { fc::Fkey_f24       , 0, "FE" },  // F24 function key
  { fc::Fkey_f25       , 0, "FF" },  // F25 function key
  { fc::Fkey_f26       , 0, "FG" },  // F26 function key
  { fc::Fkey_f27       , 0, "FH" },  // F27 function key
  { fc::Fkey_f28       , 0, "FI" },  // F28 function key
  { fc::Fkey_f29       , 0, "FJ" },  // F29 function key
  { fc::Fkey_f30       , 0, "FK" },  // F30 function key
  { fc::Fkey_f31       , 0, "FL" },  // F31 function key
  { fc::Fkey_f32       , 0, "FM" },  // F32 function key
  { fc::Fkey_f33       , 0, "FN" },  // F33 function key
  { fc::Fkey_f34       , 0, "FO" },  // F34 function key
  { fc::Fkey_f35       , 0, "FP" },  // F35 function key
  { fc::Fkey_f36       , 0, "FQ" },  // F36 function key
  { fc::Fkey_f37       , 0, "FR" },  // F37 function key
  { fc::Fkey_f38       , 0, "FS" },  // F38 function key
  { fc::Fkey_f39       , 0, "FT" },  // F39 function key
  { fc::Fkey_f40       , 0, "FU" },  // F40 function key
  { fc::Fkey_f41       , 0, "FV" },  // F41 function key
  { fc::Fkey_f42       , 0, "FW" },  // F42 function key
  { fc::Fkey_f43       , 0, "FX" },  // F43 function key
  { fc::Fkey_f44       , 0, "FY" },  // F44 function key
  { fc::Fkey_f45       , 0, "FZ" },  // F45 function key
  { fc::Fkey_f46       , 0, "Fa" },  // F46 function key
  { fc::Fkey_f47       , 0, "Fb" },  // F47 function key
  { fc::Fkey_f48       , 0, "Fc" },  // F48 function key
  { fc::Fkey_f49       , 0, "Fd" },  // F49 function key
  { fc::Fkey_f50       , 0, "Fe" },  // F50 function key
  { fc::Fkey_f51       , 0, "Ff" },  // F51 function key
  { fc::Fkey_f52       , 0, "Fg" },  // F52 function key
  { fc::Fkey_f53       , 0, "Fh" },  // F53 function key
  { fc::Fkey_f54       , 0, "Fi" },  // F54 function key
  { fc::Fkey_f55       , 0, "Fj" },  // F55 function key
  { fc::Fkey_f56       , 0, "Fk" },  // F56 function key
  { fc::Fkey_f57       , 0, "Fl" },  // F57 function key
  { fc::Fkey_f58       , 0, "Fm" },  // F58 function key
  { fc::Fkey_f59       , 0, "Fn" },  // F59 function key
  { fc::Fkey_f60       , 0, "Fo" },  // F60 function key
  { fc::Fkey_f61       , 0, "Fp" },  // F61 function key
  { fc::Fkey_f62       , 0, "Fq" },  // F62 function key
  { fc::Fkey_f63       , 0, "Fr" },  // F63 function key
  { 0                  , 0, "\0" }
};

fmetakeymap Fmetakey[] =
{
  { fc::Fmkey_ic                   , "\033[2;3~"   },  // M-insert
  { fc::Fmkey_ic                   , "\033\033[2~" },  // M-insert
  { fc::Fmkey_dc                   , "\033[3;3~"   },  // M-delete
  { fc::Fmkey_dc                   , "\033\033[3~" },  // M-delete
  { fc::Fmkey_home                 , "\033[1;3H"   },  // M-home
  { fc::Fmkey_home                 , "\033\033[1~" },  // M-home
  { fc::Fmkey_end                  , "\033[1;3F"   },  // M-end
  { fc::Fmkey_end                  , "\033\033[4~" },  // M-end
  { fc::Fmkey_ppage                , "\033[5;3~"   },  // M-prev-page
  { fc::Fmkey_ppage                , "\033\033[5~" },  // M-prev-page
  { fc::Fmkey_npage                , "\033[6;3~"   },  // M-next-page
  { fc::Fmkey_npage                , "\033\033[6~" },  // M-next-page
  { fc::Fmkey_f1                   , "\033[1;3P"   },  // M-f1
  { fc::Fmkey_f1                   , "\033\033[11~"},  // M-f1
  { fc::Fmkey_f2                   , "\033[1;3Q"   },  // M-f2
  { fc::Fmkey_f2                   , "\033\033[12~"},  // M-f2
  { fc::Fmkey_f3                   , "\033[1;3R"   },  // M-f3
  { fc::Fmkey_f3                   , "\033\033[13~"},  // M-f3
  { fc::Fmkey_f4                   , "\033[1;3S"   },  // M-f4
  { fc::Fmkey_f4                   , "\033\033[14~"},  // M-f4
  { fc::Fmkey_f5                   , "\033[15;3~"  },  // M-f5
  { fc::Fmkey_f5                   , "\033\033[15~"},  // M-f5
  { fc::Fmkey_f6                   , "\033[17;3~"  },  // M-f6
  { fc::Fmkey_f6                   , "\033\033[17~"},  // M-f6
  { fc::Fmkey_f7                   , "\033[18;3~"  },  // M-f7
  { fc::Fmkey_f7                   , "\033\033[18~"},  // M-f7
  { fc::Fmkey_f8                   , "\033[19;3~"  },  // M-f8
  { fc::Fmkey_f8                   , "\033\033[19~"},  // M-f8
  { fc::Fmkey_f9                   , "\033[20;3~"  },  // M-f9
  { fc::Fmkey_f9                   , "\033\033[20~"},  // M-f9
  { fc::Fmkey_f10                  , "\033[21;3~"  },  // M-f10
  { fc::Fmkey_f10                  , "\033\033[21~"},  // M-f10
  { fc::Fmkey_f11                  , "\033[23;3~"  },  // M-f11
  { fc::Fmkey_f11                  , "\033\033[23~"},  // M-f11
  { fc::Fmkey_f12                  , "\033[24;3~"  },  // M-f12
  { fc::Fmkey_f12                  , "\033\033[24~"},  // M-f12
  { fc::Fmkey_up                   , "\033[1;3A"   },  // M-up
  { fc::Fmkey_up                   , "\033\033[A"  },  // M-up
  { fc::Fmkey_down                 , "\033[1;3B"   },  // M-down
  { fc::Fmkey_down                 , "\033\033[B"  },  // M-down
  { fc::Fmkey_right                , "\033[1;3C"   },  // M-right
  { fc::Fmkey_right                , "\033\033[C"  },  // M-right
  { fc::Fmkey_left                 , "\033[1;3D"   },  // M-left
  { fc::Fmkey_left                 , "\033\033[D"  },  // M-left
  { fc::Fmkey_sic                  , "\033[2;4~"   },  // shift-M-insert
  { fc::Fmkey_sdc                  , "\033[3;4~"   },  // shift-M-delete
  { fc::Fmkey_shome                , "\033[1;4H"   },  // shift-M-home
  { fc::Fmkey_send                 , "\033[1;4F"   },  // shift-M-end
  { fc::Fmkey_sppage               , "\033[5;4~"   },  // shift-M-prev-page
  { fc::Fmkey_snpage               , "\033[6;4~"   },  // shift-M-next-page
  { fc::Fmkey_sf1                  , "\033[1;4P"   },  // shift-M-f1
  { fc::Fmkey_sf2                  , "\033[1;4Q"   },  // shift-M-f2
  { fc::Fmkey_sf3                  , "\033[1;4R"   },  // shift-M-f3
  { fc::Fmkey_sf4                  , "\033[1;4S"   },  // shift-M-f4
  { fc::Fmkey_sf5                  , "\033[15;4~"  },  // shift-M-f5
  { fc::Fmkey_sf6                  , "\033[17;4~"  },  // shift-M-f6
  { fc::Fmkey_sf7                  , "\033[18;4~"  },  // shift-M-f7
  { fc::Fmkey_sf8                  , "\033[19;4~"  },  // shift-M-f8
  { fc::Fmkey_sf9                  , "\033[20;4~"  },  // shift-M-f9
  { fc::Fmkey_sf10                 , "\033[21;4~"  },  // shift-M-f10
  { fc::Fmkey_sf11                 , "\033[23;4~"  },  // shift-M-f11
  { fc::Fmkey_sf12                 , "\033[24;4~"  },  // shift-M-f12
  { fc::Fmkey_sup                  , "\033[1;4A"   },  // shift-M-up
  { fc::Fmkey_sdown                , "\033[1;4B"   },  // shift-M-down
  { fc::Fmkey_sright               , "\033[1;4C"   },  // shift-M-right
  { fc::Fmkey_sleft                , "\033[1;4D"   },  // shift-M-left
  { fc::Fckey_ic                   , "\033[2;5~"   },  // ctrl-insert
  { fc::Fckey_dc                   , "\033[3;5~"   },  // ctrl-delete
  { fc::Fckey_home                 , "\033[1;5H"   },  // ctrl-home
  { fc::Fckey_end                  , "\033[1;5F"   },  // ctrl-end
  { fc::Fckey_ppage                , "\033[5;5~"   },  // ctrl-prev-page
  { fc::Fckey_npage                , "\033[6;5~"   },  // ctrl-next-page
  { fc::Fckey_up                   , "\033[1;5A"   },  // ctrl-up
  { fc::Fckey_down                 , "\033[1;5B"   },  // ctrl-down
  { fc::Fckey_right                , "\033[1;5C"   },  // ctrl-right
  { fc::Fckey_left                 , "\033[1;5D"   },  // ctrl-left
  { fc::Fckey_sic                  , "\033[2;6~"   },  // shift-ctrl-M-insert
  { fc::Fckey_sdc                  , "\033[3;6~"   },  // shift-ctrl-M-delete
  { fc::Fckey_shome                , "\033[1;6H"   },  // shift-ctrl-M-home
  { fc::Fckey_send                 , "\033[1;6F"   },  // shift-ctrl-M-end
  { fc::Fckey_sppage               , "\033[5;6~"   },  // shift-ctrl-M-prev-page
  { fc::Fckey_snpage               , "\033[6;6~"   },  // shift-ctrl-M-next-page
  { fc::Fckey_sup                  , "\033[1;6A"   },  // shift-ctrl-M-up
  { fc::Fckey_sdown                , "\033[1;6B"   },  // shift-ctrl-M-down
  { fc::Fckey_sright               , "\033[1;6C"   },  // shift-ctrl-M-right
  { fc::Fckey_sleft                , "\033[1;6D"   },  // shift-ctrl-M-left
  { fc::Fcmkey_ic                  , "\033[2;7~"   },  // ctrl-M-insert
  { fc::Fcmkey_dc                  , "\033[3;7~"   },  // ctrl-M-delete
  { fc::Fcmkey_home                , "\033[1;7H"   },  // ctrl-M-home
  { fc::Fcmkey_end                 , "\033[1;7F"   },  // ctrl-M-end
  { fc::Fcmkey_ppage               , "\033[5;7~"   },  // ctrl-M-prev-page
  { fc::Fcmkey_npage               , "\033[6;7~"   },  // ctrl-M-next-page
  { fc::Fcmkey_up                  , "\033[1;7A"   },  // ctrl-M-up
  { fc::Fcmkey_down                , "\033[1;7B"   },  // ctrl-M-down
  { fc::Fcmkey_right               , "\033[1;7C"   },  // ctrl-M-right
  { fc::Fcmkey_left                , "\033[1;7D"   },  // ctrl-M-left
  { fc::Fcmkey_sic                 , "\033[2;8~"   },  // shift-ctrl-M-insert
  { fc::Fcmkey_sdc                 , "\033[3;8~"   },  // shift-ctrl-M-delete
  { fc::Fcmkey_shome               , "\033[1;8H"   },  // shift-ctrl-M-home
  { fc::Fcmkey_send                , "\033[1;8F"   },  // shift-ctrl-M-end
  { fc::Fcmkey_sppage              , "\033[5;8~"   },  // shift-ctrl-M-prev-page
  { fc::Fcmkey_snpage              , "\033[6;8~"   },  // shift-ctrl-M-next-page
  { fc::Fcmkey_sf1                 , "\033[1;8P"   },  // shift-ctrl-M-f1
  { fc::Fcmkey_sf2                 , "\033[1;8Q"   },  // shift-ctrl-M-f2
  { fc::Fcmkey_sf3                 , "\033[1;8R"   },  // shift-ctrl-M-f3
  { fc::Fcmkey_sf4                 , "\033[1;8S"   },  // shift-ctrl-M-f4
  { fc::Fcmkey_sf5                 , "\033[15;8~"  },  // shift-ctrl-M-f5
  { fc::Fcmkey_sf6                 , "\033[17;8~"  },  // shift-ctrl-M-f6
  { fc::Fcmkey_sf7                 , "\033[18;8~"  },  // shift-ctrl-M-f7
  { fc::Fcmkey_sf8                 , "\033[19;8~"  },  // shift-ctrl-M-f8
  { fc::Fcmkey_sf9                 , "\033[20;8~"  },  // shift-ctrl-M-f9
  { fc::Fcmkey_sf10                , "\033[21;8~"  },  // shift-ctrl-M-f10
  { fc::Fcmkey_sf11                , "\033[23;8~"  },  // shift-ctrl-M-f11
  { fc::Fcmkey_sf12                , "\033[24;8~"  },  // shift-ctrl-M-f12
  { fc::Fcmkey_sup                 , "\033[1;8A"   },  // shift-ctrl-M-up
  { fc::Fcmkey_sdown               , "\033[1;8B"   },  // shift-ctrl-M-down
  { fc::Fcmkey_sright              , "\033[1;8C"   },  // shift-ctrl-M-right
  { fc::Fcmkey_sleft               , "\033[1;8D"   },  // shift-ctrl-M-left
  { fc::Fkey_menu                  , "\033[29~"    },  // menu
  { fc::Fkey_smenu                 , "\033[29$"    },  // shift-menu
  { fc::Fkey_smenu                 , "\033[29;2~"  },  // shift-menu
  { fc::Fckey_menu                 , "\033[29^"    },  // ctrl-menu
  { fc::Fckey_menu                 , "\033[29;5~"  },  // ctrl-menu
  { fc::Fckey_smenu                , "\033[29@"    },  // shift-ctrl-menu
  { fc::Fckey_smenu                , "\033[29;6~"  },  // shift-ctrl-menu
  { fc::Fmkey_menu                 , "\033[29;3~"  },  // M-menu
  { fc::Fmkey_smenu                , "\033[29;4~"  },  // shift-M-menu
  { fc::Fcmkey_menu                , "\033[29;7~"  },  // ctrl-M-menu
  { fc::Fcmkey_smenu               , "\033[29;8~"  },  // shift-ctrl-M-menu
  { fc::Fkey_escape_mintty         , "\033O["},  // mintty Esc
  { fc::Fmkey_tab                  , "\033\t"},  // M-tab
  { fc::Fmkey_enter                , "\033\n"},  // M-enter
  { fc::Fmkey_enter                , "\033\r"},  // M-enter
  { fc::Fmkey_space                , "\033 " },  // M-' '
  { fc::Fmkey_bang                 , "\033!" },  // M-!
  { fc::Fmkey_quotes               , "\033\""},  // M-"
  { fc::Fmkey_hash                 , "\033#" },  // M-#
  { fc::Fmkey_dollar               , "\033$" },  // M-$
  { fc::Fmkey_percent              , "\033%" },  // M-%
  { fc::Fmkey_ampersand            , "\033&" },  // M-&
  { fc::Fmkey_apostrophe           , "\033'" },  // M-'
  { fc::Fmkey_left_parenthesis     , "\033(" },  // M-(
  { fc::Fmkey_right_parenthesis    , "\033)" },  // M-)
  { fc::Fmkey_asterisk             , "\033*" },  // M-*
  { fc::Fmkey_plus                 , "\033+" },  // M-+
  { fc::Fmkey_comma                , "\033," },  // M-,
  { fc::Fmkey_minus                , "\033-" },  // M-'-'
  { fc::Fmkey_full_stop            , "\033." },  // M-.
  { fc::Fmkey_slash                , "\033/" },  // M-/
  { fc::Fmkey_0                    , "\0330" },  // M-0
  { fc::Fmkey_1                    , "\0331" },  // M-1
  { fc::Fmkey_2                    , "\0332" },  // M-2
  { fc::Fmkey_3                    , "\0333" },  // M-3
  { fc::Fmkey_4                    , "\0334" },  // M-4
  { fc::Fmkey_5                    , "\0335" },  // M-5
  { fc::Fmkey_6                    , "\0336" },  // M-6
  { fc::Fmkey_7                    , "\0337" },  // M-7
  { fc::Fmkey_8                    , "\0338" },  // M-8
  { fc::Fmkey_9                    , "\0339" },  // M-9
  { fc::Fmkey_colon                , "\033:" },  // M-:
  { fc::Fmkey_semicolon            , "\033;" },  // M-;
  { fc::Fmkey_less_than            , "\033<" },  // M-<
  { fc::Fmkey_equals               , "\033=" },  // M-=
  { fc::Fmkey_greater_than         , "\033>" },  // M->
  { fc::Fmkey_question_mark        , "\033?" },  // M-?
  { fc::Fmkey_at                   , "\033@" },  // M-@
  { fc::Fmkey_A                    , "\033A" },  // shift-M-A
  { fc::Fmkey_B                    , "\033B" },  // shift-M-B
  { fc::Fmkey_C                    , "\033C" },  // shift-M-C
  { fc::Fmkey_D                    , "\033D" },  // shift-M-D
  { fc::Fmkey_E                    , "\033E" },  // shift-M-E
  { fc::Fmkey_F                    , "\033F" },  // shift-M-F
  { fc::Fmkey_G                    , "\033G" },  // shift-M-G
  { fc::Fmkey_H                    , "\033H" },  // shift-M-H
  { fc::Fmkey_I                    , "\033I" },  // shift-M-I
  { fc::Fmkey_J                    , "\033J" },  // shift-M-J
  { fc::Fmkey_K                    , "\033K" },  // shift-M-K
  { fc::Fmkey_L                    , "\033L" },  // shift-M-L
  { fc::Fmkey_M                    , "\033M" },  // shift-M-M
  { fc::Fmkey_N                    , "\033N" },  // shift-M-N
  { fc::Fmkey_O                    , "\033O" },  // shift-M-O
  { fc::Fmkey_P                    , "\033P" },  // shift-M-P
  { fc::Fmkey_Q                    , "\033Q" },  // shift-M-Q
  { fc::Fmkey_R                    , "\033R" },  // shift-M-R
  { fc::Fmkey_S                    , "\033S" },  // shift-M-S
  { fc::Fmkey_T                    , "\033T" },  // shift-M-T
  { fc::Fmkey_U                    , "\033U" },  // shift-M-U
  { fc::Fmkey_V                    , "\033V" },  // shift-M-V
  { fc::Fmkey_W                    , "\033W" },  // shift-M-W
  { fc::Fmkey_X                    , "\033X" },  // shift-M-X
  { fc::Fmkey_Y                    , "\033Y" },  // shift-M-Y
  { fc::Fmkey_Z                    , "\033Z" },  // shift-M-Z
  { fc::Fmkey_left_square_bracket  , "\033[" },  // M-[
  { fc::Fmkey_backslash            , "\033\\"},  // M-'\'
  { fc::Fmkey_right_square_bracket , "\033]" },  // M-]
  { fc::Fmkey_caret                , "\033^" },  // M-^
  { fc::Fmkey_underscore           , "\033_" },  // M-_
  { fc::Fmkey_grave_accent         , "\033`" },  // M-`
  { fc::Fmkey_a                    , "\033a" },  // M-a
  { fc::Fmkey_b                    , "\033b" },  // M-b
  { fc::Fmkey_c                    , "\033c" },  // M-c
  { fc::Fmkey_d                    , "\033d" },  // M-d
  { fc::Fmkey_e                    , "\033e" },  // M-e
  { fc::Fmkey_f                    , "\033f" },  // M-f
  { fc::Fmkey_g                    , "\033g" },  // M-g
  { fc::Fmkey_h                    , "\033h" },  // M-h
  { fc::Fmkey_i                    , "\033i" },  // M-i
  { fc::Fmkey_j                    , "\033j" },  // M-j
  { fc::Fmkey_k                    , "\033k" },  // M-k
  { fc::Fmkey_l                    , "\033l" },  // M-l
  { fc::Fmkey_m                    , "\033m" },  // M-m
  { fc::Fmkey_n                    , "\033n" },  // M-n
  { fc::Fmkey_o                    , "\033o" },  // M-o
  { fc::Fmkey_p                    , "\033p" },  // M-p
  { fc::Fmkey_q                    , "\033q" },  // M-q
  { fc::Fmkey_r                    , "\033r" },  // M-r
  { fc::Fmkey_s                    , "\033s" },  // M-s
  { fc::Fmkey_t                    , "\033t" },  // M-t
  { fc::Fmkey_u                    , "\033u" },  // M-u
  { fc::Fmkey_v                    , "\033v" },  // M-v
  { fc::Fmkey_w                    , "\033w" },  // M-w
  { fc::Fmkey_x                    , "\033x" },  // M-x
  { fc::Fmkey_y                    , "\033y" },  // M-y
  { fc::Fmkey_z                    , "\033z" },  // M-z
  { fc::Fmkey_left_curly_bracket   , "\033{" },  // M-{
  { fc::Fmkey_vertical_bar         , "\033|" },  // M-|
  { fc::Fmkey_right_curly_bracket  , "\033}" },  // M-}
  { fc::Fmkey_tilde                , "\033~" },  // M-~
  { 0                              , "\0"  }
};

keyname FkeyName[] =
{
  { fc::Fckey_a                   , "Ctrl+A" },
  { fc::Fckey_b                   , "Ctrl+B" },
  { fc::Fckey_c                   , "Ctrl+C" },
  { fc::Fckey_d                   , "Ctrl+D" },
  { fc::Fckey_e                   , "Ctrl+E" },
  { fc::Fckey_f                   , "Ctrl+F" },
  { fc::Fckey_g                   , "Ctrl+G" },
  { fc::Fkey_erase                , "Backspace" },  // Ctrl+H
  { fc::Fkey_tab                  , "Tab" },        // Ctrl+I
  { fc::Fckey_j                   , "Ctrl+J" },
  { fc::Fckey_h                   , "Ctrl+K" },
  { fc::Fckey_l                   , "Ctrl+L" },
  { fc::Fkey_return               , "Return" },     // Ctrl+M
  { fc::Fckey_n                   , "Ctrl+N" },
  { fc::Fckey_o                   , "Ctrl+O" },
  { fc::Fckey_p                   , "Ctrl+P" },
  { fc::Fckey_q                   , "Ctrl+Q" },
  { fc::Fckey_r                   , "Ctrl+R" },
  { fc::Fckey_s                   , "Ctrl+S" },
  { fc::Fckey_t                   , "Ctrl+T" },
  { fc::Fckey_u                   , "Ctrl+U" },
  { fc::Fckey_v                   , "Ctrl+V" },
  { fc::Fckey_w                   , "Ctrl+W" },
  { fc::Fckey_x                   , "Ctrl+X" },
  { fc::Fckey_y                   , "Ctrl+Y" },
  { fc::Fckey_z                   , "Ctrl+Z" },
  { fc::Fkey_escape               , "Esc" },        // Ctrl+[
  { fc::Fkey_escape_mintty        , "Esc" },
  { fc::Fckey_backslash           , "Ctrl+\\" },
  { fc::Fckey_right_square_bracket, "Ctrl+]" },
  { fc::Fckey_caret               , "Ctrl+^" },
  { fc::Fckey_underscore          , "Ctrl+_" },
  { fc::Fkey_space                , "Space" },
  { fc::Fckey_space               , "Ctrl+Space" },  // Ctrl+(Space or @)
  { fc::Fkey_backspace            , "Backspace" },
  { fc::Fkey_catab                , "Clear-All-Tabs" },
  { fc::Fkey_clear                , "Clear-Screen" },
  { fc::Fkey_ctab                 , "Clear-Tab" },
  { fc::Fkey_dc                   , "Del" },
  { fc::Fkey_dl                   , "Del-line" },
  { fc::Fkey_down                 , "Down" },
  { fc::Fkey_eic                  , "Exit-Ins" },
  { fc::Fkey_eol                  , "Clear-End-of-Line" },
  { fc::Fkey_eos                  , "Clear-End-of-Screen" },
  { fc::Fkey_f0                   , "F0" },
  { fc::Fkey_f1                   , "F1" },
  { fc::Fkey_f2                   , "F2" },
  { fc::Fkey_f3                   , "F3" },
  { fc::Fkey_f4                   , "F4" },
  { fc::Fkey_f5                   , "F5" },
  { fc::Fkey_f6                   , "F6" },
  { fc::Fkey_f7                   , "F7" },
  { fc::Fkey_f8                   , "F8" },
  { fc::Fkey_f9                   , "F9" },
  { fc::Fkey_f10                  , "F10" },
  { fc::Fkey_home                 , "Home" },
  { fc::Fkey_ic                   , "Ins" },
  { fc::Fkey_il                   , "Ins-Line" },
  { fc::Fkey_left                 , "Left" },
  { fc::Fkey_ll                   , "Lower-Left" },
  { fc::Fkey_npage                , "PgDn" },
  { fc::Fkey_ppage                , "PgUp" },
  { fc::Fkey_right                , "Right" },
  { fc::Fkey_sf                   , "Scroll-Forward" },
  { fc::Fkey_sr                   , "Scroll-Backward" },
  { fc::Fkey_stab                 , "Set-Tab" },
  { fc::Fkey_up                   , "Up" },
  { fc::Fkey_a1                   , "Upper-Left" },
  { fc::Fkey_a3                   , "Upper-Right" },
  { fc::Fkey_b2                   , "Center" },
  { fc::Fkey_c1                   , "Lower-Left" },
  { fc::Fkey_c3                   , "Lower-Right" },
  { fc::Fkey_btab                 , "Shift+Tab" },
  { fc::Fkey_beg                  , "Begin" },
  { fc::Fkey_cancel               , "Cancel" },
  { fc::Fkey_close                , "Close" },
  { fc::Fkey_command              , "Command" },
  { fc::Fkey_copy                 , "Copy" },
  { fc::Fkey_create               , "Create" },
  { fc::Fkey_end                  , "End" },
  { fc::Fkey_enter                , "Enter" },
  { fc::Fkey_exit                 , "Exit" },
  { fc::Fkey_find                 , "Find" },
  { fc::Fkey_help                 , "Help" },
  { fc::Fkey_mark                 , "Mark" },
  { fc::Fkey_message              , "Message" },
  { fc::Fkey_move                 , "Move" },
  { fc::Fkey_next                 , "Next" },
  { fc::Fkey_open                 , "Open" },
  { fc::Fkey_options              , "Options" },
  { fc::Fkey_previous             , "Previous" },
  { fc::Fkey_print                , "Print" },
  { fc::Fkey_redo                 , "Redo" },
  { fc::Fkey_reference            , "Reference" },
  { fc::Fkey_refresh              , "Refresh" },
  { fc::Fkey_replace              , "Replace" },
  { fc::Fkey_restart              , "Restart" },
  { fc::Fkey_resume               , "Resume" },
  { fc::Fkey_save                 , "Save" },
  { fc::Fkey_suspend              , "Suspend" },
  { fc::Fkey_undo                 , "Undo" },
  { fc::Fkey_sbeg                 , "Shift+Begin" },
  { fc::Fkey_scancel              , "Shift+Cancel" },
  { fc::Fkey_scommand             , "Shift+Command" },
  { fc::Fkey_scopy                , "Shift+Copy" },
  { fc::Fkey_screate              , "Shift+Create" },
  { fc::Fkey_sdc                  , "Shift+Del" },
  { fc::Fkey_sdl                  , "Shift+Del-line" },
  { fc::Fkey_select               , "Select" },
  { fc::Fkey_send                 , "Shift+End" },
  { fc::Fkey_seol                 , "Shift+Clear-End-of-Line" },
  { fc::Fkey_sexit                , "Shift+Exit" },
  { fc::Fkey_sfind                , "Shift+Find" },
  { fc::Fkey_shelp                , "Shift+Help" },
  { fc::Fkey_shome                , "Shift+Home" },
  { fc::Fkey_sic                  , "Shift+Ins" },
  { fc::Fkey_sleft                , "Shift+Left" },
  { fc::Fkey_smessage             , "Shift+Message" },
  { fc::Fkey_smove                , "Shift+Move" },
  { fc::Fkey_snext                , "Shift+Next" },
  { fc::Fkey_soptions             , "Shift+Options" },
  { fc::Fkey_sprevious            , "Shift+Previous" },
  { fc::Fkey_sprint               , "Shift+Print" },
  { fc::Fkey_sredo                , "Shift+Redo" },
  { fc::Fkey_sreplace             , "Shift+Replace" },
  { fc::Fkey_sright               , "Shift+Right" },
  { fc::Fkey_srsume               , "Shift+Resume" },
  { fc::Fkey_ssave                , "Shift+Save" },
  { fc::Fkey_ssuspend             , "Shift+Suspend" },
  { fc::Fkey_sundo                , "Shift+Undo" },
  { fc::Fkey_f11                  , "F11" },
  { fc::Fkey_f12                  , "F12" },
  { fc::Fkey_f13                  , "Shift+F1" },
  { fc::Fkey_f14                  , "Shift+F2" },
  { fc::Fkey_f15                  , "Shift+F3" },
  { fc::Fkey_f16                  , "Shift+F4" },
  { fc::Fkey_f17                  , "Shift+F5" },
  { fc::Fkey_f18                  , "Shift+F6" },
  { fc::Fkey_f19                  , "Shift+F7" },
  { fc::Fkey_f20                  , "Shift+F8" },
  { fc::Fkey_f21                  , "Shift+F9" },
  { fc::Fkey_f22                  , "Shift+F10" },
  { fc::Fkey_f23                  , "Shift+F11" },
  { fc::Fkey_f24                  , "Shift+F12" },
  { fc::Fkey_f25                  , "Ctrl+F1" },
  { fc::Fkey_f26                  , "Ctrl+F2" },
  { fc::Fkey_f27                  , "Ctrl+F3" },
  { fc::Fkey_f28                  , "Ctrl+F4" },
  { fc::Fkey_f29                  , "Ctrl+F5" },
  { fc::Fkey_f30                  , "Ctrl+F6" },
  { fc::Fkey_f31                  , "Ctrl+F7" },
  { fc::Fkey_f32                  , "Ctrl+F8" },
  { fc::Fkey_f33                  , "Ctrl+F9" },
  { fc::Fkey_f34                  , "Ctrl+F10" },
  { fc::Fkey_f35                  , "Ctrl+F11" },
  { fc::Fkey_f36                  , "Ctrl+F12" },
  { fc::Fkey_f37                  , "Shift+Ctrl+F1" },
  { fc::Fkey_f38                  , "Shift+Ctrl+F2" },
  { fc::Fkey_f39                  , "Shift+Ctrl+F3" },
  { fc::Fkey_f40                  , "Shift+Ctrl+F4" },
  { fc::Fkey_f41                  , "Shift+Ctrl+F5" },
  { fc::Fkey_f42                  , "Shift+Ctrl+F6" },
  { fc::Fkey_f43                  , "Shift+Ctrl+F7" },
  { fc::Fkey_f44                  , "Shift+Ctrl+F8" },
  { fc::Fkey_f45                  , "Shift+Ctrl+F9" },
  { fc::Fkey_f46                  , "Shift+Ctrl+F10" },
  { fc::Fkey_f47                  , "Shift+Ctrl+F11" },
  { fc::Fkey_f48                  , "Shift+Ctrl+F12" },
  { fc::Fkey_f49                  , "Meta+F1" },
  { fc::Fkey_f50                  , "Meta+F2" },
  { fc::Fkey_f51                  , "Meta+F3" },
  { fc::Fkey_f52                  , "Meta+F4" },
  { fc::Fkey_f53                  , "Meta+F5" },
  { fc::Fkey_f54                  , "Meta+F6" },
  { fc::Fkey_f55                  , "Meta+F7" },
  { fc::Fkey_f56                  , "Meta+F8" },
  { fc::Fkey_f57                  , "Meta+F9" },
  { fc::Fkey_f58                  , "Meta+F10" },
  { fc::Fkey_f59                  , "Meta+F11" },
  { fc::Fkey_f60                  , "Meta+F12" },
  { fc::Fkey_f61                  , "Shift+Meta+F1" },
  { fc::Fkey_f62                  , "Shift+Meta+F2" },
  { fc::Fkey_f63                  , "Shift+Meta+F3" },
  { fc::Fmkey_ic                  , "Meta+Ins" },
  { fc::Fmkey_dc                  , "Meta+Del" },
  { fc::Fmkey_home                , "Meta+Home" },
  { fc::Fmkey_end                 , "Meta+End" },
  { fc::Fmkey_ppage               , "Meta+PgUp" },
  { fc::Fmkey_npage               , "Meta+PgDn" },
  { fc::Fmkey_f1                  , "Meta+F1" },
  { fc::Fmkey_f2                  , "Meta+F2" },
  { fc::Fmkey_f3                  , "Meta+F3" },
  { fc::Fmkey_f4                  , "Meta+F4" },
  { fc::Fmkey_f5                  , "Meta+F5" },
  { fc::Fmkey_f6                  , "Meta+F6" },
  { fc::Fmkey_f7                  , "Meta+F7" },
  { fc::Fmkey_f8                  , "Meta+F8" },
  { fc::Fmkey_f9                  , "Meta+F9" },
  { fc::Fmkey_f10                 , "Meta+F10" },
  { fc::Fmkey_f11                 , "Meta+F11" },
  { fc::Fmkey_f12                 , "Meta+F12" },
  { fc::Fmkey_up                  , "Meta+Up" },
  { fc::Fmkey_down                , "Meta+Down" },
  { fc::Fmkey_right               , "Meta+Right" },
  { fc::Fmkey_left                , "Meta+Left" },
  { fc::Fmkey_sic                 , "Shift+Meta+Ins" },
  { fc::Fmkey_sdc                 , "Shift+Meta+Del" },
  { fc::Fmkey_shome               , "Shift+Meta+Home" },
  { fc::Fmkey_send                , "Shift+Meta+End" },
  { fc::Fmkey_sppage              , "Shift+Meta+PgUp" },
  { fc::Fmkey_snpage              , "Shift+Meta+PgDn" },
  { fc::Fmkey_sf1                 , "Shift+Meta+F1" },
  { fc::Fmkey_sf2                 , "Shift+Meta+F2" },
  { fc::Fmkey_sf3                 , "Shift+Meta+F3" },
  { fc::Fmkey_sf4                 , "Shift+Meta+F4" },
  { fc::Fmkey_sf5                 , "Shift+Meta+F5" },
  { fc::Fmkey_sf6                 , "Shift+Meta+F6" },
  { fc::Fmkey_sf7                 , "Shift+Meta+F7" },
  { fc::Fmkey_sf8                 , "Shift+Meta+F8" },
  { fc::Fmkey_sf9                 , "Shift+Meta+F9" },
  { fc::Fmkey_sf10                , "Shift+Meta+F10" },
  { fc::Fmkey_sf11                , "Shift+Meta+F11" },
  { fc::Fmkey_sf12                , "Shift+Meta+F12" },
  { fc::Fmkey_sup                 , "Shift+Meta+Up" },
  { fc::Fmkey_sdown               , "Shift+Meta+Down" },
  { fc::Fmkey_sright              , "Shift+Meta+Right" },
  { fc::Fmkey_sleft               , "Shift+Meta+Left" },
  { fc::Fckey_ic                  , "Ctrl+Ins" },
  { fc::Fckey_dc                  , "Ctrl+Del" },
  { fc::Fckey_home                , "Ctrl+Home" },
  { fc::Fckey_end                 , "Ctrl+End" },
  { fc::Fckey_ppage               , "Ctrl+PgUp" },
  { fc::Fckey_npage               , "Ctrl+PgDn" },
  { fc::Fckey_up                  , "Ctrl+Up" },
  { fc::Fckey_down                , "Ctrl+Down" },
  { fc::Fckey_right               , "Ctrl+Right" },
  { fc::Fckey_left                , "Ctrl+Left" },
  { fc::Fckey_sic                 , "Shift+Ctrl+Ins" },
  { fc::Fckey_sdc                 , "Shift+Ctrl+Del" },
  { fc::Fckey_shome               , "Shift+Ctrl+Home" },
  { fc::Fckey_send                , "Shift+Ctrl+End" },
  { fc::Fckey_sppage              , "Shift+Ctrl+PgUp" },
  { fc::Fckey_snpage              , "Shift+Ctrl+PgDn" },
  { fc::Fckey_sup                 , "Shift+Ctrl+Up" },
  { fc::Fckey_sdown               , "Shift+Ctrl+Down" },
  { fc::Fckey_sright              , "Shift+Ctrl+Right" },
  { fc::Fckey_sleft               , "Shift+Ctrl+Left" },
  { fc::Fcmkey_ic                 , "Ctrl+Meta+Ins" },
  { fc::Fcmkey_dc                 , "Ctrl+Meta+Del" },
  { fc::Fcmkey_home               , "Ctrl+Meta+Home" },
  { fc::Fcmkey_end                , "Ctrl+Meta+End" },
  { fc::Fcmkey_ppage              , "Ctrl+Meta+PgUp" },
  { fc::Fcmkey_npage              , "Ctrl+Meta+PgDn" },
  { fc::Fcmkey_up                 , "Ctrl+Meta+Up" },
  { fc::Fcmkey_down               , "Ctrl+Meta+Down" },
  { fc::Fcmkey_right              , "Ctrl+Meta+Right" },
  { fc::Fcmkey_left               , "Ctrl+Meta+Left" },
  { fc::Fcmkey_sic                , "Shift+Ctrl+Meta+Ins" },
  { fc::Fcmkey_sdc                , "Shift+Ctrl+Meta+Del" },
  { fc::Fcmkey_shome              , "Shift+Ctrl+Meta+Home" },
  { fc::Fcmkey_send               , "Shift+Ctrl+Meta+End" },
  { fc::Fcmkey_sppage             , "Shift+Ctrl+Meta+PgUp" },
  { fc::Fcmkey_snpage             , "Shift+Ctrl+Meta+PgDn" },
  { fc::Fcmkey_sf1                , "Shift+Ctrl+Meta+F1" },
  { fc::Fcmkey_sf2                , "Shift+Ctrl+Meta+F2" },
  { fc::Fcmkey_sf3                , "Shift+Ctrl+Meta+F3" },
  { fc::Fcmkey_sf4                , "Shift+Ctrl+Meta+F4" },
  { fc::Fcmkey_sf5                , "Shift+Ctrl+Meta+F5" },
  { fc::Fcmkey_sf6                , "Shift+Ctrl+Meta+F6" },
  { fc::Fcmkey_sf7                , "Shift+Ctrl+Meta+F7" },
  { fc::Fcmkey_sf8                , "Shift+Ctrl+Meta+F8" },
  { fc::Fcmkey_sf9                , "Shift+Ctrl+Meta+F9" },
  { fc::Fcmkey_sf10               , "Shift+Ctrl+Meta+F10" },
  { fc::Fcmkey_sf11               , "Shift+Ctrl+Meta+F11" },
  { fc::Fcmkey_sf12               , "Shift+Ctrl+Meta+F12" },
  { fc::Fcmkey_sup                , "Shift+Ctrl+Meta+Up" },
  { fc::Fcmkey_sdown              , "Shift+Ctrl+Meta+Down" },
  { fc::Fcmkey_sright             , "Shift+Ctrl+Meta+Right" },
  { fc::Fcmkey_sleft              , "Shift+Ctrl+Meta+Left" },
  { fc::Fkey_menu                 , "Menu" },
  { fc::Fkey_smenu                , "Shift+Menu" },
  { fc::Fckey_menu                , "Ctrl+Menu" },
  { fc::Fckey_smenu               , "Shift+Ctrl+Menu" },
  { fc::Fmkey_menu                , "Meta+Menu" },
  { fc::Fmkey_smenu               , "Shift+Meta+Menu" },
  { fc::Fcmkey_menu               , "Ctrl+Meta+Menu" },
  { fc::Fcmkey_smenu              , "Shift+Ctrl+Meta+Menu" },
  { fc::Fmkey_tab                 , "Meta+Tab" },
  { fc::Fmkey_enter               , "Meta+Enter" },
  { fc::Fmkey_space               , "Meta+Space" },
  { fc::Fmkey_bang                , "Meta+!" },
  { fc::Fmkey_quotes              , "Meta+\"" },
  { fc::Fmkey_hash                , "Meta+#" },
  { fc::Fmkey_dollar              , "Meta+$" },
  { fc::Fmkey_percent             , "Meta+%" },
  { fc::Fmkey_ampersand           , "Meta+&" },
  { fc::Fmkey_apostrophe          , "Meta+'" },
  { fc::Fmkey_left_parenthesis    , "Meta+(" },
  { fc::Fmkey_right_parenthesis   , "Meta+)" },
  { fc::Fmkey_asterisk            , "Meta+*" },
  { fc::Fmkey_plus                , "Meta++" },
  { fc::Fmkey_comma               , "Meta+," },
  { fc::Fmkey_minus               , "Meta+-" },
  { fc::Fmkey_full_stop           , "Meta+." },
  { fc::Fmkey_slash               , "Meta+/" },
  { fc::Fmkey_0                   , "Meta+0" },
  { fc::Fmkey_1                   , "Meta+1" },
  { fc::Fmkey_2                   , "Meta+2" },
  { fc::Fmkey_3                   , "Meta+3" },
  { fc::Fmkey_4                   , "Meta+4" },
  { fc::Fmkey_5                   , "Meta+5" },
  { fc::Fmkey_6                   , "Meta+6" },
  { fc::Fmkey_7                   , "Meta+7" },
  { fc::Fmkey_8                   , "Meta+8" },
  { fc::Fmkey_9                   , "Meta+9" },
  { fc::Fmkey_colon               , "Meta+:" },
  { fc::Fmkey_semicolon           , "Meta+;" },
  { fc::Fmkey_less_than           , "Meta+<" },
  { fc::Fmkey_equals              , "Meta+=" },
  { fc::Fmkey_greater_than        , "Meta+>" },
  { fc::Fmkey_question_mark       , "Meta+?" },
  { fc::Fmkey_at                  , "Meta+@" },
  { fc::Fmkey_A                   , "Shift+Meta+A" },
  { fc::Fmkey_B                   , "Shift+Meta+B" },
  { fc::Fmkey_C                   , "Shift+Meta+C" },
  { fc::Fmkey_D                   , "Shift+Meta+D" },
  { fc::Fmkey_E                   , "Shift+Meta+E" },
  { fc::Fmkey_F                   , "Shift+Meta+F" },
  { fc::Fmkey_G                   , "Shift+Meta+G" },
  { fc::Fmkey_H                   , "Shift+Meta+H" },
  { fc::Fmkey_I                   , "Shift+Meta+I" },
  { fc::Fmkey_J                   , "Shift+Meta+J" },
  { fc::Fmkey_K                   , "Shift+Meta+K" },
  { fc::Fmkey_L                   , "Shift+Meta+L" },
  { fc::Fmkey_M                   , "Shift+Meta+M" },
  { fc::Fmkey_N                   , "Shift+Meta+N" },
  { fc::Fmkey_O                   , "Shift+Meta+O" },
  { fc::Fmkey_P                   , "Shift+Meta+P" },
  { fc::Fmkey_Q                   , "Shift+Meta+Q" },
  { fc::Fmkey_R                   , "Shift+Meta+R" },
  { fc::Fmkey_S                   , "Shift+Meta+S" },
  { fc::Fmkey_T                   , "Shift+Meta+T" },
  { fc::Fmkey_U                   , "Shift+Meta+U" },
  { fc::Fmkey_V                   , "Shift+Meta+V" },
  { fc::Fmkey_W                   , "Shift+Meta+W" },
  { fc::Fmkey_X                   , "Shift+Meta+X" },
  { fc::Fmkey_Y                   , "Shift+Meta+Y" },
  { fc::Fmkey_Z                   , "Shift+Meta+Z" },
  { fc::Fmkey_left_square_bracket , "Meta+[" },
  { fc::Fmkey_backslash           , "Meta+\\" },
  { fc::Fmkey_right_square_bracket, "Meta+]" },
  { fc::Fmkey_caret               , "Meta+^" },
  { fc::Fmkey_underscore          , "Meta+_" },
  { fc::Fmkey_grave_accent        , "Meta+`" },
  { fc::Fmkey_a                   , "Meta+A" },
  { fc::Fmkey_b                   , "Meta+B" },
  { fc::Fmkey_c                   , "Meta+C" },
  { fc::Fmkey_d                   , "Meta+D" },
  { fc::Fmkey_e                   , "Meta+E" },
  { fc::Fmkey_f                   , "Meta+F" },
  { fc::Fmkey_g                   , "Meta+G" },
  { fc::Fmkey_h                   , "Meta+H" },
  { fc::Fmkey_i                   , "Meta+I" },
  { fc::Fmkey_j                   , "Meta+J" },
  { fc::Fmkey_k                   , "Meta+K" },
  { fc::Fmkey_l                   , "Meta+L" },
  { fc::Fmkey_m                   , "Meta+M" },
  { fc::Fmkey_n                   , "Meta+N" },
  { fc::Fmkey_o                   , "Meta+O" },
  { fc::Fmkey_p                   , "Meta+P" },
  { fc::Fmkey_q                   , "Meta+Q" },
  { fc::Fmkey_r                   , "Meta+R" },
  { fc::Fmkey_s                   , "Meta+S" },
  { fc::Fmkey_t                   , "Meta+T" },
  { fc::Fmkey_u                   , "Meta+U" },
  { fc::Fmkey_v                   , "Meta+V" },
  { fc::Fmkey_w                   , "Meta+W" },
  { fc::Fmkey_x                   , "Meta+X" },
  { fc::Fmkey_y                   , "Meta+Y" },
  { fc::Fmkey_z                   , "Meta+Z" },
  { fc::Fmkey_left_curly_bracket  , "Meta+{" },
  { fc::Fmkey_vertical_bar        , "Meta+|" },
  { fc::Fmkey_right_curly_bracket , "Meta+}" },
  { fc::Fmkey_tilde               , "Meta+~" },
  { fc::Fkey_mouse                , "xterm mouse" },
  { fc::Fkey_extended_mouse       , "SGR extended mouse" },
  { fc::Fkey_urxvt_mouse          , "urxvt mouse extension" },
  { 0                             , "\0" }
};

}  // namespace fc

}  // namespace finalcut
