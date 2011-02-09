/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "UI/PluginTreeView.h"
#include "UI/MainWindow.h"
#include "Model/ClientServerPluginModel.h"
#include "Client/Communication/GridManager.h"
#include "Shared/ClientServerPlugin/ClientServerPluginFactoryManager.h"
#include "Shared/ClientServerPlugin/ClientServerPluginFactory.h"
#include "Client/ClientServerPlugin/ServerPluginManager.h"
#include "Client/ClientServerPlugin/ServerPlugin.h"
#include "qsignalmapper.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

PluginTreeView::PluginTreeView( QWidget* pParent /*= 0*/ ):
    QTreeView( pParent ),
    mModel( new ClientServerPluginModel( this ) )
{
    QTreeView::setDragDropMode( QTreeView::NoDragDrop );
    QTreeView::setModel( mModel );
    QWidget::setContextMenuPolicy( Qt::CustomContextMenu );
    QObject::connect( this, SIGNAL( customContextMenuRequested(const QPoint&) ), 
        this, SLOT( showContextMenu(const QPoint&) ) );
}

PluginTreeView::~PluginTreeView()
{

}

void PluginTreeView::init()
{
    EditorGlobals::mGrid->connectServerChange( sigc::mem_fun( this, &PluginTreeView::serverChange ) );

    // Create new plugin actions.
    QSignalMapper* newPluginSignalMapper = new QSignalMapper( this );
    QSignalMapper* pluginActionSignalMapper = new QSignalMapper( this );
    const ClientServerPluginFactories& factories = 
        ClientServerPluginFactoryManager::getPluginFactories();
    QList<QAction*> actions;
    for( ClientServerPluginFactories::const_iterator i = factories.begin(); i != factories.end(); ++i )
    {

        try
        {
            QString className = i->second->getTypeName().c_str();
            const camp::Class& metaclass = camp::classByName( className.toStdString() );
            QString icon = ":/Icons/Icons/categories/xfce-system.png";
            if( metaclass.hasTag( "QtIcon" ) ) icon = metaclass.tag( "QtIcon" ).to<QString>();

            // Add create plugin menu action.
            QAction* newPluginAction = new QAction( QIcon( icon ), className, this );
            QObject::connect( newPluginAction, SIGNAL( triggered() ), newPluginSignalMapper, SLOT( map() ) );
            newPluginSignalMapper->setMapping( newPluginAction, (int)i->second->getType() );
            actions.push_back( newPluginAction );

            // Add plugin specific actions.
            std::size_t functionCount = metaclass.functionCount();
            for( std::size_t j = 0; j < functionCount; ++j )
            {
                const camp::Function& function = metaclass.function( j );
                if( function.hasTag( "QtPopupMenu" ) )
                {
                    QString pluginActionIcon = ":/Icons/Icons/categories/xfce-system.png";
                    QString pluginActionName = QString::fromStdString( function.name() );
                    if( function.hasTag( "QtIcon" ) )  pluginActionIcon = function.tag( "QtIcon" ).to<QString>();

                    QAction* pluginAction = new QAction( QIcon( pluginActionIcon ), pluginActionName, this );
                    pluginAction->setProperty( "TypeName", QVariant( className ) );
                    pluginAction->setProperty( "Type", QVariant( i->first ) );
                    pluginAction->setProperty( "FunctionName", QVariant( pluginActionName ) );
                    QObject::connect( pluginAction, SIGNAL( triggered() ), pluginActionSignalMapper, SLOT( map() ) );
                    pluginActionSignalMapper->setMapping( pluginAction, pluginAction );
                    mPluginActions[ i->first ].push_back( pluginAction );
                }
            }
        }
        catch( camp::Error e )
        {
            LOGE << e.what();
        }
    }
    QObject::connect( newPluginSignalMapper, SIGNAL( mapped(int) ), this, SLOT( createPlugin(int) ) );
    QObject::connect( pluginActionSignalMapper, SIGNAL( mapped(QObject*) ), this, SLOT( pluginAction(QObject*) ) );

    mPluginMenu = new QMenu( "Create plugin", this );
    mPluginMenu->setIcon( QIcon( ":/Icons/Icons/categories/xfce-system.png" ) );
    mPluginMenu->addActions( actions );
    mPluginMenuAction = mPluginMenu->menuAction();

    PluginTreeView::clear();
}

ServerPlugin& PluginTreeView::getServerPlugin( const QModelIndex& rIndex )
{
    ClientServerPluginTypeEnum type = 
        (ClientServerPluginTypeEnum)rIndex.data( Qt::UserRole + 1 ).toInt();
    return static_cast<ServerPlugin&>( mPluginManager->getPlugin( type ) );
}

void PluginTreeView::load( const QModelIndex& rIndex )
{
    EditorGlobals::mMainWindow->mUI.propertyBrowser->load( 
        PluginTreeView::getServerPlugin( rIndex ) );
    EditorGlobals::mMainWindow->mUI.propertyBrowser->setEnabled( true );
}

void PluginTreeView::selectionChanged( const QItemSelection& rSelected, 
    const QItemSelection& rDeselected )
{
    // Let the tree view handle the selection so the updated selection can be used below.
    QTreeView::selectionChanged( rSelected, rDeselected );

    QModelIndexList selectedIndexes = QTreeView::selectionModel()->selectedIndexes();
    if( !selectedIndexes.size() )
    {
        // No items are selected.
        PluginTreeView::clear();
    }
    else
    {
        // One item is selected, multiple selected items are not possible.
        PluginTreeView::load( selectedIndexes.at( 0 ) );
    }

    QTreeView::selectionChanged( rSelected, rDeselected );
}

void PluginTreeView::showContextMenu( const QPoint& rPoint )
{
    if( EditorGlobals::mOffline )
    {
        QModelIndex index = QTreeView::indexAt( rPoint );
        QMenu contextMenu;

        // Add create new plugin and destroy plugin actions.
        if( index.isValid() )
        {
            contextMenu.addMenu( mPluginMenu );
            contextMenu.addAction( EditorGlobals::mMainWindow->mUI.actionDestroy_plugin );
        }
        else
        {
            contextMenu.addMenu( mPluginMenu );
        }

        // Add plugin specific actions.
        bool addSeperator = true;
        ClientServerPluginTypeEnum type = (ClientServerPluginTypeEnum)index.data( Qt::UserRole + 1 ).toInt();
        for( QList<QAction*>::iterator i = mPluginActions[ type ].begin(); 
            i != mPluginActions[ type ].end(); ++i )
        {
            if( addSeperator )
            {
                contextMenu.addSeparator();
                addSeperator = false;
            }
            contextMenu.addAction( *i );
        }

        contextMenu.exec( QTreeView::mapToGlobal( rPoint ) );
    }
}

void PluginTreeView::search()
{
    QString text = EditorGlobals::mMainWindow->mUI.editPluginSearch->text();
    if( text.isEmpty() )
        PluginTreeView::clear();
    else
        QTreeView::keyboardSearch( text );
}

void PluginTreeView::createPlugin( int pluginType )
{
    try
    {
        if( mPluginManager ) mPluginManager->createPlugin( (ClientServerPluginTypeEnum)pluginType );
    }
    catch( Exception e )
    {
        LOGE << "Could not create plugin: " << e.what();
    }
}

void PluginTreeView::pluginAction( QObject* pObject )
{
    String className = pObject->property( "TypeName" ).toString().toStdString();
    String functionName = pObject->property( "FunctionName" ).toString().toStdString();
    ClientServerPluginTypeEnum type = (ClientServerPluginTypeEnum)pObject->property( "Type" ).toInt();

    try
    {
        camp::classByName( className ).function( functionName ).call( mPluginManager->getPlugin( 
            type ) );
    }
    catch( camp::Error e )
    {
    	LOGE << "Cannot execute function " << functionName << " in class " << className << ": " << 
            e.what();
    }
    catch( Exception e )
    {
        LOGE << "Cannot execute function " << functionName << " in class " << className << ": " << 
            e.what();
    }
}

void PluginTreeView::destroyPlugin()
{
    try
    {
        QModelIndexList selectedIndexes = QTreeView::selectionModel()->selectedIndexes();
        mPluginManager->destroyPlugin( 
            PluginTreeView::getServerPlugin( selectedIndexes.at( 0 ) ).getType() );
    }
    catch( Exception e )
    {
        LOGE << "Could not destroy plugin: " << e.what();
    }
}

void PluginTreeView::focusInEvent( QFocusEvent* pEvent )
{
    // Reload one item into property grid when getting focus.
    QModelIndexList selectedIndexes = PluginTreeView::selectionModel()->selectedIndexes();
    if( selectedIndexes.size() == 1 )
    {
        PluginTreeView::load( selectedIndexes.at( 0 ) );
        pEvent->accept();
    }

    QTreeView::focusInEvent( pEvent );
}

void PluginTreeView::mousePressEvent( QMouseEvent* pEvent )
{
    QModelIndex index = QTreeView::indexAt( pEvent->pos() );

    if( !index.isValid() )
        QTreeView::clearSelection();

    QTreeView::mousePressEvent( pEvent );
}

void PluginTreeView::serverChange( ServerAbstract& rServer, bool created )
{
    // TODO: Support multiple servers.
    if( created )
    {
        mPluginManager = &rServer.getPluginManager(); 
        mModel->setPluginManager( rServer.getPluginManager() );
    }
}

void PluginTreeView::clearActions()
{
    // Creating plugins from the client is not possible yet.
}

void PluginTreeView::clear()
{
    // Clear selection
    QTreeView::clearSelection();

    // Clear property browser
    EditorGlobals::mMainWindow->mUI.propertyBrowser->clear();

    // Clear actions
    PluginTreeView::clearActions();
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia