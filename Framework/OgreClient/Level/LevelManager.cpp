/*
--------------------------------------------------------------------------------
This file is part of Diversia engine
http://www.diversia.org/
Copyright (c) 2008-2011 Gabriël Konat
--------------------------------------------------------------------------------
*/

#include "OgreClient/Platform/StableHeaders.h"

#include "Client/Plugin/ClientPluginManager.h"
#include "OgreClient/Level/LevelManager.h"
#include "OgreClient/Resource/ResourceManager.h"
#include "Util/Serialization/XMLSerializationFile.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

LevelManager::LevelManager( Mode mode, PluginState state, ClientPluginManager& rPluginManager, 
    RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
    RakNet::NetworkIDManager& rNetworkIDManager ):
    ClientPlugin( mode, state, rPluginManager, rRakPeer, rReplicaManager, rNetworkIDManager ),
    mResourceManager( Plugin::getPluginManager().getPlugin<ResourceManager>() )
{
	PropertySynchronization::storeUserObject();
    Plugin::getPluginManager().connect( sigc::mem_fun( this, &LevelManager::pluginChange ) );
}

LevelManager::~LevelManager()
{

}

void LevelManager::create()
{
    ClientPluginManager& pluginManager = ClientPlugin::getClientPluginManager();
    const Plugins& plugins = pluginManager.getPlugins();
    for( Plugins::const_iterator i = plugins.begin(); i != plugins.end(); ++i )
    {
        mLoadedPlugins.insert( i->first );
    }

    // TODO: Implement real loading completed signal?
    ClientPlugin::mLoadingCompletedSignal( *this );
}

void LevelManager::setServerState( ServerState serverState )
{
    
}

void LevelManager::store( const Path& rFile )
{
    ClientPluginManager& pluginManager = ClientPlugin::getClientPluginManager();

    // Store currently loaded plugins.
    mLoadedPlugins.clear();
    const Plugins& plugins = pluginManager.getPlugins();
    for( Plugins::const_iterator i = plugins.begin(); i != plugins.end(); ++i )
    {
        mLoadedPlugins.insert( i->first );
    }

    // Serialize to XML
    XMLSerializationFile* file = new XMLSerializationFile( rFile, camp::Args( "NoSerialization", 
        "NoLevelSerialization" ), false );
    file->serialize( ClientPlugin::getClientPluginManager(), false );
    file->save( true );
    delete file;
}

void LevelManager::load( const Path& rFile )
{
    // Swap loaded plugins into last loaded plugins because these will be overridden.
    mLastLoadedPlugins.swap( mLoadedPlugins );
    mLoadedPlugins.clear();

    // Reset all plugins to the initial state.
    ClientPlugin::getClientPluginManager().reset();

    // Deserialize from XML
    XMLSerializationFile* file = new XMLSerializationFile( rFile, "NoLevelSerialization", false );
    file->load();
    file->deserialize( ClientPlugin::getClientPluginManager(), false );
    delete file;

    // Remove plugins that were loaded before, but are not loaded in the new level.
    std::vector<PluginTypeEnum> remove;
    std::set_symmetric_difference( mLastLoadedPlugins.begin(), mLastLoadedPlugins.end(), 
        mLoadedPlugins.begin(), mLoadedPlugins.end(), std::inserter( remove, remove.begin() ) );
    ClientPluginManager& pluginManager = ClientPlugin::getClientPluginManager();
    for( std::vector<PluginTypeEnum>::const_iterator i = remove.begin(); i != remove.end(); ++i )
    {
        pluginManager.destroyPlugin( *i );
    }
}

Ogre::StringVectorPtr LevelManager::list()
{
    return Ogre::ResourceGroupManager::getSingletonPtr()->findResourceLocation( 
        mResourceManager.getGroup(), "*.lvl" );
}

void LevelManager::pluginChange( Plugin& rPlugin, bool created )
{
    if( created ) 
        mLoadedPlugins.insert( rPlugin.getType() );
    else
        mLoadedPlugins.erase( rPlugin.getType() );
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia
