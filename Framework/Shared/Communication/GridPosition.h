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

#ifndef DIVERSIA_SHARED_GRIDPOSITION_H
#define DIVERSIA_SHARED_GRIDPOSITION_H

#include "Shared/Platform/Prerequisites.h"

#include "Shared/Communication/ServerDirection.h"

namespace Diversia
{
//------------------------------------------------------------------------------

/**
Class containing the Diversia grid position of a server and some convenience methods for
conversions. 
**/
class DIVERSIA_SHARED_API GridPosition
{
public:
    /** 
    Default constructor.
    **/
    GridPosition();
    /**
    Constructor. 
    
    @param  x   The x coordinate. 
    @param  z   The z coordinate. 
    **/
    GridPosition( short x, short z );
    /**
    Constructor. 
    
    @param  rPosition   The position. 
    **/
    GridPosition( const std::pair<short, short>& rPosition );
    /**
    Constructor. 
    
    @param  rPosition   The position. 
    **/
    GridPosition( const Vector2& rPosition );
    /**
    Construct from a server position (position on the terrain/level).
    
    @param  rOgreServerPosition The server position. 
    **/
    GridPosition( const ServerPosition& rServerPosition );
    /**
    Sets the position. 
    
    @param  rPosition   The position. 
    **/
    inline void setPosition( const std::pair<short, short>& rPosition ) { mPosition = rPosition; }
    /** 
    Gets the position.
    **/
    inline const std::pair<short, short>& getPosition() const { return mPosition; }
    /**
    Sets the position to a specified direction from the current position.
    
    @param  direction   The direction. 
    **/
    void setPositionAtDirection( Direction direction );
    /**
    Gets the position at a specified direction from the current position.
    
    @param  direction   The direction. 
    
    @return The position at given direction. 
    **/
    std::pair<short, short> getPositionAtDirection( Direction direction ) const;
    /**
    Gets the distance between this position and the given position.
    
    @param  rGridPosition The other server position.
    
    @return The distance between this server position and given server position. 
    **/
    unsigned short distanceBetween( const GridPosition& rGridPosition ) const;
    /** 
    Less than comparison of x and z value.
    **/
    bool operator<( const GridPosition& rGridPosition ) const;
    /** 
    Equality comparison of x and z value.
    **/
    bool operator==( const GridPosition& rGridPosition ) const;
    /** 
    Non-equality comparison of x and z value.
    **/
    bool operator!=( const GridPosition& rGridPosition ) const;
    /** 
    Gets the position as an ostream.
    **/
    friend std::ostream& operator<<( std::ostream& os, const GridPosition& rGridPosition );
    /** 
    Gets the position as a string.
    **/
    operator String() const;

private:
    std::pair<short, short> mPosition;

};

//------------------------------------------------------------------------------
} // Namespace Diversia

#endif // DIVERSIA_SHARED_GRIDPOSITION_H