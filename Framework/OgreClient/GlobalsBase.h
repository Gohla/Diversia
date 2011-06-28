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

#ifndef DIVERSIA_OGRECLIENT_GLOBALSBASE_H
#define DIVERSIA_OGRECLIENT_GLOBALSBASE_H

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

class /*DIVERSIA_OGRECLIENT_API*/ GlobalsBase /*: public boost::noncopyable*/
{
public:
    static ApplicationBase*         mApp;
    static StateMachine*            mState;
    static ConfigManager*           mConfig;
    static GraphicsManager*         mGraphics;
    static Ogre::SceneManager*      mScene;
    static CameraManager*           mCamera;
    static GridManager*             mGrid;
    static TextCanvasBatch*         mText;
    static PhysicsManager*          mPhysics;
    static btDiscreteDynamicsWorld* mWorld;
    static btAxisSweep3*            mBroadphase;
    static AudioManager*            mAudio;
    static InputManager*            mInput;
    static ObjectSelection*         mSelection;
    static UndoStack*               mUndoStack;
    static bool                     mOffline;

    static sigc::signal<void>*          mEarlyUpdateSignal;
    static sigc::signal<void, Real>*    mEarlyFrameSignal;
    static sigc::signal<void>*          mUpdateSignal;
    static sigc::signal<void, Real>*    mFrameSignal;
    static sigc::signal<void>*          mLateUpdateSignal;
    static sigc::signal<void, Real>*    mLateFrameSignal;

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

#endif // DIVERSIA_OGRECLIENT_GLOBALSBASE_H