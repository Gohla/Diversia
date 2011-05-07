/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "Object/EditorObject.h"
#include "OgreClient/Graphics/RotationGizmo.h"
#include "OgreClient/Graphics/TranslationGizmo.h"
#include "OgreClient/Object/SceneNode.h"

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

EditorObject::EditorObject( const String& rName, Mode mode, NetworkingType type, 
    const String& rDisplayName, RakNet::RakNetGUID source, RakNet::RakNetGUID ownGUID, 
    RakNet::RakNetGUID serverGUID, sigc::signal<void>& rUpdateSignal, 
    ClientObjectManager& rObjectManager, PermissionManager& rPermissionManager, 
    RakNet::ReplicaManager3& rReplicaManager, RakNet::NetworkIDManager& rNetworkIDManager, 
    RakNet::RPC3& rRPC3 ):
    ClientObject( rName, mode, type, rDisplayName, source, ownGUID, serverGUID, rUpdateSignal, 
        rObjectManager, rPermissionManager, rReplicaManager, rNetworkIDManager, rRPC3 ),
    mGizmo( 0 )
{

}

EditorObject::~EditorObject()
{
    if( mGizmo ) delete mGizmo;
}

void EditorObject::setSelected( bool selected )
{
    if( mGizmo ) 
    {
        delete mGizmo;
        mGizmo = 0;
    }

    if( selected )
    {
        mGizmo = new RotationGizmo( *this );
        Object::getComponent<SceneNode>().getNode()->addChild( mGizmo->getSceneNode() );
    }

    ClientObject::setSelected( selected );
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia