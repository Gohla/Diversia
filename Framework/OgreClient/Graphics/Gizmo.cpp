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

#include "OgreClient/Graphics/Gizmo.h"
#include "OgreClient/Input/InputManager.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

Gizmo::GizmoMouse* Gizmo::mMouse = 0;

Gizmo::Gizmo( ClientObject& rControlledObject ):
    mGizmoNode( GlobalsBase::mScene->createSceneNode() ),
    mControlledObject( rControlledObject )
{
    mGizmoNode->setInheritScale( false );
    mGizmoNode->setScale( 3.0, 3.0, 3.0 );

    if( !mMouse ) mMouse = new Gizmo::GizmoMouse();
}

Gizmo::~Gizmo()
{
    GlobalsBase::mScene->destroySceneNode( mGizmoNode );
}

//------------------------------------------------------------------------------

Gizmo::GizmoMouse::GizmoMouse():
    mCapture( false )
{
    GlobalsBase::mInput->subscribeMouse( *this );
}

bool Gizmo::GizmoMouse::mouseMoved( const MouseState& rState )
{
    mMouseState = rState;
    return mCapture;
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia