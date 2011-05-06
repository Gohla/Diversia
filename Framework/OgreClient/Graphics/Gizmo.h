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
    
    @param [in,out] rControlledObject   The object this gizmo should control. 
    **/
    Gizmo( ClientObject& rControlledObject );
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
    inline ClientObject& getControlledObject() const { return mControlledObject; }
   
protected:
    enum Axis 
    {
        X_AXIS,
        Y_AXIS,
        Z_AXIS
    };

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
    Ogre::SceneNode*    mGizmoNode;
    ClientObject&       mControlledObject;

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::Gizmo, 
    &Diversia::OgreClient::Bindings::CampBindings::bindGizmo );

#endif // DIVERSIA_OGRECLIENT_GIZMO_H