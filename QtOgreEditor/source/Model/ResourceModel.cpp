/*
--------------------------------------------------------------------------------
This file is part of Diversia engine
http://www.diversia.org/
Copyright (c) 2008-2011 Gabriël Konat
--------------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "Model/ResourceModel.h"
#include "OgreClient/Resource/ResourceManager.h"

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

ResourceItem::ResourceItem( const ResourceInfo& rResourceInfo ):
    QStandardItem(),
    mResourceInfo( rResourceInfo )
{
    QStandardItem::setText( QString( mResourceInfo.mFile.filename().c_str() ) );
    QStandardItem::setData( QVariant( mResourceInfo.mType ), Qt::UserRole + 1 );
    QStandardItem::setEditable( false );
    QStandardItem::setDropEnabled( false );
    QStandardItem::setDragEnabled( true );

    QIcon icon;
    switch( mResourceInfo.mType )
    {
        case RESOURCETYPE_GENERIC: icon = QIcon( ":/Icons/Icons/mimetypes/unknown.png" ); break;
        case RESOURCETYPE_OGREMESH: icon = QIcon( ":/Icons/Icons/apps/blender.png" ); break;
        case RESOURCETYPE_BULLETMESH: icon = QIcon( ":/Icons/Icons/apps/preferences-desktop-launch-feedback.png" ); break;
        case RESOURCETYPE_SKELETON: icon = QIcon( ":/Icons/Icons/apps/preferences-desktop-assistive-technology.png" ); break;
        case RESOURCETYPE_TEXTURE: icon = QIcon( ":/Icons/Icons/apps/gimp.png" ); break;
        case RESOURCETYPE_MATERIALSCRIPT: icon = QIcon( ":/Icons/Icons/apps/access.png" ); break;
        case RESOURCETYPE_PARTICLESCRIPT: icon = QIcon( ":/Icons/Icons/apps/preferences-desktop-default-applications.png" ); break;
        case RESOURCETYPE_LUASCRIPT: icon = QIcon( ":/Icons/Icons/mimetypes/stock_script.png" ); break;
        case RESOURCETYPE_AUDIO: icon = QIcon( ":/Icons/Icons/mimetypes/audio-x-generic.png" ); break;
        case RESOURCETYPE_LEVEL: icon = QIcon( ":/Icons/Icons/status/folder-drag-accept.png" ); break;
    }
    QStandardItem::setIcon( icon );
}

ResourceItem::~ResourceItem()
{

}

//------------------------------------------------------------------------------

ResourceModel::ResourceModel( QObject* pParent /*= 0*/ ):
    QStandardItemModel( pParent ),
    mResourceManager( 0 )
{
    QStringList headers; headers << "Name";
    QStandardItemModel::setHorizontalHeaderLabels( headers );
}

ResourceModel::~ResourceModel()
{

}

void ResourceModel::setResourceManager( ResourceManager& rResourceManager )
{
    mResourceManager = &rResourceManager;
    mResourceManager->connectInitialized( sigc::mem_fun( this, &ResourceModel::resourcesInitialised ) );
}

void ResourceModel::resourcesInitialised( ResourceManager& rResourceManager )
{
    ResourceModel::clear();

    ResourceList resources = mResourceManager->list();
    for( ResourceList::const_iterator i = resources.begin(); i != resources.end(); ++i )
    {
        ResourceItem* item = new ResourceItem( *i );
        QStandardItemModel::appendRow( item );
    }
}

void ResourceModel::clear()
{
    QStandardItemModel::clear();
    mResourceItems.clear();
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia
