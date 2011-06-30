/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_PLUGINMODEL_H
#define DIVERSIA_QTOGREEDITOR_PLUGINMODEL_H

#include "Platform/Prerequisites.h"

namespace Diversia
{
namespace QtOgreEditor
{
 //------------------------------------------------------------------------------

/**
Qt model item that represents a client-server plugin in the Diversia object/component system.
**/
class PluginItem : public QStandardItem, public sigc::trackable
{
public:
    PluginItem( Plugin& rPlugin );
    ~PluginItem();

    inline Plugin& getPlugin() { return mPlugin; }
    inline int type() const { return QStandardItem::UserType; }

private:
    Plugin& mPlugin;

};

//------------------------------------------------------------------------------

typedef std::map<PluginTypeEnum, PluginItem*> PluginItems;

/**
Qt model that represents the Diversia client-server plugin system.

@note setPluginManager must be called before this model becomes active.
**/
class PluginModel : public QStandardItemModel, public sigc::trackable
{
    Q_OBJECT

public:
    /**
    Constructor. 
    
    @param [in,out] pParent The parent object.
    **/
    PluginModel( QObject* pParent = 0 );
    /**
    Destructor. 
    **/
    ~PluginModel();

    /**
    Sets the plugin manager for this model, the model will then monitor the plugin manager for
    changes and update the model.
    **/
    void setPluginManager( ClientPluginManager& rPluginManager );
    /**
    Gets a plugin item. 
    
    @param  type    The plugin type. 
    **/
    PluginItem& getPluginItem( PluginTypeEnum type ) const;
    /**
    Gets the model index for a plugin item.
    
    @param [in,out] rObject The plugin. 
    **/
    QModelIndex getPluginIndex( Plugin& rPlugin ) const;
    /**
    Gets the model index for a plugin, using the plugin type.
    
    @param  type    The plugin type. 
    **/
    QModelIndex getPluginIndex( PluginTypeEnum type ) const;

private:
    void pluginChange( Plugin& rPlugin, bool created );

    ClientPluginManager*    mPluginManager;
    PluginItems             mPluginItems;

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_PLUGINMODEL_H