/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

Original file used from the OGRE project, license:
-----------------------------------------------------------------------------
Copyright (c) 2000-2009 Torus Knot Software Ltd

Original free version by:
-----------------------------------------------------------------------------
Magic Software, Inc.
http://www.geometrictools.com/
Copyright (c) 1998-2008

Source code from Magic Software is supplied under the terms of a license
agreement and may not be copied or disclosed except in accordance with the
terms of that agreement.  The various license agreements may be found at
the Magic Software web site.  This file is subject to the license.
--------------------------------------------------------------------------------
*/

#ifndef DIVERSIA_UTIL_PLANE_H
#define DIVERSIA_UTIL_PLANE_H

#include "Util/Platform/Prerequisites.h"

#include "Util/Math/Vector3.h"

//------------------------------------------------------------------------------
namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------
/**
 * Defines a plane in 3D space.
 *
 * A plane is defined in 3D space by the equation
 *  Ax + By + Cz + D = 0
 *
 * This equates to a vector (the normal of the plane, whose x, y and z
 *  components equate to the coefficients A, B and C respectively), and a
 *  constant (D) which is the distance along the normal you have to go to
 *  move the plane back to the origin.
 */
class DIVERSIA_UTIL_API Plane
{
public:
	/** Default constructor - sets everything to 0.
	*/
	Plane();
	Plane(const Plane& rhs);
	/**
	 * Construct a plane through a normal, and a distance to move the plane
	 *  along the normal.
	 */
	Plane(const Vector3& rkNormal, Real fConstant);
	/**
	 * Construct a plane using the 4 constants directly
	 */
	Plane(Real a, Real b, Real c, Real d);
	Plane(const Vector3& rkNormal, const Vector3& rkPoint);
	Plane(const Vector3& rkPoint0, const Vector3& rkPoint1,
		const Vector3& rkPoint2);

	/**
	 * The "positive side" of the plane is the half space to which the
	 *  plane normal points. The "negative side" is the other half
	 *  space. The flag "no side" indicates the plane itself.
	 */
	enum Side
	{
		NO_SIDE,
		POSITIVE_SIDE,
		NEGATIVE_SIDE,
		BOTH_SIDE
	};

	Side getSide(const Vector3& rkPoint) const;

	/**
	 * Returns the side where the aligneBox is. the flag BOTH_SIDE indicates an
	 *  intersecting box. One corner ON the plane is sufficient to consider the
	 *  box and the plane intersecting.
	 */
	Side getSide(const BoundingBox& rkBox) const;

	/**
	 * Returns which side of the plane that the given box lies on.
	 *  The box is defined as centre/half-size pairs for effectively.
	 *
	 * @param centre The centre of the box.
	 * @param halfSize The half-size of the box.
	 * @returns
	 *  POSITIVE_SIDE if the box complete lies on the "positive side" of the plane,
	 *  NEGATIVE_SIDE if the box complete lies on the "negative side" of the plane,
	 *  and BOTH_SIDE if the box intersects the plane.
	 */
	Side getSide(const Vector3& centre, const Vector3& halfSize) const;

	/**
	 * This is a pseudodistance. The sign of the return value is
	 *  positive if the point is on the positive side of the plane,
	 *  negative if the point is on the negative side, and zero if the
	 *  point is on the plane.
	 *
	 * The absolute value of the return value is the true distance only
	 *  when the plane normal is a unit length vector.
	 */
	Real getDistance(const Vector3& rkPoint) const;

	/**
	 * Redefine this plane based on 3 points.
	 */
	void redefine(const Vector3& rkPoint0, const Vector3& rkPoint1,
		const Vector3& rkPoint2);

	/**
	 * Redefine this plane based on a normal and a point.
	 */
	void redefine(const Vector3& rkNormal, const Vector3& rkPoint);

	/**
	 * Project a vector onto the plane.
	 *
	 * This gives you the element of the input vector that is perpendicular
	 *  to the normal of the plane. You can get the element which is parallel
	 *  to the normal of the plane by subtracting the result of this method
	 *  from the original vector, since parallel + perpendicular = original.
	 *
	 * @param v The input vector
	 */
	Vector3 projectVector(const Vector3& v) const;

	/**
	 * Normalises the plane.
	 *
	 * This method normalises the plane's normal and the length scale of d
	 *  is as well.
	 *
	 * @note This function will not crash for zero-sized vectors, but there
	 *  will be no changes made to their components.
	 * @returns The previous length of the plane's normal.
	 */
	Real normalise(void);

	Vector3 normal;
	Real d;

	/// Comparison operator
	bool operator==(const Plane& rhs) const
	{
		return (rhs.d == d && rhs.normal == normal);
	}
	bool operator!=(const Plane& rhs) const
	{
		return (rhs.d != d && rhs.normal != normal);
	}
};
//------------------------------------------------------------------------------
typedef std::vector<Plane> PlaneList;
//------------------------------------------------------------------------------
} // namespace Util
} // namespace Diversia
//------------------------------------------------------------------------------

#endif // DIVERSIA_UTIL_PLANE_H