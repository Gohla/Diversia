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

#ifndef DIVERSIA_CLIENT_SERVERNEIGHBORSPLUGIN_H
#define DIVERSIA_CLIENT_SERVERNEIGHBORSPLUGIN_H

#include "Client/Platform/Prerequisites.h"

#include "Client/ClientServerPlugin/ServerPlugin.h"
#include "Shared/Communication/ServerNeighbors.h"

namespace Diversia
{
namespace Client
{
//------------------------------------------------------------------------------

class DIVERSIA_CLIENT_API ServerNeighborsPlugin : public ServerPlugin
{
public:
    ServerNeighborsPlugin( Mode mode, PluginState state, ServerPluginManager& rPluginManager, 
        RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
        RakNet::NetworkIDManager& rNetworkIDManager );

    /**
    Gets the plugin type.
    **/
    inline ClientServerPluginTypeEnum getType() const { return CLIENTSERVERPLUGINTYPE_SERVERNEIGHBORS; }
    static inline ClientServerPluginTypeEnum getTypeStatic() { return CLIENTSERVERPLUGINTYPE_SERVERNEIGHBORS; }
    /**
    Gets the plugin type name.
    **/
    inline String getTypeName() const { return CLIENTSERVERPLUGINNAME_SERVERNEIGHBORS; }
    static inline String getTypeNameStatic() { return CLIENTSERVERPLUGINNAME_SERVERNEIGHBORS; }

    /**
    Connects a slot to the neighbors changed signal.
    
    @param [in,out] rSlot   The slot (signature: void func(ServerNeighbors&) to connect. 
    
    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connect( const sigc::slot<void, ServerNeighbors&>& rSlot ) 
    {
        return mNeighborUpdateSignal.connect( rSlot );
    }
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    /**
    Creates the specific part of this plugin. This is called in the tick/frame update after the
    plugin is created.
    **/
    void create();
    /**
    Notifies the plugin about a server state change.
    **/
    void setServerState( ServerState serverState );

    // TODO: Use property syncing once camp supports std::map.
    bool DeserializeConstruction( RakNet::BitStream* pConstructionBitstream, 
        RakNet::Connection_RM3* pSourceConnection );
    void Deserialize( RakNet::DeserializeParameters* pDeserializeParameters );

    ServerNeighbors mServerNeighbors;

    sigc::signal<void, ServerNeighbors&> mNeighborUpdateSignal;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Client::ServerNeighborsPlugin, 
    &Diversia::Client::Bindings::CampBindings::bindServerNeighborsPlugin );

#endif // DIVERSIA_CLIENT_SERVERNEIGHBORSPLUGIN_H