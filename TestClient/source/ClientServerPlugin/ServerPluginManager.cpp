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

#include "ClientServerPlugin/ServerPluginManager.h"

namespace Diversia
{
namespace TestClient 
{
//------------------------------------------------------------------------------

ServerPluginManager::ServerPluginManager( Mode mode, RakPeerInterface& rRakPeer,
    RakNet::ReplicaManager3& rReplicaManager, NetworkIDManager& rNetworkIDManager ):
    ClientServerPluginManager( mode, rRakPeer, rReplicaManager, rNetworkIDManager )
{
    mUpdateConnection = Globals::mUpdateSignal.connect( sigc::mem_fun( this, 
        &ClientServerPluginManager::update ) );
    mUpdateConnection.block( true );
}

ServerPluginManager::~ServerPluginManager()
{

}

//------------------------------------------------------------------------------
} // Namespace TestClient
} // Namespace Diversia