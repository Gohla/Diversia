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
#include "Client/Permission/PermissionManager.h"
#include "GameMode/EditorGameMode.h"
#include "OgreClient/Graphics/SceneManagerPlugin.h"
#include "State/LoadingState.h"
#include "UI/MainWindow.h"
#include "UI/ObjectTreeView.h"
#include "Util/Serialization/XMLSerializationFile.h"
#include "Util/State/StateMachine.h"
#include <QFileDialog>
#include <QPainter>

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
    mUI.pluginsDock->setStyle( new IconDockStyle( 
        QIcon( ":/Icons/Icons/status/nm-device-wired.png"), mUI.pluginsDock->style() ) );
    mUI.propertyBrowserDock->setStyle( new IconDockStyle( 
        QIcon( ":/Icons/Icons/categories/xfce-system.png" ), mUI.propertyBrowserDock->style() ) );
    mUI.consoleDock->setStyle( new IconDockStyle( 
        QIcon( ":/Icons/Icons/apps/utilities-terminal.png" ), mUI.consoleDock->style() ) );
    // View action icons
    mUI.objectsDock->toggleViewAction()->setIcon( 
        QIcon( ":/Icons/Icons/mimetypes/gnome-package.png" ) );
    mUI.pluginsDock->toggleViewAction()->setIcon( 
        QIcon( ":/Icons/Icons/status/nm-device-wired.png" ) );
    mUI.propertyBrowserDock->toggleViewAction()->setIcon( 
        QIcon( ":/Icons/Icons/categories/xfce-system.png" ) );
    mUI.consoleDock->toggleViewAction()->setIcon( 
        QIcon( ":/Icons/Icons/apps/utilities-terminal.png" ) );
    // View menu actions
    mUI.menuView->addAction( mUI.objectsDock->toggleViewAction() );
    mUI.menuView->addAction( mUI.pluginsDock->toggleViewAction() );
    mUI.menuView->addAction( mUI.propertyBrowserDock->toggleViewAction() );
    mUI.menuView->addAction( mUI.consoleDock->toggleViewAction() );

    QObject::connect( mUI.actionConnect, SIGNAL( triggered() ), &mConnectDialog, SLOT( show() ) );
    QObject::connect( mUI.actionNew, SIGNAL( triggered() ), &mNewGameDialog, SLOT( show() ) );

    EditorGlobals::mMainWindow = this;
}

MainWindow::~MainWindow()
{
    EditorGlobals::mMainWindow = 0;
}

void MainWindow::init()
{
    mUI.treeViewObjects->init();
    mUI.treeViewPlugins->init();
    mConnectDialog.init();
}

void MainWindow::exit()
{
    LOGE << "TestError";
    LOGW << "TestWarning";
    LOGI << "TestInfo";
    LOGD << "TestDebug";
    LOGC << "TestCritical";

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

void MainWindow::logSettingsChanged()
{

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