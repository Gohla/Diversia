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

#include "DefaultClient/Platform/StableHeaders.h"

#include "Client/ClientServerPlugin/ServerPluginManager.h"
#include "Client/Communication/GridManager.h"
#include "Client/Communication/Server.h"
#include "Client/Communication/ServerConnection.h"
#include "Client/Communication/ServerNeighborsPlugin.h"
#include "Client/Lua/LuaPlugin.h"
#include "Client/Object/ClientObjectManager.h"
#include "Client/Permission/PermissionManager.h"
#include "DefaultClient/ClientApplication.h"
#include "DefaultClient/GUI/GUIManager.h"
#include "DefaultClient/GUI/LoginGUI.h"
#include "DefaultClient/GameMode/DefaultGameMode.h"
#include "DefaultClient/Input/OISInputManager.h"
#include "DefaultClient/State/MenuState.h"
#include "OgreClient/Audio/AudioManager.h"
#include "OgreClient/GameMode/GameModePlugin.h"
#include "OgreClient/Graphics/CameraManager.h"
#include "OgreClient/Graphics/Fader.h"
#include "OgreClient/Graphics/GraphicsManager.h"
#include "OgreClient/Graphics/SkyPlugin.h"
#include "OgreClient/Graphics/Terrain.h"
#include "OgreClient/Object/Animation.h"
#include "OgreClient/Object/AreaTrigger.h"
#include "OgreClient/Object/Audio.h"
#include "OgreClient/Object/Camera.h"
#include "OgreClient/Object/CollisionShape.h"
#include "OgreClient/Object/Entity.h"
#include "OgreClient/Object/ForceField.h"
#include "OgreClient/Object/Light.h"
#include "OgreClient/Object/LuaObjectScript.h"
#include "OgreClient/Object/Mesh.h"
#include "OgreClient/Object/Particle.h"
#include "OgreClient/Object/RigidBody.h"
#include "OgreClient/Object/SceneNode.h"
#include "OgreClient/Object/Text.h"
#include "OgreClient/Physics/PhysicsManager.h"
#include "OgreClient/Resource/ResourceManager.h"
#include "Shared/ClientServerPlugin/ClientServerPluginManager.h"
#include "Shared/ClientServerPlugin/Factories/ObjectManagerFactory.h"
#include "Shared/ClientServerPlugin/Factories/TemplatePluginFactory.h"
#include "Shared/Communication/GridPosition.h"
#include "Shared/Communication/ServerInfo.h"
#include "Shared/Crash/CrashReporter.h"
#include "Shared/Object/TemplateComponentFactory.h"
#include "Util/Serialization/XMLSerializationFile.h"
#include "Util/State/StateMachine.h"

namespace Diversia
{
namespace DefaultClient 
{
//------------------------------------------------------------------------------

ClientApplication::ClientApplication():
    mShutdown( false ),
    mShutdownRequested( false ),
    mLogLevel( LOG_INFO )
{
    ClientGlobals::mApp = this;
    ClientGlobals::mEarlyUpdateSignal = &mEarlyUpdateSignal;
    ClientGlobals::mEarlyFrameSignal = &mEarlyFrameSignal;
    ClientGlobals::mUpdateSignal = &mUpdateSignal;
    ClientGlobals::mFrameSignal = &mFrameSignal;
    ClientGlobals::mLateUpdateSignal = &mLateUpdateSignal;
    ClientGlobals::mLateFrameSignal = &mLateFrameSignal;

    DelayedCall::setUpdateSignal( mUpdateSignal );
}

ClientApplication::~ClientApplication()
{
    ClientGlobals::mGrid = 0;
    ClientGlobals::mLateFrameSignal = 0;
    ClientGlobals::mLateUpdateSignal = 0;
    ClientGlobals::mFrameSignal = 0;
    ClientGlobals::mUpdateSignal = 0;
    ClientGlobals::mConfig = 0;
    ClientGlobals::mState = 0;
    ClientGlobals::mApp = 0;
}

void ClientApplication::init()
{
    // Init curl
    curl_global_init( CURL_GLOBAL_DEFAULT );

    // Load configuration
    mConfigManager.reset( new ConfigManager( new XMLSerializationFile( "config.xml" ) ) );
    ClientGlobals::mConfig = mConfigManager.get();
    mConfigManager->load();

    try
    {
        mConfigManager->registerObject( this );

        // Initialize logging
#ifdef DIVERSIA_DEBUG
        mLogger.reset( new Logger( mLogLevel ) );
#else
        mLogger.reset( new Logger( mLogLevel, false, true ) );
#endif

        // Initialize crash reporter
        CrashReporter* reporter = CrashReporter::createCrashReporter();
        mCrashReporter.reset( reporter );
        mCrashReporter->setEnabled( true );
        mCrashReporter->setSilent( false );
        mCrashReporter->setAppName( "Diversia_OgreEditor" );
        mCrashReporter->setAppVersion( "trunk" ); // TODO: Get revision number
        mConfigManager->registerObject( reporter );

        // Add component factories
        TemplateComponentFactory<SceneNode, ClientObject, false, false, true>::registerFactory();
        Object::addAutoCreateComponent<SceneNode>( "Node" );
        TemplateComponentFactory<Mesh, ClientObject, false>::registerFactory();
        TemplateComponentFactory<Entity, ClientObject, false>::registerFactory();
        TemplateComponentFactory<Light, ClientObject, false>::registerFactory();
        TemplateComponentFactory<Camera, ClientObject, false, true, true>::registerFactory();
        TemplateComponentFactory<Animation, ClientObject, true>::registerFactory();
        TemplateComponentFactory<Text, ClientObject, true>::registerFactory();
        TemplateComponentFactory<CollisionShape, ClientObject, false>::registerFactory();
        TemplateComponentFactory<RigidBody, ClientObject, false>::registerFactory();
        TemplateComponentFactory<AreaTrigger, ClientObject, false>::registerFactory();
        TemplateComponentFactory<ForceField, ClientObject, false>::registerFactory();
        TemplateComponentFactory<Audio, ClientObject, true>::registerFactory();
        TemplateComponentFactory<LuaObjectScript, ClientObject, true>::registerFactory();
        TemplateComponentFactory<Particle, ClientObject, true>::registerFactory();

        // Add plugin factories
        TemplatePluginFactory<PermissionManager, ServerPluginManager>::registerFactory();
        TemplatePluginFactory<ResourceManager, ServerPluginManager>::registerFactory();
        ObjectManagerFactory<ClientObjectManager, ServerPluginManager>::registerFactory( mUpdateSignal, mLateUpdateSignal );
        TemplatePluginFactory<ServerNeighborsPlugin, ServerPluginManager>::registerFactory();
        TemplatePluginFactory<SkyPlugin, ServerPluginManager>::registerFactory();
        TemplatePluginFactory<Terrain, ServerPluginManager>::registerFactory();
        TemplatePluginFactory<LuaPlugin, ServerPluginManager>::registerFactory();
        ClientServerPluginManager::addAutoCreatePlugin<LuaPlugin>();

        // Override the default game mode.
        TemplatePluginFactory<GameModePlugin, ServerPluginManager>::registerFactory();
        GameModePlugin::setDefaultSlot( sigc::ptr_fun( &DefaultGameMode::createGameMode ) );

        // Initialize graphics
        mGraphicsManager.reset( new GraphicsManager() );
        mConfigManager->registerObject( *mGraphicsManager );
        mGraphicsManager->init();
        mGraphicsManager->connectWindowClosing( sigc::mem_fun( this, &ClientApplication::quitSoon ) );

        // Initialize physics
        mPhysicsManager.reset( new PhysicsManager() );
        mConfigManager->registerObject( *mPhysicsManager );
        mPhysicsManager->init();

        // Initialize audio manager.
        mAudioManager.reset( new AudioManager() );
        mAudioManager->init();
        mConfigManager->registerObject( *mAudioManager );

        // Initialize camera manager
        mCameraManager.reset( new CameraManager() );

        // Initialize input
        mInputManager.reset( new OISInputManager() );

        // Initialize GUI
        mGUIManager.reset( new GUIManager() );
        mConfigManager->registerObject( LoginGUI::getSettings() );

        // Initialize grid manager
        mGridManager.reset( new GridManager( mUpdateSignal ) );
        mConfigManager->registerObject( mGridManager.get() );
        mConfigManager->registerObject( ServerConnection::getSettings() );
        mCameraManager->setGridManager( *mGridManager.get() );
        ClientGlobals::mGrid = mGridManager.get();

        // Push menu state to the state machine.
        mStateMachine.reset( new StateMachine() );
        ClientGlobals::mState = mStateMachine.get();
        mStateMachine->pushState( new MenuState() );
    }
    catch( Exception e )
    {
        // Save configuration
        mConfigManager->save();

        // Cleanup curl
        curl_global_cleanup();

        LOGC << e.what();

        throw e;
    }

    // Run the game loop
    run();

    // Save configuration
    mConfigManager->save();

    // Cleanup curl
    curl_global_cleanup();
}

void ClientApplication::run()
{
    Ogre::Root* root = mGraphicsManager->getRoot();
    Ogre::RenderWindow* window = mGraphicsManager->getWindow();

    root->getRenderSystem()->_initRenderTargets();
    root->clearEventTimes();

    boost::timer timer;

    while( !mShutdown )
    {
        if( mShutdownRequested )
        {
            mStateMachine->popTo( 0 );
            mShutdown = true;
        }

        const Real elapsed = timer.elapsed();
        timer.restart();

        mEarlyUpdateSignal();
        mEarlyFrameSignal( elapsed );
        mUpdateSignal();
        mFrameSignal( elapsed );
        mLateUpdateSignal();
        mLateFrameSignal( elapsed );

        Ogre::WindowEventUtilities::messagePump();

        // Render even when the window is inactive.
        if( window->isActive() )
        {
            root->renderOneFrame();
        }
        else if( window->isVisible() )
        {
            root->renderOneFrame();
            window->update();

#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32
            Sleep( ( 1.0 / 60.0 ) * 1000.0 );
#else
            usleep( ( 1.0 / 60.0 ) * 1000000.0 );
#endif
        }
        else if( !window->isActive() && !window->isVisible() )
        {
#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32
            Sleep( ( 1.0 / 60.0 ) * 1000.0 );
#else
            usleep( ( 1.0 / 60.0 ) * 1000000.0 );
#endif
        }
    }
}

void ClientApplication::quitSoon()
{
    ClientGlobals::mGraphics->getFader()->startFadeOut();
    DelayedCall::create( sigc::mem_fun( this, &ClientApplication::quit ), 1.1 );
}

//------------------------------------------------------------------------------
} // Namespace DefaultClient
} // Namespace Diversia