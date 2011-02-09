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

#ifndef DIVERSIA_SERVER_PERMISSIONMANAGER_H
#define DIVERSIA_SERVER_PERMISSIONMANAGER_H

#include "Platform/Prerequisites.h"

#include "ClientServerPlugin/ClientPlugin.h"
#include "User/UserManager.h"
#include "User/User.h"
#include "Shared/Permission/Permission.h"

namespace Diversia
{
namespace Server
{
//------------------------------------------------------------------------------

class PermissionManager : public ClientPlugin
{
public:
    /**
    Constructor. 
    
    @param  mode                        The mode (Client/Server) the plugin will run in. 
    @param [in,out] rPluginManager      The plugin manager. 
    @param [in,out] rRakPeer            The peer interface. 
    @param [in,out] rReplicaManager     The replica manager. 
    @param [in,out] rNetworkIDManager   The network ID manager. 
    **/
    PermissionManager( Mode mode, ClientPluginManager& rPluginManager, 
        RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
        RakNet::NetworkIDManager& rNetworkIDManager );

	/**
    Gets the plugin type.
    **/
    inline ClientServerPluginTypeEnum getType() const { return CLIENTSERVERPLUGINTYPE_PERMISSIONMANAGER; }
    static inline ClientServerPluginTypeEnum getTypeStatic() { return CLIENTSERVERPLUGINTYPE_PERMISSIONMANAGER; }
    /**
    Gets the plugin type name.
    **/
    inline String getTypeName() const { return CLIENTSERVERPLUGINNAME_PERMISSIONMANAGER; }
    static inline String getTypeNameStatic() { return CLIENTSERVERPLUGINNAME_PERMISSIONMANAGER; }
    /**
    Sets the session manager. 
    
    @param [in,out] rUserManager    The user manager.
    **/
    void setSessionManager( SessionManager& rSessionManager );

    /**
    Sets or adds a user permission. 
    
    @param  rUserName           Name of the user. 
    @param  rPermissionName     Name of the permission. 
    @param  allowed             True to allow the action, false to deny.
    @param  maxItemsAllowed     The maximum items allowed. 
    @param  maxItemsPerTime     The maximum items allowed per given time frame. 
    @param  timeSeconds         The time frame in seconds.
    @param  rLowerBounds        The lower bounds. 
    @param  rUpperBounds        The upper bounds. 
    @param  adjustBounds        True to adjust bounds, false to deny when value is out of bounds.
    **/
    template <typename T> void setPermission( const String& rUserName, 
        const String& rPermissionName, bool allowed, unsigned int maxItemsAllowed, 
        unsigned int maxItemsPerTime, double timeSeconds, const T& rLowerBounds, 
        const T& rUpperBounds, bool adjustBounds )
    {
        DivAssert( mUserManager, "UserManager not set." );

        // Update permission if it already exists, add if it doesn't exist.
        User& user = mUserManager->getUser( rUserName );
        if( user.hasPermission( rPermissionName ) )
        {
            Permission& permission = user.getPermission( rPermissionName );
            permission.setAllowed( allowed );
            permission.setMaxItemsAllowed( maxItemsAllowed );
            permission.setMaxItemsPerTime( maxItemsPerTime );
            permission.setTimeSeconds( timeSeconds );
            permission.setLowerBounds( rLowerBounds );
            permission.setUpperBounds( rUpperBounds );
        }
        else
        {
            user.addPermission( rPermissionName, allowed, maxItemsAllowed, maxItemsPerTime, 
                timeSeconds, rLowerBounds, rUpperBounds, adjustBounds );
        }
    }
    /**
    Sets or adds a user permission. 
    
    @param  rUserName           Name of the user. 
    @param  rPermissionName     Name of the permission. 
    @param  allowed             True to allow the action, false to deny.
    @param  maxItemsAllowed     The maximum items allowed. 
    @param  maxItemsPerTime     The maximum items allowed per given time frame. 
    @param  timeSeconds         The time frame in seconds.
    **/
    void setPermission( const String& rUserName, const String& rPermissionName, bool allowed, 
        unsigned int maxItemsAllowed = MAXUINT, unsigned int maxItemsPerTime = MAXUINT, 
        double timeSeconds = 9999999999 );
    /**
    Gets a permission. 
    
    @param  guid    Unique session identifier to check the permission for.
    @param  rName   The name of the permission. 
    **/
    Permission& getPermission( RakNet::RakNetGUID guid, const String& rName );

    /**
    Check permission.

    @param  guid    Unique session identifier to check the permission for.
    @param  rName   The name of the permission. 
    
    @return Result of the permission check.
    **/
    Permission::ResultType checkPermission( RakNet::RakNetGUID guid, const String& rName );
    /**
    Check permission. Only returns if the action is allowed or not.

    @param  guid    Unique session identifier to check the permission for.
    @param  rName   The name of the permission. 
    
    @return True if the action is allowed, false if not.
    **/
    bool checkPermissionAllowed( RakNet::RakNetGUID guid, const String& rName );
    /**
    Check permission. Throws an exception if permission is denied.

    @param  guid    Unique session identifier to check the permission for.
    @param  rName   The name of the permission. 
    @param  rDesc   Exception description.
    @param  rSrc    Exception source.
    **/
    void checkPermissionThrows( RakNet::RakNetGUID guid, const String& rName, const String& rDesc, 
        const String& rSrc );
    /**
    Check permission. Throws an exception if permission is denied.

    @param  guid    Unique session identifier to check the permission for.
    @param  rName   The name of the permission. 
    @param  rSrc    Exception source.
    **/
    void checkPermissionThrows( RakNet::RakNetGUID guid, const String& rName, const String& rSrc );
    /**
    Check permission and adjusts value if necessary.
     
    @param  guid    Unique session identifier to check the permission for.
    @param  rName           Name of the permission. 
    @param [in,out] rValue  The value to check permission for.
    
    @return Result of the permission check.
    **/
    Permission::ResultType checkPermission( RakNet::RakNetGUID guid, const String& rName, 
        camp::Value& rValue );
    /**
    Check permission and adjusts value of necessary. Only returns if the action is allowed or not.
    
    @param  guid    Unique session identifier to check the permission for.
    @param  rName           Name of the permission. 
    @param [in,out] rValue  The value to check permission for.
    
    @return True if the action is allowed, false if not.
    **/
    bool checkPermissionAllowed( RakNet::RakNetGUID guid, const String& rName, camp::Value& rValue );
    /**
    Check permission and adjusts value of necessary. Throws an exception if permission is denied.

    @param  guid    Unique session identifier to check the permission for.
    @param          rName   Name of the permission. 
    @param [in,out] rValue  The value to check permission for.
    @param          rDesc   Exception description.
    @param          rSrc    Exception source.
    **/
    void checkPermissionThrows( RakNet::RakNetGUID guid, const String& rName, camp::Value& rValue, 
        const String& rDesc, const String& rSrc );
    /**
    Check permission and adjusts value of necessary. Throws an exception if permission is denied.

    @param  guid    Unique session identifier to check the permission for.
    @param          rName   Name of the permission. 
    @param [in,out] rValue  The value to check permission for.
    @param          rSrc    Exception source.
    **/
    void checkPermissionThrows( RakNet::RakNetGUID guid, const String& rName, camp::Value& rValue, 
        const String& rSrc );
    
private:
    /**
	Creates the specific part of this plugin. This is called in the tick/frame update after the
	plugin is created.
	**/
	void create();

    void SerializeConstruction( RakNet::BitStream* pConstructionBitstream, 
        RakNet::Connection_RM3* pDestinationConnection );
    // TODO: Send permission updates.

    std::map<RakNet::RakNetGUID, User*> mUserLookup;

    SessionManager* mSessionManager;
    UserManager*    mUserManager;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Server::PermissionManager, 
    &Diversia::Server::Bindings::CampBindings::bindPermissionManager );

#endif // DIVERSIA_SERVER_PERMISSIONMANAGER_H