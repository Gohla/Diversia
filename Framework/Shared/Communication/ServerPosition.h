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

#ifndef DIVERSIA_SHARED_SERVERPOSITION_H
#define DIVERSIA_SHARED_SERVERPOSITION_H

#include "Shared/Platform/Prerequisites.h"

namespace Diversia
{
//------------------------------------------------------------------------------

class DIVERSIA_SHARED_API ServerPosition
{
public:
    /**
    Default constructor, sets position to 0,0.
    **/
    ServerPosition();
    /**
    Constructor. 
    
    @param  x   The x coordinate. 
    @param  z   The z coordinate. 
    **/
    ServerPosition( Real x, Real z );
    /**
    Constructor. 
    
    @param  rPosition   The position. 
    **/
    ServerPosition( const Vector2& rPosition );
    /**
    Constructor. 
    
    @param  rPosition   The position, uses Vector3::x and Vector3::z.
    **/
    ServerPosition( const Vector3& rPosition );
    /**
    Construct from grid position.
    
    @param  rGridPosition   The grid position. 
    **/
    ServerPosition( const GridPosition& rGridPosition );
    /**
    Set the offset of the origin.
    **/
    static inline void setOffset( const Vector2& rOffset ) { msOffset = rOffset; }
    /**
    Gets the offset of the origin.
    **/
    static inline const Vector2& getOffset() { return msOffset; }
    /**
    Set the position.
    **/
    inline void setPosition( const Vector2& rPosition ) { mPosition = rPosition; }
    /**
    Gets the position.
    **/
    inline const Vector2& getPosition() const { return mPosition; }
    /**
    Gets the 3D position
    **/
    inline Vector3 get3DPosition() const { return Vector3( mPosition.x, 0, mPosition.y ); }
    /**
    Transform the position to a 'local' position using a ServerPosition
    **/
    void transformLocal( const GridPosition& rGridPosition );
    /**
    Less than comparison of x and z value.
    **/
    bool operator<( const ServerPosition& rServerPosition ) const;
    /**
    Equality comparison of x and z value.
    **/
    bool operator==( const ServerPosition& rServerPosition ) const;
    /**
    Gets the position as an ostream.
    **/
    friend std::ostream& operator<<( std::ostream& os, const ServerPosition& rServerPosition );

private:
    Vector2         mPosition;
    static Vector2  msOffset;
};

//------------------------------------------------------------------------------
} // Namespace Diversia

#endif // DIVERSIA_SHARED_SERVERPOSITION_H