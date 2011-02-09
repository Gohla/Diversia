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

#ifndef DIVERSIA_SERVER_SESSIONMANAGER_H
#define DIVERSIA_SERVER_SESSIONMANAGER_H

#include "Platform/Prerequisites.h"

#include "User/Session.h"

namespace Diversia
{
namespace Server
{
//------------------------------------------------------------------------------

typedef std::map<RakNet::RakNetGUID, Session*> Sessions;

class SessionManager : public sigc::trackable
{
public:
    /**
    Default constructor. 
    **/
    SessionManager( UserManager& rUserManager, ClientPluginManager& rPluginManager, 
        sigc::signal<void>& rUpdateSignal );
    /**
    Destructor. 
    **/
    ~SessionManager();

    /**
    Adds a session. 
    
    @param  guid            The unique session ID. 
    @param [in,out] rUser   The user that's associated with the session. 
    
    @return The created session. 
    **/
    Session& addSession( RakNet::RakNetGUID guid, User& rUser );
    /**
    Gets a session. 
    
    @param  guid   The unique session ID.
    **/
    Session& getSession( RakNet::RakNetGUID guid ) const;
    /**
    Gets the sessions. 
    **/
    inline const Sessions& getSessions() const { return mSessions; }
    /**
    Query if a session exists.
    
    @param  guid   The unique session ID.
    
    @return True if the session exists, false if not. 
    **/
    bool hasSession( RakNet::RakNetGUID guid ) const;
    /**
    Removes a session by name.
    
    @param  guid   The unique session ID.
    **/
    void removeSession( RakNet::RakNetGUID guid );
    /**
    Removes all sessions.
    **/
    void clear();

    /**
    Gets the user manager. 
    **/
    inline UserManager& getUserManager() const { return mUserManager; }

    /**
    Connects a slot to the session added/removed signal. 
    
    @param [in,out] rSlot   The slot (signature: void func(Session&, bool [true when session is
                            added, false when removed])) to connect. 
    
    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connect( const sigc::slot<void, Session&, bool>& rSlot ) 
    { 
        return mSignal.connect( rSlot ); 
    }

    
private:
    /**
    Removes a guest user once all replica's are destroyed.
    **/
    void update();

    Sessions                            mSessions;

    sigc::signal<void, Session&, bool>  mSignal;
    sigc::connection                    mUpdateConnection;
    std::set<RakNet::RakNetGUID>        mGuestsToDestroy;

    UserManager&                        mUserManager;
    ClientPluginManager&                mPluginManager;
};

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia

#endif // DIVERSIA_SERVER_SESSIONMANAGER_H
