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

#ifndef DIVERSIA_UTIL_MATRIX3_H
#define DIVERSIA_UTIL_MATRIX3_H

#include "Util/Math/Vector3.h"

// NOTE.  The (x,y,z) coordinate system is assumed to be right-handed.
// Coordinate axis rotation matrices are of the form
//   RX =    1       0       0
//           0     cos(t) -sin(t)
//           0     sin(t)  cos(t)
// where t > 0 indicates a counterclockwise rotation in the yz-plane
//   RY =  cos(t)    0     sin(t)
//           0       1       0
//        -sin(t)    0     cos(t)
// where t > 0 indicates a counterclockwise rotation in the zx-plane
//   RZ =  cos(t) -sin(t)    0
//         sin(t)  cos(t)    0
//           0       0       1
// where t > 0 indicates a counterclockwise rotation in the xy-plane.

//------------------------------------------------------------------------------
namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------
/** A 3x3 matrix which can represent rotations around axes.
    @note
        <b>All the code is adapted from the Wild Magic 0.2 Matrix
        library (http://www.magic-software.com).
    @par
        The coordinate system is assumed to be <b>right-handed</b>.
*/
class DIVERSIA_UTIL_API Matrix3
{
public:
    // construction
    Matrix3 ();
    Matrix3 (const Real arr[3][3]);
    Matrix3 (const Matrix3& rkMatrix);
    Matrix3 (Real fEntry00, Real fEntry01, Real fEntry02,
                Real fEntry10, Real fEntry11, Real fEntry12,
                Real fEntry20, Real fEntry21, Real fEntry22);

    // member access, allows use of construct mat[r][c]
    Real* operator[] (int iRow) const;
    operator Real* ();
    Vector3 GetColumn (int iCol) const;
    void SetColumn(int iCol, const Vector3& vec);
    void FromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);

    // assignment and comparison
    Matrix3& operator= (const Matrix3& rkMatrix);
    bool operator== (const Matrix3& rkMatrix) const;
    bool operator!= (const Matrix3& rkMatrix) const;

    // arithmetic operations
    Matrix3 operator+ (const Matrix3& rkMatrix) const;
    Matrix3 operator- (const Matrix3& rkMatrix) const;
    Matrix3 operator* (const Matrix3& rkMatrix) const;
    Matrix3 operator- () const;

    // matrix * vector [3x3 * 3x1 = 3x1]
    Vector3 operator* (const Vector3& rkVector) const;

    // vector * matrix [1x3 * 3x3 = 1x3]
    friend Vector3 operator* (const Vector3& rkVector,
        const Matrix3& rkMatrix);

    // matrix * scalar
    Matrix3 operator* (Real fScalar) const;

    // scalar * matrix
    friend Matrix3 operator* (Real fScalar, const Matrix3& rkMatrix);

    /** Function for writing to a stream.
	*/
	inline DIVERSIA_UTIL_API friend std::ostream& operator <<
		( std::ostream& o, const Matrix3& m )
	{
		o << "Matrix3(";
		for (size_t i = 0; i < 3; ++i)
		{
			o << " row" << (unsigned)i << "{";
			for(size_t j = 0; j < 3; ++j)
			{
				o << m[i][j] << " ";
			}
			o << "}";
		}
		o << ")";
		return o;
	}

    // utilities
    Matrix3 Transpose () const;
    bool Inverse (Matrix3& rkInverse, Real fTolerance = 1e-06) const;
    Matrix3 Inverse (Real fTolerance = 1e-06) const;
    Real Determinant () const;

    // singular value decomposition
    void SingularValueDecomposition (Matrix3& rkL, Vector3& rkS,
        Matrix3& rkR) const;
    void SingularValueComposition (const Matrix3& rkL,
        const Vector3& rkS, const Matrix3& rkR);

    // Gram-Schmidt orthonormalization (applied to columns of rotation matrix)
    void Orthonormalize ();

    // orthogonal Q, diagonal D, upper triangular U stored as (u01,u02,u12)
    void QDUDecomposition (Matrix3& rkQ, Vector3& rkD,
        Vector3& rkU) const;

    Real SpectralNorm () const;

    // matrix must be orthonormal
    void ToAxisAngle (Vector3& rkAxis, Radian& rfRadians) const;
    void FromAxisAngle (const Vector3& rkAxis, Real fRadians);

    // The matrix must be orthonormal.  The decomposition is yaw*pitch*roll
    // where yaw is rotation about the Up vector, pitch is rotation about the
    // Right axis, and roll is rotation about the Direction axis.
    bool ToEulerAnglesXYZ (Radian& rfYAngle, Radian& rfPAngle,
        Radian& rfRAngle) const;
    bool ToEulerAnglesXZY (Radian& rfYAngle, Radian& rfPAngle,
        Radian& rfRAngle) const;
    bool ToEulerAnglesYXZ (Radian& rfYAngle, Radian& rfPAngle,
        Radian& rfRAngle) const;
    bool ToEulerAnglesYZX (Radian& rfYAngle, Radian& rfPAngle,
        Radian& rfRAngle) const;
    bool ToEulerAnglesZXY (Radian& rfYAngle, Radian& rfPAngle,
        Radian& rfRAngle) const;
    bool ToEulerAnglesZYX (Radian& rfYAngle, Radian& rfPAngle,
        Radian& rfRAngle) const;
    void FromEulerAnglesXYZ (Radian fYAngle, Radian fPAngle, Radian fRAngle);
    void FromEulerAnglesXZY (Radian fYAngle, Radian fPAngle, Radian fRAngle);
    void FromEulerAnglesYXZ (Radian fYAngle, Radian fPAngle, Radian fRAngle);
    void FromEulerAnglesYZX (Radian fYAngle, Radian fPAngle, Radian fRAngle);
    void FromEulerAnglesZXY (Radian fYAngle, Radian fPAngle, Radian fRAngle);
    void FromEulerAnglesZYX (Radian fYAngle, Radian fPAngle, Radian fRAngle);

    // eigensolver, matrix must be symmetric
    void EigenSolveSymmetric (Real afEigenvalue[3],
        Vector3 akEigenvector[3]) const;

    static void TensorProduct (const Vector3& rkU, const Vector3& rkV,
        Matrix3& rkProduct);

    static const Real EPSILON;
    static const Matrix3 ZERO;
    static const Matrix3 IDENTITY;

    static void bindMatrix3();

protected:
    // support for eigensolver
    void Tridiagonal (Real afDiag[3], Real afSubDiag[3]);
    bool QLAlgorithm (Real afDiag[3], Real afSubDiag[3]);

    // support for singular value decomposition
    static const Real ms_fSvdEpsilon;
    static const int ms_iSvdMaxIterations;
    static void Bidiagonalize (Matrix3& kA, Matrix3& kL,
        Matrix3& kR);
    static void GolubKahanStep (Matrix3& kA, Matrix3& kL,
        Matrix3& kR);

    // support for spectral norm
    static Real MaxCubicRoot (Real afCoeff[3]);

    Real m[3][3];

    // for faster access
    friend class Matrix4;
};
//------------------------------------------------------------------------------
} // namespace Util
} // namespace Diversia

CAMP_AUTO_TYPE( Diversia::Util::Matrix3, &Diversia::Util::Matrix3::bindMatrix3 );

#endif // DIVERSIA_UTIL_MATRIX3_H