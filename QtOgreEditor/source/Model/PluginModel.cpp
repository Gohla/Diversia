/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include <sigc++/adaptors/hide.h>

#include "Model/PluginModel.h"
#include "Client/Plugin/ClientPlugin.h"
#include "Client/Plugin/ClientPluginManager.h"

namespace Diversia
{
namespace QtOgreEditor 
{

//------------------------------------------------------------------------------

PluginItem::PluginItem( Plugin& rPlugin ):
    QStandardItem(),
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

PluginModel::PluginModel( QObject* pParent /*= 0 */ ):
    QStandardItemModel( pParent ),
    mPluginManager( 0 )
{
    QStringList headers; headers << "Name";
    QStandardItemModel::setHorizontalHeaderLabels( headers );
}

PluginModel::~PluginModel()
{

}

void PluginModel::setPluginManager( ClientPluginManager& rPluginManager )
{
    mPluginManager = &rPluginManager;
    mPluginManager->connect( sigc::mem_fun( this, &PluginModel::pluginChange ) );

    // Add existing plugins to model.
    const Plugins& plugins = mPluginManager->getPlugins();
    for( Plugins::const_iterator i = plugins.begin(); i != plugins.end(); ++i )
    {
        Plugin& plugin = mPluginManager->getPlugin( i->first );
        PluginItem* item = new PluginItem( plugin );
        mPluginItems.insert( std::make_pair( plugin.getType(), item ) );
        QStandardItemModel::appendRow( item );
    }
}

PluginItem& PluginModel::getPluginItem( PluginTypeEnum type ) const
{
    PluginItems::const_iterator i = mPluginItems.find( type );
    if( i != mPluginItems.end() )
    {
        return *i->second;
    }
    else
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Plugin item not found in model", 
        "PluginModel::getPluginItem" ); 
}

QModelIndex PluginModel::getPluginIndex( Plugin& rPlugin ) const
{
    return PluginModel::getPluginIndex( rPlugin.getType() );
}

QModelIndex PluginModel::getPluginIndex( PluginTypeEnum type ) const
{
    return PluginModel::getPluginItem( type ).index();
}

void PluginModel::pluginChange( Plugin& rPlugin, bool created )
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