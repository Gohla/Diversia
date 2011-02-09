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

#ifndef DIVERSIA_SERVER_CLIENTOBJECTMANAGER_H
#define DIVERSIA_SERVER_CLIENTOBJECTMANAGER_H

#include "Platform/Prerequisites.h"

#include "Object/ObjectManager.h"
#include "ClientServerPlugin/ClientPlugin.h"

namespace Diversia
{
namespace Server
{
//------------------------------------------------------------------------------

class ServerObjectManager : public ClientPlugin, public ObjectManager
{
public:
    /**
    Constructor. 
    
    @param  mode                        The mode (Client/Server) the plugin will run in. 
    @param [in,out] rUpdateSignal       The frame/tick update signal.
    @param [in,out] rPluginManager      The plugin manager. 
    @param [in,out] rRakPeer            The peer interface. 
    @param [in,out] rReplicaManager     The replica manager. 
    @param [in,out] rNetworkIDManager   The network ID manager. 
    **/
    ServerObjectManager( Mode mode, sigc::signal<void>& rUpdateSignal, 
        ClientPluginManager& rPluginManager, RakNet::RakPeerInterface& rRakPeer,
        RakNet::ReplicaManager3& rReplicaManager, RakNet::NetworkIDManager& rNetworkIDManager );
    /**
    Destructor. 
    **/
    ~ServerObjectManager();

	/**
    Gets the plugin type.
    **/
    inline ClientServerPluginTypeEnum getType() const { return CLIENTSERVERPLUGINTYPE_OBJECTMANAGER; }
    static inline ClientServerPluginTypeEnum getTypeStatic() { return CLIENTSERVERPLUGINTYPE_OBJECTMANAGER; }
    /**
    Gets the plugin type name.
    **/
    inline String getTypeName() const { return CLIENTSERVERPLUGINNAME_OBJECTMANAGER; }
    static inline String getTypeNameStatic() { return CLIENTSERVERPLUGINNAME_OBJECTMANAGER; }
    
private:
    friend class ServerObject;	///< For delayed destruction.

    Object& createObjectImpl( const String& rName, NetworkingType type, const String& rDisplayName,
        RakNet::RakNetGUID source );
    void queryDestroyObject( Object& rObject, RakNet::RakNetGUID source );

    /**
	Creates the specific part of this plugin. This is called in the tick/frame update after the
	plugin is created.
	**/
    void create();

    PermissionManager&  mPermissionManager;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Server::ServerObjectManager, 
    &Diversia::Server::Bindings::CampBindings::bindServerObjectManager );

#endif // DIVERSIA_SERVER_CLIENTOBJECTMANAGER_H