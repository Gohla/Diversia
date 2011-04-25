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

#ifndef DIVERSIA_CLIENT_SERVERPLUGIN_H
#define DIVERSIA_CLIENT_SERVERPLUGIN_H

#include "Client/Platform/Prerequisites.h"

#include "Shared/ClientServerPlugin/ClientServerPlugin.h"
#include "Client/Communication/ServerAbstract.h"
#include "Shared/Camp/PropertySynchronization.h"

namespace Diversia
{
namespace Client
{
//------------------------------------------------------------------------------

class DIVERSIA_CLIENT_API ServerPlugin : public ClientServerPlugin, public PropertySynchronization
{
public:
    /**
    Constructor. 
    
    @param  mode                        The mode (Client/Server) the plugin will run in. 
    @param  state                       The initial state of this plugin.
    @param [in,out] rPluginManager      The plugin manager. 
    @param [in,out] rRakPeer            The peer interface. 
    @param [in,out] rReplicaManager     The replica manager. 
    @param [in,out] rNetworkIDManager   The network ID manager. 
    **/
    ServerPlugin( Mode mode, PluginState state, ServerPluginManager& rPluginManager, 
        RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
        RakNet::NetworkIDManager& rNetworkIDManager );
    /**
    Destructor. 
    **/
    virtual ~ServerPlugin() {}

    /**
    Gets the server plugin manager. 
    **/
    ServerPluginManager& getServerPluginManager() const;
    /**
    Gets the server that this plugin belongs to.
    **/
    ServerAbstract& getServer() const;
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
    
    @param [in,out] rSlot   The slot (signature: void func(ServerPlugin&)) to connect. 
    
    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connectLoadingComplete( const sigc::slot<void, ServerPlugin&>& rSlot )
    {
        return mLoadingCompletedSignal.connect( rSlot );
    }

protected:
    /**
    Notifies the plugin about a server state change.
    **/
    virtual void setServerState( ServerState serverState ) = 0;
    /**
    Called if the offline mode is changed.
    
    @param  offlineMode True if set to offline mode, false if set to online mode.
    **/
    inline virtual void offlineModeChanged( bool offlineMode ) {}

    /**
    Implemented in ServerPlugin, but can be overridden. Be sure to call the function in this 
    class first in your overridden function before doing your own processing!
    **/
    virtual void SerializeConstruction( RakNet::BitStream* pConstructionBitstream, 
        RakNet::Connection_RM3* pDestinationConnection );
    virtual bool DeserializeConstruction( RakNet::BitStream* pConstructionBitstream, 
        RakNet::Connection_RM3* pSourceConnection );
    virtual RakNet::RM3SerializationResult Serialize( 
        RakNet::SerializeParameters* pSerializeParameters );
    virtual void Deserialize( RakNet::DeserializeParameters* pDeserializeParameters );

    void querySetProperty( const String& rQuery, camp::Value& rValue );
    void queryInsertProperty( const String& rQuery, camp::Value& rValue );

    sigc::signal<void, ServerPlugin&> mLoadingCompletedSignal;

private:
    friend class ServerPluginManager;	///< ServerPluginManager may call setServerState.

    void pluginCreated( ClientServerPlugin& rPlugin, bool created );

    PermissionManager*      mPermissionManager;
    bool                    mOfflineMode;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Client::ServerPlugin, 
    &Diversia::Client::Bindings::CampBindings::bindServerPlugin );

#endif // DIVERSIA_CLIENT_SERVERPLUGIN_H