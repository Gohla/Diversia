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

#ifndef DIVERSIA_CLIENT_SERVER_H
#define DIVERSIA_CLIENT_SERVER_H

#include "Client/Platform/Prerequisites.h"

#include "Shared/Communication/GridPosition.h"
#include "Shared/Communication/ServerInfo.h"
#include "Shared/Communication/UserInfo.h"
#include "Client/Communication/ServerConnection.h"
#include "Client/Communication/ServerAbstract.h"

namespace Diversia
{
namespace Client
{
//------------------------------------------------------------------------------

class DIVERSIA_CLIENT_API Server : public ServerAbstract, public sigc::trackable
{
public:
    /**
    Constructor. 
    
    @param  rGridPosition           The server position. 
    @param  rServerInfo             Information describing the server. 
    @param  rUserInfo               Information describing the user. 
    @param [in,out] rUpdateSignal   The frame/tick update signal. 
    **/
    Server( const GridPosition& rGridPosition, const ServerInfo& rServerInfo, 
        const UserInfo& rUserInfo, sigc::signal<void>& rUpdateSignal );
    /**
    Destructor. 
    **/
    ~Server();

    /**
    Connects to the server in half connected state.
    
    @return True if it succeeds, false if it fails. 
    **/
    bool connect();
    /**
    Disconnects from the server. 
    **/
    void disconnect();
    /**
    Sets the state to half connected. 
    **/
    void setStateHalfConnected();
    /**
    Sets the state to connected. 
    **/
    void setStateConnected();
    /**
    Sets the state to connected active. 
    **/
    void setStateConnectedActive();

    /**
    Gets the server state. 
    **/
    inline ServerState getServerState() const { return mServerState; }
    /**
    Gets the grid position of the server.
    **/
    inline const GridPosition& getGridPosition() const { return mGridPosition; }
    /**
    Gets the server information. 
    **/
    inline const ServerInfo& getServerInfo() const { return mServerInfo; }
    /**
    Gets the user information.  
    **/
    inline const UserInfo& getUserInfo() const { return mUserInfo; }
    /**
    Gets the server connection. 
    **/
    inline ServerConnection& getServerConnection() { return *mServerConnection; }
    /**
    Gets the plugin manager. 
    **/
    inline ClientPluginManager& getPluginManager() { return *mPluginManager; }
    /**
    Query if the server connected. (Server state >= HALFCONNECTED)
    
    @return True if connected, false if not. 
    **/
    inline bool isConnected() { return mServerState >= HALFCONNECTED; }
    /**
    Query if the server is loading. (Server state == LOADING )
    
    @return True if loading, false if not. 
    **/
    inline bool isLoading() { return mServerState == LOADING; }
    /**
    Query if the server is connecting. (Server state == CONNECTING )
    
    @return True if connecting, false if not. 
    **/
    inline bool isConnecting() { return mServerState == CONNECTING; }

    void create();

    /**
    Connects a slot to the server state changed signal. 
    
    @param [in,out] rSlot   The slot (signature: void func(ServerState, ServerAbstract&) to connect. 
    
    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connect( const sigc::slot<void, ServerState, ServerAbstract&>& rSlot ) 
    {
        return mStateChangedSignal.connect( rSlot );
    }
    
private:
    /**
    Sets the state of the server
    
    @param  state   The state. 
    **/
    void setState( ServerState state );

    void connectionStateChanged( ServerConnection::State state, 
        ServerConnection& rServerConnection );
    void pluginChange( Plugin& rPlugin, bool created );
    void pluginLoadingComplete();

    GridPosition                            mGridPosition;
    ServerInfo                              mServerInfo;
    UserInfo                                mUserInfo;
    ServerState                             mServerState;
    ServerState                             mTargetState;
    boost::scoped_ptr<ServerConnection>     mServerConnection;
    boost::scoped_ptr<ClientPluginManager>  mPluginManager;

    sigc::signal<void, ServerState, ServerAbstract&>    mStateChangedSignal;
    sigc::connection                                    mPluginChangeConnection;
};

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia

#endif // DIVERSIA_CLIENT_SERVER_H