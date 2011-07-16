/*
--------------------------------------------------------------------------------
This file is part of Diversia engine
http://www.diversia.org/
Copyright (c) 2008-2011 Gabriël Konat
--------------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "Client/Communication/GridManager.h"
#include "Client/Communication/ServerAbstract.h"
#include "Client/Plugin/ClientPluginManager.h"
#include "Model/ResourceModel.h"
#include "OgreClient/Resource/ResourceManager.h"
#include "Shared/Plugin/Plugin.h"
#include "UI/ResourceTreeView.h"

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

ResourceTreeView::ResourceTreeView( QWidget* pParent /*= 0*/ ):
    QTreeView( pParent )
{
    mModel = new ResourceModel( this );

    QTreeView::setModel( mModel );
    QTreeView::setSelectionMode( QAbstractItemView::SingleSelection );
}

ResourceTreeView::~ResourceTreeView()
{

}

void ResourceTreeView::init()
{
    EditorGlobals::mGrid->connectServerChange( sigc::mem_fun( this, 
        &ResourceTreeView::serverChange ) );
}

void ResourceTreeView::serverChange( ServerAbstract& rServer, bool created )
{
    // TODO: Support multiple servers.
    if( created )
        rServer.getPluginManager().connect( sigc::mem_fun( this, 
        &ResourceTreeView::pluginChange ) );
}

void ResourceTreeView::pluginChange( Plugin& rPlugin, bool created )
{
    switch( rPlugin.getType() ) 
    {
        case PLUGINTYPE_RESOURCEMANAGER:
            mModel->setResourceManager( static_cast<ResourceManager&>( rPlugin ) );
            break;
    }
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia
