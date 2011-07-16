/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "Model/ObjectComponentModel.h"
#include "Object/Object.h"
#include "Object/ObjectManager.h"
#include "UI/MainWindow.h"
#include "UI/ObjectTreeView.h"
#include <sigc++/adaptors/hide.h>

namespace Diversia
{
namespace QtOgreEditor 
{

//------------------------------------------------------------------------------

ComponentItem::ComponentItem( Component& rComponent ):
    QStandardItem(),
    mComponent( rComponent )
{
    QStandardItem::setText( QString( rComponent.getName().c_str() ) );
    QStandardItem::setData( QVariant( REPLICATYPE_COMPONENT ), Qt::UserRole + 1 );
    QStandardItem::setData( QVariant( QString( rComponent.getObject().getName().c_str() ) ), 
        Qt::UserRole + 2 );
    QStandardItem::setData( QVariant( QString( rComponent.getName().c_str() ) ), Qt::UserRole + 3 );
    QStandardItem::setEditable( false );
    QStandardItem::setDropEnabled( false );
    QStandardItem::setDragEnabled( false );

    try
    {
        QStandardItem::setIcon( QIcon( QString( camp::classByName( rComponent.getTypeName() ).tag( 
            "QtIcon" ).to<String>().c_str() ) ) );
    }
    catch( camp::Error e )
    {
        LOGE << e.what();
    }

    rComponent.connectDestruction( sigc::hide( sigc::mem_fun( this, &ComponentItem::destroyed ) ) );
}

ComponentItem::~ComponentItem()
{

}

void ComponentItem::destroyed()
{
    QStandardItem* parentItem = QStandardItem::parent();
    if( parentItem )
        parentItem->removeRow( QStandardItem::row() );
    else
    {
        static_cast<ObjectComponentModel*>( QStandardItem::model() )->removeRow( 
            QStandardItem::row() );
    }
}

//------------------------------------------------------------------------------

ObjectItem::ObjectItem( Object& rObject ):
    QStandardItem(),
    mObject( rObject )
{
    QStandardItem::setText( QString( rObject.getDisplayName().c_str() ) );
    QStandardItem::setData( QVariant( REPLICATYPE_OBJECT ), Qt::UserRole + 1 );
    QStandardItem::setData( QVariant( QString( rObject.getName().c_str() ) ), Qt::UserRole + 2 );
    QStandardItem::setIcon( QIcon( ":/Icons/Icons/mimetypes/gnome-package.png" ) );
    QStandardItem::setEditable( true );
    QStandardItem::setDropEnabled( true );
    QStandardItem::setDragEnabled( true );

    rObject.connectComponentChange( sigc::mem_fun( this, &ObjectItem::componentChange ) );
    rObject.connectParentChange( sigc::mem_fun( this, &ObjectItem::parentChange ) );
    rObject.connectDestruction( sigc::hide( sigc::mem_fun( this, &ObjectItem::destroyed ) ) );
    rObject.connectDisplayNameChange( sigc::mem_fun( this, &ObjectItem::displayNameChanged ) );
}

ObjectItem::~ObjectItem()
{

}

void ObjectItem::setData( const QVariant& rValue, int role /*= Qt::UserRole + 1*/ )
{
    QStandardItem::setData( rValue, role );

    if( role == Qt::EditRole )
    {
        mObject.setDisplayName( QStandardItem::data( Qt::EditRole ).toString().toStdString() );
    }
}

void ObjectItem::setParent( Object* pParent )
{
    mObject.parent( pParent );
}

void ObjectItem::setParent( ObjectItem* pParent )
{
    mObject.parent( &pParent->getObject() );
}

void ObjectItem::unparent()
{
    mObject.parent( 0 );
}

void ObjectItem::componentChange( Component& rComponent, bool created )
{
    if( created )
    {
        QStandardItem::appendRow( new ComponentItem( rComponent ) );
        EditorGlobals::mMainWindow->mUI.treeViewObjects->checkHiddenItems();
    }
}

void ObjectItem::parentChange( Object* pNewParent )
{
    ObjectComponentModel* model = static_cast<ObjectComponentModel*>( QStandardItem::model() );

    if( pNewParent )
    {
        DivAssert( model, "No model was set for object item." );

        // Add item to new parent item and remove item from old parent item.
        try
        {
            QStandardItem* parentItem = QStandardItem::parent();
            if( parentItem )
                parentItem->takeRow( QStandardItem::row() );
            else
                model->takeRow( QStandardItem::row() );

            model->getObjectItem( pNewParent->getName() ).appendRow( this );  
        }
        catch( Exception e )
        {
        	DivAssert( 0, "Parent not found in model for existing object item." );
        }
    }
    else
    {
        // Add object item to root item and remove item from old parent item.
        QStandardItem* parentItem = QStandardItem::parent();
        if( parentItem )
            parentItem->takeRow( QStandardItem::row() );
        else
            model->takeRow( QStandardItem::row() );

        model->appendRow( this );  
    }
}

void ObjectItem::displayNameChanged( const String& rDisplayName )
{
    QString displayName( rDisplayName.c_str() );
    if( QStandardItem::text() != displayName )
        QStandardItem::setText( displayName );
}

void ObjectItem::destroyed()
{
    QStandardItem* parentItem = QStandardItem::parent();
    if( parentItem )
        parentItem->removeRow( QStandardItem::row() );
    else
    {
        static_cast<ObjectComponentModel*>( QStandardItem::model() )->removeRow( 
            QStandardItem::row() );
    }
}

//------------------------------------------------------------------------------

ObjectComponentModel::ObjectComponentModel( QObject* pParent /*= 0*/ ):
    QStandardItemModel( pParent )
{
    QStringList headers; headers << "Name";
    QStandardItemModel::setHorizontalHeaderLabels( headers );
    QStandardItemModel::setSupportedDragActions( Qt::DropActions( Qt::MoveAction ) );

    // Supported mime types
    mMimeTypes << QLatin1String( "diversia/x-object" ) << QLatin1String( "diversia/x-component" );
}

ObjectComponentModel::~ObjectComponentModel()
{

}

void ObjectComponentModel::setObjectManager( ObjectManager& rObjectManager )
{
    mObjectManager = &rObjectManager;
    mObjectManager->connect( sigc::mem_fun( this, &ObjectComponentModel::objectChange ) );
}

ObjectItem& ObjectComponentModel::getObjectItem( const String& rObjectName ) const
{
    ObjectItems::const_iterator i = mObjectItems.find( rObjectName );
    if( i != mObjectItems.end() )
    {
        return *i->second;
    }
    else
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Object item not found in model", 
            "ObjectComponentModel::getObjectItem" ); 
}

QModelIndex ObjectComponentModel::getObjectIndex( Object& rObject ) const
{
    return ObjectComponentModel::getObjectIndex( rObject.getName() );
}

QModelIndex ObjectComponentModel::getObjectIndex( const String& rObjectName ) const
{
    return ObjectComponentModel::getObjectItem( rObjectName ).index();
}

void ObjectComponentModel::objectChange( Object& rObject, bool created )
{
    if( created )
    {
        ObjectItem* item = new ObjectItem( rObject );
        mObjectItems.insert( std::make_pair( rObject.getName(), item ) );

        if( rObject.getParentObject() )
        {
            try
            {
                ObjectComponentModel::getObjectItem( 
                    rObject.getParentObject()->getName() ).appendRow( item );
                EditorGlobals::mMainWindow->mUI.treeViewObjects->checkHiddenItems();
            }
            catch( Exception e )
            {
            	DivAssert( 0, "Parent not found in model for new object item." );
            }
        }
        else
        {
            QStandardItemModel::appendRow( item );
            EditorGlobals::mMainWindow->mUI.treeViewObjects->checkHiddenItems();
        }
    }
    else
    {
        mObjectItems.erase( rObject.getName() );
    }
}

QStringList ObjectComponentModel::mimeTypes() const
{
    return mMimeTypes;
}

QMimeData* ObjectComponentModel::mimeData( const QModelIndexList& rIndexes ) const
{
    if( !rIndexes.size() ) return 0;

    QSet<ObjectItem*> objects;
    QSet<ComponentItem*> components;

    for( QModelIndexList::const_iterator i = rIndexes.begin(); i != rIndexes.end(); ++i )
    {
        QStandardItem* item = QStandardItemModel::itemFromIndex( *i );
        int type = (*i).data( Qt::UserRole + 1 ).toInt();

        switch( type )
        {
            case REPLICATYPE_OBJECT: 
            {
                objects.insert( static_cast<ObjectItem*>( item ) );
                break;
            }
            case REPLICATYPE_COMPONENT:
            {
                components.insert( static_cast<ComponentItem*>( item )  );
                break;
            }
        }
    }

    if( !objects.size() && !components.size() ) return 0;

    QMimeData* data = new QMimeData();
    QByteArray objectsEncoded;
    QDataStream objectsStream( &objectsEncoded, QIODevice::WriteOnly );
    QByteArray componentsEncoded;
    QDataStream componentsStream( &componentsEncoded, QIODevice::WriteOnly );

    objectsStream << objects.size();
    for( QSet<ObjectItem*>::iterator i = objects.begin(); i != objects.end(); ++i )
    {
        objectsStream << QString( (*i)->getObject().getName().c_str() );
    }

    componentsStream << components.size();
    for( QSet<ComponentItem*>::iterator i = components.begin(); i != components.end(); ++i )
    {
        componentsStream << QString( (*i)->getComponent().getName().c_str() ) <<
            QString( (*i)->getOwnerObject().getName().c_str() );
    }

    if( objects.size() ) data->setData( QLatin1String( "diversia/x-object" ), objectsEncoded );
    if( components.size() ) data->setData( QLatin1String( "diversia/x-component" ), 
        componentsEncoded );
    return data;
}

bool ObjectComponentModel::dropMimeData( const QMimeData* pData, Qt::DropAction action, int row, 
    int column, const QModelIndex& rParent )
{
    if( !pData->hasFormat( "diversia/x-object" ) ) return false;

    QByteArray data = pData->data( QLatin1String( "diversia/x-object" ) );
    QDataStream stream( &data, QIODevice::ReadOnly );

    // Iterate over all objects that are being dropped.
    int objectsSize; stream >> objectsSize;
    for( int i = 0; i < objectsSize; ++i )
    {
        QString sourceName;
        stream >> sourceName;

        QModelIndex destination = rParent;
        if( destination.isValid() && 
            destination.data( Qt::UserRole + 1 ).toInt() == REPLICATYPE_OBJECT )
        {
            // Dropping on another object.
            QString destinationName = destination.data( Qt::UserRole + 2 ).toString();
            
            ObjectItems::iterator sourceIter = mObjectItems.find( sourceName.toStdString() );
            ObjectItems::iterator destinationIter = mObjectItems.find( 
                destinationName.toStdString() );

            if( sourceIter != mObjectItems.end() && destinationIter != mObjectItems.end() )
            {
                sourceIter->second->setParent( destinationIter->second );
            }
        }
        else if( !destination.isValid() )
        {
            // Dropping on root (unparenting).
            ObjectItems::iterator sourceIter = mObjectItems.find( sourceName.toStdString() );
            if( sourceIter != mObjectItems.end() )
            {
                sourceIter->second->unparent();
            }
        }
    }

    return true;
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia