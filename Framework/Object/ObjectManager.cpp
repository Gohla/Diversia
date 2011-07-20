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

#include "Object/Platform/StableHeaders.h"

#include "Object/ObjectManager.h"
#include "Object/Object.h"

namespace Diversia
{
namespace ObjectSystem 
{
//------------------------------------------------------------------------------

ObjectManager::ObjectManager( Mode mode, RakNet::RakNetGUID ownGUID, RakNet::RakNetGUID serverGUID,
    sigc::signal<void>& rUpdateSignal, sigc::signal<void>& rLateUpdateSignal, 
    ObjectTemplateManager& rObjectTemplateManager, RakNet::ReplicaManager3& rReplicaManager, 
    RakNet::NetworkIDManager& rNetworkIDManager, RakNet::RPC3& rRPC3, bool offlineMode /*= false*/ ): 
    mMode( mode ),
    mOfflineMode( offlineMode ),
    mOwnGUID( ownGUID ),
    mServerGUID( serverGUID ),
    mUpdateSignal( rUpdateSignal ),
    mObjectTemplateManager( rObjectTemplateManager ),
    mReplicaManager( rReplicaManager ),
    mNetworkIDManager( rNetworkIDManager ),
    mRPC3( rRPC3 )
{
    mUpdateConnection = rUpdateSignal.connect( sigc::mem_fun( this, &ObjectManager::update ) );
    mUpdateConnection.block( true );
    rLateUpdateSignal.connect( sigc::mem_fun( this, &ObjectManager::lateUpdate ) );
}

ObjectManager::~ObjectManager()
{
    ObjectManager::reset();
}

Object& ObjectManager::createObject( const String& rName, NetworkingType type /*= LOCAL*/, 
    const String& rDisplayName /*= ""*/, RakNet::RakNetGUID source /*= RakNet::RakNetGUID( 0 )*/ )
{
    if( !hasObject( rName ) )
    {
        // Default to own GUID as source.
        if( source == RakNet::RakNetGUID( 0 ) ) source = mOwnGUID;

        // Set to local if in offline mode.
        if( mOfflineMode ) type = LOCAL;

        Object& object = createObjectImpl( rName, type, rDisplayName.empty()? rName : rDisplayName, 
            source );
        mObjects.insert( std::make_pair( rName, &object ) );
        object.create();
        mObjectSignal( object, true );
        return object;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_DUPLICATE_ITEM, "Object already exists.", 
            "ObjectManager::createObject" );
    }
}

Object& ObjectManager::createRuntimeObject( const String& rName, NetworkingType type /*= LOCAL*/, 
    const String& rDisplayName /*= ""*/, RakNet::RakNetGUID source /*= RakNet::RakNetGUID( 0 ) */ )
{
    Object& object = ObjectManager::createObject( rName, type, rDisplayName, source );
    object.setRuntimeObject( true );
    return object;
}

Object& ObjectManager::getObject( const String& rName ) const
{
    Objects::const_iterator i = mObjects.find( rName );
    if( i != mObjects.end() )
    {
        return *i->second;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Object does not exist.", 
            "ObjectManager::getObject" );
    }
}

bool ObjectManager::hasObject( const String& rName ) const
{
    return mObjects.find( rName ) != mObjects.end();
}

void ObjectManager::destroyObject( Object& rObject, RakNet::RakNetGUID source /*= RakNet::RakNetGUID( 0 ) */ )
{
    // Default to own GUID as source.
    if( source == RakNet::RakNetGUID( 0 ) ) source = mOwnGUID;

    // Get permission to destroy the object.
    queryDestroyObject( rObject, source );

    if( !rObject.queryDelayedDestruction() && !rObject.delayedDestruction() )
    {
        // Destroy the object in the next tick.
        readyForDestruction( rObject );
    }

    // Object needs delayed destruction, destroy it when it calls readyForDestruction.
}

void ObjectManager::destroyObjectTree( Object& rObject, 
    RakNet::RakNetGUID source /*= RakNet::RakNetGUID( 0 ) */, bool redirectToChilds /*= false*/ )
{
    // Destroy given object.
    ObjectManager::destroyObject( rObject, source );

    // Destroy all childs.
    if( !rObject.hasParent() || redirectToChilds )
    {
        ObjectHashMap childs = rObject.getChildObjects();
        for( ObjectHashMap::iterator i = childs.begin(); i != childs.end(); ++i )
        {
            // Set redirectToChilds to true so that the call is forwarded to all childs.
            ObjectManager::destroyWholeObjectTree( *i->second, source, true );
        }
    }
}

void ObjectManager::destroyWholeObjectTree( Object& rObject, 
    RakNet::RakNetGUID source /*= RakNet::RakNetGUID( 0 ) */, bool redirectToChilds /*= false*/ )
{
    // Not the root parent and not redirecting to childs, let root handle it.
    if( !redirectToChilds && rObject.hasParent() )
    {
        ObjectManager::destroyWholeObjectTree( *rObject.getRootParentObject(), source );
        return;
    }

    // Destroy given object.
    ObjectManager::destroyObject( rObject, source );

    // Destroy all childs.
    if( !rObject.hasParent() || redirectToChilds )
    {
        ObjectHashMap childs = rObject.getChildObjects();
        for( ObjectHashMap::iterator i = childs.begin(); i != childs.end(); ++i )
        {
            // Set redirectToChilds to true so that the call is forwarded to all childs.
            ObjectManager::destroyWholeObjectTree( *i->second, source, true );
        }
    }
}

void ObjectManager::reset()
{
    for( Objects::iterator i = mObjects.begin(); i != mObjects.end(); ++i )
    {
        mObjectSignal( *i->second, false );
        delete i->second;
    }
    mObjects.clear();
    mDestroyedObjects.clear();
    mUpdateConnection.block( true );
}

void ObjectManager::offlineModeChanged( bool offlineMode )
{
    if( mOfflineMode != offlineMode )
    {
        mOfflineMode = offlineMode;

        if( mOfflineMode )
        {
            for( Objects::iterator i = mObjects.begin(); i != mObjects.end(); ++i )
            {
                i->second->setNetworkingType( LOCAL );
            }
        }
    }
}

void ObjectManager::update()
{
    OLOGD << "ObjectManager::update";

    // Destroy objects that were queued for destruction in the previous tick/frame.
    for( std::set<Object*>::iterator i = mDestroyedObjects.begin(); i != mDestroyedObjects.end(); 
        ++i )
    {
        mObjectSignal( **i, false );
        mObjects.erase( (*i)->getName() );
        if( (*i)->queryBroadcastDestruction() ) (*i)->broadcastDestruction();
        delete *i;
    }
    mDestroyedObjects.clear();

    mUpdateConnection.block( true );
}

void ObjectManager::readyForDestruction( Object& rObject )
{
    mDestroyedObjects.insert( &rObject );
    mUpdateConnection.block( false );
}

void ObjectManager::lateUpdate()
{
    Node::processQueuedUpdates();

    /*for( Objects::iterator i = mObjects.begin(); i != mObjects.end(); ++i )
        i->second->_update(true, false);*/
}

//------------------------------------------------------------------------------
} // Namespace Object
} // Namespace Diversia
