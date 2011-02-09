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

#include "User/User.h"

namespace Diversia
{
namespace Server 
{
//------------------------------------------------------------------------------

User::User( const String& rName, const String& rPassword, bool guest /*= false*/ ):
    mName( rName ),
    mPassword( rPassword ),
    mGuest( guest )
{

}

User::User( const User& rUser ):
    mName( rUser.mName ),
    mPassword( rUser.mPassword ),
    mGuest( rUser.mGuest )
{
    // Copy permissions.
    const Permissions& permissions = rUser.getPermissions();
    for( Permissions::const_iterator i = permissions.begin(); i != permissions.end(); ++i )
    {
        addPermission( *i->second );
    }
}

User::User( const User& rUser, const String& rName, const String& rPassword ):
    mName( rName ),
    mPassword( rPassword ),
    mGuest( rUser.mGuest )
{
    // Copy permissions.
    const Permissions& permissions = rUser.getPermissions();
    for( Permissions::const_iterator i = permissions.begin(); i != permissions.end(); ++i )
    {
        addPermission( *i->second );
    }
}

User::~User()
{
    for( Permissions::iterator i = mPermissions.begin(); i != mPermissions.end(); ++i )
    {
        delete i->second;
    }
}

Permission& User::addPermission( const String& rName, bool allowed, 
    unsigned int maxItemsAllowed /*= MAXUINT*/, unsigned int maxItemsPerTime /*= MAXUINT*/, 
    double timeSeconds /*= MAXDOUBLE */ )
{
    if( !hasPermission( rName ) )
    {
        Permission* permission = new Permission( rName, allowed, maxItemsAllowed, maxItemsPerTime,
            timeSeconds );
        mPermissions.insert( std::make_pair( rName, permission ) );
        return *permission;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_DUPLICATE_ITEM, "Permission already exists.", 
            "User::addPermission" );
    }
}

Permission& User::addPermission( const Permission& rPermission )
{
    if( !hasPermission( rPermission.getName() ) )
    {
        Permission* permission = new Permission( rPermission );
        mPermissions.insert( std::make_pair( permission->getName(), permission ) );
        return *permission;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_DUPLICATE_ITEM, "Permission already exists.", 
            "User::addPermission" );
    }
}

Permission& User::getPermission( const String& rName ) const
{
    Permissions::const_iterator i = mPermissions.find( rName );
    if( i != mPermissions.end() )
    {
        return *i->second;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Permission does not exist.", 
            "User::getPermission" );
    }
}

bool User::hasPermission( const String& rName ) const
{
    return mPermissions.find( rName ) != mPermissions.end();
}

void User::removePermission( const String& rName )
{
    Permissions::iterator i = mPermissions.find( rName );
    if( i != mPermissions.end() )
    {
        delete i->second;
        mPermissions.erase( i );
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Permission does not exist.", 
            "User::removePermission" );
    }
}

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia