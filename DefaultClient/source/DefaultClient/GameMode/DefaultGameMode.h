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

#ifndef DIVERSIA_DEFAULTCLIENT_DEFAULTGAMEMODE_H
#define DIVERSIA_DEFAULTCLIENT_DEFAULTGAMEMODE_H

#include "DefaultClient/Platform/Prerequisites.h"

#include "OgreClient/GameMode/GameModeBase.h"
#include "OgreClient/Input/InputManager.h"

namespace Diversia
{
namespace DefaultClient
{
//------------------------------------------------------------------------------

class DefaultGameMode : public GameModeBase, public MouseListener, public KeyboardListener, 
    public sigc::trackable
{
public:
    DefaultGameMode( GameModePlugin& rGameModePlugin );
    ~DefaultGameMode();

    void create();
    void destroy();
    void reset();

    static GameModeBase* createGameMode( GameModePlugin* pPlugin );
    
private:
    bool mouseMoved( const MouseState& rState );
    bool mousePressed( const MouseButton button );
    void mouseReleased( const MouseButton button );
    bool keyPressed( const KeyboardButton button, unsigned int text );
    void keyReleased( const KeyboardButton button, unsigned int text );
    inline int getMousePriority() const { return 2; }
    inline int getKeyboardPriority() const { return 2; }

    void setServerState( ServerState serverState );
    void update( Real timeSinceLastFrame );
    void avatarDestroyed( Object& rObject );
    void componentChange( Component& rComponent, bool created );

    ClientObjectManager&    mObjectManager;
    sigc::connection        mAvatarDestroyedConnection;
    sigc::connection        mComponentDestroyedConnection;
    sigc::connection        mUpdateConnection;

    Object*     mAvatarOffset;
    Object*     mAvatar;
    Object*     mCameraYaw;
    Object*     mCameraPitch;
    Object*     mCamera;
    Animation*  mIdleAnimation;
    Animation*  mWalkAnimation;
    Animation*  mRunAnimation;
    Text*       mNickname;

    bool            mLMB, mRMB;
    MouseState      mMouseState;
    short           mMove;
    short           mRotation;
    Real            mSpeedMultiplier;

    Real            mSpeedModifier;
    Real            mRotationModifier;

};

//------------------------------------------------------------------------------
} // Namespace DefaultClient
} // Namespace Diversia

#endif // DIVERSIA_DEFAULTCLIENT_DEFAULTGAMEMODE_H