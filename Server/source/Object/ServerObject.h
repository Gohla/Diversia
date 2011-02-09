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

#ifndef DIVERSIA_SERVER_CLIENTOBJECT_H
#define DIVERSIA_SERVER_CLIENTOBJECT_H

#include "Platform/Prerequisites.h"

#include "Object/Object.h"
#include "Object/Component.h"

namespace Diversia
{
namespace Server
{
//------------------------------------------------------------------------------

class ServerObject : public Object
{
public:
    /**
    Gets the permission manager. 
    **/
    inline PermissionManager& getPermissionManager() const { return mPermissionManager; }

private:
    friend class ServerObjectManager;	///< Only the ServerObjectManager class may construct objects. 
    friend void camp::detail::destroy<ServerObject>( const UserObject& object );  ///< Allow private access for camp.

    ServerObject( const String& rName, Mode mode, NetworkingType type, const String& rDisplayName,
        RakNet::RakNetGUID source, RakNet::RakNetGUID ownGUID, RakNet::RakNetGUID serverGUID,
        sigc::signal<void>& rUpdateSignal, ServerObjectManager& rObjectManager, 
        RakNet::ReplicaManager3& rReplicaManager, RakNet::NetworkIDManager& rNetworkIDManager, 
        PermissionManager& rPermissionManager, RakNet::RPC3& rRPC3 );
    virtual ~ServerObject();

    void queryCreateComponent( ComponentType type, RakNet::RakNetGUID source, 
        bool localOverride );
    void queryDestroyComponent( Component& rComponent, RakNet::RakNetGUID source );
    void setNetworkingTypeImpl( NetworkingType type );
    inline void querySetNetworkingTypeImpl( NetworkingType type ) { /* Always allowed */ }
    inline void cleanupQuerySetNetworkingType( NetworkingType type ) { /* No cleanup needed */ }
    void querySetParent( Object* pNewParent, RakNet::RakNetGUID source );

    RakNet::RM3ConstructionState QueryConstruction( 
        RakNet::Connection_RM3* pDestinationConnection, 
        RakNet::ReplicaManager3* pReplicaManager3 );
    bool QueryRemoteConstruction( RakNet::Connection_RM3* pSourceConnection );
    bool DeserializeDestruction( RakNet::BitStream* pDestructionBitstream, 
        RakNet::Connection_RM3* pSourceConnection );

    PermissionManager&  mPermissionManager;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Server::ServerObject, 
    &Diversia::Server::Bindings::CampBindings::bindServerObject );

#endif // DIVERSIA_SERVER_CLIENTOBJECT_H