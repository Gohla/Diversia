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

#ifndef DIVERSIA_CLIENT_GRIDMANAGER_H
#define DIVERSIA_CLIENT_GRIDMANAGER_H

#include "Client/Platform/Prerequisites.h"

#include "Shared/Communication/GridPosition.h"
#include "Client/Communication/Server.h"

namespace Diversia
{
namespace Client
{
//------------------------------------------------------------------------------

typedef std::map<GridPosition, ServerAbstract*> ServerGrid;
typedef std::set<ServerAbstract*> Servers;

class DIVERSIA_CLIENT_API GridManager : public boost::noncopyable, public sigc::trackable
{
public:
    /**
    Constructor. 
    
    @param [in,out] rUpdateSignal   The frame/tick update signal. 
    **/
    GridManager( sigc::signal<void>& rUpdateSignal );
    /**
    Destructor. 
    **/
    virtual ~GridManager();

    /**
    Creates a dummy server for offline mode.
    
    @return The created server. 
    **/
    ServerAbstract& createOfflineServer();
    /**
    Creates a server. 
    
    @param  rGridPosition   The grid position. 
    @param  rServerInfo     Information describing the server.
    @param  rUserInfo       Information describing the user. 

    @throw  Exception       When a server already exists on given grid position. 
    
    @return The created server. 
    **/
    ServerAbstract& createServer( const GridPosition& rGridPosition, const ServerInfo& rServerInfo,
        const UserInfo& rUserInfo );
    /**
    Gets a server by grid position.
    
    @param  rGridPosition   The grid position. 

    @throw  Exception       When server doesn't exist.
    **/
    ServerAbstract& getServer( const GridPosition& rGridPosition );
    /**
    Gets the active server. 

    @throw  Exception       When active server hasn't been set yet.
    **/
    ServerAbstract& getActiveServer();
    /**
    Gets the server grid.
    **/
    inline const ServerGrid& getServers() { return mServerGrid; }
    /**
    Query if a server exists.
    
    @param  rGridPosition   The grid position. 
    
    @return True if server exists, false if not. 
    **/
    bool hasServer( const GridPosition& rGridPosition );
    /**
    Destroys a server by name. The server will be removed from the grid in the next update tick.
    
    @param  rGridPosition   The grid position. 

    @throw  Exception       When server doesn't exist.
    **/
    void destroyServer( const GridPosition& rGridPosition );
    /**
    Destroys all servers and resets the grid state to the initial state.
    **/
    void clear();
    /**
    Sets the avatar object to check for a server switch.
    
    @param [in,out] rObject The avatar object. 
    **/
    inline void setAvatar( Object& rObject ) { mAvatar = &rObject; }
    /**
    Gets the avatar object.

    @throw  Exception       When avatar object hasn't been set yet.
    **/
    Object& getAvatar();

    /**
    Connects a slot to the server created/destroyed signal. 
    
    @param [in,out] rSlot   The slot (signature: void func(ServerAbstract&, bool [true when server is 
    created, false when destroyed])) to connect. 
    
    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connectServerChange( const sigc::slot<void, ServerAbstract&, bool>& rSlot ) 
    {
        return mServerChangeSignal.connect( rSlot );
    }
    /**
    Connects a slot to the loading completed signal. 
    
    @param [in,out] rSlot   The slot (signature: void func()) to connect. 
    
    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connectLoadingCompleted( const sigc::slot<void>& rSlot ) 
    {
        return mLoadingCompletedSignal.connect( rSlot );
    }
    /**
    Connects a slot to the active server disconnect signal. 
    
    @param [in,out] rSlot   The slot (signature: void func(ServerAbstract& [active server])) to connect. 
    
    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connectActiveServerDisconnect( const sigc::slot<void, ServerAbstract&>& rSlot ) 
    {
        return mActiveServerDisconnectSignal.connect( rSlot );
    }
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    void update();
    void serverStateChanged( ServerState state, ServerAbstract& rServer );
    void setActiveServer( const GridPosition& rGridPosition );
    void setState( const GridPosition& rGridPosition );

    ServerGrid                          mServerGrid;
    Servers                             mRemovedServers;
    ServerAbstract*                     mActiveServer;
    Object*                             mAvatar;
    Vector3                             mAvatarLastPosition;
    boost::scoped_ptr<boost::timer>     mSwitchTimer;
    bool                                mSwitching;
    std::set<GridPosition>              mLoadingServers;
    bool                                mLoadingSignalEmitted;

    sigc::signal<void, ServerAbstract&, bool>   mServerChangeSignal;
    sigc::signal<void>                          mLoadingCompletedSignal;
    sigc::signal<void, ServerAbstract&>         mActiveServerDisconnectSignal;
    sigc::signal<void>&                         mUpdateSignal;

    // Options
    unsigned short  mConnectRange;
    unsigned short  mHalfConnectRange;
    Real            mSwitchStayTimeS;
    
};

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Client::GridManager, 
    &Diversia::Client::Bindings::CampBindings::bindGridManager );

#endif // DIVERSIA_CLIENT_GRIDMANAGER_H