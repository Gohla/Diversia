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

#ifndef DIVERSIA_SHARED_SERVERNEIGHBORS_H
#define DIVERSIA_SHARED_SERVERNEIGHBORS_H

#include "Shared/Platform/Prerequisites.h"

#include "Shared/Communication/BitStream.h"
#include "Shared/Communication/ServerDirection.h"
#include "Shared/Communication/ServerInfo.h"

namespace Diversia
{
//------------------------------------------------------------------------------

typedef std::map<Direction, ServerInfo> NeighborsMap;

class DIVERSIA_SHARED_API ServerNeighbors
{
public:
    /**
    Adds a neighbor server. 
    
    @param  direction   The direction of the server.
    @param  rServerInfo Information describing the server. 
    **/
    void addServer( Direction direction, const ServerInfo& rServerInfo );
    /**
    Gets a server. 

    @param  direction   The direction of the server. 
    **/
    ServerInfo& getServer( Direction direction );
    /**
    Gets the neighbors. 
    **/
    inline const NeighborsMap& getNeighbors() { return mNeighbors; }
    /**
    Query if there is a server at given direction.
    
    @param  direction   The direction of the server.
    
    @return True if there is a server at given direction, false if not. 
    **/
    bool hasServer( Direction direction );
    /**
    Removes a neighbor server.
    
    @param  direction   The direction of the server.
    **/
    void removeServer( Direction direction );
    /**
    Removes all neighbors.
    **/
    void clear();

    /**
    Read from a RakNet bitstream.
    **/
    friend RakNet::BitStream& operator>>( RakNet::BitStream& in, ServerNeighbors& out )
    {
        out.clear();
        in >> out.mNeighbors;

        return in;
    }
    /**
    Write to a RakNet bitstream.
    **/
    friend RakNet::BitStream& operator<<( RakNet::BitStream& out, ServerNeighbors& in )
    {
        out << in.mNeighbors;

        return out;
    }

private:
    friend class Shared::Bindings::CampBindings;    ///< Allow private access for camp bindings.

    NeighborsMap mNeighbors;
};

//------------------------------------------------------------------------------
} // Namespace Diversia

CAMP_AUTO_TYPE( Diversia::ServerNeighbors, 
    &Diversia::Shared::Bindings::CampBindings::bindServerNeighbors );

#endif // DIVERSIA_SHARED_SERVERNEIGHBORS_H