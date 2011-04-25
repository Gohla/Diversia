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

#ifndef DIVERSIA_CLIENT_SERVERPLUGINMANAGER_H
#define DIVERSIA_CLIENT_SERVERPLUGINMANAGER_H

#include "Client/Platform/Prerequisites.h"

#include "Client/ClientServerPlugin/ServerPlugin.h"
#include "Shared/ClientServerPlugin/ClientServerPluginManager.h"

namespace Diversia
{
namespace Client
{
//------------------------------------------------------------------------------

class DIVERSIA_CLIENT_API ServerPluginManager : public ClientServerPluginManager, public sigc::trackable
{
public:
    /**
    Constructor. 
    
    @param  mode                        The mode the client-server plugin manager must run in. 
    @param  state                       The initial state to set plugins to.
    @param [in,out] rUpdateSignal       The frame/tick update signal. 
    @param [in,out] rServer             The server this plugin manager is part of. 
    @param [in,out] rRakPeer            The peer interface. 
    @param [in,out] rReplicaManager     Replica manager. 
    @param [in,out] rNetworkIDManager   Network ID manager. 
    @param  offlineMode                 True to set offline mode. Defaults to false.
    **/
    ServerPluginManager( Mode mode, PluginState state, sigc::signal<void>& rUpdateSignal, 
        ServerAbstract& rServer, RakNet::RakPeerInterface& rRakPeer, 
        RakNet::ReplicaManager3& rReplicaManager, RakNet::NetworkIDManager& rNetworkIDManager,
        bool offlineMode = false );
    /**
    Destructor. 
    **/
    ~ServerPluginManager();

    /**
    Gets the server this plugin manager is part of.
    **/
    inline ServerAbstract& getServer() const { return mServer; }
    /**
    Notifies all plugins about a server state change.
    **/
    void setServerState( ServerState serverState );
    /**
    Query if this plugin is offline mode. 
    
    @return True if offline mode, false if not. 
    **/
    inline bool isOfflineMode() const { return mOfflineMode; }
    /**
    Sets this plugin to online or offline mode.
    
    @param  offlineMode True to set to offline mode, false to set to online mode.
    **/
    void setOfflineMode( bool offlineMode );

    /**
    Connects a slot to the loading completed signal. 
    
    @param [in,out] rSlot   The slot (signature: void func()) to connect. 
    
    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connectLoadingComplete( const sigc::slot<void>& rSlot )
    {
        return mLoadingCompletedSignal.connect( rSlot );
    }
    
private:
    friend class ServerPlugin;	///< ServerPlugin may call pluginAdded and pluginRemoved. 

    void pluginChanged( ClientServerPlugin& rPlugin, bool created );
    void pluginLoadingComplete( ServerPlugin& rServerPlugin );

    ServerAbstract&                     mServer;
    std::set<ClientServerPluginTypeEnum>    mLoadingPlugins;
    sigc::signal<void>                  mLoadingCompletedSignal;
    bool                                mOfflineMode;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Client::ServerPluginManager, 
    &Diversia::Client::Bindings::CampBindings::bindServerPluginManager );

#endif // DIVERSIA_CLIENT_SERVERPLUGINMANAGER_H