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

namespace Diversia
{
namespace Client 
{
//------------------------------------------------------------------------------

ClientPluginManager::ClientPluginManager( Mode mode, PluginState state, 
    sigc::signal<void>& rUpdateSignal, ServerAbstract& rServer, RakNet::RakPeerInterface& rRakPeer, 
    RakNet::ReplicaManager3& rReplicaManager, RakNet::NetworkIDManager& rNetworkIDManager, 
    bool offlineMode /*= false*/ ):
    PluginManager( mode, state, rUpdateSignal, rRakPeer, rReplicaManager, 
        rNetworkIDManager ),
    mServer( rServer ),
    mOfflineMode( offlineMode )
{
    PluginManager::connect( sigc::mem_fun( this, &ClientPluginManager::pluginChanged ) );
}

ClientPluginManager::~ClientPluginManager()
{

}

void ClientPluginManager::setServerState( ServerState serverState )
{
    Plugins& plugins = PluginManager::getPluginsMutable();
    for( Plugins::iterator i = plugins.begin(); i != plugins.end(); ++i )
    {
        // Assume every plugin inherits from ClientPlugin on the client.. TODO: Find a better way.
        ClientPlugin* plugin = static_cast<ClientPlugin*>( i->second );
        plugin->setServerState( serverState );
    }
}

void ClientPluginManager::setOfflineMode( bool offlineMode )
{
    if( mOfflineMode != offlineMode )
    {
        mOfflineMode = offlineMode;

        Plugins& plugins = PluginManager::getPluginsMutable();
        for( Plugins::iterator i = plugins.begin(); i != plugins.end(); ++i )
        {
            static_cast<ClientPlugin*>( i->second )->setOfflineMode( mOfflineMode );
        }
    }
}

void ClientPluginManager::pluginChanged( Plugin& rPlugin, bool created )
{
    if( created )
    {
        ClientPlugin& serverPlugin = static_cast<ClientPlugin&>( rPlugin );

        // Connect to loading complete signal.
        mLoadingPlugins.insert( serverPlugin.getType() );
        serverPlugin.connectLoadingComplete( sigc::mem_fun( this, 
            &ClientPluginManager::pluginLoadingComplete ) );

        // Set plugin to offline mode if manager is in offline mode.
        serverPlugin.setOfflineMode( mOfflineMode );
    }
    else
    {
        mLoadingPlugins.erase( rPlugin.getType() );
    }
}

void ClientPluginManager::pluginLoadingComplete( ClientPlugin& rClientPlugin )
{
    mLoadingPlugins.erase( rClientPlugin.getType() );

    // If the set of loading plugins is empty then all plugins are done loading, emit signal.
    if( mLoadingPlugins.empty() )
        mLoadingCompletedSignal();
}

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia