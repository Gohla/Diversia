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

#include "Platform/StableHeaders.h"

#include "Communication/ServerNeighborsPlugin.h"

namespace Diversia
{
namespace Server 
{
//------------------------------------------------------------------------------

ServerNeighborsPlugin::ServerNeighborsPlugin( Mode mode, ClientPluginManager& rPluginManager, 
    RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
    RakNet::NetworkIDManager& rNetworkIDManager ):
    ClientPlugin( mode, rPluginManager, rRakPeer, rReplicaManager, rNetworkIDManager )
{

}

void ServerNeighborsPlugin::SerializeConstruction( RakNet::BitStream* pConstructionBitstream, 
    RakNet::Connection_RM3* pDestinationConnection )
{
    // Send neighbor map.
    *pConstructionBitstream << mServerNeighbors;
}

RakNet::RM3SerializationResult ServerNeighborsPlugin::Serialize( 
    RakNet::SerializeParameters* pSerializeParameters )
{
    // Send neighbor map.
    pSerializeParameters->outputBitstream[0] << mServerNeighbors;

    // Let RakNet figure out if the neighbors should be serialized.
    return RakNet::RM3SR_BROADCAST_IDENTICALLY;
}

void ServerNeighborsPlugin::create()
{

}

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia