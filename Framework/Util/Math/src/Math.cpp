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

#include "Util/Math/Math.h"

#include "Util/Math/AsmMath.h"
#include "Util/Math/Vector2.h"
#include "Util/Math/Vector3.h"
#include "Util/Camp/BindingType.h"

//------------------------------------------------------------------------------
namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------
const Real Math::POS_INFINITY = std::numeric_limits<Real>::infinity();
const Real Math::NEG_INFINITY = -std::numeric_limits<Real>::infinity();
const Real Math::PI = Real(4.0 * atan(1.0));
const Real Math::TWO_PI = Real(2.0 * PI);
const Real Math::HALF_PI = Real(0.5 * PI);
const Real Math::fDeg2Rad = PI / Real(180.0);
const Real Math::fRad2Deg = Real(180.0) / PI;

Math::AngleUnit Math::msAngleUnit;
//------------------------------------------------------------------------------
Math::Math()
{
	msAngleUnit = AU_DEGREE;
}
//------------------------------------------------------------------------------
Math::~Math()
{
}
//------------------------------------------------------------------------------
int Math::ISign (int iValue)
{
	return ( iValue > 0 ? +1 : ( iValue < 0 ? -1 : 0 ) );
}
//------------------------------------------------------------------------------
Radian Math::ACos (Real fValue)
{
	if ( -1.0 < fValue )
	{
		if ( fValue < 1.0 )
			return Radian(acos(fValue));
		else
			return Radian(0.0);
	}
	else
	{
		return Radian(PI);
	}
}
//------------------------------------------------------------------------------
Radian Math::ASin (Real fValue)
{
	if ( -1.0 < fValue )
	{
		if ( fValue < 1.0 )
			return Radian(asin(fValue));
		else
			return Radian(HALF_PI);
	}
	else
	{
		return Radian(-HALF_PI);
	}
}
//------------------------------------------------------------------------------
Real Math::Sign(Real fValue)
{
	if (fValue > 0.0)
		return 1.0;

	if (fValue < 0.0)
		return -1.0;

	return 0.0;
}
//------------------------------------------------------------------------------
Real Math::InvSqrt(Real fValue)
{
	return Real(asm_rsq(fValue));
}
//------------------------------------------------------------------------------
Real Math::UnitRandom()
{
	//return rand();
	return asm_rand() / asm_rand_max();
}
//------------------------------------------------------------------------------
Real Math::RangeRandom(Real fLow, Real fHigh)
{
	return (fHigh-fLow)*UnitRandom() + fLow;
}
//------------------------------------------------------------------------------
Real Math::SymmetricRandom()
{
	return 2.0f * UnitRandom() - 1.0f;
}
void Math::setAngleUnit(Math::AngleUnit unit)
{
	 msAngleUnit = unit;
}
//------------------------------------------------------------------------------
Math::AngleUnit Math::getAngleUnit(void)
{
	 return msAngleUnit;
}
//------------------------------------------------------------------------------
Real Math::AngleUnitsToRadians(Real angleunits)
{
	if (msAngleUnit == AU_DEGREE)
		 return angleunits * fDeg2Rad;
	else
		 return angleunits;
}
//------------------------------------------------------------------------------
Real Math::RadiansToAngleUnits(Real radians)
{
	if (msAngleUnit == AU_DEGREE)
		 return radians * fRad2Deg;
	else
		 return radians;
}
//------------------------------------------------------------------------------
Real Math::AngleUnitsToDegrees(Real angleunits)
{
	if (msAngleUnit == AU_RADIAN)
		 return angleunits * fRad2Deg;
	else
		 return angleunits;
}
//------------------------------------------------------------------------------
Real Math::DegreesToAngleUnits(Real degrees)
{
	if (msAngleUnit == AU_RADIAN)
		 return degrees * fDeg2Rad;
	else
		 return degrees;
}
//------------------------------------------------------------------------------
bool Math::pointInTri2D(const Vector2& p, const Vector2& a, 
		const Vector2& b, const Vector2& c)
{
	// Winding must be consistent from all edges for point to be inside
	Vector2 v1, v2;
	Real dot[3];
	bool zeroDot[3];

	v1 = b - a;
	v2 = p - a;

	// Note we don't care about normalisation here since sign is all we need
	// It means we don't have to worry about magnitude of cross products either
	dot[0] = v1.crossProduct(v2);
	zeroDot[0] = Math::RealEqual(dot[0], 0.0f, 1e-3);


	v1 = c - b;
	v2 = p - b;

	dot[1] = v1.crossProduct(v2);
	zeroDot[1] = Math::RealEqual(dot[1], 0.0f, 1e-3);

	// Compare signs (ignore colinear / coincident points)
	if(!zeroDot[0] && !zeroDot[1] 
	&& Math::Sign(dot[0]) != Math::Sign(dot[1]))
	{
		return false;
	}

	v1 = a - c;
	v2 = p - c;

	dot[2] = v1.crossProduct(v2);
	zeroDot[2] = Math::RealEqual(dot[2], 0.0f, 1e-3);
	// Compare signs (ignore colinear / coincident points)
	if((!zeroDot[0] && !zeroDot[2] 
		&& Math::Sign(dot[0]) != Math::Sign(dot[2])) ||
		(!zeroDot[1] && !zeroDot[2] 
		&& Math::Sign(dot[1]) != Math::Sign(dot[2])))
	{
		return false;
	}

	return true;
}
//-----------------------------------------------------------------------
bool Math::pointInTri3D(const Vector3& p, const Vector3& a, 
	const Vector3& b, const Vector3& c, const Vector3& normal)
{
	// Winding must be consistent from all edges for point to be inside
	Vector3 v1, v2;
	Real dot[3];
	bool zeroDot[3];

	v1 = b - a;
	v2 = p - a;

	// Note we don't care about normalisation here since sign is all we need
	// It means we don't have to worry about magnitude of cross products either
	dot[0] = v1.crossProduct(v2).dotProduct(normal);
	zeroDot[0] = Math::RealEqual(dot[0], 0.0f, 1e-3);


	v1 = c - b;
	v2 = p - b;

	dot[1] = v1.crossProduct(v2).dotProduct(normal);
	zeroDot[1] = Math::RealEqual(dot[1], 0.0f, 1e-3);

	// Compare signs (ignore colinear / coincident points)
	if(!zeroDot[0] && !zeroDot[1] 
		&& Math::Sign(dot[0]) != Math::Sign(dot[1]))
	{
		return false;
	}

	v1 = a - c;
	v2 = p - c;

	dot[2] = v1.crossProduct(v2).dotProduct(normal);
	zeroDot[2] = Math::RealEqual(dot[2], 0.0f, 1e-3);
	// Compare signs (ignore colinear / coincident points)
	if((!zeroDot[0] && !zeroDot[2] 
		&& Math::Sign(dot[0]) != Math::Sign(dot[2])) ||
		(!zeroDot[1] && !zeroDot[2] 
		&& Math::Sign(dot[1]) != Math::Sign(dot[2])))
	{
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------
bool Math::RealEqual(Real a, Real b, Real tolerance)
{
	if (fabs(b-a) <= tolerance)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------

void Radian::bindRadian()
{
    camp::Class::declare<Radian>( "Radian" )
        .tag( "BindingType", BindingType_Radian )
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
        .addOperator<camp::add, Radian, const Radian&>()
        .addOperator<camp::add, Radian, const Degree&>()
        .addOperator<camp::sub, Radian, const Radian&>()
        .addOperator<camp::sub, Radian, const Degree&>()
        .addOperator<camp::mul, Radian, const Radian&>()
        .addOperator<camp::mul, Radian, Real>()
        .addOperator<camp::div, Radian, Real>()
        .addOperator<camp::umin, Radian>()
        .addOperator<camp::eq, const Radian&>()
        .addOperator<camp::neq, const Radian&>()
        .addOperator<camp::lt, const Radian&>()
        .addOperator<camp::lte, const Radian&>()
        .addOperator<camp::gt, const Radian&>()
        .addOperator<camp::gte, const Radian&>();
        // Static functions
}

void Degree::bindDegree()
{
    camp::Class::declare<Degree>( "Degree" )
        .tag( "BindingType", BindingType_Degree )
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
        .addOperator<camp::add, Degree, const Degree&>()
        .addOperator<camp::add, Degree, const Radian&>()
        .addOperator<camp::sub, Degree, const Degree&>()
        .addOperator<camp::sub, Degree, const Radian&>()
        .addOperator<camp::mul, Degree, const Degree&>()
        .addOperator<camp::mul, Degree, Real>()
        .addOperator<camp::div, Degree, Real>()
        .addOperator<camp::umin, Degree>()
        .addOperator<camp::eq, const Degree&>()
        .addOperator<camp::neq, const Degree&>()
        .addOperator<camp::lt, const Degree&>()
        .addOperator<camp::lte, const Degree&>()
        .addOperator<camp::gt, const Degree&>()
        .addOperator<camp::gte, const Degree&>();
        // Static functions
}

void Angle::bindAngle()
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

//------------------------------------------------------------------------------
} // namespace Util
} // namespace Diversia
