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

#ifndef DIVERSIA_OGRECLIENT_GIZMO_H
#define DIVERSIA_OGRECLIENT_GIZMO_H

#include "OgreClient/Platform/Prerequisites.h"
#include "OgreClient/Input/InputManager.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API Gizmo
{
public:
    /**
    Constructor. 
    
    @param [in,out] pControlledObject   The object this gizmo should control or 0 to control no 
                                        object. Defaults to 0.
    **/
    Gizmo( ClientObject* pControlledObject = 0 );
    /**
    Destructor. 
    **/
    virtual ~Gizmo();

    /**
    Gets the gizmo scene node. 
    **/
    inline Ogre::SceneNode* getSceneNode() const { return mGizmoNode; }
    /**
    Gets the gizmo scene node. 
    **/
    inline ClientObject* getControlledObject() const { return mControlledObject; }
    /**
    Sets the controlled gizmo's.
    
    @param rGizmos  Reference to controlled gizmo's. 
    **/
    inline void setControlledGizmos( const std::set<Gizmo*>& rGizmos ) { mControlledGizmos = &rGizmos; }
    /**
    Gets the controlled gizmo's. 
    
    @return The controlled gizmo's or 0 if there are no controlled gizmo's.
    **/
    inline const std::set<Gizmo*>* getControlledGizmos() const { return mControlledGizmos; }
    /**
    Sets if the gizmo should snap to grid.
    
    @param  snap    True to snap to grid.
    **/
    inline void setSnapToGrid( bool snap ) { mSnapToGrid = snap; }
    /**
    Query if this object is snapping to grid. 
    
    @return True if snapping to grid, false if not.
    **/
    inline bool isSnappingToGrid() const { return mSnapToGrid; }
    /**
    Sets gizmo visibility.
    
    @param  visible True to show, false to hide. 
    **/
    virtual void setVisible( bool visible ) = 0;
    /**
    Call this if the transform is changed and the gizmo needs to update this.
    **/
    inline virtual void transformChanged() {}
   
protected:
    enum Axis 
    {
        X_AXIS,
        Y_AXIS,
        Z_AXIS,
        NO_AXIS
    };
    /**
    Starts or stops controlling the controlled gizmo's (if any). 
    
    @param  control     True to control, false to stop controlling. 
    @param  param       The drag parameter (axis)
    @param  rPosition   The drag start/stop position. 
    @param  duplicate   True to duplicate drag, false to normal drag.
    **/
    bool controlGizmos( bool control, int param = 0, const Vector3& rPosition = Vector3::ZERO, 
        bool duplicate = false );
    /**
    Called if this gizmo should be controlled or stop being controlled.
    
    @param  control     True to control, false to stop controlling. 
    @param  param       The drag parameter (axis)
    @param  rPosition   The drag start/stop position. 
    @param  duplicate   True to duplicate drag, false to normal drag.
    **/
    virtual void controlGizmo( bool control, int param, const Vector3& rPosition, bool duplicate, 
        Gizmo* pController ) = 0;
    /**
    Controlled gizmo update.
    **/
    virtual void controlUpdate() = 0;

    struct GizmoMouse : public MouseListener
    {
        GizmoMouse();

        inline int getMousePriority() const { return -2; }
        bool mouseMoved( const MouseState& rState );

        bool mCapture;
        MouseState mMouseState;
    };

    static GizmoMouse* mMouse;

private:
    void update();

    Ogre::SceneNode*        mGizmoNode;
    ClientObject*           mControlledObject;
    const std::set<Gizmo*>* mControlledGizmos;
    sigc::connection        mUpdateConnection;
    bool                    mSnapToGrid;

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::Gizmo, 
    &Diversia::OgreClient::Bindings::CampBindings::bindGizmo );

#endif // DIVERSIA_OGRECLIENT_GIZMO_H