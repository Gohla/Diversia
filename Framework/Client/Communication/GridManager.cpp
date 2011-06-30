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

#include "Client/Communication/GridManager.h"
#include "Client/Communication/OfflineServer.h"
#include "Client/Communication/ServerAbstract.h"
#include "Object/Object.h"
#include "Shared/Communication/ServerPosition.h"
#include <boost/timer.hpp>

namespace Diversia
{
namespace Client 
{
//------------------------------------------------------------------------------

GridManager::GridManager( sigc::signal<void>& rUpdateSignal ):
    mActiveServer( 0 ),
    mAvatar( 0 ),
    mAvatarLastPosition( Vector3::ZERO ),
    mSwitchTimer( new boost::timer() ),
    mUpdateSignal( rUpdateSignal ),
    mConnectRange( 1 ),
    mHalfConnectRange( 2 ),
    mSwitchStayTimeS( 3.0 ),
    mLoadingSignalEmitted( false )
{
    mUpdateSignal.connect( sigc::mem_fun( this, &GridManager::update ) );
}

GridManager::~GridManager()
{
    for( ServerGrid::iterator i = mServerGrid.begin(); i != mServerGrid.end(); ++i )
    {
        mServerChangeSignal( *i->second, false );
        delete i->second;
    }
}

ServerAbstract& GridManager::createOfflineServer()
{
    GridManager::clear();
    OfflineServer* server = new OfflineServer( mUpdateSignal );
    mServerGrid.insert( std::make_pair( ServerPosition( 0, 0 ), server ) );
    mActiveServer = server;
    server->connect( sigc::mem_fun( this, &GridManager::serverStateChanged ) );
    mServerChangeSignal( *server, true );
    return *server;
}

ServerAbstract& GridManager::createServer( const GridPosition& rGridPosition, 
    const ServerInfo& rServerInfo, const UserInfo& rUserInfo )
{
    if( !GridManager::hasServer( rGridPosition ) )
    {
        Server* server = new Server( rGridPosition, rServerInfo, rUserInfo, mUpdateSignal );
        mServerGrid.insert( std::make_pair( rGridPosition, server ) );

        // If the active server hasn't been set, set the first created server to the active server.
        if( !mActiveServer )
        {
            mActiveServer = server;
            mActiveServer->setStateConnectedActive();
        }

        server->connect( sigc::mem_fun( this, &GridManager::serverStateChanged ) );
        mLoadingServers.insert( rGridPosition );
        mServerChangeSignal( *server, true );
        return *server;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_DUPLICATE_ITEM, "Server already exists.", 
            "GridManager::createServer" );
    }
}

ServerAbstract& GridManager::getServer( const GridPosition& rGridPosition )
{
    ServerGrid::iterator i = mServerGrid.find( rGridPosition );
    if( i != mServerGrid.end() )
    {
        return *i->second;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Server does not exist.", 
            "GridManager::getServer" );
    }
}

ServerAbstract& GridManager::getActiveServer()
{
    if( mActiveServer )
        return *mActiveServer;
    else
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Active server has not been set.", 
            "GridManager::getActiveServer" );
}

bool GridManager::hasServer( const GridPosition& rGridPosition )
{
    return mServerGrid.find( rGridPosition ) != mServerGrid.end();
}

void GridManager::destroyServer( const GridPosition& rGridPosition )
{
    ServerGrid::iterator i = mServerGrid.find( rGridPosition );
    if( i != mServerGrid.end() )
    {
        // Destroy the server in the update function.
        mLoadingServers.erase( rGridPosition );
        mRemovedServers.insert( i->second );
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Server does not exist.", 
            "GridManager::destroyServer" );
    }
}

void GridManager::clear()
{
    for( ServerGrid::iterator i = mServerGrid.begin(); i != mServerGrid.end(); ++i )
    {
        // Destroy the server in the update function.
        mLoadingServers.erase( i->second->getGridPosition() );
        mRemovedServers.insert( i->second );
    }

    mLoadingSignalEmitted = false;
}

Object& GridManager::getAvatar()
{
    if( mAvatar )
        return *mAvatar;
    else
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Avatar has not been set.", 
            "GridManager::getAvatar" );
}

void GridManager::update()
{
    // Destroy all removed servers.
    for( Servers::iterator i = mRemovedServers.begin(); i != mRemovedServers.end(); ++i )
    {
        mServerGrid.erase( (*i)->getGridPosition() );

        if( *i == mActiveServer )
        {
            mActiveServer = 0;
            mAvatar = 0;
        }

        delete *i;
    }
    mRemovedServers.clear();

    // Check for server switch.
    if( mActiveServer && mAvatar )
    {
        const Vector3& position = mAvatar->_getDerivedPosition();
        GridPosition avatarGridPos = GridPosition( ServerPosition( position ) );
        GridPosition lastAvatarGridPos = GridPosition( ServerPosition( mAvatarLastPosition ) );

        if( avatarGridPos != lastAvatarGridPos )
        {
            // The avatar crossed a server boundary.
            mSwitching = true;
            mSwitchTimer->restart();
        }
        else if( mSwitching && mSwitchTimer->elapsed() >= mSwitchStayTimeS )
        {
            // The avatar stayed on the other server for mSwitchStayTimeS, set new active server.
            mSwitching = false;
            setActiveServer( avatarGridPos );
        }

        // Don't try to set the new active server to the current active server.
        if( mActiveServer->getGridPosition() == avatarGridPos )
        {
            mSwitching = false;
        }

        mAvatarLastPosition = position;
    }
}

void GridManager::serverStateChanged( ServerState state, ServerAbstract& rServer )
{
    // Remote server from loading servers if it's done loading.
    if( !mLoadingSignalEmitted )
    {
        if( state > LOADING )
        {
            mLoadingServers.erase( rServer.getGridPosition() );
        }

        if( mLoadingServers.empty() )
        {
            mLoadingCompletedSignal();
            mLoadingSignalEmitted = true;
        }
    }

    // Emit a signal when the active server disconnects.
    if( state == DISCOVERED && &rServer == mActiveServer )
    {
        mActiveServerDisconnectSignal( rServer );
    }
}

void GridManager::setActiveServer( const GridPosition& rGridPosition )
{
    if( !mActiveServer || mActiveServer->getGridPosition() != rGridPosition )
    {
        // Change the active server and set the new state for all other servers.
        try
        {
            mActiveServer = &GridManager::getServer( rGridPosition );
            mActiveServer->setStateConnectedActive();
            for( ServerGrid::iterator i = mServerGrid.begin(); i != mServerGrid.end(); ++i )
            {
                if( i->first != rGridPosition )
                    setState( i->first );
            }
        }
        catch ( Exception e )
        {
        	LCLOGE << "Trying to set active server to a server that doesn't exist.";
        }
    }
}

void GridManager::setState( const GridPosition& rGridPosition )
{
    ServerAbstract& server = GridManager::getServer( rGridPosition );
    unsigned short distance = mActiveServer->getGridPosition().distanceBetween( rGridPosition );
    if( distance != 0 )
    {
        if( distance <= mConnectRange )
        {
            server.setStateConnected();
        }
        else if( distance <= mHalfConnectRange )
        {
            server.setStateHalfConnected();
        }
        else
        {
            server.disconnect();
        }
    }
}

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia