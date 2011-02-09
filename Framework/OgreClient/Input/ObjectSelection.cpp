/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.

Diversia is free software: you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation, either version 3 of the License, or (at your option) any later 
version.

Diversia is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with 
Diversia. If not, see <http://www.gnu.org/licenses/>.

You may contact the author of Diversia by e-mail at: equabyte@sonologic.nl
-----------------------------------------------------------------------------
*/

#include "OgreClient/Platform/StableHeaders.h"

#include "OgreClient/Input/ObjectSelection.h"
#include "OgreClient/Input/InputManager.h"
#include "OgreClient/Graphics/QueryFlags.h"
#include "OgreClient/Graphics/CameraManager.h"
#include "OgreClient/Graphics/GraphicsManager.h"
#include "Client/Object/ClientObject.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

SelectionRectangle::SelectionRectangle( const String& rName ) : 
    Ogre::ManualObject( rName )
{
    Ogre::ManualObject::setRenderQueueGroup( Ogre::RENDER_QUEUE_OVERLAY );
    Ogre::ManualObject::setUseIdentityProjection( true );
    Ogre::ManualObject::setUseIdentityView( true );
}

void SelectionRectangle::setCorners( float left, float top, float right, float bottom )
{
    left = left * 2 - 1;
    right = right * 2 - 1;
    top = 1 - top * 2;
    bottom = 1 - bottom * 2;

    clear();
    begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);
    position(left, top, -1);
    position(right, top, -1);
    position(right, bottom, -1);
    position(left, bottom, -1);
    position(left, top, -1);

    index(0);
    index(1);
    index(2);
    index(3);
    index(0);

    end();

    Ogre::AxisAlignedBox box;
    box.setInfinite();
    setBoundingBox( box );
}

//------------------------------------------------------------------------------

ObjectSelection::ObjectSelection( unsigned int queryMask /*= QueryFlags_Entity*/ ):
    mCamera( GlobalsBase::mCamera->getActiveCamera() ),
    mCollisionTools( new MOC::CollisionTools( GlobalsBase::mScene ) ),
    mClick( false ),
    mDragClick( false ),
    mDragging( false ),
    mObjectUnderMouse( 0 ),
    mDraggingObject( 0 ),
    mRectangle( new SelectionRectangle( "Selection" ) ),
    mDoVolumeQuery( false ),
    mQueryMask( queryMask )
{
    GlobalsBase::mScene->getRootSceneNode()->createChildSceneNode()->attachObject( mRectangle );
    mVolumeQuery = GlobalsBase::mScene->createPlaneBoundedVolumeQuery( 
        Ogre::PlaneBoundedVolumeList() );

    GlobalsBase::mUpdateSignal->connect( sigc::mem_fun( this, &ObjectSelection::update ) );
    GlobalsBase::mCamera->connect( sigc::mem_fun( this, &ObjectSelection::cameraChange ) );
    GlobalsBase::mInput->subscribeMouse( *this );

    GlobalsBase::mSelection = this;
}

ObjectSelection::~ObjectSelection()
{
    GlobalsBase::mSelection = 0;

    GlobalsBase::mInput->unsubscribeMouse( *this );

    GlobalsBase::mScene->destroyQuery( mVolumeQuery );
    delete mRectangle;
}

void ObjectSelection::select( ClientObject& rObject, bool silent /*= false*/ )
{
    SelectedObjects::iterator i = mSelectedObjects.find( &rObject );
    if( i == mSelectedObjects.end() )
    {
        rObject.setSelected( true );
        mSelectedObjects.insert( &rObject );
        mObjectConnections.insert( std::make_pair( &rObject, rObject.connectDestruction( 
            sigc::mem_fun( this, &ObjectSelection::objectDestroyed ) ) ) );

        if( silent ) return;
        
        mSelectedSignal( rObject, true );
        CLOGD << "Selecting object " << rObject.getName();
    }
}

void ObjectSelection::deselect( ClientObject& rObject, bool silent /*= false*/ )
{
    rObject.setSelected( false );
    mSelectedObjects.erase( &rObject );

    // Disconnect from destruction signal.
    ObjectConnections::iterator i = mObjectConnections.find( &rObject );
    if( i != mObjectConnections.end() )
    {
        i->second.disconnect();
        mObjectConnections.erase( i );
    }

    if( silent ) return;

    mSelectedSignal( rObject, false );
    CLOGD << "Deselecting object " << rObject.getName();
}

void ObjectSelection::toggleSelection( ClientObject& rObject, bool silent /*= false*/ )
{
    rObject.isSelected() ? ObjectSelection::deselect( rObject, silent ) : 
        ObjectSelection::select( rObject, silent );
}

void ObjectSelection::cropSelection( ClientObject& rObject, bool silent /*= false*/ )
{
    ObjectSelection::deselectAll( true );
    ObjectSelection::select( rObject, silent );
}

void ObjectSelection::deselectAll( bool silent /*= false*/ )
{
    for( SelectedObjects::iterator i = mSelectedObjects.begin(); i != mSelectedObjects.end(); )
    {
        ClientObject* object = *i;
        object->setSelected( false );

        // Disconnect from destruction signal.
        ObjectConnections::iterator j = mObjectConnections.find( object );
        if( j != mObjectConnections.end() )
        {
            j->second.disconnect();
            mObjectConnections.erase( j );
        }

        mSelectedObjects.erase( i++ );

        if( !silent )
        {
            mSelectedSignal( *object, false );
            CLOGD << "Deselecting object " << object->getName();
        }
    }
}

bool ObjectSelection::mouseMoved( const MouseState& rState )
{
    mMouseState = rState;

    if( mDoVolumeQuery )
    {
        mDragStop.x = (float)mMouseState.x.abs / (float)GlobalsBase::mInput->getWindowWidth();
        mDragStop.y = (float)mMouseState.y.abs / (float)GlobalsBase::mInput->getWindowHeight();
        return true;
    }

    // Allow other mouse listeners to process mouse movement if mouse movement is not used.
    return mClick || mDragging || mDragClick;
}

bool ObjectSelection::mousePressed( const MouseButton button )
{
    switch( button )
    {
        case MB_Left:
        {
            if( !mClick )
            {
                mClick = true;
                mClickTimer.reset();

                if( GlobalsBase::mInput->getKeyboardState().isButtonDown( KC_LCONTROL ) )
                {
                    mDragStart.x = (float)mMouseState.x.abs / 
                        (float)GlobalsBase::mInput->getWindowWidth();
                    mDragStart.y = (float)mMouseState.y.abs / (
                        float)GlobalsBase::mInput->getWindowHeight();
                    mDragClick = true;
                    mDraggingObject = 0;
                    return true;
                }

                if( mObjectUnderMouse )
                {
                    mDragClick = true;
                    mDraggingObject = mObjectUnderMouse;
                    return true;
                }
            }

            break;
        }
    }

    return false;
}

void ObjectSelection::mouseReleased( const MouseButton button )
{
    switch( button )
    {
        case MB_Left:
        {
            if( mObjectUnderMouse && mClick && mClickTimer.getMilliseconds() < 150 )
            {
                if( GlobalsBase::mInput->getKeyboardState().isButtonDown( KC_LCONTROL ) && 
                    mObjectUnderMouse->isSelected() )
                {
                    // Remove object under cursor from selection.
                    ObjectSelection::deselect( *mObjectUnderMouse );
                }
                else if( GlobalsBase::mInput->getKeyboardState().isButtonDown( KC_LCONTROL ) && 
                    !mObjectUnderMouse->isSelected() )
                {
                    // Add object under cursor to selection.
                    ObjectSelection::select( *mObjectUnderMouse );
                    mClickSignal( *mObjectUnderMouse );
                }
                else if( !GlobalsBase::mInput->getKeyboardState().isButtonDown( KC_LCONTROL ) && 
                    mObjectUnderMouse->isSelected() )
                {
                    // Crop selection to object under cursor.
                    ObjectSelection::cropSelection( *mObjectUnderMouse );
                    mClickSignal( *mObjectUnderMouse );
                }
                else if( !GlobalsBase::mInput->getKeyboardState().isButtonDown( KC_LCONTROL ) && 
                    !mObjectUnderMouse->isSelected() )
                {
                    // Deselect everything and only select the object under cursor.
                    ObjectSelection::deselectAll();
                    ObjectSelection::select( *mObjectUnderMouse );
                    mClickSignal( *mObjectUnderMouse );
                }
            }
            else if( mClick && mClickTimer.getMilliseconds() < 150 && 
                !GlobalsBase::mInput->getKeyboardState().isButtonDown( KC_LCONTROL ) )
            {
                ObjectSelection::deselectAll();
            }
            else if( mDoVolumeQuery )
            {
                ObjectSelection::doVolumeSelect();
            }
            else if( mDragging )
            {
                mDragSignal( *mDraggingObject, false );
                CLOGD << "Stopped dragging object " << mDraggingObject->getName();
            }
        }
    }

    mClick = false;
    mDragClick = false;
    mDragging = false;
    mDoVolumeQuery = false;
    mDraggingObject = 0;
    mRectangle->setVisible( false );
}

void ObjectSelection::update()
{
    ClientObject* currentObjectUnderMouse = mObjectUnderMouse;
    mObjectUnderMouse = 0;
    Ogre::Entity* entity = NULL;
    Ogre::Vector3 result = Ogre::Vector3::ZERO;
    float distToColl;

    // Find object under the mouse.
    if ( mCollisionTools->raycastFromCamera( GlobalsBase::mGraphics->getWindow(), mCamera, 
        Ogre::Vector2( mMouseState.x.abs, mMouseState.y.abs ), result, entity, distToColl, 
        mQueryMask ) )
    {
        // Entity detected under the mouse.
        ClientObject* object = ObjectSelection::getObject( entity );
        if( object != currentObjectUnderMouse )
        {
            if( currentObjectUnderMouse )
            {
                mHoverSignal( *currentObjectUnderMouse, false );
            }
            mObjectUnderMouse = object;
            mHoverSignal( *mObjectUnderMouse, true );
        }
        else
        {
            mObjectUnderMouse = object;
        }
    }

    // Detect dragging
    if( mDragClick && GlobalsBase::mInput->getKeyboardState().isButtonDown( KC_LCONTROL ) && 
        !mDoVolumeQuery )
    {
        // If Ctrl is hold, start a volume query to select multiple objects.
        mDoVolumeQuery = true;
        mDragStop = mDragStart;
        mRectangle->clear();
        mRectangle->setVisible( true );
    }
    else if( mDragClick && mClickTimer.getMilliseconds() >= 150 && !mDragging && mDraggingObject )
    {
        mDragging = true;
        mDragSignal( *mDraggingObject, true );
        CLOGD << "Started dragging object " << mDraggingObject->getName();
    }

    // Update volume query rectangle.
    if( mDoVolumeQuery )
    {
        mRectangle->setCorners( mDragStart, mDragStop );
    }
}

void ObjectSelection::objectDestroyed( Object& rObject )
{
    ObjectSelection::deselect( static_cast<ClientObject&>( rObject ) );
}

void ObjectSelection::cameraChange( Ogre::Camera* pCamera )
{
    mCamera = pCamera;
}

ClientObject* ObjectSelection::getObject( Ogre::MovableObject* pMovableObject )
{
    const Ogre::Any& object = pMovableObject->getUserAny();
    if( !object.isEmpty() )
    {
        return Ogre::any_cast<ClientObject*>( object );
    }

    return 0;
}

void ObjectSelection::doVolumeSelect()
{
    float left = mDragStart.x, right = mDragStop.x,
        top = mDragStart.y, bottom = mDragStop.y;

    if (left > right)
        swap(left, right);

    if (top > bottom)
        swap(top, bottom);

    if ((right - left) * (bottom - top) < 0.0001)
        return;

    // Convert screen positions into rays 
    Ogre::Ray topLeft = mCamera->getCameraToViewportRay(left, top);
    Ogre::Ray topRight = mCamera->getCameraToViewportRay(right, top);
    Ogre::Ray bottomLeft = mCamera->getCameraToViewportRay(left, bottom);
    Ogre::Ray bottomRight = mCamera->getCameraToViewportRay(right, bottom);

    // The plane faces the counter clockwise position.
    Ogre::PlaneBoundedVolume vol;
    vol.planes.push_back(Ogre::Plane(topLeft.getPoint(3), topRight.getPoint(3), bottomRight.getPoint(3)));
    vol.planes.push_back(Ogre::Plane(topLeft.getOrigin(), topLeft.getPoint(100), topRight.getPoint(100)));
    vol.planes.push_back(Ogre::Plane(topLeft.getOrigin(), bottomLeft.getPoint(100), topLeft.getPoint(100)));
    vol.planes.push_back(Ogre::Plane(bottomLeft.getOrigin(), bottomRight.getPoint(100), bottomLeft.getPoint(100)));
    vol.planes.push_back(Ogre::Plane(topRight.getOrigin(), topRight.getPoint(100), bottomRight.getPoint(100)));

    Ogre::PlaneBoundedVolumeList volList;
    volList.push_back(vol);

    mVolumeQuery->setVolumes(volList);
    mVolumeQuery->setQueryMask( mQueryMask );
    Ogre::SceneQueryResult result = mVolumeQuery->execute();

    for ( Ogre::SceneQueryResultMovableList::iterator i = result.movables.begin(); 
        i != result.movables.end(); ++i )
    {
        ClientObject* object = ObjectSelection::getObject( *i );
        if( object ) ObjectSelection::select( *object );
    }
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia