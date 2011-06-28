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

#include "Util/Camp/BindingType.h"

namespace Diversia
{
namespace Util 
{
//------------------------------------------------------------------------------

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

void bindBindingType()
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

//------------------------------------------------------------------------------
} // Namespace Util
} // Namespace Diversia
