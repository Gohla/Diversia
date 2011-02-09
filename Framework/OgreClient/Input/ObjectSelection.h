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

#ifndef DIVERSIA_OGRECLIENT_OBJECTSELECTION_H
#define DIVERSIA_OGRECLIENT_OBJECTSELECTION_H

#include "OgreClient/Platform/Prerequisites.h"

#include "OgreClient/Input/InputManager.h"
#include "OgreClient/Graphics/QueryFlags.h"

#include <MOC/CollisionTools.h>

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API SelectionRectangle : public Ogre::ManualObject
{
public:
    /**
    Constructor. 
    
    @param  rName   Name of the manual object.
    **/
    SelectionRectangle( const String& rName );

    /**
    Sets the corners of the SelectionRectangle. Every parameter should be in the range [0, 1]
    representing a percentage of the screen the SelectionRectangle should take up. 
    
    @param  left    The left position. 
    @param  top     The top position.
    @param  right   The right position. 
    @param  bottom  The bottom position. 
    **/
    void setCorners( float left, float top, float right, float bottom );
    /**
    Sets the corners of the SelectionRectangle. Every parameter should be in the range [0, 1]
    representing a percentage of the screen the SelectionRectangle should take up. 
    
    @param  topLeft     The top left position
    @param  bottomRight The bottom right position.
    **/
    inline void setCorners( const Ogre::Vector2& rTopLeft, const Ogre::Vector2& rBottomRight )
    {
        SelectionRectangle::setCorners( rTopLeft.x, rTopLeft.y, rBottomRight.x, rBottomRight.y );
    }
};

//------------------------------------------------------------------------------

typedef std::set<ClientObject*> SelectedObjects;
typedef std::map<ClientObject*, sigc::connection> ObjectConnections;

/**
This class provides events for mouse interaction on objects such as hovering, click and dragging an 
object.
**/
class DIVERSIA_OGRECLIENT_API ObjectSelection : public MouseListener, public sigc::trackable
{
public:
    /**
    Default constructor. 
    
    @param  queryMask   The query mask to use when doing (volume) selections.
    **/
    ObjectSelection( unsigned int queryMask = QueryFlags_Entity );
    /**
    Destructor. 
    **/
    ~ObjectSelection();

    /**
    Gets the selected objects. 
    **/
    inline const SelectedObjects& getSelectedObjects() const { return mSelectedObjects; }
    /**
    Gets the query mask that is used when doing (volume) selections.
    **/
    inline unsigned int getQueryMask() const { return mQueryMask; }
    /**
    Sets the query mask to use when doing (volume) selections.
    **/
    inline void setQueryMask( unsigned int queryMask ) { mQueryMask = queryMask; }
    /**
    Adds an object to the selection. 
    
    @param [in,out] rObject The object to add to the selection.
    @param  silent          True to silently add, don't emit signal. Defaults to false.
    **/
    void select( ClientObject& rObject, bool silent = false );
    /**
    Removes an object from the selection.

    @param [in,out] rObject The object remove from the selection.
    @param  silent          True to silently remove, don't emit signal. Defaults to false.
    **/
    void deselect( ClientObject& rObject, bool silent = false );
    /**
    Toggles selection for an object.

    @param [in,out] rObject The object to toggle the selection for
    @param  silent          True to silently toggle, don't emit signal. Defaults to false.
    **/
    void toggleSelection( ClientObject& rObject, bool silent = false );
    /**
    Crops the selection to given object. All objects will be deselected except the given object, 
    if given object has not been selected yet it will be selected.
    
    @param [in,out] rObject The object
    @param  silent          True to silently crop, don't emit signals. Defaults to false.
    **/
    void cropSelection( ClientObject& rObject, bool silent = false );
    /**
    Deselects all objects.

    @param  silent          True to silently crop, don't emit signals. Defaults to false.
    **/
    void deselectAll( bool silent = false );

    /**
    Connects a slot to the object hovered/unhovered signal.

    @param [in,out] rSlot   The slot (signature: void func(Object&, bool [true if entered hover,
                            false if leaving hover])) to connect. 
    
    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connectHover( const sigc::slot<void, ClientObject&, bool>& rSlot ) 
    {
        return mHoverSignal.connect( rSlot );
    }
    /**
    Connects a slot to the object clicked signal.

    @param [in,out] rSlot   The slot (signature: void func(Object&)) to connect. 
    
    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connectClick( const sigc::slot<void, ClientObject&>& rSlot ) 
    {
        return mClickSignal.connect( rSlot );
    }
    /**
    Connects a slot to the object selected/deselected signal.

    @param [in,out] rSlot   The slot (signature: void func(Object&, bool [true if selected, false 
                            if unselected])) to connect. 
    
    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connectSelected( const sigc::slot<void, ClientObject&, bool>& rSlot ) 
    {
        return mSelectedSignal.connect( rSlot );
    }
    /**
    Connects a slot to the object drag signal.

    @param [in,out] rSlot   The slot (signature: void func(Object&, bool [true if starting to drag,
                            false if stopping to drag])) to connect. 
    
    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connectDrag( const sigc::slot<void, ClientObject&, bool>& rSlot ) 
    {
        return mDragSignal.connect( rSlot );
    }

private:
    bool mouseMoved( const MouseState& rState );
    bool mousePressed( const MouseButton button );
    void mouseReleased( const MouseButton button );
    inline int getMousePriority() const { return 2; }

    void update();
    void objectDestroyed( Object& rObject );
    void cameraChange( Ogre::Camera* pCamera );

    ClientObject* getObject( Ogre::MovableObject* pMovableObject );
    void doVolumeSelect();

    inline static void swap(float &x, float &y)
    {
        float tmp = x;
        x = y;
        y = tmp;
    }
    
    sigc::signal<void, ClientObject&, bool> mHoverSignal;
    sigc::signal<void, ClientObject&>       mClickSignal;
    sigc::signal<void, ClientObject&, bool> mSelectedSignal;
    sigc::signal<void, ClientObject&, bool> mDragSignal;

    Ogre::Camera*                           mCamera;
    MouseState                              mMouseState;
    MOC::CollisionTools*                    mCollisionTools;

    Ogre::Timer                             mClickTimer;
    bool                                    mClick;
    bool                                    mDragClick;
    bool                                    mDragging;
    Ogre::Vector2                           mDragStart;
    Ogre::Vector2                           mDragStop;
    Ogre::PlaneBoundedVolumeListSceneQuery* mVolumeQuery;
    SelectionRectangle*                     mRectangle;
    bool                                    mDoVolumeQuery;
    unsigned int                            mQueryMask;

    ClientObject*                           mObjectUnderMouse;
    ClientObject*                           mDraggingObject;
    SelectedObjects                         mSelectedObjects;
    ObjectConnections                       mObjectConnections;

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

#endif // DIVERSIA_OGRECLIENT_OBJECTSELECTION_H