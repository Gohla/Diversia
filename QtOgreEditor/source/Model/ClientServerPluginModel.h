/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_CLIENTSERVERPLUGINMODEL_H
#define DIVERSIA_QTOGREEDITOR_CLIENTSERVERPLUGINMODEL_H

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
    PluginItem( ClientServerPlugin& rPlugin );
    ~PluginItem();

    inline ClientServerPlugin& getPlugin() { return mPlugin; }
    inline int type() const { return QStandardItem::UserType; }

private:
    ClientServerPlugin& mPlugin;

};

//------------------------------------------------------------------------------

typedef std::map<ClientServerPluginTypeEnum, PluginItem*> PluginItems;

/**
Qt model that represents the Diversia client-server plugin system.

@note setPluginManager must be called before this model becomes active.
**/
class ClientServerPluginModel : public QStandardItemModel, public sigc::trackable
{
    Q_OBJECT

public:
    /**
    Constructor. 
    
    @param [in,out] pParent The parent object.
    **/
    ClientServerPluginModel( QObject* pParent = 0 );
    /**
    Destructor. 
    **/
    ~ClientServerPluginModel();

    /**
    Sets the plugin manager for this model, the model will then monitor the plugin manager for
    changes and update the model.
    **/
    void setPluginManager( ServerPluginManager& rPluginManager );
    /**
    Gets a plugin item. 
    
    @param  type    The plugin type. 
    **/
    PluginItem& getPluginItem( ClientServerPluginTypeEnum type ) const;
    /**
    Gets the model index for a plugin item.
    
    @param [in,out] rObject The plugin. 
    **/
    QModelIndex getPluginIndex( ClientServerPlugin& rPlugin ) const;
    /**
    Gets the model index for a plugin, using the plugin type.
    
    @param  type    The plugin type. 
    **/
    QModelIndex getPluginIndex( ClientServerPluginTypeEnum type ) const;

private:
    void pluginChange( ClientServerPlugin& rPlugin, bool created );

    ServerPluginManager*    mPluginManager;
    PluginItems             mPluginItems;

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_CLIENTSERVERPLUGINMODEL_H