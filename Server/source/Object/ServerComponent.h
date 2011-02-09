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

#ifndef DIVERSIA_SERVER_SERVERCOMPONENT_H
#define DIVERSIA_SERVER_SERVERCOMPONENT_H

#include "Platform/Prerequisites.h"

#include "Object/Component.h"
#include "Object/ServerObject.h"
#include "Shared/Resource/ResourceInfo.h"
#include "Shared/Camp/PropertySynchronization.h"

namespace Diversia
{
namespace Server
{
//------------------------------------------------------------------------------

class ServerComponent : public Component, public PropertySynchronization
{
public:
    /**
    Constructor. 
    
    @param  rName           The name of the component. 
    @param  mode            The mode (Client/Server) the component will run in. 
    @param  networkingType  If the component should be a remote or local object. (Local means the
                            component will not be propagated to the client/server) 
    @param  type            Component type. 
    @param  source          The source of this component. 
    @param  localOverride   True to override component to local mode even if the object is a
                            remote object. 
    @param [in,out] rObject The object that created this component. 
    **/
    ServerComponent( const String& rName, Mode mode, NetworkingType networkingType, ComponentType type, 
        RakNet::RakNetGUID source, bool localOverride, ServerObject& rObject );
    /**
    Destructor. 
    **/
    virtual ~ServerComponent();

    /**
    Gets the server object.
    **/
    inline ServerObject& getServerObject()
    { 
        return static_cast<ServerObject&>( Component::getObject() ); 
    }
    
protected:
    void setNetworkingTypeImpl( NetworkingType type );
    inline void querySetNetworkingTypeImpl( NetworkingType type ) { /* Always allowed */ }
    inline void cleanupQuerySetNetworkingType( NetworkingType type ) { /* No cleanup needed */ }

    void querySetPropertyDeserialize( const String& rQuery, camp::Value& rValue, 
        RakNet::RakNetGUID source );
    void queryInsertPropertyDeserialize( const String& rQuery, camp::Value& rValue, 
        RakNet::RakNetGUID source );

    /**
    Implemented in ServerComponent, but can be overridden.
    **/
    virtual RakNet::RM3ConstructionState QueryConstruction( 
        RakNet::Connection_RM3* pDestinationConnection, 
        RakNet::ReplicaManager3* pReplicaManager3 );
    virtual bool QueryRemoteConstruction( RakNet::Connection_RM3* pSourceConnection );
    virtual RakNet::RM3QuerySerializationResult QuerySerialization( 
        RakNet::Connection_RM3* pDestinationConnection );
    inline virtual void SerializeDestruction( RakNet::BitStream* pDestructionBitstream, 
        RakNet::Connection_RM3* pDestinationConnection ) {}
    virtual bool DeserializeDestruction( RakNet::BitStream* pDestructionBitstream, 
        RakNet::Connection_RM3* pSourceConnection );

    /**
    Provide implementation for these functions in specialized component classes.
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

    PermissionManager& mPermissionManager;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Server::ServerComponent, 
    &Diversia::Server::Bindings::CampBindings::bindServerComponent );

#endif // DIVERSIA_SERVER_SERVERCOMPONENT_H