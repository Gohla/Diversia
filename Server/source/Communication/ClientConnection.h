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

#ifndef DIVERSIA_SERVER_CLIENTCONNECTION_H
#define DIVERSIA_SERVER_CLIENTCONNECTION_H

#include "Platform/Prerequisites.h"

#include "Shared/Communication/ServerInfo.h"
#include "Shared/Communication/ReplicaManager.h"
#include "User/UserManager.h"

namespace Diversia
{
namespace Server
{
//------------------------------------------------------------------------------

class ClientConnection : public sigc::trackable
{
public:
    /**
    Constructor. 
    
    @param [in,out] rUpdateSignal   The update signal. 
    **/
    ClientConnection( sigc::signal<void>& rUpdateSignal );
    /**
    Destructor. 
    **/
    ~ClientConnection();

    /**
    Gets the plugin manager. 
    **/
    inline ClientPluginManager& getPluginManager() { return *mPluginManager; }
    /**
    Gets the user manager. 
    **/
    inline UserManager& getUserManager() { return mUserManager; }
    /**
    Gets the session manager. 
    **/
    inline SessionManager& getSessionManager() { return *mSessionManager.get(); }

    /**
    Starts listening for clients.

    @return True if it succeeds, false if it fails. 
    **/
    bool listen();
    /**
    Disconnects all clients and stops listening. 
    **/
    void disconnect();
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    /**
    Receives packets. 
    **/
    void update();
    void pluginChange( ClientServerPlugin& rPlugin, bool created );

    ClientPluginManager*                mPluginManager;
    UserManager                         mUserManager;
    boost::scoped_ptr<SessionManager>   mSessionManager;
    sigc::connection                    mPluginChangeConnection;

    RakNet::RakPeerInterface&   mRakPeer;
    RakNet::NetworkIDManager    mNetworkIDManager;
    ReplicaManager              mReplicaManager;
    RakNet::RPC3                mRPC3;

    /**
    Settings for client connection.
    **/
    static struct Settings
    {
        Settings():
            mServerInfo( ServerInfo( "127.0.0.1", 8500, "Diversia server" ) ),
            mMaxConnections( 32 ),
            mThreadSleepTimer( 10 ),
            mConnectionAttemptCount( 12 ),
            mTimeBetweenConnectionAttemptsMS( 500 ),
            mTimeoutMS( 0 ),
            mShutdownBlockDuractionMS( 5000 ) 
        {
            // Add default plugins.
            mPlugins.push_back( CLIENTSERVERPLUGINTYPE_RESOURCEMANAGER );
            mPlugins.push_back( CLIENTSERVERPLUGINTYPE_OBJECTMANAGER );
            mPlugins.push_back( CLIENTSERVERPLUGINTYPE_SERVERNEIGHBORS );
            mPlugins.push_back( CLIENTSERVERPLUGINTYPE_SKY );
            mPlugins.push_back( CLIENTSERVERPLUGINTYPE_GAMEMODE );
            mPlugins.push_back( CLIENTSERVERPLUGINTYPE_TERRAIN );
        }

        std::vector<ClientServerPluginTypeEnum> mPlugins;

        ServerInfo      mServerInfo;
        unsigned short  mMaxConnections;
        unsigned short  mThreadSleepTimer;
        unsigned short  mConnectionAttemptCount;
        unsigned short  mTimeBetweenConnectionAttemptsMS;
        RakNet::Time    mTimeoutMS;	///< 0 uses default value. 
        unsigned short  mShutdownBlockDuractionMS;
    } msSettings;

public:
    /**
    Gets the settings. 
    **/
    inline static ClientConnection::Settings& getSettings() { return msSettings; }

};

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Server::ClientConnection, 
    &Diversia::Server::Bindings::CampBindings::bindClientConnection );

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Server::ClientConnection::Settings, 
    &Diversia::Server::Bindings::CampBindings::bindClientConnectionSettings );

#endif // DIVERSIA_SERVER_CLIENTCONNECTION_H