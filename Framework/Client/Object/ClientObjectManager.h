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
#include "Client/Plugin/ClientPlugin.h"

namespace Diversia
{
namespace Client
{
//------------------------------------------------------------------------------

class DIVERSIA_CLIENT_API ClientObjectManager : public ClientPlugin, public ObjectManager
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
    ClientObjectManager( Mode mode, PluginState state, sigc::signal<void>& rUpdateSignal, 
        sigc::signal<void>& rLateUpdateSignal, ClientPluginManager& rPluginManager, 
        RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
        RakNet::NetworkIDManager& rNetworkIDManager );
    /**
    Destructor. 
    **/
    virtual ~ClientObjectManager();

    /**
    Gets the plugin type.
    **/
    inline PluginTypeEnum getType() const { return PLUGINTYPE_OBJECTMANAGER; }
    static inline PluginTypeEnum getTypeStatic() { return PLUGINTYPE_OBJECTMANAGER; }
    /**
    Gets the plugin type name.
    **/
    inline String getTypeName() const { return PLUGINNAME_OBJECTMANAGER; }
    static inline String getTypeNameStatic() { return PLUGINNAME_OBJECTMANAGER; }
    
protected:
    friend class ClientObject;	///< For delayed destruction.

    virtual Object& createObjectImpl( const String& rName, NetworkingType type, const String& rDisplayName,
        RakNet::RakNetGUID source );
    virtual void queryDestroyObject( Object& rObject, RakNet::RakNetGUID source );

    void load();
    void unload();
    inline void setServerState( ServerState serverState ) {}
    void offlineModeChanged( bool offlineMode );

    PermissionManager&  mPermissionManager;

private:
    void stateChanged( PluginState state, PluginState prevState );

    CAMP_CLASS(ClientObjectManager)

};

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Client::ClientObjectManager, 
    &Diversia::Client::Bindings::CampBindings::bindClientObjectManager );

#endif // DIVERSIA_CLIENT_CLIENTOBJECTMANAGER_H