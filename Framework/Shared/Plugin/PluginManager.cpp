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

#include "Shared/Plugin/Plugin.h"
#include "Shared/Plugin/PluginFactory.h"
#include "Shared/Plugin/PluginFactoryManager.h"
#include "Shared/Plugin/PluginManager.h"
#include "Util/Serialization/MemorySerialization.h"

namespace Diversia
{
//------------------------------------------------------------------------------

PluginTypes PluginManager::mAutoCreatePlugins = PluginTypes();

PluginManager::PluginManager( Mode mode, PluginState state, 
    sigc::signal<void>& rUpdateSignal, RakNet::RakPeerInterface& rRakPeer, 
    RakNet::ReplicaManager3& rReplicaManager, RakNet::NetworkIDManager& rNetworkIDManager ):
    mMode( mode ),
    mPluginState( state ),
    mPrevPluginState( state ),
    mStoredState( 0 ),
    mUpdateSignal( rUpdateSignal ),
    mRakPeer( rRakPeer ),
    mReplicaManager( rReplicaManager ),
    mNetworkIDManager( rNetworkIDManager )
{
    mUpdateConnection = mUpdateSignal.connect( sigc::mem_fun( this, 
        &PluginManager::update ) );
}

PluginManager::~PluginManager()
{
    // Destroy all plugins that were queued for destruction in the next tick.
    for( PluginTypes::reverse_iterator i = mDestroyedPlugins.rbegin(); 
        i != mDestroyedPlugins.rend(); ++i )
    {
        Plugin& plugin = PluginManager::getPlugin( *i );
        mPluginSignal( plugin, false );
        mPlugins.erase( *i );
        plugin.checkBroadcastDestruction();
        PluginFactoryManager::getPluginFactory( *i ).destroy( plugin );
    }

    // Destroy all components immediately. This is safe since objects are already destroyed in the 
    // next tick.
    for( Plugins::reverse_iterator i = mPlugins.rbegin(); i != mPlugins.rend(); ++i )
    {
        mPluginSignal( *i->second, false );
        i->second->checkBroadcastDestruction();
        PluginFactoryManager::getPluginFactory( i->second->getType() ).
            destroy( *i->second );
    }

    if( mStoredState ) delete mStoredState;
}

Plugin& PluginManager::createPlugin( PluginTypeEnum type )
{
    if( !PluginManager::hasPlugin( type ) )
    {
        // Create the plugin now, but the plugin specific part is initialized in the next
        // tick/frame update.
        Plugin& plugin = 
            PluginFactoryManager::getPluginFactory( type ).create( mMode, mPluginState, 
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
            "Plugin of this type already exists.", "PluginManager::createPlugin" );
    }
}

Plugin& PluginManager::getPlugin( PluginTypeEnum type ) const
{
    Plugins::const_iterator i = mPlugins.find( type );
    if( i != mPlugins.end() )
    {
        return *i->second;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Plugin not found.", 
            "PluginManager::getPlugin" );
    }
}

bool PluginManager::hasPlugin( PluginTypeEnum type ) const
{
    return mPlugins.find( type ) != mPlugins.end();
}

void PluginManager::destroyPlugin( PluginTypeEnum type )
{
    if( PluginManager::hasPlugin( type ) )
    {
        // Destroy plugin in the next tick.
        mDestroyedPlugins.insert( type );
        mCreatedPlugins.erase( type );
        mUpdateConnection.block( false );
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Plugin not found.", 
            "PluginManager::hasPlugin" );
    }
}

void PluginManager::addAutoCreatePlugin( PluginTypeEnum type )
{
    mAutoCreatePlugins.insert( type );
}

bool PluginManager::hasAutoCreatePlugin( PluginTypeEnum type )
{
    return mAutoCreatePlugins.find( type ) != mAutoCreatePlugins.end();
}

void PluginManager::setState( PluginState state )
{
    if( state == mPluginState ) return;

    mPrevPluginState = mPluginState;
    mPluginState = state;

    if( mPluginState == PLAY && mPrevPluginState == STOP ) PluginManager::storeState();
    for( Plugins::iterator i = mPlugins.begin(); i != mPlugins.end(); ++i )
    {
        i->second->setState( mPluginState, mPrevPluginState );
    }
    if( mPluginState == STOP ) PluginManager::restoreState();

    mPluginStateChange( mPluginState, mPrevPluginState );
}

void PluginManager::storeState()
{
    if( mStoredState ) delete mStoredState;
    //mStoredState = new XMLSerializationFile( "", "NoSerialization", false, true );
    mStoredState = new MemorySerialization( "NoSerialization", false );
    mStoredState->serialize( this, false );
}

void PluginManager::restoreState()
{
    if( mStoredState )
    {
        mStoredState->deserialize( this, false );
        delete mStoredState;
        mStoredState = 0;
    }
}

void PluginManager::loadAll()
{
    for( Plugins::iterator i = mPlugins.begin(); i != mPlugins.end(); ++i )
        if( !mDestroyedPlugins.count( i->first ) ) 
            mCreatedPlugins.insert( i->first );

    mUpdateConnection.block( false );
}

void PluginManager::unloadAll()
{
    for( Plugins::reverse_iterator i = mPlugins.rbegin(); i != mPlugins.rend(); ++i ) 
        i->second->unload();
}

void PluginManager::autoCreatePlugins()
{
    // Create auto create plugins.
    for( PluginTypes::iterator i = mAutoCreatePlugins.begin(); 
        i != mAutoCreatePlugins.end(); ++i )
    {
        PluginManager::createPlugin( *i );
    }
}

void PluginManager::update()
{
    SLOGD << "PluginManager::update";

    // Initialize plugins that were created in the previous tick/frame.
    for( PluginTypes::iterator i = mCreatedPlugins.begin(); i != mCreatedPlugins.end(); 
        ++i )
    {
        // Initializes the plugin.
        PluginManager::getPlugin( *i ).load();
    }
    mCreatedPlugins.clear();

    // Destroy plugins that were queued for destruction in the previous tick/frame.
    for( PluginTypes::reverse_iterator i = mDestroyedPlugins.rbegin(); 
        i != mDestroyedPlugins.rend(); ++i )
    {
        Plugin& plugin = PluginManager::getPlugin( *i );
        mPluginSignal( plugin, false );
        mPlugins.erase( *i );
        plugin.checkBroadcastDestruction();
        PluginFactoryManager::getPluginFactory( *i ).destroy( plugin );
    }
    mDestroyedPlugins.clear();

    // Block updates until a plugin must be added or destroyed.
    mUpdateConnection.block( true );
}

//------------------------------------------------------------------------------
} // Namespace Diversia
