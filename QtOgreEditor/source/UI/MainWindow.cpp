/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "Client/ClientServerPlugin/ServerPluginManager.h"
#include "Client/Communication/GridManager.h"
#include "Client/Communication/ServerAbstract.h"
#include "Client/Object/ClientObjectManager.h"
#include "Client/Object/ClientObjectTemplateManager.h"
#include "Client/Permission/PermissionManager.h"
#include "Client/Undo/UndoStack.h"
#include "GameMode/EditorGameMode.h"
#include "OgreClient/Graphics/SceneManagerPlugin.h"
#include "State/LoadingState.h"
#include "UI/MainWindow.h"
#include "UI/ObjectTreeView.h"
#include "Util/Serialization/XMLSerializationFile.h"
#include "Util/State/StateMachine.h"
#include <QFileDialog>
#include <QPainter>
#include <QSignalMapper>

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

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
    // View action icons
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
    // View menu actions
    mUI.menuView->addAction( mUI.objectsDock->toggleViewAction() );
    mUI.menuView->addAction( mUI.objectTemplatesDock->toggleViewAction() );
    mUI.menuView->addAction( mUI.pluginsDock->toggleViewAction() );
    mUI.menuView->addAction( mUI.propertyBrowserDock->toggleViewAction() );
    mUI.menuView->addAction( mUI.consoleDock->toggleViewAction() );

    QObject::connect( mUI.actionConnect, SIGNAL( triggered() ), &mConnectDialog, SLOT( show() ) );
    QObject::connect( mUI.actionNew, SIGNAL( triggered() ), &mNewGameDialog, SLOT( show() ) );

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
}

void MainWindow::exit()
{
    QMessageBox msgBox;
    msgBox.setText( "Are you sure you want to exit?" );
    msgBox.setIcon( QMessageBox::Warning );
    msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
    msgBox.setDefaultButton( QMessageBox::No );
    if( msgBox.exec() == QMessageBox::Yes )
        QWidget::close();
}

void MainWindow::disconnect()
{
    QMessageBox msgBox;
    msgBox.setText( "Are you sure you want to disconnect from the server?" );
    msgBox.setIcon( QMessageBox::Warning );
    msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
    msgBox.setDefaultButton( QMessageBox::No );
    if( msgBox.exec() == QMessageBox::Yes )
        EditorGlobals::mState->popTo( 1 );
}

void MainWindow::save()
{
    if( EditorGlobals::mCurrentFile.isEmpty() )
        MainWindow::saveAs();

    try
    {
        SerializationFile* file = new XMLSerializationFile( EditorGlobals::mCurrentFile.toStdString(), 
            "NoSerialization", false );
        file->serialize( EditorGlobals::mGrid->getActiveServer().getPluginManager(), false );
        file->save();
        delete file;
    }
    catch( Exception e )
    {
        LOGE << "Could not save: " << e.what();	
    }
}

void MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName( this, "Game file", "", 
        tr( "Game files (*.xml)" ) );
    if( !fileName.isEmpty() )
    {
        EditorGlobals::mCurrentFile = fileName;

        try
        {
            SerializationFile* file = new XMLSerializationFile( fileName.toStdString(), 
                "NoSerialization", false );
            file->serialize( EditorGlobals::mGrid->getActiveServer().getPluginManager(), false );
            file->save();
            delete file;
        }
        catch( Exception e )
        {
            LOGE << "Could not save: " << e.what();	
        }
    }
}

void MainWindow::load()
{
    QString fileName = QFileDialog::getOpenFileName( this, "Game file", "", 
        tr( "Game files (*.xml)" ) );

    if( !fileName.isEmpty() )
    {
        // TODO: This needs to wait 1 tick.
        EditorGlobals::mState->popTo( 1 );

        EditorGlobals::mGrid->createOfflineServer();
        EditorGlobals::mState->pushState( new LoadingState() );
        ServerPluginManager& pluginManager = EditorGlobals::mGrid->getActiveServer().getPluginManager();
        pluginManager.createPlugin<PermissionManager>();
        pluginManager.createPlugin<ClientObjectTemplateManager>();
        pluginManager.createPlugin<ClientObjectManager>();

        try
        {
            SerializationFile* file = new XMLSerializationFile( fileName.toStdString(), 
                "NoSerialization", false );
            file->load();
            file->deserialize( pluginManager, false );
            delete file;
        }
        catch( Exception e )
        {
            LOGE << "Could not load: " << e.what();	
        }

        EditorGlobals::mCurrentFile = fileName;
        EditorGlobals::mOffline = true;
    }
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

void MainWindow::undo()
{
    GlobalsBase::mUndoStack->undo();
}

void MainWindow::redo()
{
    GlobalsBase::mUndoStack->redo();
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
    settings.endGroup(); // MainWindow

    QMainWindow::closeEvent( pEvent );
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