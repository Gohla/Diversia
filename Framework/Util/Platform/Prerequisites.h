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

#ifndef DIVERSIA_UTIL_PREREQUISITES_H
#define DIVERSIA_UTIL_PREREQUISITES_H

// Platform-specific stuff
#include "Util/Build/Platform.h"

// Needed for DIVERSIA_WCHAR_T_STRINGS below
#include <string>

#if DIVERSIA_COMPILER == DIVERSIA_COMPILER_MSVC
// disable: "conversion from 'double' to 'float', possible loss of data
#   pragma warning (disable : 4244)

// disable: "truncation from 'double' to 'float'
#   pragma warning (disable : 4305)

// disable: "<type> needs to have dll-interface to be used by clients'
// Happens on STL member variables which are not public therefore is ok
#   pragma warning (disable : 4251)

// disable: "non dll-interface class used as base for dll-interface class"
// Happens when deriving from Singleton because bug in compiler ignores
// template export
#   pragma warning (disable : 4275)

// disable: "no suitable definition provided for explicit template
// instantiation request" Occurs in VC7 for no justifiable reason on all
// #includes of Singleton
#   pragma warning (disable : 4661)

// disable: deprecation warnings when using CRT calls in VC8
// These show up on all C runtime lib code in VC8, disable since they clutter
// the warnings with things we may not be able to do anything about (e.g.
// generated code from nvparse etc). I doubt very much that these calls
// will ever be actually removed from VC anyway, it would break too much code.
#   pragma warning (disable: 4996)
#endif

namespace Diversia
{
namespace Util
{
// Exports
#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32
#   ifndef DIVERSIA_UTIL_STATIC
#       ifndef DIVERSIA_UTIL_API
#           if defined( DIVERSIA_UTIL_EXPORTS )
#               define DIVERSIA_UTIL_API __declspec(dllexport)
#           else
#               if defined(__MINGW32__)
#                   define DIVERSIA_UTIL_API
#               else
#                   define DIVERSIA_UTIL_API __declspec(dllimport)
#               endif
#           endif
#       endif
#   else
#       define DIVERSIA_UTIL_API
#   endif
#endif

#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_LINUX || DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_APPLE
#   if defined( DIVERSIA_UTIL_GCC_VISIBILITY )
#       define DIVERSIA_UTIL_API  __attribute__ ((visibility("default")))
#   else
#       define DIVERSIA_UTIL_API
#   endif
#endif

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

// Forward declarations
// Helper
class Exception;
class ConsoleInput;

// Math
class Angle;
class BoundingBox;
class BoundingSphere;
class Colour;
class Degree;
class Math;
class Matrix3;
class Matrix4;
class Node;
class Plane;
class Quaternion;
class Radian;
class Ray;
class Vector2;
class Vector3;
class Vector4;

// Serialization
class SerializationFile;
class XMLSerializationFile;

// Job
class Job;
class Request;
class RequestManager;

// State
class State;
class StateMachine;
} // namespace Util
} // namespace Diversia

// Include after the configuration has been set.
#include "Util/Build/StdHeaders.h"

namespace Diversia
{
namespace Util
{
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
typedef std::vector<Path> Paths;
} // namespace Util
} // namespace Diversia

// Camp bindings
#include "Util/Camp/CampBindings.h"

// Logging
#include "Util/Log/Log.h"
namespace Diversia
{
namespace Util
{
namespace Log
{
// Static logger
static boost::log::sources::severity_channel_logger_mt< Diversia::Util::LogLevel > lg
(
    boost::log::keywords::severity = Diversia::Util::LOG_INFO,
    boost::log::keywords::channel = String( "Util" )
);

// Easy logging defines.
#define ULOG(severity) BOOST_LOG_SEV(Diversia::Util::Log::lg, severity)
#define ULOGC BOOST_LOG_SEV(Diversia::Util::Log::lg, Diversia::Util::LOG_CRITICAL)
#define ULOGE BOOST_LOG_SEV(Diversia::Util::Log::lg, Diversia::Util::LOG_ERROR)
#define ULOGW BOOST_LOG_SEV(Diversia::Util::Log::lg, Diversia::Util::LOG_WARNING)
#define ULOGI BOOST_LOG_SEV(Diversia::Util::Log::lg, Diversia::Util::LOG_INFO)
#define ULOGD BOOST_LOG_SEV(Diversia::Util::Log::lg, Diversia::Util::LOG_DEBUG)
#define ULOGDE BOOST_LOG_SEV(Diversia::Util::Log::lg, Diversia::Util::LOG_ENTRYEXIT)
} // Namespace Log
} // Namespace Util
} // Namespace Diversia

#endif // DIVERSIA_UTIL_PREREQUISITES_H