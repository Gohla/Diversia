/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_DEFAULTCLIENT_DEFAULTCLIENTOBJECTMANAGER_H
#define DIVERSIA_DEFAULTCLIENT_DEFAULTCLIENTOBJECTMANAGER_H

#include "DefaultClient/Platform/Prerequisites.h"

#include "Client/Object/ClientObjectManager.h"

namespace Diversia
{
namespace DefaultClient
{
//------------------------------------------------------------------------------

class DefaultClientObjectManager : public ClientObjectManager
{
public:
    DefaultClientObjectManager( Mode mode, PluginState state, sigc::signal<void>& rUpdateSignal, 
        sigc::signal<void>& rLateUpdateSignal, ClientPluginManager& rPluginManager, 
        RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
        RakNet::NetworkIDManager& rNetworkIDManager );
    virtual ~DefaultClientObjectManager();

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
    
private:
    ObjectSelector* mObjectSelector;

    CAMP_CLASS(DefaultClientObjectManager)

};

//------------------------------------------------------------------------------
} // Namespace DefaultClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::DefaultClient::DefaultClientObjectManager, 
    &Diversia::DefaultClient::Bindings::CampBindings::bindDefaultClientObjectManager );

#endif // DIVERSIA_DEFAULTCLIENT_DEFAULTCLIENTOBJECTMANAGER_H