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
std::set<EditorObject*> EditorObject::mSelectedObjects = std::set<EditorObject*>();
std::set<Gizmo*> EditorObject::mControlledGizmos = std::set<Gizmo*>();
Gizmo* EditorObject::mMultiGizmo = 0;

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
    ClientObject::setSelected( selected );

    mSelected = selected;

    if( selected )
    {
        EditorObject::checkGizmo();
        EditorObject::selectionChange( this, true );
    }
    else
    {
        EditorObject::selectionChange( this, false );
        EditorObject::checkGizmo();
    }
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
        mGizmo = EditorObject::createGizmo( mGizmoMode, this );

        if( mGizmo ) 
        {
            // Remove from root scene node and add to this object's scene node.
            mGizmo->getSceneNode()->getParent()->removeChild( mGizmo->getSceneNode() );
            Object::getComponent<SceneNode>().getNode()->addChild( mGizmo->getSceneNode() );

            mGizmo->setSnapToGrid( mSnapToGrid );
        }
    }
}

void EditorObject::selectionChange( EditorObject* pObject, bool selected )
{
    if( selected )
    {
        mSelectedObjects.insert( pObject );
        if( pObject->mGizmo ) mControlledGizmos.insert( pObject->mGizmo );
    }
    else
    {
        mSelectedObjects.erase( pObject );
        if( pObject->mGizmo ) mControlledGizmos.erase( pObject->mGizmo );
    }

    if( mGizmoMode == NONE ) return; 

    if( mSelectedObjects.size() > 1 )
    {
        Ogre::Vector3 centerPosition( Ogre::Vector3::ZERO );

        for( std::set<EditorObject*>::iterator i = mSelectedObjects.begin(); 
            i != mSelectedObjects.end(); ++i )
        {
            (*i)->mGizmo->setVisible( false );
            centerPosition += (*i)->mGizmo->getSceneNode()->_getDerivedPosition();
        }

        if( !mMultiGizmo && mGizmoMode != NONE )
        {
            mMultiGizmo = EditorObject::createGizmo( mGizmoMode );
            mMultiGizmo->setControlledGizmos( mControlledGizmos );
        }
        if( mMultiGizmo ) 
        {
            mMultiGizmo->getSceneNode()->setPosition( centerPosition / mSelectedObjects.size() );
            mMultiGizmo->transformChanged();
            mMultiGizmo->setSnapToGrid( mSnapToGrid );
        }
    }
    else if( mSelectedObjects.size() )
    {
        (*mSelectedObjects.begin())->mGizmo->setVisible( true );

        if( mMultiGizmo )
        {
            delete mMultiGizmo;
            mMultiGizmo = 0;
        }
    }
}

Gizmo* EditorObject::createGizmo( GizmoMode mode, EditorObject* pObject /*= 0*/ )
{
    switch( mode )
    {
        case MOVEMENT: return new TranslationGizmo( pObject );
        case ROTATION: return new RotationGizmo( pObject );
        case SCALING: return new ScaleGizmo( pObject );
        case NONE: return 0;
    }

    return 0;
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia