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

#ifndef DIVERSIA_SERVER_RESOURCEMANAGERPLUGIN_H
#define DIVERSIA_SERVER_RESOURCEMANAGERPLUGIN_H

#include "Platform/Prerequisites.h"

#include "ClientServerPlugin/ClientPlugin.h"
#include "Shared/Resource/ResourceInfo.h"

namespace Diversia
{
namespace Server
{
//------------------------------------------------------------------------------

class ResourceManagerPlugin : public ClientPlugin
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
    ResourceManagerPlugin( Mode mode, ClientPluginManager& rPluginManager, 
        RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
        RakNet::NetworkIDManager& rNetworkIDManager );

    /**
    Gets the plugin type.
    **/
    inline ClientServerPluginTypeEnum getType() const { return CLIENTSERVERPLUGINTYPE_RESOURCEMANAGER; }
    static inline ClientServerPluginTypeEnum getTypeStatic() { return CLIENTSERVERPLUGINTYPE_RESOURCEMANAGER; }
    /**
    Gets the plugin type name.
    **/
    inline String getTypeName() const { return CLIENTSERVERPLUGINNAME_RESOURCEMANAGER; }
    static inline String getTypeNameStatic() { return CLIENTSERVERPLUGINNAME_RESOURCEMANAGER; }
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    /**
	Creates the specific part of this plugin. This is called in the tick/frame update after the
	plugin is created.
	**/
    inline void create() {}

    ResourceLocationType mType;
    String mResourceLocation;

    CAMP_RTTI()
};

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Server::ResourceManagerPlugin, 
    &Diversia::Server::Bindings::CampBindings::bindResourceManagerPlugin );

#endif // DIVERSIA_SERVER_RESOURCEMANAGERPLUGIN_H