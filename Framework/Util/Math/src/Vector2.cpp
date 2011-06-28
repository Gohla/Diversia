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
#include "Util/Math/Vector2.h"

namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------

const Vector2 Vector2::ZERO( 0, 0);
const Vector2 Vector2::UNIT_X( 1, 0);
const Vector2 Vector2::UNIT_Y( 0, 1);
const Vector2 Vector2::NEGATIVE_UNIT_X( -1,  0);
const Vector2 Vector2::NEGATIVE_UNIT_Y(  0, -1);
const Vector2 Vector2::UNIT_SCALE(1, 1);

void Vector2::bindVector2()
{
    camp::Class::declare<Vector2>( "Vector2" )
        .tag( "BindingType", BindingType_Vector2 )
        // Constructors
        .constructor0()
        .constructor2<Real, Real>()
        .constructor1<Real>()
        // Static member variables
        // Properties (read-only)
        .property( "Length", &Vector2::length )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "SquaredLength", &Vector2::squaredLength )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "IsZeroLength", &Vector2::isZeroLength )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "NormalisedCopy", &Vector2::normalisedCopy )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        // Properties (read/write)
        .property( "X", &Vector2::x )
            .tag( "Configurable" )
            .tag( "QtDoublePrecicion", 4 )
        .property( "Y", &Vector2::y )
            .tag( "Configurable" )
            .tag( "QtDoublePrecicion", 4 )
        // Functions
        .function( "Dot", &Vector2::dotProduct )
        .function( "Normalize", &Vector2::normalise )
        .function( "MidPoint", &Vector2::midPoint )
        .function( "MakeFloor", &Vector2::makeFloor )
        .function( "MakeCeil", &Vector2::makeCeil )
        .function( "Perpendicular", &Vector2::perpendicular )
        .function( "Cross", &Vector2::crossProduct )
        .function( "RandomDeviant", &Vector2::randomDeviant )
        .function( "Reflect", &Vector2::reflect )
        // Operators
        .addOperator<camp::add, Vector2, const Vector2&>()
        .addOperator<camp::sub, Vector2, const Vector2&>()
        .addOperator<camp::mul, Vector2, const Vector2&>()
        .addOperator<camp::mul, Vector2, Real>()
        .addOperator<camp::div, Vector2, const Vector2&>()
        .addOperator<camp::div, Vector2, Real>()
        .addOperator<camp::umin, Vector2>()
        .addOperator<camp::eq, const Vector2&>()
        .addOperator<camp::neq, const Vector2&>()
        .addOperator<camp::lt, const Vector2&>()
        .addOperator<camp::lte, const Vector2&>()
        .addOperator<camp::gt, const Vector2&>()
        .addOperator<camp::gte, const Vector2&>();
        // Static functions
}

//------------------------------------------------------------------------------
} // namespace Util
} // namespace Diversia
