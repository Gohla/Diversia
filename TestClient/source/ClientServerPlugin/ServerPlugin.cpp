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

#include "ClientServerPlugin/ServerPlugin.h"
#include "Permission/PermissionManager.h"
#include "Shared/Camp/CampStringInterpreter.h"
#include "Shared/Camp/CampBitStream.h"

namespace Diversia
{
namespace TestClient 
{
//------------------------------------------------------------------------------

ServerPlugin::ServerPlugin( Mode mode, ClientServerPluginManager& rPluginManager, 
    RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
    NetworkIDManager& rNetworkIDManager, camp::UserObject userObject ):
    ClientServerPlugin( mode, rPluginManager, rRakPeer, rReplicaManager, rNetworkIDManager ),
    mUserObject( userObject ),
    mNextSerialization( MAXUINT )
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

ServerPlugin::~ServerPlugin()
{

}

void ServerPlugin::set( const String& rQuery, const camp::Value& rValue )
{
    if( mPermissionManager )
    {
        camp::Value value = rValue.to<camp::UserObject>().copy();

        // Check permission. Permission name: SetPropertyOn<PluginName>Plugin_<PropertyName>
        mPermissionManager->checkPermissionThrows( std::string( "SetPropertyOn" ) + 
            getTypeName() +  std::string( "Plugin_" ) + 
            CampStringInterpreter::removeArrayIdentifiers( rQuery ), 
            value, "ServerPlugin::set" );

        // Set property.
        CampStringInterpreter::set( mUserObject, rQuery, value );
        mOutputPropertyTransaction.addChangedProperty( rQuery, value );

        // Serialize changes in 100ms.
        if( mNextSerialization == MAXUINT )
        {
            mNextSerialization = RakNet::GetTime() + 100;
        }
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "PermissionManager has not been created yet", 
            "ServerPlugin::set" );
    }
}

camp::Value ServerPlugin::get( const String& rQuery )
{
    return CampStringInterpreter::get( mUserObject, rQuery );
}

void ServerPlugin::SerializeConstruction( RakNet::BitStream* pConstructionBitstream, 
    RakNet::Connection_RM3* pDestinationConnection )
{
    CampBitStream::serialize( mUserObject, *pConstructionBitstream, "NoBitStream" );
}

bool ServerPlugin::DeserializeConstruction( RakNet::BitStream* pConstructionBitstream, 
    RakNet::Connection_RM3* pSourceConnection )
{
    CampBitStream::deserialize( mUserObject, *pConstructionBitstream, "NoBitStream" );
    return true;
}

RakNet::RM3SerializationResult ServerPlugin::Serialize( 
    RakNet::SerializeParameters* pSerializeParameters )
{
    if( RakNet::GetTime() >= mNextSerialization )
    {
        pSerializeParameters->outputBitstream[7] << mOutputPropertyTransaction;
        mOutputPropertyTransaction.reset();
        mNextSerialization = MAXUINT;
        return RakNet::RM3SR_SERIALIZED_ALWAYS_IDENTICALLY;	///< RakNet doesn't have to check changes.
    } 
    else
    {
        return RakNet::RM3SR_DO_NOT_SERIALIZE;
    }
}

void ServerPlugin::Deserialize( RakNet::DeserializeParameters* pDeserializeParameters )
{
    pDeserializeParameters->serializationBitstream[7] >> mInputPropertyTransaction;
    mInputPropertyTransaction.apply( mUserObject );
    mInputPropertyTransaction.reset();
}

void ServerPlugin::pluginCreated( ClientServerPlugin& rPlugin, bool created )
{
    if( rPlugin.getType() == PermissionManager::getTypeStatic() && created )
    {
        mPermissionManager = &static_cast<PermissionManager&>( rPlugin );
    }
}

//------------------------------------------------------------------------------
} // Namespace TestClient
} // Namespace Diversia