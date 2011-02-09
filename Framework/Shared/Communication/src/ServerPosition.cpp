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

#include "Shared/Communication/ServerPosition.h"
#include "Shared/Communication/GridPosition.h"

namespace Diversia
{
//------------------------------------------------------------------------------

Vector2 ServerPosition::msOffset = Vector2::ZERO;

ServerPosition::ServerPosition():
    mPosition( Vector2::ZERO )
{

}

ServerPosition::ServerPosition( Real x, Real z ):
    mPosition( Vector2( x, z ) )
{

}

ServerPosition::ServerPosition( const Vector2& rPosition ):
    mPosition( rPosition )
{

}

ServerPosition::ServerPosition( const Vector3& rPosition ):
    mPosition( rPosition.x, rPosition.z )
{

}

ServerPosition::ServerPosition( const GridPosition& rGridPosition  )
{
    std::pair<short, short> pos = rGridPosition.getPosition();
    mPosition = Vector2( 
        ( ( pos.first * (Real)DIVERSIA_SERVER_SIZE ) + msOffset.x ), 
        ( pos.second * (Real)DIVERSIA_SERVER_SIZE ) + msOffset.y );
}

bool ServerPosition::operator<( const ServerPosition& rServerPosition ) const
{
    return mPosition < rServerPosition.getPosition();
}

bool ServerPosition::operator==( const ServerPosition& rServerPosition ) const
{
    return mPosition == rServerPosition.getPosition();
}

void ServerPosition::transformLocal( const GridPosition& rGridPosition )
{
    mPosition.x -= (Real)rGridPosition.getPosition().first * (Real)DIVERSIA_SERVER_SIZE;
    mPosition.y -= (Real)rGridPosition.getPosition().second * (Real)DIVERSIA_SERVER_SIZE;
}

std::ostream& operator<<( std::ostream& os, const ServerPosition& rServerPosition )
{
    Vector2 pos = rServerPosition.getPosition();
    return os << pos.x << "," << pos.y;
}

//------------------------------------------------------------------------------
} // Namespace Diversia