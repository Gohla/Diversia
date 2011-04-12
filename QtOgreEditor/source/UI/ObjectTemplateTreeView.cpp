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
#include "Client/Object/ClientObjectTemplateManager.h"
#include "Model/ObjectComponentTemplateModel.h"
#include "Shared/ClientServerPlugin/ClientServerPlugin.h"
#include "Shared/ClientServerPlugin/ClientServerPluginManager.h"
#include "UI/ObjectTemplateTreeView.h"

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

ObjectTemplateTreeView::ObjectTemplateTreeView( QWidget* pParent ):
    QTreeView( pParent ),
    mModel( new ObjectComponentTemplateModel( this ) )
{
    QTreeView::setDragDropMode( QTreeView::InternalMove );
    QTreeView::setSelectionMode( QAbstractItemView::ExtendedSelection );
    QTreeView::setModel( mModel );
    QWidget::setContextMenuPolicy( Qt::CustomContextMenu );
}

ObjectTemplateTreeView::~ObjectTemplateTreeView()
{

}

void ObjectTemplateTreeView::init()
{
    EditorGlobals::mGrid->connectServerChange( sigc::mem_fun( this, 
        &ObjectTemplateTreeView::serverChange ) );

    //ObjectTreeView::clear();
}

void ObjectTemplateTreeView::serverChange( ServerAbstract& rServer, bool created )
{
    // TODO: Support multiple servers.
    if( created )
        rServer.getPluginManager().connect( sigc::mem_fun( this, 
        &ObjectTemplateTreeView::pluginChange ) );
}

void ObjectTemplateTreeView::pluginChange( ClientServerPlugin& rPlugin, bool created )
{
    if( rPlugin.getType() == CLIENTSERVERPLUGINTYPE_OBJECTTEMPLATEMANAGER )
    {
        mObjectTemplateManager = &static_cast<ClientObjectTemplateManager&>( rPlugin );
        mModel->setObjectTemplateManager( *mObjectTemplateManager );
    }
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia