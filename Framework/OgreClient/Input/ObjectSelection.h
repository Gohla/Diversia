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

typedef sigc::slot<void> ClickSlot;
typedef sigc::slot<void, bool, int> HoverSlot;
typedef sigc::slot<void, bool> SelectSlot;
typedef sigc::signal<void, const camp::UserObject&, bool> SelectSignal;
typedef sigc::slot<void, bool, int> DragSlot;

typedef std::map<camp::UserObject, ClickSlot> ClickSlotMap;
typedef std::map<camp::UserObject, HoverSlot> HoverSlotMap;
typedef std::map<camp::UserObject, SelectSlot> SelectSlotMap;
typedef std::map<camp::UserObject, DragSlot> DragSlotMap;

typedef boost::tuple<camp::UserObject*, int> ParamsTuple;

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

typedef std::set<camp::UserObject> SelectedObjects;

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
    void select( const camp::UserObject& rObject, bool silent = false );
    /**
    Removes an object from the selection.

    @param [in,out] rObject The object remove from the selection.
    @param  silent          True to silently remove, don't emit signal. Defaults to false.
    **/
    void deselect( const camp::UserObject& rObject, bool silent = false );
    /**
    Query if given object is selected. 
    
    @param [in,out] rObject The object to check selection for.
    
    @return True if selected, false if not. 
    **/
    bool isSelected( const camp::UserObject& rObject );
    /**
    Toggles selection for an object.

    @param [in,out] rObject The object to toggle the selection for
    @param  silent          True to silently toggle, don't emit signal. Defaults to false.
    **/
    void toggleSelection( const camp::UserObject& rObject, bool silent = false );
    /**
    Crops the selection to given object. All objects will be deselected except the given object, 
    if given object has not been selected yet it will be selected.
    
    @param [in,out] rObject The object
    @param  silent          True to silently crop, don't emit signals. Defaults to false.
    **/
    void cropSelection( const camp::UserObject& rObject, bool silent = false );
    /**
    Deselects all objects.

    @param  silent          True to silently crop, don't emit signals. Defaults to false.
    **/
    void deselectAll( bool silent = false );
    /**
    Sets a slot to the object clicked signal.
    
    @param  rObject The object to set the slot for.
    @param  rSlot   The slot (signature: void func()) to set. 
    **/
    inline void setClickSlot( const camp::UserObject& rObject, const ClickSlot& rSlot ) 
    {
        mClickSlots[rObject] = rSlot;
    }
    /**
    Removes the a slot from the clicked signal.
    
    @param  rObject The object to remove the slot for.
    **/
    inline void removeClickSlot( const camp::UserObject& rObject ) { mClickSlots.erase( rObject ); }
    /**
    Sets a slot to the object hovered/unhovered signal. 
    
    @param  rObject The object to set the slot for.
    @param  rSlot   The slot (signature: void func(bool [true if entered hover, false if leaving
                    hover])) to set. 
    **/
    inline void setHoverSlot( const camp::UserObject& rObject, const HoverSlot& rSlot ) 
    {
        mHoverSlots[rObject] = rSlot;
    }
    /**
    Removes the a slot from the hover signal.
    
    @param  rObject The object to remove the slot for.
    **/
    inline void removeHoverSlot( const camp::UserObject& rObject ) { mHoverSlots.erase( rObject ); }
    /**
    Sets a slot to the object selected/deselected signal.
    
    @param  rObject The object to set the slot for. 
    @param  rSlot   The slot (signature: void func(bool [true if selected, false if unselected]))
                    to connect. 
    **/
    inline void setSelectSlot( const camp::UserObject& rObject, const SelectSlot& rSlot, bool ignoreSilent = false ) 
    {
        if( ignoreSilent ) mSelectIgnoreSilentSlots[rObject] = rSlot; else mSelectSlots[rObject] = rSlot;
    }
    /**
    Removes the a slot from the select signal.
    
    @param  rObject The object to remove the slot for.
    **/
    inline void removeSelectSlot( const camp::UserObject& rObject ) { mSelectSlots.erase( rObject ); mSelectIgnoreSilentSlots.erase( rObject ); }
    /**
    Connects a slot to the object selected/deselected signal.

    @param [in,out] rSlot   The slot (signature: void func(camp::UserObject&, bool [true if 
                            selected, false if unselected])) to connect. 
    
    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connectSelected( const SelectSignal::slot_type& rSlot ) 
    {
        return mSelectSignal.connect( rSlot );
    }
    /**
    Sets a slot to the object drag signal.
    
    @param  rObject The object to set the slot for.
    @param  rSlot   The slot (signature: void func(bool [true if starting to drag, false if
                    stopping to drag])) to connect. 
    **/
    inline void setDragSlot( const camp::UserObject& rObject, const DragSlot& rSlot ) 
    {
        mDragSlots[rObject] = rSlot;
    }
    /**
    Removes the a slot from the drag signal.
    
    @param  rObject The object to remove the slot for.
    **/
    inline void removeDragSlot( const camp::UserObject& rObject ) { mDragSlots.erase( rObject ); }

private:
    bool mouseMoved( const MouseState& rState );
    bool mousePressed( const MouseButton button );
    void mouseReleased( const MouseButton button );
    inline int getMousePriority() const { return 2; }

    void update();
    void cameraChange( Ogre::Camera* pCamera );

    ParamsTuple getParams( Ogre::MovableObject* pMovableObject );
    void doVolumeSelect();

    inline static void swap(float &x, float &y)
    {
        float tmp = x;
        x = y;
        y = tmp;
    }

    inline void fireClick( const camp::UserObject& rObject )
    {
        ClickSlotMap::iterator i = mClickSlots.find( rObject );
        if( i != mClickSlots.end() ) i->second();
    }
    inline void fireHover( const camp::UserObject& rObject, bool hover, int param )
    {
        HoverSlotMap::iterator i = mHoverSlots.find( rObject );
        if( i != mHoverSlots.end() ) i->second( hover, param );
    }
    inline void fireSelect( const camp::UserObject& rObject, bool selected, bool silent )
    {
        SelectSlotMap::iterator i = mSelectIgnoreSilentSlots.find( rObject );
        if( i != mSelectIgnoreSilentSlots.end() ) i->second( selected );

        if( silent ) return;

        mSelectSignal( rObject, selected );
        i = mSelectSlots.find( rObject );
        if( i != mSelectSlots.end() ) i->second( selected );
    }
    inline void fireDrag( const camp::UserObject& rObject, bool dragStart, int param )
    {
        DragSlotMap::iterator i = mDragSlots.find( rObject );
        if( i != mDragSlots.end() ) i->second( dragStart, param );
    }

    ClickSlotMap    mClickSlots;
    HoverSlotMap    mHoverSlots;
    SelectSlotMap   mSelectSlots;
    SelectSlotMap   mSelectIgnoreSilentSlots;
    SelectSignal    mSelectSignal;
    DragSlotMap     mDragSlots;

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

    camp::UserObject*                       mObjectUnderMouse;
    int                                     mParamUnderMouse;
    camp::UserObject*                       mDraggingObject;
    int                                     mDraggingParam;
    SelectedObjects                         mSelectedObjects;

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

#endif // DIVERSIA_OGRECLIENT_OBJECTSELECTION_H