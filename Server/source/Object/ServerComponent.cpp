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

#include "Object/ServerComponent.h"
#include "Object/ServerObject.h"
#include "Object/ComponentFactoryManager.h"
#include "Object/ComponentFactory.h"
#include "Permission/PermissionManager.h"
#include "Shared/Camp/CampStringInterpreter.h"

namespace Diversia
{
namespace Server 
{
//------------------------------------------------------------------------------

ServerComponent::ServerComponent( const String& rName, Mode mode, NetworkingType networkingType, 
    ComponentType type, RakNet::RakNetGUID source, bool localOverride, ServerObject& rObject ):
    Component( rName, mode, networkingType, type, source, localOverride, rObject ),
    PropertySynchronization( mode ),
    mPermissionManager( rObject.getPermissionManager() )
{
    // Need to use the source here because CreatingSystemGUID has not been set yet.
    if( ServerComponent::getNetworkingType() == REMOTE && 
        !Component::isCreatedByServer() )
    {
        // Add to the user's item counter if this component is created by a client.
        mPermissionManager.getPermission( source, 
            "ObjectManager_CreateRemoteComponent" ).addItem();

        if( ServerComponent::getServerObject().isCreatedBySource( source ) )
        {
            mPermissionManager.getPermission( source, 
                "ObjectManager_CreateRemoteComponentOnOwnObject" ).addItem();
        }
        else
        {
            mPermissionManager.getPermission( source, 
                "ObjectManager_CreateRemoteComponentOnOtherObject" ).addItem();
        }

        // Permission name: <Component>_Create
        mPermissionManager.getPermission( source, 
            ComponentFactoryManager::getComponentFactory( 
            mType ).getTypeName() + String( "_Create" ) ).addItem();
    }
}

ServerComponent::~ServerComponent()
{
    if( ServerComponent::getNetworkingType() == REMOTE && 
        !Component::isCreatedByServer() )
    {
        // Remove from the user's item counter if this component is created by a client.
        mPermissionManager.getPermission( Component::getSourceGUID(), 
            "ObjectManager_CreateRemoteComponent" ).removeItem();

        if( ServerComponent::getServerObject().isCreatedBy( Component::getSourceGUID() ) )
        {
            mPermissionManager.getPermission( Component::getSourceGUID(), 
                "ObjectManager_CreateRemoteComponentOnOwnObject" ).removeItem();
        }
        else
        {
            mPermissionManager.getPermission( Component::getSourceGUID(), 
                "ObjectManager_CreateRemoteComponentOnOtherObject" ).removeItem();
        }

        // Permission name: <Component>_Create
        mPermissionManager.getPermission( Component::getSourceGUID(), 
            ComponentFactoryManager::getComponentFactory( 
            mType ).getTypeName() + String( "_Create" ) ).removeItem();
    }
}

void ServerComponent::setNetworkingTypeImpl( NetworkingType type )
{
    // TODO: Implement server networking type change.
}

void ServerComponent::querySetPropertyDeserialize( const String& rQuery, camp::Value& rValue, 
    RakNet::RakNetGUID source )
{
    // Permission name: InsertValueIn<Ownership><ComponentName>Component_<PropertyName>
    mPermissionManager.checkPermissionThrows( source, String( "InsertValueIn" ) + 
        String( Component::isCreatedBySource( source ) ? "Own" : "Other" ) + 
        getTypeName() + String( "Component_" ) + 
        CampStringInterpreter::removeArrayIdentifiers( rQuery ), 
        rValue, "ServerComponent::querySetPropertyDeserialize" );
}

void ServerComponent::queryInsertPropertyDeserialize( const String& rQuery, camp::Value& rValue, 
    RakNet::RakNetGUID source )
{
    // Permission name: SetPropertyOn<Ownership><ComponentName>Component_<PropertyName>
    mPermissionManager.checkPermissionThrows( source, String( "SetPropertyOn" ) + 
        String( Component::isCreatedBySource( source ) ? "Own" : "Other" ) + 
        getTypeName() + String( "Component_" ) + 
        CampStringInterpreter::removeArrayIdentifiers( rQuery ), 
        rValue, "ServerComponent::queryInsertPropertyDeserialize" );
}

RakNet::RM3ConstructionState ServerComponent::QueryConstruction( 
    RakNet::Connection_RM3* pDestinationConnection, RakNet::ReplicaManager3* pReplicaManager3 )
{
    // No permission checking needed, this is only called if the server creates a component.
    return Replica3::QueryConstruction_ClientConstruction( pDestinationConnection,
        Component::getMode() == SERVER ? true : false );
}

bool ServerComponent::QueryRemoteConstruction( RakNet::Connection_RM3* pSourceConnection )
{
    // Always allow, permission checking is done in Object.
    return true;
}

RakNet::RM3QuerySerializationResult ServerComponent::QuerySerialization( 
    RakNet::Connection_RM3* pDestinationConnection )
{
    // Allow serializations for components created by the same client. Override and replace this 
    // behavior if needed.
/*
    if( Component::isCreatedBySource( pDestinationConnection->GetRakNetGUID() ) )
        return Replica3::QuerySerialization_ClientSerializable( pDestinationConnection,
        Component::getMode() == SERVER ? true : false );
    else
        return Replica3::QuerySerialization_ServerSerializable( pDestinationConnection,
        Component::getMode() == SERVER ? true : false );*/

    return RakNet::RM3QSR_CALL_SERIALIZE;
}

bool ServerComponent::DeserializeDestruction( RakNet::BitStream* pDestructionBitstream, 
    RakNet::Connection_RM3* pSourceConnection )
{
    if( ServerComponent::getNetworkingType() == REMOTE && 
        !ServerComponent::getLocalOverride() )
    {
        if( Component::isCreatedBySource( pSourceConnection->GetRakNetGUID() ) )
        {
            if( ServerComponent::getServerObject().isCreatedBySource( 
                pSourceConnection->GetRakNetGUID() ) )
            {
                return mPermissionManager.checkPermissionAllowed( pSourceConnection->GetRakNetGUID(), 
                    "ObjectManager_DestroyOwnComponentOnOwnObject" );
            }
            else
            {
                return mPermissionManager.checkPermissionAllowed( pSourceConnection->GetRakNetGUID(), 
                    "ObjectManager_DestroyOwnComponentOnOtherObject" );
            }
        }
        else
        {
            if( ServerComponent::getServerObject().isCreatedBySource( 
                pSourceConnection->GetRakNetGUID() ) )
            {
                return mPermissionManager.checkPermissionAllowed( pSourceConnection->GetRakNetGUID(), 
                    "ObjectManager_DestroyOtherComponentOnOwnObject" );
            }
            else
            {
                return mPermissionManager.checkPermissionAllowed( pSourceConnection->GetRakNetGUID(), 
                    "ObjectManager_DestroyOtherComponentOnOtherObject" );
            }
        }
    }

    DivAssert( 0, "Should not reach here" );
    return false;
}

void ServerComponent::SerializeConstruction( RakNet::BitStream* pConstructionBitstream, 
    RakNet::Connection_RM3* pDestinationConnection )
{
    PropertySynchronization::doSerializeConstruction( pConstructionBitstream, 
        pDestinationConnection );
}

bool ServerComponent::DeserializeConstruction( RakNet::BitStream* pConstructionBitstream, 
    RakNet::Connection_RM3* pSourceConnection )
{
    PropertySynchronization::doDeserializeConstruction( pConstructionBitstream, pSourceConnection );
    return true;
}

RakNet::RM3SerializationResult ServerComponent::Serialize( 
    RakNet::SerializeParameters* pSerializeParameters )
{
    return PropertySynchronization::doSerialize( pSerializeParameters );
}

void ServerComponent::Deserialize( RakNet::DeserializeParameters* pDeserializeParameters )
{
    PropertySynchronization::doDeserialize( pDeserializeParameters );
}

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia