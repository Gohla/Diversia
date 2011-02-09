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

#ifndef DIVERSIA_SERVER_CLIENTPLUGIN_H
#define DIVERSIA_SERVER_CLIENTPLUGIN_H

#include "Platform/Prerequisites.h"

#include "Shared/ClientServerPlugin/ClientServerPlugin.h"
#include "Shared/Camp/PropertySynchronization.h"

namespace Diversia
{
namespace Server
{
//------------------------------------------------------------------------------

class ClientPlugin : public ClientServerPlugin, public PropertySynchronization
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
    ClientPlugin( Mode mode, ClientPluginManager& rPluginManager, 
        RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
        RakNet::NetworkIDManager& rNetworkIDManager );
    /**
    Destructor. 
    **/
    virtual ~ClientPlugin() {}

protected:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    void querySetPropertyDeserialize( const String& rQuery, camp::Value& rValue, 
        RakNet::RakNetGUID source );
    void queryInsertPropertyDeserialize( const String& rQuery, camp::Value& rValue, 
        RakNet::RakNetGUID source );

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

private:
    void pluginCreated( ClientServerPlugin& rPlugin, bool created );

    camp::UserObject    mUserObject;

    PermissionManager*  mPermissionManager;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Server::ClientPlugin, 
    &Diversia::Server::Bindings::CampBindings::bindClientPlugin );

#endif // DIVERSIA_SERVER_CLIENTPLUGIN_H