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

#include "Shared/Platform/StableHeaders.h"

#include "Shared/Communication/GridPosition.h"
#include "Shared/Communication/ServerPosition.h"

namespace Diversia
{
//------------------------------------------------------------------------------

GridPosition::GridPosition():
    mPosition( std::pair<short, short>( 0, 0 ) )
{

}

GridPosition::GridPosition( short x, short z ):
    mPosition( std::make_pair( x, z ) )
{

}


GridPosition::GridPosition( const std::pair<short, short>& rPosition ):
    mPosition( rPosition )
{

}

GridPosition::GridPosition( const Vector2& rPosition ):
    mPosition( std::make_pair( (short)rPosition.x, (short)rPosition.y ) )
{

}

GridPosition::GridPosition( const ServerPosition& rServerPosition )
{
    Vector2 serverOffset = ServerPosition::getOffset();
    Vector2 pos = rServerPosition.getPosition();
    Real xRounder = pos.x < 0 ? -0.5 : 0.5;
    Real yRounder = pos.y < 0 ? -0.5 : 0.5;
    mPosition = std::pair<short, short>( 
        short( ( ( pos.x + serverOffset.x ) / (Real)DIVERSIA_SERVER_SIZE ) + xRounder ), 
        short( ( ( pos.y + serverOffset.y ) / (Real)DIVERSIA_SERVER_SIZE ) + yRounder ) 
    );

}

void GridPosition::setPositionAtDirection( Direction direction )
{
    mPosition = GridPosition::getPositionAtDirection( direction );
}

std::pair<short, short> GridPosition::getPositionAtDirection( Direction direction ) const
{
    std::pair<short, short> pos = mPosition;

    switch ( direction )
    {
        case NORTH:
        {
            pos.second += 1;
            return pos;
        }
        case NORTH_EAST:
        {
            pos.first += 1;
            pos.second += 1;
            return pos;
        }
        case EAST:
        {
            pos.first += 1;
            return pos;
        }
        case SOUTH_EAST:
        {
            pos.first += 1;
            pos.second -= 1;
            return pos;
        }
        case SOUTH:
        {
            pos.second -= 1;
            return pos;
        }
        case SOUTH_WEST:
        {
            pos.second -= 1;
            pos.first -= 1;
            return pos;
        }
        case WEST:
        {
            pos.first -= 1;
            return pos;
        }
        case NORTH_WEST:
        {
            pos.first -= 1;
            pos.second += 1;
            return pos;
        }
    }

    return pos;
}

unsigned short GridPosition::distanceBetween( const GridPosition& rGridPosition ) const
{
    short xDist = mPosition.first - rGridPosition.getPosition().first;
    if ( xDist < 0 ) xDist *= -1;
    short zDist = mPosition.second - rGridPosition.getPosition().second;
    if ( zDist < 0 ) zDist *= -1;

    return (unsigned short)std::max( xDist, zDist );
}

bool GridPosition::operator<( const GridPosition& rGridPosition ) const
{
    return mPosition < rGridPosition.getPosition();
}

bool GridPosition::operator==( const GridPosition& rGridPosition ) const
{
    return rGridPosition.getPosition() == mPosition;
}

bool GridPosition::operator!=( const GridPosition& rGridPosition ) const
{
    return !operator==( rGridPosition );
}

std::ostream& operator<<( std::ostream& os, const GridPosition& rGridPosition )
{
    std::pair<short, short> pos = rGridPosition.getPosition();
    return os << pos.first << "," << pos.second;
}

GridPosition::operator String() const
{
    std::stringstream ss;
    ss << mPosition.first << "," << mPosition.second;
    return ss.str();
}

//------------------------------------------------------------------------------
} // Namespace Diversia