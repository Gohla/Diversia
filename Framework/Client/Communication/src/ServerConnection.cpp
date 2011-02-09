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

#include "Client/Platform/StableHeaders.h"

#include "Client/Communication/ServerConnection.h"

namespace Diversia
{
namespace Client 
{
//------------------------------------------------------------------------------

ServerConnection::Settings ServerConnection::msSettings = ServerConnection::Settings();

ServerConnection::StateType ServerConnection::stateType( ServerConnection::State state )
{
    if( state <= DENIED ) return TYPE_CONNECTIONFAILED;
    if( state == CONNLOST || state == DISCONNECTED ) return TYPE_DISCONNECTED;
    if( state == CONNECTING || state == AUTHENTICATING ) return TYPE_CONNECTING;
    if( state == CONNECTED ) return TYPE_CONNECTED;
    
    return TYPE_NONE;
}

ServerConnection::ServerConnection( const ServerInfo& rServerInfo, 
    sigc::signal<void>& rUpdateSignal ):
    mServerInfo( rServerInfo ),
    mConnectionState( DISCONNECTED ),
    mRakPeer( *RakNet::RakPeerInterface::GetInstance() )
{
    rUpdateSignal.connect( sigc::mem_fun( this, &ServerConnection::update ) );

    mRPC3.SetNetworkIDManager( &mNetworkIDManager );
    mReplicaManager.SetNetworkIDManager( &mNetworkIDManager );

    mRakPeer.AttachPlugin( &mRPC3 );
    mRakPeer.AttachPlugin( &mReplicaManager );
    
}

ServerConnection::~ServerConnection()
{
    mRakPeer.DetachPlugin( &mRPC3 );
    mRakPeer.Shutdown( msSettings.mShutdownBlockDuractionMS );
    RakNet::RakPeerInterface::DestroyInstance( &mRakPeer );
}

bool ServerConnection::connect()
{
    if( mConnectionState != AUTHENTICATING && mConnectionState != CONNECTED )
    {
        if( mRakPeer.Startup( 1, &RakNet::SocketDescriptor(), 1 ) == RakNet::RAKNET_STARTED )
        {
            if( msSettings.mTimeoutMS != 0 )
                mRakPeer.SetTimeoutTime( msSettings.mTimeoutMS, RakNet::UNASSIGNED_SYSTEM_ADDRESS );

            if( mRakPeer.Connect( mServerInfo.mAddress.c_str(), mServerInfo.mPort, 0, 0, 0, 0,
                msSettings.mConnectionAttemptCount, msSettings.mTimeBetweenConnectionAttemptsMS, 
                msSettings.mTimeoutMS ) == RakNet::CONNECTION_ATTEMPT_STARTED )
            {
                // RakNet is trying to connect.
                ServerConnection::setState( CONNECTING );
                return true;
            }

            // Server address or port are invalid.
            ServerConnection::setState( CONNFAIL );
            return false;
        }

        // Could not create socket or thread. 
        ServerConnection::setState( SOCKETFAIL );
        return false;
    }

    return false;
}

void ServerConnection::disconnect()
{
    mRakPeer.Shutdown( msSettings.mShutdownBlockDuractionMS );
    ServerConnection::setState( DISCONNECTED );
}

void ServerConnection::update()
{
    RakNet::Packet *packet;
    for( packet = mRakPeer.Receive(); packet; mRakPeer.DeallocatePacket( packet ), 
        packet = mRakPeer.Receive() )
    {
        switch ( packet->data[0] )
        {
            case ID_DISCONNECTION_NOTIFICATION:
                // Connection lost normally.
                LCLOGI << "Disconnected from server: " << packet->systemAddress.ToString();
                mRakPeer.DeallocatePacket( packet );
                disconnect();
                return;
            case ID_ALREADY_CONNECTED:
                // Already connected to the server.
                LCLOGE << "Already connected to server: " << packet->systemAddress.ToString();
                mRakPeer.DeallocatePacket( packet );
                disconnect();
                return;
            case ID_CONNECTION_BANNED:
                // Banned from this server.
                LCLOGE << "Banned from server: " << packet->systemAddress.ToString();
                mRakPeer.DeallocatePacket( packet );
                disconnect();
                ServerConnection::setState( BANNED );
                return;
            case ID_CONNECTION_ATTEMPT_FAILED:
                // Connection attempt failed.
                LCLOGE << "Connection attempt failed: " << packet->systemAddress.ToString();
                mRakPeer.DeallocatePacket( packet );
                disconnect();
                ServerConnection::setState( CONNFAIL );
                return;
            case ID_NO_FREE_INCOMING_CONNECTIONS:
                // The server is full.
                LCLOGE << "Server is full: " << packet->systemAddress.ToString();
                mRakPeer.DeallocatePacket( packet );
                disconnect();
                ServerConnection::setState( FULL );
                return;
            case ID_CONNECTION_LOST:
                // Packet modification, disconnect.
                LCLOGE << "Connection to server was lost: " << packet->systemAddress.ToString();
                mRakPeer.DeallocatePacket( packet );
                disconnect();
                ServerConnection::setState( CONNLOST );
                return;
            case ID_CONNECTION_REQUEST_ACCEPTED:
                // Client successfully connected, authenticate with the server.
                LCLOGI << "Connected successfully, authenticating: " << 
                    packet->systemAddress.ToString();
                ServerConnection::setState( AUTHENTICATING );
                // TODO: Authentication, set state to CONNECTED for now.
                ServerConnection::setState( CONNECTED );
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
                LCLOGE << ss.str();
                break;
            }
        }
    }
}

void ServerConnection::setState( State state )
{
    mConnectionState = state;
    mStateChangedSignal( state, *this );
}

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia