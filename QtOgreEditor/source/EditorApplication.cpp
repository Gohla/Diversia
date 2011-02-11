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
#include "Client/Object/ClientObjectManager.h"
#include "Client/Permission/PermissionManager.h"
#include "EditorApplication.h"
#include "GameMode/EditorGameMode.h"
#include "Graphics/OgreWidget.h"
#include "Object/Object.h"
#include "OgreClient/Audio/AudioManager.h"
#include "OgreClient/GameMode/GameModePlugin.h"
#include "OgreClient/Graphics/CameraManager.h"
#include "OgreClient/Graphics/GraphicsManager.h"
#include "OgreClient/Graphics/SceneManagerPlugin.h"
#include "OgreClient/Graphics/SkyPlugin.h"
#include "OgreClient/Graphics/Terrain.h"
#include "OgreClient/Input/ObjectSelection.h"
#include "OgreClient/Object/Animation.h"
#include "OgreClient/Object/AreaTrigger.h"
#include "OgreClient/Object/Audio.h"
#include "OgreClient/Object/Camera.h"
#include "OgreClient/Object/CollisionShape.h"
#include "OgreClient/Object/Entity.h"
#include "OgreClient/Object/LuaObjectScript.h"
#include "OgreClient/Object/Mesh.h"
#include "OgreClient/Object/Particle.h"
#include "OgreClient/Object/RigidBody.h"
#include "OgreClient/Object/SceneNode.h"
#include "OgreClient/Object/Text.h"
#include "OgreClient/Physics/PhysicsManager.h"
#include "OgreClient/Resource/ResourceManager.h"
#include "Shared/ClientServerPlugin/Factories/ObjectManagerFactory.h"
#include "Shared/ClientServerPlugin/Factories/TemplatePluginFactory.h"
#include "Shared/Crash/CrashReporter.h"
#include "Shared/Object/TemplateComponentFactory.h"
#include "State/InitialState.h"
#include "UI/MainWindow.h"
#include "Util/Serialization/XMLSerializationFile.h"
#include "Util/State/StateMachine.h"
#include <boost/lambda/lambda.hpp>

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

EditorApplication::EditorApplication( int argc, char* argv[] ):
    QApplication( argc, argv ),
    mLogFormatter
    (
        boost::log::formatters::format( "[%1%] [%2%] %3%" )
            % boost::log::formatters::date_time( "TimeStamp", "%H:%M:%S" )
            % boost::log::formatters::attr< String >( "Channel" )
            % boost::log::formatters::message()
    ),
    mLogLevel( LOG_INFO ),
    mUpdateRate( 1.0 / 60.0 * 1000.0 )
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

void EditorApplication::init()
{
    // Init curl
    curl_global_init( CURL_GLOBAL_DEFAULT );

    // Load configuration
    mConfigManager.reset( new ConfigManager( new XMLSerializationFile( "config.xml" ) ) );
    EditorGlobals::mConfig = mConfigManager.get();
    mConfigManager->load();

    try
    {
        mConfigManager->registerObject( this );

        // Initialize logging
        mLogger.reset( new Logger( mLogLevel, false, true ) );
        boost::shared_ptr<boost::log::core> core = boost::log::core::get();
        boost::shared_ptr<TextEditLogger> backend = boost::make_shared<TextEditLogger>( mLogLevel );
        typedef boost::log::sinks::synchronous_sink<TextEditLogger> SinkType;
        boost::shared_ptr<SinkType> sink( new SinkType( backend ) );
        core->add_sink( sink );

        // Initialize crash reporter
        CrashReporter* reporter = CrashReporter::createCrashReporter();
        mCrashReporter.reset( reporter );
        mCrashReporter->setEnabled( true );
        mCrashReporter->setSilent( false );
        mCrashReporter->setAppName( "Diversia_QtOgreEditor" );
        mCrashReporter->setAppVersion( "trunk" ); // TODO: Get revision number
        mConfigManager->registerObject( reporter );

        // Initialize main window
        mMainWindow.reset( new MainWindow() ); 

        // Add component factories, get camp class to ensure that the class is registered.
        TemplateComponentFactory<SceneNode, ClientObject, false, false, true>::registerFactory();
        Object::addAutoCreateComponent<SceneNode>( "Node" );
        camp::classByType<SceneNode>();
        TemplateComponentFactory<Mesh, ClientObject, false>::registerFactory();
        camp::classByType<Mesh>();
        TemplateComponentFactory<Entity, ClientObject, false>::registerFactory();
        camp::classByType<Entity>();
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
        TemplateComponentFactory<Audio, ClientObject, true>::registerFactory();
        camp::classByType<Audio>();
        TemplateComponentFactory<LuaObjectScript, ClientObject, true>::registerFactory();
        camp::classByType<LuaObjectScript>();
        TemplateComponentFactory<Particle, ClientObject, true>::registerFactory();
        camp::classByType<Particle>();

        // Add plugin factories, get camp class to ensure that the class is registered.
        TemplatePluginFactory<PermissionManager, ServerPluginManager>::registerFactory();
        camp::classByType<PermissionManager>();
        TemplatePluginFactory<ResourceManager, ServerPluginManager>::registerFactory();
        camp::classByType<ResourceManager>();
        ObjectManagerFactory<ClientObjectManager, ServerPluginManager>::registerFactory( mUpdateSignal, mLateUpdateSignal );
        camp::classByType<ClientObjectManager>();
        TemplatePluginFactory<ServerNeighborsPlugin, ServerPluginManager>::registerFactory();
        camp::classByType<ServerNeighborsPlugin>();
        TemplatePluginFactory<SkyPlugin, ServerPluginManager>::registerFactory();
        camp::classByType<SkyPlugin>();
        TemplatePluginFactory<Terrain, ServerPluginManager>::registerFactory();
        camp::classByType<Terrain>();
        TemplatePluginFactory<SceneManagerPlugin, ServerPluginManager>::registerFactory();
        camp::classByType<SceneManagerPlugin>();
        TemplatePluginFactory<LuaPlugin, ServerPluginManager>::registerFactory();
        camp::classByType<LuaPlugin>();
        ClientServerPluginManager::addAutoCreateComponent<LuaPlugin>();

        // Override the client's default game mode with one for the editor.
        TemplatePluginFactory<GameModePlugin, ServerPluginManager>::registerFactory();
        camp::classByType<GameModePlugin>();
        GameModePlugin::setDefaultSlot( sigc::ptr_fun( &EditorGameMode::createGameMode ) );
        GameModePlugin::forceDefaultConstruction( true );

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
        mObjectSelection.reset( new ObjectSelection() );

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
    mTimer.restart();
    QObject::connect( mUpdateTimer, SIGNAL( timeout() ), this, SLOT( update() ) );
    if( mUpdateRate )
        mUpdateTimer->start( mUpdateRate );
    else
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

void EditorApplication::writeLogRecord( const boost::log::basic_record<char>& rRecord )
{
    // Format log message
    StringStream ss;
    mLogFormatter( ss, rRecord );
    QString message( ss.str().c_str() );

    boost::mutex::scoped_lock lock( logRecordMutex );

    LogLevel logLevel = LOG_INFO;
    String channel;
    if( boost::log::extract<LogLevel>( "Severity", rRecord.attribute_values(), 
        boost::lambda::var( logLevel ) = boost::lambda::_1 ) && 
        boost::log::extract<String>( "Channel", rRecord.attribute_values(), 
        boost::lambda::var( channel ) = boost::lambda::_1 ) )
    {

        // Send log message to log list widget.
        QListWidgetItem* item = new QListWidgetItem( message, mMainWindow->mUI.logListWidget );
        item->setData( Qt::UserRole, logLevel ); // Log level
        item->setData( Qt::UserRole + 1, QString::fromStdString( channel ) ); // Log channel
        item->setData( Qt::UserRole + 2, mMainWindow->isSeverityChecked( logLevel ) ); // Shown because of severity?
        item->setData( Qt::UserRole + 3, mMainWindow->isSourceChecked( channel ) ); // Shown because of source?
        mMainWindow->checkLogItem( mMainWindow->mUI.logListWidget->model()->index( 
            mMainWindow->mUI.logListWidget->row( item ), 0 ) );
        mMainWindow->mUI.logListWidget->scrollToItem( item, QAbstractItemView::PositionAtBottom );

        // Select color
        switch( logLevel )
        {
            case LOG_CRITICAL: item->setForeground( QBrush( Qt::darkRed ) ); break;
            case LOG_ERROR: item->setForeground( QBrush( Qt::red ) ); break;
            case LOG_WARNING: item->setForeground( QBrush( Qt::darkYellow ) ); break;
            case LOG_INFO: item->setForeground( QBrush( Qt::black ) ); break;
            case LOG_DEBUG: item->setForeground( QBrush( Qt::gray ) ); break;
            case LOG_ENTRYEXIT: item->setForeground( QBrush( Qt::lightGray ) ); break;
        }

        // Select icon
        switch( logLevel )
        {
            case LOG_CRITICAL: item->setIcon( QIcon( ":/Icons/Icons/status/dialog-error.png" ) ); break;
            case LOG_ERROR: item->setIcon( QIcon( ":/Icons/Icons/status/dialog-error.png" ) ); break;
            case LOG_WARNING: item->setIcon( QIcon( ":/Icons/Icons/status/dialog-warning.png" ) ); break;
            case LOG_INFO: item->setIcon( QIcon( ":/Icons/Icons/status/dialog-info.png" ) ); break;
            case LOG_DEBUG: item->setIcon( QIcon( ":/Icons/Icons/status/dialog-question.png" ) );break;
            case LOG_ENTRYEXIT: item->setIcon( QIcon( ":/Icons/Icons/status/dialog-question.png" ) ); break;
        }

        // Send errors to status bar.
        if( logLevel >= LOG_ERROR )
        {
            QPalette palette; palette.setColor( QPalette::WindowText, Qt::red );
            mMainWindow->mUI.statusBar->setPalette( palette );
            mMainWindow->mUI.statusBar->showMessage( message, 8000 );
        }
    }
}

void EditorApplication::update()
{
    const Real elapsed = mTimer.elapsed();
    mTimer.restart();

    mEarlyUpdateSignal();
    mEarlyFrameSignal( elapsed );
    mUpdateSignal();
    mFrameSignal( elapsed );
    mLateUpdateSignal();
    mLateFrameSignal( elapsed );
    mMainWindow->mUI.graphicsView->update();

    // Write logs to text edit
    /*if( !mLogRecords.size() ) return;
    boost::mutex::scoped_lock lock( logRecordMutex );
    QListWidgetItem* item = 0;
    for( LogRecords::const_iterator i = mLogRecords.begin();i != mLogRecords.end(); ++i )
    {
        const boost::log::record& record = *i;
        // Format log message
        StringStream ss;
        //mLogFormatter( ss, record );
        QString message( record.message().c_str() );
        
        LogLevel logLevel = LOG_INFO;
        if( boost::log::extract<LogLevel>( "Severity", record.attribute_values(), 
            boost::lambda::var( logLevel ) = boost::lambda::_1 ) )
        {
            // Send log message to log list widget.
            item = new QListWidgetItem( message, mMainWindow->mUI.logListWidget );
            
            // Select color
            switch( logLevel )
            {
                case LOG_CRITICAL: item->setForeground( QBrush( Qt::darkRed ) ); break;
                case LOG_ERROR: item->setForeground( QBrush( Qt::red ) ); break;
                case LOG_WARNING: item->setForeground( QBrush( Qt::darkYellow ) ); break;
                case LOG_DEBUG: item->setForeground( QBrush( Qt::gray ) ); break;
                case LOG_ENTRYEXIT: item->setForeground( QBrush( Qt::lightGray ) ); break;
            }

            // Send errors to status bar.
            if( logLevel >= LOG_ERROR )
            {
                QPalette palette; palette.setColor( QPalette::WindowText, Qt::red );
                mMainWindow->mUI.statusBar->setPalette( palette );
                mMainWindow->mUI.statusBar->showMessage( message, 8000 );
            }
        }
    }

    // Scroll to last item
    if( item ) mMainWindow->mUI.logListWidget->scrollToItem( item, QAbstractItemView::PositionAtBottom );
    mLogRecords.clear();*/
}

bool EditorApplication::notify( QObject* pObject, QEvent* pEvent )
{
    try
    {
        return QApplication::notify( pObject, pEvent );
    }
    catch( Exception e )
    {
        e.log( Diversia::QtOgreEditor::Log::lg, true );

#ifdef DIVERSIA_DEBUG
        DivAssert( 0, "DIVERSIA UNHANDLED EXCEPTION" );
#else
        EditorApplication::unhandledException( e.what() );
#endif
    }
    catch( Ogre::Exception e )
    {
        LOGC << "OGRE UNHANDLED EXCEPTION: " << e.what();

#ifdef DIVERSIA_DEBUG
        DivAssert( 0, "OGRE UNHANDLED EXCEPTION" );
#else
        EditorApplication::unhandledException( e.what() );
#endif
    }
    catch( camp::Error e )
    {
        LOGC << "CAMP UNHANDLED EXCEPTION: " << e.what();

#ifdef DIVERSIA_DEBUG
        DivAssert( 0, "CAMP UNHANDLED EXCEPTION" );
#else
        EditorApplication::unhandledException( e.what() );
#endif
    }
    catch( std::exception e )
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

void TextEditLogger::do_consume( record_type const& rRecord, target_string_type const& rMessage )
{
    static_cast<EditorApplication*>( EditorGlobals::mApp )->writeLogRecord( rRecord );
}

//------------------------------------------------------------------------------

} // Namespace QtOgreEditor
} // Namespace Diversia