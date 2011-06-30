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
#include "Util/Math/Vector3.h"

namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------

const Vector3 Vector3::ZERO( 0, 0, 0 );
const Vector3 Vector3::UNIT_X( 1, 0, 0 );
const Vector3 Vector3::UNIT_Y( 0, 1, 0 );
const Vector3 Vector3::UNIT_Z( 0, 0, 1 );
const Vector3 Vector3::NEGATIVE_UNIT_X( -1,  0,  0 );
const Vector3 Vector3::NEGATIVE_UNIT_Y(  0, -1,  0 );
const Vector3 Vector3::NEGATIVE_UNIT_Z(  0,  0, -1 );
const Vector3 Vector3::UNIT_SCALE(1, 1, 1);

void Vector3::bindVector3()
{
    camp::Class::declare<Vector3>( "Vector3" )
        .tag( "BindingType", BindingType_Vector3 )
        // Constructors
        .constructor0()
        .constructor3<Real, Real, Real>()
        .constructor1<Real>()
        // Static member variables
        .property( "Zero", &Vector3::getZero )
            .tag( "Static" )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        // Properties (read-only)
        .property( "Length", &Vector3::length )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "SquaredLength", &Vector3::squaredLength )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "IsZeroLength", &Vector3::isZeroLength )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "NormalisedCopy", &Vector3::normalisedCopy )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        // Properties (read/write)
        .property( "X", &Vector3::x )
            .tag( "Configurable" )
            .tag( "QtDoublePrecicion", 4 )
        .property( "Y", &Vector3::y )
            .tag( "Configurable" )
            .tag( "QtDoublePrecicion", 4 )
        .property( "Z", &Vector3::z )
            .tag( "Configurable" )
            .tag( "QtDoublePrecicion", 4 )
        // Functions
        .function( "Distance", &Vector3::distance )
        .function( "SquaredDistance", &Vector3::squaredDistance )
        .function( "Dot", &Vector3::dotProduct )
        .function( "AbsDot", &Vector3::absDotProduct )
        .function( "Normalize", &Vector3::normalise )
        .function( "Cross", &Vector3::crossProduct )
        .function( "MidPoint", &Vector3::midPoint )
        .function( "MakeFloor", &Vector3::makeFloor )
        .function( "MakeCeil", &Vector3::makeCeil )
        .function( "Perpendicular", &Vector3::perpendicular )
        .function( "RandomDeviant", &Vector3::randomDeviant )
        .function( "AngleBetween", &Vector3::angleBetween )
        .function( "GetRotationTo", &Vector3::getRotationTo )
        .function( "Reflect", &Vector3::reflect )
        .function( "PositionEquals", &Vector3::positionEquals )
        .function( "PositionCloses", &Vector3::positionCloses )
        .function( "DirectionEquals", &Vector3::directionEquals )
        // Operators
        .addOperator<camp::add, Vector3, const Vector3&>()
        .addOperator<camp::sub, Vector3, const Vector3&>()
        .addOperator<camp::mul, Vector3, const Vector3&>()
        .addOperator<camp::mul, Vector3, Real>()
        .addOperator<camp::div, Vector3, const Vector3&>()
        .addOperator<camp::div, Vector3, Real>()
        .addOperator<camp::umin, Vector3>()
        .addOperator<camp::eq, const Vector3&>()
        .addOperator<camp::neq, const Vector3&>()
        .addOperator<camp::lt, const Vector3&>()
        .addOperator<camp::lte, const Vector3&>()
        .addOperator<camp::gt, const Vector3&>()
        .addOperator<camp::gte, const Vector3&>();
        // Static functions
}

//------------------------------------------------------------------------------
} // namespace Util
} // namespace Diversia
