/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include "OgreClient/Platform/StableHeaders.h"

#include "Client/Object/ClientObject.h"
#include "OgreClient/Graphics/CameraManager.h"
#include "OgreClient/Graphics/GeometryHelper.h"
#include "OgreClient/Graphics/QueryFlags.h"
#include "OgreClient/Graphics/TranslationGizmo.h"
#include "OgreClient/Input/ObjectSelection.h"
#include "Util/Math/Node.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

const Real TranslationGizmo::mSelectionHelperSize = 0.2f;

TranslationGizmo::TranslationGizmo( ClientObject* pControlledObject /*= 0*/ ):
    Gizmo( pControlledObject ),
    mHoverAxis( NO_AXIS ),
    mDragAxis( NO_AXIS ),
    mLineX( GeometryHelper::createLine( "Axis_X", Ogre::Vector3::ZERO, 
        Ogre::Vector3::UNIT_X ) ),
    mLineY( GeometryHelper::createLine( "Axis_Y", Ogre::Vector3::ZERO, 
        Ogre::Vector3::UNIT_Y ) ),
    mLineZ( GeometryHelper::createLine( "Axis_Z", Ogre::Vector3::ZERO, 
        Ogre::Vector3::UNIT_Z ) ),
    mConeX( GeometryHelper::createCone( "Axis_X", Ogre::Vector3::UNIT_X, 
        Ogre::Vector3::UNIT_X, 0.03f, 8, 0.1f ) ),
    mConeY( GeometryHelper::createCone( "Axis_Y", Ogre::Vector3::UNIT_Y, 
        Ogre::Vector3::UNIT_Y, 0.03f, 8, 0.1f ) ),
    mConeZ( GeometryHelper::createCone( "Axis_Z", Ogre::Vector3::UNIT_Z, 
        Ogre::Vector3::UNIT_Z, 0.03f, 8, 0.1f ) ),
    mSelectionHelperX( TranslationGizmo::createSelectionHelperBox( 
        Ogre::Vector3( 0.5f + mSelectionHelperSize * 0.5f, 0.0f, 0.0f ), 
        Ogre::Vector3( 1.0f + mSelectionHelperSize, mSelectionHelperSize, mSelectionHelperSize ) ) ),
    mSelectionHelperY( TranslationGizmo::createSelectionHelperBox( 
        Ogre::Vector3( 0.0f, 0.5f + mSelectionHelperSize * 0.5f, 0.0f ), 
        Ogre::Vector3( mSelectionHelperSize, 1.0f + mSelectionHelperSize, mSelectionHelperSize ) ) ),
    mSelectionHelperZ( TranslationGizmo::createSelectionHelperBox( 
        Ogre::Vector3( 0.0f, 0.0f, 0.5f + mSelectionHelperSize * 0.5f ), 
        Ogre::Vector3( mSelectionHelperSize, mSelectionHelperSize, 1.0f + mSelectionHelperSize ) ) ),
    mXNode( Gizmo::getSceneNode()->createChildSceneNode() ),
    mYNode( Gizmo::getSceneNode()->createChildSceneNode() ),
    mZNode( Gizmo::getSceneNode()->createChildSceneNode() ),
    mDesiredPosition( pControlledObject ? pControlledObject->getPosition() : 
        toVector3<Vector3>( Gizmo::getSceneNode()->getPosition() ) ),
    mClearMouseState( true )
{
    mController = this;
    mUserObject = this;

    Gizmo::getSceneNode()->setInheritOrientation( false );

    // Set render queues
    mLineX->setRenderQueueGroup( Ogre::RENDER_QUEUE_9);
    mLineY->setRenderQueueGroup( Ogre::RENDER_QUEUE_9 );
    mLineZ->setRenderQueueGroup( Ogre::RENDER_QUEUE_9 );
    mConeX->setRenderQueueGroup( Ogre::RENDER_QUEUE_9 );
    mConeY->setRenderQueueGroup( Ogre::RENDER_QUEUE_9 );
    mConeZ->setRenderQueueGroup( Ogre::RENDER_QUEUE_9 );

    // Setup axis geometry
    Gizmo::getSceneNode()->attachObject( mLineX ); 
    Gizmo::getSceneNode()->attachObject( mLineY ); 
    Gizmo::getSceneNode()->attachObject( mLineZ ); 
    Gizmo::getSceneNode()->attachObject( mConeX ); 
    Gizmo::getSceneNode()->attachObject( mConeY ); 
    Gizmo::getSceneNode()->attachObject( mConeZ ); 

    // Setup selection helpers
    Gizmo::getSceneNode()->attachObject( mSelectionHelperX );
    mSelectionHelperX->getUserObjectBindings().setUserAny( "Object", Ogre::Any( &mUserObject ) );
    mSelectionHelperX->getUserObjectBindings().setUserAny( "Param", Ogre::Any( (int)X_AXIS ) );
    Gizmo::getSceneNode()->attachObject( mSelectionHelperY );
    mSelectionHelperY->getUserObjectBindings().setUserAny( "Object", Ogre::Any( &mUserObject ) );
    mSelectionHelperY->getUserObjectBindings().setUserAny( "Param", Ogre::Any( (int)Y_AXIS ) );
    Gizmo::getSceneNode()->attachObject( mSelectionHelperZ );
    mSelectionHelperZ->getUserObjectBindings().setUserAny( "Object", Ogre::Any( &mUserObject ) );
    mSelectionHelperZ->getUserObjectBindings().setUserAny( "Param", Ogre::Any( (int)Z_AXIS ) );

    // Setup transformation helper nodes.
    mXNode->translate( 1.0f, 0.0f, 0.0f );
    mYNode->translate( 0.0f, 1.0f, 0.0f );
    mZNode->translate( 0.0f, 0.0f, 1.0f );

    // Get hover and drag notifications.
    GlobalsBase::mSelection->setHoverSlot( mUserObject, sigc::mem_fun( this, 
        &TranslationGizmo::hover ) );
    GlobalsBase::mSelection->setDragSlot( mUserObject, sigc::mem_fun( this, 
        &TranslationGizmo::drag ) );

    mUpdateConnection = GlobalsBase::mUpdateSignal->connect( sigc::mem_fun( this, 
        &TranslationGizmo::update ) );
    mUpdateConnection.block( true );
    
    if( pControlledObject ) mPositionChangeConnection = 
        pControlledObject->connectLocalTransformChange( sigc::mem_fun( this, 
        &TranslationGizmo::transformChange ) );
}

TranslationGizmo::~TranslationGizmo()
{
    GlobalsBase::mScene->destroyManualObject( mLineX );
    GlobalsBase::mScene->destroyManualObject( mLineY );
    GlobalsBase::mScene->destroyManualObject( mLineZ );
    GlobalsBase::mScene->destroyManualObject( mConeX );
    GlobalsBase::mScene->destroyManualObject( mConeY );
    GlobalsBase::mScene->destroyManualObject( mConeZ );
    GlobalsBase::mScene->destroyEntity( mSelectionHelperX );
    GlobalsBase::mScene->destroyEntity( mSelectionHelperY );
    GlobalsBase::mScene->destroyEntity( mSelectionHelperZ );
    GlobalsBase::mScene->destroySceneNode( mXNode );
    GlobalsBase::mScene->destroySceneNode( mYNode );
    GlobalsBase::mScene->destroySceneNode( mZNode );

    GlobalsBase::mSelection->removeObject( mUserObject );
    GlobalsBase::mSelection->removeHoverSlot( mUserObject );
    GlobalsBase::mSelection->removeDragSlot( mUserObject );
    mUpdateConnection.disconnect();
    mPositionChangeConnection.disconnect();

    if( mDragAxis != NO_AXIS ) mMouse->mCapture = false;
}

void TranslationGizmo::setVisible( bool visible )
{
    mLineX->setVisible( visible );
    mLineY->setVisible( visible );
    mLineZ->setVisible( visible );
    mConeX->setVisible( visible );
    mConeY->setVisible( visible );
    mConeZ->setVisible( visible );

    if( visible )
    {
        mSelectionHelperX->setQueryFlags( QueryFlags_Gizmo );
        mSelectionHelperY->setQueryFlags( QueryFlags_Gizmo );
        mSelectionHelperZ->setQueryFlags( QueryFlags_Gizmo );
    }
    else 
    {
        mSelectionHelperX->setQueryFlags( QueryFlags_NonSelectable );
        mSelectionHelperY->setQueryFlags( QueryFlags_NonSelectable );
        mSelectionHelperZ->setQueryFlags( QueryFlags_NonSelectable );
    }
}

void TranslationGizmo::transformChanged()
{
    mDesiredPosition = toVector3<Vector3>( Gizmo::getSceneNode()->getPosition() );
}

Ogre::Entity* TranslationGizmo::createSelectionHelperBox( const Ogre::Vector3& center, 
    const Ogre::Vector3& size )
{
    Ogre::Entity* entity = GeometryHelper::createAxisAlignedBoxEntity( "BaseWhiteNoLighting", 
        center, size );
    entity->setVisible( false );
    entity->setQueryFlags( QueryFlags_Gizmo );
    return entity;
}

void TranslationGizmo::hover( bool hoverIn, int param )
{
    if( hoverIn ) mHoverAxis = (Axis)param;
    else mHoverAxis = NO_AXIS;

    TranslationGizmo::checkHighlight();
}

void TranslationGizmo::drag( bool dragStart, int param, const Vector3& rPosition )
{
    if( dragStart ) mDragAxis = (Axis)param;
    else mDragAxis = NO_AXIS;

    TranslationGizmo::checkHighlight();

    if( mDragAxis != NO_AXIS ) 
    {
        // Duplicate if control key is being hold
        if( Gizmo::getControlledObject() && GlobalsBase::mInput->getKeyboardState().isButtonDown( 
            KC_LCONTROL ) ) Gizmo::getControlledObject()->duplicate();

        mMouse->mCapture = true;
        mController = this;

        // If this is a multi selection gizmo (no controllable object) then it will control
        // a drag operation of multiple gizmo's. The gizmo class will call the update() method.
        if( Gizmo::controlGizmos( true, param, rPosition, 
            GlobalsBase::mInput->getKeyboardState().isButtonDown( KC_LCONTROL ) ) )
        {
            mClearMouseState = false;
        }
        else
        {
            mClearMouseState = true;
            mUpdateConnection.block( false );
        }
    }
    else
    {
        mMouse->mCapture = false;
        mUpdateConnection.block( true );

        Gizmo::controlGizmos( false, param, rPosition, 
            GlobalsBase::mInput->getKeyboardState().isButtonDown( KC_LCONTROL ) );
    }
}

void TranslationGizmo::controlGizmo( bool control, int param, const Vector3& rPosition, 
    bool duplicate, Gizmo* pController )
{
    mClearMouseState = false;

    if( control )
    {
        mDragAxis = (Axis)param;
        mController = static_cast<TranslationGizmo*>( pController );
        mDesiredPosition = Gizmo::getControlledObject()->getPosition();
        if( duplicate && Gizmo::getControlledObject() ) Gizmo::getControlledObject()->duplicate();
    }
    else
    {
        mDragAxis = NO_AXIS;
    }
}

void TranslationGizmo::checkHighlight()
{
    mLineX->setMaterialName( 0, mHoverAxis == X_AXIS || mDragAxis == X_AXIS ? "Axis_Selected" : "Axis_X" );
    mConeX->setMaterialName( 0, mHoverAxis == X_AXIS || mDragAxis == X_AXIS ? "Axis_Selected" : "Axis_X" );
    mLineY->setMaterialName( 0, mHoverAxis == Y_AXIS || mDragAxis == Y_AXIS ? "Axis_Selected" : "Axis_Y" );
    mConeY->setMaterialName( 0, mHoverAxis == Y_AXIS || mDragAxis == Y_AXIS ? "Axis_Selected" : "Axis_Y" );
    mLineZ->setMaterialName( 0, mHoverAxis == Z_AXIS || mDragAxis == Z_AXIS ? "Axis_Selected" : "Axis_Z" );
    mConeZ->setMaterialName( 0, mHoverAxis == Z_AXIS || mDragAxis == Z_AXIS ? "Axis_Selected" : "Axis_Z" );
}

void TranslationGizmo::update()
{
    Ogre::Camera* camera = GlobalsBase::mCamera->getActiveCamera();
    Ogre::Matrix4 projView = camera->getProjectionMatrix() * camera->getViewMatrix();

    float incMouseX = mMouse->mMouseState.x.rel * 0.1;
    float incMouseY = mMouse->mMouseState.y.rel * 0.1;

    switch( mDragAxis )
    {
        case X_AXIS: 
        {
            Ogre::Vector3 vec = ( projView * mController->mXNode->_getDerivedPosition() ) - 
                ( projView * mController->Gizmo::getSceneNode()->_getDerivedPosition() );
            vec.z = 0.0f;
            /*if ( vec.length() != 0.0f ) */vec.normalise();/* else return;*/
            float x = (incMouseX * vec.x) - (incMouseY * vec.y);
            mDesiredPosition += Vector3( x, 0.0, 0.0 ); break;
        }
        case Y_AXIS: 
        {
            Ogre::Vector3 vec = ( projView * mController->mYNode->_getDerivedPosition() ) - 
                ( projView * mController->Gizmo::getSceneNode()->_getDerivedPosition() );
            vec.z = 0.0f;
            /*if ( vec.length() != 0.0f ) */vec.normalise();/* else return;*/
            float y = (incMouseX * vec.x) - (incMouseY * vec.y);
            mDesiredPosition += Vector3( 0.0, y, 0.0 ); break;
        }
        case Z_AXIS: 
        {
            Ogre::Vector3 vec = ( projView * mController->mZNode->_getDerivedPosition() ) - 
                ( projView * mController->Gizmo::getSceneNode()->_getDerivedPosition() );
            vec.z = 0.0f;
            /*if ( vec.length() != 0.0f ) */vec.normalise();/* else return;*/
            float z = (incMouseX * vec.x) - (incMouseY * vec.y);
            mDesiredPosition += Vector3( 0.0, 0.0, z ); break;
        }
    }

    mPositionChangeConnection.block( true );

    if( Gizmo::isSnappingToGrid() )
    {
        Vector3 newPosition;
        newPosition.x = (int)mDesiredPosition.x % 256;
        newPosition.y = (int)mDesiredPosition.y % 256;
        newPosition.z = (int)mDesiredPosition.z % 256;

        if( Gizmo::getControlledObject() ) Gizmo::getControlledObject()->setPosition( newPosition );
        else Gizmo::getSceneNode()->setPosition( toVector3<Ogre::Vector3>( newPosition ) );
    }
    else 
    {
        if( Gizmo::getControlledObject() ) Gizmo::getControlledObject()->setPosition( mDesiredPosition );
        else Gizmo::getSceneNode()->setPosition( toVector3<Ogre::Vector3>( mDesiredPosition ) );
    }

    mPositionChangeConnection.block( false );

    if( mClearMouseState ) mMouse->mMouseState.clear();
}

void TranslationGizmo::transformChange( const Node& rNode )
{
    mDesiredPosition = rNode.getPosition();
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia