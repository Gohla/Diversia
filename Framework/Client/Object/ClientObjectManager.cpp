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

#include "Client/Plugin/ClientPluginManager.h"
#include "Client/Communication/Server.h"
#include "Client/Communication/ServerConnection.h"
#include "Client/Lua/LuaPlugin.h"
#include "Client/Object/ClientObject.h"
#include "Client/Object/ClientObjectManager.h"
#include "Client/Object/ClientObjectTemplateManager.h"
#include "Client/Permission/PermissionManager.h"
#include "Shared/Lua/LuaManager.h"

namespace Diversia
{
namespace Client 
{
//------------------------------------------------------------------------------

ClientObjectManager::ClientObjectManager( Mode mode, PluginState state, 
    sigc::signal<void>& rUpdateSignal, sigc::signal<void>& rLateUpdateSignal, 
    ClientPluginManager& rPluginManager, RakNet::RakPeerInterface& rRakPeer, 
    RakNet::ReplicaManager3& rReplicaManager, RakNet::NetworkIDManager& rNetworkIDManager ):
    ObjectManager( mode, rRakPeer.GetMyGUID(), rRakPeer.GetGUIDFromIndex( 0 ), rUpdateSignal, 
        rLateUpdateSignal, rPluginManager.getPlugin<ClientObjectTemplateManager>(), rReplicaManager, 
        rNetworkIDManager, rPluginManager.getServer().getServerConnection().getRPC3() ),
    ClientPlugin( mode, state, rPluginManager, rRakPeer, rReplicaManager, rNetworkIDManager ),
    mPermissionManager( rPluginManager.getPlugin<PermissionManager>() )
{
    PropertySynchronization::storeUserObject();

    try
    {
        Plugin::getPluginManager().getPlugin<LuaPlugin>().get().object( 
            "ObjectManager" ) = this;
    }
    catch( Exception e )
    {
    	LCLOGE << "Could not add ObjectManager object to lua: " << e.what();
    }
}

ClientObjectManager::~ClientObjectManager()
{

}

Object& ClientObjectManager::createObjectImpl( const String& rName, NetworkingType type, 
    const String& rDisplayName, RakNet::RakNetGUID source )
{
    // Only check permission if this client is creating the object.
    if( source != ObjectManager::getServerGUID() )
    {
        if( type == REMOTE )
        {
            mPermissionManager.checkPermissionThrows( "ObjectManager_CreateRemoteObject", 
                "ClientObjectManager::createObjectImpl" );
        }
        else if( type == LOCAL )
        {
            mPermissionManager.checkPermissionThrows( "ObjectManager_CreateLocalObject", 
                "ClientObjectManager::createObjectImpl" );
        }
    }

    return *new ClientObject( rName, ObjectManager::getMode(), type, rDisplayName, source, 
        ObjectManager::getOwnGUID(), ObjectManager::getServerGUID(), 
        ObjectManager::getUpdateSignal(), *this, mPermissionManager,
        ObjectManager::getReplicaManager(), ObjectManager::getNetworkIDManager(),
        ObjectManager::getRPC3() );
}

void ClientObjectManager::queryDestroyObject( Object& rObject, RakNet::RakNetGUID source )
{
    // Only check permission if this client is destroying the object.
    if( source != ObjectManager::getServerGUID() )
    {
        if( rObject.getNetworkingType() == REMOTE )
        {
            if( rObject.isCreatedBy( source ) )
            {
                mPermissionManager.checkPermissionThrows( "ObjectManager_DestroyOwnObject", 
                    "ClientObjectManager::createObjectImpl" );
            }
            else
            {
                mPermissionManager.checkPermissionThrows( "ObjectManager_DestroyOtherObject", 
                    "ClientObjectManager::createObjectImpl" );
            }
        }
        else if( rObject.getNetworkingType() == LOCAL )
        {
            mPermissionManager.checkPermissionThrows( "ObjectManager_DestroyLocalObject", 
                "ClientObjectManager::createObjectImpl" );
        }
    }
}

void ClientObjectManager::load()
{
    // TODO: Implement real loading completed signal
    ClientPlugin::mLoadingCompletedSignal( *this );
}

void ClientObjectManager::unload()
{
    ObjectManager::reset();
}

void ClientObjectManager::offlineModeChanged( bool offlineMode )
{
    ObjectManager::offlineModeChanged( offlineMode );
}

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia