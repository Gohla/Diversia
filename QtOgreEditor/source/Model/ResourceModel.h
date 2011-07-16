/**
--------------------------------------------------------------------------------
This file is part of Diversia engine
http://www.diversia.org/
Copyright (c) 2008-2011 Gabriël Konat
--------------------------------------------------------------------------------
**/

#ifndef DIVERSIA_QTOGREEDITOR_RESOURCEMODEL_H
#define DIVERSIA_QTOGREEDITOR_RESOURCEMODEL_H

#include "Shared/Resource/ResourceInfo.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

/**
Qt model item that represents a resource in Ogre's resource system.
**/
class ResourceItem : public QStandardItem
{
public:
    ResourceItem( const ResourceInfo& rResourceInfo );
    ~ResourceItem();

    inline const ResourceInfo& getResourceInfo() { return mResourceInfo; }
    inline ResourceType getType() { return mResourceInfo.mType; }
    inline const Path& getFile() { return mResourceInfo.mFile; }

private:
    ResourceInfo mResourceInfo;

};

//------------------------------------------------------------------------------

typedef std::multimap<ResourceType, ResourceItem*> ResourceItems;

/**
Qt model that represents the resource system.

@note setResourceManager must be called before this model becomes active.
**/
class ResourceModel : public QStandardItemModel
{
    Q_OBJECT

public:
    /**
    Constructor. 
    
    @param [in,out] pParent The parent object.
    **/
    ResourceModel( QObject* pParent = 0 );
    /**
    Destructor. 
    **/
    ~ResourceModel();

	/**
    Sets the resource manager for this model, the model will then monitor the resource manager for
    changes and update the model.
    **/
    void setResourceManager( ResourceManager& rResourceManager );
    
private:
    void resourcesInitialised( ResourceManager& rResourceManager );
    void clear();

    ResourceManager*    mResourceManager;
    ResourceItems       mResourceItems;

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_RESOURCEMODEL_H
