/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

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

#ifndef DIVERSIA_PLATFORM_H
#define DIVERSIA_PLATFORM_H

#include "Util/Build/Config.h"

namespace Diversia
{
// Initial platform/compiler-related stuff to set.
#define DIVERSIA_PLATFORM_WIN32 1
#define DIVERSIA_PLATFORM_LINUX 2
#define DIVERSIA_PLATFORM_APPLE 3
#define DIVERSIA_PLATFORM_SYMBIAN 4

#define DIVERSIA_COMPILER_MSVC 1
#define DIVERSIA_COMPILER_GNUC 2
#define DIVERSIA_COMPILER_BORL 3
#define DIVERSIA_COMPILER_WINSCW 4
#define DIVERSIA_COMPILER_GCCE 5

#define DIVERSIA_ENDIAN_LITTLE 1
#define DIVERSIA_ENDIAN_BIG 2

#define DIVERSIA_ARCHITECTURE_32 1
#define DIVERSIA_ARCHITECTURE_64 2

/* Finds the compiler type and version.
*/
#if defined( __GCCE__ )
#   define DIVERSIA_COMPILER DIVERSIA_COMPILER_GCCE
#   define DIVERSIA_COMP_VER _MSC_VER
//#	include <staticlibinit_gcce.h> // This is a GCCE toolchain workaround needed when compiling with GCCE
#elif defined( __WINSCW__ )
#   define DIVERSIA_COMPILER DIVERSIA_COMPILER_WINSCW
#   define DIVERSIA_COMP_VER _MSC_VER
#elif defined( _MSC_VER )
#   define DIVERSIA_COMPILER DIVERSIA_COMPILER_MSVC
#   define DIVERSIA_COMP_VER _MSC_VER
#elif defined( __GNUC__ )
#   define DIVERSIA_COMPILER DIVERSIA_COMPILER_GNUC
#   define DIVERSIA_COMP_VER (((__GNUC__)*100) + \
        (__GNUC_MINOR__*10) + \
        __GNUC_PATCHLEVEL__)

#elif defined( __BORLANDC__ )
#   define DIVERSIA_COMPILER DIVERSIA_COMPILER_BORL
#   define DIVERSIA_COMP_VER __BCPLUSPLUS__
#   define __FUNCTION__ __FUNC__
#else
#   pragma error "No known compiler. Abort! Abort!"

#endif

// See if we can use __forceinline
// or if we need to use __inline instead
#if DIVERSIA_COMPILER == DIVERSIA_COMPILER_MSVC
#   if DIVERSIA_COMP_VER >= 1200
#       define _INLINE __forceinline
#   endif
#elif defined(__MINGW32__)
#   if !defined(_INLINE)
#       define _INLINE __inline
#   endif
#else
#   define _INLINE __inline
#endif

// Finds the current platform
#if defined( __SYMBIAN32__ )
#   define DIVERSIA_PLATFORM DIVERSIA_PLATFORM_SYMBIAN
#elif defined( __WIN32__ ) || defined( _WIN32 )
#   define DIVERSIA_PLATFORM DIVERSIA_PLATFORM_WIN32
#elif defined( __APPLE_CC__)
#   define DIVERSIA_PLATFORM DIVERSIA_PLATFORM_APPLE
#else
#   define DIVERSIA_PLATFORM DIVERSIA_PLATFORM_LINUX
#endif

// Find the arch type
#if defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__) || defined(__alpha__) || defined(__ia64__) || defined(__s390__) || defined(__s390x__)
#   define DIVERSIA_ARCH_TYPE DIVERSIA_ARCHITECTURE_64
#else
#   define DIVERSIA_ARCH_TYPE DIVERSIA_ARCHITECTURE_32
#endif

// For generating compiler warnings - should work on any compiler
// As a side note, if you start your message with 'Warning: ', the MSVC
// IDE actually does catch a warning :)
#define DIVERSIA_QUOTE_INPLACE(x) # x
#define DIVERSIA_QUOTE(x) DIVERSIA_QUOTE_INPLACE(x)
#define DIVERSIA_WARN( x )  message( __FILE__ "(" QUOTE( __LINE__ ) ") : " x "\n" )

// Debugging info
#if defined(DBGINFO) || defined(_DEBUG) || defined(DEBUG)
#   define DIVERSIA_DEBUGINFO
#endif

// Profiling
#if defined(PROFILING)
#   define DIVERSIA_PROFILING
#endif

//------------------------------------------------------------------------------
// Windows Settings
#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32

// Win32 compilers use _DEBUG for specifying debug builds.
#   ifdef _DEBUG
#       define DIVERSIA_DEBUG
#   endif

// Disable unicode support on MingW at the moment, poorly supported in stdlibc++
// STLPORT fixes this though so allow if found
// MinGW C++ Toolkit supports unicode and sets the define __MINGW32_TOOLKIT_UNICODE__ in _mingw.h
#if defined( __MINGW32__ ) && !defined(_STLPORT_VERSION)
#   include<_mingw.h>
#   if defined(__MINGW32_TOOLBOX_UNICODE__)
#	    define DIVERSIA_UNICODE_SUPPORT 1
#   else
#       define DIVERSIA_UNICODE_SUPPORT 0
#   endif
#else
#	define DIVERSIA_UNICODE_SUPPORT 1
#endif

#define NOMINMAX

#endif
//------------------------------------------------------------------------------
// Linux/Apple Settings
#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_LINUX || DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_APPLE

// A quick define to overcome different names for the same function
#   define stricmp strcasecmp

// Unlike the Win32 compilers, Linux compilers seem to use DEBUG for when
// specifying a debug build.
#   ifdef DEBUG
#       define DIVERSIA_DEBUG
#   endif

/*#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_APPLE
    #define DIVERSIA_PLATFORM_LIB "DiversiaPlatform.bundle"
#else
    //DIVERSIA_PLATFORM_LINUX
    #define DIVERSIA_PLATFORM_LIB "libDiversiaPlatform.so"
#endif*/

// Always enable unicode support for the moment
// Perhaps disable in old versions of gcc if necessary
//#define DIVERSIA_UNICODE_SUPPORT 1

#endif
//------------------------------------------------------------------------------
// Check for release mode.
#if !defined(DIVERSIA_DEBUGINFO) && !defined(DIVERSIA_PROFILING) && !defined(DIVERSIA_DEBUG)
#   define DIVERSIA_RELEASE
#endif

// Integer formats of fixed bit width
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

// define uint64 type
#if DIVERSIA_COMPILER == DIVERSIA_COMPILER_MSVC
    typedef unsigned __int64 uint64;
#else
    typedef unsigned long long uint64;
#endif

//------------------------------------------------------------------------------
// Hash map/set
#if DIVERSIA_COMPILER == DIVERSIA_COMPILER_GNUC && DIVERSIA_COMP_VER >= 310 && !defined(STLPORT)
#   if DIVERSIA_COMP_VER >= 430
#       define DiversiaHashMap ::std::tr1::unordered_map
#		define DiversiaHashSet ::std::tr1::unordered_set
#    else
#       define DiversiaHashMap ::__gnu_cxx::hash_map
#       define DiversiaHashSet ::__gnu_cxx::hash_set
#    endif
#else
#   if DIVERSIA_COMPILER == DIVERSIA_COMPILER_MSVC
#       if DIVERSIA_COMP_VER > 1300 && !defined(_STLP_MSVC)
#           define DiversiaHashMap ::stdext::hash_map
#           define DiversiaHashSet ::stdext::hash_set
#       else
#           define DiversiaHashMap ::std::hash_map
#           define DiversiaHashSet ::std::hash_set
#       endif
#   else
#       define DiversiaHashMap ::std::hash_map
#       define DiversiaHashSet ::std::hash_set
#   endif
#endif

//------------------------------------------------------------------------------
} // namespace Diversia

#endif // DIVERSIA_PLATFORM_H