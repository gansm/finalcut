// File: ftermcap.h
// Provides: class FTermcap
//
//  Standalone class
//  ════════════════
//
// ▕▔▔▔▔▔▔▔▔▔▔▏
// ▕ FTermcap ▏
// ▕▁▁▁▁▁▁▁▁▁▁▏

#ifndef _FTERMCAP_H
#define _FTERMCAP_H


//----------------------------------------------------------------------
// class FTermcap
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FTermcap
{
 public:
   typedef struct
   {
     char* string;
     char  tname[3];
   }
   tcap_map;

   static bool background_color_erase;
   static bool automatic_left_margin;
   static bool automatic_right_margin;
   static bool eat_nl_glitch;
   static bool ansi_default_color;
   static bool osc_support;
   static int  max_color;
   static uInt tabstop;
   static uInt attr_without_color;

 private:
   static tcap_map* tcap;

 public:
   FTermcap()
   { }

  ~FTermcap()
   { }

   tcap_map* getTermcapMap()
   {
     return tcap;
   }
  
   void setTermcapMap (tcap_map* t)
   {
     tcap = t;
   }
};
#pragma pack(pop)

#endif  // _FTERMCAP_H
