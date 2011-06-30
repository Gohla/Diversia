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

#include "Object/ComponentFactoryManager.h"
#include "Object/ComponentFactory.h"
#include "Client/Object/ClientComponent.h"
#include "Client/Object/ClientObject.h"
#include "Client/Permission/PermissionManager.h"
#include "Shared/Camp/CampStringInterpreter.h"

namespace Diversia
{
namespace Client 
{
//------------------------------------------------------------------------------

ClientComponent::ClientComponent( const String& rName, Mode mode, NetworkingType networkingType, 
    ComponentType type, RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject ):
    Component( rName, mode, networkingType, type, source, localOverride, rObject ),
    PropertySynchronization( mode ),
    mPermissionManager( rObject.getPermissionManager() )
{
    if( Component::getNetworkingType() == REMOTE && !Component::getLocalOverride() &&
        Component::isCreatedByOwnGUID() )
    {
        // Creating remote component on this client.
        mPermissionManager.getPermission( "ObjectManager_CreateRemoteComponent" ).addItem();

        if( rObject.isCreatedBySource( source ) )
        {
            mPermissionManager.getPermission(
                "ObjectManager_CreateRemoteComponentOnOwnObject" ).addItem();
        }
        else
        {
            mPermissionManager.getPermission( 
                "ObjectManager_CreateRemoteComponentOnOtherObject" ).addItem();
        }
    }
    else if( Component::getNetworkingType() == LOCAL || Component::getLocalOverride() )
    {
        // Creating local component.
        mPermissionManager.getPermission( "ObjectManager_CreateLocalComponent" ).addItem();
    }

    if( Component::isCreatedByOwnGUID() )
    {
        // Permission name: <Component>_Create
        mPermissionManager.getPermission( ComponentFactoryManager::getComponentFactory( 
            mType ).getTypeName() + String( "_Create" ) ).addItem();
    }
}

ClientComponent::~ClientComponent()
{
    if( Component::getNetworkingType() == REMOTE && !Component::getLocalOverride() &&
        Component::isCreatedByOwnGUID() )
    {
        // Destroying remote component created by this client.
        mPermissionManager.getPermission( "ObjectManager_CreateRemoteComponent" ).removeItem();

        if( ClientComponent::getClientObject().isCreatedBySource( Component::getSourceGUID() ) )
        {
            mPermissionManager.getPermission(  
                "ObjectManager_CreateRemoteComponentOnOwnObject" ).removeItem();
        }
        else
        {
            mPermissionManager.getPermission( 
                "ObjectManager_CreateRemoteComponentOnOtherObject" ).removeItem();
        }
    }
    else if( Component::getNetworkingType() == LOCAL || Component::getLocalOverride() )
    {
        // Destroying local component.
        mPermissionManager.getPermission( "ObjectManager_CreateLocalComponent" ).removeItem();
    }

    if( Component::isCreatedByOwnGUID() )
    {
        // Permission name: <Component>_Create
        mPermissionManager.getPermission( ComponentFactoryManager::getComponentFactory( 
            mType ).getTypeName() + String( "_Create" ) ).removeItem();
    }
}

void ClientComponent::setNetworkingTypeImpl( NetworkingType type )
{
    // If the component is overridden to local networking mode, changing the network type has no 
    // influence on this component, so no permissions need to change.
    //if( Component::getLocalOverride() ) return;

    if( type == REMOTE )
    {
        if( ClientComponent::getClientObject().isCreatedByOwnGUID() )
        {
            mPermissionManager.getPermission( 
                "ObjectManager_CreateRemoteComponentOnOwnObject" ).addItem();
        }
        else
        {
            mPermissionManager.getPermission( 
                "ObjectManager_CreateRemoteComponentOnOtherObject" ).addItem();
        }

        mPermissionManager.getPermission( "ObjectManager_CreateRemoteComponent" ).addItem();
        mPermissionManager.getPermission( "ObjectManager_CreateLocalComponent" ).removeItem();
    }
    else if( type == LOCAL )
    {
        if( ClientComponent::getClientObject().isCreatedByOwnGUID() )
        {
            mPermissionManager.getPermission( 
                "ObjectManager_CreateRemoteComponentOnOwnObject" ).removeItem();
        }
        else
        {
            mPermissionManager.getPermission( 
                "ObjectManager_CreateRemoteComponentOnOtherObject" ).removeItem();
        }

        mPermissionManager.getPermission( "ObjectManager_CreateRemoteComponent" ).removeItem();
        mPermissionManager.getPermission( "ObjectManager_CreateLocalComponent" ).addItem();
    }
}

void ClientComponent::querySetNetworkingTypeImpl( NetworkingType type )
{
    // If the component is overridden to local networking mode, changing the network type has no 
    // influence on this component, so always allow it.
    if( Component::getLocalOverride() ) return;

    if( type == REMOTE )
    {
        // Change networking type from local to remote.
        mPermissionManager.checkPermissionThrows( "ObjectManager_CreateRemoteComponent", 
            "ClientComponent::setNetworkingType" );

        if( ClientComponent::getClientObject().isCreatedByOwnGUID() )
        {
            mPermissionManager.checkPermissionThrows(
                "ObjectManager_CreateRemoteComponentOnOwnObject", 
                "ClientComponent::setNetworkingType" );

            mPermissionManager.getPermission( 
                "ObjectManager_CreateRemoteComponentOnOwnObject" ).addItem();
        }
        else
        {
            mPermissionManager.checkPermissionThrows( 
                "ObjectManager_CreateRemoteComponentOnOtherObject", 
                "ClientComponent::setNetworkingType" );

            mPermissionManager.getPermission( 
                "ObjectManager_CreateRemoteComponentOnOtherObject" ).addItem();
        }

        mPermissionManager.getPermission( "ObjectManager_CreateRemoteComponent" ).addItem();
        mPermissionManager.getPermission( "ObjectManager_CreateLocalComponent" ).removeItem( true );
    }
    else if( type == LOCAL )
    {
        // Change networking type from remote to local.
        mPermissionManager.checkPermissionThrows( "ObjectManager_CreateLocalComponent", 
            "ClientComponent::setNetworkingType" );

        if( ClientComponent::getClientObject().isCreatedByOwnGUID() )
        {
            mPermissionManager.getPermission( 
                "ObjectManager_CreateRemoteComponentOnOwnObject" ).removeItem( true );
        }
        else
        {
            mPermissionManager.getPermission( 
                "ObjectManager_CreateRemoteComponentOnOtherObject" ).removeItem( true );
        }

        mPermissionManager.getPermission( "ObjectManager_CreateRemoteComponent" ).removeItem( true );
        mPermissionManager.getPermission( "ObjectManager_CreateLocalComponent" ).addItem();
    }
}

void ClientComponent::cleanupQuerySetNetworkingType( NetworkingType type )
{
    // If the component is overridden to local networking mode, changing the network type has no 
    // influence on this component, so there is nothing to clean up.
    if( Component::getLocalOverride() ) return;

    if( type == REMOTE /*&& !Component::getLocalOverride()*/ )
    {
        if( ClientComponent::getClientObject().isCreatedByOwnGUID() )
        {
            mPermissionManager.getPermission( 
                "ObjectManager_CreateRemoteComponentOnOwnObject" ).removeItem( true );
        }
        else
        {
            mPermissionManager.getPermission( 
                "ObjectManager_CreateRemoteComponentOnOtherObject" ).removeItem( true );
        }

        mPermissionManager.getPermission( "ObjectManager_CreateRemoteComponent" ).removeItem( true );
        mPermissionManager.getPermission( "ObjectManager_CreateLocalComponent" ).addItem();
    }
    else if( type == LOCAL )
    {
        if( ClientComponent::getClientObject().isCreatedByOwnGUID() )
        {
            mPermissionManager.getPermission( 
                "ObjectManager_CreateRemoteComponentOnOwnObject" ).addItem();
        }
        else
        {
            mPermissionManager.getPermission( 
                "ObjectManager_CreateRemoteComponentOnOtherObject" ).addItem();
        }

        mPermissionManager.getPermission( "ObjectManager_CreateRemoteComponent" ).addItem();
        mPermissionManager.getPermission( "ObjectManager_CreateLocalComponent" ).removeItem( true );
    }
}

void ClientComponent::querySetProperty( const String& rQuery, camp::Value& rValue )
{
    // Permission name: SetPropertyOn<Ownership><ComponentName>Component_<PropertyName>
    mPermissionManager.checkPermissionThrows( String( "SetPropertyOn" ) + 
        String( Component::isCreatedByOwnGUID() ? "Own" : "Other" ) + getTypeName() + 
        String( "Component_" ) + CampStringInterpreter::removeArrayIdentifiers( rQuery ), 
        rValue, "ClientComponent::querySetProperty" );
}

void ClientComponent::queryInsertProperty( const String& rQuery, camp::Value& rValue )
{
    // Permission name: InsertValueIn<Ownership><ComponentName>Component_<PropertyName>
    mPermissionManager.checkPermissionThrows( String( "InsertValueIn" ) + 
        String( Component::isCreatedByOwnGUID() ? "Own" : "Other" ) + getTypeName() + 
        String( "Component_" ) + CampStringInterpreter::removeArrayIdentifiers( rQuery ), 
        rValue, "ClientComponent::queryInsertProperty" );
}

RakNet::RM3ConstructionState ClientComponent::QueryConstruction( 
    RakNet::Connection_RM3* pDestinationConnection, RakNet::ReplicaManager3* pReplicaManager3 )
{
    // Always allow, permission checking is done in Object.
    return Replica3::QueryConstruction_ClientConstruction( pDestinationConnection,
        Component::getMode() == SERVER ? true : false );
}

bool ClientComponent::QueryRemoteConstruction( RakNet::Connection_RM3* pSourceConnection )
{
    // Always allow the server to create components.
    return true;
}

RakNet::RM3QuerySerializationResult ClientComponent::QuerySerialization( 
    RakNet::Connection_RM3* pDestinationConnection )
{
    // Allow serializations for components created by this client. Override and replace this 
    // behavior if needed.
    if( Component::isCreatedByOwnGUID() )
        return Replica3::QuerySerialization_ClientSerializable( pDestinationConnection, 
        Component::getMode() == SERVER ? true : false );
    else
        return Replica3::QuerySerialization_ServerSerializable( pDestinationConnection, 
        Component::getMode() == SERVER ? true : false );
}

void ClientComponent::SerializeConstruction( RakNet::BitStream* pConstructionBitstream, 
    RakNet::Connection_RM3* pDestinationConnection )
{
    PropertySynchronization::doSerializeConstruction( pConstructionBitstream, 
        pDestinationConnection );
}

bool ClientComponent::DeserializeConstruction( RakNet::BitStream* pConstructionBitstream, 
    RakNet::Connection_RM3* pSourceConnection )
{
    PropertySynchronization::doDeserializeConstruction( pConstructionBitstream, pSourceConnection );
    return true;
}

RakNet::RM3SerializationResult ClientComponent::Serialize( 
    RakNet::SerializeParameters* pSerializeParameters )
{
    return PropertySynchronization::doSerialize( pSerializeParameters );
}

void ClientComponent::Deserialize( RakNet::DeserializeParameters* pDeserializeParameters )
{
    PropertySynchronization::doDeserialize( pDeserializeParameters );
}

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia