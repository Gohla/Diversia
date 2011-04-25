/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "Model/ObjectComponentTemplateModel.h"
#include "Object/ObjectTemplate.h"
#include "Object/ObjectTemplateManager.h"
#include <sigc++/adaptors/hide.h>

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

ComponentTemplateItem::ComponentTemplateItem( ComponentTemplate& rComponentTemplate ):
    mComponentTemplate( rComponentTemplate )
{
    QStandardItem::setText( QString( rComponentTemplate.getName().c_str() ) );
    QStandardItem::setData( QVariant( REPLICATYPE_COMPONENTTEMPLATE ), Qt::UserRole + 1 );
    QStandardItem::setData( QVariant( QString( rComponentTemplate.getObjectTemplate().getName().c_str() ) ), 
        Qt::UserRole + 2 );
    QStandardItem::setData( QVariant( QString( rComponentTemplate.getName().c_str() ) ), Qt::UserRole + 3 );
    QStandardItem::setEditable( false );
    QStandardItem::setDropEnabled( false );
    QStandardItem::setDragEnabled( false );

    try
    {
        QStandardItem::setIcon( QIcon( QString( camp::classByName( rComponentTemplate.getTypeName() ).tag( 
            "QtIcon" ).to<String>().c_str() ) ) );
    }
    catch( camp::Error e )
    {
        LOGE << e.what();
    }

    rComponentTemplate.connectDestruction( sigc::hide( sigc::mem_fun( this, &ComponentTemplateItem::destroyed ) ) );
}

ComponentTemplateItem::~ComponentTemplateItem()
{

}

void ComponentTemplateItem::destroyed()
{
    QStandardItem* parentItem = QStandardItem::parent();
    if( parentItem )
        parentItem->removeRow( QStandardItem::row() );
    else
    {
        static_cast<ObjectComponentTemplateModel*>( QStandardItem::model() )->removeRow( 
            QStandardItem::row() );
    }
}

//------------------------------------------------------------------------------

ObjectTemplateItem::ObjectTemplateItem( ObjectTemplate& rObjectTemplate ):
    mObjectTemplate( rObjectTemplate )
{
    QStandardItem::setText( QString( rObjectTemplate.getDisplayName().c_str() ) );
    QStandardItem::setData( QVariant( REPLICATYPE_OBJECTTEMPLATE ), Qt::UserRole + 1 );
    QStandardItem::setData( QVariant( QString( rObjectTemplate.getName().c_str() ) ), Qt::UserRole + 2 );
    QStandardItem::setIcon( QIcon( ":/Icons/Icons/mimetypes/gnome-package.png" ) );
    QStandardItem::setEditable( true );
    QStandardItem::setDropEnabled( true );
    QStandardItem::setDragEnabled( true );

    rObjectTemplate.connectComponentTemplateChange( sigc::mem_fun( this, &ObjectTemplateItem::componentChange ) );
    rObjectTemplate.connectParentChange( sigc::mem_fun( this, &ObjectTemplateItem::parentChange ) );
    rObjectTemplate.connectDestruction( sigc::hide( sigc::mem_fun( this, &ObjectTemplateItem::destroyed ) ) );
    rObjectTemplate.connectDisplayNameChange( sigc::mem_fun( this, &ObjectTemplateItem::displayNameChanged ) );
}

ObjectTemplateItem::~ObjectTemplateItem()
{

}

void ObjectTemplateItem::setData( const QVariant& rValue, int role /*= Qt::UserRole + 1*/ )
{
    QStandardItem::setData( rValue, role );

    if( role == Qt::EditRole )
    {
        mObjectTemplate.setDisplayName( QStandardItem::data( Qt::EditRole ).toString().toStdString() );
    }
}

void ObjectTemplateItem::setParent( ObjectTemplate* pParent )
{
    mObjectTemplate.parent( pParent );
}

void ObjectTemplateItem::setParent( ObjectTemplateItem* pParent )
{
    mObjectTemplate.parent( &pParent->getObjectTemplate() );
}

void ObjectTemplateItem::unparent()
{
    mObjectTemplate.parent( 0 );
}

void ObjectTemplateItem::componentChange( ComponentTemplate& rComponentTemplate, bool created )
{
    if( created )
        QStandardItem::appendRow( new ComponentTemplateItem( rComponentTemplate ) );
}

void ObjectTemplateItem::parentChange( ObjectTemplate* pNewParent )
{
    ObjectComponentTemplateModel* model = static_cast<ObjectComponentTemplateModel*>( QStandardItem::model() );

    if( pNewParent )
    {
        DivAssert( model, "No model was set for object template item." );

        // Add item to new parent item and remove item from old parent item.
        try
        {
            QStandardItem* parentItem = QStandardItem::parent();
            if( parentItem )
                parentItem->takeRow( QStandardItem::row() );
            else
                model->takeRow( QStandardItem::row() );

            model->getObjectTemplateItem( pNewParent->getName() ).appendRow( this );  
        }
        catch( Exception e )
        {
        	DivAssert( 0, "Parent not found in model for existing object template item." );
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

void ObjectTemplateItem::displayNameChanged( const String& rDisplayName )
{
    QString displayName( rDisplayName.c_str() );
    if( QStandardItem::text() != displayName )
        QStandardItem::setText( displayName );
}

void ObjectTemplateItem::destroyed()
{
    QStandardItem* parentItem = QStandardItem::parent();
    if( parentItem )
        parentItem->removeRow( QStandardItem::row() );
    else
    {
        static_cast<ObjectComponentTemplateModel*>( QStandardItem::model() )->removeRow( 
            QStandardItem::row() );
    }
}

//------------------------------------------------------------------------------

ObjectComponentTemplateModel::ObjectComponentTemplateModel( QObject* pParent /*= 0*/ ):
    QStandardItemModel( pParent )
{
    QStringList headers; headers << "Name";
    QStandardItemModel::setHorizontalHeaderLabels( headers );
    QStandardItemModel::setSupportedDragActions( Qt::DropActions( Qt::MoveAction ) );

    // Supported mime types
    mMimeTypes << QLatin1String( "diversia/x-objecttemplate" ) << 
        QLatin1String( "diversia/x-componenttemplate" );
}

ObjectComponentTemplateModel::~ObjectComponentTemplateModel()
{

}

void ObjectComponentTemplateModel::setObjectTemplateManager( ObjectTemplateManager& rObjectTemplateManager )
{
    mObjectTemplateManager = &rObjectTemplateManager;
    mObjectTemplateManager->connect( sigc::mem_fun( this, &ObjectComponentTemplateModel::objectChange ) );
}

ObjectTemplateItem& ObjectComponentTemplateModel::getObjectTemplateItem( const String& rObjectTemplateName ) const
{
    ObjectTemplateItems::const_iterator i = mObjectTemplateItems.find( rObjectTemplateName );
    if( i != mObjectTemplateItems.end() )
    {
        return *i->second;
    }
    else
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Object template item not found in model", 
            "ObjectComponentTemplateModel::getObjectTemplateItem" ); 
}

QModelIndex ObjectComponentTemplateModel::getObjectTemplateIndex( ObjectTemplate& rObjectTemplate ) const
{
    return ObjectComponentTemplateModel::getObjectTemplateIndex( rObjectTemplate.getName() );
}

QModelIndex ObjectComponentTemplateModel::getObjectTemplateIndex( const String& rObjectTemplateName ) const
{
    return ObjectComponentTemplateModel::getObjectTemplateItem( rObjectTemplateName ).index();
}

void ObjectComponentTemplateModel::objectChange( ObjectTemplate& rObjectTemplate, bool created )
{
    if( created )
    {
        ObjectTemplateItem* item = new ObjectTemplateItem( rObjectTemplate );
        mObjectTemplateItems.insert( std::make_pair( rObjectTemplate.getName(), item ) );

        if( rObjectTemplate.getParentObjectTemplate() )
        {
            try
            {
                ObjectComponentTemplateModel::getObjectTemplateItem( 
                    rObjectTemplate.getParentObjectTemplate()->getName() ).appendRow( item );
            }
            catch( Exception e )
            {
            	DivAssert( 0, "Parent not found in model for new object template item." );
            }
        }
        else
        {
            QStandardItemModel::appendRow( item );
        }
    }
    else
    {
        mObjectTemplateItems.erase( rObjectTemplate.getName() );
    }
}

QStringList ObjectComponentTemplateModel::mimeTypes() const
{
    return mMimeTypes;
}

QMimeData* ObjectComponentTemplateModel::mimeData( const QModelIndexList& rIndexes ) const
{
    if( !rIndexes.size() ) return 0;

    QSet<ObjectTemplateItem*> objects;
    QSet<ComponentTemplateItem*> components;

    for( QModelIndexList::const_iterator i = rIndexes.begin(); i != rIndexes.end(); ++i )
    {
        QStandardItem* item = QStandardItemModel::itemFromIndex( *i );
        int type = (*i).data( Qt::UserRole + 1 ).toInt();

        switch( type )
        {
            case REPLICATYPE_OBJECT: 
            {
                objects.insert( static_cast<ObjectTemplateItem*>( item ) );
                break;
            }
            case REPLICATYPE_COMPONENT:
            {
                components.insert( static_cast<ComponentTemplateItem*>( item )  );
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
    for( QSet<ObjectTemplateItem*>::iterator i = objects.begin(); i != objects.end(); ++i )
    {
        objectsStream << QString( (*i)->getObjectTemplate().getName().c_str() );
    }

    componentsStream << components.size();
    for( QSet<ComponentTemplateItem*>::iterator i = components.begin(); i != components.end(); ++i )
    {
        componentsStream << QString( (*i)->getComponentTemplate().getName().c_str() ) <<
            QString( (*i)->getOwnerObjectTemplate().getName().c_str() );
    }

    if( objects.size() ) data->setData( QLatin1String( "diversia/x-objecttemplate" ), objectsEncoded );
    if( components.size() ) data->setData( QLatin1String( "diversia/x-componenttemplate" ), 
        componentsEncoded );
    return data;
}

bool ObjectComponentTemplateModel::dropMimeData( const QMimeData* pData, Qt::DropAction action, int row, 
    int column, const QModelIndex& rParent )
{
    if( !pData->hasFormat( "diversia/x-objecttemplate" ) ) return false;

    QByteArray data = pData->data( QLatin1String( "diversia/x-objecttemplate" ) );
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
            
            ObjectTemplateItems::iterator sourceIter = mObjectTemplateItems.find( sourceName.toStdString() );
            ObjectTemplateItems::iterator destinationIter = mObjectTemplateItems.find( 
                destinationName.toStdString() );

            if( sourceIter != mObjectTemplateItems.end() && destinationIter != mObjectTemplateItems.end() )
            {
                sourceIter->second->setParent( destinationIter->second );
            }
        }
        else if( !destination.isValid() )
        {
            // Dropping on root (unparenting).
            ObjectTemplateItems::iterator sourceIter = mObjectTemplateItems.find( sourceName.toStdString() );
            if( sourceIter != mObjectTemplateItems.end() )
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