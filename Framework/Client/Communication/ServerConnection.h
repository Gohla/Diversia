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

#ifndef DIVERSIA_CLIENT_SERVERCONNECTION_H
#define DIVERSIA_CLIENT_SERVERCONNECTION_H

#include "Client/Platform/Prerequisites.h"

#include "Shared/Communication/ServerInfo.h"
#include "Shared/Communication/ReplicaManager.h"

namespace Diversia
{
namespace Client
{
//------------------------------------------------------------------------------

class DIVERSIA_CLIENT_API ServerConnection : public sigc::trackable
{
public:
    /**
    Values that represent the state of a connection.
    **/
    enum State
    {
        SOCKETFAIL = 0, ///< Connecting to the server failed, could not create new socket or thread. 
        CONNFAIL,	    ///< Connecting to the server failed, invalid server address/port.
        BANNED,         ///< Connecting to the server failed, client is banned. 
        FULL,           ///< Connecting to the server failed, server is full.
        AUTHFAIL,	    ///< Authentication failed, invalid user/pass.
        DENIED,	        ///< Authentication succeed, but client has no permissions to join the server.
        CONNLOST,	    ///< Server was connected before (past authentication), but connection was lost. 
        DISCONNECTED,	///< Server was connected before (past authentication), but client disconnected. 
        CONNECTING,     ///< Server is connecting. 
        AUTHENTICATING,	///< Server is checking authentication. 
        CONNECTED	    ///< Connected to the server. 
    };

    /**
    Values that represent the type of a state of a connection.
    **/
    enum StateType
    {
        TYPE_NONE = 0,	        ///< No type. 
        TYPE_DISCONNECTED,	    ///< Disconnect from server. 
        TYPE_CONNECTIONFAILED,	///< Connecting to the server failed. 
        TYPE_CONNECTING,	    ///< Server is connecting. 
        TYPE_CONNECTED	        ///< Server is connected. 
    };

    static ServerConnection::StateType stateType( ServerConnection::State state );
    /**
    Constructor. 
    
    @param  rServerInfo             Information describing the server. 
    @param [in,out] rUpdateSignal   The frame/tick update signal. 
    **/
    ServerConnection( const ServerInfo& rServerInfo, sigc::signal<void>& rUpdateSignal );
    /**
    Destructor. 
    **/
    virtual ~ServerConnection();

    /**
    Connects to the server. 
    
    @return True if it succeeds, false if it fails. 
    **/
    bool connect();
    /**
    Disconnects from the server.
    **/
    void disconnect();
    /**
    Gets the server information. 
    **/
    inline const ServerInfo& getServerInfo() const { return mServerInfo; }
    /**
    Gets the connection state. 
    **/
    inline ServerConnection::State getConnectionState() const { return mConnectionState; }
    /**
    Gets the RakPeer. 
    **/
    inline RakNet::RakPeerInterface& getRakPeer() { return mRakPeer; }
    /**
    Gets the network ID manager. 
    **/
    inline RakNet::NetworkIDManager& getNetworkIDManager() { return mNetworkIDManager; }
    /**
    Gets the replica manager. 
    **/
    inline ReplicaManager& getReplicaManager() { return mReplicaManager; }
    /**
    Gets the RPC3 plugin.
    **/
    inline RakNet::RPC3& getRPC3() { return mRPC3; }
    /**
    Connects a slot to the connection state changed signal. 
    
    @param [in,out] rSlot   The slot (signature: void func(ServerConnection::State, 
                            ServerConnection&) to connect. 
    
    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connect( 
        const sigc::slot<void, ServerConnection::State, ServerConnection&>& rSlot ) 
    {
        return mStateChangedSignal.connect( rSlot );
    }

private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    /**
    Receives packets. 
    **/
    void update();
    /**
    Sets the connection state. 
    **/
    void setState( State state );

    ServerInfo                  mServerInfo;
    State                       mConnectionState;

    RakNet::RakPeerInterface&   mRakPeer;
    RakNet::NetworkIDManager    mNetworkIDManager;
    ReplicaManager              mReplicaManager;
    RakNet::RPC3                mRPC3;

    sigc::signal<void, State, ServerConnection&> mStateChangedSignal;

    /**
    Settings for server connection.
    **/
    static struct Settings
    {
        Settings(): 
            mThreadSleepTimer( 10 ),
            mConnectionAttemptCount( 12 ),
            mTimeBetweenConnectionAttemptsMS( 500 ),
            mTimeoutMS( 0 ),
            mShutdownBlockDuractionMS( 5000 ) {}

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
    inline static ServerConnection::Settings& getSettings() { return msSettings; }

};

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Client::ServerConnection::Settings, 
    &Diversia::Client::Bindings::CampBindings::bindServerConnectionSettings );

#endif // DIVERSIA_CLIENT_SERVERCONNECTION_H