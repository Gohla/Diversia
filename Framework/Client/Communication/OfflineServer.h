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

#ifndef DIVERSIA_CLIENT_OFFLINESERVER_H
#define DIVERSIA_CLIENT_OFFLINESERVER_H

#include "Client/Platform/Prerequisites.h"
#include "Client/Communication/ServerAbstract.h"
#include "Shared/Communication/GridPosition.h"
#include "Shared/Communication/ServerInfo.h"
#include "Shared/Communication/UserInfo.h"

namespace Diversia
{
namespace Client
{
//------------------------------------------------------------------------------

class DIVERSIA_CLIENT_API OfflineServer : public ServerAbstract
{
public:
    /**
    Constructor. 

    @param [in,out] rUpdateSignal   The frame/tick update signal. 
    **/
    OfflineServer( sigc::signal<void>& rUpdateSignal );
    /**
    Destructor. 
    **/
    ~OfflineServer();

    /**
    Gets the server state. 
    **/
    inline ServerState getServerState() { return mServerState; }
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
    void pluginLoadingComplete();

    ServerState                             mServerState;
    GridPosition                            mGridPosition;
    ServerInfo                              mServerInfo;
    UserInfo                                mUserInfo;
    sigc::signal<void>                      mDummySignal;
    boost::scoped_ptr<ServerConnection>     mServerConnection;
    boost::scoped_ptr<ClientPluginManager>  mPluginManager;

    sigc::connection mLoadingConnection;
    sigc::signal<void, ServerState, ServerAbstract&> mStateChangedSignal;
    
};

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia

#endif // DIVERSIA_CLIENT_OFFLINESERVER_H