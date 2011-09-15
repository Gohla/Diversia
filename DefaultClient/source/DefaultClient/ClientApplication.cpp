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

#include "Client/Communication/GridManager.h"
#include "Client/Communication/Server.h"
#include "Client/Communication/ServerAbstract.h"
#include "Client/Communication/ServerConnection.h"
#include "Client/Communication/ServerNeighborsPlugin.h"
#include "Client/Lua/LuaPlugin.h"
#include "Client/Object/ClientObjectManager.h"
#include "Client/Object/ClientObjectTemplateManager.h"
#include "Client/Permission/PermissionManager.h"
#include "Client/Plugin/ClientPluginManager.h"
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
#include "OgreClient/Graphics/SceneManagerPlugin.h"
#include "OgreClient/Graphics/SkyPlugin.h"
#include "OgreClient/Graphics/Terrain.h"
#include "OgreClient/Level/LevelManager.h"
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
#include "Shared/Communication/GridPosition.h"
#include "Shared/Communication/ServerInfo.h"
#include "Shared/Crash/CrashReporter.h"
#include "Shared/Object/TemplateComponentFactory.h"
#include "Shared/Plugin/Factories/ObjectManagerFactory.h"
#include "Shared/Plugin/Factories/TemplatePluginFactory.h"
#include "Shared/Plugin/PluginManager.h"
#include "State/LoadingState.h"
#include "Util/Config/ConfigManager.h"
#include "Util/Serialization/XMLSerializationFile.h"
#include "Util/State/StateMachine.h"
#include <boost/program_options.hpp>

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

void ClientApplication::init( int argc, char* argv[] )
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
#ifdef DIVERSIA_DEBUGINFO
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

        // Init program options
        using namespace boost::program_options;
        options_description desc( "Options" );
        desc.add_options()
            ( "help", "Shows help" )
            ( "offline,o", bool_switch( &GlobalsBase::mOffline ), "Start client in offline mode." )
            ( "media,m", value( &mAddMedia ), "Adds given path as a resource location." )
            ( "offline-file,f", value( &mOfflineFile ), "Offline plugin manager xml file to load." )
            ( "offline-script,s", value( &mOfflineScript ), "Offline lua script to load." )
        ;

        variables_map vm;        
        store( command_line_parser( argc, argv ).options( desc ).run(), vm );
        notify( vm );    

        if( vm.count( "help" ) ) {
            LOGI << "Usage: " << Path( argv[0] ).filename() << " [options]";
            LOGI << desc;
            ClientApplication::exit();
            return;
        }

        // Add component factories, get camp class to ensure that the class is registered.
        TemplateComponentFactory<SceneNode, ClientObject, false, false, true>::registerFactory();
        Object::addAutoCreateComponent<SceneNode>( "Node" );
        camp::classByType<SceneNode>();
        TemplateComponentFactory<Mesh, ClientObject, false>::registerFactory();
        camp::classByType<Mesh>();
        TemplateComponentFactory<Entity, ClientObject, false>::registerFactory();
        camp::classByType<Entity>();
        TemplateComponentFactory<Light, ClientObject, false>::registerFactory();
        camp::classByType<Light>();
        TemplateComponentFactory<Camera, ClientObject, false, true, true>::registerFactory();
        camp::classByType<Camera>();
        TemplateComponentFactory<Animation, ClientObject, true>::registerFactory();
        camp::classByType<Animation>();
        TemplateComponentFactory<Text, ClientObject, true>::registerFactory();
        camp::classByType<Text>();
        TemplateComponentFactory<CollisionShape, ClientObject, false>::registerFactory();
        camp::classByType<CollisionShape>();
        TemplateComponentFactory<RigidBody, ClientObject, false>::registerFactory();
        camp::classByType<RigidBody>();
        TemplateComponentFactory<AreaTrigger, ClientObject, false>::registerFactory();
        camp::classByType<AreaTrigger>();
        TemplateComponentFactory<ForceField, ClientObject, false>::registerFactory();
        camp::classByType<ForceField>();
        TemplateComponentFactory<Audio, ClientObject, true>::registerFactory();
        camp::classByType<Audio>();
        TemplateComponentFactory<LuaObjectScript, ClientObject, true>::registerFactory();
        camp::classByType<LuaObjectScript>();
        TemplateComponentFactory<Particle, ClientObject, true>::registerFactory();
        camp::classByType<Particle>();

        // Add plugin factories, get camp class to ensure that the class is registered.
        TemplatePluginFactory<PermissionManager, ClientPluginManager>::registerFactory();
        camp::classByType<PermissionManager>();
        PluginManager::addAutoCreatePlugin<PermissionManager>();
        TemplatePluginFactory<ResourceManager, ClientPluginManager>::registerFactory();
        camp::classByType<ResourceManager>();
        PluginManager::addAutoCreatePlugin<ResourceManager>();
        TemplatePluginFactory<ClientObjectTemplateManager, ClientPluginManager>::registerFactory();
        camp::classByType<ClientObjectTemplateManager>();
        PluginManager::addAutoCreatePlugin<ClientObjectTemplateManager>();
        ObjectManagerFactory<ClientObjectManager, ClientPluginManager>::registerFactory( mUpdateSignal, mLateUpdateSignal );
        camp::classByType<ClientObjectManager>();
        PluginManager::addAutoCreatePlugin<ClientObjectManager>();
        TemplatePluginFactory<ServerNeighborsPlugin, ClientPluginManager>::registerFactory();
        camp::classByType<ServerNeighborsPlugin>();
        TemplatePluginFactory<SkyPlugin, ClientPluginManager>::registerFactory();
        camp::classByType<SkyPlugin>();
        TemplatePluginFactory<Terrain, ClientPluginManager>::registerFactory();
        camp::classByType<Terrain>();
        TemplatePluginFactory<SceneManagerPlugin, ClientPluginManager>::registerFactory();
        camp::classByType<SceneManagerPlugin>();
        TemplatePluginFactory<LuaPlugin, ClientPluginManager>::registerFactory();
        camp::classByType<LuaPlugin>();
        PluginManager::addAutoCreatePlugin<LuaPlugin>();
        TemplatePluginFactory<LevelManager, ClientPluginManager>::registerFactory();
        camp::classByType<LevelManager>();

        // Override the default game mode.
        TemplatePluginFactory<GameModePlugin, ClientPluginManager>::registerFactory();
        GameModePlugin::setDefaultSlot( sigc::ptr_fun( &DefaultGameMode::createGameMode ) );
        PluginManager::addAutoCreatePlugin<GameModePlugin>();

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

        if( !GlobalsBase::mOffline )
        {
            mStateMachine->pushState( new MenuState() );
        }
        else 
        {
            mGridManager->createOfflineServer();
            mStateMachine->pushState( new LoadingState() );
            ClientPluginManager& pluginManager = mGridManager->getActiveServer().getPluginManager();

            if( vm.count( "media" ) )
            {
                ResourceManager& resourceManager = pluginManager.getPlugin<ResourceManager>();
                resourceManager.setGroup( mAddMedia.leaf() );
                resourceManager.setResourceLocation( mAddMedia.string() );
            }

            if( vm.count( "offline-file" ) ) 
            {
                SerializationFile* file = new XMLSerializationFile( 
                    mAddMedia / mOfflineFile, "NoSerialization", 
                    false );
                file->load();
                file->deserialize( pluginManager, false );
                delete file;
            }
            else if( vm.count( "offline-script" ) )
            {
                const camp::Class& metaclass = camp::classByType<GameModePlugin>();
                camp::UserObject gameMode = pluginManager.createPlugin<GameModePlugin>();
                metaclass.property( "ClientSecurityLevel" ).set( gameMode, LUASEC_LOW );
                const camp::ArrayProperty& scriptsProp = static_cast<const camp::ArrayProperty&>( 
                     metaclass.property( "ClientScriptFiles" ) );
                scriptsProp.insert( gameMode, scriptsProp.size( gameMode ), mOfflineScript );
            }
            else
            {
                LOGC << "Running in offline mode but no file or script was given.";
                ClientApplication::exit();
                return;
            }
        }
    }
    /*catch( const Exception& e )
    {
        LOGC << e.what();
        ClientApplication::exit();
        throw e;
    }*/
    catch( const Ogre::Exception& e )
    {
        LOGC << e.what();
        ClientApplication::exit();
        throw e;
    }

    // Run the game loop
    run();

    ClientApplication::exit();
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

void ClientApplication::exit()
{
    // Save configuration
    mConfigManager->save();

    // Cleanup curl
    curl_global_cleanup();
}

//------------------------------------------------------------------------------
} // Namespace DefaultClient
} // Namespace Diversia