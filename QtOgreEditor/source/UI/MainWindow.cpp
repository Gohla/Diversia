/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "Client/Communication/GridManager.h"
#include "Client/Communication/ServerAbstract.h"
#include "Client/Object/ClientObjectManager.h"
#include "Client/Object/ClientObjectTemplateManager.h"
#include "Client/Permission/PermissionManager.h"
#include "Client/Plugin/ClientPluginManager.h"
#include "Client/Undo/UndoStack.h"
#include "GameMode/EditorGameMode.h"
#include "Object/EditorObject.h"
#include "OgreClient/Graphics/SceneManagerPlugin.h"
#include "OgreClient/Input/ObjectSelection.h"
#include "OgreClient/Level/LevelManager.h"
#include "OgreClient/Resource/ResourceManager.h"
#include "State/LoadingState.h"
#include "State/PauseState.h"
#include "State/PlayState.h"
#include "UI/MainWindow.h"
#include "UI/ObjectTreeView.h"
#include "Util/Serialization/XMLSerializationFile.h"
#include "Util/State/StateMachine.h"
#include <QActionGroup>
#include <QFileDialog>
#include <QPainter>
#include <QSignalMapper>
#include <sigc++/adaptors/retype_return.h>

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

inline QString strippedName( const QString &fullFileName )
{
    return QFileInfo( fullFileName ).fileName();
}

MainWindow::MainWindow( QWidget* pParent, Qt::WFlags flags ):
    QMainWindow( pParent, flags )
{
    mUI.setupUi( this );

    // Setup docks
    QMainWindow::splitDockWidget( mUI.objectsDock, mUI.propertyBrowserDock, Qt::Horizontal );
    QMainWindow::tabifyDockWidget( mUI.objectsDock, mUI.pluginsDock );
    mUI.objectsDock->raise();
    // Dock icons
    mUI.objectsDock->setStyle( new IconDockStyle( 
        QIcon( ":/Icons/Icons/mimetypes/gnome-package.png" ), mUI.objectsDock->style() ) );
    mUI.objectTemplatesDock->setStyle( new IconDockStyle( 
        QIcon( ":/Icons/Icons/mimetypes/gnome-package.png" ), mUI.objectTemplatesDock->style() ) );
    mUI.pluginsDock->setStyle( new IconDockStyle( 
        QIcon( ":/Icons/Icons/status/nm-device-wired.png"), mUI.pluginsDock->style() ) );
    mUI.propertyBrowserDock->setStyle( new IconDockStyle( 
        QIcon( ":/Icons/Icons/categories/xfce-system.png" ), mUI.propertyBrowserDock->style() ) );
    mUI.consoleDock->setStyle( new IconDockStyle( 
        QIcon( ":/Icons/Icons/apps/utilities-terminal.png" ), mUI.consoleDock->style() ) );
    // Dock view action icons.
    mUI.objectsDock->toggleViewAction()->setIcon( 
        QIcon( ":/Icons/Icons/mimetypes/gnome-package.png" ) );
    mUI.objectTemplatesDock->toggleViewAction()->setIcon( 
        QIcon( ":/Icons/Icons/mimetypes/gnome-package.png" ) );
    mUI.pluginsDock->toggleViewAction()->setIcon( 
        QIcon( ":/Icons/Icons/status/nm-device-wired.png" ) );
    mUI.propertyBrowserDock->toggleViewAction()->setIcon( 
        QIcon( ":/Icons/Icons/categories/xfce-system.png" ) );
    mUI.consoleDock->toggleViewAction()->setIcon( 
        QIcon( ":/Icons/Icons/apps/utilities-terminal.png" ) );
    mUI.undoDock->toggleViewAction()->setIcon( 
        QIcon( ":/Icons/Icons/actions/media-skip-backward.png" ) );
    // Toolbar view action icons.
    mUI.defaultToolBar->toggleViewAction()->setIcon( 
        QIcon( ":/Icons/Icons/actions/reload.png" ) );
    /*mUI.fileToolBar->toggleViewAction()->setIcon( 
        QIcon( ":/Icons/Icons/actions/bookmark-new.png" ) );*/
    mUI.stateToolBar->toggleViewAction()->setIcon( 
        QIcon( ":/Icons/Icons/actions/media-playback-start.png" ) );
    mUI.undoToolBar->toggleViewAction()->setIcon( 
        QIcon( ":/Icons/Icons/actions/media-skip-backward.png" ) );
    mUI.manipulationToolBar->toggleViewAction()->setIcon( 
        QIcon( ":/Icons/Icons/editor/move.png" ) );
    // View menu actions
    mUI.menuView->addAction( mUI.objectsDock->toggleViewAction() );
    mUI.menuView->addAction( mUI.objectTemplatesDock->toggleViewAction() );
    mUI.menuView->addAction( mUI.pluginsDock->toggleViewAction() );
    mUI.menuView->addAction( mUI.propertyBrowserDock->toggleViewAction() );
    mUI.menuView->addAction( mUI.consoleDock->toggleViewAction() );
    mUI.menuView->addSeparator();
    mUI.menuView->addAction( mUI.defaultToolBar->toggleViewAction() );
    /*mUI.menuView->addAction( mUI.fileToolBar->toggleViewAction() );*/
    mUI.menuView->addAction( mUI.stateToolBar->toggleViewAction() );
    mUI.menuView->addAction( mUI.undoToolBar->toggleViewAction() );
    mUI.menuView->addAction( mUI.manipulationToolBar->toggleViewAction() );

    // Dialog signals
    QObject::connect( mUI.actionConnect, SIGNAL( triggered() ), &mConnectDialog, SLOT( show() ) );
    QObject::connect( mUI.actionGame_new, SIGNAL( triggered() ), &mNewGameDialog, SLOT( show() ) );

    // Setup gizmo actions
    mGizmoActions = new QActionGroup( this );
    mGizmoActions->addAction( mUI.actionSelection_mode );
    mGizmoActions->addAction( mUI.actionMovement_mode );
    mGizmoActions->addAction( mUI.actionRotation_mode );
    mGizmoActions->addAction( mUI.actionScaling_mode );
    mGizmoActions->setExclusive( true );
    mUI.manipulationToolBar->addActions( mGizmoActions->actions() );
    QObject::connect( mGizmoActions, SIGNAL( triggered(QAction*) ), this, 
        SLOT( gizmoModeChange(QAction*) ) );

    // Setup log severity controls.
    QSignalMapper* severitySignalMapper = new QSignalMapper( this );

    mUI.logSeverityDebugCheckBox->setProperty( "LogLevel", LOG_DEBUG );
    QObject::connect( mUI.logSeverityDebugCheckBox, SIGNAL( stateChanged(int) ), severitySignalMapper, SLOT( map() ) );
    severitySignalMapper->setMapping( mUI.logSeverityDebugCheckBox, mUI.logSeverityDebugCheckBox );

    mUI.logSeverityInfoCheckBox->setProperty( "LogLevel", LOG_INFO );
    QObject::connect( mUI.logSeverityInfoCheckBox, SIGNAL( stateChanged(int) ), severitySignalMapper, SLOT( map() ) );
    severitySignalMapper->setMapping( mUI.logSeverityInfoCheckBox, mUI.logSeverityInfoCheckBox );

    mUI.logSeverityWarningCheckBox->setProperty( "LogLevel", LOG_WARNING );
    QObject::connect( mUI.logSeverityWarningCheckBox, SIGNAL( stateChanged(int) ), severitySignalMapper, SLOT( map() ) );
    severitySignalMapper->setMapping( mUI.logSeverityWarningCheckBox, mUI.logSeverityWarningCheckBox );

    mUI.logSeverityErrorCheckBox->setProperty( "LogLevel", LOG_ERROR );
    QObject::connect( mUI.logSeverityErrorCheckBox, SIGNAL( stateChanged(int) ), severitySignalMapper, SLOT( map() ) );
    severitySignalMapper->setMapping( mUI.logSeverityErrorCheckBox, mUI.logSeverityErrorCheckBox );

    mUI.logSeverityCriticalCheckBox->setProperty( "LogLevel", LOG_CRITICAL );
    QObject::connect( mUI.logSeverityCriticalCheckBox, SIGNAL( stateChanged(int) ), severitySignalMapper, SLOT( map() ) );
    severitySignalMapper->setMapping( mUI.logSeverityCriticalCheckBox, mUI.logSeverityCriticalCheckBox );

    QObject::connect( severitySignalMapper, SIGNAL( mapped(QWidget*) ), this, SLOT( logSeverityChange(QWidget*) ) );

    // Setup log source controls.
    QSignalMapper* sourcesSignalMapper = new QSignalMapper( this );

    QObject::connect( mUI.logSourceUtilCheckBox, SIGNAL( stateChanged(int) ), sourcesSignalMapper, SLOT( map() ) );
    sourcesSignalMapper->setMapping( mUI.logSourceUtilCheckBox, mUI.logSourceUtilCheckBox );

    QObject::connect( mUI.logSourceObjectCheckBox, SIGNAL( stateChanged(int) ), sourcesSignalMapper, SLOT( map() ) );
    sourcesSignalMapper->setMapping( mUI.logSourceObjectCheckBox, mUI.logSourceObjectCheckBox );

    QObject::connect( mUI.logSourceSharedCheckBox, SIGNAL( stateChanged(int) ), sourcesSignalMapper, SLOT( map() ) );
    sourcesSignalMapper->setMapping( mUI.logSourceSharedCheckBox, mUI.logSourceSharedCheckBox );

    QObject::connect( mUI.logSourceClientCheckBox, SIGNAL( stateChanged(int) ), sourcesSignalMapper, SLOT( map() ) );
    sourcesSignalMapper->setMapping( mUI.logSourceClientCheckBox, mUI.logSourceClientCheckBox );

    QObject::connect( mUI.logSourceOgreClientCheckBox, SIGNAL( stateChanged(int) ), sourcesSignalMapper, SLOT( map() ) );
    sourcesSignalMapper->setMapping( mUI.logSourceOgreClientCheckBox, mUI.logSourceOgreClientCheckBox );

    QObject::connect( mUI.logSourceOgreCheckBox, SIGNAL( stateChanged(int) ), sourcesSignalMapper, SLOT( map() ) );
    sourcesSignalMapper->setMapping( mUI.logSourceOgreCheckBox, mUI.logSourceOgreCheckBox );

    QObject::connect( mUI.logSourceQtOgreEditorCheckBox, SIGNAL( stateChanged(int) ), sourcesSignalMapper, SLOT( map() ) );
    sourcesSignalMapper->setMapping( mUI.logSourceQtOgreEditorCheckBox, mUI.logSourceQtOgreEditorCheckBox );

    QObject::connect( mUI.logSourceLuaCheckBox, SIGNAL( stateChanged(int) ), sourcesSignalMapper, SLOT( map() ) );
    sourcesSignalMapper->setMapping( mUI.logSourceLuaCheckBox, mUI.logSourceLuaCheckBox );

    QObject::connect( sourcesSignalMapper, SIGNAL( mapped(QWidget*) ), this, SLOT( logSourceChange(QWidget*) ) );

    // Setup recent games and levels menus.
    mRecentGameGroup = new QActionGroup( this );
    mRecentGameGroup->setExclusive( false );
    for( unsigned short i = 0; i < cMaxRecentGames; ++i )
    {
        QAction* action = new QAction( this );
        action->setVisible( false );
        QObject::connect( action, SIGNAL( triggered() ), this, SLOT( loadRecentGame() ) );
        mRecentGameActions[i] = action;
        mUI.menuFile->insertAction( mUI.actionExit, action );
        mRecentGameGroup->addAction( action );
    }
    mRecentGameSeperator = mUI.menuFile->insertSeparator( mRecentGameActions[0] );
    mUI.menuFile->insertSeparator( mUI.actionExit );
    MainWindow::updateRecentGames();

    mLevelGroup = new QActionGroup( this );
    mLevelGroup->setExclusive( false );
    mUI.menuLevel->insertSeparator( 0 );
    for( unsigned short i = 0; i < cMaxLevels; ++i )
    {
        QAction* action = new QAction( this );
        action->setVisible( false );
        QObject::connect( action, SIGNAL( triggered() ), this, SLOT( loadListLevel() ) );
        mLevelActions[i] = action;
        mUI.menuLevel->addAction( action );
        mLevelGroup->addAction( action );
    }

    // Restore state
    QSettings settings( "Diversia", "QtOgreEditor" );
    settings.beginGroup( "MainWindow" );
    QMainWindow::restoreGeometry( settings.value( "Geometry" ).toByteArray() );
    QMainWindow::restoreState( settings.value( "State" ).toByteArray() );

    settings.beginGroup( "Log" );
    settings.beginGroup( "Severity" );
    mUI.logSeverityDebugCheckBox->setChecked( settings.value( "DebugCheckBox", false ).toBool() );
    mUI.logSeverityInfoCheckBox->setChecked( settings.value( "InfoCheckBox", true ).toBool() );
    mUI.logSeverityWarningCheckBox->setChecked( settings.value( "WarningCheckBox", true ).toBool() );
    mUI.logSeverityErrorCheckBox->setChecked( settings.value( "ErrorCheckBox", true ).toBool() );
    mUI.logSeverityCriticalCheckBox->setChecked( settings.value( "CriticalCheckBox", true ).toBool() );
    settings.endGroup(); // Severity

    settings.beginGroup( "Source" );
    mUI.logSourceUtilCheckBox->setChecked( settings.value( "UtilCheckBox", true ).toBool() );
    mUI.logSourceObjectCheckBox->setChecked( settings.value( "ObjectCheckBox", true ).toBool() );
    mUI.logSourceSharedCheckBox->setChecked( settings.value( "SharedCheckBox", true ).toBool() );
    mUI.logSourceClientCheckBox->setChecked( settings.value( "ClientCheckBox", true ).toBool() );
    mUI.logSourceOgreClientCheckBox->setChecked( settings.value( "OgreClientCheckBox", true ).toBool() );
    mUI.logSourceOgreCheckBox->setChecked( settings.value( "OgreCheckBox", true ).toBool() );
    mUI.logSourceQtOgreEditorCheckBox->setChecked( settings.value( "QtOgreEditorCheckBox", true ).toBool() );
    mUI.logSourceLuaCheckBox->setChecked( settings.value( "LuaCheckBox", true ).toBool() );
    settings.endGroup(); // Source
    settings.endGroup(); // Log

    settings.beginGroup( "Objects" );
    mUI.actionShow_runtime_objects->setChecked( settings.value( "ShowRuntimeObjects", false ).toBool() );
    mUI.actionShow_default_components->setChecked( settings.value( "ShowDefaultComponents", false ).toBool() );
    settings.endGroup(); // Objects

    settings.beginGroup( "Manipulation" );
    mUI.actionSnap_to_grid->setChecked( settings.value( "SnapToGrid", false ).toBool() );
    mUI.actionSelection_mode->setChecked( settings.value( "SelectionMode", true ).toBool() );
    mUI.actionMovement_mode->setChecked( settings.value( "MovementMode", false ).toBool() );
    mUI.actionRotation_mode->setChecked( settings.value( "RotationMode", false ).toBool() );
    mUI.actionScaling_mode->setChecked( settings.value( "ScalingMode", false ).toBool() );
    settings.endGroup(); // Manipulation

    settings.endGroup(); // MainWindow

    EditorGlobals::mMainWindow = this;
}

MainWindow::~MainWindow()
{
    EditorGlobals::mMainWindow = 0;
}

void MainWindow::init()
{
    mUI.treeViewObjects->init();
    mUI.treeViewObjectTemplates->init();
    mUI.treeViewPlugins->init();
    mConnectDialog.init();

    // Disable volume selection if selection mode action is not checked.
    if( !mUI.actionSelection_mode->isChecked() ) GlobalsBase::mSelection->enableVolumeSelect( false );
}

void MainWindow::updateLevels()
{
    try
    {
        ClientPluginManager& pluginManager = EditorGlobals::mGrid->getActiveServer().getPluginManager();
        LevelManager& levelManager = pluginManager.getPlugin<LevelManager>();
        ResourceManager& resourceManager = pluginManager.getPlugin<ResourceManager>();
        Ogre::StringVectorPtr levels = levelManager.list();

        int numListLevels = qMin( levels->size(), (std::size_t)cMaxLevels );

        for( int i = 0; i < numListLevels; ++i ) 
        {
            Path filePath = resourceManager.getResourcePath() / levels->at( i );
            QString file = QString( filePath.string().c_str() );
            QString text = tr( "&%1 %2" ).arg( i + 1 ).arg( strippedName( file ) );
            mLevelActions[i]->setText( text );
            mLevelActions[i]->setData( file );
            mLevelActions[i]->setVisible( true );
        }

        for( int i = numListLevels; i < cMaxLevels; ++i )
            mLevelActions[i]->setVisible( false );
    }
    catch( Exception e )
    {
        // Ignore error	
    }
}

void MainWindow::exit()
{
    QMessageBox msgBox;
    msgBox.setText( "Are you sure you want to exit?" );
    msgBox.setIcon( QMessageBox::Warning );
    msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
    msgBox.setDefaultButton( QMessageBox::No );
    if( msgBox.exec() == QMessageBox::Yes )
    {
        // First pop to edit (stopped) state, if we are in played or paused state.
        EditorGlobals::mState->popTo( 3 );

        // Pop to initial state in the next tick so that plugins can handle the transition to the
        // stop state.
        DelayedCall::create( sigc::bind( sigc::mem_fun( EditorGlobals::mState, 
            &StateMachine::popTo ), 1 ), 0 );
        // Close the main widget after popping to the initial state.
        DelayedCall::create( sigc::hide_return( sigc::mem_fun( this, 
            &QWidget::close ) ), 0 );
    }
}

void MainWindow::disconnect()
{
    QMessageBox msgBox;
    msgBox.setText( "Are you sure you want to disconnect from the server?" );
    msgBox.setIcon( QMessageBox::Warning );
    msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
    msgBox.setDefaultButton( QMessageBox::No );
    if( msgBox.exec() == QMessageBox::Yes )
    {
        // First pop to edit (stopped) state, if we are in played or paused state.
        EditorGlobals::mState->popTo( 3 );

        // Pop to initial state in the next tick so that plugins can handle the transition to the
        // stop state.
        DelayedCall::create( sigc::bind( sigc::mem_fun( EditorGlobals::mState, 
            &StateMachine::popTo ), 1 ), 0 );
    }
}

void MainWindow::saveGame()
{
    if( EditorGlobals::mCurrentGame.isEmpty() )
        MainWindow::saveGameAs();

    try
    {
        SerializationFile* file = new XMLSerializationFile( EditorGlobals::mCurrentGame.toStdString(), 
            "NoSerialization", false );
        file->serialize( EditorGlobals::mGrid->getActiveServer().getPluginManager(), false );
        file->save();
        delete file;
    }
    catch( Exception e )
    {
        LOGE << "Could not save game: " << e.what();	
    }
}

void MainWindow::saveGameAs()
{
    QString fileName = QFileDialog::getSaveFileName( this, "Diversia game file", "", 
        tr( "Diversia game files (*.xml)" ) );

    if( !fileName.isEmpty() )
    {
        try
        {
            SerializationFile* file = new XMLSerializationFile( fileName.toStdString(), 
                "NoSerialization", false );
            file->serialize( EditorGlobals::mGrid->getActiveServer().getPluginManager(), false );
            file->save();
            delete file;

            EditorGlobals::mCurrentGame = fileName;
        }
        catch( Exception e )
        {
            LOGE << "Could not save game: " << e.what();	
        }
    }
}

void MainWindow::loadGame()
{
    QString fileName = QFileDialog::getOpenFileName( this, "Diversia game file", "", 
        tr( "Diversia game files (*.xml)" ) );

    if( !fileName.isEmpty() )
    {
        MainWindow::_loadGame( fileName );
    }
}

void MainWindow::loadRecentGame()
{
    QAction* action = qobject_cast<QAction*>( sender() );
    if( action ) MainWindow::_loadGame( action->data().toString() );
}

void MainWindow::saveLevel()
{
    if( EditorGlobals::mCurrentLevel.isEmpty() )
        MainWindow::saveLevelAs();

    try
    {
        ClientPluginManager& pluginManager = EditorGlobals::mGrid->getActiveServer().getPluginManager();
        LevelManager& levelManager = pluginManager.getPlugin<LevelManager>();
        levelManager.storeLevel( EditorGlobals::mCurrentLevel.toStdString() );
    }
    catch( Exception e )
    {
        LOGE << "Could not save level: " << e.what();	
    }
}

void MainWindow::saveLevelAs()
{
    QSettings settings( "Diversia", "QtOgreEditor" );
    QString fileName = QFileDialog::getSaveFileName( this, "Diversia level file", 
        settings.value( "LastGame", QString() ).toString(), tr( "Diversia level files (*.lvl)" ) );

    if( !fileName.isEmpty() )
    {
        try
        {
            ClientPluginManager& pluginManager = EditorGlobals::mGrid->getActiveServer().getPluginManager();
            LevelManager& levelManager = pluginManager.getPlugin<LevelManager>();
            levelManager.storeLevel( fileName.toStdString() );

            MainWindow::updateLevels();
            EditorGlobals::mCurrentLevel = fileName;
        }
        catch( Exception e )
        {
            LOGE << "Could not save level: " << e.what();	
        }
    }
}

void MainWindow::loadLevel()
{
    QSettings settings( "Diversia", "QtOgreEditor" );
    QString fileName = QFileDialog::getOpenFileName( this, "Diversia level file", 
        settings.value( "LastGame", QString() ).toString(), tr( "Diversia level files (*.lvl)" ) );

    if( !fileName.isEmpty() )
    {
        MainWindow::_loadLevel( fileName );
    }
}

void MainWindow::loadListLevel()
{
    QAction* action = qobject_cast<QAction*>( sender() );
    if( action ) MainWindow::_loadLevel( action->data().toString() );
}

void MainWindow::logSeverityChange( QWidget* pWidget )
{
    QCheckBox* checkbox = qobject_cast<QCheckBox*>( pWidget );
    QAbstractItemModel* model = mUI.logListWidget->model();
    QModelIndexList list = model->match( model->index( 0, 0 ), Qt::UserRole, 
        checkbox->property( "LogLevel" ), -1 );

    for( QModelIndexList::iterator i = list.begin();i != list.end(); ++i )
    {
        model->setData( *i, checkbox->isChecked(), Qt::UserRole + 2 );
        MainWindow::checkLogItem( *i );
    }

    if( !list.empty() ) mUI.logListWidget->scrollToBottom();
}

void MainWindow::logSourceChange( QWidget* pWidget )
{
    QCheckBox* checkbox = qobject_cast<QCheckBox*>( pWidget );
    QAbstractItemModel* model = mUI.logListWidget->model();
    QModelIndexList list = model->match( model->index( 0, 0 ), Qt::UserRole + 1, 
        checkbox->text(), -1 );

    for( QModelIndexList::iterator i = list.begin();i != list.end(); ++i )
    {
        model->setData( *i, checkbox->isChecked(), Qt::UserRole + 3 );
        MainWindow::checkLogItem( *i );
    }

    if( !list.empty() ) mUI.logListWidget->scrollToBottom();
}

void MainWindow::play()
{
    try
    {
        GlobalsBase::mState->pushState( new PlayState() );
    }
    catch( const Exception& e )
    {
    	LOGE << "Could not play the game: " << e.what();
    }
}

void MainWindow::pause( bool pause )
{
    try
    {
        if( pause )
        {
            GlobalsBase::mState->pushState( new PauseState() );
        }
        else
        {
            // Pop the pause state off the stack.
            GlobalsBase::mState->popState();
        }
    }
    catch( const Exception& e )
    {
    	LOGE << "Could not pause the game: " << e.what();
    }
}

void MainWindow::stop()
{
    try
    {
        // Pop the play state off the stack.
        GlobalsBase::mState->popState();
    }
    catch( const Exception& e )
    {
        LOGE << "Could not stop the game: " << e.what();
    }
}

void MainWindow::undo()
{
    GlobalsBase::mUndoStack->undo();
}

void MainWindow::redo()
{
    GlobalsBase::mUndoStack->redo();
}

void MainWindow::_loadGame( const QString& rFile )
{
    EditorGlobals::mCurrentGame = rFile;
    EditorGlobals::mOffline = true;

    // TODO: This needs to wait 1 tick.
    EditorGlobals::mState->popTo( 1 );

    EditorGlobals::mGrid->createOfflineServer();
    EditorGlobals::mState->pushState( new LoadingState() );
    ClientPluginManager& pluginManager = EditorGlobals::mGrid->getActiveServer().getPluginManager();
    pluginManager.createPlugin<PermissionManager>();
    pluginManager.createPlugin<ClientObjectTemplateManager>();
    pluginManager.createPlugin<ClientObjectManager>();

    try
    {
        SerializationFile* file = new XMLSerializationFile( rFile.toStdString(), 
            "NoSerialization", false );
        file->load();
        file->deserialize( pluginManager, false );
        delete file;

        // Update recent games and path
        QSettings settings( "Diversia", "QtOgreEditor" );
        QStringList games = settings.value( "RecentGames" ).toStringList();
        games.removeAll( rFile );
        games.prepend( rFile );
        while( games.size() > cMaxRecentGames ) games.removeLast();
        settings.setValue( "RecentGames", games );
        settings.setValue( "LastGame", rFile );
        MainWindow::updateRecentGames();
    }
    catch( Exception e )
    {
        LOGE << "Could not load game: " << e.what();

        EditorGlobals::mCurrentGame = "";
        EditorGlobals::mOffline = false;
    }
}

void MainWindow::_loadLevel( const QString& rFile )
{
    try
    {
        ClientPluginManager& pluginManager = EditorGlobals::mGrid->getActiveServer().getPluginManager();
        LevelManager& levelManager = pluginManager.getPlugin<LevelManager>();
        levelManager.loadLevel( rFile.toStdString() );
        EditorGlobals::mCurrentGame = rFile;
    }
    catch( Exception e )
    {
        LOGE << "Could not load level: " << e.what();	
    }
}

void MainWindow::checkLogItem( const QModelIndex& rIndex )
{
    QAbstractItemModel* model = mUI.logListWidget->model();
    if( !model->data( rIndex, Qt::UserRole + 2 ).toBool() || 
        !model->data( rIndex, Qt::UserRole + 3 ).toBool() )
        mUI.logListWidget->item( rIndex.row() )->setHidden( true );
    else
        mUI.logListWidget->item( rIndex.row() )->setHidden( false );
}

bool MainWindow::isSeverityChecked( LogLevel logLevel )
{
    switch( logLevel )
    {
        case LOG_DEBUG: return mUI.logSeverityDebugCheckBox->isChecked();
        case LOG_INFO: return mUI.logSeverityInfoCheckBox->isChecked();
        case LOG_WARNING: return mUI.logSeverityWarningCheckBox->isChecked();
        case LOG_ERROR: return mUI.logSeverityErrorCheckBox->isChecked();
        case LOG_CRITICAL: return mUI.logSeverityCriticalCheckBox->isChecked();
    }

    return false;
}

bool MainWindow::isSourceChecked( const String& rSource )
{
    if( rSource == "Util" ) return mUI.logSourceUtilCheckBox->isChecked();
    else if( rSource == "Object" ) return mUI.logSourceObjectCheckBox->isChecked();
    else if( rSource == "Shared" ) return mUI.logSourceSharedCheckBox->isChecked();
    else if( rSource == "Client" ) return mUI.logSourceClientCheckBox->isChecked();
    else if( rSource == "OgreClient" ) return mUI.logSourceOgreClientCheckBox->isChecked();
    else if( rSource == "Ogre" ) return mUI.logSourceOgreCheckBox->isChecked();
    else if( rSource == "QtOgreEditor" ) return mUI.logSourceQtOgreEditorCheckBox->isChecked();
    else if( rSource == "Lua" ) return mUI.logSourceLuaCheckBox->isChecked();

    return false;
}

void MainWindow::updateRecentGames()
{
    QSettings settings( "Diversia", "QtOgreEditor" );
    QStringList games = settings.value( "RecentGames" ).toStringList();

    int numRecentGames = qMin( games.size(), (int)cMaxRecentGames );

    for( int i = 0; i < numRecentGames; ++i ) 
    {
        QString text = tr( "&%1 %2" ).arg( i + 1 ).arg( strippedName( games[i] ) );
        mRecentGameActions[i]->setText( text );
        mRecentGameActions[i]->setData( games[i] );
        mRecentGameActions[i]->setVisible( true );
    }

    for( int i = numRecentGames; i < cMaxRecentGames; ++i )
        mRecentGameActions[i]->setVisible( false );

    mRecentGameSeperator->setVisible( numRecentGames > 0 );
}

void MainWindow::closeEvent( QCloseEvent* pEvent )
{
    QSettings settings( "Diversia", "QtOgreEditor" );
    settings.beginGroup( "MainWindow" );
    settings.setValue( "Geometry", QMainWindow::saveGeometry() );
    settings.setValue( "State", QMainWindow::saveState() );

    settings.beginGroup( "Log" );
    settings.beginGroup( "Severity" );
    settings.setValue( "DebugCheckBox", mUI.logSeverityDebugCheckBox->isChecked() );
    settings.setValue( "InfoCheckBox", mUI.logSeverityInfoCheckBox->isChecked() );
    settings.setValue( "WarningCheckBox", mUI.logSeverityWarningCheckBox->isChecked() );
    settings.setValue( "ErrorCheckBox", mUI.logSeverityErrorCheckBox->isChecked() );
    settings.setValue( "CriticalCheckBox", mUI.logSeverityCriticalCheckBox->isChecked() );
    settings.endGroup(); // Severity

    settings.beginGroup( "Source" );
    settings.setValue( "UtilCheckBox", mUI.logSourceUtilCheckBox->isChecked() );
    settings.setValue( "ObjectCheckBox", mUI.logSourceObjectCheckBox->isChecked() );
    settings.setValue( "SharedCheckBox", mUI.logSourceSharedCheckBox->isChecked() );
    settings.setValue( "ClientCheckBox", mUI.logSourceClientCheckBox->isChecked() );
    settings.setValue( "OgreClientCheckBox", mUI.logSourceOgreClientCheckBox->isChecked() );
    settings.setValue( "OgreCheckBox", mUI.logSourceOgreCheckBox->isChecked() );
    settings.setValue( "QtOgreEditorCheckBox", mUI.logSourceQtOgreEditorCheckBox->isChecked() );
    settings.setValue( "LuaCheckBox", mUI.logSourceLuaCheckBox->isChecked() );
    settings.endGroup(); // Source
    settings.endGroup(); // Log

    settings.beginGroup( "Objects" );
    settings.setValue( "ShowRuntimeObjects", mUI.actionShow_runtime_objects->isChecked() );
    settings.setValue( "ShowDefaultComponents", mUI.actionShow_default_components->isChecked() );
    settings.endGroup(); // Objects

    settings.beginGroup( "Manipulation" );
    settings.setValue( "SnapToGrid", mUI.actionSnap_to_grid->isChecked() );
    settings.setValue( "SelectionMode", mUI.actionSelection_mode->isChecked() );
    settings.setValue( "MovementMode", mUI.actionMovement_mode->isChecked() );
    settings.setValue( "RotationMode", mUI.actionRotation_mode->isChecked() );
    settings.setValue( "ScalingMode", mUI.actionScaling_mode->isChecked() );
    settings.endGroup(); // Manipulation

    settings.endGroup(); // MainWindow

    QMainWindow::closeEvent( pEvent );
}

void MainWindow::gizmoModeChange( QAction* pAction )
{
    // Only enable volume selection in selection mode.
    if( pAction == mUI.actionSelection_mode ) GlobalsBase::mSelection->enableVolumeSelect( true );
    else GlobalsBase::mSelection->enableVolumeSelect( false );
}

//------------------------------------------------------------------------------

IconDockStyle::IconDockStyle( const QIcon& rIcon, QStyle* pStyle /*= 0*/ ):
    QProxyStyle( pStyle ),
    mIcon( rIcon )
{

}

void IconDockStyle::drawControl( ControlElement element, const QStyleOption* pOption, 
    QPainter* pPainter, const QWidget* pWidget /*= 0*/ ) const
{
    if( element == QStyle::CE_DockWidgetTitle )
    {
        // Width of the icon.
        const int width = 16;
        // Margin of title from frame.
        const int margin = 0;
        // Spacing between icon and title, even number.
        const int spacing = 4;

        QPoint icon_point( spacing / 2 + margin + pOption->rect.left(), 
            margin + pOption->rect.center().y() - width / 2 );

        pPainter->drawPixmap( icon_point, mIcon.pixmap( width, width ) );

        const_cast<QStyleOption*>( pOption )->rect = pOption->rect.adjusted( width + spacing, 0, 0, 0 );
    }

    QProxyStyle::baseStyle()->drawControl( element, pOption, pPainter, pWidget );
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia