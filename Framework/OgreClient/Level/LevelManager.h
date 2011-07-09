/*
--------------------------------------------------------------------------------
This file is part of Diversia engine
http://www.diversia.org/
Copyright (c) 2008-2011 Gabriël Konat
--------------------------------------------------------------------------------
*/

#ifndef DIVERSIA_OGRECLIENT_LEVELMANAGER_H
#define DIVERSIA_OGRECLIENT_LEVELMANAGER_H

#include "Client/Plugin/ClientPlugin.h"


namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API LevelManager : public ClientPlugin, public sigc::trackable
{
public:
    LevelManager( Mode mode, PluginState state, ClientPluginManager& rPluginManager, 
        RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
        RakNet::NetworkIDManager& rNetworkIDManager );
    ~LevelManager();

    /**
    Gets the plugin type.
    **/
    inline PluginTypeEnum getType() const { return PLUGINTYPE_LEVELMANAGER; }
    static inline PluginTypeEnum getTypeStatic() { return PLUGINTYPE_LEVELMANAGER; }
    /**
    Gets the plugin type name.
    **/
    inline String getTypeName() const { return PLUGINNAME_LEVELMANAGER; }
    static inline String getTypeNameStatic() { return PLUGINNAME_LEVELMANAGER; }

    /**
    Serializes the current state of the plugin manager to a file. 
    
    @param  rFile   The file to serialize to.
    **/
    void store( const Path& rFile );
    /**
    Deserializes state in given file to the plugin manager.
    
    @param  rFile   The file to deserialize from.
    **/
    void load( const Path& rFile );
    /**
    Returns a list of all available levels.
    
    @return A list of all available levels. 
    **/
    Ogre::StringVectorPtr list();
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    void create();
    inline void reset() {}
    void setServerState( ServerState serverState );

    void pluginChange( Plugin& rPlugin, bool created );

    ResourceManager&            mResourceManager;
    String                      mDefaultLevel;
    std::set<PluginTypeEnum>    mLoadedPlugins;
    std::set<PluginTypeEnum>    mLastLoadedPlugins;

	CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::LevelManager, 
    &Diversia::OgreClient::Bindings::CampBindings::bindLevelManager );

#endif // DIVERSIA_OGRECLIENT_LEVELMANAGER_H
