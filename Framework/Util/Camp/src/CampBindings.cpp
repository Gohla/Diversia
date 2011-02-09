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

#include "Util/Platform/StableHeaders.h"

#include "Util/Camp/CampBindings.h"

#include "Util/Math/Vector2.h"
#include "Util/Math/Vector3.h"
#include "Util/Math/Vector4.h"
#include "Util/Math/Colour.h"
#include "Util/Math/Quaternion.h"
#include "Util/Math/Matrix3.h"
#include "Util/Math/Matrix4.h"
#include "Util/Math/Math.h"
#include "Util/Math/Node.h"

namespace Diversia
{
namespace Util 
{

BindingType getBindingType( const camp::Value& rValue )
{
    switch( rValue.type() )
    {
        case camp::noType:
            return BindingType_unknown;
        case camp::boolType:
            return BindingType_bool;
        case camp::intType:
            return BindingType_int;
        case camp::realType:
            return BindingType_float;
        case camp::stringType:
            return BindingType_String;
        case camp::enumType:
            return BindingType_long;
        case camp::arrayType:
            return BindingType_unknown;
        case camp::userType:
        {
            const camp::Class& metaClass = rValue.to<camp::UserObject>().getClass();
            if( metaClass.hasTag( "BindingType" ) )
                return metaClass.tag( "BindingType" );
            else
                return BindingType_unknown;
        }
        default:
            return BindingType_unknown;
    }

    return BindingType_unknown;
}

namespace Bindings
{
//------------------------------------------------------------------------------

void CampBindings::bindBindingType()
{
    camp::Enum::declare<BindingType>( "BindingType" )
        .value( "bool", BindingType_bool )
        .value( "short", BindingType_short )
        .value( "unsigned_Short", BindingType_unsigned_Short )
        .value( "int", BindingType_int )
        .value( "unsigned_int", BindingType_unsigned_int )
        .value( "long", BindingType_long ) 
        .value( "unsigned_long", BindingType_unsigned_long )
        .value( "float", BindingType_float )
        .value( "double", BindingType_double )
        .value( "String", BindingType_String )
        .value( "Vector2", BindingType_Vector2 ) 
        .value( "Vector3", BindingType_Vector3 )
        .value( "Vector4", BindingType_Vector4 )
        .value( "Colour", BindingType_Colour )
        .value( "Quaternion", BindingType_Quaternion )
        .value( "Matrix3", BindingType_Matrix3 )
        .value( "Matrix4", BindingType_Matrix4 )
        .value( "Radian", BindingType_Radian )
        .value( "Degree", BindingType_Degree )
        .value( "Angle", BindingType_Angle )
        .value( "ResourceInfo", BindingType_ResourceInfo );
}

void CampBindings::bindVector2()
{
    camp::Class::declare<Vector2>( "Vector2" )
        .tag( "BindingType", BindingType_Vector2 )
        // Constructors
        .constructor0()
        .constructor2<Real, Real>()
        .constructor1<Real>()
        // Member variables
        .property( "X", &Vector2::x )
            .tag( "Configurable" )
            .tag( "QtDoublePrecicion", 4 )
        .property( "Y", &Vector2::y )
            .tag( "Configurable" )
            .tag( "QtDoublePrecicion", 4 )
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
        // Functions
        .function( "Dot", &Vector2::dotProduct )
        .function( "Normalize", &Vector2::normalise )
        .function( "MidPoint", &Vector2::midPoint )
        .function( "MakeFloor", &Vector2::makeFloor )
        .function( "MakeCeil", &Vector2::makeCeil )
        .function( "Perpendicular", &Vector2::perpendicular )
        .function( "Cross", &Vector2::crossProduct )
        .function( "RandomDeviant", &Vector2::randomDeviant )
        .function( "Reflect", &Vector2::reflect );
        // Static functions
        // Operators
}

void CampBindings::bindVector3()
{
    camp::Class::declare<Vector3>( "Vector3" )
        .tag( "BindingType", BindingType_Vector3 )
        .tag( "EnableAddOperator" )
        .tag( "EnableSubtractOperator" )
        .tag( "EnableMultiplyOperator" )
        .tag( "EnableDivideOperator" )
        .tag( "EnableUnaryMinusOperator" )
        .tag( "EnableEqualsOperator" )
        .tag( "EnableLessOperator" )
        .tag( "EnableLessEqualsOperator" )
        // Constructors
        .constructor0()
        .constructor3<Real, Real, Real>()
        .constructor1<Real>()
        // Member variables
        .property( "X", &Vector3::x )
            .tag( "Configurable" )
            .tag( "QtDoublePrecicion", 4 )
        .property( "Y", &Vector3::y )
            .tag( "Configurable" )
            .tag( "QtDoublePrecicion", 4 )
        .property( "Z", &Vector3::z )
            .tag( "Configurable" )
            .tag( "QtDoublePrecicion", 4 )
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
        .function( "AddVector3", (Vector3(Vector3::*)(const Vector3&)const)&Vector3::operator+ )
        .function( "SubtractVector3", (Vector3(Vector3::*)(const Vector3&)const)&Vector3::operator- )
        .function( "MultiplyVector3", (Vector3(Vector3::*)(const Vector3&)const)&Vector3::operator* )
        .function( "MultiplyReal", (Vector3(Vector3::*)(Real)const)&Vector3::operator* )
        .function( "DivideVector3", (Vector3(Vector3::*)(const Vector3&)const)&Vector3::operator/ )
        .function( "DivideReal", (Vector3(Vector3::*)(Real)const)&Vector3::operator/ )
        .function( "UnaryMinus", (Vector3(Vector3::*)()const)&Vector3::operator-)
        .function( "EqualsVector3", (bool(Vector3::*)(const Vector3&)const)&Vector3::operator== )
        .function( "LessVector3", (bool(Vector3::*)(const Vector3&)const)&Vector3::operator< )
        .function( "LessEqualsVector3", (bool(Vector3::*)(const Vector3&)const)&Vector3::operator<= );
        // Static functions
        // Operators
}

void CampBindings::bindVector4()
{
    camp::Class::declare<Vector4>( "Vector4" )
        .tag( "BindingType", BindingType_Vector4 )
        // Constructors
        .constructor0()
        .constructor4<Real, Real, Real, Real>()
        .constructor1<Real>()
        // Member variables
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
        // Static member variables
        // Properties (read-only)
        // Properties (read/write)
        // Functions
        .function( "Dot", &Vector4::dotProduct );
        // Static functions
        // Operators
}

void CampBindings::bindColour()
{
    camp::Class::declare<Colour>( "Colour" )
        .tag( "BindingType", BindingType_Colour )
        // Constructors
        .constructor0()
        .constructor3<float, float, float>()
        .constructor4<float, float, float, float>()
        // Member variables
        .property( "R", &Colour::r )
            .tag( "Configurable" )
            .tag( "QtDoublePrecicion", 4 )
        .property( "G", &Colour::g )
            .tag( "Configurable" )
            .tag( "QtDoublePrecicion", 4 )
        .property( "B", &Colour::b )
            .tag( "Configurable" )
            .tag( "QtDoublePrecicion", 4 )
        .property( "A", &Colour::a )
            .tag( "Configurable" )
            .tag( "QtDoublePrecicion", 4 );
        // Static member variables
        // Properties (read-only)
        // Properties (read/write)
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindQuaternion()
{
    camp::Class::declare<Quaternion>( "Quaternion" )
        .tag( "BindingType", BindingType_Quaternion )
        .tag( "EnableAddOperator" )
        .tag( "EnableSubtractOperator" )
        .tag( "EnableMultiplyOperator" )
        .tag( "EnableUnaryMinusOperator" )
        .tag( "EnableEqualsOperator" )
        // Constructors
        .constructor0()
        .constructor4<Real, Real, Real, Real>()
        .constructor2<const Radian&, const Vector3&>()
        .constructor3<const Vector3&, const Vector3&, const Vector3&>()
        // Member variables
        .property( "X", &Quaternion::x )
            .tag( "Configurable" )
            .tag( "QtDoublePrecicion", 4 )
        .property( "Y", &Quaternion::y )
            .tag( "Configurable" )
            .tag( "QtDoublePrecicion", 4 )
        .property( "Z", &Quaternion::z )
            .tag( "Configurable" )
            .tag( "QtDoublePrecicion", 4 )
        .property( "W", &Quaternion::w )
            .tag( "Configurable" )
            .tag( "QtDoublePrecicion", 4 )
        // Static member variables
        // Properties (read-only)
        .property( "XAxis", &Quaternion::xAxis )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "YAxis", &Quaternion::yAxis )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "ZAxis", &Quaternion::zAxis )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "NormalizedCopy", &Quaternion::Norm )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "Inverse", &Quaternion::Inverse )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "UnitInverse", &Quaternion::UnitInverse )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "Exp", &Quaternion::Exp )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "Log", &Quaternion::Log )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "IsNaN", &Quaternion::isNaN )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "Roll", boost::bind( &Quaternion::getRoll, _1, true ) )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "ShortestRoll", boost::bind( &Quaternion::getRoll, _1, false ) )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "Pitch", boost::bind( &Quaternion::getPitch, _1, true ) )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "ShortestPitch", boost::bind( &Quaternion::getPitch, _1, false ) )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "Yaw", boost::bind( &Quaternion::getYaw, _1, true ) )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "ShortestYaw", boost::bind( &Quaternion::getYaw, _1, false ) )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        // Properties (read/write)
        // Functions
        .function( "Dot", &Quaternion::Dot )
        .function( "Normalize", &Quaternion::normalise )
        // Operators
        .function( "AddQuaternion", (Quaternion(Quaternion::*)(const Quaternion&)const)&Quaternion::operator+ )
        .function( "SubtractQuaternion", (Quaternion(Quaternion::*)(const Quaternion&)const)&Quaternion::operator- )
        .function( "MultiplyQuaternion", (Quaternion(Quaternion::*)(const Quaternion&)const)&Quaternion::operator* )
        .function( "MultiplyVector3", (Vector3(Quaternion::*)(const Vector3&)const)&Quaternion::operator* )
        .function( "MultiplyReal", (Quaternion(Quaternion::*)(Real)const)&Quaternion::operator* )
        .function( "UnaryMinus", (Quaternion(Quaternion::*)()const)&Quaternion::operator-)
        .function( "EqualsQuaternion", (bool(Quaternion::*)(const Quaternion&)const)&Quaternion::operator== );
        // Static functions
        // Operators
}

void CampBindings::bindMatrix3()
{
    camp::Class::declare<Matrix3>( "Matrix3" )
        .tag( "BindingType", BindingType_Matrix3 )
        // Constructors
        .constructor0();
        // Member variables
        // Static member variables
        // Properties (read-only)
        // Properties (read/write)
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindMatrix4()
{
    camp::Class::declare<Matrix4>( "Matrix4" )
        .tag( "BindingType", BindingType_Matrix4 )
        // Constructors
        .constructor0();
        // Member variables
        // Static member variables
        // Properties (read-only)
        // Properties (read/write)
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindRadian()
{
    camp::Class::declare<Radian>( "Radian" )
        .tag( "BindingType", BindingType_Radian )
        .tag( "EnableAddOperator" )
        .tag( "EnableSubtractOperator" )
        .tag( "EnableMultiplyOperator" )
        .tag( "EnableDivideOperator" )
        .tag( "EnableUnaryMinusOperator" )
        .tag( "EnableEqualsOperator" )
        .tag( "EnableLessOperator" )
        .tag( "EnableLessEqualsOperator" )
    	// Constructors
        .constructor1<Real>()
        .constructor1<const Degree&>()
        .constructor1<const Radian&>()
    	// Properties (read-only)
        .property( "Degrees", &Radian::valueDegrees )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "AngleUnits", &Radian::valueAngleUnits )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
    	// Properties (read/write)
        .property( "Radians", &Radian::mRad )
            .tag( "Configurable" )
    	// Functions
        // Operators
        .function( "AddRadian", (Radian(Radian::*)(const Radian&)const)&Radian::operator+ )
        .function( "AddDegree", (Radian(Radian::*)(const Degree&)const)&Radian::operator+ )
        .function( "SubtractRadian", (Radian(Radian::*)(const Radian&)const)&Radian::operator- )
        .function( "SubtractDegree", (Radian(Radian::*)(const Degree&)const)&Radian::operator- )
        .function( "MultiplyRadian", (Radian(Radian::*)(const Radian&)const)&Radian::operator* )
        .function( "MultiplyReal", (Radian(Radian::*)(Real)const)&Radian::operator* )
        .function( "DivideReal", (Radian(Radian::*)(Real)const)&Radian::operator/ )
        .function( "UnaryMinus", (Radian(Radian::*)()const)&Radian::operator-)
        .function( "EqualsRadian", (bool(Radian::*)(const Radian&)const)&Radian::operator== )
        .function( "LessRadian", (bool(Radian::*)(const Radian&)const)&Radian::operator< )
        .function( "LessEqualsRadian", (bool(Radian::*)(const Radian&)const)&Radian::operator<= );
    	// Static functions
    	// Operators
}

void CampBindings::bindDegree()
{
    camp::Class::declare<Degree>( "Degree" )
        .tag( "BindingType", BindingType_Degree )
        .tag( "EnableAddOperator" )
        .tag( "EnableSubtractOperator" )
        .tag( "EnableMultiplyOperator" )
        .tag( "EnableDivideOperator" )
        .tag( "EnableUnaryMinusOperator" )
        .tag( "EnableEqualsOperator" )
        .tag( "EnableLessOperator" )
        .tag( "EnableLessEqualsOperator" )
    	// Constructors
        .constructor1<Real>()
        .constructor1<const Radian&>()
    	// Properties (read-only)
        .property( "Radians", &Degree::valueRadians )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "AngleUnits", &Degree::valueAngleUnits )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
    	// Properties (read/write)
        .property( "Degrees", &Degree::mDeg )
            .tag( "Configurable" )
    	// Functions
        // Operators
        .function( "AddDegree", (Degree(Degree::*)(const Degree&)const)&Degree::operator+ )
        .function( "AddRadian", (Degree(Degree::*)(const Radian&)const)&Degree::operator+ )
        .function( "SubtractDegree", (Degree(Degree::*)(const Degree&)const)&Degree::operator- )
        .function( "SubtractRadian", (Degree(Degree::*)(const Radian&)const)&Degree::operator- )
        .function( "MultiplyDegree", (Degree(Degree::*)(const Degree&)const)&Degree::operator* )
        .function( "MultiplyReal", (Degree(Degree::*)(Real)const)&Degree::operator* )
        .function( "DivideReal", (Degree(Degree::*)(Real)const)&Degree::operator/ )
        .function( "UnaryMinus", (Degree(Degree::*)()const)&Degree::operator-)
        .function( "EqualsDegree", (bool(Degree::*)(const Degree&)const)&Degree::operator== )
        .function( "LessDegree", (bool(Degree::*)(const Degree&)const)&Degree::operator< )
        .function( "LessEqualsDegree", (bool(Degree::*)(const Degree&)const)&Degree::operator<= );
    	// Static functions
    	// Operators
}

void CampBindings::bindAngle()
{
    camp::Class::declare<Angle>( "Angle" )
        .tag( "BindingType", BindingType_Angle )
    	// Constructors
        .constructor1<Real>()
    	// Properties (read-only)
        .property( "Radian", &Angle::operator Radian )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "Degree", &Angle::operator Degree )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
    	// Properties (read/write)
        .property( "Angle", &Angle::mAngle )
            .tag( "Configurable" );
    	// Functions
    	// Static functions
    	// Operators
}

void CampBindings::bindLogLevel()
{
    camp::Enum::declare<LogLevel>( "LogLevel" )
        .value( "EntryExit", LOG_ENTRYEXIT )
        .value( "Debug", LOG_DEBUG )
        .value( "Info", LOG_INFO )
        .value( "Warning", LOG_WARNING )
        .value( "Error", LOG_ERROR )
        .value( "Critical", LOG_CRITICAL );
}

void CampBindings::bindNode()
{
    camp::Class::declare<Node>( "Node" )
    	// Constructors
    	// Properties (read-only)
        .property( "LocalAxes", &Node::getLocalAxes )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "FullTransform", &Node::_getFullTransform )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "DerivedScale", &Node::_getDerivedScale )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "InitialPosition", &Node::getInitialPosition )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "InitialOrientation", &Node::getInitialOrientation )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "InitialScale", &Node::getInitialScale )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
    	// Properties (read/write)
        .property( "Position", (Vector3&(Node::*)())&Node::getPosition, (void(Node::*)(const Vector3&))&Node::setPosition )
            .tag( "Configurable" )
            .tag( "NoBitStream" )
            .tag( "QtContinuousUpdate" )
        .property( "DerivedPosition", &Node::_getDerivedPosition, &Node::_setDerivedPosition )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "Orientation", (Quaternion&(Node::*)())&Node::getOrientation, (void(Node::*)(Quaternion))&Node::setOrientation )
            .tag( "Configurable" )
            .tag( "NoBitStream" )
            .tag( "QtContinuousUpdate" )
        .property( "DerivedOrientation", &Node::_getDerivedOrientation, &Node::_setDerivedOrientation )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "Scale", (Vector3&(Node::*)())&Node::getScale, (void(Node::*)(const Vector3&))&Node::setScale )
            .tag( "Configurable" )
            .tag( "NoBitStream" )
            .tag( "QtContinuousUpdate" )
        .property( "InheritOrientation", &Node::getInheritOrientation, &Node::setInheritOrientation )
            .tag( "Configurable" )
        .property( "InheritScale", &Node::getInheritScale, &Node::setInheritScale )
            .tag( "Configurable" )
    	// Functions
        .function( "Translate", (void(Node::*)(const Vector3&, Node::TransformSpace))&Node::translate)
        .function( "Roll", &Node::roll )
        .function( "Pitch", &Node::pitch )
        .function( "Yaw", &Node::yaw )
        .function( "Rotate", (void(Node::*)(const Vector3&, const Radian&, Node::TransformSpace))&Node::rotate )
        .function( "SetInitialState", &Node::setInitialState )
        .function( "ResetToInitialState", &Node::resetToInitialState )
        .function( "ConvertWorldToLocalPosition", &Node::convertWorldToLocalPosition )
        .function( "ConvertLocalToWorldPosition", &Node::convertLocalToWorldPosition )
        .function( "ConvertWorldToLocalOrientation", &Node::convertWorldToLocalOrientation )
        .function( "ConvertLocalToWorldOrientation", &Node::convertLocalToWorldOrientation );
    	// Static functions
    	// Operators
}

void CampBindings::bindTransformSpace()
{
    camp::Enum::declare<Node::TransformSpace>( "TransformSpace" )
    	.value( "Local", Node::TS_LOCAL )
        .value( "Parent", Node::TS_PARENT )
        .value( "World", Node::TS_WORLD );
}

//------------------------------------------------------------------------------
} // Namespace Bindings
} // Namespace Util
} // Namespace Diversia