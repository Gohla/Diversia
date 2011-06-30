/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_PLUGINTREEVIEW_H
#define DIVERSIA_QTOGREEDITOR_PLUGINTREEVIEW_H

#include "Platform/Prerequisites.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

class PluginTreeView : public QTreeView, public sigc::trackable
{
    Q_OBJECT

public:
    /**
    Constructor. 
    
    @param [in,out] pParent The parent object. 
    **/
    PluginTreeView( QWidget* pParent = 0 );
    /**
    Destructor. 
    **/
    ~PluginTreeView();

    /**
    Initialises this object. 
    **/
    void init();
    /**
    Gets the server plugin that is contained in given model index.
    **/
    ClientPlugin& getClientPlugin( const QModelIndex& rIndex );
    /**
    Loads given model index into the property grid.
    **/
    void load( const QModelIndex& rIndex );

private slots:
    void selectionChanged( const QItemSelection& rSelected, const QItemSelection& rDeselected );
    void showContextMenu( const QPoint& rPoint );
    void search();
    void createPlugin( int pluginType );
    void pluginAction( QObject* pObject );
    void destroyPlugin();
    void focusInEvent( QFocusEvent* pEvent );

private:
    void mousePressEvent( QMouseEvent* pEvent );
    void serverChange( ServerAbstract& rServer, bool created );
    void clearActions();
    void clear();

    ClientPluginManager*        mPluginManager;
    PluginModel*    mModel;

    QMenu*                                                  mPluginMenu;
    QAction*                                                mPluginMenuAction;
    std::map< PluginTypeEnum, QList<QAction*> > mPluginActions;

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_PLUGINTREEVIEW_H