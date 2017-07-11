// File: ftermbuffer.h
// Provides: class FTermBuffer
//
//  Standalone class
//  ════════════════
//
// ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
// ▕ FTermBuffer ▏
// ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏


#ifndef FTERMBUFFER_H
#define FTERMBUFFER_H

#include <vector>
#include <sstream>  // std::stringstream

#include "fvterm.h"
#include "fstring.h"


//----------------------------------------------------------------------
// class FTermBuffer
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FTermBuffer
{
 public:
   // Typedef
   typedef FOptiAttr::char_data  char_data;

   // Constructor
   explicit FTermBuffer();

   // Destructor
   virtual ~FTermBuffer();

   // Overloaded operators
   template<class type> FTermBuffer& operator << (const type&);

   // Accessors
   virtual const char*    getClassName() const;
   int                    getLength() const;

   // Inquiry
   bool 	isEmpty () const;

   // Methods
   void                   clear();
   int                    writef (const wchar_t*, ...);
   int                    writef (const char*, ...)
   #if defined(__clang__)
     __attribute__((__format__ (__printf__, 2, 3)))
   #elif defined(__GNUC__)
     __attribute__ ((format (printf, 2, 3)))
   #endif
                          ;
   int                    write (const std::wstring&);
   int                    write (const wchar_t*);
   int                    write (const char*);
   int                    write (const std::string&);
   int                    write (const FString&);
   int                    write (int);
   FTermBuffer&           write ();
   std::vector<char_data> getBuffer();

 private:
   std::vector<char_data> data;
};

#pragma pack(pop)


// FTermBuffer inline functions
//----------------------------------------------------------------------
template<class type>
inline FTermBuffer& FTermBuffer::operator << (const type& s)
{
  std::ostringstream outstream;
  outstream << s;
  write (outstream.str());
  return *this;
}

//----------------------------------------------------------------------
inline const char* FTermBuffer::getClassName() const
{ return "FTermBuffer"; }

//----------------------------------------------------------------------
inline int FTermBuffer::getLength() const
{ return int(data.size()); }

//----------------------------------------------------------------------
inline bool FTermBuffer::isEmpty() const
{ return data.empty(); }

//----------------------------------------------------------------------
inline void FTermBuffer::clear()
{ data.clear(); }

//----------------------------------------------------------------------
inline FTermBuffer& FTermBuffer::write()
{ return *this; }

//----------------------------------------------------------------------
inline std::vector<FTermBuffer::char_data> FTermBuffer::getBuffer()
{ return data; }

#endif  // FTERMBUFFER_H
