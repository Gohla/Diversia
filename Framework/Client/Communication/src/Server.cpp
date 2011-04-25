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

#include "Client/Platform/StableHeaders.h"

#include "Client/Communication/Server.h"
#include "Client/Object/ClientObjectManager.h"
#include "Client/ClientServerPlugin/ServerPluginManager.h"
#include "Shared/ClientServerPlugin/ClientServerPlugin.h"
#include "Shared/Communication/ReplicaManager.h"

namespace Diversia
{
namespace Client 
{
//------------------------------------------------------------------------------

Server::Server( const GridPosition& rGridPosition, const ServerInfo& rServerInfo,
    const UserInfo& rUserInfo, sigc::signal<void>& rUpdateSignal ):
    mGridPosition( rGridPosition ),
    mServerInfo( rServerInfo ),
    mUserInfo( rUserInfo ),
    mServerState( DISCOVERED ),
    mTargetState( DISCOVERED ),
    mServerConnection( new ServerConnection( rServerInfo, rUpdateSignal ) )
{
    mPluginManager.reset( new ServerPluginManager( CLIENT, STOP, rUpdateSignal, *this,
        mServerConnection->getRakPeer(), mServerConnection->getReplicaManager(), 
        mServerConnection->getNetworkIDManager() ) );

    mServerConnection->getReplicaManager().setPluginManager( *mPluginManager );

    // Subscribe for connection state changes.
    mServerConnection->connect( sigc::mem_fun( this, &Server::connectionStateChanged ) );

    // Subscribe for plugin creation, the object manager needs to be set in the replica manager.
    mPluginChangeConnection = mPluginManager->connect( sigc::mem_fun( this, 
        &Server::pluginChange ) );

    // Subscribe for plugin loading completion.
    mPluginManager->connectLoadingComplete( sigc::mem_fun( this, &Server::pluginLoadingComplete ) );
}

Server::~Server()
{

}

bool Server::connect()
{
    if( !isConnected() && mServerConnection->connect() )
    {
        Server::setState( CONNECTING );
        mTargetState = HALFCONNECTED;
        return true;
    }

    // Return true if the server is already connected, false if it was not already connected.
    return isConnected();
}

void Server::disconnect()
{
    if( isConnected() || isConnecting() )
    {
        mServerConnection->disconnect();
        Server::setState( DISCOVERED );
        mTargetState = DISCOVERED;
    }
}

void Server::setStateHalfConnected()
{
    if( isConnected() )
    {
        if( mServerState != HALFCONNECTED )
            Server::setState( HALFCONNECTED );
    }
    else if( !isConnecting() )
    {
        // Server is disconnected, try to connect.
        Server::connect();
    }
}

void Server::setStateConnected()
{
    if( isConnected() )
    {
        if( mServerState != CONNECTED )
            Server::setState( CONNECTED );
    }
    else
    {
        if( !isConnecting() )
            Server::connect();

        mTargetState = CONNECTED;
    }
}

void Server::setStateConnectedActive()
{
    if( isConnected() )
    {
        if( mServerState != CONNECTEDACTIVE )
            Server::setState( CONNECTEDACTIVE );
    }
    else
    {
        if( !isConnecting() )
            Server::connect();

        mTargetState = CONNECTEDACTIVE;
    }
}

void Server::setState( ServerState state )
{
    mServerState = state;
    mPluginManager->setServerState( state );
    mStateChangedSignal( state, *this );
}

void Server::connectionStateChanged( ServerConnection::State state, 
    ServerConnection& rServerConnection )
{
    ServerConnection::StateType type = ServerConnection::stateType( state );
    if( isConnected() || isLoading() )
    {
        // See if the state was set to disconnected.
        if( type == ServerConnection::TYPE_DISCONNECTED )
        {
            // Connection was lost, set state to discovered.
            Server::setState( DISCOVERED );
            mTargetState = DISCOVERED;
        }
    }
    else if( isConnecting() )
    {
        // See if the state was set to connected or a failed state.
        if( type == ServerConnection::TYPE_CONNECTED )
        {
            // Connection successful, set server state to loading.
            Server::setState( LOADING );
        }
        else if( type == ServerConnection::TYPE_CONNECTIONFAILED )
        {
            // Connecting failed, set state to discovered.
            Server::setState( DISCOVERED );
            mTargetState = DISCOVERED;
        }
    }
}

void Server::pluginChange( ClientServerPlugin& rPlugin, bool created )
{
    if( created && rPlugin.getType() == CLIENTSERVERPLUGINTYPE_OBJECTMANAGER )
    {
        mServerConnection->getReplicaManager().setObjectManager( 
            static_cast<ClientObjectManager&>( rPlugin ) );
        mPluginChangeConnection.disconnect();
    }
}

void Server::pluginLoadingComplete()
{
    if( Server::isLoading() )
    {
        // Done loading, set state to target state.
        Server::setState( mTargetState );
    }
}

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia