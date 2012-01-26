/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "DefaultClient/Platform/StableHeaders.h"

#include "DefaultClient/Object/DefaultClientObjectManager.h"
#include "OgreClient/Object/ObjectSelector.h"

namespace Diversia
{
namespace DefaultClient 
{
//------------------------------------------------------------------------------

DefaultClientObjectManager::DefaultClientObjectManager( Mode mode, PluginState state, 
    sigc::signal<void>& rUpdateSignal, sigc::signal<void>& rLateUpdateSignal, 
    ClientPluginManager& rPluginManager, RakNet::RakPeerInterface& rRakPeer, 
    RakNet::ReplicaManager3& rReplicaManager, RakNet::NetworkIDManager& rNetworkIDManager ):
    ClientObjectManager( mode, state, rUpdateSignal, rLateUpdateSignal, rPluginManager, rRakPeer, 
        rReplicaManager, rNetworkIDManager )
{
    mObjectSelector = new ObjectSelector( *this );
}

DefaultClientObjectManager::~DefaultClientObjectManager()
{
    delete mObjectSelector;
}

//------------------------------------------------------------------------------
} // Namespace DefaultClient
} // Namespace Diversia