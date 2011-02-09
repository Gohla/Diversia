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

#ifndef DIVERSIA_SHARED_CLIENTSERVERPLUGIN_H
#define DIVERSIA_SHARED_CLIENTSERVERPLUGIN_H

#include "Shared/Platform/Prerequisites.h"
#include "Shared/ClientServerPlugin/ClientServerPluginManager.h"
#include <RakNet/RakPeerInterface.h>

namespace Diversia
{
//------------------------------------------------------------------------------

class DIVERSIA_SHARED_API ClientServerPlugin : public RakNet::Replica3
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
    ClientServerPlugin( Mode mode, ClientServerPluginManager& rPluginManager,
        RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager,
        RakNet::NetworkIDManager& rNetworkIDManager );
    /**
    Destructor.
    **/
    virtual ~ClientServerPlugin();
    /**
    Gets the type for this plugin.
    **/
    virtual ClientServerPluginTypeEnum getType() const = 0;
    /**
    Gets the type name for this plugin.
    **/
    virtual String getTypeName() const = 0;

    /**
    Gets the object manager this object belongs to.
    **/
    inline ClientServerPluginManager& getPluginManager() const { return mPluginManager; }
    /**
    Gets the rakpeer interface.
    **/
    inline RakNet::RakPeerInterface& getRakPeer() const { return mRakPeer; }
    /**
    Gets the replica manager.
    **/
    inline RakNet::ReplicaManager3& getReplicaManager() const { return mReplicaManager; }
    /**
    Gets the network ID manager.
    **/
    inline RakNet::NetworkIDManager& getNetworkIDManager() const { return mNetworkIDManager; }
    /**
    Gets the mode.
    **/
    inline Mode getMode() const { return mMode; }
    /**
    Gets the network unique identifier as a string.
    **/
    inline String getGUIDString()
    {
        return  boost::lexical_cast<String>( GetNetworkID() ) +
                boost::lexical_cast<String>( mRakPeer.GetMyGUID().g );
    }
    /**
    Connects a slot to the destruction signal.

    @param [in,out] rSlot   The slot (signature: void func(ClientServerPlugin& [this plugin])) to
                            connect.

    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connectDestruction( const sigc::slot<void, ClientServerPlugin&>& rSlot )
    {
        return mDestructionSignal.connect( rSlot );
    }

protected:
    friend class ClientServerPluginManager;

    /**
	Creates the specific part of this plugin. This is called in the tick/frame update after the
	plugin is created.
	**/
	virtual void create() = 0;

    /**
    Provide implementation for these functions in specialized plugin classes. Defaults are provided
    that do nothing.
    **/
    inline virtual void SerializeConstruction( RakNet::BitStream* pConstructionBitstream,
        RakNet::Connection_RM3* pDestinationConnection ) {}
    inline virtual bool DeserializeConstruction( RakNet::BitStream* pConstructionBitstream,
        RakNet::Connection_RM3* pSourceConnection ) { return true; }
    inline virtual void SerializeDestruction( RakNet::BitStream* pDestructionBitstream,
        RakNet::Connection_RM3* pDestinationConnection ) {}
    inline virtual bool DeserializeDestruction( RakNet::BitStream* pDestructionBitstream,
        RakNet::Connection_RM3* pSourceConnection ) { return true; }
    inline virtual RakNet::RM3SerializationResult Serialize(
        RakNet::SerializeParameters* pSerializeParameters )
    {
        return RakNet::RM3SR_NEVER_SERIALIZE_FOR_THIS_CONNECTION;
    }
    inline virtual void Deserialize( RakNet::DeserializeParameters* pDeserializeParameters ) {}
    inline virtual RakNet::RM3QuerySerializationResult QuerySerialization(
        RakNet::Connection_RM3* pDestinationConnection )
    {
        return RakNet::RM3QSR_CALL_SERIALIZE;
        /*Replica3::QuerySerialization_ServerSerializable( pDestinationConnection,
            mMode == SERVER ? true : false );*/
    }

    /**
    Not used in ClientServerPlugin.
    **/
    inline virtual RakNet::RM3ActionOnPopConnection QueryActionOnPopConnection(
        RakNet::Connection_RM3* pDroppedConnection ) const { return RakNet::RM3AOPC_DO_NOTHING; }

private:
    /**
    Check if the plugin should be broadcasted.
    **/
    void checkBroadcastConstruction();
    /**
    Broadcasts this plugin to the network.
    **/
    void broadcastConstruction();
    /**
    Check if the plugin should be destroyed on the network. Call this before destroying the
    plugin.
    **/
    void checkBroadcastDestruction();
	/**
	Destroys this plugin from the network.
	**/
	void broadcastDestruction();

    /**
    Implemented by ClientServerPlugin.
    **/
    void WriteAllocationID( RakNet::Connection_RM3* pConnection,
        RakNet::BitStream* pAllocationIdBitstream ) const;
    void DeallocReplica( RakNet::Connection_RM3* pSourceConnection );
    RakNet::RM3ConstructionState QueryConstruction( RakNet::Connection_RM3* pDestinationConnection,
        RakNet::ReplicaManager3* pReplicaManager3 );
    bool QueryRemoteConstruction( RakNet::Connection_RM3* pSourceConnection );

    Mode                            mMode;
    ClientServerPluginManager&      mPluginManager;
    RakNet::RakPeerInterface&       mRakPeer;
    RakNet::ReplicaManager3&        mReplicaManager;
    RakNet::NetworkIDManager&       mNetworkIDManager;

    sigc::signal<void, ClientServerPlugin&> mDestructionSignal;

    CAMP_RTTI()
};

//------------------------------------------------------------------------------
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::ClientServerPlugin,
    &Diversia::Shared::Bindings::CampBindings::bindClientServerPlugin );

#endif // DIVERSIA_SHARED_CLIENTSERVERPLUGIN_H