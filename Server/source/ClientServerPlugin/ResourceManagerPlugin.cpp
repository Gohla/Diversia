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

#include "ClientServerPlugin/ResourceManagerPlugin.h"

namespace Diversia
{
namespace Server 
{
//------------------------------------------------------------------------------

ResourceManagerPlugin::ResourceManagerPlugin( Mode mode, ClientPluginManager& rPluginManager, 
    RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
    RakNet::NetworkIDManager& rNetworkIDManager ):
    ClientPlugin( mode, rPluginManager, rRakPeer, rReplicaManager, rNetworkIDManager ),
    mType( RESOURCELOCATIONTYPE_URL ),
    mResourceLocation( "http://127.0.0.1/" )
{
    PropertySynchronization::storeUserObject();
}

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia