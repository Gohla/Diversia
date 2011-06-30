/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_OBJECTTEMPLATETREEVIEW_H
#define DIVERSIA_QTOGREEDITOR_OBJECTTEMPLATETREEVIEW_H

#include "Platform/Prerequisites.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

class ObjectTemplateTreeView : public QTreeView, public sigc::trackable
{
    Q_OBJECT

public:
    /**
    Constructor. 
    
    @param [in,out] pParent The parent object. 
    **/
    ObjectTemplateTreeView( QWidget* pParent = 0 );
    /**
    Destructor. 
    **/
    ~ObjectTemplateTreeView();

    /**
    Initialises this object. 
    **/
    void init();
    /**
    Query if given model index contains an object template.
    **/
    bool isObjectTemplate( const QModelIndex& rIndex );
    /**
    Query if given model index contains a component template.
    **/
    bool isComponentTemplate( const QModelIndex& rIndex );
    /**
    Gets the replica type (object or component template) of the given model index.
    **/
    ReplicaType getReplicaType( const QModelIndex& rIndex );
    /**
    Gets the object template that is contained in given model index.

    @throws InvalidParametersException  If given model index does not contain an object template.
    **/
    ClientObjectTemplate& getObjectTemplate( const QModelIndex& rIndex );
    /**
    Gets the component template that is contained in given model index.

    @throws InvalidParametersException  If given model index does not contain a component template.
    **/
    ClientComponentTemplate& getComponentTemplate( const QModelIndex& rIndex );
    /**
    Loads given model index into the property grid and sets up the actions accordingly.
    **/
    void load( const QModelIndex& rIndex );

private slots:
    void selectionChanged( const QItemSelection& rSelected, const QItemSelection& rDeselected );
    void showContextMenu( const QPoint& rPoint );
    void search();
    void instantiate();
    void createNewObjectTemplate();
    void createChildObjectTemplate();
    void createComponentTemplate( int componentType );
    void destroyObjectTemplate();
    void destroyComponentTemplate();
    void destroySelected();
    void focusInEvent( QFocusEvent* pEvent );

private:
    void destroy( const QModelIndex& rIndex );
    void destroyComponentTemplate( const QModelIndex& rIndex );
    void destroyObjectTemplate( const QModelIndex& rIndex );
    void mousePressEvent( QMouseEvent* pEvent );
    void dropEvent( QDropEvent* pEvent );
    void serverChange( ServerAbstract& rServer, bool created );
    void pluginChange( Plugin& rPlugin, bool created );
    void clearActions();
    void clear();

    ClientObjectManager*          mObjectManager;
    ClientObjectTemplateManager*  mObjectTemplateManager;
    ObjectComponentTemplateModel* mModel;

    static unsigned int mObjectCounter;

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_OBJECTTEMPLATETREEVIEW_H