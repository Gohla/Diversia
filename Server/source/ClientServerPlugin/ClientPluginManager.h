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

#ifndef DIVERSIA_SERVER_CLIENTPLUGINMANAGER_H
#define DIVERSIA_SERVER_CLIENTPLUGINMANAGER_H

#include "Platform/Prerequisites.h"

#include "Shared/ClientServerPlugin/ClientServerPluginManager.h"

namespace Diversia
{
namespace Server
{
//------------------------------------------------------------------------------

class ClientPluginManager : public ClientServerPluginManager, public sigc::trackable
{
public:
    /**
    Constructor. 
    
    @param  mode                        The mode the client-server plugin manager must run in. 
    @param [in,out] rUpdateSignal       The frame/tick update signal. 
    @param [in,out] rRakPeer            The peer interface. 
    @param [in,out] rReplicaManager     Replica manager. 
    @param [in,out] rNetworkIDManager   Network ID manager. 
    @param [in,out] rRPC3               RPC3 object. 
    **/
    ClientPluginManager( Mode mode, sigc::signal<void>& rUpdateSignal, 
        RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
        RakNet::NetworkIDManager& rNetworkIDManager, RakNet::RPC3& rRPC3 );
    /**
    Destructor. 
    **/
    ~ClientPluginManager();

    /**
    Gets the RPC3 object.
    **/
    inline RakNet::RPC3& getRPC3() const { return mRPC3; }

private:
    RakNet::RPC3& mRPC3;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Server::ClientPluginManager, 
    &Diversia::Server::Bindings::CampBindings::bindClientPluginManager );

#endif // DIVERSIA_SERVER_CLIENTPLUGINMANAGER_H