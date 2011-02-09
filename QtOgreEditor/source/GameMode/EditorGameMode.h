/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_EDITORGAMEMODE_H
#define DIVERSIA_QTOGREEDITOR_EDITORGAMEMODE_H

#include "Platform/Prerequisites.h"

#include "OgreClient/GameMode/GameModePlugin.h"
#include "OgreClient/GameMode/GameModeBase.h"
#include "OgreClient/Input/InputManager.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

class EditorGameMode : public GameModeBase, public MouseListener, public KeyboardListener, 
    public sigc::trackable
{
public:
    EditorGameMode( GameModePlugin& rGameModePlugin );
    ~EditorGameMode();

	static GameModeBase* createGameMode( GameModePlugin* pPlugin );
    
private:
    friend class MainWindow;

    void create();
    void destroy();
    void reset();
    void setServerState( ServerState serverState );

    bool mouseMoved( const MouseState& rState );
    bool mousePressed( const MouseButton button );
    void mouseReleased( const MouseButton button );
    bool keyPressed( const KeyboardButton button, unsigned int text );
    void keyReleased( const KeyboardButton button, unsigned int text );
    inline int getMousePriority() const { return 1; }
    inline int getKeyboardPriority() const { return 1; }

    void update( Real timeSinceLastFrame );
    void cameraDestroyed( Object& rObject );
    void componentChange( Component& rComponent, bool created );

    ClientObjectManager&    mObjectManager;
    sigc::connection        mCameraPitchDestroyedConnection;
    sigc::connection        mCameraYawDestroyedConnection;
    sigc::connection        mComponentDestroyedConnection;
    sigc::connection        mUpdateConnection;
    unsigned char           mDestroyNextUpdate;

    Object*                 mCameraPitch;
    Object*                 mCameraYaw;
    bool                    mRMB;
    MouseState              mMouseState;
    short                   mMove;
    short                   mRotation;
    Real                    mSpeedMultiplier;
    Real                    mSpeedModifier;
    Real                    mRotationModifier;

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_EDITORGAMEMODE_H