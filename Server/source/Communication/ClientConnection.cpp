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

#include "Platform/StableHeaders.h"

#include "ClientServerPlugin/ClientPluginManager.h"
#include "Communication/ClientConnection.h"
#include "Object/ServerObjectManager.h"
#include "Permission/PermissionManager.h"
#include "Shared/Lua/LuaManager.h"
#include "User/SessionManager.h"

namespace Diversia
{
namespace Server 
{
//------------------------------------------------------------------------------

ClientConnection::Settings ClientConnection::msSettings = ClientConnection::Settings();

ClientConnection::ClientConnection( sigc::signal<void>& rUpdateSignal ):
    mRakPeer( *RakNet::RakPeerInterface::GetInstance() )
{
    LOGI << "Initializing client connection";

    rUpdateSignal.connect( sigc::mem_fun( this, &ClientConnection::update ) );

    mRPC3.SetNetworkIDManager( &mNetworkIDManager );
    mReplicaManager.SetNetworkIDManager( &mNetworkIDManager );
    mRakPeer.AttachPlugin( &mRPC3 );
    mRakPeer.AttachPlugin( &mReplicaManager );

    // Initialize plugin manager
    mPluginManager = new ClientPluginManager( SERVER, rUpdateSignal, mRakPeer, 
        mReplicaManager, mNetworkIDManager, mRPC3 );
    mReplicaManager.setPluginManager( *mPluginManager );

    // Initialize user manager
    mSessionManager.reset( new SessionManager( mUserManager, *mPluginManager, 
        rUpdateSignal ) );

    // Initialize permission manager
    PermissionManager& permissionManager = mPluginManager->createPlugin<PermissionManager>();
    permissionManager.setSessionManager( *mSessionManager.get() );

    // Connect to plugin change signal to set the object manager when it's created.
    mPluginChangeConnection = mPluginManager->connect( sigc::mem_fun( this, 
        &ClientConnection::pluginChange ) );

    // Load all other plugins and set configuration for them. Copy vector to set to remove
    // duplicates and fix the load order.
    std::set<ClientServerPluginTypeEnum> plugins;
    std::copy( msSettings.mPlugins.begin(), msSettings.mPlugins.end(), std::inserter( plugins, 
        plugins.end() ) );
    for( std::set<ClientServerPluginTypeEnum>::iterator i = plugins.begin(); i!= plugins.end(); 
        ++i )
    {
        ClientServerPlugin& plugin = mPluginManager->createPlugin( *i );
        LOGI << "Loading client-server plugin: " << plugin.getTypeName();
        Globals::mConfig->registerObject( plugin );
    }
    
    // Load user settings after loading all plugins (PermissionManager), so default permissions get 
    // overridden.
    Globals::mConfig->registerObject( mUserManager );

    Globals::mClient = this;

    Globals::mLua->object( "ClientConnection" ) = this;
}

ClientConnection::~ClientConnection()
{
    LOGI << "Destroying client connection";

    Globals::mClient = 0;

    delete mPluginManager;

    mRakPeer.DetachPlugin( &mRPC3 );
    mRakPeer.Shutdown( msSettings.mShutdownBlockDuractionMS );
    RakNet::RakPeerInterface::DestroyInstance( &mRakPeer );
}

bool ClientConnection::listen()
{
    RakNet::SocketDescriptor sd;
    sd.port = msSettings.mServerInfo.mPort;
    strcpy( sd.hostAddress, msSettings.mServerInfo.mAddress.c_str() );

    mRakPeer.SetMaximumIncomingConnections( msSettings.mMaxConnections );
    if( msSettings.mTimeoutMS != 0 )
        mRakPeer.SetTimeoutTime( msSettings.mTimeoutMS, RakNet::UNASSIGNED_SYSTEM_ADDRESS );

    if( mRakPeer.Startup( msSettings.mMaxConnections, &sd, 1 ) != RakNet::RAKNET_STARTED )
    {
        DIVERSIA_EXCEPT( Exception::ERR_INTERNAL_ERROR, "Could not create a new socket or thread.", 
            "ClientConnection::listen" );
    }

    LOGI << "Listening for client connections on " << msSettings.mServerInfo.getAddressMerged();

    return true;
}

void ClientConnection::disconnect()
{
    mRakPeer.Shutdown( msSettings.mShutdownBlockDuractionMS );
    mSessionManager->clear();
}

void ClientConnection::update()
{
    RakNet::Packet *packet;
    for( packet = mRakPeer.Receive(); packet; mRakPeer.DeallocatePacket( packet ), 
        packet = mRakPeer.Receive() )
    {
        switch ( packet->data[0] )
        {
            case ID_CONNECTION_ATTEMPT_FAILED:
                // Connection attempt failed.
                LOGD << "Client connection attempt failed: " << packet->systemAddress.ToString();
                break;
            case ID_NO_FREE_INCOMING_CONNECTIONS:
                // The server is full.
                LOGD << "Client cannot connect, server is full: " << packet->systemAddress.ToString();
                break; 
            case ID_DISCONNECTION_NOTIFICATION:
                LOGD << "Client disconnected: " << packet->systemAddress.ToString();
                mSessionManager->removeSession( packet->guid );
                break;
            case ID_CONNECTION_LOST:
                LOGD << "Client lost connection: " << packet->systemAddress.ToString();
                mSessionManager->removeSession( packet->guid );
                break;
            case ID_NEW_INCOMING_CONNECTION:
                LOGD << "Incoming client connection: " << packet->systemAddress.ToString();
                // TODO: Authentication
                mSessionManager->addSession( packet->guid, mUserManager.createGuest() );
                break;
            case ID_CONNECTION_REQUEST_ACCEPTED:
                LOGD << "Client connection request accepted: " << packet->systemAddress.ToString();
                break;
            case ID_RPC_REMOTE_ERROR:
            {
                std::stringstream ss;

                // Recipient system returned an RPC3 error
                switch ( packet->data[1] )
                {
                    case RakNet::RPC_ERROR_NETWORK_ID_MANAGER_UNAVAILABLE:
                        ss << "RPC3 error, network ID manager unavailable";
                        break;
                    case RakNet::RPC_ERROR_OBJECT_DOES_NOT_EXIST:
                        ss << "RPC3 error, object does not exist";
                        break;
                    case RakNet::RPC_ERROR_FUNCTION_INDEX_OUT_OF_RANGE:
                        ss << "RPC3 error, function index out of range";
                        break;
                    case RakNet::RPC_ERROR_FUNCTION_NOT_REGISTERED:
                        ss << "RPC3 error, function not registered";
                        break;
                    case RakNet::RPC_ERROR_FUNCTION_NO_LONGER_REGISTERED:
                        ss << "RPC3 error, function no longer registered";
                        break;
                    case RakNet::RPC_ERROR_CALLING_CPP_AS_C:
                        ss << "RPC3 error, calling cpp function as c function";
                        break;
                    case RakNet::RPC_ERROR_CALLING_C_AS_CPP:
                        ss << "RPC3 error, calling c function as cpp function";
                        break;
                }
                ss << ", at function: " << (char*)packet->data+2;
                LOGE << ss.str();
                break;
            }
        }
    }
}

void ClientConnection::pluginChange( ClientServerPlugin& rPlugin, bool created )
{
    if( created && rPlugin.getType() == CLIENTSERVERPLUGINTYPE_OBJECTMANAGER )
    {
        mReplicaManager.setObjectManager( static_cast<ServerObjectManager&>( rPlugin ) );
        mPluginChangeConnection.disconnect();
    }
}

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia