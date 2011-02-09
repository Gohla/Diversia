/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_OBJECTCOMPONENTMODEL_H
#define DIVERSIA_QTOGREEDITOR_OBJECTCOMPONENTMODEL_H

#include "Platform/Prerequisites.h"

#include "Object/Component.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

/**
Qt model item that represents a component in the Diversia object/component system.
**/
class ComponentItem : public QStandardItem, public sigc::trackable
{
public:
    ComponentItem( Component& rComponent );
    ~ComponentItem();

    inline Component& getComponent() { return mComponent; }
    inline Object& getOwnerObject() { return mComponent.getObject(); }
    inline int type() const { return QStandardItem::UserType + 1; }

private:
    void destroyed();

    Component& mComponent;

};

//------------------------------------------------------------------------------

/**
Qt model item that represents an object in the Diversia object/component system.
**/
class ObjectItem : public QStandardItem, public sigc::trackable
{
public:
    ObjectItem( Object& rObject );
    ~ObjectItem();

    void setData( const QVariant& rValue, int role = Qt::UserRole + 1 );
    void setParent( Object* pParent );
    void setParent( ObjectItem* pParent );
    void unparent();
    inline Object& getObject() { return mObject; }
    inline int type() const { return QStandardItem::UserType; }

private:
    void componentChange( Component& rComponent, bool created );
    void parentChange( Object* pNewParent );
    void displayNameChanged( const String& rDisplayName );
    void destroyed();

    Object& mObject;

};

//------------------------------------------------------------------------------

typedef std::map<String, ObjectItem*> ObjectItems;

/**
Qt model that represents the Diversia object/component system.

@note setObjectManager must be called before this model becomes active.
**/
class ObjectComponentModel : public QStandardItemModel, public sigc::trackable
{
    Q_OBJECT

public:
    /**
    Constructor. 
    
    @param [in,out] pParent The parent object.
    **/
    ObjectComponentModel( QObject* pParent = 0 );
    /**
    Destructor. 
    **/
    ~ObjectComponentModel();

    /**
    Sets the object manager for this model, the model will then monitor the object manager for
    changes and update the model.
    **/
    void setObjectManager( ObjectManager& rObjectManager );
    /**
    Gets an object item. 
    
    @param  rObjectName Name of the object.
    **/
    ObjectItem& getObjectItem( const String& rObjectName ) const;
    /**
    Gets the model index for an object.
    
    @param [in,out] rObject The object. 
    **/
    QModelIndex getObjectIndex( Object& rObject ) const;
    /**
    Gets the model index for an object, using the object name.
    
    @param  rObjectName Name of the object. 
    **/
    QModelIndex getObjectIndex( const String& rObjectName ) const;

private:
    void objectChange( Object& rObject, bool created );
    QStringList mimeTypes() const;
    QMimeData* mimeData( const QModelIndexList& rIndexes ) const;
    bool dropMimeData( const QMimeData* pData, Qt::DropAction action, int row, int column, 
        const QModelIndex& rParent );

    ObjectManager*  mObjectManager;
    ObjectItems     mObjectItems;
    QStringList     mMimeTypes;

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_OBJECTCOMPONENTMODEL_H