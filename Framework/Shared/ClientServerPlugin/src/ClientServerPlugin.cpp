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

#include "Shared/ClientServerPlugin/ClientServerPlugin.h"

namespace Diversia
{
//------------------------------------------------------------------------------

ClientServerPlugin::ClientServerPlugin( Mode mode,
    ClientServerPluginManager& rPluginManager, RakNet::RakPeerInterface& rRakPeer,
    RakNet::ReplicaManager3& rReplicaManager, RakNet::NetworkIDManager& rNetworkIDManager ):
    mMode( mode ),
    mPluginManager( rPluginManager ),
    mRakPeer( rRakPeer ),
    mReplicaManager( rReplicaManager ),
    mNetworkIDManager( rNetworkIDManager )
{
    this->SetNetworkIDManager( &mNetworkIDManager );

    checkBroadcastConstruction();
}

ClientServerPlugin::~ClientServerPlugin()
{
}

void ClientServerPlugin::checkBroadcastConstruction()
{
    // Only broadcast construction on the server, clients cannot construct plugins.
    if( mMode == SERVER )
    {
        broadcastConstruction();
    }
}

void ClientServerPlugin::broadcastConstruction()
{
    mReplicaManager.Reference( this );
}

void ClientServerPlugin::checkBroadcastDestruction()
{
    // Only broadcast destruction on the server, clients cannot destroy plugins.
    if( mMode == SERVER )
    {
        broadcastDestruction();
    }
}

void ClientServerPlugin::broadcastDestruction()
{
    Replica3::BroadcastDestruction();
}

void ClientServerPlugin::WriteAllocationID( RakNet::Connection_RM3* pConnection,
    RakNet::BitStream* pAllocationIdBitstream ) const
{
    // Write plugin replica type and plugin type.
    pAllocationIdBitstream->Write<ReplicaType>( REPLICATYPE_CLIENTSERVERPLUGIN );
    pAllocationIdBitstream->Write<ClientServerPluginTypeEnum>( getType() );
}

void ClientServerPlugin::DeallocReplica( RakNet::Connection_RM3* pSourceConnection )
{
    mPluginManager.destroyPlugin( getType() );
}

RakNet::RM3ConstructionState ClientServerPlugin::QueryConstruction(
    RakNet::Connection_RM3* pDestinationConnection, RakNet::ReplicaManager3* pReplicaManager3 )
{
    return Replica3::QueryConstruction_ServerConstruction( pDestinationConnection,
        mMode == SERVER ? true : false );
}

bool ClientServerPlugin::QueryRemoteConstruction( RakNet::Connection_RM3* pSourceConnection )
{
    return Replica3::QueryRemoteConstruction_ClientConstruction( pSourceConnection,
        mMode == SERVER ? true : false );
}

//------------------------------------------------------------------------------
} // Namespace Diversia