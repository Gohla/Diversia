/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "Client/Communication/GridManager.h"
#include "Client/Communication/ServerNeighborsPlugin.h"
#include "Client/Lua/LuaPlugin.h"
#include "Client/Object/ClientObjectTemplateManager.h"
#include "Client/Permission/PermissionManager.h"
#include "Client/Undo/UndoStack.h"
#include "EditorApplication.h"
#include "GameMode/EditorGameMode.h"
#include "Graphics/OgreWidget.h"
#include "Log/QtLogger.h"
#include "Object/EditorObjectManager.h"
#include "Object/Object.h"
#include "OgreClient/Audio/AudioManager.h"
#include "OgreClient/GameMode/GameModePlugin.h"
#include "OgreClient/Graphics/CameraManager.h"
#include "OgreClient/Graphics/GraphicsManager.h"
#include "OgreClient/Graphics/SceneManagerPlugin.h"
#include "OgreClient/Graphics/SkyPlugin.h"
#include "OgreClient/Graphics/Terrain.h"
#include "OgreClient/Input/ObjectSelection.h"
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
#include "Shared/Crash/CrashReporter.h"
#include "Shared/Object/TemplateComponentFactory.h"
#include "Shared/Plugin/Factories/ObjectManagerFactory.h"
#include "Shared/Plugin/Factories/TemplatePluginFactory.h"
#include "State/InitialState.h"
#include "UI/MainWindow.h"
#include "Util/Config/ConfigManager.h"
#include "Util/Serialization/XMLSerializationFile.h"
#include "Util/State/StateMachine.h"
#include <boost/timer.hpp>

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

EditorApplication::EditorApplication( int argc, char* argv[] ):
    QApplication( argc, argv ),
    mTimer( new boost::timer() ),
    mStopUpdates( true )
{
    if( !QResource::registerResource( "../../qt/MainWindow.rcc" ) )
    {
        QResource::registerResource( "MainWindow.rcc" );
    }

    EditorGlobals::mApp = this;
    EditorGlobals::mEarlyUpdateSignal = &mEarlyUpdateSignal;
    EditorGlobals::mEarlyFrameSignal = &mEarlyFrameSignal;
    EditorGlobals::mUpdateSignal = &mUpdateSignal;
    EditorGlobals::mFrameSignal = &mFrameSignal;
    EditorGlobals::mLateUpdateSignal = &mLateUpdateSignal;
    EditorGlobals::mLateFrameSignal = &mLateFrameSignal;

    DelayedCall::setUpdateSignal( mUpdateSignal );

    mUpdateTimer = new QTimer( this );
}

EditorApplication::~EditorApplication()
{
    mUpdateTimer->stop();
    delete mUpdateTimer;
}

void EditorApplication::init( int argc, char* argv[] )
{
    // Init curl
    curl_global_init( CURL_GLOBAL_DEFAULT );

    // Load configuration
    mConfigManager.reset( new ConfigManager( new XMLSerializationFile( "config.xml" ) ) );
    EditorGlobals::mConfig = mConfigManager.get();
    mConfigManager->load();

    try
    {
        // Initialize logging
        mLogger.reset( new Logger( LOG_DEBUG, false, true ) );

        // Initialize crash reporter
        CrashReporter* reporter = CrashReporter::createCrashReporter();
        mCrashReporter.reset( reporter );
        mCrashReporter->setEnabled( true );
        mCrashReporter->setSilent( false );
        mCrashReporter->setAppName( "Diversia_QtOgreEditor" );
        mCrashReporter->setAppVersion( "trunk" ); // TODO: Get revision number
        mConfigManager->registerObject( reporter );

        // Initialize undo
        GlobalsBase::mUndoStack = new UndoStack();

        // Initialize main window
        mMainWindow.reset( new MainWindow() );

        // Initialize Qt logger
        boost::shared_ptr<boost::log::core> core = boost::log::core::get();
        mQtLogger = boost::make_shared<QtLogger>();
        typedef boost::log::sinks::synchronous_sink<QtLogger> SinkType;
        boost::shared_ptr<SinkType> sink( new SinkType( mQtLogger ) );
        core->add_sink( sink );

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
        TemplatePluginFactory<ResourceManager, ClientPluginManager>::registerFactory();
        camp::classByType<ResourceManager>();
        TemplatePluginFactory<ClientObjectTemplateManager, ClientPluginManager>::registerFactory();
        camp::classByType<ClientObjectTemplateManager>();
        ObjectManagerFactory<EditorObjectManager, ClientPluginManager>::registerFactory( mUpdateSignal, mLateUpdateSignal );
        camp::classByType<EditorObjectManager>();
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

        // Override the client's default game mode with one for the editor.
        TemplatePluginFactory<GameModePlugin, ClientPluginManager>::registerFactory();
        camp::classByType<GameModePlugin>();
        GameModePlugin::setDefaultSlot( sigc::ptr_fun( &EditorGameMode::createGameMode ) );

        // Initialize graphics
        mGraphicsManager.reset( new GraphicsManager() );
        mConfigManager->registerObject( *mGraphicsManager );
        mGraphicsManager->init( EditorGlobals::mOgreWidget->width(), 
            EditorGlobals::mOgreWidget->height(), EditorGlobals::mOgreWidget->getWidgetHandle() );
        EditorGlobals::mOgreWidget->init( mGraphicsManager->getRoot(), mGraphicsManager->getWindow() );

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

        // Initialize object selection
        mObjectSelection.reset( new ObjectSelection( QueryFlags_Entity | QueryFlags_Particle | QueryFlags_Gizmo ) );

        // Initialize grid manager
        mGridManager.reset( new GridManager( mUpdateSignal ) );
        mConfigManager->registerObject( mGridManager.get() );
        mConfigManager->registerObject( ServerConnection::getSettings() );
        mCameraManager->setGridManager( *mGridManager.get() );
        EditorGlobals::mGrid = mGridManager.get();

        // Initialize main window
        mConfigManager->registerObject( mMainWindow->getConnectDialog() );
        mMainWindow->init();

        // Initialize UI
        mMainWindow->show();
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
}

void EditorApplication::run()
{
    // Start the update timer.
    mTimer->restart();
    QObject::connect( mUpdateTimer, SIGNAL( timeout() ), this, SLOT( update() ) );
    mUpdateTimer->setSingleShot( false );
    mUpdateTimer->setInterval( 0 );
    mUpdateTimer->start();

    // Push menu state to the state machine.
    mStateMachine.reset( new StateMachine() );
    EditorGlobals::mState = mStateMachine.get();
    mStateMachine->pushState( new InitialState() );

    // Start app, returns when app closes.
    QApplication::exec();

    // Save configuration
    mConfigManager->save();

    // Cleanup curl
    curl_global_cleanup();
}

void EditorApplication::update()
{
    const Real elapsed = mTimer->elapsed();
    mTimer->restart();

    if( !mStopUpdates ) 
    {
        mEarlyUpdateSignal();
        mEarlyFrameSignal( elapsed );
    }

    mUpdateSignal();
    mFrameSignal( elapsed );
    mLateUpdateSignal();
    mLateFrameSignal( elapsed );
    mMainWindow->mUI.graphicsView->update();
}

bool EditorApplication::notify( QObject* pObject, QEvent* pEvent )
{
    try
    {
        return QApplication::notify( pObject, pEvent );
    }
    catch( const Exception& e )
    {
        e.log( Diversia::QtOgreEditor::Log::lg, true );

#ifdef DIVERSIA_DEBUG
        DivAssert( 0, "DIVERSIA UNHANDLED EXCEPTION" );
#else
        EditorApplication::unhandledException( e.what() );
#endif
    }
    catch( const Ogre::Exception& e )
    {
        LOGC << "OGRE UNHANDLED EXCEPTION: " << e.what();

#ifdef DIVERSIA_DEBUG
        DivAssert( 0, "OGRE UNHANDLED EXCEPTION" );
#else
        EditorApplication::unhandledException( e.what() );
#endif
    }
    catch( const camp::Error& e )
    {
        LOGC << "CAMP UNHANDLED EXCEPTION: " << e.what();

#ifdef DIVERSIA_DEBUG
        DivAssert( 0, "CAMP UNHANDLED EXCEPTION" );
#else
        EditorApplication::unhandledException( e.what() );
#endif
    }
    catch( const std::exception& e )
    {
        LOGC << "OTHER UNHANDLED EXCEPTION: " << e.what();

#ifdef DIVERSIA_DEBUG
        DivAssert( 0, "OTHER UNHANDLED EXCEPTION" );
#else
        EditorApplication::unhandledException( e.what() );
#endif
    }
    catch( ... )
    {
        LOGC << "UNKNOWN UNHANDLED EXCEPTION";

#ifdef DIVERSIA_DEBUG
        DivAssert( 0, "UNKNOWN UNHANDLED EXCEPTION" );
#else
        EditorApplication::unhandledException( "Unknown unhandled exception." );
#endif
    }
    return QApplication::notify( pObject, pEvent );
    return false;
}

void EditorApplication::unhandledException( const String& rExceptionString )
{
    QMessageBox msgBox;
    QString message = 
        "Diversia has encountered an unhandled exception:\n\n" + 
        QString( rExceptionString.c_str() ) + 
        QString( "\n\nPress close to close the application or Ignore to try to ignore the exception." );
    msgBox.setText( message );
    msgBox.setIcon( QMessageBox::Critical );
    msgBox.setStandardButtons( QMessageBox::Close | QMessageBox::Ignore );
    msgBox.setDefaultButton( QMessageBox::Close );
    if( msgBox.exec() == QMessageBox::Close )
        QApplication::exit();
}

//------------------------------------------------------------------------------

} // Namespace QtOgreEditor
} // Namespace Diversia