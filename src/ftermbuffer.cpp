// File: ftermbuffer.cpp
// Provides: class FTermBuffer

#include "ftermbuffer.h"


//----------------------------------------------------------------------
// class FTermBuffer
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FTermBuffer::FTermBuffer()
  : data()
{ }

//----------------------------------------------------------------------
FTermBuffer::~FTermBuffer() // destructor
{ }


// public methods of FTermBuffer
//----------------------------------------------------------------------
int FTermBuffer::writef (const wchar_t* format, ...)
{
  assert ( format != 0 );
  static const int buf_size = 1024;
  wchar_t buffer[buf_size];
  va_list args;

  va_start (args, format);
  std::vswprintf (buffer, buf_size, format, args);
  va_end (args);

  FString str(buffer);
  return write(str);
}

//----------------------------------------------------------------------
int FTermBuffer::writef (const char* format, ...)
{
  assert ( format != 0 );
  int   len;
  char  buf[512];
  char* buffer;
  va_list args;

  buffer = buf;
  va_start (args, format);
  len = vsnprintf (buffer, sizeof(buf), format, args);
  va_end (args);

  if ( len >= int(sizeof(buf)) )
  {
    try
    {
      buffer = new char[len+1]();
    }
    catch (const std::bad_alloc& ex)
    {
      std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
      return -1;
    }

    va_start (args, format);
    vsnprintf (buffer, uLong(len+1), format, args);
    va_end (args);
  }

  FString str(buffer);
  int ret = write(str);

  if ( buffer != buf )
    delete[] buffer;

  return ret;
}

//----------------------------------------------------------------------
int FTermBuffer::write (const std::wstring& s)
{
  assert ( ! s.empty() );
  return write (FString(s));
}

//----------------------------------------------------------------------
int FTermBuffer::write (const wchar_t* s)
{
  assert ( s != 0 );
  return write (FString(s));
}

//----------------------------------------------------------------------
int FTermBuffer::write (const char* s)
{
  assert ( s != 0 );
  FString str(s);
  return write(str);
}

//----------------------------------------------------------------------
int FTermBuffer::write (const std::string& s)
{
  assert ( ! s.empty() );
  return write (FString(s));
}

//----------------------------------------------------------------------
int FTermBuffer::write (const FString& s)
{
  assert ( ! s.isNull() );
  register int len = 0;
  const wchar_t* p = s.wc_str();

  if ( p )
  {
    while ( *p )
    {
      char_data  nc; // next character
      nc = FVTerm::getAttribute();
      nc.code = *p;
      nc.attr.bit.no_changes = false;
      nc.attr.bit.printed = false;

      data.push_back(nc);

      p++;
      len++;
    } // end of while
  }

  return len;
}

//----------------------------------------------------------------------
int FTermBuffer::write (register int c)
{
  char_data nc; // next character
  nc = FVTerm::getAttribute();
  nc.code = c;
  nc.attr.bit.no_changes = false;
  nc.attr.bit.printed = false;

  data.push_back(nc);
  return 1;
}


// FTermBuffer non-member operators
//----------------------------------------------------------------------
std::vector< FTermBuffer::char_data>& operator << ( std::vector<FTermBuffer::char_data>& termString
                                                  , const FTermBuffer& buf )
{
  if ( ! buf.data.empty() )
    termString.assign(buf.data.begin(), buf.data.end());

  return termString;
}
