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

#ifndef DIVERSIA_SERVER_APPLICATION_H
#define DIVERSIA_SERVER_APPLICATION_H

#include "Platform/Prerequisites.h"

namespace Diversia
{
namespace Server
{
//------------------------------------------------------------------------------

class Application
{
public:
    Application();
    ~Application();

    void init();
    void run();
    inline void quit() { mShutdown = true; }
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    sigc::signal<void>                  mUpdateSignal;
    sigc::signal<void, Real>            mFrameSignal;
    bool                                mShutdown;

    boost::scoped_ptr<Logger>               mLogger;
    boost::scoped_ptr<ConfigManager>        mConfigManager;
    boost::scoped_ptr<CrashReporter>        mCrashReporter;
    boost::scoped_ptr<LocalResourceManager> mResourceManager;
    boost::scoped_ptr<LuaManager>           mLuaManager;
    boost::scoped_ptr<PhysicsManager>       mPhysicsManager;
    boost::scoped_ptr<ClientConnection>     mClientConnection;

    // Settings
    LogLevel        mLogLevel;
    unsigned int    mSleepMS;

};

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Server::Application, 
    &Diversia::Server::Bindings::CampBindings::bindApplication );

#endif // DIVERSIA_SERVER_APPLICATION_H