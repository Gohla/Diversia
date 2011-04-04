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
#include "Client/Permission/PermissionManager.h"
#include "Shared/Camp/CampStringInterpreter.h"

namespace Diversia
{
namespace Client 
{
//------------------------------------------------------------------------------

ClientComponentTemplate::ClientComponentTemplate( const String& rName, Mode mode, 
    NetworkingType networkingType, ComponentType type, RakNet::RakNetGUID source, 
    bool localOverride, ClientObjectTemplate& rObjectTemplate ):
    ComponentTemplate( rName, mode, networkingType, type, source, localOverride, rObjectTemplate ),
    PropertySynchronization( mode ),
    mPermissionManager( rObjectTemplate.getPermissionManager() )
{
    PropertySynchronization::storeUserObject();

    if( ComponentTemplate::getNetworkingType() == REMOTE && !ComponentTemplate::getLocalOverride() &&
        ComponentTemplate::isCreatedByOwnGUID() )
    {
        // Creating remote component on this client.
        mPermissionManager.getPermission( "ObjectTemplateManager_CreateRemoteComponentTemplate" ).addItem();

        if( rObjectTemplate.isCreatedBySource( source ) )
        {
            mPermissionManager.getPermission(
                "ObjectTemplateManager_CreateRemoteComponentTemplateOnOwnObjectTemplate" ).addItem();
        }
        else
        {
            mPermissionManager.getPermission( 
                "ObjectTemplateManager_CreateRemoteComponentTemplateOnOtherObjectTemplate" ).addItem();
        }
    }
    else if( ComponentTemplate::getNetworkingType() == LOCAL || ComponentTemplate::getLocalOverride() )
    {
        // Creating local component.
        mPermissionManager.getPermission( "ObjectTemplateManager_CreateLocalComponentTemplate" ).addItem();
    }
}

ClientComponentTemplate::~ClientComponentTemplate()
{
    if( ComponentTemplate::getNetworkingType() == REMOTE && !ComponentTemplate::getLocalOverride() &&
        ComponentTemplate::isCreatedByOwnGUID() )
    {
        // Destroying remote component created by this client.
        mPermissionManager.getPermission( "ObjectTemplateManager_CreateRemoteComponentTemplate" ).removeItem();

        if( ClientComponentTemplate::getClientObjectTemplate().isCreatedBySource( ComponentTemplate::getSourceGUID() ) )
        {
            mPermissionManager.getPermission(  
                "ObjectTemplateManager_CreateRemoteComponentTemplateOnOwnObjectTemplate" ).removeItem();
        }
        else
        {
            mPermissionManager.getPermission( 
                "ObjectTemplateManager_CreateRemoteComponentTemplateOnOtherObjectTemplate" ).removeItem();
        }
    }
    else if( ComponentTemplate::getNetworkingType() == LOCAL || ComponentTemplate::getLocalOverride() )
    {
        // Destroying local component.
        mPermissionManager.getPermission( "ObjectTemplateManager_CreateLocalComponentTemplate" ).removeItem();
    }
}

void ClientComponentTemplate::setNetworkingTypeImpl( NetworkingType type )
{
    if( type == REMOTE )
    {
        if( ClientComponentTemplate::getClientObjectTemplate().isCreatedByOwnGUID() )
        {
            mPermissionManager.getPermission( 
                "ObjectTemplateManager_CreateRemoteComponentTemplateOnOwnObjectTemplate" ).addItem();
        }
        else
        {
            mPermissionManager.getPermission( 
                "ObjectTemplateManager_CreateRemoteComponentTemplateOnOtherObjectTemplate" ).addItem();
        }

        mPermissionManager.getPermission( "ObjectTemplateManager_CreateRemoteComponentTemplate" ).addItem();
        mPermissionManager.getPermission( "ObjectTemplateManager_CreateLocalComponentTemplate" ).removeItem();
    }
    else if( type == LOCAL )
    {
        if( ClientComponentTemplate::getClientObjectTemplate().isCreatedByOwnGUID() )
        {
            mPermissionManager.getPermission( 
                "ObjectTemplateManager_CreateRemoteComponentTemplateOnOwnObjectTemplate" ).removeItem();
        }
        else
        {
            mPermissionManager.getPermission( 
                "ObjectTemplateManager_CreateRemoteComponentTemplateOnOtherObjectTemplate" ).removeItem();
        }

        mPermissionManager.getPermission( "ObjectTemplateManager_CreateRemoteComponentTemplate" ).removeItem();
        mPermissionManager.getPermission( "ObjectTemplateManager_CreateLocalComponentTemplate" ).addItem();
    }
}

void ClientComponentTemplate::querySetNetworkingTypeImpl( NetworkingType type )
{
    // If the component is overridden to local networking mode, changing the network type has no 
    // influence on this component, so always allow it.
    if( ComponentTemplate::getLocalOverride() ) return;

    if( type == REMOTE )
    {
        // Change networking type from local to remote.
        mPermissionManager.checkPermissionThrows( "ObjectTemplateManager_CreateRemoteComponentTemplate", 
            "ClientComponentTemplate::setNetworkingType" );

        if( ClientComponentTemplate::getClientObjectTemplate().isCreatedByOwnGUID() )
        {
            mPermissionManager.checkPermissionThrows(
                "ObjectTemplateManager_CreateRemoteComponentTemplateOnOwnObjectTemplate", 
                "ClientComponentTemplate::setNetworkingType" );

            mPermissionManager.getPermission( 
                "ObjectTemplateManager_CreateRemoteComponentTemplateOnOwnObjectTemplate" ).addItem();
        }
        else
        {
            mPermissionManager.checkPermissionThrows( 
                "ObjectTemplateManager_CreateRemoteComponentTemplateOnOtherObjectTemplate", 
                "ClientComponentTemplate::setNetworkingType" );

            mPermissionManager.getPermission( 
                "ObjectTemplateManager_CreateRemoteComponentTemplateOnOtherObjectTemplate" ).addItem();
        }

        mPermissionManager.getPermission( "ObjectTemplateManager_CreateRemoteComponentTemplate" ).addItem();
        mPermissionManager.getPermission( "ObjectTemplateManager_CreateLocalComponentTemplate" ).removeItem( true );
    }
    else if( type == LOCAL )
    {
        // Change networking type from remote to local.
        mPermissionManager.checkPermissionThrows( "ObjectTemplateManager_CreateLocalComponentTemplate", 
            "ClientComponentTemplate::setNetworkingType" );

        if( ClientComponentTemplate::getClientObjectTemplate().isCreatedByOwnGUID() )
        {
            mPermissionManager.getPermission( 
                "ObjectTemplateManager_CreateRemoteComponentTemplateOnOwnObjectTemplate" ).removeItem( true );
        }
        else
        {
            mPermissionManager.getPermission( 
                "ObjectTemplateManager_CreateRemoteComponentTemplateOnOtherObjectTemplate" ).removeItem( true );
        }

        mPermissionManager.getPermission( "ObjectTemplateManager_CreateRemoteComponentTemplate" ).removeItem( true );
        mPermissionManager.getPermission( "ObjectTemplateManager_CreateLocalComponentTemplate" ).addItem();
    }
}

void ClientComponentTemplate::cleanupQuerySetNetworkingType( NetworkingType type )
{
    // If the component is overridden to local networking mode, changing the network type has no 
    // influence on this component, so there is nothing to clean up.
    if( ComponentTemplate::getLocalOverride() ) return;

    if( type == REMOTE )
    {
        if( ClientComponentTemplate::getClientObjectTemplate().isCreatedByOwnGUID() )
        {
            mPermissionManager.getPermission( 
                "ObjectTemplateManager_CreateRemoteComponentTemplateOnOwnObjectTemplate" ).removeItem( true );
        }
        else
        {
            mPermissionManager.getPermission( 
                "ObjectTemplateManager_CreateRemoteComponentTemplateOnOtherObjectTemplate" ).removeItem( true );
        }

        mPermissionManager.getPermission( "ObjectTemplateManager_CreateRemoteComponentTemplate" ).removeItem( true );
        mPermissionManager.getPermission( "ObjectTemplateManager_CreateLocalComponentTemplate" ).addItem();
    }
    else if( type == LOCAL )
    {
        if( ClientComponentTemplate::getClientObjectTemplate().isCreatedByOwnGUID() )
        {
            mPermissionManager.getPermission( 
                "ObjectTemplateManager_CreateRemoteComponentTemplateOnOwnObjectTemplate" ).addItem();
        }
        else
        {
            mPermissionManager.getPermission( 
                "ObjectTemplateManager_CreateRemoteComponentTemplateOnOtherObjectTemplate" ).addItem();
        }

        mPermissionManager.getPermission( "ObjectTemplateManager_CreateRemoteComponentTemplate" ).addItem();
        mPermissionManager.getPermission( "ObjectTemplateManager_CreateLocalComponentTemplate" ).removeItem( true );
    }
}

void ClientComponentTemplate::querySetProperty( const String& rQuery, camp::Value& rValue )
{
    // Permission name: SetPropertyOn<Ownership><ComponentName>ComponentTemplate_<PropertyName>
    mPermissionManager.checkPermissionThrows( String( "SetPropertyOn" ) + 
        String( ComponentTemplate::isCreatedByOwnGUID() ? "Own" : "Other" ) + getTypeName() + 
        String( "ComponentTemplate_" ) + CampStringInterpreter::removeArrayIdentifiers( rQuery ), 
        rValue, "ClientComponentTemplate::querySetProperty" );
}

void ClientComponentTemplate::queryInsertProperty( const String& rQuery, camp::Value& rValue )
{
    // Permission name: InsertValueIn<Ownership><ComponentName>ComponentTemplate_<PropertyName>
    mPermissionManager.checkPermissionThrows( String( "InsertValueIn" ) + 
        String( ComponentTemplate::isCreatedByOwnGUID() ? "Own" : "Other" ) + getTypeName() + 
        String( "ComponentTemplate_" ) + CampStringInterpreter::removeArrayIdentifiers( rQuery ), 
        rValue, "ClientComponentTemplate::queryInsertProperty" );
}

RakNet::RM3ConstructionState ClientComponentTemplate::QueryConstruction( 
    RakNet::Connection_RM3* pDestinationConnection, RakNet::ReplicaManager3* pReplicaManager3 )
{
    // Always allow, permission checking is done in object template.
    return Replica3::QueryConstruction_ClientConstruction( pDestinationConnection,
        ComponentTemplate::getMode() == SERVER ? true : false );
}

bool ClientComponentTemplate::QueryRemoteConstruction( RakNet::Connection_RM3* pSourceConnection )
{
    // Always allow the server to create component templates.
    return true;
}

RakNet::RM3QuerySerializationResult ClientComponentTemplate::QuerySerialization( 
    RakNet::Connection_RM3* pDestinationConnection )
{
    // Allow serializations for component templates created by this client.
    if( ComponentTemplate::isCreatedByOwnGUID() )
        return Replica3::QuerySerialization_ClientSerializable( pDestinationConnection, 
        ComponentTemplate::getMode() == SERVER ? true : false );
    else
        return Replica3::QuerySerialization_ServerSerializable( pDestinationConnection, 
        ComponentTemplate::getMode() == SERVER ? true : false );
}

void ClientComponentTemplate::SerializeConstruction( RakNet::BitStream* pConstructionBitstream, 
    RakNet::Connection_RM3* pDestinationConnection )
{
    PropertySynchronization::doSerializeConstruction( pConstructionBitstream, 
        pDestinationConnection );
}

bool ClientComponentTemplate::DeserializeConstruction( RakNet::BitStream* pConstructionBitstream, 
    RakNet::Connection_RM3* pSourceConnection )
{
    PropertySynchronization::doDeserializeConstruction( pConstructionBitstream, pSourceConnection );
    return true;
}

RakNet::RM3SerializationResult ClientComponentTemplate::Serialize( 
    RakNet::SerializeParameters* pSerializeParameters )
{
    return PropertySynchronization::doSerialize( pSerializeParameters );
}

void ClientComponentTemplate::Deserialize( RakNet::DeserializeParameters* pDeserializeParameters )
{
    PropertySynchronization::doDeserialize( pDeserializeParameters );
}

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia