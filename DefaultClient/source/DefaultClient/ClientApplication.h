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

#ifndef DIVERSIA_DEFAULTCLIENT_CLIENTAPPLICATION_H
#define DIVERSIA_DEFAULTCLIENT_CLIENTAPPLICATION_H

#include "DefaultClient/Platform/Prerequisites.h"

#include "OgreClient/ApplicationBase.h"

namespace Diversia
{
namespace DefaultClient
{
//------------------------------------------------------------------------------

class ClientApplication : public ApplicationBase
{
public:
    ClientApplication();
    virtual ~ClientApplication();

    void init( int argc, char* argv[] );
    void run();
    void quitSoon();
    inline void quit() { mShutdownRequested = true; }
    inline void stopUpdates( bool stop = true ) { }
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    void exit();

    boost::scoped_ptr<Logger>           mLogger;
    boost::scoped_ptr<ConfigManager>    mConfigManager;
    boost::scoped_ptr<CrashReporter>    mCrashReporter;
    boost::scoped_ptr<GraphicsManager>  mGraphicsManager;
    boost::scoped_ptr<PhysicsManager>   mPhysicsManager;
    boost::scoped_ptr<AudioManager>     mAudioManager;
    boost::scoped_ptr<CameraManager>    mCameraManager;
    boost::scoped_ptr<InputManager>     mInputManager;
    boost::scoped_ptr<GUIManager>       mGUIManager;
    boost::scoped_ptr<GridManager>      mGridManager;
    boost::scoped_ptr<StateMachine>     mStateMachine;

    sigc::signal<void>          mEarlyUpdateSignal;
    sigc::signal<void, Real>    mEarlyFrameSignal;
    sigc::signal<void>          mUpdateSignal;
    sigc::signal<void, Real>    mFrameSignal;
    sigc::signal<void>          mLateUpdateSignal;
    sigc::signal<void, Real>    mLateFrameSignal;
    bool                        mShutdown;
    bool                        mShutdownRequested;

    // Options
    Path mOfflineFile;

    // Settings
    LogLevel mLogLevel;

};

//------------------------------------------------------------------------------
} // Namespace DefaultClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::DefaultClient::ClientApplication, 
    &Diversia::DefaultClient::Bindings::CampBindings::bindClientApplication );

#endif // DIVERSIA_DEFAULTCLIENT_CLIENTAPPLICATION_H