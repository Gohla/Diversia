/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

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

#include "Shared/Platform/StableHeaders.h"

#include "Shared/Communication/ReplicaConnection.h"
#include "Shared/Communication/BitStream.h"
#include "Shared/ClientServerPlugin/ClientServerPluginManager.h"
#include "Shared/ClientServerPlugin/ClientServerPlugin.h"
#include "Object/ObjectManager.h"
#include "Object/Object.h"
#include "Object/Component.h"

namespace Diversia
{
//------------------------------------------------------------------------------

ReplicaConnection::ReplicaConnection( RakNet::SystemAddress systemAddress, RakNet::RakNetGUID guid, 
    ObjectManager* pObjectManager, ClientServerPluginManager& rPluginManager ):
    Connection_RM3( systemAddress, guid ),
    mObjectManager( pObjectManager ),
    mPluginManager( rPluginManager )
{

}

RakNet::Replica3* ReplicaConnection::AllocReplica( RakNet::BitStream* pAllocationIdBitstream, 
    RakNet::ReplicaManager3* pReplicaManager3 )
{
    ReplicaType replicaType; pAllocationIdBitstream->Read<ReplicaType>( replicaType );

    switch( replicaType )
    {
        case REPLICATYPE_OBJECT:
        {
            if( !mObjectManager )
            {
                SLOGE << "Cannot create object, object manager has not been set yet, object " <<
                    "has been dropped";

                return 0;
            }

            RakNet::RakString name; pAllocationIdBitstream->Read<RakNet::RakString>( name );
            RakNet::RakString displayName; pAllocationIdBitstream->Read<RakNet::RakString>( 
                displayName );
            try
            {
                return &mObjectManager->createObject( String( name.C_String() ), 
                    REMOTE, String( displayName.C_String() ), 
                    RakNet::Connection_RM3::GetRakNetGUID() );
            }
            catch ( Exception e )
            {
                SLOGE << "Could not create object " << String( name.C_String() ) << " from " << 
                    RakNet::Connection_RM3::GetRakNetGUID().g << ": " << e.getDescription();
            }
            break;
        }
        case REPLICATYPE_COMPONENT:
        {
            ComponentType type; pAllocationIdBitstream->Read<ComponentType>( type );
            RakNet::RakString name; *pAllocationIdBitstream >> name;
            RakNet::NetworkID objectID; pAllocationIdBitstream->Read<RakNet::NetworkID>( objectID );
            Object* object = pReplicaManager3->GetNetworkIDManager()->GET_OBJECT_FROM_ID<Object*>( 
                objectID );
            // Do not create a component if the object does not exist. It could have been deleted
            // by the server before this component came in.
            if( object != 0 ) 
            {
                try
                {
                    return &object->createComponent( type, String( name.C_String() ), false, 
                        RakNet::Connection_RM3::GetRakNetGUID() );
                }
                catch ( Exception e )
                {
                    SLOGE << "Could not create component " << String( name.C_String() ) << 
                        " from " << RakNet::Connection_RM3::GetRakNetGUID().g << ": " << 
                        e.getDescription();
                }
            }
            else
            {
                SLOGE << "Could not find the object named " << String( name.C_String() ) << 
                    " that a component created by " << RakNet::Connection_RM3::GetRakNetGUID().g << 
                    " belongs to.";
            }
            break;
        }
        case REPLICATYPE_CLIENTSERVERPLUGIN:
        {
            ClientServerPluginTypeEnum type; 
            pAllocationIdBitstream->Read<ClientServerPluginTypeEnum>( type );

            try
            {
                return &mPluginManager.createPlugin( type );
            }
            catch ( Exception e )
            {
                SLOGE << "Could not create plugin of type " << type << 
                    " from " << RakNet::Connection_RM3::GetRakNetGUID().g << ": " << 
                    e.getDescription();
            }
            break;
        }
        default:
            SLOGE << "Unknown replica type received from " << 
                RakNet::Connection_RM3::GetRakNetGUID().g;
            break;
    }

    return 0;
}

//------------------------------------------------------------------------------
} // Namespace Diversia
