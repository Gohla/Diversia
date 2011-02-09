/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.

Diversia is free software: you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation, either version 3 of the License, or (at your option) any later 
version.

Diversia is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with 
Diversia. If not, see <http://www.gnu.org/licenses/>.

You may contact the author of Diversia by e-mail at: equabyte@sonologic.nl
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "ClientServerPlugin/ClientPluginManager.h"

namespace Diversia
{
namespace Server 
{
//------------------------------------------------------------------------------

ClientPluginManager::ClientPluginManager( Mode mode, sigc::signal<void>& rUpdateSignal, 
    RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
    RakNet::NetworkIDManager& rNetworkIDManager, RakNet::RPC3& rRPC3 ):
    ClientServerPluginManager( mode, rUpdateSignal, rRakPeer, rReplicaManager, rNetworkIDManager ),
    mRPC3( rRPC3 )
{

}

ClientPluginManager::~ClientPluginManager()
{

}

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia