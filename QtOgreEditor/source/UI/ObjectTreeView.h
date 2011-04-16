/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_OBJECTTREEVIEW_H
#define DIVERSIA_QTOGREEDITOR_OBJECTTREEVIEW_H

#include "Platform/Prerequisites.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

class ObjectTreeView : public QTreeView, public sigc::trackable
{
    Q_OBJECT

public:
    /**
    Constructor. 
    
    @param [in,out] pParent The parent object. 
    **/
    ObjectTreeView( QWidget* pParent = 0 );
    /**
    Destructor. 
    **/
    ~ObjectTreeView();

    /**
    Initialises this object. 
    **/
    void init();
    /**
    Query if given model index contains an object.
    **/
    bool isObject( const QModelIndex& rIndex );
    /**
    Query if given model index contains a component.
    **/
    bool isComponent( const QModelIndex& rIndex );
    /**
    Gets the replica type (object or component) of the given model index.
    **/
    ReplicaType getReplicaType( const QModelIndex& rIndex );
    /**
    Gets the object that is contained in given model index.

    @throws InvalidParametersException  If given model index does not contain an object.
    **/
    ClientObject& getObject( const QModelIndex& rIndex );
    /**
    Gets the component that is contained in given model index.

    @throws InvalidParametersException  If given model index does not contain a component.
    **/
    ClientComponent& getComponent( const QModelIndex& rIndex );
    /**
    Loads given model index into the property grid and sets up the actions accordingly.
    **/
    void load( const QModelIndex& rIndex );

private slots:
    void selectionChanged( const QItemSelection& rSelected, const QItemSelection& rDeselected );
    void showContextMenu( const QPoint& rPoint );
    void search();
    void createNewObject();
    void createChildObject();
    void createComponent( int componentType );
    void componentAction( QObject* pObject );
    void createTemplate();
    void destroyObject();
    void destroyComponent();
    void destroySelected();
    void focusInEvent( QFocusEvent* pEvent );

private:
    void destroy( const QModelIndex& rIndex );
    void destroyComponent( const QModelIndex& rIndex );
    void destroyObject( const QModelIndex& rIndex );
    void mousePressEvent( QMouseEvent* pEvent );
    void dropEvent( QDropEvent* pEvent );
    void serverChange( ServerAbstract& rServer, bool created );
    void pluginChange( ClientServerPlugin& rPlugin, bool created );
    void objectSelectionChange( ClientObject& rObject, bool selected );
    void clearActions();
    void clear();

    ClientObjectManager*            mObjectManager;
    ClientObjectTemplateManager*    mObjectTemplateManager;
    ObjectComponentModel*           mModel;

    QMenu*                                      mComponentMenu;
    QAction*                                    mComponentMenuAction;
    std::map< ComponentType, QList<QAction*> >  mComponentActions;

    static unsigned int     mObjectCounter;

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_OBJECTTREEVIEW_H