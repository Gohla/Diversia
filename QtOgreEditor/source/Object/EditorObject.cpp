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
MultiSelection EditorObject::mMultiSelection = MultiSelection();

MultiSelection::MultiSelection() :
    mMultiGizmo( 0 )
{

}

void MultiSelection::init()
{
    QObject::connect( EditorGlobals::mMainWindow->mGizmoActions, SIGNAL( triggered(QAction*) ), 
        this, SLOT( gizmoModeChange(QAction*) ) );
    QObject::connect( EditorGlobals::mMainWindow->mUI.actionSnap_to_grid, SIGNAL( toggled(bool) ),
        this, SLOT( snapToGridChange(bool) ) );
}

void MultiSelection::selectionChange( EditorObject* pObject, bool selected )
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

    if( EditorObject::mGizmoMode == EditorObject::NONE ) return; 

    if( mSelectedObjects.size() > 1 )
    {
        Vector3 centerPosition( Vector3::ZERO );

        for( std::set<EditorObject*>::iterator i = mSelectedObjects.begin(); 
            i != mSelectedObjects.end(); ++i )
        {
            (*i)->mGizmo->setVisible( false );
            centerPosition += (*i)->_getDerivedPosition();
        }

        if( !mMultiGizmo && EditorObject::mGizmoMode != EditorObject::NONE )
        {
            mMultiGizmo = EditorObject::createGizmo( EditorObject::mGizmoMode, 0, 
                "MultiSelectGizmo" );
            mMultiGizmo->setControlledGizmos( mControlledGizmos );
        }
        if( mMultiGizmo ) 
        {
            mMultiGizmo->getSceneNode()->setPosition( toVector3<Ogre::Vector3>( 
                centerPosition / mSelectedObjects.size() ) );
            mMultiGizmo->transformChanged();
            mMultiGizmo->setSnapToGrid( EditorObject::mSnapToGrid );
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

void MultiSelection::gizmoChange( EditorObject* pObject )
{
    if( mSelectedObjects.find( pObject ) == mSelectedObjects.end() ) return;

    if( pObject->mLastGizmo ) mControlledGizmos.erase( pObject->mLastGizmo );
    if( pObject->mGizmo ) 
    {
        mControlledGizmos.insert( pObject->mGizmo );
        if( mSelectedObjects.size() > 1 ) pObject->mGizmo->setVisible( false );
    }

    if( mMultiGizmo ) mMultiGizmo->setControlledGizmos( mControlledGizmos );
}

void MultiSelection::gizmoModeChange( QAction* action )
{
    if( mSelectedObjects.size() < 2 ) return;

    Vector3 position = Vector3::ZERO;
    if( mMultiGizmo ) 
    {
        position = toVector3<Vector3>( mMultiGizmo->getSceneNode()->getPosition() );
        delete mMultiGizmo;
        mMultiGizmo = 0;
    }

    mMultiGizmo = EditorObject::createGizmo( EditorObject::getGizmoMode( action ), 0, 
        "MultiSelectGizmo" );
    if( !mMultiGizmo ) return;

    if( position == Vector3::ZERO )
    {
        for( std::set<EditorObject*>::iterator i = mSelectedObjects.begin(); 
            i != mSelectedObjects.end(); ++i )
        {
            position += (*i)->_getDerivedPosition();
        }

        position /= mSelectedObjects.size();
    }

    mMultiGizmo->setControlledGizmos( mControlledGizmos );
    mMultiGizmo->getSceneNode()->setPosition( toVector3<Ogre::Vector3>( position ) );
    mMultiGizmo->transformChanged();
    mMultiGizmo->setSnapToGrid( EditorObject::mSnapToGrid );
}

void MultiSelection::snapToGridChange( bool snap )
{
    if( mMultiGizmo ) 
    {
        mMultiGizmo->setSnapToGrid( snap );
        mMultiGizmo->transformChanged();
    }
}

//------------------------------------------------------------------------------

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
    mLastGizmo( 0 ),
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

    static bool init = true; if( init ) { mMultiSelection.init(); init = false; }
}

EditorObject::~EditorObject()
{
    if( mGizmo ) delete mGizmo;
}

void EditorObject::selected( bool selected )
{
    ClientObject::selected( selected );

    mSelected = selected;

    if( selected )
    {
        EditorObject::checkGizmo();
        mMultiSelection.selectionChange( this, true );
    }
    else
    {
        mMultiSelection.selectionChange( this, false );
        EditorObject::checkGizmo();
    }
}

void EditorObject::gizmoModeChange( QAction* action )
{
    mGizmoMode = EditorObject::getGizmoMode( action );
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
        mLastGizmo = mGizmo;
        delete mGizmo;
        mGizmo = 0;
    }

    if( mSelected )
    {
        mGizmo = EditorObject::createGizmo( mGizmoMode, this, 
            boost::lexical_cast<String>( NetworkIDObject::GetNetworkID() ) );

        if( mGizmo ) 
        {
            // Remove from root scene node and add to this object's scene node.
            mGizmo->getSceneNode()->getParent()->removeChild( mGizmo->getSceneNode() );
            Object::getComponent<SceneNode>().getNode()->addChild( mGizmo->getSceneNode() );

            mGizmo->setSnapToGrid( mSnapToGrid );
        }
    }

    mMultiSelection.gizmoChange( this );
}

EditorObject::GizmoMode EditorObject::getGizmoMode( QAction* action )
{
    if( action == EditorGlobals::mMainWindow->mUI.actionMovement_mode ) return MOVEMENT;
    else if( action == EditorGlobals::mMainWindow->mUI.actionRotation_mode ) return ROTATION;
    else if( action == EditorGlobals::mMainWindow->mUI.actionScaling_mode ) return SCALING;
    else if( action == EditorGlobals::mMainWindow->mUI.actionSelection_mode ) return NONE;

    return NONE;
}

Gizmo* EditorObject::createGizmo( GizmoMode mode, EditorObject* pObject /*= 0*/, 
    const String& rName /*= ""*/ )
{
    switch( mode )
    {
        case MOVEMENT: return new TranslationGizmo( pObject );
        case ROTATION: return new RotationGizmo( pObject, rName );
        case SCALING: return new ScaleGizmo( pObject );
        case NONE: return 0;
    }

    return 0;
}

//------------------------------------------------------------------------------

} // Namespace QtOgreEditor
} // Namespace Diversia