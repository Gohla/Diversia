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

#include "Shared/Plugin/Plugin.h"

namespace Diversia
{
//------------------------------------------------------------------------------

Plugin::Plugin( Mode mode, PluginState state,
    PluginManager& rPluginManager, RakNet::RakPeerInterface& rRakPeer,
    RakNet::ReplicaManager3& rReplicaManager, RakNet::NetworkIDManager& rNetworkIDManager ):
    mMode( mode ),
    mPluginState( state ),
    mPluginManager( rPluginManager ),
    mRakPeer( rRakPeer ),
    mReplicaManager( rReplicaManager ),
    mNetworkIDManager( rNetworkIDManager )
{
    this->SetNetworkIDManager( &mNetworkIDManager );

    checkBroadcastConstruction();
}

Plugin::~Plugin()
{

}

void Plugin::setState( PluginState state, PluginState prevState )
{
    if( state == mPluginState ) return;

    mPluginState = state;
    stateChanged( state, prevState );
}

void Plugin::checkBroadcastConstruction()
{
    // Only broadcast construction on the server, clients cannot construct plugins.
    if( mMode == SERVER )
    {
        broadcastConstruction();
    }
}

void Plugin::broadcastConstruction()
{
    mReplicaManager.Reference( this );
}

void Plugin::checkBroadcastDestruction()
{
    // Only broadcast destruction on the server, clients cannot destroy plugins.
    if( mMode == SERVER )
    {
        broadcastDestruction();
    }
}

void Plugin::broadcastDestruction()
{
    Replica3::BroadcastDestruction();
}

void Plugin::WriteAllocationID( RakNet::Connection_RM3* pConnection,
    RakNet::BitStream* pAllocationIdBitstream ) const
{
    // Write plugin replica type and plugin type.
    pAllocationIdBitstream->Write<ReplicaType>( REPLICATYPE_PLUGIN );
    pAllocationIdBitstream->Write<PluginTypeEnum>( getType() );
}

void Plugin::DeallocReplica( RakNet::Connection_RM3* pSourceConnection )
{
    mPluginManager.destroyPlugin( getType() );
}

RakNet::RM3ConstructionState Plugin::QueryConstruction(
    RakNet::Connection_RM3* pDestinationConnection, RakNet::ReplicaManager3* pReplicaManager3 )
{
    return Replica3::QueryConstruction_ServerConstruction( pDestinationConnection,
        mMode == SERVER ? true : false );
}

bool Plugin::QueryRemoteConstruction( RakNet::Connection_RM3* pSourceConnection )
{
    return Replica3::QueryRemoteConstruction_ClientConstruction( pSourceConnection,
        mMode == SERVER ? true : false );
}

//------------------------------------------------------------------------------
} // Namespace Diversia