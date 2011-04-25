/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "Camp/CampPropertyManager.h"
#include "Client/ClientServerPlugin/ServerPluginManager.h"
#include "Client/Communication/GridManager.h"
#include "Client/Communication/ServerAbstract.h"
#include "Client/Object/ClientComponentTemplate.h"
#include "Client/Object/ClientObjectManager.h"
#include "Client/Object/ClientObjectTemplate.h"
#include "Client/Object/ClientObjectTemplateManager.h"
#include "Model/ObjectComponentTemplateModel.h"
#include "Shared/ClientServerPlugin/ClientServerPlugin.h"
#include "Shared/ClientServerPlugin/ClientServerPluginManager.h"
#include "UI/MainWindow.h"
#include "UI/ObjectTemplateTreeView.h"
#include "UI/PropertyBrowser.h"

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

unsigned int ObjectTemplateTreeView::mObjectCounter = 0;

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

    ObjectTemplateTreeView::clear();
}

bool ObjectTemplateTreeView::isObjectTemplate( const QModelIndex& rIndex )
{
    return ObjectTemplateTreeView::getReplicaType( rIndex ) == REPLICATYPE_OBJECTTEMPLATE;
}

bool ObjectTemplateTreeView::isComponentTemplate( const QModelIndex& rIndex )
{
    return ObjectTemplateTreeView::getReplicaType( rIndex ) == REPLICATYPE_COMPONENTTEMPLATE;
}

ReplicaType ObjectTemplateTreeView::getReplicaType( const QModelIndex& rIndex )
{
    if( rIndex.isValid() ) return (ReplicaType)rIndex.data( Qt::UserRole + 1 ).toInt();
    return 0xFF;
}

ClientObjectTemplate& ObjectTemplateTreeView::getObjectTemplate( const QModelIndex& rIndex )
{
    if( !ObjectTemplateTreeView::isObjectTemplate( rIndex ) )
        DIVERSIA_EXCEPT( Exception::ERR_INVALIDPARAMS, 
        "Given QModelIndex does not represent an object template.", 
        "ObjectTemplateTreeView::getObjectTemplate" );

    String name = rIndex.data( Qt::UserRole + 2 ).toString().toStdString();
    return static_cast<ClientObjectTemplate&>( mObjectTemplateManager->getObjectTemplate( name ) );
}

ClientComponentTemplate& ObjectTemplateTreeView::getComponentTemplate( const QModelIndex& rIndex )
{
    if( !ObjectTemplateTreeView::isComponentTemplate( rIndex ) )
        DIVERSIA_EXCEPT( Exception::ERR_INVALIDPARAMS, 
        "Given QModelIndex does not represent a component template.", 
        "ObjectTemplateTreeView::getComponentTemplate" );

    String objectName = rIndex.data( Qt::UserRole + 2 ).toString().toStdString();
    ObjectTemplate& objectTemplate = mObjectTemplateManager->getObjectTemplate( objectName );
    String componentName = rIndex.data( Qt::UserRole + 3 ).toString().toStdString();
    return static_cast<ClientComponentTemplate&>( objectTemplate.getComponentTemplate( componentName ) );
}

void ObjectTemplateTreeView::load( const QModelIndex& rIndex )
{
    switch( ObjectTemplateTreeView::getReplicaType( rIndex ) )
    {
        case REPLICATYPE_OBJECTTEMPLATE: 
        {
            // Load property browser with object template.
            EditorGlobals::mMainWindow->mUI.propertyBrowser->load( 
                ObjectTemplateTreeView::getObjectTemplate( rIndex ) );
            EditorGlobals::mMainWindow->mUI.propertyBrowser->setEnabled( true );

            // Set actions to toolbar.
            ObjectTemplateTreeView::clearActions();
            EditorGlobals::mMainWindow->mUI.toolBarObjectTemplate->addAction( 
                EditorGlobals::mMainWindow->mUI.actionInstantiate_template );
            EditorGlobals::mMainWindow->mUI.toolBarObjectTemplate->addAction( 
                EditorGlobals::mMainWindow->mUI.actionCreate_child_object_template );
            EditorGlobals::mMainWindow->mUI.toolBarObjectTemplate->addAction( 
                EditorGlobals::mMainWindow->mUI.actionDestroy_object_template );
            // TODO: EditorGlobals::mMainWindow->mUI.toolBarObjectTemplate->addAction( mComponentMenuAction );

            break;
        }
        case REPLICATYPE_COMPONENTTEMPLATE:
        {
            // Load property browser with component template.
            ComponentTemplate& componentTemplate = ObjectTemplateTreeView::getComponentTemplate( 
                rIndex );
            camp::UserObject object = componentTemplate;
            const camp::Class& metaclass = object.getClass();
            const camp::DictionaryProperty& property = static_cast<const camp::DictionaryProperty&>( 
                metaclass.property( "Properties" ) );
            const Properties& properties = componentTemplate.getProperties();
            PropertyBrowser* browser = EditorGlobals::mMainWindow->mUI.propertyBrowser;
            CampPropertyManager* propertyManager = browser->getPropertyManager();

            browser->clear();
            propertyManager->addClass( metaclass, object );
            QtProperty* templateGroup = propertyManager->addGroupProperty( "Template properties" );
            for( Properties::const_iterator i = properties.begin(); i != properties.end(); ++i)
            {
                templateGroup->addSubProperty( propertyManager->addProperty( 
                    new CampValueMapPropertyData( property, object, i->first ) ) );
            }
            browser->addProperty( templateGroup );
            propertyManager->setup( object );
            // TODO: Updates from external sources (script, network) must update the property browser.
            EditorGlobals::mMainWindow->mUI.propertyBrowser->setEnabled( true );
            
            // Set actions to toolbar.
            ObjectTemplateTreeView::clearActions();
            EditorGlobals::mMainWindow->mUI.toolBarObjectTemplate->addAction( 
                EditorGlobals::mMainWindow->mUI.actionDestroy_component_template );

            break;
        }
    }
}

void ObjectTemplateTreeView::selectionChanged( const QItemSelection& rSelected, 
    const QItemSelection& rDeselected )
{
    // Let the tree view handle the selection so the updated selection can be used below.
    QTreeView::selectionChanged( rSelected, rDeselected );

    QModelIndexList selectedIndexes = QTreeView::selectionModel()->selectedIndexes();
    if( !selectedIndexes.size() )
    {
        // No items are selected.
        ObjectTemplateTreeView::clear();
    }
    else if( selectedIndexes.size() > 1 )
    {
        // More than one item is selected.
        EditorGlobals::mMainWindow->mUI.propertyBrowser->clear();
        EditorGlobals::mMainWindow->mUI.propertyBrowser->setEnabled( false );
        ObjectTemplateTreeView::clearActions();
        EditorGlobals::mMainWindow->mUI.toolBarObjectTemplate->addAction( 
            EditorGlobals::mMainWindow->mUI.actionDestroy_selected_templates );
    }
    else 
    {
        // One item is selected.
        ObjectTemplateTreeView::load( selectedIndexes.at( 0 ) );
    }
}

void ObjectTemplateTreeView::showContextMenu( const QPoint& rPoint )
{
    QModelIndex index = QTreeView::indexAt( rPoint );
    QMenu contextMenu;
    if( index.isValid() )
    {
        switch( ObjectTemplateTreeView::getReplicaType( index ) )
        {
            case REPLICATYPE_OBJECTTEMPLATE: 
            {
                contextMenu.addAction( EditorGlobals::mMainWindow->mUI.actionInstantiate_template );
                contextMenu.addAction( EditorGlobals::mMainWindow->mUI.actionCreate_child_object_template );
                contextMenu.addAction( EditorGlobals::mMainWindow->mUI.actionDestroy_object_template );
                // TODO: contextMenu.addMenu( mComponentMenu );
                break;
            }
            case REPLICATYPE_COMPONENTTEMPLATE:
            {
                contextMenu.addAction( EditorGlobals::mMainWindow->mUI.actionDestroy_component_template );
                break;
            }
        }
    }
    else
    {
        contextMenu.addAction( EditorGlobals::mMainWindow->mUI.actionCreate_new_object_template );
    }

    contextMenu.exec( QTreeView::mapToGlobal( rPoint ) );
}

void ObjectTemplateTreeView::search()
{
    QString text = EditorGlobals::mMainWindow->mUI.editObjectTemplateSearch->text();
    if( text.isEmpty() )
        ObjectTemplateTreeView::clear();
    else
        QTreeView::keyboardSearch( text );
}

void ObjectTemplateTreeView::instantiate()
{
    static unsigned int counter = 0;

    try
    {
        // TODO: Use commands to enable undo.
        ObjectTemplateTreeView::getObjectTemplate( QAbstractItemView::currentIndex() ).createObject( 
            *mObjectManager, String( "ObjectFromObjectTemplate" ) + mObjectManager->getGUIDString() 
            + boost::lexical_cast<String>( ++counter ), LOCAL );
    }
    catch( Exception e )
    {
    	LOGE << "Could not instantiate object template: " << e.what();
    }
}

void ObjectTemplateTreeView::createNewObjectTemplate()
{
    try
    {
        // TODO: Use commands to enable undo.
        mObjectTemplateManager->createObjectTemplate( String( "EditorObjectTemplate" ) + 
            mObjectTemplateManager->getGUIDString() +  boost::lexical_cast<String>( 
            ++mObjectCounter ), LOCAL, "EditorObjectTemplate" );
    }
    catch( Exception e )
    {
        LOGE << "Could not create new object template: " << e.what();
    }
}

void ObjectTemplateTreeView::createChildObjectTemplate()
{
    try
    {
        QModelIndex index = QAbstractItemView::currentIndex();
        ObjectTemplate& rootObjectTemplate = ObjectTemplateTreeView::getObjectTemplate( index );

        // TODO: Use commands to enable undo.
        rootObjectTemplate.createChildObjectTemplate( String( "EditorObjectTemplate" ) + 
            mObjectTemplateManager->getGUIDString() + boost::lexical_cast<String>( 
            ++mObjectCounter ), "EditorObjectTemplate" );
    }
    catch( Exception e )
    {
        LOGE << "Could not create child object template: " << e.what();
    }
}

void ObjectTemplateTreeView::createComponentTemplate( int componentType )
{
    // TODO: Create component templates
}

void ObjectTemplateTreeView::destroyObjectTemplate()
{
    QModelIndexList selectedIndexes = QTreeView::selectionModel()->selectedIndexes();
    if( selectedIndexes.size() == 1 )
        ObjectTemplateTreeView::destroyObjectTemplate( selectedIndexes.at( 0 ) );
}

void ObjectTemplateTreeView::destroyComponentTemplate()
{
    QModelIndexList selectedIndexes = QTreeView::selectionModel()->selectedIndexes();
    if( selectedIndexes.size() == 1 )
        ObjectTemplateTreeView::destroyComponentTemplate( selectedIndexes.at( 0 ) );
}

void ObjectTemplateTreeView::destroySelected()
{
    QModelIndexList selectedIndexes = QTreeView::selectionModel()->selectedIndexes();
    for( QModelIndexList::iterator i = selectedIndexes.begin(); i != selectedIndexes.end(); ++i )
    {
        ObjectTemplateTreeView::destroy( *i );
    }
}

void ObjectTemplateTreeView::focusInEvent( QFocusEvent* pEvent )
{
    // Reload one item into property grid when getting focus.
    QModelIndexList selectedIndexes = QTreeView::selectionModel()->selectedIndexes();
    if( selectedIndexes.size() == 1 )
    {
        ObjectTemplateTreeView::load( selectedIndexes.at( 0 ) );
        pEvent->accept();
    }

    QTreeView::focusInEvent( pEvent );
}

void ObjectTemplateTreeView::destroy( const QModelIndex& rIndex )
{
    switch( ObjectTemplateTreeView::getReplicaType( rIndex ) )
    {
        case REPLICATYPE_OBJECTTEMPLATE: ObjectTemplateTreeView::destroyObjectTemplate( rIndex ); break;
        case REPLICATYPE_COMPONENTTEMPLATE: ObjectTemplateTreeView::destroyComponentTemplate( rIndex ); break;
    }
}

void ObjectTemplateTreeView::destroyObjectTemplate( const QModelIndex& rIndex )
{
    try
    {
        mObjectTemplateManager->destroyObjectTemplateTree( ObjectTemplateTreeView::getObjectTemplate( rIndex ) );
    }
    catch( Exception e )
    {
        LOGE << "Could not destroy object template(s): " << e.what();
    }
}

void ObjectTemplateTreeView::destroyComponentTemplate( const QModelIndex& rIndex )
{
    try
    {
        ObjectTemplateTreeView::getComponentTemplate( rIndex ).destroyComponentTemplate();
    }
    catch( Exception e )
    {
        LOGE << "Could not destroy component template: " << e.what();
    }
}

void ObjectTemplateTreeView::mousePressEvent( QMouseEvent* pEvent )
{
    QModelIndex index = QTreeView::indexAt( pEvent->pos() );

    if( !index.isValid() )
        QTreeView::clearSelection();

    QTreeView::mousePressEvent( pEvent );
}

void ObjectTemplateTreeView::dropEvent( QDropEvent* pEvent )
{
    // Stop the abstract item view from rearranging items, the model will do this!
    try
    {
        QAbstractItemView::dropEvent( pEvent );
    }
    catch( Exception e )
    {
        LOGE << "Could not change parent object template: " << e.what();
    }

    pEvent->setDropAction( Qt::IgnoreAction );
    pEvent->accept();
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
    switch( rPlugin.getType() ) 
    {
        case CLIENTSERVERPLUGINTYPE_OBJECTMANAGER:
            mObjectManager = &static_cast<ClientObjectManager&>( rPlugin );
            break;
        case CLIENTSERVERPLUGINTYPE_OBJECTTEMPLATEMANAGER:
            mObjectTemplateManager = &static_cast<ClientObjectTemplateManager&>( rPlugin );
            mModel->setObjectTemplateManager( *mObjectTemplateManager );
            break;
    }
}

void ObjectTemplateTreeView::clearActions()
{
    EditorGlobals::mMainWindow->mUI.toolBarObjectTemplate->removeAction( 
        EditorGlobals::mMainWindow->mUI.actionInstantiate_template );
    EditorGlobals::mMainWindow->mUI.toolBarObjectTemplate->removeAction( 
        EditorGlobals::mMainWindow->mUI.actionCreate_child_object_template );
    EditorGlobals::mMainWindow->mUI.toolBarObjectTemplate->removeAction( 
        EditorGlobals::mMainWindow->mUI.actionDestroy_object_template );
    // TODO: EditorGlobals::mMainWindow->mUI.toolBarObjectTemplate->removeAction( mComponentMenuAction );
    EditorGlobals::mMainWindow->mUI.toolBarObjectTemplate->removeAction( 
        EditorGlobals::mMainWindow->mUI.actionDestroy_component_template );
    EditorGlobals::mMainWindow->mUI.toolBarObjectTemplate->removeAction( 
        EditorGlobals::mMainWindow->mUI.actionDestroy_selected_templates );
    EditorGlobals::mMainWindow->mUI.toolBarObjectTemplate->removeAction( 
        EditorGlobals::mMainWindow->mUI.actionCreate_new_object_template );
}

void ObjectTemplateTreeView::clear()
{
    // Clear selection
    QTreeView::clearSelection();

    // Clear property browser
    EditorGlobals::mMainWindow->mUI.propertyBrowser->clear();
    EditorGlobals::mMainWindow->mUI.propertyBrowser->setEnabled( false );

    // Clear actions
    ObjectTemplateTreeView::clearActions();

    EditorGlobals::mMainWindow->mUI.toolBarObjectTemplate->addAction( 
        EditorGlobals::mMainWindow->mUI.actionCreate_new_object_template );
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia