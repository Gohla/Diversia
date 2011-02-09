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

#ifndef DIVERSIA_CLIENT_CLIENTOBJECT_H
#define DIVERSIA_CLIENT_CLIENTOBJECT_H

#include "Client/Platform/Prerequisites.h"

#include "Object/Object.h"
#include "Client/Object/ClientObjectManager.h"

namespace Diversia
{
namespace Client
{
//------------------------------------------------------------------------------

class DIVERSIA_CLIENT_API ClientObject : public Object
{
public:
    /**
    Gets the object manager. 
    **/
    inline ClientObjectManager& getClientObjectManager() 
    { 
        return static_cast<ClientObjectManager&>( Object::getObjectManager() ); 
    }
    /**
    Gets the permission manager. 
    **/
    inline PermissionManager& getPermissionManager() const { return mPermissionManager; }
    /**
    Query if this object is selected. 
    **/
    inline bool isSelected() const { return mSelected; }
    /**
    Selects or unselects this object.
    **/
    void setSelected( bool selected );

private:
    friend class ClientObjectManager;	///< Only the ClientObjectManager class may construct objects. 
    friend void camp::detail::destroy<ClientObject>( const UserObject& object );  ///< Allow private access for camp.

    ClientObject( const String& rName, Mode mode, NetworkingType type, const String& rDisplayName, 
        RakNet::RakNetGUID source, RakNet::RakNetGUID ownGUID, RakNet::RakNetGUID serverGUID, 
        sigc::signal<void>& rUpdateSignal, ClientObjectManager& rObjectManager, 
        PermissionManager& rPermissionManager, RakNet::ReplicaManager3& rReplicaManager,
        RakNet::NetworkIDManager& rNetworkIDManager, RakNet::RPC3& rRPC3 );
    virtual ~ClientObject();

    void queryCreateComponent( ComponentType type, RakNet::RakNetGUID source, 
        bool localOverride );
    void queryDestroyComponent( Component& rComponent, RakNet::RakNetGUID source );
    void setNetworkingTypeImpl( NetworkingType type );
    void querySetNetworkingTypeImpl( NetworkingType type );
    void cleanupQuerySetNetworkingType( NetworkingType type );
    void querySetParent( Object* pNewParent, RakNet::RakNetGUID source );

    RakNet::RM3ConstructionState QueryConstruction( 
        RakNet::Connection_RM3* pDestinationConnection, 
        RakNet::ReplicaManager3* pReplicaManager3 );
    bool QueryRemoteConstruction ( RakNet::Connection_RM3* pSourceConnection );

    PermissionManager&      mPermissionManager;

    bool mSelected;

    CAMP_RTTI()
};

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Client::ClientObject, 
    &Diversia::Client::Bindings::CampBindings::bindClientObject );

#endif // DIVERSIA_CLIENT_CLIENTOBJECT_H