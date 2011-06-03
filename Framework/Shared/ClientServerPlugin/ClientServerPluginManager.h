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

#ifndef DIVERSIA_SHARED_CLIENTSERVERPLUGINMANAGER_H
#define DIVERSIA_SHARED_CLIENTSERVERPLUGINMANAGER_H

#include "Shared/Platform/Prerequisites.h"

namespace Diversia
{
//------------------------------------------------------------------------------

typedef std::map<ClientServerPluginTypeEnum, ClientServerPlugin*> ClientServerPlugins;
typedef std::set<ClientServerPluginTypeEnum> ClientServerPluginTypes;

/**
Values that represent the state of a plugin.
**/
enum PluginState 
{
    STOP = 0,	///< Plugin is stopped and is not updated.
    PLAY,	    ///< Plugin is active and is updated. State is saved before going into this state.
    PAUSE	    ///< Plugin is paused and is not updated. 
};

class DIVERSIA_SHARED_API ClientServerPluginManager : public boost::noncopyable
{
public:
    /**
    Constructor. 

    @param  mode                        The mode the client-server plugin manager must run in.
    @param  state                       The initial state to set plugins to.
    @param [in,out] rUpdateSignal       The frame/tick update signal.
    @param [in,out] rRakPeer            The peer interface.
    @param [in,out] rReplicaManager     Replica manager. 
    @param [in,out] rNetworkIDManager   Network ID manager. 
    **/
    ClientServerPluginManager( Mode mode, PluginState state, sigc::signal<void>& rUpdateSignal, 
        RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
        RakNet::NetworkIDManager& rNetworkIDManager );
    /**
    Destructor. 
    **/
    virtual ~ClientServerPluginManager();

    /**
    Creates a client-server plugin. The plugin specific part is initialized in the next update tick.
    
    @param  type    The type of the client-server plugin.
                    
    @throw  Exception   When a client-server plugin with that name already exists.
    **/
    ClientServerPlugin& createPlugin( ClientServerPluginTypeEnum type );
    /**
    Creates a client-server plugin. The plugin specific part is initialized in the next update tick.
                    
    @throw  Exception   When a client-server plugin with that name already exists.
    **/
    template<class T> inline T& createPlugin()
    {
        return static_cast<T&>( createPlugin( T::getTypeStatic() ) );
    }
    /**
    Gets a client-server plugin using the plugin's C++ type as template parameter.

    @throw  Exception   When client-server plugin doesn't exist.
    **/
    template<class T> inline T& getPlugin() const
    {
        ClientServerPlugins::const_iterator i = mPlugins.find( T::getTypeStatic() );
        if ( i != mPlugins.end() )
        {
            return *static_cast<T*>( i->second );
        }
        else
        {
            DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Plugin not found.", 
                "ClientServerPluginManager::getPlugin" );
        }
    }
    /**
    Gets a client-server plugin by type.
    
    @param  type    The type of the client-server plugin.
    
    @throw  Exception   When client-server plugin doesn't exist.
    **/
    ClientServerPlugin& getPlugin( ClientServerPluginTypeEnum type ) const;
    /**
    Gets the map of client-server plugins.
    **/
    inline const ClientServerPlugins& getPlugins() const { return mPlugins; }
    /**
    Query if a client-server plugin exists using the plugin's C++ type as template parameter.
    
    @return True if client-server plugin exists, false if not.
    **/
    template<class T> inline bool hasPlugin() const
    {
        if ( mPlugins.find( T::getTypeStatic() ) != mPlugins.end() )
        {
            return true;
        }
        return false;
    }
    /**
    Query if a client-server plugin exists by type.
    
    @param  type    The type of the client-server plugin.
    
    @return True if client-server plugin exists, false if not. 
    **/
    bool hasPlugin( ClientServerPluginTypeEnum type ) const;
    /**
    Destroys a client-server plugin by type. The plugin will be destroyed the next update tick.
    
    @param  type    The type of the client-server plugin.

    @throw  Exception   When a plugin with that name doesn't exist.
    **/
    void destroyPlugin( ClientServerPluginTypeEnum type );
    /**
    Adds a plugin to the auto create plugin list. All plugin in that list will be
    automatically created. This should only be used on the client because the server already has
    code that automatically creates plugins using the ConfigManager.
    **/
    template<class T> static void addAutoCreatePlugin()
    {
        ClientServerPluginManager::addAutoCreatePlugin( T::getTypeStatic() );
    }
    /**
    Adds a plugin to the auto create plugin list. All plugin in that list will be
    automatically created. This should only be used on the client because the server already has
    code that automatically creates plugins using the ConfigManager.
    
    @param  type    The type of the plugin. 
    **/
    static void addAutoCreatePlugin( ClientServerPluginTypeEnum type );
    /**
    Query if given plugin type will be automatically created on plugin manager creation
    
    @param  type    The plugin type. 
    
    @return True if given plugin type will be automatically created on plugin manager creation, 
            false otherwise.       
    **/
    static bool hasAutoCreatePlugin( ClientServerPluginTypeEnum type );
    /**
    Gets the plugin state. 
    **/
    inline PluginState getState() const { return mPluginState; }
    /**
    Sets the plugin state. 
    
    @param  state   The plugin state to set.
    **/
    void setState( PluginState state );
    /**
    Stores the state of all plugins.
    **/
    void storeState();
    /**
    Restores the last stored state.
    **/
    void restoreState();

    /**
    Gets the replica manager. 
    **/
    inline RakNet::ReplicaManager3& getReplicaManager() const { return mReplicaManager; }
    /**
    Gets the network ID manager. 
    **/
    inline RakNet::NetworkIDManager& getNetworkIDManager() const { return mNetworkIDManager; }
    /**
    Gets the mode. 
    **/
    inline Mode getMode() const { return mMode; }
    /**
    Gets the frame/tick update signal.
    **/
    inline sigc::signal<void>& getUpdateSignal() { return mUpdateSignal; }

    /**
    Connects a slot to the client-server plugin created/destroyed signal. 
    
    @param [in,out] rSlot   The slot (signature: void func(ClientServerPluginTypeEnum&, 
                            bool [true when plugin is created, false when destroyed])) to connect. 
    
    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connect( const sigc::slot<void, ClientServerPlugin&, bool>& rSlot ) 
    {
        return mPluginSignal.connect( rSlot ); 
    }
    /**
    Connects a slot to the plugin state changed signal.
    
    @param [in,out] rSlot   The slot (signature: void func(PluginState [new state], PluginState 
    [previous state])) to connect. 
    
    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connectPluginStateChange( const sigc::slot<void, PluginState, PluginState>& rSlot ) 
    {
        return mPluginStateChange.connect( rSlot ); 
    }

protected:
    /**
    Gets the map of client-server plugins.
    **/
    inline ClientServerPlugins& getPluginsMutable() { return mPlugins; }

private:
    friend class Shared::Bindings::CampBindings;    ///< Allow private access for camp bindings.

    /**
    Creates and destroys client-server plugins.
    **/
    void update();

    Mode                                            mMode;
    PluginState                                     mPluginState;
    PluginState                                     mPrevPluginState;
    sigc::signal<void, PluginState, PluginState>    mPluginStateChange;
    SerializationFile*                              mStoredState;
    ClientServerPlugins                             mPlugins;
    ClientServerPluginTypes                         mDestroyedPlugins;
    ClientServerPluginTypes                         mCreatedPlugins;
    sigc::signal<void, ClientServerPlugin&, bool>   mPluginSignal;
    static ClientServerPluginTypes                  mAutoCreatePlugins;

    sigc::signal<void>&                             mUpdateSignal;
    sigc::connection                                mUpdateConnection;

    RakNet::RakPeerInterface&   mRakPeer;
    RakNet::ReplicaManager3&    mReplicaManager; 
    RakNet::NetworkIDManager&   mNetworkIDManager;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::ClientServerPluginManager, 
    &Diversia::Shared::Bindings::CampBindings::bindClientServerPluginManager );

#endif // DIVERSIA_SHARED_CLIENTSERVERPLUGINMANAGER_H