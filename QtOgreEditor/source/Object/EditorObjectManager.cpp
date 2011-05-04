/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "Client/Permission/PermissionManager.h"
#include "Object/EditorObject.h"
#include "Object/EditorObjectManager.h"
#include "Object/ObjectManager.h"
#include "OgreClient/Object/ObjectSelector.h"

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

EditorObjectManager::EditorObjectManager( Mode mode, PluginState state, 
    sigc::signal<void>& rUpdateSignal, sigc::signal<void>& rLateUpdateSignal, 
    ServerPluginManager& rPluginManager, RakNet::RakPeerInterface& rRakPeer, 
    RakNet::ReplicaManager3& rReplicaManager, RakNet::NetworkIDManager& rNetworkIDManager ):
    ClientObjectManager( mode, state, rUpdateSignal, rLateUpdateSignal, rPluginManager, rRakPeer, 
        rReplicaManager, rNetworkIDManager )
{
    mObjectSelector = new ObjectSelector( *this );
}

EditorObjectManager::~EditorObjectManager()
{
    delete mObjectSelector;
}

Object& EditorObjectManager::createObjectImpl( const String& rName, NetworkingType type, 
    const String& rDisplayName, RakNet::RakNetGUID source )
{
    // Only check permission if this client is creating the object.
    if( source != ObjectManager::getServerGUID() )
    {
        if( type == REMOTE )
        {
            mPermissionManager.checkPermissionThrows( "ObjectManager_CreateRemoteObject", 
                "ClientObjectManager::createObjectImpl" );
        }
        else if( type == LOCAL )
        {
            mPermissionManager.checkPermissionThrows( "ObjectManager_CreateLocalObject", 
                "ClientObjectManager::createObjectImpl" );
        }
    }

    return *new EditorObject( rName, ObjectManager::getMode(), type, rDisplayName, source, 
        ObjectManager::getOwnGUID(), ObjectManager::getServerGUID(), 
        ObjectManager::getUpdateSignal(), *this, mPermissionManager,
        ObjectManager::getReplicaManager(), ObjectManager::getNetworkIDManager(),
        ObjectManager::getRPC3() );
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia