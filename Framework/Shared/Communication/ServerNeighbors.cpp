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

#include "Shared/Communication/ServerNeighbors.h"

namespace Diversia
{
//------------------------------------------------------------------------------

void ServerNeighbors::addServer( Direction direction, const ServerInfo& rServerInfo )
{
    if( !hasServer( direction ) )
    {
        mNeighbors.insert( std::make_pair( direction, rServerInfo ) );
    }
    else
        DIVERSIA_EXCEPT( Exception::ERR_DUPLICATE_ITEM, "Neighbor server already exists.", 
            "ServerNeighbors::addServer" );
}

ServerInfo& ServerNeighbors::getServer( Direction direction )
{
    if( hasServer( direction ) )
    {
        return mNeighbors.find( direction )->second;
    }
    else
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Neighbor server does not exist.", 
            "ServerNeighbors::getServer" );
} 

bool ServerNeighbors::hasServer( Direction direction )
{
    return mNeighbors.find( direction ) != mNeighbors.end();
}

void ServerNeighbors::removeServer( Direction direction )
{
    if( hasServer( direction ) )
    {
        mNeighbors.erase( direction );
    }
    else
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Neighbor server does not exist.", 
            "ServerNeighbors::removeServer" );
}

void ServerNeighbors::clear()
{
    mNeighbors.clear();
}

//------------------------------------------------------------------------------
} // Namespace Diversia