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

#include "Util/Platform/Prerequisites.h"

#define rep(z, n, text) t.insert( t.end(), d ## n );
#define repmap(z, n, text) rContainer.insert( std::make_pair( key, d ## n ) );

#ifndef BOOST_PP_IS_ITERATING
#   ifndef DIVERSIA_UTIL_CONTAINERINITIALIZER_H
#       define DIVERSIA_UTIL_CONTAINERINITIALIZER_H
#       define BOOST_PP_ITERATION_LIMITS ( 1, 20 )
#       define BOOST_PP_FILENAME_1 "Util/Helper/ContainerInitializer.h"
#       include BOOST_PP_ITERATE()
#   endif // DIVERSIA_UTIL_CONTAINERINITIALIZER_H
#else
#   define n BOOST_PP_ITERATION()

    template<class Container>
    Container initializer( BOOST_PP_ENUM_PARAMS(n, typename const Container::value_type& d) )
    {
        Container t;
        BOOST_PP_REPEAT(n, rep, ~)
        return t;
    }

    template<class Container>
    void mapInitializer( Container& rContainer, typename const Container::key_type& key,
        BOOST_PP_ENUM_PARAMS(n, typename const Container::mapped_type& d) )
    {
        BOOST_PP_REPEAT(n, repmap, ~)
    }

#endif

#undef rep
#undef repmap
#undef n