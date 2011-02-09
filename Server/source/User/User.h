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

#ifndef DIVERSIA_SERVER_USER_H
#define DIVERSIA_SERVER_USER_H

#include "Platform/Prerequisites.h"

#include "Shared/Permission/Permission.h"

namespace Diversia
{
namespace Server
{
//------------------------------------------------------------------------------

typedef std::map<String, Permission*> Permissions;

class User
{
public:
    /**
    Constructor. 
    
    @param  rName       The user name. 
    @param  rPassword   The user's password. 
    @param  guest       True if this is a guest user. 
    **/
    User( const String& rName, const String& rPassword, bool guest = false );
    /**
    Copy constructor. 
    **/
    User( const User& rUser );
    /**
    Copy constructor with name and password.

    @param  rName       The user name. 
    @param  rPassword   The user's password. 
    **/
    User( const User& rUser, const String& rName, const String& rPassword );
    /**
    Destructor. 
    **/
    ~User();

    /**
    Adds a permission. 
    
    @param  rName               The name of the permission.
    @param  allowed             True to allow the action, false to deny.
    @param  maxItemsAllowed     The maximum items allowed. 
    @param  maxItemsPerTime     The maximum items allowed per given time frame. 
    @param  timeSeconds         The time frame in seconds.
    @param  rLowerBounds        The lower bounds. 
    @param  rUpperBounds        The upper bounds. 
    @param  adjustBounds        True to adjust bounds, false to deny when value is out of bounds.
    
    @return The created permission. 
    **/
    template <typename T> Permission& addPermission( const String& rName, bool allowed, 
        unsigned int maxItemsAllowed, unsigned int maxItemsPerTime, double timeSeconds,
        const T& rLowerBounds, const T& rUpperBounds, bool adjustBounds )
    {
        if( !hasPermission( rName ) )
        {
            Permission* permission = new Permission( rName, allowed, maxItemsAllowed, maxItemsPerTime,
                timeSeconds, rLowerBounds, rUpperBounds, adjustBounds );
            mPermissions.insert( std::make_pair( rName, permission ) );
            return *permission;
        }
        else
        {
            DIVERSIA_EXCEPT( Exception::ERR_DUPLICATE_ITEM, "Permission already exists.", 
                "User::addPermission" );
        }
    }
    /**
    Adds a permission. 
    
    @param  rName               The name of the permission.
    @param  allowed             True to allow the action, false to deny.
    @param  maxItemsAllowed     The maximum items allowed. 
    @param  maxItemsPerTime     The maximum items allowed per given time frame. 
    @param  timeSeconds         The time frame in seconds.
    
    @return The created permission. 
    **/
    Permission& addPermission( const String& rName, bool allowed, 
        unsigned int maxItemsAllowed = MAXUINT, unsigned int maxItemsPerTime = MAXUINT, 
        double timeSeconds = 9999999999 );
    /**
    Adds a permission with default values.
    
    @param  rName               The name of the permission.
    
    @return The created permission. 
    **/
    inline Permission& addPermission( const String& rName ) { return User::addPermission( rName, false ); }
    /**
    Adds a permission. Creates a copy of the given permission.
    
    @param  rPermission The permission to copy.
    
    @return The created (copied) permission. 
    **/
    Permission& addPermission( const Permission& rPermission );
    /**
    Gets a permission. 
    
    @param  rName   The name of the permission.
    **/
    Permission& getPermission( const String& rName ) const;
    /**
    Gets the permissions. 
    **/
    inline const Permissions& getPermissions() const { return mPermissions; }
    /**
    Query if a permission exists.
    
    @param  rName   The name of the permission. 
    
    @return True if the permission exists, false if not. 
    **/
    bool hasPermission( const String& rName ) const;
    /**
    Removes a permission by name.
    
    @param  rName   The name of the permission. 
    **/
    void removePermission( const String& rName );

    /**
    Gets the user name. 
    **/
    inline const String& getName() const { return mName; }
    /**
    Query if this user is a guest.  
    **/
    inline bool isGuest() const { return mGuest; }

private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    String      mName;
    String      mPassword;
    bool        mGuest;

    Permissions mPermissions;

};

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Server::User, 
    &Diversia::Server::Bindings::CampBindings::bindUser );

#endif // DIVERSIA_SERVER_USER_H