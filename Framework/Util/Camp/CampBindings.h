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

#ifndef DIVERSIA_UTIL_CAMPBINDINGS_H
#define DIVERSIA_UTIL_CAMPBINDINGS_H

#include "Util/Platform/Prerequisites.h"

#include "Util/Camp/ValueMapper.h"

namespace Diversia
{
namespace Util
{

enum BindingType
{
    BindingType_unknown,
    BindingType_bool,
    BindingType_short,
    BindingType_unsigned_Short,
    BindingType_int,
    BindingType_unsigned_int,
    BindingType_long, 
    BindingType_unsigned_long,
    BindingType_float,
    BindingType_double,
    BindingType_String,
    BindingType_Vector2, 
    BindingType_Vector3,
    BindingType_Vector4,
    BindingType_Colour,
    BindingType_Quaternion,
    BindingType_Matrix3,
    BindingType_Matrix4,
    BindingType_Radian,
    BindingType_Degree,
    BindingType_Angle,
    BindingType_ResourceInfo
};

BindingType DIVERSIA_UTIL_API getBindingType( const camp::Value& rValue );

namespace Bindings
{
//------------------------------------------------------------------------------

class DIVERSIA_UTIL_API CampBindings
{
public:
    static void bindBindingType();
    static void bindVector2();
    static void bindVector3();
    static void bindVector4();
    static void bindColour();
    static void bindQuaternion();
    static void bindMatrix3();
    static void bindMatrix4();
    static void bindRadian();
    static void bindDegree();
    static void bindAngle();
    static void bindLogLevel();
    static void bindNode();
    static void bindTransformSpace();

};

//------------------------------------------------------------------------------
} // Namespace Bindings
} // Namespace Util
} // Namespace Diversia

CAMP_AUTO_TYPE( Diversia::Util::BindingType, 
    Diversia::Util::Bindings::CampBindings::bindBindingType )

#endif // DIVERSIA_UTIL_CAMPBINDINGS_H