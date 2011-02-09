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

#include "Platform/StableHeaders.h"

#include "User/UserManager.h"
#include "User/Group.h"
#include "User/User.h"

namespace Diversia
{
namespace Server 
{
//------------------------------------------------------------------------------

UserManager::UserManager()
{
    // Add guest template user.
    mUsers.insert( std::make_pair( "Guest", new User( "Guest", "", true ) ) );
}

UserManager::~UserManager()
{
    for( Groups::iterator i = mGroups.begin(); i != mGroups.end(); ++i )
    {
        delete i->second;
    }

    for( Users::iterator i = mUsers.begin(); i != mUsers.end(); ++i )
    {
        delete i->second;
    }
}

Group& UserManager::addGroup( const String& rName )
{
    if( !hasGroup( rName ) )
    {
        Group* group = new Group( rName );
        mGroups.insert( std::make_pair( rName, group ) );
        mGroupSignal( *group, true );
        return *group;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_DUPLICATE_ITEM, "Group already exists.", 
            "UserManager::addGroup" );
    }
}

Group& UserManager::getGroup( const String& rName ) const
{
    Groups::const_iterator i = mGroups.find( rName );
    if( i != mGroups.end() )
    {
        return *i->second;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Group does not exist.", 
            "UserManager::getGroup" );
    }
}

bool UserManager::hasGroup( const String& rName ) const
{
    return mGroups.find( rName ) != mGroups.end();
}

void UserManager::removeGroup( const String& rName )
{
    Groups::iterator i = mGroups.find( rName );
    if( i != mGroups.end() )
    {
        mGroupSignal( *i->second, false );
        delete i->second;
        mGroups.erase( i );
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Group does not exist.", 
            "UserManager::removeGroup" );
    }
}

User& UserManager::addUser( const String& rName, const String& rPassword )
{
    if( !hasUser( rName ) )
    {
        User* user = new User( rName, rPassword );
        mUsers.insert( std::make_pair( rName, user ) );
        mUserSignal( *user, true );
        return *user;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_DUPLICATE_ITEM, "User already exists.", 
            "UserManager::addUser" );
    }
}

User& UserManager::getUser( const String& rName ) const
{
    Users::const_iterator i = mUsers.find( rName );
    if( i != mUsers.end() )
    {
        return *i->second;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "User does not exist.", 
            "UserManager::getUser" );
    }
}

bool UserManager::hasUser( const String& rName ) const
{
    return mUsers.find( rName ) != mUsers.end();
}

void UserManager::removeUser( const String& rName )
{
    Users::iterator i = mUsers.find( rName );
    if( i != mUsers.end() )
    {
        mUserSignal( *i->second, false );
        delete i->second;
        mUsers.erase( i );
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "User does not exist.", 
            "UserManager::removeUser" );
    }
}

User& UserManager::createGuest()
{
    static unsigned int guestCounter = 0;

    std::stringstream ss;
    ss << "Guest" << guestCounter++;

    User* user = new User( getUser( "Guest" ), ss.str(), "" );
    mUsers.insert( std::make_pair( ss.str(), user ) );
    mUserSignal( *user, true );
    return *user;
}

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia