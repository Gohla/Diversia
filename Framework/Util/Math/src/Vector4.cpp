/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

Original file used from the OGRE project, license:
-----------------------------------------------------------------------------
Copyright (c) 2000-2009 Torus Knot Software Ltd

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

#include "Util/Platform/StableHeaders.h"

#include "Util/Camp/BindingType.h"
#include "Util/Math/Vector4.h"

namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------

const Vector4 Vector4::ZERO( 0, 0, 0, 0 );

void Vector4::bindVector4()
{
    camp::Class::declare<Vector4>( "Vector4" )
        .tag( "BindingType", BindingType_Vector4 )
        // Constructors
        .constructor0()
        .constructor4<Real, Real, Real, Real>()
        .constructor1<Real>()
        // Static member variables
        // Properties (read-only)
        // Properties (read/write)
        .property( "X", &Vector4::x )
            .tag( "Configurable" )
            .tag( "QtDoublePrecicion", 4 )
        .property( "Y", &Vector4::y )
            .tag( "Configurable" )
            .tag( "QtDoublePrecicion", 4 )
        .property( "Z", &Vector4::z )
            .tag( "Configurable" )
            .tag( "QtDoublePrecicion", 4 )
        .property( "W", &Vector4::w )
            .tag( "Configurable" )
            .tag( "QtDoublePrecicion", 4 )
        // Functions
        .function( "Dot", &Vector4::dotProduct )
        // Operators
        .addOperator<camp::add, Vector4, const Vector4&>()
        .addOperator<camp::sub, Vector4, const Vector4&>()
        .addOperator<camp::mul, Vector4, const Vector4&>()
        .addOperator<camp::mul, Vector4, Real>()
        .addOperator<camp::div, Vector4, const Vector4&>()
        .addOperator<camp::div, Vector4, Real>()
        .addOperator<camp::umin, Vector4>()
        .addOperator<camp::eq, const Vector4&>()
        .addOperator<camp::neq, const Vector4&>()
        .addOperator<camp::lt, const Vector4&>()
        .addOperator<camp::lte, const Vector4&>()
        .addOperator<camp::gt, const Vector4&>()
        .addOperator<camp::gte, const Vector4&>();
        // Static functions
}

//------------------------------------------------------------------------------
} // namespace Util
} // namespace Diversia
