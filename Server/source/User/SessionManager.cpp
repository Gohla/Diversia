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

#include "User/SessionManager.h"
#include "User/UserManager.h"
#include "User/User.h"

#include "Object/ServerObjectManager.h"
#include "Object/ServerObject.h"
#include "Object/ServerComponent.h"

#include "ClientServerPlugin/ClientPluginManager.h"

namespace Diversia
{
namespace Server 
{
//------------------------------------------------------------------------------

SessionManager::SessionManager( UserManager& rUserManager, ClientPluginManager& rPluginManager,
    sigc::signal<void>& rUpdateSignal ):
    mUserManager( rUserManager ),
    mPluginManager( rPluginManager )
{
    mUpdateConnection = rUpdateSignal.connect( sigc::mem_fun( this, 
        &SessionManager::update ) );
    mUpdateConnection.block( true );
}

SessionManager::~SessionManager()
{
    for( Sessions::iterator i = mSessions.begin(); i != mSessions.end(); ++i )
    {
        delete i->second;
    }

    mUpdateConnection.disconnect();
}

Session& SessionManager::addSession( RakNet::RakNetGUID guid, User& rUser )
{
    if( !hasSession( guid ) )
    {
        Session* session = new Session( guid, rUser );
        mSessions.insert( std::make_pair( guid, session ) );
        mSignal( *session, true );

        // TODO: Get all objects a certain user made, and add the correct items to that user's
        // permissions.

        return *session;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_DUPLICATE_ITEM, "A session already exists for this connection.", 
            "SessionManager::addSession" );
    }
}

Session& SessionManager::getSession( RakNet::RakNetGUID guid ) const
{
    Sessions::const_iterator i = mSessions.find( guid );
    if( i != mSessions.end() )
    {
        return *i->second;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Session does not exist.", 
            "SessionManager::getSession" );
    }
}

bool SessionManager::hasSession( RakNet::RakNetGUID guid ) const
{
    return mSessions.find( guid ) != mSessions.end();
}

void SessionManager::removeSession( RakNet::RakNetGUID guid )
{
    Sessions::iterator i = mSessions.find( guid );
    if( i != mSessions.end() )
    {
        // Remove the user if it's a guest user.
        if( i->second->getUser().isGuest() )
        {
            DataStructures::Multilist<ML_STACK, RakNet::Replica3*> replicaList;
            ServerObjectManager& objectManager = mPluginManager.getPlugin<ServerObjectManager>();
            objectManager.ObjectManager::getReplicaManager().GetReplicasCreatedByGuid( guid, 
                replicaList );

            // Destroy all guest objects.
            while( replicaList.GetSize() > 0 )
            {
                RakNet::Replica3* replica = replicaList.Pop();
                ServerObject* object = dynamic_cast<ServerObject*>( replica );
                if ( object != NULL )
                {
                	objectManager.destroyObject( object->getName() );
                }
            }

            // Destroy all guest components.
            replicaList.Clear();
            objectManager.ObjectManager::getReplicaManager().GetReplicasCreatedByGuid( guid, 
                replicaList );

            while( replicaList.GetSize() > 0 )
            {
                RakNet::Replica3* replica = replicaList.Pop();
                ServerComponent* component = dynamic_cast<ServerComponent*>( replica );
                if ( component != NULL )
                {
                    component->destroyComponent();
                }
            }

            // Delay removing the guest user once all replica's have been destroyed.
            mGuestsToDestroy.insert( guid );
            mUpdateConnection.block( false );
            
        }
        else
        {
            mSignal( *i->second, false );
            delete i->second;
            mSessions.erase( i );
        }
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Session does not exist.", 
            "SessionManager::removeSession" );
    }
}

void SessionManager::clear()
{
    for( Sessions::iterator i = mSessions.begin(); i != mSessions.end(); )
    {
        Sessions::iterator thisI = i++;
        SessionManager::removeSession( thisI->first );
    }
}

void SessionManager::update()
{
    LOGD << "SessionManager::update";

    bool block = true;

    for( std::set<RakNet::RakNetGUID>::iterator i = mGuestsToDestroy.begin(); i != mGuestsToDestroy.end(); )
    {
        DataStructures::Multilist<ML_STACK, RakNet::Replica3*> replicaList;
        ServerObjectManager& objectManager = mPluginManager.getPlugin<ServerObjectManager>();
        objectManager.ObjectManager::getReplicaManager().GetReplicasCreatedByGuid( *i, replicaList );

        if( replicaList.IsEmpty() )
        {
            // Guest does not own any more replica's, safe to destroy the guest user and session.
            Session& session = getSession( *i );
            mUserManager.removeUser( session.getUser().getName() );

            mSignal( session, false );
            delete &session;
            mSessions.erase( *i );

            mGuestsToDestroy.erase( i++ );
        }
        else
        {
            block = false;
            i++;
        }
    }

    if( block )
        mUpdateConnection.block( true );
}

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia
