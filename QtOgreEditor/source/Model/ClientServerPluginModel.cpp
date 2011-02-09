/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include <sigc++/adaptors/hide.h>

#include "Model/ClientServerPluginModel.h"
#include "Client/ClientServerPlugin/ServerPlugin.h"
#include "Client/ClientServerPlugin/ServerPluginManager.h"

namespace Diversia
{
namespace QtOgreEditor 
{

//------------------------------------------------------------------------------

PluginItem::PluginItem( ClientServerPlugin& rPlugin ):
    mPlugin( rPlugin )
{
    QStandardItem::setText( QString( rPlugin.getTypeName().c_str() ) );
    QStandardItem::setData( QVariant( rPlugin.getType() ), Qt::UserRole + 1 );
    QStandardItem::setEditable( false );
    QStandardItem::setDropEnabled( false );
    QStandardItem::setDragEnabled( false );

    try
    {
        QStandardItem::setIcon( QIcon( QString( camp::classByName( rPlugin.getTypeName() ).tag( 
            "QtIcon" ).to<String>().c_str() ) ) );
    }
    catch( camp::Error e )
    {
    	LOGE << e.what();
    }
}

PluginItem::~PluginItem()
{

}

//------------------------------------------------------------------------------

ClientServerPluginModel::ClientServerPluginModel( QObject* pParent /*= 0 */ ):
    QStandardItemModel( pParent )
{
    QStringList headers; headers << "Name";
    QStandardItemModel::setHorizontalHeaderLabels( headers );
}

ClientServerPluginModel::~ClientServerPluginModel()
{

}

void ClientServerPluginModel::setPluginManager( ServerPluginManager& rPluginManager )
{
    mPluginManager = &rPluginManager;
    mPluginManager->connect( sigc::mem_fun( this, &ClientServerPluginModel::pluginChange ) );

    // Add existing plugins to model.
    const ClientServerPlugins& plugins = mPluginManager->getPlugins();
    for( ClientServerPlugins::const_iterator i = plugins.begin(); i != plugins.end(); ++i )
    {
        ClientServerPlugin& plugin = mPluginManager->getPlugin( i->first );
        PluginItem* item = new PluginItem( plugin );
        mPluginItems.insert( std::make_pair( plugin.getType(), item ) );
        QStandardItemModel::appendRow( item );
    }
}

PluginItem& ClientServerPluginModel::getPluginItem( ClientServerPluginTypeEnum type ) const
{
    PluginItems::const_iterator i = mPluginItems.find( type );
    if( i != mPluginItems.end() )
    {
        return *i->second;
    }
    else
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Plugin item not found in model", 
        "ClientServerPluginModel::getPluginItem" ); 
}

QModelIndex ClientServerPluginModel::getPluginIndex( ClientServerPlugin& rPlugin ) const
{
    return ClientServerPluginModel::getPluginIndex( rPlugin.getType() );
}

QModelIndex ClientServerPluginModel::getPluginIndex( ClientServerPluginTypeEnum type ) const
{
    return ClientServerPluginModel::getPluginItem( type ).index();
}

void ClientServerPluginModel::pluginChange( ClientServerPlugin& rPlugin, bool created )
{
    if( created )
    {
        PluginItem* item = new PluginItem( rPlugin );
        mPluginItems.insert( std::make_pair( rPlugin.getType(), item ) );
        QStandardItemModel::appendRow( item );
    }
    else
    {
        PluginItems::iterator i = mPluginItems.find( rPlugin.getType() );
        if( i != mPluginItems.end() )
        {
            QStandardItemModel::removeRow( i->second->row() );
            mPluginItems.erase( i );
        }
    }
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia