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

#ifndef DIVERSIA_CLIENT_CLIENTCOMPONENT_H
#define DIVERSIA_CLIENT_CLIENTCOMPONENT_H

#include "Client/Platform/Prerequisites.h"

#include "Client/Object/ClientObject.h"
#include "Client/ClientServerPlugin/ServerPluginManager.h"
#include "Shared/Resource/ResourceInfo.h"
#include "Shared/Camp/PropertySynchronization.h"
#include "Object/Component.h"

namespace Diversia
{
namespace Client
{
//------------------------------------------------------------------------------

class DIVERSIA_CLIENT_API ClientComponent : public Component, public PropertySynchronization
{
public:
    /**
    Gets the object that owns this component.
    **/
    inline ClientObject& getClientObject() const
    {
        return static_cast<ClientObject&>( Component::getObject() );
    }
    /**
    Gets the server that this components belongs to.
    **/
    inline ServerAbstract& getServer() const
    {
        return ClientComponent::getClientObject().getClientObjectManager().
            getServerPluginManager().getServer();
    }

protected:
    ClientComponent( const String& rName, Mode mode, NetworkingType networkingType, 
        ComponentType type, RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject );
    virtual ~ClientComponent();

    void setNetworkingTypeImpl( NetworkingType type );
    void querySetNetworkingTypeImpl( NetworkingType type );
    void cleanupQuerySetNetworkingType( NetworkingType type );
    void querySetProperty( const String& rQuery, camp::Value& rValue );
    void queryInsertProperty( const String& rQuery, camp::Value& rValue );
    /**
    Notification when object is selected.
    **/
    inline virtual void setSelected( bool selected ) {}

    /**
    Implemented in ClientComponent, but can be overridden.
    **/
    virtual RakNet::RM3ConstructionState QueryConstruction( 
        RakNet::Connection_RM3* pDestinationConnection, 
        RakNet::ReplicaManager3* pReplicaManager3 );
    virtual bool QueryRemoteConstruction( RakNet::Connection_RM3* pSourceConnection );
    virtual RakNet::RM3QuerySerializationResult QuerySerialization( 
        RakNet::Connection_RM3* pDestinationConnection );
    inline virtual void SerializeDestruction( RakNet::BitStream* pDestructionBitstream, 
        RakNet::Connection_RM3* pDestinationConnection ) {}
    inline virtual bool DeserializeDestruction( RakNet::BitStream* pDestructionBitstream, 
        RakNet::Connection_RM3* pSourceConnection ) { return true; }

    /**
    Implemented in ClientComponent, but can be overridden. Be sure to call the function in this 
    class first in your overridden function before doing your own processing!
    **/
    virtual void SerializeConstruction( RakNet::BitStream* pConstructionBitstream, 
        RakNet::Connection_RM3* pDestinationConnection );
    virtual bool DeserializeConstruction( RakNet::BitStream* pConstructionBitstream, 
        RakNet::Connection_RM3* pSourceConnection );
    virtual RakNet::RM3SerializationResult Serialize( 
        RakNet::SerializeParameters* pSerializeParameters );
    virtual void Deserialize( RakNet::DeserializeParameters* pDeserializeParameters );

    ResourceList mResourceList;

private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.
    friend void camp::detail::destroy<ClientComponent>( const UserObject& object );  ///< Allow private access for camp.
    friend class ClientObject;

    PermissionManager& mPermissionManager;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Client::ClientComponent, 
    &Diversia::Client::Bindings::CampBindings::bindClientComponent );

#endif // DIVERSIA_CLIENT_CLIENTCOMPONENT_H