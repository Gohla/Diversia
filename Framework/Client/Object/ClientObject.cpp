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
#include "Client/Object/ClientObject.h"
#include "Client/Object/ClientComponent.h"
#include "Client/Permission/PermissionManager.h"

namespace Diversia
{
namespace Client 
{
//------------------------------------------------------------------------------

ClientObject::ClientObject( const String& rName, Mode mode, NetworkingType type, 
    const String& rDisplayName, RakNet::RakNetGUID source, RakNet::RakNetGUID ownGUID, 
    RakNet::RakNetGUID serverGUID, sigc::signal<void>& rUpdateSignal, 
    ClientObjectManager& rObjectManager, PermissionManager& rPermissionManager,  
    RakNet::ReplicaManager3& rReplicaManager, RakNet::NetworkIDManager& rNetworkIDManager,
    RakNet::RPC3& rRPC3 ):
    Object( rName, mode, type, rDisplayName, source, ownGUID, serverGUID, rUpdateSignal, 
        rObjectManager, rReplicaManager, rNetworkIDManager, rRPC3 ),
    mPermissionManager( rPermissionManager ),
    mSelected( false )
{
    if( Object::getNetworkingType() == REMOTE && Object::isCreatedByOwnGUID() )
    {
        // Add to item counter if this object is created by us.
        mPermissionManager.getPermission( "ObjectManager_CreateRemoteObject" ).addItem();
    }
    else if( Object::getNetworkingType() == LOCAL )
    {
        mPermissionManager.getPermission( "ObjectManager_CreateLocalObject" ).addItem();
    }
}

ClientObject::~ClientObject()
{
    if( Object::getNetworkingType() == REMOTE && Object::isCreatedByOwnGUID() )
    {
        // Remove from item counter if this object is created by us.
        mPermissionManager.getPermission( "ObjectManager_CreateRemoteObject" ).removeItem();
    }
    else if( Object::getNetworkingType() == LOCAL )
    {
        mPermissionManager.getPermission( "ObjectManager_CreateLocalObject" ).removeItem();
    }
}

void ClientObject::setSelected( bool selected )
{
    if( selected == mSelected ) return;

    mSelected = selected;

    // Notify components
    for( ComponentsByName::const_iterator i = Object::getComponentsByName().begin(); 
        i != Object::getComponentsByName().end(); ++i )
    {
        static_cast<ClientComponent*>( i->second )->setSelected( selected );
    }
}

void ClientObject::queryCreateComponent( ComponentType type, RakNet::RakNetGUID source, bool localOverride )
{
    // Only check permission if this client is creating the component.
    if( source != Object::getServerGUID() )
    {
        if( ClientObject::getNetworkingType() == REMOTE && !localOverride )
        {
            mPermissionManager.checkPermissionThrows( "ObjectManager_CreateRemoteComponent", 
                "ClientObject::queryCreateComponent" );

            if( Object::isCreatedBy( source ) )
            {
                mPermissionManager.checkPermissionThrows( 
                    "ObjectManager_CreateRemoteComponentOnOwnObject", 
                    "ClientObject::queryCreateComponent" );
            }
            else
            {
                mPermissionManager.checkPermissionThrows( 
                    "ObjectManager_CreateRemoteComponentOnOtherObject", 
                    "ClientObject::queryCreateComponent" );
            }
        }
        else if( ClientObject::getNetworkingType() == LOCAL || localOverride )
        {
            mPermissionManager.checkPermissionThrows( "ObjectManager_CreateLocalComponent", 
                "ClientObject::queryCreateComponent" );
        }

        // Permission name: <Component>_Create
        mPermissionManager.checkPermissionThrows( 
            ComponentFactoryManager::getComponentFactory( type ).getTypeName() + String( "_Create" ), 
            "ClientObject::queryCreateComponent" );
    }
}

void ClientObject::queryDestroyComponent( Component& rComponent, RakNet::RakNetGUID source )
{
    // Only check permission if this client is destroying the component.
    if( source != Object::getServerGUID() )
    {
        if( rComponent.getNetworkingType() == REMOTE && !rComponent.getLocalOverride() )
        {
            if( rComponent.isCreatedBy( source ) )
            {
                if( Object::isCreatedBy( source ) )
                {
                    mPermissionManager.checkPermissionThrows( 
                        "ObjectManager_DestroyOwnComponentOnOwnObject", 
                        "ClientObject::queryDestroyComponent" );
                }
                else
                {
                    mPermissionManager.checkPermissionThrows( 
                        "ObjectManager_DestroyOwnComponentOnOtherObject", 
                        "ClientObject::queryDestroyComponent" );
                }
            }
            else
            {
                if( Object::isCreatedBy( source ) )
                {
                    mPermissionManager.checkPermissionThrows( 
                        "ObjectManager_DestroyOtherComponentOnOwnObject", 
                        "ClientObject::queryDestroyComponent" );
                }
                else
                {
                    mPermissionManager.checkPermissionThrows( 
                        "ObjectManager_DestroyOtherComponentOnOtherObject", 
                        "ClientObject::queryDestroyComponent" );
                }
            }
        }
        else if( rComponent.getNetworkingType() == LOCAL || 
            rComponent.getLocalOverride() )
        {
            mPermissionManager.checkPermissionThrows( 
                "ObjectManager_DestroyLocalComponent", 
                "ClientObject::queryDestroyComponent" );
        }
    }
}

RakNet::RM3ConstructionState ClientObject::QueryConstruction( 
    RakNet::Connection_RM3* pDestinationConnection, RakNet::ReplicaManager3* pReplicaManager3 )
{
    // Always allow, permission checking is done in ObjectManager.
    return Replica3::QueryConstruction_ClientConstruction( pDestinationConnection, 
        Object::getMode() == SERVER ? true : false );
}

bool ClientObject::QueryRemoteConstruction( RakNet::Connection_RM3* pSourceConnection )
{
    // Always allow the server to create objects.
    return true;
}

void ClientObject::setNetworkingTypeImpl( NetworkingType type )
{
    if( type == REMOTE )
    {
        mPermissionManager.getPermission( "ObjectManager_CreateLocalObject" ).removeItem();
        mPermissionManager.getPermission( "ObjectManager_CreateRemoteObject" ).addItem();
    }
    else if( type == LOCAL )
    {
        mPermissionManager.getPermission( "ObjectManager_CreateRemoteObject" ).removeItem();
        mPermissionManager.getPermission( "ObjectManager_CreateLocalObject" ).addItem();
    }
}

void ClientObject::querySetNetworkingTypeImpl( NetworkingType type )
{
    if( type == REMOTE )
    {
        mPermissionManager.checkPermissionThrows( "ObjectManager_CreateRemoteObject", 
            "ClientObject::setNetworkingType" );

        // Simulate setting object to remote, changes will be cleaned up later.
        mPermissionManager.getPermission( "ObjectManager_CreateLocalObject" ).removeItem( true );
        mPermissionManager.getPermission( "ObjectManager_CreateRemoteObject" ).addItem();
    }
    else if( type == LOCAL )
    {
        // Disallow setting someone else's object to local mode.
        if( !Object::isCreatedByOwnGUID() )
        {
            DIVERSIA_EXCEPT( Exception::ERR_PERMISSION_DENIED, 
                "Permission denied, cannot set a non-owned object to local networking mode.", 
                "ClientObject::querySetNetworkingTypeImpl" );
        }

        mPermissionManager.checkPermissionThrows( "ObjectManager_CreateLocalObject", 
            "ClientObject::setNetworkingType" );

        // Simulate setting object to local, changes will be cleaned up later.
        mPermissionManager.getPermission( "ObjectManager_CreateRemoteObject" ).removeItem( true );
        mPermissionManager.getPermission( "ObjectManager_CreateLocalObject" ).addItem();
    }
}

void ClientObject::cleanupQuerySetNetworkingType( NetworkingType type )
{
    if( type == REMOTE )
    {
        mPermissionManager.getPermission( "ObjectManager_CreateLocalObject" ).addItem();
        mPermissionManager.getPermission( "ObjectManager_CreateRemoteObject" ).removeItem( true );
    }
    else if( type == LOCAL )
    {
        mPermissionManager.getPermission( "ObjectManager_CreateRemoteObject" ).addItem();
        mPermissionManager.getPermission( "ObjectManager_CreateLocalObject" ).removeItem( true );
    }
}

void ClientObject::querySetParent( Object* pNewParent, RakNet::RakNetGUID source )
{
    // The server may always change the parent.
    if( source == Object::getServerGUID() ) return;

    if( !pNewParent )
    {
        if( Object::isCreatedByOwnGUID() )
            mPermissionManager.checkPermissionThrows( "Object_UnparentOnOwnObject", 
                "ClientObject::querySetParent" );
        else
            mPermissionManager.checkPermissionThrows( "Object_UnparentOnOtherObject",
                "ClientObject::querySetParent" );
    }
    else
    {
        if( Object::isCreatedByOwnGUID() )
        {
            if( pNewParent->isCreatedByOwnGUID() )
                mPermissionManager.checkPermissionThrows( "Object_SetOwnParentOnOwnObject", 
                    "ClientObject::querySetParent" );
            else
                mPermissionManager.checkPermissionThrows( "Object_SetOtherParentOnOwnObject", 
                    "ClientObject::querySetParent" );
        }
        else
        {
            if( pNewParent->isCreatedByOwnGUID() )
                mPermissionManager.checkPermissionThrows( "Object_SetOwnParentOnOtherObject", 
                    "ClientObject::querySetParent" );
            else
                mPermissionManager.checkPermissionThrows( "Object_SetOtherParentOnOtherObject", 
                    "ClientObject::querySetParent" );
        }
    }
}

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia