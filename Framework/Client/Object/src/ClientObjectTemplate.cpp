/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include "Client/Platform/StableHeaders.h"

#include "Client/Object/ClientComponentTemplate.h"
#include "Client/Object/ClientObjectTemplate.h"
#include "Client/Permission/PermissionManager.h"
#include "Object/ComponentFactory.h"
#include "Object/ComponentFactoryManager.h"
#include "Shared/Camp/CampStringInterpreter.h"

namespace Diversia
{
namespace Client 
{
//------------------------------------------------------------------------------

ClientObjectTemplate::ClientObjectTemplate( const String& rName, Mode mode, NetworkingType type,
    const String& rDisplayName, RakNet::RakNetGUID source, RakNet::RakNetGUID ownGUID, 
    RakNet::RakNetGUID serverGUID, ObjectTemplateManager& rObjectTemplateManager, 
    PermissionManager& rPermissionManager, RakNet::ReplicaManager3& rReplicaManager, 
    RakNet::NetworkIDManager& rNetworkIDManager ):
    ObjectTemplate( rName, mode, type, rDisplayName, source, ownGUID, serverGUID, 
        rObjectTemplateManager, rReplicaManager, rNetworkIDManager ),
    PropertySynchronization( mode ),
    mPermissionManager( rPermissionManager )
{
    PropertySynchronization::storeUserObject();

    if( ObjectTemplate::getNetworkingType() == REMOTE && ObjectTemplate::isCreatedByOwnGUID() )
    {
        // Add to item counter if this object is created by us.
        mPermissionManager.getPermission( "ObjectTemplateManager_CreateRemoteObjectTemplate" ).addItem();
    }
    else if( ObjectTemplate::getNetworkingType() == LOCAL )
    {
        mPermissionManager.getPermission( "ObjectTemplateManager_CreateLocalObjectTemplate" ).addItem();
    }
}

ClientObjectTemplate::~ClientObjectTemplate()
{
    if( ObjectTemplate::getNetworkingType() == REMOTE && ObjectTemplate::isCreatedByOwnGUID() )
    {
        // Remove from item counter if this object is created by us.
        mPermissionManager.getPermission( "ObjectTemplateManager_CreateRemoteObjectTemplate" ).removeItem();
    }
    else if( ObjectTemplate::getNetworkingType() == LOCAL )
    {
        mPermissionManager.getPermission( "ObjectTemplateManager_CreateLocalObjectTemplate" ).removeItem();
    }
}

void ClientObjectTemplate::queryCreateComponentTemplate( ComponentType type, bool localOverride, 
    RakNet::RakNetGUID source )
{
    // Only check permission if this client is creating the component template.
    if( source != ObjectTemplate::getServerGUID() )
    {
        if( ClientObjectTemplate::getNetworkingType() == REMOTE && !localOverride )
        {
            mPermissionManager.checkPermissionThrows( "ObjectTemplateManager_CreateRemoteComponentTemplate", 
                "ClientObjectTemplate::queryCreateComponentTemplate" );

            if( ObjectTemplate::isCreatedBy( source ) )
            {
                mPermissionManager.checkPermissionThrows( 
                    "ObjectTemplateManager_CreateRemoteComponentOnOwnObjectTemplate", 
                    "ClientObjectTemplate::queryCreateComponentTemplate" );
            }
            else
            {
                mPermissionManager.checkPermissionThrows( 
                    "ObjectTemplateManager_CreateRemoteComponentOnOtherObjectTemplate", 
                    "ClientObjectTemplate::queryCreateComponentTemplate" );
            }
        }
        else if( ClientObjectTemplate::getNetworkingType() == LOCAL || localOverride )
        {
            mPermissionManager.checkPermissionThrows( "ObjectTemplateManager_CreateLocalComponentTemplate", 
                "ClientObjectTemplate::queryCreateComponentTemplate" );
        }

        // Permission name: <Component>Template_Create
        mPermissionManager.checkPermissionThrows( 
            ComponentFactoryManager::getComponentFactory( type ).getTypeName() + String( "Template_Create" ), 
            "ClientObjectTemplate::queryCreateComponentTemplate" );
    }
}

ComponentTemplate* ClientObjectTemplate::createComponentTemplateImpl( ComponentType type, 
    const String& rName, bool localOverride, RakNet::RakNetGUID source )
{
    return new ClientComponentTemplate( rName, ObjectTemplate::getMode(), 
        ObjectTemplate::getNetworkingType(), type, source, localOverride, *this );
}

void ClientObjectTemplate::queryDestroyComponentTemplate( ComponentTemplate& rTemplate, 
    RakNet::RakNetGUID source )
{
    // Only check permission if this client is destroying the component.
    if( source != ObjectTemplate::getServerGUID() )
    {
        if( rTemplate.getNetworkingType() == REMOTE && !rTemplate.getLocalOverride() )
        {
            if( rTemplate.isCreatedBy( source ) )
            {
                if( ObjectTemplate::isCreatedBy( source ) )
                {
                    mPermissionManager.checkPermissionThrows( 
                        "ObjectTemplateManager_DestroyOwnComponentOnOwnObjectTemplate", 
                        "ClientObjectTemplate::queryDestroyComponentTemplate" );
                }
                else
                {
                    mPermissionManager.checkPermissionThrows( 
                        "ObjectTemplateManager_DestroyOwnComponentOnOtherObjectTemplate", 
                        "ClientObjectTemplate::queryDestroyComponentTemplate" );
                }
            }
            else
            {
                if( ObjectTemplate::isCreatedBy( source ) )
                {
                    mPermissionManager.checkPermissionThrows( 
                        "ObjectTemplateManager_DestroyOtherTemplateOnOwnObjectTemplate", 
                        "ClientObjectTemplate::queryDestroyComponentTemplate" );
                }
                else
                {
                    mPermissionManager.checkPermissionThrows( 
                        "ObjectTemplateManager_DestroyOtherTemplateOnOtherObjectTemplate", 
                        "ClientObjectTemplate::queryDestroyComponentTemplate" );
                }
            }
        }
        else if( rTemplate.getNetworkingType() == LOCAL || 
            rTemplate.getLocalOverride() )
        {
            mPermissionManager.checkPermissionThrows( 
                "ObjectTemplateManager_DestroyLocalComponentTemplate", 
                "ClientObjectTemplate::queryDestroyComponentTemplate" );
        }
    }
}

void ClientObjectTemplate::setNetworkingTypeImpl( NetworkingType type )
{
    if( type == REMOTE )
    {
        mPermissionManager.getPermission( "ObjectTemplateManager_CreateLocalObjectTemplate" ).removeItem();
        mPermissionManager.getPermission( "ObjectTemplateManager_CreateRemoteObjectTemplate" ).addItem();
    }
    else if( type == LOCAL )
    {
        mPermissionManager.getPermission( "ObjectTemplateManager_CreateRemoteObjectTemplate" ).removeItem();
        mPermissionManager.getPermission( "ObjectTemplateManager_CreateLocalObjectTemplate" ).addItem();
    }
}

void ClientObjectTemplate::querySetNetworkingTypeImpl( NetworkingType type )
{
    if( type == REMOTE )
    {
        mPermissionManager.checkPermissionThrows( "ObjectTemplateManager_CreateRemoteObjectTemplate", 
            "ClientObjectTemplate::setNetworkingType" );

        // Simulate setting object to remote, changes will be cleaned up later.
        mPermissionManager.getPermission( "ObjectTemplateManager_CreateLocalObjectTemplate" ).removeItem( true );
        mPermissionManager.getPermission( "ObjectTemplateManager_CreateRemoteObjectTemplate" ).addItem();
    }
    else if( type == LOCAL )
    {
        // Disallow setting someone else's object to local mode.
        if( !ObjectTemplate::isCreatedByOwnGUID() )
        {
            DIVERSIA_EXCEPT( Exception::ERR_PERMISSION_DENIED, 
                "Permission denied, cannot set a non-owned object template to local networking mode.", 
                "ClientObjectTemplate::querySetNetworkingTypeImpl" );
        }

        mPermissionManager.checkPermissionThrows( "ObjectTemplateManager_CreateLocalObjectTemplate", 
            "ClientObjectTemplate::setNetworkingType" );

        // Simulate setting object to local, changes will be cleaned up later.
        mPermissionManager.getPermission( "ObjectTemplateManager_CreateRemoteObjectTemplate" ).removeItem( true );
        mPermissionManager.getPermission( "ObjectTemplateManager_CreateLocalObjectTemplate" ).addItem();
    }
}

void ClientObjectTemplate::cleanupQuerySetNetworkingType( NetworkingType type )
{
    if( type == REMOTE )
    {
        mPermissionManager.getPermission( "ObjectTemplateManager_CreateLocalObjectTemplate" ).addItem();
        mPermissionManager.getPermission( "ObjectTemplateManager_CreateRemoteObjectTemplate" ).removeItem( true );
    }
    else if( type == LOCAL )
    {
        mPermissionManager.getPermission( "ObjectTemplateManager_CreateRemoteObjectTemplate" ).addItem();
        mPermissionManager.getPermission( "ObjectTemplateManager_CreateLocalObjectTemplate" ).removeItem( true );
    }
}

void ClientObjectTemplate::querySetParent( ObjectTemplate* pNewParent, RakNet::RakNetGUID source )
{
    // The server may always change the parent.
    if( source == ObjectTemplate::getServerGUID() ) return;

    if( !pNewParent )
    {
        if( ObjectTemplate::isCreatedByOwnGUID() )
            mPermissionManager.checkPermissionThrows( "ObjectTemplate_UnparentOnOwnObjectTemplate", 
            "ClientObjectTemplate::querySetParent" );
        else
            mPermissionManager.checkPermissionThrows( "ObjectTemplate_UnparentOnOtherObjectTemplate",
            "ClientObjectTemplate::querySetParent" );
    }
    else
    {
        if( ObjectTemplate::isCreatedByOwnGUID() )
        {
            if( pNewParent->isCreatedByOwnGUID() )
                mPermissionManager.checkPermissionThrows( "ObjectTemplate_SetOwnParentOnOwnObjectTemplate", 
                "ClientObjectTemplate::querySetParent" );
            else
                mPermissionManager.checkPermissionThrows( "ObjectTemplate_SetOtherParentOnOwnObjectTemplate", 
                "ClientObjectTemplate::querySetParent" );
        }
        else
        {
            if( pNewParent->isCreatedByOwnGUID() )
                mPermissionManager.checkPermissionThrows( "ObjectTemplate_SetOwnParentOnOtherObjectTemplate", 
                "ClientObjectTemplate::querySetParent" );
            else
                mPermissionManager.checkPermissionThrows( "ObjectTemplate_SetOtherParentOnOtherObjectTemplate", 
                "ClientObjectTemplate::querySetParent" );
        }
    }
}

void ClientObjectTemplate::querySetProperty( const String& rQuery, camp::Value& rValue )
{
    // Permission name: SetPropertyOn<Ownership>ObjectTemplate_<PropertyName>
    mPermissionManager.checkPermissionThrows( String( "SetPropertyOn" ) + 
        String( ObjectTemplate::isCreatedByOwnGUID() ? "Own" : "Other" ) + 
        String( "ObjectTemplate_" ) + CampStringInterpreter::removeArrayIdentifiers( rQuery ), 
        rValue, "ClientObjectTemplate::querySetProperty" );
}

void ClientObjectTemplate::queryInsertProperty( const String& rQuery, camp::Value& rValue )
{
    // Permission name: InsertValueIn<Ownership>ObjectTemplate_<PropertyName>
    mPermissionManager.checkPermissionThrows( String( "InsertValueIn" ) + 
        String( ObjectTemplate::isCreatedByOwnGUID() ? "Own" : "Other" ) + 
        String( "ObjectTemplate_" ) + CampStringInterpreter::removeArrayIdentifiers( rQuery ), 
        rValue, "ClientObjectTemplate::queryInsertProperty" );
}

RakNet::RM3ConstructionState ClientObjectTemplate::QueryConstruction( 
    RakNet::Connection_RM3* pDestinationConnection, RakNet::ReplicaManager3* pReplicaManager3 )
{
    // Always allow, permission checking is done in ObjectTemplateManager.
    return Replica3::QueryConstruction_ClientConstruction( pDestinationConnection, 
        ObjectTemplate::getMode() == SERVER ? true : false );
}

bool ClientObjectTemplate::QueryRemoteConstruction( RakNet::Connection_RM3* pSourceConnection )
{
    // Always allow the server to create object templates.
    return true;
}

RakNet::RM3QuerySerializationResult ClientObjectTemplate::QuerySerialization( 
    RakNet::Connection_RM3* pDestinationConnection )
{
    // Allow serializations for object templates created by this client.
    if( ObjectTemplate::isCreatedByOwnGUID() )
        return Replica3::QuerySerialization_ClientSerializable( pDestinationConnection, 
        ObjectTemplate::getMode() == SERVER ? true : false );
    else
        return Replica3::QuerySerialization_ServerSerializable( pDestinationConnection, 
        ObjectTemplate::getMode() == SERVER ? true : false );
}

void ClientObjectTemplate::SerializeConstruction( RakNet::BitStream* pConstructionBitstream, 
    RakNet::Connection_RM3* pDestinationConnection )
{
    PropertySynchronization::doSerializeConstruction( pConstructionBitstream, 
        pDestinationConnection );
}

bool ClientObjectTemplate::DeserializeConstruction( RakNet::BitStream* pConstructionBitstream, 
    RakNet::Connection_RM3* pSourceConnection )
{
    PropertySynchronization::doDeserializeConstruction( pConstructionBitstream, pSourceConnection );
    return true;
}

RakNet::RM3SerializationResult ClientObjectTemplate::Serialize( 
    RakNet::SerializeParameters* pSerializeParameters )
{
    return PropertySynchronization::doSerialize( pSerializeParameters );
}

void ClientObjectTemplate::Deserialize( RakNet::DeserializeParameters* pDeserializeParameters )
{
    PropertySynchronization::doDeserialize( pDeserializeParameters );
}

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia