/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_OBJECTCOMPONENTTEMPLATEMODEL_H
#define DIVERSIA_QTOGREEDITOR_OBJECTCOMPONENTTEMPLATEMODEL_H

#include "Platform/Prerequisites.h"

#include "Object/ComponentTemplate.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

/**
Qt model item that represents a component in the Diversia object/component system.
**/
class ComponentTemplateItem : public QStandardItem, public sigc::trackable
{
public:
    ComponentTemplateItem( ComponentTemplate& rComponentTemplate );
    ~ComponentTemplateItem();

    inline ComponentTemplate& getComponentTemplate() { return mComponentTemplate; }
    inline ObjectTemplate& getOwnerObjectTemplate() { return mComponentTemplate.getObjectTemplate(); }
    inline int type() const { return QStandardItem::UserType + 1; }

private:
    void destroyed();

    ComponentTemplate& mComponentTemplate;

};

//------------------------------------------------------------------------------

/**
Qt model item that represents an object in the Diversia object/component system.
**/
class ObjectTemplateItem : public QStandardItem, public sigc::trackable
{
public:
    ObjectTemplateItem( ObjectTemplate& rObjectTemplate );
    ~ObjectTemplateItem();

    void setData( const QVariant& rValue, int role = Qt::UserRole + 1 );
    void setParent( ObjectTemplate* pParent );
    void setParent( ObjectTemplateItem* pParent );
    void unparent();
    inline ObjectTemplate& getObjectTemplate() { return mObjectTemplate; }
    inline int type() const { return QStandardItem::UserType; }

private:
    void componentChange( ComponentTemplate& rComponentTemplate, bool created );
    void parentChange( ObjectTemplate* pNewParent );
    void displayNameChanged( const String& rDisplayName );
    void destroyed();

    ObjectTemplate& mObjectTemplate;

};

//------------------------------------------------------------------------------

typedef std::map<String, ObjectTemplateItem*> ObjectTemplateItems;

/**
Qt model that represents the Diversia object/component system.

@note setObjectTemplateManager must be called before this model becomes active.
**/
class ObjectComponentTemplateModel : public QStandardItemModel, public sigc::trackable
{
    Q_OBJECT

public:
    /**
    Constructor. 
    
    @param [in,out] pParent The parent object.
    **/
    ObjectComponentTemplateModel( QObject* pParent = 0 );
    /**
    Destructor. 
    **/
    ~ObjectComponentTemplateModel();

    /**
    Sets the object manager for this model, the model will then monitor the object manager for
    changes and update the model.
    **/
    void setObjectTemplateManager( ObjectTemplateManager& rObjectTemplateManager );
    /**
    Gets an object item. 
    
    @param  rObjectTemplateName Name of the object.
    **/
    ObjectTemplateItem& getObjectTemplateItem( const String& rObjectTemplateName ) const;
    /**
    Gets the model index for an object.
    
    @param [in,out] rObjectTemplate The object. 
    **/
    QModelIndex getObjectTemplateIndex( ObjectTemplate& rObjectTemplate ) const;
    /**
    Gets the model index for an object, using the object name.
    
    @param  rObjectTemplateName Name of the object. 
    **/
    QModelIndex getObjectTemplateIndex( const String& rObjectTemplateName ) const;

private:
    void objectChange( ObjectTemplate& rObjectTemplate, bool created );
    QStringList mimeTypes() const;
    QMimeData* mimeData( const QModelIndexList& rIndexes ) const;
    bool dropMimeData( const QMimeData* pData, Qt::DropAction action, int row, int column, 
        const QModelIndex& rParent );

    ObjectTemplateManager*  mObjectTemplateManager;
    ObjectTemplateItems     mObjectTemplateItems;
    QStringList             mMimeTypes;

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_OBJECTCOMPONENTTEMPLATEMODEL_H