/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "Object/EditorObject.h"
#include "OgreClient/Graphics/RotationGizmo.h"
#include "OgreClient/Graphics/ScaleGizmo.h"
#include "OgreClient/Graphics/TranslationGizmo.h"
#include "OgreClient/Object/SceneNode.h"
#include "UI/MainWindow.h"

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

EditorObject::GizmoMode EditorObject::mGizmoMode = EditorObject::NONE;
bool EditorObject::mSnapToGrid = false;

EditorObject::EditorObject( const String& rName, Mode mode, NetworkingType type, 
    const String& rDisplayName, RakNet::RakNetGUID source, RakNet::RakNetGUID ownGUID, 
    RakNet::RakNetGUID serverGUID, sigc::signal<void>& rUpdateSignal, 
    ClientObjectManager& rObjectManager, PermissionManager& rPermissionManager, 
    RakNet::ReplicaManager3& rReplicaManager, RakNet::NetworkIDManager& rNetworkIDManager, 
    RakNet::RPC3& rRPC3 ):
    QObject( 0 ),
    ClientObject( rName, mode, type, rDisplayName, source, ownGUID, serverGUID, rUpdateSignal, 
        rObjectManager, rPermissionManager, rReplicaManager, rNetworkIDManager, rRPC3 ),
    mGizmo( 0 ),
    mSelected( false )
{
    if( EditorGlobals::mMainWindow->mUI.actionMovement_mode->isChecked() ) mGizmoMode = MOVEMENT;
    else if( EditorGlobals::mMainWindow->mUI.actionRotation_mode->isChecked() ) mGizmoMode = ROTATION;
    else if( EditorGlobals::mMainWindow->mUI.actionScaling_mode->isChecked() ) mGizmoMode = SCALING;
    else if( EditorGlobals::mMainWindow->mUI.actionSelection_mode->isChecked() ) mGizmoMode = NONE;

    if( EditorGlobals::mMainWindow->mUI.actionSnap_to_grid->isChecked() ) mSnapToGrid = true;

    QObject::connect( EditorGlobals::mMainWindow->mGizmoActions, SIGNAL( triggered(QAction*) ), 
        this, SLOT( gizmoModeChange(QAction*) ) );
    QObject::connect( EditorGlobals::mMainWindow->mUI.actionSnap_to_grid, SIGNAL( toggled(bool) ),
        this, SLOT( snapToGridChange(bool) ) );
}

EditorObject::~EditorObject()
{
    if( mGizmo ) delete mGizmo;
}

void EditorObject::setSelected( bool selected )
{
    mSelected = selected;

    EditorObject::checkGizmo();
    ClientObject::setSelected( selected );
}

void EditorObject::gizmoModeChange( QAction* action )
{
    if( action == EditorGlobals::mMainWindow->mUI.actionMovement_mode ) mGizmoMode = MOVEMENT;
    else if( action == EditorGlobals::mMainWindow->mUI.actionRotation_mode ) mGizmoMode = ROTATION;
    else if( action == EditorGlobals::mMainWindow->mUI.actionScaling_mode ) mGizmoMode = SCALING;
    else if( action == EditorGlobals::mMainWindow->mUI.actionSelection_mode ) mGizmoMode = NONE;

    EditorObject::checkGizmo();
}

void EditorObject::snapToGridChange( bool snap )
{
    mSnapToGrid = snap;
    if( mGizmo ) mGizmo->setSnapToGrid( snap );
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
            case MOVEMENT: mGizmo = new TranslationGizmo( *this ); break;
            case ROTATION: mGizmo = new RotationGizmo( *this ); break;
            case SCALING: mGizmo = new ScaleGizmo( *this ); break;
            case NONE: mGizmo = 0; break;
        }

        if( mGizmo ) 
        {
            Object::getComponent<SceneNode>().getNode()->addChild( mGizmo->getSceneNode() );
            mGizmo->setSnapToGrid( mSnapToGrid );
        }
    }
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia