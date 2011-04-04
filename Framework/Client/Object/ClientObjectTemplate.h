/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_CLIENT_CLIENTOBJECTTEMPLATE_H
#define DIVERSIA_CLIENT_CLIENTOBJECTTEMPLATE_H

#include "Client/Platform/Prerequisites.h"

#include "Client/Object/ClientObjectTemplateManager.h"
#include "Object/ObjectTemplate.h"
#include "Shared/Camp/PropertySynchronization.h"

namespace Diversia
{
namespace Client
{
//------------------------------------------------------------------------------

class DIVERSIA_CLIENT_API ClientObjectTemplate : public ObjectTemplate, 
    public PropertySynchronization
{
public:
    /**
    Gets the object manager. 
    **/
    inline ClientObjectTemplateManager& getClientObjectTemplateManager() 
    { 
        return static_cast<ClientObjectTemplateManager&>( ObjectTemplate::getObjectTemplateManager() ); 
    }
    /**
    Gets the permission manager. 
    **/
    inline PermissionManager& getPermissionManager() const { return mPermissionManager; }

private:
    friend class ClientObjectTemplateManager;	///< Only ClientObjectTemplateManager may create object templates. 

    ClientObjectTemplate( const String& rName, Mode mode, NetworkingType type,
        const String& rDisplayName, RakNet::RakNetGUID source, RakNet::RakNetGUID ownGUID, 
        RakNet::RakNetGUID serverGUID, ObjectTemplateManager& rObjectTemplateManager, 
        PermissionManager& rPermissionManager, RakNet::ReplicaManager3& rReplicaManager, 
        RakNet::NetworkIDManager& rNetworkIDManager );
    ~ClientObjectTemplate();

    void queryCreateComponentTemplate( ComponentType type, bool localOverride, 
        RakNet::RakNetGUID source );
    ComponentTemplate* createComponentTemplateImpl( ComponentType type, const String& rName, 
        bool localOverride, RakNet::RakNetGUID source );
    void queryDestroyComponentTemplate( ComponentTemplate& rTemplate, RakNet::RakNetGUID source );
    void setNetworkingTypeImpl( NetworkingType type );
    void querySetNetworkingTypeImpl( NetworkingType type );
    void cleanupQuerySetNetworkingType( NetworkingType type );
    void querySetParent( ObjectTemplate* pNewParent, RakNet::RakNetGUID source );
    void querySetProperty( const String& rQuery, camp::Value& rValue );
    void queryInsertProperty( const String& rQuery, camp::Value& rValue );

    RakNet::RM3ConstructionState QueryConstruction(
        RakNet::Connection_RM3* pDestinationConnection,
        RakNet::ReplicaManager3* pReplicaManager3 );
    bool QueryRemoteConstruction( RakNet::Connection_RM3* pSourceConnection );
    RakNet::RM3QuerySerializationResult QuerySerialization(
        RakNet::Connection_RM3* pDestinationConnection );
    void SerializeConstruction( RakNet::BitStream* pConstructionBitstream,
        RakNet::Connection_RM3* pDestinationConnection );
    bool DeserializeConstruction( RakNet::BitStream* pConstructionBitstream,
        RakNet::Connection_RM3* pSourceConnection );
    RakNet::RM3SerializationResult Serialize(
        RakNet::SerializeParameters* pSerializeParameters );
    void Deserialize( RakNet::DeserializeParameters* pDeserializeParameters );

    PermissionManager& mPermissionManager;

    CAMP_CLASS(ClientObjectTemplate)

};

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Client::ClientObjectTemplate, 
    &Diversia::Client::Bindings::CampBindings::bindClientObjectTemplate );

#endif // DIVERSIA_CLIENT_CLIENTOBJECTTEMPLATE_H