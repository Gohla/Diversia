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

#ifndef DIVERSIA_TESTCLIENT_CLIENTPLUGINMANAGER_H
#define DIVERSIA_TESTCLIENT_CLIENTPLUGINMANAGER_H

#include "Platform/Prerequisites.h"

#include "Shared/ClientServerPlugin/ClientServerPluginManager.h"

namespace Diversia
{
namespace TestClient
{
//------------------------------------------------------------------------------

class ServerPluginManager : public ClientServerPluginManager
{
public:
    /**
    Constructor. 
    
    @param  mode                        The mode the client-server plugin manager must run in. 
    @param [in,out] rRakPeer            The peer interface.
    @param [in,out] rReplicaManager     Replica manager. 
    @param [in,out] rNetworkIDManager   Network ID manager. 
    **/
    ServerPluginManager( Mode mode, RakPeerInterface& rRakPeer,
        RakNet::ReplicaManager3& rReplicaManager, NetworkIDManager& rNetworkIDManager );
    /**
    Destructor. 
    **/
    ~ServerPluginManager();
    
private:
    /**
	Gets the connection between the update function and the tick/frame signal.
	**/
    inline sigc::connection& getUpdateConnection() { return mUpdateConnection; }

    sigc::connection mUpdateConnection;

};

//------------------------------------------------------------------------------
} // Namespace TestClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::TestClient::ServerPluginManager, 
    &Diversia::TestClient::Bindings::CampBindings::bindServerPluginManager );

#endif // DIVERSIA_TESTCLIENT_CLIENTPLUGINMANAGER_H