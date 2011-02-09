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

#ifndef DIVERSIA_SERVER_USERMANAGER_H
#define DIVERSIA_SERVER_USERMANAGER_H

#include "Platform/Prerequisites.h"

namespace Diversia
{
namespace Server
{
//------------------------------------------------------------------------------

typedef std::map<String, Group*> Groups;
typedef std::map<String, User*> Users;

class UserManager
{
public:
    /**
    Default constructor. 
    **/
    UserManager();
    /**
    Destructor. 
    **/
    ~UserManager();

    /**
    Adds a group. 
    
    @param  rName   The name of the group.
    
    @return The created group. 
    **/
    Group& addGroup( const String& rName );
    /**
    Gets a group. 
    
    @param  rName   The name of the group.
    **/
    Group& getGroup( const String& rName ) const;
    /**
    Gets the groups. 
    **/
    inline const Groups& getGroups() const { return mGroups; }
    /**
    Query if a group exists.
    
    @param  rName   The name of the group. 
    
    @return True if the group exists, false if not. 
    **/
    bool hasGroup( const String& rName ) const;
    /**
    Removes a group by name.
    
    @param  rName   The name of the group. 
    **/
    void removeGroup( const String& rName );
    /**
    Adds a user. 
    
    @param  rName       The name of the user.
    
    @return The created user. 
    **/
    inline User& addUser( const String& rName ) { return UserManager::addUser( rName, "" ); }
    /**
    Adds a user. 
    
    @param  rName       The name of the user.
    @param  rPassword   The password of the user.
    
    @return The created user. 
    **/
    User& addUser( const String& rName, const String& rPassword );
    /**
    Gets a user. 
    
    @param  rName   The name of the user.
    **/
    User& getUser( const String& rName ) const;
    /**
    Gets the users. 
    **/
    inline const Users& getUsers() const { return mUsers; }
    /**
    Query if a user exists.
    
    @param  rName   The name of the user. 
    
    @return True if the user exists, false if not. 
    **/
    bool hasUser( const String& rName ) const;
    /**
    Removes a user by name.
    
    @param  rName   The name of the user. 
    **/
    void removeUser( const String& rName );

    /**
    Creates a temporary guest user that copies permissions from the guest user.
    **/
    User& createGuest();

    /**
    Connects a slot to the group added/removed signal. 
    
    @param [in,out] rSlot   The slot (signature: void func(Group&, bool [true when group is
                            added, false when removed])) to connect. 
    
    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connectGroup( const sigc::slot<void, Group&, bool>& rSlot ) 
    { 
        return mGroupSignal.connect( rSlot ); 
    }
    /**
    Connects a slot to the user added/removed signal. 
    
    @param [in,out] rSlot   The slot (signature: void func(User&, bool [true when user is
                            added, false when removed])) to connect. 
    
    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connectUser( const sigc::slot<void, User&, bool>& rSlot ) 
    { 
        return mUserSignal.connect( rSlot ); 
    }

    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    Groups                              mGroups;
    Users                               mUsers;

    sigc::signal<void, Group&, bool>    mGroupSignal;
    sigc::signal<void, User&, bool>     mUserSignal;

};

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Server::UserManager, 
    &Diversia::Server::Bindings::CampBindings::bindUserManager );

#endif // DIVERSIA_SERVER_USERMANAGER_H