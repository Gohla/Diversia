/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.

Diversia is free software: you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation, either version 3 of the License, or (at your option) any later 
version.

Diversia is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with 
Diversia. If not, see <http://www.gnu.org/licenses/>.

You may contact the author of Diversia by e-mail at: equabyte@sonologic.nl
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_OGRECLIENT_RESOURCEMANAGER_H
#define DIVERSIA_OGRECLIENT_RESOURCEMANAGER_H

#include "OgreClient/Platform/Prerequisites.h"

#include "Client/Plugin/ClientPlugin.h"
#include "Shared/Resource/ResourceInfo.h"
#include <boost/multi_index/key_extractors.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index_container.hpp>

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

typedef sigc::slot<void, Ogre::ResourcePtr> ResourceCallback;
typedef sigc::slot<void> SimpleResourceCallback;
typedef std::map<std::pair<String, Ogre::ResourceHandle>, Ogre::BackgroundProcessTicket> HandleTickets;
typedef std::map<Ogre::BackgroundProcessTicket, Ogre::ResourcePtr> TicketResources;

// Bidirectional map with indexes as ordered_non_unique, used for ticket callbacks.
struct left{};
struct right{};

template<typename FromType,typename ToType>
struct bidirectional_map
{
    typedef std::_Pair_base<FromType,ToType> value_type;

    typedef boost::multi_index::multi_index_container<
        value_type,
        boost::multi_index::indexed_by
        <
            boost::multi_index::ordered_non_unique
            <
                boost::multi_index::tag<left>, boost::multi_index::member<value_type,FromType,&value_type::first>
            >,
            boost::multi_index::ordered_non_unique
            <
                boost::multi_index::tag<right>, boost::multi_index::member<value_type,ToType,&value_type::second>
            >
        >

  > type;

};

typedef bidirectional_map<Ogre::BackgroundProcessTicket, ResourceCallback>::type TicketCallbacks;

//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API ResourceManager : public ClientPlugin, public Ogre::ResourceBackgroundQueue::Listener
{
public:
    ResourceManager( Mode mode, PluginState state, ClientPluginManager& rPluginManager, 
        RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
        RakNet::NetworkIDManager& rNetworkIDManager );
    /**
    Destructor. 
    **/
    virtual ~ResourceManager();

    /**
    Gets the resource group. 
    **/
    inline const String& getGroup() const { return mGroup; }
    /**
    Gets the resource path. 
    **/
    inline const Path& getResourcePath() const { return mResourcePath; }
    /**
    Gets a resource or optionally create one if it doesn't exist.
    
    @param  rResource   Resource information.
    **/
    Ogre::ResourcePtr getResource( const ResourceInfo& rResource, bool declare = false ) const;
    /**
    Gets a list of all resources.
    **/
    ResourceList list();
    /**
    Gets the plugin type.
    **/
    inline PluginTypeEnum getType() const { return PLUGINTYPE_RESOURCEMANAGER; }
    static inline PluginTypeEnum getTypeStatic() { return PLUGINTYPE_RESOURCEMANAGER; }
    /**
    Gets the plugin type name.
    **/
    inline String getTypeName() const { return PLUGINNAME_RESOURCEMANAGER; }
    static inline String getTypeNameStatic() { return PLUGINNAME_RESOURCEMANAGER; }

    /**
    Sets the resource location. 
    
    @param  rResourceLocation   The resource location. 
    **/
    void setResourceLocation( const String& rResourceLocation );
    /**
    Sets the group name. Cannot be changed one set. 
    
    @param  rGroup  The group name
    **/
    void setGroup( const String& rGroup );
    /**
    Loads a resource. 
    
    @param  rResource       The resource information.
    @param  rCallbackSlot   The callback slot that gets called when the resource is done loading.
                            When the resource is already loaded, the callback will be called
                            immediately.

    @throws FileNotFoundException   If given file name doesn't exist.
    **/
    Ogre::ResourcePtr loadResource( const ResourceInfo& rResource, 
        const ResourceCallback& rCallbackSlot );
    /**
    Loads multiple resources 
    
    @param  rResources      The resources.
    @param  rCallbackSlot   The callback slot that gets called when the resources are done loading. 
                            When all resources are already loaded, the callback will be called
                            immediately. 

    @throws FileNotFoundException   If given file name doesn't exist.
    **/
    void loadResources( const ResourceList& rResources, 
        const SimpleResourceCallback& rCallbackSlot );
    /**
    Connects a slot to the resources initialized signal. If the resources are already initialized, 
    the slot will be called immediately.
    
    @param [in,out] rSlot   The slot (signature: void func(ResourceManager&)) to connect. 
    
    @return Connection object to block or disconnect the connection, or an empty connection when
            the entity is already loaded.
    **/
    sigc::connection connectInitialized( sigc::slot<void, ResourceManager&> rSlot );

    /**
    Gets the resource manager name for given resource type.
    
    @param  type    The resource type.
    **/
    static const char* getResourceManagerName( ResourceType type );

    void operationCompleted( Ogre::BackgroundProcessTicket ticket, 
        const Ogre::BackgroundProcessResult& rResult );
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    /**
    Utility class to load multiple resources.
    **/
    class ResourcesLoader : public sigc::trackable
    {
    public:
        void addResource( Ogre::ResourcePtr pResource );
        void loadingComplete( Ogre::ResourcePtr pResource );
        
        static ResourcesLoader* create( const SimpleResourceCallback& rCompletedSlot );

    private:
        friend class ResourceManager;

        ResourcesLoader();
        ResourcesLoader( const SimpleResourceCallback& rCompletedSlot );
        ~ResourcesLoader();

        std::set< std::pair<String, Ogre::ResourceHandle> > mResources;
        SimpleResourceCallback                              mCompletedSlot;
    };

    void load();
    void unload();
    void destroy();
    inline void setServerState( ServerState serverState ) {}

    ServerAbstract& mServer;
    Ogre::ResourceGroupManager& mRGM;
    Ogre::ResourceBackgroundQueue& mRBQ;

    ResourceLocationType mType;
    String mResourceLocation;
    Path mResourcePath;
    String mGroup;
    Ogre::BackgroundProcessTicket mInitializationTicket;
    bool mCreated;
    bool mInitializing;
    sigc::signal<void, ResourceManager&> mInitializedSignal;
    TicketCallbacks mTicketCallbacks;
    HandleTickets mHandleTickets;
    TicketResources mTicketResources;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::ResourceManager, 
    &Diversia::OgreClient::Bindings::CampBindings::bindResourceManager );

#endif // DIVERSIA_OGRECLIENT_RESOURCEMANAGER_H