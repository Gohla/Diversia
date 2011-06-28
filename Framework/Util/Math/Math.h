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

#ifndef DIVERSIA_UTIL_MATH_H
#define DIVERSIA_UTIL_MATH_H

//------------------------------------------------------------------------------
namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------

class Radian
{
public:
	explicit Radian (Real r = 0) : mRad(r) {}
	Radian(const Degree& d);
	Radian(const Radian& r) : mRad(r.mRad) {}
	Radian& operator = ( const Real& f ) { mRad = f; return *this; }
	Radian& operator = ( const Radian& r ) { mRad = r.mRad; return *this; }
	Radian& operator = ( const Degree& d );

	Real valueDegrees() const; // see bottom of this file
	Real valueRadians() const { return mRad; }
	Real valueAngleUnits() const;

	const Radian& operator + () const { return *this; }
	Radian operator + ( const Radian& r ) const { return Radian ( mRad + r.mRad ); }
	Radian operator + ( const Degree& d ) const;
	Radian& operator += ( const Radian& r ) { mRad += r.mRad; return *this; }
	Radian& operator += ( const Degree& d );
	Radian operator - () const { return Radian(-mRad); }
	Radian operator - ( const Radian& r ) const { return Radian ( mRad - r.mRad ); }
	Radian operator - ( const Degree& d ) const;
	Radian& operator -= ( const Radian& r ) { mRad -= r.mRad; return *this; }
	Radian& operator -= ( const Degree& d );
	Radian operator * ( Real f ) const { return Radian ( mRad * f ); }
	Radian operator * ( const Radian& f ) const { return Radian ( mRad * f.mRad ); }
	Radian& operator *= ( Real f ) { mRad *= f; return *this; }
	Radian operator / ( Real f ) const { return Radian ( mRad / f ); }
	Radian& operator /= ( Real f ) { mRad /= f; return *this; }

	bool operator <  ( const Radian& r ) const { return mRad <  r.mRad; }
	bool operator <= ( const Radian& r ) const { return mRad <= r.mRad; }
	bool operator == ( const Radian& r ) const { return mRad == r.mRad; }
	bool operator != ( const Radian& r ) const { return mRad != r.mRad; }
	bool operator >= ( const Radian& r ) const { return mRad >= r.mRad; }
	bool operator >  ( const Radian& r ) const { return mRad >  r.mRad; }

    static void bindRadian();

private:

	Real mRad;
}; // class Radian

template <typename ToType, typename FromType>
ToType toRadian(const FromType& c)
{
    return ToType(c.valueRadians());
}

//------------------------------------------------------------------------------
/** Wrapper class which indicates a given angle value is in Degrees.
@remarks
	Degree values are interchangeable with Radian values, and conversions
	will be done automatically between them.
*/
class Degree
{
public:
	explicit Degree ( Real d=0 ) : mDeg(d) {}
	Degree ( const Radian& r ) : mDeg(r.valueDegrees()) {}
	Degree& operator = ( const Real& f ) { mDeg = f; return *this; }
	Degree& operator = ( const Degree& d ) { mDeg = d.mDeg; return *this; }
	Degree& operator = ( const Radian& r ) { mDeg = r.valueDegrees(); return *this; }

	Real valueDegrees() const { return mDeg; }
	Real valueRadians() const; // see bottom of this file
	Real valueAngleUnits() const;

	const Degree& operator + () const { return *this; }
	Degree operator + ( const Degree& d ) const { return Degree ( mDeg + d.mDeg ); }
	Degree operator + ( const Radian& r ) const { return Degree ( mDeg + r.valueDegrees() ); }
	Degree& operator += ( const Degree& d ) { mDeg += d.mDeg; return *this; }
	Degree& operator += ( const Radian& r ) { mDeg += r.valueDegrees(); return *this; }
	Degree operator - () const { return Degree(-mDeg); }
	Degree operator - ( const Degree& d ) const { return Degree ( mDeg - d.mDeg ); }
	Degree operator - ( const Radian& r ) const { return Degree ( mDeg - r.valueDegrees() ); }
	Degree& operator -= ( const Degree& d ) { mDeg -= d.mDeg; return *this; }
	Degree& operator -= ( const Radian& r ) { mDeg -= r.valueDegrees(); return *this; }
	Degree operator * ( Real f ) const { return Degree ( mDeg * f ); }
	Degree operator * ( const Degree& f ) const { return Degree ( mDeg * f.mDeg ); }
	Degree& operator *= ( Real f ) { mDeg *= f; return *this; }
	Degree operator / ( Real f ) const { return Degree ( mDeg / f ); }
	Degree& operator /= ( Real f ) { mDeg /= f; return *this; }

	bool operator <  ( const Degree& d ) const { return mDeg <  d.mDeg; }
	bool operator <= ( const Degree& d ) const { return mDeg <= d.mDeg; }
	bool operator == ( const Degree& d ) const { return mDeg == d.mDeg; }
	bool operator != ( const Degree& d ) const { return mDeg != d.mDeg; }
	bool operator >= ( const Degree& d ) const { return mDeg >= d.mDeg; }
	bool operator >  ( const Degree& d ) const { return mDeg >  d.mDeg; }

    static void bindDegree();

private:

	Real mDeg; // if you get an error here - make sure to define/typedef 'Real' first
}; // class Degree

template <typename ToType, typename FromType>
ToType toDegree(const FromType& c)
{
    return ToType(c.valueDegrees());
}

//------------------------------------------------------------------------------
/** Wrapper class which identifies a value as the currently default angle
	type, as defined by Math::setAngleUnit.
@remarks
	Angle values will be automatically converted between radians and degrees,
	as appropriate.
*/
class Angle
{
public:
	explicit Angle ( Real angle=0 ) : mAngle(angle) {}
	operator Radian() const;
	operator Degree() const;

    static void bindAngle();

private:

    Real mAngle;
};
//------------------------------------------------------------------------------
// these functions could not be defined within the class definition of class
// Radian because they required class Degree to be defined
inline Radian::Radian ( const Degree& d ) : mRad(d.valueRadians()) {
}
inline Radian& Radian::operator = ( const Degree& d ) {
	mRad = d.valueRadians(); return *this;
}
inline Radian Radian::operator + ( const Degree& d ) const {
	return Radian ( mRad + d.valueRadians() );
}
inline Radian& Radian::operator += ( const Degree& d ) {
	mRad += d.valueRadians();
	return *this;
}
inline Radian Radian::operator - ( const Degree& d ) const {
	return Radian ( mRad - d.valueRadians() );
}
inline Radian& Radian::operator -= ( const Degree& d ) {
	mRad -= d.valueRadians();
	return *this;
}
//------------------------------------------------------------------------------
class DIVERSIA_UTIL_API Math
{
public:
	/** The angular units used by the API. This functionality is now deprecated in favor
		of discreet angular unit types ( see Degree and Radian above ). The only place
		this functionality is actually still used is when parsing files. Search for
		usage of the Angle class for those instances
	*/
	enum AngleUnit
	{
		AU_DEGREE,
		AU_RADIAN
	};

protected:
	// angle units used by the api
	static AngleUnit msAngleUnit;

public:
	/** Default constructor.
	*/
	Math();

	/** Default destructor.
	*/
	~Math();

    static inline bool isNaN(Real f)
    {
        // std::isnan() is C99, not supported by all compilers
        // However NaN always fails this next test, no other number does.
        return f != f;
    }

	static inline int IAbs (int iValue) { return ( iValue >= 0 ? iValue : -iValue ); }
	static inline int ICeil (float fValue) { return int(ceil(fValue)); }
	static inline int IFloor (float fValue) { return int(floor(fValue)); }
	static int ISign (int iValue);

	static inline Real Abs(Real fValue) { return Real(fabs(fValue)); }
	static inline Degree Abs (const Degree& dValue) { return Degree(fabs(dValue.valueDegrees())); }
	static inline Radian Abs (const Radian& rValue) { return Radian(fabs(rValue.valueRadians())); }
	static Radian ACos(Real fValue);
	static Radian ASin(Real fValue);
	static inline Radian ATan(Real fValue) { return Radian(atan(fValue)); }
	static inline Radian ATan2(Real fY, Real fX) { return Radian(atan2(fY,fX)); }
	static inline Real Ceil(Real fValue) { return Real(ceil(fValue)); }

	/** Cosine function.
		@param
			fValue Angle in radians
	*/
	static inline Real Cos(Radian& fValue) { return Real(cos(fValue.valueRadians())); }
	/** Cosine function.
		@param
			fValue Angle in radians
	*/
	static inline Real Cos(Real fValue) { return Real(cos(fValue)); }

	static inline Real Exp(Real fValue) { return Real(exp(fValue)); }

	static inline Real Floor(Real fValue) { return Real(floor(fValue)); }

	static inline Real Log(Real fValue) { return Real(log(fValue)); }

	static inline Real Pow(Real fBase, Real fExponent) { return Real(pow(fBase,fExponent)); }

	static Real Sign(Real fValue);
	static inline Radian Sign ( const Radian& rValue )
	{
		return Radian(Sign(rValue.valueRadians()));
	}
	static inline Degree Sign ( const Degree& dValue )
	{
		return Degree(Sign(dValue.valueDegrees()));
	}

	/** Sine function.
		@param
			fValue Angle in radians
	*/
	static inline Real Sin(Radian& fValue) { return Real(sin(fValue.valueRadians())); }
	/** Sine function.
		@param
			fValue Angle in radians
	*/
	static inline Real Sin(Real fValue) { return Real(sin(fValue)); }

	static inline Real Sqr(Real fValue) { return Real(fValue*fValue); }

	static inline Real Sqrt(Real fValue) { return Real(sqrt(fValue)); }

	static inline Radian Sqrt(const Radian& fValue) { return Radian(sqrt(fValue.valueRadians())); }

	static inline Degree Sqrt(const Degree& fValue) { return Degree(sqrt(fValue.valueDegrees())); }

	/** Inverse square root i.e. 1 / Sqrt(x), good for vector
		normalisation.
	*/
	static Real InvSqrt(Real fValue);

	static Real UnitRandom(); // in [0,1]

	static Real RangeRandom(Real fLow, Real fHigh); // in [fLow,fHigh]

	static Real SymmetricRandom();  // in [-1,1]

	/** Tangent function.
		@param
			fValue Angle in radians
	*/
	static inline Real Tan(const Radian& fValue) { return Real(tan(fValue.valueRadians())); }
	/** Tangent function.
		@param
			fValue Angle in radians
	*/
	static inline Real Tan(Real fValue) { return Real(tan(fValue)); }

	static Real inline DegreesToRadians(Real degrees) { return degrees * fDeg2Rad; }
	static Real inline RadiansToDegrees(Real radians) { return radians * fRad2Deg; }

	/** These functions used to set the assumed angle units (radians or degrees)
	     expected when using the Angle type.
	@par
	     You can set this directly after creating a new Root, and also before/after resource creation,
	     depending on whether you want the change to affect resource files.
	*/
	static void setAngleUnit(AngleUnit unit);
	/** Get the unit being used for angles. */
	static AngleUnit getAngleUnit(void);

	/** Convert from the current AngleUnit to radians. */
	static Real AngleUnitsToRadians(Real units);
	/** Convert from radians to the current AngleUnit . */
	static Real RadiansToAngleUnits(Real radians);
	/** Convert from the current AngleUnit to degrees. */
	static Real AngleUnitsToDegrees(Real units);
	/** Convert from degrees to the current AngleUnit. */
	static Real DegreesToAngleUnits(Real degrees);

	/** Checks whether a given point is inside a triangle, in a
            2-dimensional (Cartesian) space.
            @remarks
                The vertices of the triangle must be given in either
                trigonometrical (anticlockwise) or inverse trigonometrical
                (clockwise) order.
            @param
                p The point.
            @param
                a The triangle's first vertex.
            @param
                b The triangle's second vertex.
            @param
                c The triangle's third vertex.
            @returns
                If the point resides in the triangle, <b>true</b> is
                returned.
            @par
                If the point is outside the triangle, <b>false</b> is
                returned.
        */
        static bool pointInTri2D(const Vector2& p, const Vector2& a,
			const Vector2& b, const Vector2& c);

       /** Checks whether a given 3D point is inside a triangle.
       @remarks
            The vertices of the triangle must be given in either
            trigonometrical (anticlockwise) or inverse trigonometrical
            (clockwise) order, and the point must be guaranteed to be in the
			same plane as the triangle
        @param
            p The point.
        @param
            a The triangle's first vertex.
        @param
            b The triangle's second vertex.
        @param
            c The triangle's third vertex.
		@param
			normal The triangle plane's normal (passed in rather than calculated
				on demand since the caller may already have it)
        @returns
            If the point resides in the triangle, <b>true</b> is
            returned.
        @par
            If the point is outside the triangle, <b>false</b> is
            returned.
        */
       static bool pointInTri3D(const Vector3& p, const Vector3& a,
		const Vector3& b, const Vector3& c, const Vector3& normal);

	/** Compare 2 reals, using tolerance for inaccuracies.
	*/
	static bool RealEqual(Real a, Real b,
		Real tolerance = std::numeric_limits<Real>::epsilon());

	/** Clamp a value within an inclusive range. */
	template <typename T>
	static T Clamp(T val, T minval, T maxval)
	{
		assert(minval < maxval && "Invalid clamp range");
		return std::max(std::min(val, maxval), minval);
	}

	static const Real POS_INFINITY;
	static const Real NEG_INFINITY;
	static const Real PI;
	static const Real TWO_PI;
	static const Real HALF_PI;
	static const Real fDeg2Rad;
	static const Real fRad2Deg;
};
//------------------------------------------------------------------------------
// these functions must be defined down here, because they rely on the
// angle unit conversion functions in class Math:
//------------------------------------------------------------------------------
inline Real Radian::valueDegrees() const
{
	return Math::RadiansToDegrees(mRad);
}
//------------------------------------------------------------------------------
inline Real Radian::valueAngleUnits() const
{
	return Math::RadiansToAngleUnits(mRad);
}
//------------------------------------------------------------------------------
inline Real Degree::valueRadians() const
{
	return Math::DegreesToRadians(mDeg);
}
//------------------------------------------------------------------------------
inline Real Degree::valueAngleUnits() const
{
	return Math::DegreesToAngleUnits(mDeg);
}
//------------------------------------------------------------------------------
inline Angle::operator Radian() const
{
	return Radian(Math::AngleUnitsToRadians(mAngle));
}
//------------------------------------------------------------------------------
inline Angle::operator Degree() const
{
	return Degree(Math::AngleUnitsToDegrees(mAngle));
}
//------------------------------------------------------------------------------
inline Radian operator * (Real a, const Radian& b)
{
	return Radian (a * b.valueRadians());
}
//------------------------------------------------------------------------------
inline Radian operator / (Real a, const Radian& b)
{
	return Radian (a / b.valueRadians());
}
//------------------------------------------------------------------------------
inline Degree operator * (Real a, const Degree& b)
{
	return Degree (a * b.valueDegrees());
}
//------------------------------------------------------------------------------
inline Degree operator / (Real a, const Degree& b)
{
	return Degree (a / b.valueDegrees());
}
//------------------------------------------------------------------------------
} // namespace Util
} // namespace Diversia
//------------------------------------------------------------------------------

CAMP_AUTO_TYPE( Diversia::Util::Radian, &Diversia::Util::Radian::bindRadian );
CAMP_AUTO_TYPE( Diversia::Util::Degree, &Diversia::Util::Degree::bindDegree );
CAMP_AUTO_TYPE( Diversia::Util::Angle, &Diversia::Util::Angle::bindAngle );

#endif // DIVERSIA_UTIL_MATH_H