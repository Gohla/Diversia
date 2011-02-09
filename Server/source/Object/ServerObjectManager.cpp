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

#include "ClientServerPlugin/ClientPluginManager.h"
#include "Object/ServerObject.h"
#include "Object/ServerObjectManager.h"
#include "Permission/PermissionManager.h"
#include "Shared/Lua/LuaManager.h"

namespace Diversia
{
namespace Server 
{
//------------------------------------------------------------------------------
 
ServerObjectManager::ServerObjectManager( Mode mode, sigc::signal<void>& rUpdateSignal, 
    ClientPluginManager& rPluginManager, RakNet::RakPeerInterface& rRakPeer, 
    RakNet::ReplicaManager3& rReplicaManager, RakNet::NetworkIDManager& rNetworkIDManager ):
    ObjectManager( mode, rRakPeer.GetMyGUID(), rRakPeer.GetMyGUID(), rUpdateSignal, 
        rReplicaManager, rNetworkIDManager, rPluginManager.getRPC3() ),
    ClientPlugin( mode, rPluginManager, rRakPeer, rReplicaManager, rNetworkIDManager ),
    mPermissionManager( rPluginManager.getPlugin<PermissionManager>() )
{
    PropertySynchronization::storeUserObject();

    Globals::mLua->object( "ObjectManager" ) = this;
}

ServerObjectManager::~ServerObjectManager()
{

}

Object& ServerObjectManager::createObjectImpl( const String& rName, NetworkingType type, 
    const String& rDisplayName, RakNet::RakNetGUID source )
{
    // Only check permission if a client is creating the object.
    if( source != ObjectManager::getServerGUID() && type == REMOTE )
    {
        mPermissionManager.checkPermissionThrows( source, "ObjectManager_CreateRemoteObject", 
            "ServerObjectManager::createObjectImpl" );
    }

    return *new ServerObject( rName, ObjectManager::getMode(), type, rDisplayName, source, 
        ObjectManager::getOwnGUID(), ObjectManager::getServerGUID(), 
        ObjectManager::getUpdateSignal(), *this, ObjectManager::getReplicaManager(), 
        ObjectManager::getNetworkIDManager(), mPermissionManager, ObjectManager::getRPC3() );
}

void ServerObjectManager::queryDestroyObject( Object& rObject, RakNet::RakNetGUID source )
{
    // Only check permission if a client is destroying the object.
    if( source != ObjectManager::getServerGUID() && rObject.getNetworkingType() == 
        REMOTE )
    {
        if( rObject.isCreatedBy( source ) )
        {
            mPermissionManager.checkPermissionThrows( source, "ObjectManager_DestroyOwnObject", 
                "ServerObjectManager::createObjectImpl" );
        }
        else
        {
            mPermissionManager.checkPermissionThrows( source, "ObjectManager_DestroyOtherObject", 
                "ServerObjectManager::createObjectImpl" );
        }
    }
}

void ServerObjectManager::create()
{

}

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia