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
   // Typedef
   typedef struct
   {
     char* string;
     char  tname[3];
   }
   tcap_map;

   // Constructors
   FTermcap()
   { }

   // Destructor
  ~FTermcap()
   { }

   // Accessor
   tcap_map* getTermcapMap()
   {
     return tcap;
   }

   // Mutator
   void setTermcapMap (tcap_map* t)
   {
     tcap = t;
   }

   // Data Members
   static bool background_color_erase;
   static bool automatic_left_margin;
   static bool automatic_right_margin;
   static bool eat_nl_glitch;
   static bool ansi_default_color;
   static bool osc_support;
   static bool no_utf8_acs_chars;
   static int  max_color;
   static uInt tabstop;
   static uInt attr_without_color;

 private:
   // Data Members
   static tcap_map* tcap;
};
#pragma pack(pop)

#endif  // _FTERMCAP_H
