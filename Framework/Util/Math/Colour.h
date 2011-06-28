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

#ifndef DIVERSIA_UTIL_COLOUR_H
#define DIVERSIA_UTIL_COLOUR_H

//------------------------------------------------------------------------------
namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------
typedef uint32 RGBA;
typedef uint32 ARGB;
typedef uint32 ABGR;
typedef uint32 BGRA;
//------------------------------------------------------------------------------
/**
 * Colour class.
 *
 * Colour is represented as 4 components, each of which is a floating-point
 *  value from 0.0 to 1.0.
 *
 * The 3 'normal' colour components are red, green and blue, a higher
 *  number indicating greater amounts of that component in the colour.
 *  The forth component is the 'alpha' value, which represents
 *  transparency. In this case, 0.0 is completely transparent and 1.0 is
 *  fully opaque.
 */
class DIVERSIA_UTIL_API Colour
{
public:
	static const Colour ZERO;
	static const Colour AliceBlue;		// R:240 G:248 B:255
	static const Colour AntiqueWhite;	// R:250 G:235 B:215
	static const Colour Beige;			// R:245 G:245 B:220
	static const Colour Black;			// R:0   G:0   B:0
	static const Colour Blue;			// R:0   G:0   B:255
	static const Colour CornflowerBlue;	// R:100 G:149 B:237
	static const Colour Green;			// R:0   G:128 B:0
	static const Colour Grey;			// R:128 G:128 B:128
	static const Colour Lime;			// R:255 G:0   B:255
	static const Colour Orange;			// R:255 G:165 B:0
	static const Colour Red;			// R:255 G:0   B:0
	static const Colour SteelBlue;		// R:70  G:130 B:180
	static const Colour Violet;			// R:238 G:130 B:238
	static const Colour White;			// R:255 G:255 B:255
	static const Colour Yellow;			// R:255 G:255 B:0

	explicit Colour(float red = 1.0f,
					float green = 1.0f,
					float blue = 1.0f,
					float alpha = 1.0f ) : r(red), g(green), b(blue), a(alpha)
	{}

	bool operator==(const Colour& rhs) const;
	bool operator!=(const Colour& rhs) const;

	inline bool operator < ( const Colour& rhs ) const
	{
		if( r < rhs.r && g < rhs.g && b < rhs.b && a < rhs.a ) return true;
		return false;
	}
	inline bool operator > ( const Colour& rhs ) const
	{
		if( r > rhs.r && g > rhs.g && b > rhs.b && a > rhs.a ) return true;
		return false;
	}
	inline bool operator <= ( const Colour& rhs ) const
	{
		if( r <= rhs.r && g <= rhs.g && b <= rhs.b && a <= rhs.a ) return true;
		return false;
	}
	inline bool operator >= ( const Colour& rhs ) const
	{
		if( r >= rhs.r && g >= rhs.g && b >= rhs.b && a >= rhs.a ) return true;
		return false;
	}

	float r,g,b,a;

	/** Retrieves colour as RGBA.
	*/
	RGBA getAsRGBA(void) const;

	/** Retrieves colour as ARGB.
	*/
	ARGB getAsARGB(void) const;

	/** Retrieves colour as BGRA.
	*/
	BGRA getAsBGRA(void) const;

	/** Retrieves colours as ABGR */
	ABGR getAsABGR(void) const;

	/** Sets colour as RGBA.
	*/
	void setAsRGBA(const RGBA val);

	/** Sets colour as ARGB.
	*/
	void setAsARGB(const ARGB val);

	/** Sets colour as BGRA.
	*/
	void setAsBGRA(const BGRA val);

	/** Sets colour as ABGR.
	*/
	void setAsABGR(const ABGR val);

	/** Clamps colour value to the range [0, 1].
	*/
	void saturate(void)
	{
		if (r < 0)
			r = 0;
		else if (r > 1)
			r = 1;

		if (g < 0)
			g = 0;
		else if (g > 1)
			g = 1;

		if (b < 0)
			b = 0;
		else if (b > 1)
			b = 1;

		if (a < 0)
			a = 0;
		else if (a > 1)
			a = 1;
	}

	/** As saturate, except that this colour value is unaffected and
		the saturated colour value is returned as a copy. */
	Colour saturateCopy(void) const
	{
		Colour ret = *this;
		ret.saturate();
		return ret;
	}

	/// Array accessor operator
	inline float operator [] (const size_t i) const
	{
		assert( i < 4 );

		return *(&r+i);
	}

	/// Array accessor operator
	inline float& operator [] (const size_t i)
	{
		assert( i < 4 );

		return *(&r+i);
	}

	/// Pointer accessor for direct copying
	inline float* ptr()
	{
		return &r;
	}
	/// Pointer accessor for direct copying
	inline const float* ptr() const
	{
		return &r;
	}

	// arithmetic operations
	inline Colour operator + (const Colour& rkVector) const
	{
		Colour kSum;

		kSum.r = r + rkVector.r;
		kSum.g = g + rkVector.g;
		kSum.b = b + rkVector.b;
		kSum.a = a + rkVector.a;

		return kSum;
	}

	inline Colour operator - (const Colour& rkVector) const
	{
		Colour kDiff;

		kDiff.r = r - rkVector.r;
		kDiff.g = g - rkVector.g;
		kDiff.b = b - rkVector.b;
		kDiff.a = a - rkVector.a;

		return kDiff;
	}

	inline Colour operator * (const float fScalar) const
	{
		Colour kProd;

		kProd.r = fScalar*r;
		kProd.g = fScalar*g;
		kProd.b = fScalar*b;
		kProd.a = fScalar*a;

		return kProd;
	}

	inline Colour operator * (const Colour& rhs) const
	{
		Colour kProd;

		kProd.r = rhs.r * r;
		kProd.g = rhs.g * g;
		kProd.b = rhs.b * b;
		kProd.a = rhs.a * a;

		return kProd;
	}

	inline Colour operator / (const Colour& rhs) const
	{
		Colour kProd;

		kProd.r = rhs.r / r;
		kProd.g = rhs.g / g;
		kProd.b = rhs.b / b;
		kProd.a = rhs.a / a;

		return kProd;
	}

	inline Colour operator / (const float fScalar) const
	{
		assert( fScalar != 0.0 );

		Colour kDiv;

		float fInv = 1.0 / fScalar;
		kDiv.r = r * fInv;
		kDiv.g = g * fInv;
		kDiv.b = b * fInv;
		kDiv.a = a * fInv;

		return kDiv;
	}

	inline friend Colour operator * (const float fScalar, const Colour& rkVector )
	{
		Colour kProd;

		kProd.r = fScalar * rkVector.r;
		kProd.g = fScalar * rkVector.g;
		kProd.b = fScalar * rkVector.b;
		kProd.a = fScalar * rkVector.a;

		return kProd;
	}

	// arithmetic updates
	inline Colour& operator += (const Colour& rkVector)
	{
		r += rkVector.r;
		g += rkVector.g;
		b += rkVector.b;
		a += rkVector.a;

		return *this;
	}

	inline Colour& operator -= (const Colour& rkVector)
	{
		r -= rkVector.r;
		g -= rkVector.g;
		b -= rkVector.b;
		a -= rkVector.a;

		return *this;
	}

	inline Colour& operator *= (const float fScalar)
	{
		r *= fScalar;
		g *= fScalar;
		b *= fScalar;
		a *= fScalar;
		return *this;
	}

	inline Colour& operator /= (const float fScalar)
	{
		assert( fScalar != 0.0 );

		float fInv = 1.0 / fScalar;

		r *= fInv;
		g *= fInv;
		b *= fInv;
		a *= fInv;

		return *this;
	}

	/** Set a colour value from Hue, Saturation and Brightness.
	@param hue Hue value, scaled to the [0,1] range as opposed to the 0-360
	@param saturation Saturation level, [0,1]
	@param brightness Brightness level, [0,1]
	*/
	void setHSB(Real hue, Real saturation, Real brightness);

	/** Convert the current colour to Hue, Saturation and Brightness values.
	@param hue Output hue value, scaled to the [0,1] range as opposed to the 0-360
	@param saturation Output saturation level, [0,1]
	@param brightness Output brightness level, [0,1]
	*/
	void getHSB(Real* hue, Real* saturation, Real* brightness) const;

    static void bindColour();
};
//------------------------------------------------------------------------------
/**
 * Converter between two colours i.e. Ogre::ColourValue to Egine::Colour
 *
 * This template is used to convert from Egine::Colour to Colour
 * of another library
 *
 * @note The colour type must have the public members x, y and z
 * @usage NxVec2 nv; Ogre::Vector2 ov = toVector2<Ogre::Vector2>( nv );
 * @link Thanks to Kojack http://www.ogre3d.org/phpBB2/viewtopic.php?t=38999
 */
template <typename ToType, typename FromType>
ToType toColour(const FromType& c)
{
	return ToType(c.r, c.g, c.b, c.a);
}
//------------------------------------------------------------------------------
} // namespace Util
} // namespace Diversia

CAMP_AUTO_TYPE( Diversia::Util::Colour, &Diversia::Util::Colour::bindColour );

#endif // DIVERSIA_UTIL_COLOUR_H