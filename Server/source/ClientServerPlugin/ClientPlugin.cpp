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

#include "ClientServerPlugin/ClientPlugin.h"
#include "ClientServerPlugin/ClientPluginManager.h"
#include "Permission/PermissionManager.h"
#include "Shared/Camp/CampStringInterpreter.h"

namespace Diversia
{
namespace Server 
{
//------------------------------------------------------------------------------

ClientPlugin::ClientPlugin( Mode mode, ClientPluginManager& rPluginManager, 
    RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
    RakNet::NetworkIDManager& rNetworkIDManager ):
    ClientServerPlugin( mode, rPluginManager, rRakPeer, rReplicaManager, rNetworkIDManager ),
    PropertySynchronization( mode )
{
    try
    {
        mPermissionManager = &rPluginManager.getPlugin<PermissionManager>();
    }
    catch ( Exception e )
    {
        mPermissionManager = 0;
    }
}

void ClientPlugin::querySetPropertyDeserialize( const String& rQuery, camp::Value& rValue, 
    RakNet::RakNetGUID source )
{
    // Permission name: InsertValueIn<PluginName>Plugin_<PropertyName>
    mPermissionManager->checkPermissionThrows( source, String( "InsertValueIn" ) +  
        getTypeName() + String( "Plugin_" ) + 
        CampStringInterpreter::removeArrayIdentifiers( rQuery ), 
        rValue, "ClientPlugin::Deserialize" );
}

void ClientPlugin::queryInsertPropertyDeserialize( const String& rQuery, camp::Value& rValue, 
    RakNet::RakNetGUID source )
{
    // Permission name: SetPropertyOn<PluginName>Plugin_<PropertyName>
    mPermissionManager->checkPermissionThrows( source, String( "SetPropertyOn" ) +  
        getTypeName() + String( "Plugin_" ) + 
        CampStringInterpreter::removeArrayIdentifiers( rQuery ), 
        rValue, "ClientPlugin::Deserialize" );
}

void ClientPlugin::SerializeConstruction( RakNet::BitStream* pConstructionBitstream, 
    RakNet::Connection_RM3* pDestinationConnection )
{
    PropertySynchronization::doSerializeConstruction( pConstructionBitstream, 
        pDestinationConnection );
}

bool ClientPlugin::DeserializeConstruction( RakNet::BitStream* pConstructionBitstream, 
    RakNet::Connection_RM3* pSourceConnection )
{
    PropertySynchronization::doDeserializeConstruction( pConstructionBitstream, pSourceConnection );
    return true;
}

RakNet::RM3SerializationResult ClientPlugin::Serialize( 
    RakNet::SerializeParameters* pSerializeParameters )
{
    return PropertySynchronization::doSerialize( pSerializeParameters );
}

void ClientPlugin::Deserialize( RakNet::DeserializeParameters* pDeserializeParameters )
{
    PropertySynchronization::doDeserialize( pDeserializeParameters );
}

void ClientPlugin::pluginCreated( ClientServerPlugin& rPlugin, bool created )
{
    if( rPlugin.getType() == PermissionManager::getTypeStatic() && created )
    {
        mPermissionManager = &static_cast<PermissionManager&>( rPlugin );
    }
}

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia