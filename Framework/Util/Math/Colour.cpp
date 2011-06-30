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
#include "Util/Math/Colour.h"
#include "Util/Math/Math.h"

//------------------------------------------------------------------------------
namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------
const Colour Colour::ZERO =				Colour(0.0,0.0,0.0,0.0);
const Colour Colour::AliceBlue =		Colour(0.941,0.973,1.0); // Should be 0.xxxxxxf
const Colour Colour::AntiqueWhite =		Colour(0.98,0.922,0.843);
const Colour Colour::Beige =			Colour(0.961,0.961,0.863);
const Colour Colour::Black =			Colour(0.0,0.0,0.0);
const Colour Colour::Blue =				Colour(0.0,0.0,1.0);
const Colour Colour::CornflowerBlue =	Colour(/*((uint32)255 << 24) + ((uint32)100 << 16) + ((uint32)149 << 8) + 237*/0.392,0.584,0.929);
const Colour Colour::Green =			Colour(0.0,0.502,0.0);
const Colour Colour::Grey =				Colour(0.5,0.5,0.5);
const Colour Colour::Lime =				Colour(0.0,1.0,0.0);
const Colour Colour::Orange =			Colour(1.0,0.647,0.0);
const Colour Colour::Red =				Colour(1.0,0.0,0.0);
const Colour Colour::SteelBlue =		Colour(0.275,0.51,0.706);
const Colour Colour::Violet =			Colour(0.933,0.51,0.933);
const Colour Colour::White =			Colour(1.0,1.0,1.0);
const Colour Colour::Yellow =			Colour(1.0,1.0,0.0);
//------------------------------------------------------------------------------
#if EGINE_UTIL_ENDIAN == EGINE_UTIL_ENDIAN_BIG
ABGR Colour::getAsABGR(void) const
#else
RGBA Colour::getAsRGBA(void) const
#endif
{
	uint8 val8;
	uint32 val32 = 0;

	// Convert to 32bit pattern
	// (RGBA = 8888)

	// Red
	val8 = static_cast<uint8>(r * 255);
	val32 = val8 << 24;

	// Green
	val8 = static_cast<uint8>(g * 255);
	val32 += val8 << 16;

	// Blue
	val8 = static_cast<uint8>(b * 255);
	val32 += val8 << 8;

	// Alpha
	val8 = static_cast<uint8>(a * 255);
	val32 += val8;

	return val32;
}
//------------------------------------------------------------------------------
#if EGINE_UTIL_ENDIAN == EGINE_UTIL_ENDIAN_BIG
BGRA Colour::getAsBGRA(void) const
#else
ARGB Colour::getAsARGB(void) const
#endif
{
	uint8 val8;
	uint32 val32 = 0;

	// Convert to 32bit pattern
	// (ARGB = 8888)

	// Alpha
	val8 = static_cast<uint8>(a * 255);
	val32 = val8 << 24;

	// Red
	val8 = static_cast<uint8>(r * 255);
	val32 += val8 << 16;

	// Green
	val8 = static_cast<uint8>(g * 255);
	val32 += val8 << 8;

	// Blue
	val8 = static_cast<uint8>(b * 255);
	val32 += val8;

	return val32;
}
//------------------------------------------------------------------------------
#if EGINE_UTIL_ENDIAN == EGINE_UTIL_ENDIAN_BIG
ARGB Colour::getAsARGB(void) const
#else
BGRA Colour::getAsBGRA(void) const
#endif
{
	uint8 val8;
	uint32 val32 = 0;

	// Convert to 32bit pattern
	// (ARGB = 8888)

	// Blue
	val8 = static_cast<uint8>(b * 255);
	val32 = val8 << 24;

	// Green
	val8 = static_cast<uint8>(g * 255);
	val32 += val8 << 16;

	// Red
	val8 = static_cast<uint8>(r * 255);
	val32 += val8 << 8;

	// Alpha
	val8 = static_cast<uint8>(a * 255);
	val32 += val8;

	return val32;
}
//------------------------------------------------------------------------------
#if EGINE_UTIL_ENDIAN == EGINE_UTIL_ENDIAN_BIG
RGBA Colour::getAsRGBA(void) const
#else
ABGR Colour::getAsABGR(void) const
#endif
{
	uint8 val8;
	uint32 val32 = 0;

	// Convert to 32bit pattern
	// (ABRG = 8888)

	// Alpha
	val8 = static_cast<uint8>(a * 255);
	val32 = val8 << 24;

	// Blue
	val8 = static_cast<uint8>(b * 255);
	val32 += val8 << 16;

	// Green
	val8 = static_cast<uint8>(g * 255);
	val32 += val8 << 8;

	// Red
	val8 = static_cast<uint8>(r * 255);
	val32 += val8;

	return val32;
}
//------------------------------------------------------------------------------
#if EGINE_UTIL_ENDIAN == EGINE_UTIL_ENDIAN_BIG
void Colour::setAsABGR(const ABGR val)
#else
void Colour::setAsRGBA(const RGBA val)
#endif
{
	uint32 val32 = val;

	// Convert from 32bit pattern
	// (RGBA = 8888)

	// Red
	r = ((val32 >> 24) & 0xFF) / 255.0f;

	// Green
	g = ((val32 >> 16) & 0xFF) / 255.0f;

	// Blue
	b = ((val32 >> 8) & 0xFF) / 255.0f;

	// Alpha
	a = (val32 & 0xFF) / 255.0f;
}
//------------------------------------------------------------------------------
#if EGINE_UTIL_ENDIAN == EGINE_UTIL_ENDIAN_BIG
void Colour::setAsBGRA(const BGRA val)
#else
void Colour::setAsARGB(const ARGB val)
#endif
{
	uint32 val32 = val;

	// Convert from 32bit pattern
	// (ARGB = 8888)

	// Alpha
	a = ((val32 >> 24) & 0xFF) / 255.0f;

	// Red
	r = ((val32 >> 16) & 0xFF) / 255.0f;

	// Green
	g = ((val32 >> 8) & 0xFF) / 255.0f;

	// Blue
	b = (val32 & 0xFF) / 255.0f;
}
//------------------------------------------------------------------------------
#if EGINE_UTIL_ENDIAN == EGINE_UTIL_ENDIAN_BIG
void Colour::setAsARGB(const ARGB val)
#else
void Colour::setAsBGRA(const BGRA val)
#endif
{
	uint32 val32 = val;

	// Convert from 32bit pattern
	// (ARGB = 8888)

	// Blue
	b = ((val32 >> 24) & 0xFF) / 255.0f;

	// Green
	g = ((val32 >> 16) & 0xFF) / 255.0f;

	// Red
	r = ((val32 >> 8) & 0xFF) / 255.0f;

	// Alpha
	a = (val32 & 0xFF) / 255.0f;
}
//------------------------------------------------------------------------------
#if EGINE_UTIL_ENDIAN == EGINE_UTIL_ENDIAN_BIG
void Colour::setAsRGBA(const RGBA val)
#else
void Colour::setAsABGR(const ABGR val)
#endif
{
	uint32 val32 = val;

	// Convert from 32bit pattern
	// (ABGR = 8888)

	// Alpha
	a = ((val32 >> 24) & 0xFF) / 255.0f;

	// Blue
	b = ((val32 >> 16) & 0xFF) / 255.0f;

	// Green
	g = ((val32 >> 8) & 0xFF) / 255.0f;

	// Red
	r = (val32 & 0xFF) / 255.0f;
}
//------------------------------------------------------------------------------
bool Colour::operator==(const Colour& rhs) const
{
	return (r == rhs.r &&
		g == rhs.g &&
		b == rhs.b &&
		a == rhs.a);
}
//------------------------------------------------------------------------------
bool Colour::operator!=(const Colour& rhs) const
{
	return !(*this == rhs);
}
//------------------------------------------------------------------------------
void Colour::setHSB(Real hue, Real saturation, Real brightness)
{
	// wrap hue
	if (hue > 1.0f)
	{
		hue -= (int)hue;
	}
	else if (hue < 0.0f)
	{
		hue += (int)hue + 1;
	}
	// clamp saturation / brightness
	saturation = std::min(saturation, (Real)1.0);
	saturation = std::max(saturation, (Real)0.0);
	brightness = std::min(brightness, (Real)1.0);
	brightness = std::max(brightness, (Real)0.0);

	if (brightness == 0.0f)
	{   
		// early exit, this has to be black
		r = g = b = 0.0f;
		return;
	}

	if (saturation == 0.0f)
	{   
		// early exit, this has to be grey

		r = g = b = brightness;
		return;
	}

	Real hueDomain  = hue * 6.0f;
	if (hueDomain >= 6.0f)
	{
		// wrap around, and allow mathematical errors
		hueDomain = 0.0f;
	}
	unsigned short domain = (unsigned short)hueDomain;
	Real f1 = brightness * (1 - saturation);
	Real f2 = brightness * (1 - saturation * (hueDomain - domain));
	Real f3 = brightness * (1 - saturation * (1 - (hueDomain - domain)));

	switch (domain)
	{
	case 0:
		// red domain; green ascends
		r = brightness;
		g = f3;
		b = f1;
		break;
	case 1:
		// yellow domain; red descends
		r = f2;
		g = brightness;
		b = f1;
		break;
	case 2:
		// green domain; blue ascends
		r = f1;
		g = brightness;
		b = f3;
		break;
	case 3:
		// cyan domain; green descends
		r = f1;
		g = f2;
		b = brightness;
		break;
	case 4:
		// blue domain; red ascends
		r = f3;
		g = f1;
		b = brightness;
		break;
	case 5:
		// magenta domain; blue descends
		r = brightness;
		g = f1;
		b = f2;
		break;
	}
}
//------------------------------------------------------------------------------
void Colour::getHSB(Real* hue, Real* saturation, Real* brightness) const
{

	Real vMin = std::min(r, std::min(g, b));
	Real vMax = std::max(r, std::max(g, b));
	Real delta = vMax - vMin;

	*brightness = vMax;

	if (Math::RealEqual(delta, 0.0f, 1e-6))
	{
		// grey
		*hue = 0;
		*saturation = 0;
	}
	else									
	{
		// a colour
		*saturation = delta / vMax;

		Real deltaR = (((vMax - r) / 6.0f) + (delta / 2.0f)) / delta;
		Real deltaG = (((vMax - g) / 6.0f) + (delta / 2.0f)) / delta;
		Real deltaB = (((vMax - b) / 6.0f) + (delta / 2.0f)) / delta;

		if (Math::RealEqual(r, vMax))
			*hue = deltaB - deltaG;
		else if (Math::RealEqual(g, vMax))
			*hue = 0.3333333f + deltaR - deltaB;
		else if (Math::RealEqual(b, vMax)) 
			*hue = 0.6666667f + deltaG - deltaR;

		if (*hue < 0.0f) 
			*hue += 1.0f;
		if (*hue > 1.0f)
			*hue -= 1.0f;
	}
}

void Colour::bindColour()
{
    camp::Class::declare<Colour>( "Colour" )
        .tag( "BindingType", BindingType_Colour )
        // Constructors
        .constructor0()
        .constructor3<float, float, float>()
        .constructor4<float, float, float, float>()
        // Static member variables
        // Properties (read-only)
        // Properties (read/write)
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
            .tag( "QtDoublePrecicion", 4 )
        // Functions
        // Operators
        .addOperator<camp::add, Colour, const Colour&>()
        .addOperator<camp::sub, Colour, const Colour&>()
        .addOperator<camp::mul, Colour, const Colour&>()
        .addOperator<camp::mul, Colour, Real>()
        .addOperator<camp::div, Colour, const Colour&>()
        .addOperator<camp::div, Colour, Real>()
        .addOperator<camp::eq, const Colour&>()
        .addOperator<camp::neq, const Colour&>()
        .addOperator<camp::lt, const Colour&>()
        .addOperator<camp::lte, const Colour&>()
        .addOperator<camp::gt, const Colour&>()
        .addOperator<camp::gte, const Colour&>();
        // Static functions
}

//------------------------------------------------------------------------------
} // namespace Util
} // namespace Diversia
//------------------------------------------------------------------------------