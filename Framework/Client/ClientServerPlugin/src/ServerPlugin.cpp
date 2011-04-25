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

#include "Client/Platform/StableHeaders.h"

#include "Client/ClientServerPlugin/ServerPluginManager.h"
#include "Client/ClientServerPlugin/ServerPlugin.h"
#include "Client/Permission/PermissionManager.h"
#include "Shared/ClientServerPlugin/ClientServerPluginManager.h"
#include "Shared/Camp/CampStringInterpreter.h"

namespace Diversia
{
namespace Client 
{
//------------------------------------------------------------------------------

ServerPlugin::ServerPlugin( Mode mode, PluginState state, ServerPluginManager& rPluginManager, 
    RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
    RakNet::NetworkIDManager& rNetworkIDManager ):
    ClientServerPlugin( mode, state, rPluginManager, rRakPeer, rReplicaManager, rNetworkIDManager ),
    PropertySynchronization( mode )
{
    try
    {
        mPermissionManager = &rPluginManager.getPlugin<PermissionManager>();
    }
    catch( Exception e )
    {
    	mPermissionManager = 0;
    }
}

ServerPluginManager& ServerPlugin::getServerPluginManager() const
{
    // Not inline because of cyclic dependency.
    return static_cast<ServerPluginManager&>( ClientServerPlugin::getPluginManager() );
}

ServerAbstract& ServerPlugin::getServer() const
{
    // Not inline because of cyclic dependency.
    return ServerPlugin::getServerPluginManager().getServer();
}

void ServerPlugin::setOfflineMode( bool offlineMode )
{
    if( offlineMode != mOfflineMode )
    {
        mOfflineMode = offlineMode;
        offlineModeChanged( mOfflineMode );
    }
}

void ServerPlugin::SerializeConstruction( RakNet::BitStream* pConstructionBitstream, 
    RakNet::Connection_RM3* pDestinationConnection )
{
    PropertySynchronization::doSerializeConstruction( pConstructionBitstream, 
        pDestinationConnection );
}

bool ServerPlugin::DeserializeConstruction( RakNet::BitStream* pConstructionBitstream, 
    RakNet::Connection_RM3* pSourceConnection )
{
    PropertySynchronization::doDeserializeConstruction( pConstructionBitstream, pSourceConnection );
    return true;
}

RakNet::RM3SerializationResult ServerPlugin::Serialize( 
    RakNet::SerializeParameters* pSerializeParameters )
{
    return PropertySynchronization::doSerialize( pSerializeParameters );
}

void ServerPlugin::Deserialize( RakNet::DeserializeParameters* pDeserializeParameters )
{
    PropertySynchronization::doDeserialize( pDeserializeParameters );
}

void ServerPlugin::querySetProperty( const String& rQuery, camp::Value& rValue )
{
    if( mPermissionManager )
    {
        // Check permission. Permission name: SetPropertyOn<PluginName>Plugin_<PropertyName>
        mPermissionManager->checkPermissionThrows( String( "SetPropertyOn" ) + 
            getTypeName() + String( "Plugin_" ) + 
            CampStringInterpreter::removeArrayIdentifiers( rQuery ), 
            rValue, "ServerPlugin::querySetProperty" );
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_INVALID_STATE, "Permission manager has not been set yet.", 
	        "ServerPlugin::querySetProperty" );
    }
}

void ServerPlugin::queryInsertProperty( const String& rQuery, camp::Value& rValue )
{
    if( mPermissionManager )
    {
        // Check permission. Permission name: InsertValueIn<PluginName>Plugin_<PropertyName>
        mPermissionManager->checkPermissionThrows( String( "InsertValueIn" ) + 
            getTypeName() + String( "Plugin_" ) + 
            CampStringInterpreter::removeArrayIdentifiers( rQuery ), 
            rValue, "ServerPlugin::queryInsertProperty" );
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_INVALID_STATE, "Permission manager has not been set yet.", 
            "ServerPlugin::queryInsertProperty" );
    }
}

void ServerPlugin::pluginCreated( ClientServerPlugin& rPlugin, bool created )
{
    if( rPlugin.getType() == PermissionManager::getTypeStatic() && created )
    {
        mPermissionManager = &static_cast<PermissionManager&>( rPlugin );
    }
}

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia