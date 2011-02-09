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

#include "Object/ServerObject.h"
#include "Object/ServerObjectManager.h"
#include "Object/ComponentFactoryManager.h"
#include "Object/ComponentFactory.h"
#include "Object/Component.h"

#include "Permission/PermissionManager.h"

namespace Diversia
{
namespace Server 
{
//------------------------------------------------------------------------------

ServerObject::ServerObject( const String& rName, Mode mode, NetworkingType type, 
    const String& rDisplayName, RakNet::RakNetGUID source, RakNet::RakNetGUID ownGUID,
    RakNet::RakNetGUID serverGUID, sigc::signal<void>& rUpdateSignal, 
    ServerObjectManager& rObjectManager, RakNet::ReplicaManager3& rReplicaManager, 
    RakNet::NetworkIDManager& rNetworkIDManager, PermissionManager& rPermissionManager, 
    RakNet::RPC3& rRPC3 ):
    Object( rName, mode, type, rDisplayName, source, ownGUID, serverGUID, rUpdateSignal, 
        rObjectManager, rReplicaManager, rNetworkIDManager, rRPC3 ),
    mPermissionManager( rPermissionManager )
{
    if( ServerObject::getNetworkingType() == REMOTE && !Object::isCreatedByServer() )
    {
        // Add to the user's item counter if this object is created by a client.
        mPermissionManager.getPermission( Object::getSourceGUID(), 
            "ObjectManager_CreateRemoteObject" ).addItem();
    }
}

ServerObject::~ServerObject()
{
    if( ServerObject::getNetworkingType() == REMOTE && !Object::isCreatedByServer() )
    {
        // Remote from the user's item counter if this object is created by a client.
        mPermissionManager.getPermission( Object::getSourceGUID(), 
            "ObjectManager_CreateRemoteObject" ).removeItem();
    }
}

void ServerObject::queryCreateComponent( ComponentType type, RakNet::RakNetGUID source, 
    bool localOverride )
{
    // Only check permission if a client is creating the component.
    if( source != Object::getServerGUID() && ServerObject::getNetworkingType() == 
        REMOTE && !localOverride )
    {
        mPermissionManager.checkPermissionThrows( source, "ObjectManager_CreateRemoteComponent", 
            "ServerObject::queryCreateComponent" );

        if( Object::isCreatedBy( source ) )
        {
            mPermissionManager.checkPermissionThrows( source, 
                "ObjectManager_CreateRemoteComponentOnOwnObject", 
                "ServerObject::queryCreateComponent" );
        }
        else
        {
            mPermissionManager.checkPermissionThrows( source, 
                "ObjectManager_CreateRemoteComponentOnOtherObject", 
                "ServerObject::queryCreateComponent" );
        }

        // Permission name: <Component>_Create
        mPermissionManager.checkPermissionThrows( source, 
            ComponentFactoryManager::getComponentFactory( type ).getTypeName() + String( "_Create" ), 
            "ServerObject::queryCreateComponent" );
    }
}

void ServerObject::queryDestroyComponent( Component& rComponent, RakNet::RakNetGUID source )
{
    // Only check permission if this client is destroying the component.
    if( source != Object::getServerGUID() && rComponent.getNetworkingType() == 
        REMOTE && !rComponent.getLocalOverride() )
    {
        if( rComponent.isCreatedBySource( source ) )
        {
            if( Object::isCreatedBySource( source ) )
            {
                mPermissionManager.checkPermissionThrows( source, 
                    "ObjectManager_DestroyOwnComponentOnOwnObject", 
                    "ClientObject::queryDestroyComponent" );
            }
            else
            {
                mPermissionManager.checkPermissionThrows( source, 
                    "ObjectManager_DestroyOwnComponentOnOtherObject", 
                    "ClientObject::queryDestroyComponent" );
            }
        }
        else
        {
            if( Object::isCreatedBySource( source ) )
            {
                mPermissionManager.checkPermissionThrows( source, 
                    "ObjectManager_DestroyOtherComponentOnOwnObject", 
                    "ClientObject::queryDestroyComponent" );
            }
            else
            {
                mPermissionManager.checkPermissionThrows( source, 
                    "ObjectManager_DestroyOtherComponentOnOtherObject", 
                    "ClientObject::queryDestroyComponent" );
            }
        }
    }
}

RakNet::RM3ConstructionState ServerObject::QueryConstruction( 
    RakNet::Connection_RM3* pDestinationConnection, RakNet::ReplicaManager3* pReplicaManager3 )
{
    // Always allow the server to create objects.
    return Replica3::QueryConstruction_ClientConstruction( pDestinationConnection,
        Object::getMode() == SERVER ? true : false );
}

bool ServerObject::QueryRemoteConstruction( RakNet::Connection_RM3* pSourceConnection )
{
    // Always allow, permission checking is done in ObjectManager.
    return true;
}

bool ServerObject::DeserializeDestruction( RakNet::BitStream* pDestructionBitstream, 
    RakNet::Connection_RM3* pSourceConnection )
{
    // This is only called if a client wants to destroy an object.
    if( Object::isCreatedBySource( pSourceConnection->GetRakNetGUID() ) )
    {
        return mPermissionManager.checkPermissionAllowed( pSourceConnection->GetRakNetGUID(), 
            "ObjectManager_DestroyOwnObject" );
    }
    else
    {
        return mPermissionManager.checkPermissionAllowed( pSourceConnection->GetRakNetGUID(), 
            "ObjectManager_DestroyOtherObject" );
    }

    DivAssert( 0, "Should not reach here" );
    return false;
}

void ServerObject::setNetworkingTypeImpl( NetworkingType type )
{
    // Networking type may always be changed on the server.
    if( Object::getMode() == SERVER )
        return;

    // Changing networking type is always allowed on the server because only the server can call 
    // this function. We do however need to add/remove to/from the object amount if the networking
    // type changes for consistency.
    if( type == REMOTE )
    {
        // Change networking type from local to remote.
        mPermissionManager.getPermission( Object::getSourceGUID(), 
            "ObjectManager_CreateLocalObject" ).removeItem();
        mPermissionManager.getPermission( Object::getSourceGUID(), 
            "ObjectManager_CreateRemoteObject" ).addItem();
    }
    else if( type == LOCAL )
    {
        // Change networking type from remote to local.
        mPermissionManager.getPermission( Object::getSourceGUID(), 
            "ObjectManager_CreateRemoteObject" ).removeItem();
        mPermissionManager.getPermission( Object::getSourceGUID(), 
            "ObjectManager_CreateLocalObject" ).addItem();
    }
}

void ServerObject::querySetParent( Object* pNewParent, RakNet::RakNetGUID source )
{
    // The server may always change the parent.
    if( source == Object::getServerGUID() ) return;

    if( !pNewParent )
    {
        if( Object::isCreatedBySource( source ) )
            mPermissionManager.checkPermissionThrows( source, "Object_UnparentOnOwnObject", 
            "ServerObject::querySetParent" );
        else
            mPermissionManager.checkPermissionThrows( source, "Object_UnparentOnOtherObject",
            "ServerObject::querySetParent" );
    }
    else
    {
        if( Object::isCreatedBySource( source ) )
        {
            if( pNewParent->isCreatedBySource( source ) )
                mPermissionManager.checkPermissionThrows( source, 
                "Object_SetOwnParentOnOwnObject", 
                "ServerObject::querySetParent" );
            else
                mPermissionManager.checkPermissionThrows( source, 
                "Object_SetOtherParentOnOwnObject", 
                "ServerObject::querySetParent" );
        }
        else
        {
            if( pNewParent->isCreatedBySource( source ) )
                mPermissionManager.checkPermissionThrows( source, 
                "Object_SetOwnParentOnOtherObject", 
                "ServerObject::querySetParent" );
            else
                mPermissionManager.checkPermissionThrows( source, 
                "Object_SetOtherParentOnOtherObject", 
                "ServerObject::querySetParent" );
        }
    }
}

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia