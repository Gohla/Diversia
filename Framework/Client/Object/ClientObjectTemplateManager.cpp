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

#include "Client/Plugin/ClientPluginManager.h"
#include "Client/Lua/LuaPlugin.h"
#include "Client/Object/ClientObjectManager.h"
#include "Client/Object/ClientObjectTemplate.h"
#include "Client/Object/ClientObjectTemplateManager.h"
#include "Client/Permission/PermissionManager.h"

namespace Diversia
{
namespace Client 
{
//------------------------------------------------------------------------------

ClientObjectTemplateManager::ClientObjectTemplateManager( Mode mode, PluginState state, 
    ClientPluginManager& rPluginManager, RakNet::RakPeerInterface& rRakPeer, 
    RakNet::ReplicaManager3& rReplicaManager, RakNet::NetworkIDManager& rNetworkIDManager ):
    ObjectTemplateManager( mode, rRakPeer.GetMyGUID(), rRakPeer.GetGUIDFromIndex( 0 ), rReplicaManager, 
        rNetworkIDManager ),
    ClientPlugin( mode, state, rPluginManager, rRakPeer, rReplicaManager, rNetworkIDManager ),
    mPermissionManager( rPluginManager.getPlugin<PermissionManager>() )
{
    PropertySynchronization::storeUserObject();

    try
    {
        Plugin::getPluginManager().getPlugin<LuaPlugin>().get().object( 
            "ObjectTemplateManager" ) = this;
    }
    catch( Exception e )
    {
        LCLOGE << "Could not add ObjectTemplateManager object to lua: " << e.what();
    }
}

ClientObjectTemplateManager::~ClientObjectTemplateManager()
{

}

Object& ClientObjectTemplateManager::createObject( const String& rTemplateName, 
    const String& rObjectName, NetworkingType type )
{
    return ObjectTemplateManager::getObjectTemplate( rTemplateName ).createObject( 
        Plugin::getPluginManager().getPlugin<ClientObjectManager>(), rObjectName, type );
}

void ClientObjectTemplateManager::load()
{
    // TODO: Implement real loading completed signal
    ClientPlugin::mLoadingCompletedSignal( *this );
}

void ClientObjectTemplateManager::unload()
{
    // TODO: More control over what templates get removed? Currently all templates are per game.
}

void ClientObjectTemplateManager::offlineModeChanged( bool offlineMode )
{
    ObjectTemplateManager::offlineModeChanged( offlineMode );
}

void ClientObjectTemplateManager::queryDestroyObjectTemplate( ObjectTemplate& rObjectTemplate, 
    RakNet::RakNetGUID source )
{
    // Only check permission if this client is destroying the object.
    if( source != ObjectTemplateManager::getServerGUID() )
    {
        if( rObjectTemplate.getNetworkingType() == REMOTE )
        {
            if( rObjectTemplate.isCreatedBy( source ) )
            {
                mPermissionManager.checkPermissionThrows( "ObjectTemplateManager_DestroyOwnObjectTemplate", 
                    "ClientObjectTemplateManager::createObjectTemplateImpl" );
            }
            else
            {
                mPermissionManager.checkPermissionThrows( "ObjectTemplateManager_DestroyOtherObjectTemplate", 
                    "ClientObjectTemplateManager::createObjectTemplateImpl" );
            }
        }
        else if( rObjectTemplate.getNetworkingType() == LOCAL )
        {
            mPermissionManager.checkPermissionThrows( "ObjectTemplateManager_DestroyLocalObjectTemplate", 
                "ClientObjectTemplateManager::createObjectTemplateImpl" );
        }
    }
}

ObjectTemplate& ClientObjectTemplateManager::createObjectTemplateImpl( const String& rName, 
    NetworkingType type, const String& rDisplayName, RakNet::RakNetGUID source )
{
    // Only check permission if this client is creating the object.
    if( source != ObjectTemplateManager::getServerGUID() )
    {
        if( type == REMOTE )
        {
            mPermissionManager.checkPermissionThrows( "ObjectTemplateManager_CreateRemoteObjectTemplate", 
                "ClientObjectTemplateManager::createObjectTemplateImpl" );
        }
        else if( type == LOCAL )
        {
            mPermissionManager.checkPermissionThrows( "ObjectTemplateManager_CreateLocalObjectTemplate", 
                "ClientObjectTemplateManager::createObjectTemplateImpl" );
        }
    }

    return *new ClientObjectTemplate( rName, ObjectTemplateManager::getMode(), type, rDisplayName, source, 
        ObjectTemplateManager::getOwnGUID(), ObjectTemplateManager::getServerGUID(), 
        *this, mPermissionManager, ObjectTemplateManager::getReplicaManager(), 
        ObjectTemplateManager::getNetworkIDManager() );
}

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia