/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include "Shared/Platform/StableHeaders.h"

#include "Shared/ClientServerPlugin/ClientServerPlugin.h"
#include "Shared/ClientServerPlugin/ClientServerPluginFactory.h"
#include "Shared/ClientServerPlugin/ClientServerPluginFactoryManager.h"
#include "Shared/ClientServerPlugin/ClientServerPluginManager.h"
#include "Util/Serialization/XMLSerializationFile.h"

namespace Diversia
{
//------------------------------------------------------------------------------

ClientServerPluginTypes ClientServerPluginManager::mAutoCreatePlugins = ClientServerPluginTypes();

ClientServerPluginManager::ClientServerPluginManager( Mode mode, PluginState state, 
    sigc::signal<void>& rUpdateSignal, RakNet::RakPeerInterface& rRakPeer, 
    RakNet::ReplicaManager3& rReplicaManager, RakNet::NetworkIDManager& rNetworkIDManager ):
    mMode( mode ),
    mPluginState( state ),
    mStoredState( 0 ),
    mUpdateSignal( rUpdateSignal ),
    mRakPeer( rRakPeer ),
    mReplicaManager( rReplicaManager ),
    mNetworkIDManager( rNetworkIDManager )
{
    mUpdateConnection = mUpdateSignal.connect( sigc::mem_fun( this, 
        &ClientServerPluginManager::update ) );

    // Create auto create plugins.
    for( ClientServerPluginTypes::iterator i = mAutoCreatePlugins.begin(); 
        i != mAutoCreatePlugins.end(); ++i )
    {
        ClientServerPluginManager::createPlugin( *i );
    }
}

ClientServerPluginManager::~ClientServerPluginManager()
{
    // Destroy all plugins that were queued for destruction in the next tick.
    for( ClientServerPluginTypes::reverse_iterator i = mDestroyedPlugins.rbegin(); 
        i != mDestroyedPlugins.rend(); ++i )
    {
        ClientServerPlugin& plugin = ClientServerPluginManager::getPlugin( *i );
        mPluginSignal( plugin, false );
        mPlugins.erase( *i );
        plugin.checkBroadcastDestruction();
        ClientServerPluginFactoryManager::getPluginFactory( *i ).destroy( plugin );
    }

    // Destroy all components immediately. This is safe since objects are already destroyed in the 
    // next tick.
    for( ClientServerPlugins::reverse_iterator i = mPlugins.rbegin(); i != mPlugins.rend(); ++i )
    {
        mPluginSignal( *i->second, false );
        i->second->checkBroadcastDestruction();
        ClientServerPluginFactoryManager::getPluginFactory( i->second->getType() ).
            destroy( *i->second );
    }
}

ClientServerPlugin& ClientServerPluginManager::createPlugin( ClientServerPluginTypeEnum type )
{
    if( !ClientServerPluginManager::hasPlugin( type ) )
    {
        // Create the plugin now, but the plugin specific part is initialized in the next
        // tick/frame update.
        ClientServerPlugin& plugin = 
            ClientServerPluginFactoryManager::getPluginFactory( type ).create( mMode, mPluginState, 
            *this, mRakPeer, mReplicaManager, mNetworkIDManager );
        mPlugins.insert( std::make_pair( type, &plugin ) );
        mCreatedPlugins.insert( type );
        mPluginSignal( plugin, true );
        mUpdateConnection.block( false );
        return plugin;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_DUPLICATE_ITEM, 
            "Plugin of this type already exists.", "ClientServerPluginManager::createPlugin" );
    }
}

ClientServerPlugin& ClientServerPluginManager::getPlugin( ClientServerPluginTypeEnum type ) const
{
    ClientServerPlugins::const_iterator i = mPlugins.find( type );
    if( i != mPlugins.end() )
    {
        return *i->second;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Plugin not found.", 
            "ClientServerPluginManager::getPlugin" );
    }
}

bool ClientServerPluginManager::hasPlugin( ClientServerPluginTypeEnum type ) const
{
    return mPlugins.find( type ) != mPlugins.end();
}

void ClientServerPluginManager::destroyPlugin( ClientServerPluginTypeEnum type )
{
    if( ClientServerPluginManager::hasPlugin( type ) )
    {
        // Destroy plugin in the next tick.
        mDestroyedPlugins.insert( type );
        mCreatedPlugins.erase( type );
        mUpdateConnection.block( false );
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Plugin not found.", 
            "ClientServerPluginManager::hasPlugin" );
    }
}

void ClientServerPluginManager::addAutoCreateComponent( ClientServerPluginTypeEnum type )
{
    mAutoCreatePlugins.insert( type );
}

void ClientServerPluginManager::setState( PluginState state )
{
    if( state == mPluginState ) return;

    PluginState prevState = mPluginState;
    mPluginState = state;

    if( mPluginState == PLAY ) ClientServerPluginManager::storeState();
    for( ClientServerPlugins::iterator i = mPlugins.begin(); i != mPlugins.end(); ++i )
    {
        i->second->setState( mPluginState, prevState );
    }
    if( mPluginState == STOP ) ClientServerPluginManager::restoreState();

    mPluginStateChange( mPluginState, prevState );
}

void ClientServerPluginManager::storeState()
{
    if( mStoredState ) delete mStoredState;
    mStoredState = new XMLSerializationFile( "", "NoSerialization", false, true );
    mStoredState->serialize( this, false );
}

void ClientServerPluginManager::restoreState()
{
    if( mStoredState )
    {
        mStoredState->deserialize( this, false );
        delete mStoredState;
        mStoredState = 0;
    }
}

void ClientServerPluginManager::update()
{
    SLOGD << "ClientServerPluginManager::update";

    // Initialize plugins that were created in the previous tick/frame.
    for( ClientServerPluginTypes::iterator i = mCreatedPlugins.begin(); i != mCreatedPlugins.end(); 
        ++i )
    {
        // Initializes the plugin.
        ClientServerPluginManager::getPlugin( *i ).create();
    }
    mCreatedPlugins.clear();

    // Destroy plugins that were queued for destruction in the previous tick/frame.
    for( ClientServerPluginTypes::reverse_iterator i = mDestroyedPlugins.rbegin(); 
        i != mDestroyedPlugins.rend(); ++i )
    {
        ClientServerPlugin& plugin = ClientServerPluginManager::getPlugin( *i );
        mPluginSignal( plugin, false );
        mPlugins.erase( *i );
        plugin.checkBroadcastDestruction();
        ClientServerPluginFactoryManager::getPluginFactory( *i ).destroy( plugin );
    }
    mDestroyedPlugins.clear();

    // Block updates until a plugin must be added or destroyed.
    mUpdateConnection.block( true );
}

//------------------------------------------------------------------------------
} // Namespace Diversia
