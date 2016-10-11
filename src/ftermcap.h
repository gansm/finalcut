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
