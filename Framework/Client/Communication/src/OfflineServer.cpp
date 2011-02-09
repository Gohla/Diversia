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

#include "Client/Communication/OfflineServer.h"
#include "Client/Communication/ServerConnection.h"
#include "Client/ClientServerPlugin/ServerPluginManager.h"

namespace Diversia
{
namespace Client 
{
//------------------------------------------------------------------------------

OfflineServer::OfflineServer( sigc::signal<void>& rUpdateSignal ):
    mServerState( LOADING ),
    mGridPosition( 0, 0 ),
    mServerInfo( "127.0.0.1", 0, "Offline" ),
    mUserInfo( "Offline" ),
    mServerConnection( new ServerConnection( mServerInfo, mDummySignal ) )
{
    mPluginManager.reset( new ServerPluginManager( CLIENT, rUpdateSignal, *this,
        mServerConnection->getRakPeer(), mServerConnection->getReplicaManager(), 
        mServerConnection->getNetworkIDManager(), true ) );

    // Subscribe for plugin loading completion.
    mLoadingConnection = mPluginManager->connectLoadingComplete( sigc::mem_fun( this, 
        &OfflineServer::pluginLoadingComplete ) );
}

OfflineServer::~OfflineServer()
{

}

void OfflineServer::pluginLoadingComplete()
{
    mServerState = CONNECTEDACTIVE;
    mStateChangedSignal( CONNECTEDACTIVE, *this );
    mPluginManager->setServerState( CONNECTEDACTIVE );
    mLoadingConnection.disconnect();
}

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia