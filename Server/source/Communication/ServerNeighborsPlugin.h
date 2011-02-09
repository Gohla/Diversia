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

#ifndef DIVERSIA_SERVER_SERVERNEIGHBORSPLUGIN_H
#define DIVERSIA_SERVER_SERVERNEIGHBORSPLUGIN_H

#include "Platform/Prerequisites.h"

#include "ClientServerPlugin/ClientPlugin.h"
#include "Shared/Communication/ServerNeighbors.h"

namespace Diversia
{
namespace Server
{
//------------------------------------------------------------------------------

class ServerNeighborsPlugin : public ClientPlugin
{
public:
    /**
    Constructor. 
    
    @param  mode                        The mode (Client/Server) the plugin will run in. 
    @param [in,out] rPluginManager      The plugin manager. 
    @param [in,out] rRakPeer            The peer interface. 
    @param [in,out] rReplicaManager     The replica manager. 
    @param [in,out] rNetworkIDManager   The network ID manager. 
    **/
    ServerNeighborsPlugin( Mode mode, ClientPluginManager& rPluginManager, 
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

private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    /**
	Creates the specific part of this plugin. This is called in the tick/frame update after the
	plugin is created.
	**/
    void create();

    // TODO: Use property syncing once camp supports std::map.
    void SerializeConstruction( RakNet::BitStream* pConstructionBitstream, 
        RakNet::Connection_RM3* pDestinationConnection );
    RakNet::RM3SerializationResult Serialize( 
        RakNet::SerializeParameters* pSerializeParameters );

    ServerNeighbors mServerNeighbors;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Server::ServerNeighborsPlugin, 
    &Diversia::Server::Bindings::CampBindings::bindServerNeighborsPlugin );

#endif // DIVERSIA_SERVER_SERVERNEIGHBORSPLUGIN_H