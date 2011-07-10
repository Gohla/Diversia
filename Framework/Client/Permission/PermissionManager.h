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

#ifndef DIVERSIA_CLIENT_PERMISSIONMANAGER_H
#define DIVERSIA_CLIENT_PERMISSIONMANAGER_H

#include "Client/Platform/Prerequisites.h"

#include "Client/Plugin/ClientPlugin.h"
#include "Shared/Permission/Permission.h"

namespace Diversia
{
namespace Client
{
//------------------------------------------------------------------------------

typedef std::map<String, Permission*> Permissions;

class DIVERSIA_CLIENT_API PermissionManager : public ClientPlugin
{
public:
    /**
    Constructor. 
    
    @param  mode                        The mode (Client/Server) the plugin will run in. 
    @param  state                       The initial state of this plugin.
    @param [in,out] rPluginManager      The plugin manager. 
    @param [in,out] rRakPeer            The peer interface. 
    @param [in,out] rReplicaManager     The replica manager. 
    @param [in,out] rNetworkIDManager   The network ID manager. 
    **/
    PermissionManager( Mode mode, PluginState state, ClientPluginManager& rPluginManager, 
        RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
        RakNet::NetworkIDManager& rNetworkIDManager );
    /**
    Destructor. 
    **/
    ~PermissionManager();

    /**
    Gets the plugin type.
    **/
    inline PluginTypeEnum getType() const { return PLUGINTYPE_PERMISSIONMANAGER; }
    static inline PluginTypeEnum getTypeStatic() { return PLUGINTYPE_PERMISSIONMANAGER; }
    /**
    Gets the plugin type name.
    **/
    inline String getTypeName() const { return PLUGINNAME_PERMISSIONMANAGER; }
    static inline String getTypeNameStatic() { return PLUGINNAME_PERMISSIONMANAGER; }

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
        const T& rLowerBounds, const T& rUpperBounds, bool adjustBounds );
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
        double timeSeconds = 99999999999 );
    /**
    Sets the permissions from a given permissions map. Swaps the containers so the given permissions
    map should not be used after this function.
    **/
    void setPermissions( Permissions& rPermissions );
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
    Check permission.

    @param  rName   The name of the permission. 
    
    @return Result of the permission check.
    **/
    Permission::ResultType checkPermission( const String& rName );
    /**
    Check permission. Only returns if the action is allowed or not.

    @param  rName   The name of the permission. 
    
    @return True if the action is allowed, false if not.
    **/
    bool checkPermissionAllowed( const String& rName );
    /**
    Check permission. Throws an exception if permission is denied.

    @param  rName   The name of the permission. 
    @param  rDesc   Exception description.
    @param  rSrc    Exception source.
    **/
    void checkPermissionThrows( const String& rName, const String& rDesc, const String& rSrc );
    /**
    Check permission. Throws an exception if permission is denied.

    @param  rName   The name of the permission. 
    @param  rSrc    Exception source.
    **/
    void checkPermissionThrows( const String& rName, const String& rSrc );
    /**
    Check permission and adjusts value if necessary.
     
    @param  rName           Name of the permission. 
    @param [in,out] rValue  The value to check permission for.
    
    @return Result of the permission check.
    **/
    Permission::ResultType checkPermission( const String& rName, camp::Value& rValue );
    /**
    Check permission and adjusts value of necessary. Only returns if the action is allowed or not.
    
    @param  rName           Name of the permission. 
    @param [in,out] rValue  The value to check permission for.
    
    @return True if the action is allowed, false if not.
    **/
    bool checkPermissionAllowed( const String& rName, camp::Value& rValue );
    /**
    Check permission and adjusts value of necessary. Throws an exception if permission is denied.

    @param          rName   Name of the permission. 
    @param [in,out] rValue  The value to check permission for.
    @param          rDesc   Exception description.
    @param          rSrc    Exception source.
    **/
    void checkPermissionThrows( const String& rName, camp::Value& rValue, const String& rDesc, 
        const String& rSrc );
    /**
    Check permission and adjusts value of necessary. Throws an exception if permission is denied.

    @param          rName   Name of the permission. 
    @param [in,out] rValue  The value to check permission for.
    @param          rSrc    Exception source.
    **/
    void checkPermissionThrows( const String& rName, camp::Value& rValue, const String& rSrc );
    
private:
    void load();
    inline void unload() {}
    inline void setServerState( ServerState serverState ) {}

    bool DeserializeConstruction( RakNet::BitStream* pConstructionBitstream, 
        RakNet::Connection_RM3* pSourceConnection );
    // TODO: Receive permission updates.
    // TODO: Use property syncing once camp supports std::map.

    Permissions mPermissions;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Client::PermissionManager, 
    &Diversia::Client::Bindings::CampBindings::bindPermissionManager );

#endif // DIVERSIA_CLIENT_PERMISSIONMANAGER_H
