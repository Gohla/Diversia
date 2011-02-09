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

#include "Permission/PermissionManager.h"

namespace Diversia
{
namespace TestClient 
{
//------------------------------------------------------------------------------

PermissionManager::PermissionManager( Mode mode, ClientServerPluginManager& rPluginManager, 
    RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
    NetworkIDManager& rNetworkIDManager ):
    ServerPlugin( mode, rPluginManager, rRakPeer, rReplicaManager, rNetworkIDManager,
        camp::UserObject( this ) )
{
    LOGD << "PermissionManager created.";
}

PermissionManager::~PermissionManager()
{

}

template <typename T> Permission& PermissionManager::addPermission( const String& rName, 
    bool allowed, unsigned int maxItemsAllowed, unsigned int maxItemsPerTime, double timeSeconds,
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
            "PermissionManager::addPermission" );
    }
}

void PermissionManager::setPermissions( Permissions& rPermissions )
{
    mPermissions.swap( rPermissions );
}

Permission& PermissionManager::addPermission( const String& rName, bool allowed, 
    unsigned int maxItemsAllowed /*= MAXUINT*/, unsigned int maxItemsPerTime /*= MAXUINT*/, 
    double timeSeconds /*= MAXDOUBLE*/ )
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
            "PermissionManager::addPermission" );
    }
}

Permission& PermissionManager::getPermission( const String& rName ) const
{
    Permissions::const_iterator i = mPermissions.find( rName );
    if( i != mPermissions.end() )
    {
        return *i->second;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Permission does not exist.", 
            "PermissionManager::getPermission" );
    }
}

bool PermissionManager::hasPermission( const String& rName ) const
{
    return mPermissions.find( rName ) != mPermissions.end();
}

void PermissionManager::removePermission( const String& rName )
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
            "PermissionManager::removePermission" );
    }
}

Permission::ResultType PermissionManager::checkPermission( const String& rName )
{
    return getPermission( rName ).checkPermission();
}


bool PermissionManager::checkPermissionAllowed( const String& rName )
{
    return getPermission( rName ).checkPermissionAllowed();
}

void PermissionManager::checkPermissionThrows( const String& rName, const String& rDesc, 
    const String& rSrc )
{
    getPermission( rName ).checkPermissionThrows( rDesc, rSrc );
}

void PermissionManager::checkPermissionThrows( const String& rName, const String& rSrc )
{
    getPermission( rName ).checkPermissionThrows( rSrc );
}

Permission::ResultType PermissionManager::checkPermission( const String& rName, camp::Value& rValue )
{
    return getPermission( rName ).checkPermission( rValue );
}

bool PermissionManager::checkPermissionAllowed( const String& rName, camp::Value& rValue )
{
    return getPermission( rName ).checkPermissionAllowed( rValue );
}

void PermissionManager::checkPermissionThrows( const String& rName, camp::Value& rValue, 
    const String& rDesc, const String& rSrc )
{
    getPermission( rName ).checkPermissionThrows( rValue, rDesc, rSrc );
}

void PermissionManager::checkPermissionThrows( const String& rName, camp::Value& rValue, 
    const String& rSrc )
{
    getPermission( rName ).checkPermissionThrows( rValue, rSrc );
}

void PermissionManager::create()
{

}

bool PermissionManager::DeserializeConstruction( RakNet::BitStream* pConstructionBitstream, 
    RakNet::Connection_RM3* pSourceConnection )
{
    Permissions permissions;
    *pConstructionBitstream >> permissions;
    setPermissions( permissions );

    LOGD << "Receiving permissions from server:";
    for( Permissions::iterator i = mPermissions.begin(); i != mPermissions.end(); ++i )
    {
        LOGD << i->second->getName() << " allowed: " << i->second->getAllowed() << " maxitems: " <<
            i->second->getMaxItemsAllowed() << " time: " << i->second->getTimeSeconds();
    }

    // 'Hack' permissions
    //mPermissions.find( "ObjectManager_DestroyOtherObject" )->second->setAllowed( true );
    //mPermissions.find( "ObjectManager_DestroyOtherComponentOnOwnObject" )->second->setAllowed( true );
    //mPermissions.find( "ObjectManager_DestroyOtherComponentOnOtherObject" )->second->setAllowed( true );
    //mPermissions.find( "SetPropertyOnOwnPosition_Position" )->second->removeUpperBounds();
    //mPermissions.find( "SetPropertyOnOwnPosition_Position" )->second->removeLowerBounds();

    return true;
}

//------------------------------------------------------------------------------
} // Namespace TestClient
} // Namespace Diversia
