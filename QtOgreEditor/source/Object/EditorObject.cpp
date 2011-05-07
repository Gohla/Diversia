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

sigc::signal<void, EditorObject::GizmoMode> EditorObject::mGizmoModeSignal = sigc::signal<void, 
    EditorObject::GizmoMode>();

EditorObject::EditorObject( const String& rName, Mode mode, NetworkingType type, 
    const String& rDisplayName, RakNet::RakNetGUID source, RakNet::RakNetGUID ownGUID, 
    RakNet::RakNetGUID serverGUID, sigc::signal<void>& rUpdateSignal, 
    ClientObjectManager& rObjectManager, PermissionManager& rPermissionManager, 
    RakNet::ReplicaManager3& rReplicaManager, RakNet::NetworkIDManager& rNetworkIDManager, 
    RakNet::RPC3& rRPC3 ):
    ClientObject( rName, mode, type, rDisplayName, source, ownGUID, serverGUID, rUpdateSignal, 
        rObjectManager, rPermissionManager, rReplicaManager, rNetworkIDManager, rRPC3 ),
    mGizmo( 0 ),
    mGizmoMode( MOVEMENT ),
    mSelected( false )
{
    mGizmoModeConnection = mGizmoModeSignal.connect( sigc::mem_fun( this, 
        &EditorObject::gizmoModeChange ) );
}

EditorObject::~EditorObject()
{
    if( mGizmo ) delete mGizmo;
    mGizmoModeConnection.disconnect();
}

void EditorObject::setSelected( bool selected )
{
    mSelected = selected;

    EditorObject::checkGizmo();
    ClientObject::setSelected( selected );
}

void EditorObject::gizmoModeChange( GizmoMode mode )
{
    mGizmoMode = mode;

    EditorObject::checkGizmo();
}

void EditorObject::checkGizmo()
{
    if( mGizmo ) 
    {
        delete mGizmo;
        mGizmo = 0;
    }

    if( mSelected )
    {
        switch( mGizmoMode )
        {
            case SCALING: case NONE: mGizmo = 0; break;
            case MOVEMENT: mGizmo = new TranslationGizmo( *this ); break;
            case ROTATION: mGizmo = new RotationGizmo( *this ); break;
        }
        if( mGizmo ) Object::getComponent<SceneNode>().getNode()->addChild( mGizmo->getSceneNode() );
    }
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia