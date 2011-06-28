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

#ifndef DIVERSIA_SYSTEMHEADERS_H
#define DIVERSIA_SYSTEMHEADERS_H

#ifdef __BORLANDC__
    #define __STD_ALGORITHM
#endif

#if defined ( DIVERSIA_GCC_VISIBILITY )
/* Until libstdc++ for gcc 4.2 is released, we have to declare all
 * symbols in libstdc++.so externally visible, otherwise we end up
 * with them marked as hidden by -fvisible=hidden.
 *
 * See http://gcc.gnu.org/bugzilla/show_bug.cgi?id=20218
 */
#   pragma GCC visibility push(default)
#endif

// C includes
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdarg>
#include <cmath>
#include <cctype>

// STL containers
#include <vector>
#include <map>
#include <set>
#include <list>
#include <deque>
#include <queue>
#include <stack>
#include <bitset>

// Note - not in the original STL, but exists in SGI STL and STLport
// For gcc 4.3 see http://gcc.gnu.org/gcc-4.3/changes.html
#if (DIVERSIA_COMPILER == DIVERSIA_COMPILER_GNUC) && !defined(STLPORT)
#   if DIVERSIA_COMP_VER >= 430
#       include <tr1/unordered_map>
#       include <tr1/unordered_set> 
#   else
#       include <ext/hash_map>
#       include <ext/hash_set>
#   endif
#else
#   include <hash_set>
#   include <hash_map>
#endif 

// STL algorithms & functions
#include <algorithm>
#include <functional>
#include <limits>

// C++ Stream
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <memory>
#include <exception>
#include <typeinfo>
#include <limits>

// boost
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/record_ostream.hpp>

// libsigc++
#include <sigc++/trackable.h>
#include <sigc++/signal.h>
#include <sigc++/connection.h>
#include <sigc++/functors/slot.h>
#include <sigc++/functors/ptr_fun.h>
#include <sigc++/functors/mem_fun.h>
#include <sigc++/bind.h>

// CAMP
#include <camp/camptype.hpp>
#include <camp/class.hpp>
#include <camp/enum.hpp>
#include <camp/value.hpp>
#include <camp/userobject.hpp>

#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32
#   undef min
#   undef max
#   if defined( __MINGW32__ )
#       include <unistd.h>
#   endif
#   define WIN32_LEAN_AND_MEAN   
#   include <windows.h>
#   include <tchar.h>
#   include <signal.h>
#endif

#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_LINUX
extern "C" {

#   include <unistd.h>
#   include <dlfcn.h>

}
#endif

#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_APPLE
extern "C" {
#   include <unistd.h>
#   include <sys/param.h>
#   include <CoreFoundation/CoreFoundation.h>
}
#endif

#if defined ( DIVERSIA_GCC_VISIBILITY )
#   pragma GCC visibility pop
#endif

#endif // DIVERSIA_SYSTEMHEADERS_H