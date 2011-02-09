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

#include "OgreClient/GlobalsBase.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

ApplicationBase*            GlobalsBase::mApp = 0;
StateMachine*               GlobalsBase::mState = 0;
ConfigManager*              GlobalsBase::mConfig = 0;
GraphicsManager*            GlobalsBase::mGraphics = 0;
Ogre::SceneManager*         GlobalsBase::mScene = 0;
CameraManager*              GlobalsBase::mCamera = 0;
GridManager*                GlobalsBase::mGrid = 0;
TextCanvasBatch*            GlobalsBase::mText = 0;
PhysicsManager*             GlobalsBase::mPhysics = 0;
btDiscreteDynamicsWorld*    GlobalsBase::mWorld = 0;
btAxisSweep3*               GlobalsBase::mBroadphase = 0;
AudioManager*               GlobalsBase::mAudio = 0;
InputManager*               GlobalsBase::mInput = 0;
ObjectSelection*            GlobalsBase::mSelection = 0;
bool                        GlobalsBase::mOffline = false;

sigc::signal<void>*         GlobalsBase::mEarlyUpdateSignal = 0;
sigc::signal<void, Real>*   GlobalsBase::mEarlyFrameSignal = 0;
sigc::signal<void>*         GlobalsBase::mUpdateSignal = 0;
sigc::signal<void, Real>*   GlobalsBase::mFrameSignal = 0;
sigc::signal<void>*         GlobalsBase::mLateUpdateSignal = 0;
sigc::signal<void, Real>*   GlobalsBase::mLateFrameSignal = 0;
//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia