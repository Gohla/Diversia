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
#include "OgreClient/Graphics/GeometryHelper.h"
#include "OgreClient/Graphics/QueryFlags.h"
#include "OgreClient/Graphics/RotationGizmo.h"
#include "OgreClient/Input/ObjectSelection.h"
#include "CameraManager.h"
#include "ProceduralTorusGenerator.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

const Real RotationGizmo::mRadius = 1.0f;

RotationGizmo::RotationGizmo( ClientObject* pControlledObject /*= 0*/ ):
    Gizmo( pControlledObject ),
    mHoverAxis( NO_AXIS ),
    mDragAxis( NO_AXIS ),
    mName( boost::lexical_cast<String>( pControlledObject->GetNetworkID() ) ),
    mCircleX( GeometryHelper::createCircleYZ( "Axis_X", mRadius, mSides ) ),
    mCircleY( GeometryHelper::createCircleXZ( "Axis_Y", mRadius, mSides ) ),
    mCircleZ( GeometryHelper::createCircleXY( "Axis_Z", mRadius, mSides ) ),
    mDiscX( GeometryHelper::createCone( "Disc_X", Ogre::Vector3::ZERO, Ogre::Vector3::UNIT_X, 
        mRadius, mSides, 0.001f ) ),
    mDiscY( GeometryHelper::createCone( "Disc_Y", Ogre::Vector3::ZERO, Ogre::Vector3::UNIT_Y, 
        mRadius, mSides, 0.001f ) ),
    mDiscZ( GeometryHelper::createCone( "Disc_Z", Ogre::Vector3::ZERO, Ogre::Vector3::UNIT_Z, 
        mRadius, mSides, 0.001f ) ),
    mSelectionHelperX( RotationGizmo::createSelectionHelperBox( "X" ) ),
    mSelectionHelperY( RotationGizmo::createSelectionHelperBox( "Y" ) ),
    mSelectionHelperZ( RotationGizmo::createSelectionHelperBox( "Z" ) ),
    mArrowIndicator( GlobalsBase::mScene->createManualObject() ),
    mArrowNode( GlobalsBase::mScene->getRootSceneNode()->createChildSceneNode() )
{
    mUserObject = this;

    // Setup axis geometry
    Gizmo::getSceneNode()->attachObject( mCircleX );
    Gizmo::getSceneNode()->attachObject( mCircleY );
    Gizmo::getSceneNode()->attachObject( mCircleZ );
    mDiscX->setVisible( false );
    mDiscY->setVisible( false );
    mDiscZ->setVisible( false );
    Gizmo::getSceneNode()->attachObject( mDiscX );
    Gizmo::getSceneNode()->attachObject( mDiscY );
    Gizmo::getSceneNode()->attachObject( mDiscZ );

    // Setup selection helpers
    mSelectionHelperX->setVisible( false );
    mSelectionHelperX->setQueryFlags( QueryFlags_Gizmo );
    mSelectionHelperX->getUserObjectBindings().setUserAny( "Object", Ogre::Any( &mUserObject ) );
    mSelectionHelperX->getUserObjectBindings().setUserAny( "Param", Ogre::Any( (int)X_AXIS ) );
    mSelectionHelperY->setVisible( false );
    mSelectionHelperY->setQueryFlags( QueryFlags_Gizmo );
    mSelectionHelperY->getUserObjectBindings().setUserAny( "Object", Ogre::Any( &mUserObject ) );
    mSelectionHelperY->getUserObjectBindings().setUserAny( "Param", Ogre::Any( (int)Y_AXIS ) );
    mSelectionHelperZ->setVisible( false );
    mSelectionHelperZ->setQueryFlags( QueryFlags_Gizmo );
    mSelectionHelperZ->getUserObjectBindings().setUserAny( "Object", Ogre::Any( &mUserObject ) );
    mSelectionHelperZ->getUserObjectBindings().setUserAny( "Param", Ogre::Any( (int)Z_AXIS ) );
    Gizmo::getSceneNode()->createChildSceneNode( Ogre::Vector3::ZERO, Ogre::Quaternion( Ogre::Degree( 90 ), Ogre::Vector3::UNIT_Z ) )->attachObject( mSelectionHelperX );
    Gizmo::getSceneNode()->createChildSceneNode( Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY )->attachObject( mSelectionHelperY );
    Gizmo::getSceneNode()->createChildSceneNode( Ogre::Vector3::ZERO, Ogre::Quaternion( Ogre::Degree( 90 ), Ogre::Vector3::UNIT_X ) )->attachObject( mSelectionHelperZ );

    // Setup arrow indicator
    mArrowIndicator->setDynamic( true );
    mArrowIndicator->begin( "Axis_Selected", Ogre::RenderOperation::OT_LINE_LIST );
    mArrowIndicator->position( 0.0f, 0.0f, 0.0f );
    mArrowIndicator->position( 1.0f, 0.0f, 0.0f );
    mArrowIndicator->position( 1.0f, 0.0f, 0.0f );
    mArrowIndicator->position( 0.8f, 0.1f, 0.0f );
    mArrowIndicator->position( 1.0f, 0.0f, 0.0f );
    mArrowIndicator->position( 0.8f, -0.1f, 0.0f );
    mArrowIndicator->end();
    mArrowIndicator->setVisible( false );
    mArrowNode->setInheritOrientation( false );
    mArrowNode->attachObject( mArrowIndicator );

    // Get hover and drag notifications.
    GlobalsBase::mSelection->setHoverSlot( mUserObject, sigc::mem_fun( this, 
        &RotationGizmo::hover ) );
    GlobalsBase::mSelection->setDragSlot( mUserObject, sigc::mem_fun( this, 
        &RotationGizmo::drag ) );

    mUpdateConnection = GlobalsBase::mUpdateSignal->connect( sigc::mem_fun( this, 
        &RotationGizmo::update ) );
    mUpdateConnection.block( true );
}

RotationGizmo::~RotationGizmo()
{
    GlobalsBase::mScene->destroyManualObject( mCircleX );
    GlobalsBase::mScene->destroyManualObject( mCircleY );
    GlobalsBase::mScene->destroyManualObject( mCircleZ );
    GlobalsBase::mScene->destroyManualObject( mDiscX );
    GlobalsBase::mScene->destroyManualObject( mDiscY );
    GlobalsBase::mScene->destroyManualObject( mDiscZ );
    GlobalsBase::mScene->destroyEntity( mSelectionHelperX );
    GlobalsBase::mScene->destroyEntity( mSelectionHelperY );
    GlobalsBase::mScene->destroyEntity( mSelectionHelperZ );
    Ogre::MeshManager::getSingletonPtr()->remove( mName + "HelperX" );
    Ogre::MeshManager::getSingletonPtr()->remove( mName + "HelperY" );
    Ogre::MeshManager::getSingletonPtr()->remove( mName + "HelperZ" );
    GlobalsBase::mScene->destroyManualObject( mArrowIndicator );
    GlobalsBase::mScene->destroySceneNode( mArrowNode );

    GlobalsBase::mSelection->removeObject( mUserObject );
    GlobalsBase::mSelection->removeHoverSlot( mUserObject );
    GlobalsBase::mSelection->removeDragSlot( mUserObject );
    mUpdateConnection.disconnect();

    if( mDragAxis != NO_AXIS ) mMouse->mCapture = false;
}

void RotationGizmo::setVisible( bool visible )
{
    mCircleX->setVisible( visible );
    mCircleY->setVisible( visible );
    mCircleZ->setVisible( visible );
    mDiscX->setVisible( visible );
    mDiscY->setVisible( visible );
    mDiscZ->setVisible( visible );
}

Ogre::Entity* RotationGizmo::createSelectionHelperBox( const String& rName )
{
    Procedural::TorusGenerator()
        .setRadius( 1 )
        .setSectionRadius( 0.1 )
        .setNumSegCircle( 20 )
        .setNumSegSection( 20 )
        .realizeMesh( mName + "Helper" + rName );
    return GlobalsBase::mScene->createEntity( mName + "Helper" + rName );
}

void RotationGizmo::hover( bool hoverIn, int param )
{
    if( hoverIn ) mHoverAxis = (Axis)param;
    else mHoverAxis = NO_AXIS;

    RotationGizmo::checkHighlight();
}

void RotationGizmo::drag( bool dragStart, int param, const Vector3& rPosition )
{
    if( dragStart ) mDragAxis = (Axis)param;
    else mDragAxis = NO_AXIS;

    RotationGizmo::checkHighlight();

    if( mDragAxis != NO_AXIS ) 
    {
        mMouse->mCapture = true;
        mUpdateConnection.block( false );
        mArrowNode->setScale( Gizmo::getSceneNode()->getScale() );
        mHitPoint = toVector3<Ogre::Vector3>( rPosition );
        mRotationAccumulator = 0;

        Ogre::Plane plane( Ogre::Vector3::UNIT_X, 0.0f );
        switch( mDragAxis )
        {
            case X_AXIS:
                mRotationAxis = Ogre::Vector3::UNIT_X;
                plane.normal = Ogre::Vector3::UNIT_X;
                mDiscX->setVisible( true );
                break;
            case Y_AXIS:
                mRotationAxis = Ogre::Vector3::UNIT_Y;
                plane.normal = Ogre::Vector3::UNIT_Y;
                mDiscY->setVisible( true );
                break;
            case Z_AXIS:
                mRotationAxis = Ogre::Vector3::UNIT_Z;
                plane.normal = Ogre::Vector3::UNIT_Z;
                mDiscZ->setVisible( true );
                break;
        }

        const Ogre::Quaternion& gizmoOrientation = Gizmo::getSceneNode()->_getDerivedOrientation();
        const Ogre::Vector3& gizmoPosition = Gizmo::getSceneNode()->_getDerivedPosition();
        Ogre::Vector3 rkVector = gizmoOrientation * mRotationAxis;
        Ogre::Matrix4 projView = GlobalsBase::mCamera->getActiveCamera()->getProjectionMatrix() * 
            GlobalsBase::mCamera->getActiveCamera()->getViewMatrix();

        Ogre::Vector3 vector2 = gizmoPosition - mHitPoint;
        vector2.normalise();
        Ogre::Vector3 vector3 = vector2.crossProduct(rkVector);
        Ogre::Vector3 vector4 = mHitPoint + vector3;
        Ogre::Vector3 vector6 = vector4;
        Ogre::Vector3 vector7 = projView * mHitPoint;
        Ogre::Vector3 vector8 = projView * vector6;
        mDirector = vector8 - vector7;
        mDirector.z = 0.0f;
        if (mDirector.length() != 0.0f)
        {
            mDirector.normalise();
            Ogre::Vector3 vector9 = plane.projectVector( gizmoOrientation.Inverse() * vector2 );
            Ogre::Vector3 vector10 = plane.projectVector( gizmoOrientation.Inverse() * vector3 );
            vector9.normalise();
            vector10.normalise();
            Ogre::Vector3 scale = Gizmo::getSceneNode()->getScale();
            if (((scale.x != 0.0f) && (scale.y != 0.0f)) && (scale.z != 0.0f))
            {
                mArrowIndicator->beginUpdate( 0 );
                Ogre::Vector3 pos = gizmoPosition / scale;
                mArrowIndicator->position(pos);
                Ogre::Vector3 vector13 = pos + (gizmoOrientation * vector9);
                mArrowIndicator->position(vector13);
                Ogre::Vector3 vector1 = Ogre::Vector3::ZERO + (gizmoOrientation * vector10);
                mArrowIndicator->position(vector13);
                mArrowIndicator->position(vector13 + (gizmoOrientation * vector10));
                mArrowIndicator->position(vector13);
                mArrowIndicator->position(vector13 - (gizmoOrientation * vector10));
                mArrowIndicator->end();
                //mArrowIndicator->setVisible( true );
            }
        }
    }
    else
    {
        mMouse->mCapture = false;
        mUpdateConnection.block( true );

        mDiscX->setVisible( false );
        mDiscY->setVisible( false );
        mDiscZ->setVisible( false );
        mArrowIndicator->setVisible( false );
    }
}

void RotationGizmo::controlGizmo( bool control, int param, const Vector3& rPosition, 
    bool duplicate, Gizmo* pController )
{
 
}

void RotationGizmo::checkHighlight()
{
    mCircleX->setMaterialName( 0, mHoverAxis == X_AXIS || mDragAxis == X_AXIS ? "Axis_Selected" : "Axis_X" );
    mCircleY->setMaterialName( 0, mHoverAxis == Y_AXIS || mDragAxis == Y_AXIS ? "Axis_Selected" : "Axis_Y" );
    mCircleZ->setMaterialName( 0, mHoverAxis == Z_AXIS || mDragAxis == Z_AXIS ? "Axis_Selected" : "Axis_Z" );
}

void RotationGizmo::update()
{
    mArrowNode->setScale( Gizmo::getSceneNode()->getScale() );
    Ogre::Radian rfAngle = Ogre::Radian( (mDirector.x * (Real)mMouse->mMouseState.x.rel) - 
        (mDirector.y * (Real)mMouse->mMouseState.y.rel) ) * 0.02;
    
    if( Gizmo::isSnappingToGrid() )
    {
        mRotationAccumulator += toRadian<Radian>( rfAngle );

        if( mRotationAccumulator.valueDegrees() >= 45 )
        {
            Gizmo::getControlledObject()->rotate( toVector3<Vector3>( mRotationAxis ), 
                Radian( Degree( 45 ) ), Node::TS_LOCAL );
            mRotationAccumulator = 0;
        }
        else if( mRotationAccumulator.valueDegrees() <= -45 )
        {
            Gizmo::getControlledObject()->rotate( toVector3<Vector3>( mRotationAxis ), 
                Radian( Degree( -45 ) ), Node::TS_LOCAL );
            mRotationAccumulator = 0;
        }
    }
    else
    {
        Gizmo::getControlledObject()->rotate( toVector3<Vector3>( mRotationAxis ), 
            toRadian<Radian>( rfAngle ), Node::TS_LOCAL );
    }

    mMouse->mMouseState.clear();
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia