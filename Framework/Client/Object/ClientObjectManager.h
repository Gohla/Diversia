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

#ifndef DIVERSIA_CLIENT_CLIENTOBJECTMANAGER_H
#define DIVERSIA_CLIENT_CLIENTOBJECTMANAGER_H

#include "Client/Platform/Prerequisites.h"

#include "Object/ObjectManager.h"
#include "Client/ClientServerPlugin/ServerPlugin.h"

namespace Diversia
{
namespace Client
{
//------------------------------------------------------------------------------

class DIVERSIA_CLIENT_API ClientObjectManager : public ServerPlugin, public ObjectManager
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
    ClientObjectManager( Mode mode, sigc::signal<void>& rUpdateSignal, 
        sigc::signal<void>& rLateUpdateSignal, ServerPluginManager& rPluginManager, 
        RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
        RakNet::NetworkIDManager& rNetworkIDManager );
    /**
    Destructor. 
    **/
    virtual ~ClientObjectManager();

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
    friend class ClientObject;	///< For delayed destruction.

    Object& createObjectImpl( const String& rName, NetworkingType type, const String& rDisplayName,
        RakNet::RakNetGUID source );
    void queryDestroyObject( Object& rObject, RakNet::RakNetGUID source );

    /**
    Creates the specific part of this plugin. This is called in the tick/frame update after the
    plugin is created.
    **/
    void create();
    /**
    Notifies the plugin about a server state change.
    **/
    inline void setServerState( ServerState serverState ) {}
    /**
    Called if the offline mode is changed.
    
    @param  offlineMode True if set to offline mode, false if set to online mode.
    **/
    void offlineModeChanged( bool offlineMode );

    PermissionManager&  mPermissionManager;

    CAMP_CLASS(ClientObjectManager)

};

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Client::ClientObjectManager, 
    &Diversia::Client::Bindings::CampBindings::bindClientObjectManager );

#endif // DIVERSIA_CLIENT_CLIENTOBJECTMANAGER_H