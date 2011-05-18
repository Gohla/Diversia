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

#ifndef DIVERSIA_UTIL_VALUEMAPPER_H
#define DIVERSIA_UTIL_VALUEMAPPER_H

#include "Util/Platform/Prerequisites.h"

namespace camp_ext
{
    template <>
    struct ValueMapper<Diversia::Util::Path>
    {
        // The corresponding CAMP type is "string"
        static const int type = camp::stringType;

        // Convert from Path to std::string
        static std::string to( const Diversia::Util::Path& rSource )
        {
            return rSource.string();
        }

        // Convert from any type to Path
        template <typename T>
        static Diversia::Util::Path from( const T& rSource )
        {
            return Diversia::Util::Path( ValueMapper<std::string>::from( rSource ) );
        }
    };
}

namespace camp
{
namespace detail
{

/*
 * Specialization of CallHelperImpl for copy return Paths
 */
template <typename C>
struct CallHelperImpl<Diversia::Util::Path, C, true>
{
    template <typename F>
    static Value call(F func, C obj) {return Diversia::Util::Path(func(obj));}

    template <typename F, typename A0>
    static Value call(F func, C obj, A0 a0) {return Diversia::Util::Path(func(obj, a0));}

    template <typename F, typename A0, typename A1>
    static Value call(F func, C obj, A0 a0, A1 a1) {return Diversia::Util::Path(func(obj, a0, a1));}

    template <typename F, typename A0, typename A1, typename A2>
    static Value call(F func, C obj, A0 a0, A1 a1, A2 a2) {return Diversia::Util::Path(func(obj, a0, a1, a2));}

    template <typename F, typename A0, typename A1, typename A2, typename A3>
    static Value call(F func, C obj, A0 a0, A1 a1, A2 a2, A3 a3) {return Diversia::Util::Path(func(obj, a0, a1, a2, a3));}

    template <typename F, typename A0, typename A1, typename A2, typename A3, typename A4>
    static Value call(F func, C obj, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4) {return Diversia::Util::Path(func(obj, a0, a1, a2, a3, a4));}
};

}
}

#endif // DIVERSIA_UTIL_VALUEMAPPER_H