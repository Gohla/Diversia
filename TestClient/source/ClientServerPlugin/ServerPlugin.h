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

#ifndef DIVERSIA_TESTCLIENT_SERVERPLUGIN_H
#define DIVERSIA_TESTCLIENT_SERVERPLUGIN_H

#include "Platform/Prerequisites.h"

#include "Shared/ClientServerPlugin/ClientServerPlugin.h"
#include "Shared/Camp/PropertyTransaction.h"

namespace Diversia
{
namespace TestClient
{
//------------------------------------------------------------------------------

class ServerPlugin : public ClientServerPlugin
{
public:
    ServerPlugin( Mode mode, ClientServerPluginManager& rPluginManager, 
        RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
        NetworkIDManager& rNetworkIDManager, camp::UserObject userObject );
    virtual ~ServerPlugin();

    /**
    Sets a property using camp. 
    
    @param  rQuery  The name of the property. 
    @param  rValue  The value to set. 
    **/
    void set( const String& rQuery, const camp::Value& rValue );
    /**
    Gets a property using camp. 
    
    @param  rQuery  The name of the property. 
    **/
    camp::Value get( const String& rQuery );

    /**
    Makes properties serialize as soon as possible (when the next RakNet serialize tick occurs).
    **/
    inline void forceSerialize() { mNextSerialization = 0; }

protected:
    /**
    Implemented in ServerPlugin, but can be overridden. Be sure to call the function in this 
    class first in your overridden function before doing your own processing!
    **/
    virtual void SerializeConstruction( RakNet::BitStream* pConstructionBitstream, 
        RakNet::Connection_RM3* pDestinationConnection );
    virtual bool DeserializeConstruction( RakNet::BitStream* pConstructionBitstream, 
        RakNet::Connection_RM3* pSourceConnection );
    virtual RakNet::RM3SerializationResult Serialize( 
        RakNet::SerializeParameters* pSerializeParameters );
    virtual void Deserialize( RakNet::DeserializeParameters* pDeserializeParameters );

private:
    void pluginCreated( ClientServerPlugin& rPlugin, bool created );

    camp::UserObject    mUserObject;
    RakNetTime          mNextSerialization;
    PropertyTransaction mOutputPropertyTransaction;
    PropertyTransaction mInputPropertyTransaction;
    PermissionManager*  mPermissionManager;

};

//------------------------------------------------------------------------------
} // Namespace TestClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::TestClient::ServerPlugin, 
    &Diversia::TestClient::Bindings::CampBindings::bindServerPlugin );

#endif // DIVERSIA_TESTCLIENT_SERVERPLUGIN_H