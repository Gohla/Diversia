/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_UTIL_DEFINES_H
#define DIVERSIA_UTIL_DEFINES_H

namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------

// Shortened typedefs
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

// define the real number values to be used
// default to use 'float' unless precompiler option set
#if DIVERSIA_DOUBLE_PRECISION == 1
    typedef double Real;
#else
    typedef float Real;
#endif

#if DIVERSIA_WCHAR_T_STRINGS
    typedef std::wstring _StringBase;
#else
    typedef std::string _StringBase;
#endif

#if DIVERSIA_WCHAR_T_STRINGS
    typedef std::basic_stringstream<wchar_t,std::char_traits<wchar_t>,std::allocator<wchar_t> > _StringStreamBase;
#else
    typedef std::basic_stringstream<char,std::char_traits<char>,std::allocator<char> > _StringStreamBase;
#endif

typedef _StringBase String;
typedef _StringStreamBase StringStream;
typedef StringStream stringstream;

typedef boost::filesystem::path Path;

// Camp class macro
#define CAMP_CLASS(className) CAMP_RTTI() \
    friend class Bindings::CampBindings; \
    friend void camp::detail::destroy<className>( const UserObject& object );

//------------------------------------------------------------------------------
} // Namespace Util
} // Namespace Diversia

#endif // DIVERSIA_UTIL_DEFINES_H
