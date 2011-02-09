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

#include "Platform/StableHeaders.h"

#include "Application.h"
#include "ClientServerPlugin/ClientPlugin.h"
#include "ClientServerPlugin/ClientPluginManager.h"
#include "ClientServerPlugin/ResourceManagerPlugin.h"
#include "ClientServerPlugin/SkyPlugin.h"
#include "ClientServerPlugin/Terrain.h"
#include "Communication/ClientConnection.h"
#include "Communication/ServerNeighborsPlugin.h"
#include "GameMode/GameModePlugin.h"
#include "Object/Animation.h"
#include "Object/Audio.h"
#include "Object/CollisionShape.h"
#include "Object/Entity.h"
#include "Object/LuaObjectScript.h"
#include "Object/Mesh.h"
#include "Object/Particle.h"
#include "Object/RigidBody.h"
#include "Object/ServerObject.h"
#include "Object/ServerObjectManager.h"
#include "Object/ServerObjectManager.h"
#include "Object/Text.h"
#include "Permission/PermissionManager.h"
#include "Physics/PhysicsManager.h"
#include "Resource/LocalResourceManager.h"
#include "Shared/ClientServerPlugin/Factories/ObjectManagerFactory.h"
#include "Shared/ClientServerPlugin/Factories/TemplatePluginFactory.h"
#include "Shared/Crash/CrashReporter.h"
#include "Shared/Lua/LuaManager.h"
#include "Shared/Object/TemplateComponentFactory.h"
#include "Util/Helper/ConsoleInput.h"
#include "Util/Serialization/XMLSerializationFile.h"

namespace Diversia
{
namespace Server 
{
//------------------------------------------------------------------------------

Application::Application():
    mShutdown( false ),
    mLogLevel( LOG_INFO ),
    mSleepMS( 10 )
{
    Globals::mApp = this;
    Globals::mUpdateSignal = &mUpdateSignal;
    Globals::mFrameSignal = &mFrameSignal;

    DelayedCall::setUpdateSignal( mUpdateSignal );
}

Application::~Application()
{
    Globals::mFrameSignal = 0;
    Globals::mUpdateSignal = 0;
    Globals::mConfig = 0;
    Globals::mApp = 0;
}

void Application::init()
{
    // Load configuration
    mConfigManager.reset( new ConfigManager( new XMLSerializationFile( "config.xml" ) ) );
    Globals::mConfig = mConfigManager.get();
    mConfigManager->load();

    try
    {
        mConfigManager->registerObject( this );

        // Initialize logging
        mLogger.reset( new Logger( mLogLevel ) );

        // Initialize crash reporter
        CrashReporter* reporter = CrashReporter::createCrashReporter();
        mCrashReporter.reset( reporter );
        mCrashReporter->setEnabled( true );
        mCrashReporter->setSilent( true );
        mCrashReporter->setAppName( "Diversia_Server" );
        mCrashReporter->setAppVersion( "trunk" ); // TODO: Get revision number
        mConfigManager->registerObject( reporter );

        // Add component factories
        TemplateComponentFactory<Mesh, ServerObject, false>::registerFactory();
        TemplateComponentFactory<Entity, ServerObject, false>::registerFactory();
        TemplateComponentFactory<Animation, ServerObject, true>::registerFactory();
        TemplateComponentFactory<Text, ServerObject, true>::registerFactory();
        TemplateComponentFactory<CollisionShape, ServerObject, false>::registerFactory();
        TemplateComponentFactory<RigidBody, ServerObject, false>::registerFactory();
        TemplateComponentFactory<Audio, ServerObject, true>::registerFactory();
        TemplateComponentFactory<LuaObjectScript, ServerObject, true>::registerFactory();
        TemplateComponentFactory<Particle, ServerObject, true>::registerFactory();

        // Add plugin factories
        TemplatePluginFactory<PermissionManager, ClientPluginManager>::registerFactory();
        TemplatePluginFactory<ResourceManagerPlugin, ClientPluginManager>::registerFactory();
        ObjectManagerFactory<ServerObjectManager, ClientPluginManager>::registerFactory( mUpdateSignal );
        TemplatePluginFactory<ServerNeighborsPlugin, ClientPluginManager>::registerFactory();
        TemplatePluginFactory<SkyPlugin, ClientPluginManager>::registerFactory();
        TemplatePluginFactory<GameModePlugin, ClientPluginManager>::registerFactory();
        TemplatePluginFactory<Terrain, ClientPluginManager>::registerFactory();

        // Initialize scripting
        mLuaManager.reset( new LuaManager() );
        Globals::mLua = mLuaManager.get();
        mLuaManager->object( "Application" ) = this;

        // Initialize console input
        ConsoleInput::setCommandCallback( 
            sigc::bind( sigc::mem_fun( mLuaManager.get(), &LuaManager::execute ), "Global", "", LUASEC_LOW ) );
        ConsoleInput::setConsoleClosedCallback( sigc::mem_fun( this, &Application::quit ) );
        ConsoleInput::setUpdateSignal( mUpdateSignal );

        // Initialize resource manager
        mResourceManager.reset( new LocalResourceManager() );
        mConfigManager->registerObject( *mResourceManager );

        // Setup scripting
        mLuaManager->setRootResourceLocation( mResourceManager->getRootResourceLocation() );
        Globals::mResource->connect( ".glua", sigc::bind( sigc::mem_fun( mLuaManager.get(), 
            &LuaManager::executeFile ), "Global", "", LUASEC_LOW, false ) );

        // Initialize physics
        mPhysicsManager.reset( new PhysicsManager() );
        mConfigManager->registerObject( *mPhysicsManager );
        mPhysicsManager->init();

        // Initialize client connection
        mConfigManager->registerObject( ClientConnection::getSettings() );
        mClientConnection.reset( new ClientConnection( mUpdateSignal ) );
        mClientConnection->listen();

        // Randomize
        srand( time( NULL ) );

        // Initialize resources
        mResourceManager->init();
    }
    catch( Exception e )
    {
        // Save configuration
        mConfigManager->save();

        LOGC << e.what();

        throw e;
    }

    // Run the game loop
    run();

    // Save configuration
    mConfigManager->save();
}

void Application::run()
{
    boost::timer timer;

    while( !mShutdown )
    {
        const Real elapsed = timer.elapsed();
        timer.restart();

        // Fire update signals.
        mUpdateSignal();
        mFrameSignal( elapsed );

#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32
        Sleep( mSleepMS );
#else
        usleep( mSleepMS * 1000.0 );
#endif
    }
}

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia