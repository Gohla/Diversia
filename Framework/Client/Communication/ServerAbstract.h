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

#ifndef DIVERSIA_CLIENT_SERVERBASE_H
#define DIVERSIA_CLIENT_SERVERBASE_H

#include "Client/Platform/Prerequisites.h"

namespace Diversia
{
namespace Client
{
//------------------------------------------------------------------------------

/**
Values that represent the state of a server.
**/
enum ServerState
{
    OFFLINE = 0,    ///< Server is an offline server.
    DISCOVERED,     ///< Server has just been discovered and added to the grid. 
    CONNECTING,     ///< Server is connecting. 
    LOADING,        ///< Server is loading plugins.
    HALFCONNECTED,	///< Connected to the server, but not receiving data yet. (Server is far away) 
    CONNECTED,	    ///< Connected to the server.
    CONNECTEDACTIVE	///< Connected to the server, and is the active server.
};

class DIVERSIA_CLIENT_API ServerAbstract
{
public:
    /**
    Default constructor. 
    **/
    ServerAbstract() {}
    /**
    Destructor. 
    **/
    virtual ~ServerAbstract() {}

    /**
    Connects to the server in half connected state.
    
    @return True if it succeeds, false if it fails. 
    **/
    virtual bool connect() { return true; }
    /**
    Disconnects from the server. 
    **/
    virtual void disconnect() {}
    /**
    Sets the state to half connected. 
    **/
    virtual void setStateHalfConnected() {}
    /**
    Sets the state to connected. 
    **/
    virtual void setStateConnected() {}
    /**
    Sets the state to connected active. 
    **/
    virtual void setStateConnectedActive() {}

    /**
    Gets the server state. 
    **/
    virtual ServerState getServerState() { return OFFLINE; }
    /**
    Gets the grid position of the server.
    **/
    virtual const GridPosition& getGridPosition() const = 0;
    /**
    Gets the server information. 
    **/
    virtual const ServerInfo& getServerInfo() const = 0;
    /**
    Gets the user information.  
    **/
    virtual const UserInfo& getUserInfo() const = 0;
    /**
    Gets the server connection. 
    **/
    virtual ServerConnection& getServerConnection() = 0;
    /**
    Gets the plugin manager. 
    **/
    virtual ServerPluginManager& getPluginManager() = 0;

    /**
    Connects a slot to the server state changed signal. 
    
    @param [in,out] rSlot   The slot (signature: void func(ServerState, ServerAbstract&) to connect. 
    
    @return Connection object to block or disconnect the connection.
    **/
    virtual sigc::connection connect( const sigc::slot<void, ServerState, ServerAbstract&>& rSlot ) = 0;

};

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia

#endif // DIVERSIA_CLIENT_SERVERBASE_H