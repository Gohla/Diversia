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

#include "Object/Object.h"
#include "Object/Component.h"
#include "Object/ObjectManager.h"
#include "Object/ComponentFactoryManager.h"
#include "Object/ComponentFactory.h"

namespace Diversia
{
namespace ObjectSystem
{
//------------------------------------------------------------------------------

std::map<ComponentType, String> Object::mAutoCreateComponents = std::map<ComponentType, String>();

Object::Object( const String& rName, Mode mode, NetworkingType type, const String& rDisplayName,
    RakNet::RakNetGUID source, RakNet::RakNetGUID ownGUID, RakNet::RakNetGUID serverGUID,
    sigc::signal<void>& rUpdateSignal, ObjectManager& rObjectManager,
    RakNet::ReplicaManager3& rReplicaManager, RakNet::NetworkIDManager& rNetworkIDManager,
    RakNet::RPC3& rRPC3 ):
    Node( rName ),
    mName( rName ),
    mDisplayName( rDisplayName ),
    mDisplayNameChanged( false ),
    mMode( mode ),
    mOwnGUID( ownGUID ),
    mServerGUID( serverGUID ),
    mType( type ),
    mBroadcastingDestruction( false ),
    mRuntime( false ),
    mSource( source == serverGUID ? SERVER : CLIENT ),
    mSourceGUID( source ),
    mParentChanged( false ),
    mUpdateSignal( rUpdateSignal ),
    mObjectManager( rObjectManager ),
    mReplicaManager( rReplicaManager ),
    mNetworkIDManager( rNetworkIDManager ),
    mRPC3( rRPC3 )
{
    OLOGD << "Object " << mName << " created";

    mUpdateConnection = mUpdateSignal.connect( sigc::mem_fun( this, &Object::update ) );

    this->SetNetworkIDManager( &mNetworkIDManager );

    // Broadcast object if necessary.
    if( ( mMode == SERVER && mType == REMOTE && mSource == SERVER ) ||
        ( mMode == CLIENT && mType == REMOTE && mSource == CLIENT ) )
    {
        Object::broadcastConstruction();
    }
}

Object::~Object()
{
    mDestructionSignal( *this );

    // Destroy all components that were queued for destruction in the next tick.
    for( ComponentsByType::reverse_iterator i = mDestroyedComponents.rbegin();
        i != mDestroyedComponents.rend(); ++i )
    {
        mComponentSignal( *i->second, false );
        if( i->second->queryBroadcastDestruction() ) i->second->broadcastDestruction();
        ComponentFactoryManager::getComponentFactory( i->first ).destroy( *i->second );
    }

    // Destroy all components immediately. This is safe since objects are already destroyed in the
    // next tick.
    for( ComponentsByType::reverse_iterator i = mComponentsByType.rbegin();
        i != mComponentsByType.rend(); ++i )
    {
        mComponentSignal( *i->second, false );
        if( i->second->queryBroadcastDestruction() ) i->second->broadcastDestruction();
        ComponentFactoryManager::getComponentFactory( i->second->getType() ).destroy( *i->second );
    }

    // Notify parent that this object will be destroyed by unparenting. Set rpc3 parameter to
    // prevent this call from being remote called to the client/server.
    /*Object::parent( 0, &mRPC3 );

    // Tell all childs to unparent, because this object is being destroyed.
    for( std::set<Object*>::iterator i = mChilds.begin(); i != mChilds.end(); ++i )
    {
        (*i)->setParent( 0 );
    }*/

    OLOGD << "Object " << mName << " destroyed";
}

void Object::setNetworkingType( NetworkingType type, bool redirectToChilds /*=false*/ )
{
    if( mType != type )
    {
        // Not the root parent and not redirecting to childs, let root handle it.
        if( !redirectToChilds && Node::hasParent() )
        {
            Object::getRootParentObject()->setNetworkingType( type );
            return;
        }

        // Check if it's possible to set the networking type for the whole tree of objects. Only
        // do this for the root object, because canSetNetworkingType will go trough the whole tree.
        if( !Node::hasParent() && !redirectToChilds )
        {
            Object::querySetNetworkingType( type, redirectToChilds );
        }

        mType = type;

        // Notify derived class of networking type change.
        setNetworkingTypeImpl( type );

        if( ( mMode == SERVER && mType == REMOTE ) || ( mMode == CLIENT && mType == REMOTE &&
            mSource == CLIENT ) )
        {
            Object::broadcastConstruction();
        }
        else if( mType == LOCAL )
        {
            Object::broadcastDestruction();
        }

        // Set networking type for all components.
        for( ComponentsByType::iterator i = mComponentsByType.begin(); i != mComponentsByType.end(); ++i )
        {
            i->second->setNetworkingType( mType );
        }

        // If this object is a root parent or it's redirecting to all childs, set the networking
        // type for all childs.
        if( !Node::hasParent() || redirectToChilds )
        {
            ObjectChilds childs = Object::getChildObjects();
            for( ObjectChilds::iterator i = childs.begin(); i != childs.end(); ++i )
            {
                // Set redirectToChilds to true so that the networking type will be set across the
                // whole tree of objects.
                i->second->setNetworkingType( type, true );
            }
        }
    }
}

void Object::querySetNetworkingType( NetworkingType type, bool redirectToChilds /*= false */ )
{
    // Not the root parent and not redirecting to childs, let root handle it.
    if( !redirectToChilds && Node::hasParent() )
    {
        Object::getRootParentObject()->querySetNetworkingType( type );
        return;
    }

    // Ask derived class if the networking type can be changed.
    querySetNetworkingTypeImpl( type );

    try
    {
        std::vector<Component*> cleanup;
        try
        {
            // Check if all components' networking type can be changed.
            for( ComponentsByType::iterator i = mComponentsByType.begin(); i != mComponentsByType.end(); ++i )
            {
                i->second->querySetNetworkingTypeImpl( type );
                cleanup.push_back( i->second );
            }

            // Cleanup the changes made by querySetNetworkingTypeImpl.
            for( ComponentsByType::reverse_iterator i = mComponentsByType.rbegin(); i != mComponentsByType.rend(); ++i )
            {
                i->second->cleanupQuerySetNetworkingType( type );
            }
        }
        catch ( Exception e )
        {
            // Cleanup the changes made by querySetNetworkingTypeImpl.
            for( std::vector<Component*>::iterator i = cleanup.begin(); i != cleanup.end(); ++i )
                (*i)->cleanupQuerySetNetworkingType( type );

            throw e;
        }

        // If this object is a root parent or it's redirecting to all childs, ask if the networking
        // type can be changed for all childs.
        if( !Node::hasParent() || redirectToChilds )
        {
            ObjectChilds childs = Object::getChildObjects();
            for( ObjectChilds::iterator j = childs.begin(); j != childs.end(); ++j )
            {
                // Set redirectToChilds to true so that the networking type will be set across the
                // whole tree of objects.
                j->second->querySetNetworkingType( type, true );
            }
        }

        // Cleanup the changes made by querySetNetworkingTypeImpl.
        cleanupQuerySetNetworkingType( type );
    }
    catch ( Exception e )
    {
        // Cleanup the changes made by querySetNetworkingTypeImpl.
        cleanupQuerySetNetworkingType( type );

        throw e;
    }
}

void Object::setDisplayName( const String& rDisplayName )
{
    mDisplayName = rDisplayName;
    mDisplayNameChanged = true;
    mDisplayNameSignal( mDisplayName );
}

Object& Object::createChildObject( const String& rName )
{
    Object& object = mObjectManager.createObject( rName, mType );
    object.parent( this );
    return object;
}

Component& Object::createComponent( ComponentType type, const String& rName,
    bool localOverride /*= false*/, RakNet::RakNetGUID source /*= RakNet::RakNetGUID( 0 )*/ )
{
    if( !Object::hasComponent( rName ) && ( !Object::hasComponent( type ) ||
        ComponentFactoryManager::getComponentFactory( type ).multiple() ) )
    {
        // Default to own GUID as source.
        if( source == RakNet::RakNetGUID( 0 ) ) source = mOwnGUID;

        // Create the component now, but the component specific part is initialized in the next
        // tick/frame update.
        queryCreateComponent( type, source, localOverride );
        Component& component = ComponentFactoryManager::getComponentFactory( type ).create(
            rName, mMode, mType, source, localOverride, *this );
        mComponentsByType.insert( std::make_pair( type, &component ) );
        mComponentsByName.insert( std::make_pair( rName, &component ) );
        mComponentsByHandle.insert( std::make_pair( ComponentHandle( type, rName ), &component ) );
        mCreatedComponents.insert( std::make_pair( type, &component ) );
        mUpdateConnection.block( false );
        return component;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_DUPLICATE_ITEM,
            "Component of this type already exists in this object.", "Object::createComponent" );
    }
}

Component& Object::getComponent( ComponentType type ) const
{
    ComponentsByType::const_iterator i = mComponentsByType.find( type );
    if( i != mComponentsByType.end() )
    {
        return *i->second;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Component not found in this object.",
            "Object::getComponent" );
    }
}

Component& Object::getComponent( const String& rName ) const
{
    ComponentsByName::const_iterator i = mComponentsByName.find( rName );
    if( i != mComponentsByName.end() )
    {
        return *i->second;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Component not found in this object.",
            "Object::getComponent" );
    }
}

bool Object::hasComponent( ComponentType type ) const
{
    return mComponentsByType.find( type ) != mComponentsByType.end();
}

bool Object::hasComponent( const String& rName ) const
{
    return mComponentsByName.find( rName ) != mComponentsByName.end();
}

std::size_t Object::componentCount( ComponentType type ) const
{
    return mComponentsByType.count( type );
}

void Object::destroyComponent( ComponentType type, RakNet::RakNetGUID source
    /*= RakNet::RakNetGUID( 0 )*/ )
{
    // Don't allow components with canDestroy set to false to be destroyed.
    if( !ComponentFactoryManager::getComponentFactory( type ).canDestroy() )
    {
        DIVERSIA_EXCEPT( Exception::ERR_INVALID_STATE,
            "Components of this type cannot be destroyed.",
            "Object::destroyComponent" );
    }

    // Don't allow destroying by type if there's more than one component with that type.
    // TODO: This is probably not expected behavior, investigate if it can be allowed.
    if( mComponentsByType.count( type ) > 1 )
    {
        DIVERSIA_EXCEPT( Exception::ERR_INTERNAL_ERROR,
            "There are multiple components of this type in this object.",
            "Object::destroyComponent" );
    }

    ComponentsByType::iterator i = mComponentsByType.find( type );
    if( i != mComponentsByType.end() )
    {
        // Default to own GUID as source.
        if( source == RakNet::RakNetGUID( 0 ) ) source = mOwnGUID;

        // Get permission to destroy the component.
        queryDestroyComponent( *i->second, source );

        // Destroy component in the next tick.
        Object::removeComponent( type );

        mUpdateConnection.block( false );
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Component not found in this object.",
            "Object::destroyComponent" );
    }
}

void Object::destroyComponent( const String& rName, RakNet::RakNetGUID source
    /*= RakNet::RakNetGUID( 0 )*/ )
{
    ComponentsByName::iterator i = mComponentsByName.find( rName );
    if( i != mComponentsByName.end() )
    {
        // Don't allow components with canDestroy set to false to be destroyed.
        if( !ComponentFactoryManager::getComponentFactory( i->second->getType() ).canDestroy() )
        {
            DIVERSIA_EXCEPT( Exception::ERR_INVALID_STATE,
                "Components of this type cannot be destroyed.",
                "Object::destroyComponent" );
        }

        // Default to own GUID as source.
        if( source == RakNet::RakNetGUID( 0 ) ) source = mOwnGUID;

        // Get permission to destroy the component.
        queryDestroyComponent( *i->second, source );

        // Destroy component in the next tick.
        Object::removeComponent( rName );

        mUpdateConnection.block( false );
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Component not found in this object.",
            "Object::destroyComponent" );
    }
}

void Object::destroyComponents( ComponentType type, RakNet::RakNetGUID source
    /*= RakNet::RakNetGUID( 0 )*/ )
{
    // Don't allow components with canDestroy set to false to be destroyed.
    if( !ComponentFactoryManager::getComponentFactory( type ).canDestroy() )
    {
        DIVERSIA_EXCEPT( Exception::ERR_INVALID_STATE,
            "Components of this type cannot be destroyed.",
            "Object::destroyComponent" );
    }

    ComponentsByType::iterator i = mComponentsByType.find( type );
    if( i != mComponentsByType.end() )
    {
        // Default to own GUID as source.
        if( source == RakNet::RakNetGUID( 0 ) ) source = mOwnGUID;

        // Get permission to destroy the component.
        queryDestroyComponent( *i->second, source );

        // Destroy component in the next tick.
        Object::removeComponent( type );

        mUpdateConnection.block( false );
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Component not found in this object.",
            "Object::destroyComponent" );
    }
}

void Object::addAutoCreateComponent( ComponentType type, const String& rName )
{
    mAutoCreateComponents.insert( std::make_pair( type, rName ) );
}

bool Object::hasAutoCreateComponent( ComponentType type )
{
    return mAutoCreateComponents.find( type ) != mAutoCreateComponents.end();
}

void Object::setClientControlled(
    RakNet::RakNetGUID controller /*= RakNet::UNASSIGNED_RAKNET_GUID */ )
{
    if( controller != mController )
    {
        mController = controller;
        mControllerSignal( mController );
    }
}

void Object::parent( Object* pObject, RakNet::RakNetGUID source /*= RakNet::RakNetGUID( 0 )*/ )
{
    if( Object::getParentObject() == pObject ) return;

    // Default to own GUID as source.
    if( source == RakNet::RakNetGUID( 0 ) ) source = mOwnGUID;

    // Query if parent may be changed.
    querySetParent( pObject, source );

    if( !pObject && Node::hasParent() )
    {
        Node::getParent()->removeChild( this );

        OLOGD << "Removing parent for object " << mName;
    }
    else if( pObject )
    {
        // Remove current parent
        if( Node::hasParent() )
            Node::getParent()->removeChild( this );

        // Change object networking type to parent networking type.
        if( mType != pObject->getNetworkingType() )
            Object::setNetworkingType( pObject->getNetworkingType() );

        // Set new parent
        pObject->addChild( this );

        OLOGD << "Setting parent for object " << mName << " to " << pObject->getName();
    }
}

void Object::parent( const String& rName )
{
    if( rName.empty() )
    {
        Object::parent( 0, mOwnGUID );
    }
    else if( mObjectManager.hasObject( rName ) )
    {
        Object::parent( &mObjectManager.getObject( rName ), mOwnGUID );
    }
    else
    {
        // Parent object does not exist yet, wait for it to be created.
        mQueuedParentName = rName;
        mQueuedParentConnection = mObjectManager.connect( sigc::mem_fun( this,
            &Object::objectChange ) );
    }
}

String Object::getParentName() const
{
    if( Node::hasParent() )
        return getParentObject()->getName();

    return String();
}

void Object::update()
{
    OLOGD << "Object::update";

    // TODO: Add a timer so objects do not live forever if the components forget to call
    // readyForDestruction().

    // Initialize components that were created in the previous tick/frame.
    for( ComponentsByType::iterator i = mCreatedComponents.begin(); i != mCreatedComponents.end();
        ++i )
    {
        i->second->create();
        mComponentSignal( *i->second, true );
    }
    mCreatedComponents.clear();

    // Destroy components that were queued for destruction in the previous tick/frame. Use reverse
    // iterator so components are destroyed in reverse order.
    for( ComponentsByType::reverse_iterator i = mDestroyedComponents.rbegin();
        i != mDestroyedComponents.rend(); ++i )
    {
        mComponentSignal( *i->second, false );
        if( i->second->queryBroadcastDestruction() ) i->second->broadcastDestruction();
        ComponentFactoryManager::getComponentFactory( i->first ).destroy( *i->second );
    }
    mDestroyedComponents.clear();

    mUpdateConnection.block( true );
}

void Object::SerializeConstruction( RakNet::BitStream* pConstructionBitstream,
    RakNet::Connection_RM3* pDestinationConnection )
{
    // Serialize parent NetworkID and controller.
    if( Node::hasParent() )
    {
        pConstructionBitstream->Write1();
        pConstructionBitstream->Write( Object::getParentObject()->GetNetworkID() );
    }
    else
    {
        pConstructionBitstream->Write0();
    }
    pConstructionBitstream->Write( mController );

    // Serialize transform
    *pConstructionBitstream << Node::mPosition;
    *pConstructionBitstream << Node::mOrientation;
    *pConstructionBitstream << Node::mScale;
}

bool Object::DeserializeConstruction( RakNet::BitStream* pConstructionBitstream,
    RakNet::Connection_RM3* pSourceConnection )
{
    // Deserialize parent NetworkID and controller.
    bool hasParent = pConstructionBitstream->ReadBit();
    if( hasParent )
    {
        RakNet::NetworkID parentID; pConstructionBitstream->Read<RakNet::NetworkID>( parentID );
        Object::parent( mNetworkIDManager.GET_OBJECT_FROM_ID<Object*>( parentID ) );
    }
    pConstructionBitstream->Read<RakNet::RakNetGUID>( mController );

    // Deserialize transform
    *pConstructionBitstream >> Node::mPosition;
    *pConstructionBitstream >> Node::mOrientation;
    *pConstructionBitstream >> Node::mScale;
    Node::needUpdate();

    // Broadcast construction now if this is a remote object on the server, created by a client.
    if( mMode == SERVER && mType == REMOTE && mSource == CLIENT )
    {
        Object::broadcastConstruction();
    }

    return true;
}

bool Object::DeserializeDestruction( RakNet::BitStream* pDestructionBitstream,
    RakNet::Connection_RM3* pSourceConnection )
{
    mDestructionSignal( *this );
    return true;
}

RakNet::RM3QuerySerializationResult Object::QuerySerialization(
    RakNet::Connection_RM3* pDestinationConnection )
{
    if( mMode == SERVER || ( mMode == CLIENT && ( Object::isThisControlled() || mParentChanged ||
        mDisplayNameChanged ) ) )
    {
        return RakNet::RM3QSR_CALL_SERIALIZE;
    }

    return RakNet::RM3QSR_DO_NOT_CALL_SERIALIZE;
}

RakNet::RM3SerializationResult Object::Serialize( RakNet::SerializeParameters* pSerializeParameters )
{
    // Serialize parent
    if( Node::hasParent() && mParentChanged )
    {
        pSerializeParameters->outputBitstream[0].Write( Object::getParentObject()->GetNetworkID() );
        mParentChanged = false;
    }
    else if( mParentChanged )
    {
        pSerializeParameters->outputBitstream[1].Write1();
        mParentChanged = false;
    }

    // Serialize display name
    if( mDisplayNameChanged )
    {
        pSerializeParameters->outputBitstream[2] << RakNet::RakString( mDisplayName.c_str() );
        mDisplayNameChanged = false;
    }

    // Serialize transform
    if( mMode == SERVER || ( mMode == CLIENT && Object::isThisControlled() ) )
    {
        pSerializeParameters->outputBitstream[3] << Node::mPosition;
        pSerializeParameters->outputBitstream[3] << Node::mOrientation;
        pSerializeParameters->outputBitstream[3] << Node::mScale;
    }

    return RakNet::RM3SR_BROADCAST_IDENTICALLY;
}

void Object::Deserialize( RakNet::DeserializeParameters* pDeserializeParameters )
{
    if( mType == REMOTE )
    {
        try
        {
            if( pDeserializeParameters->bitstreamWrittenTo[0] && mType )
            {
                // Set new parent
                Object* parent = 0;

                RakNet::NetworkID parentID; pDeserializeParameters->serializationBitstream[0].Read<RakNet::NetworkID>( parentID );
                parent = mNetworkIDManager.GET_OBJECT_FROM_ID<Object*>( parentID );

                if( Object::getParentObject() != parent && parent != 0 )
                {
                    Object::parent( parent, pDeserializeParameters->sourceConnection->GetRakNetGUID() );
                    mParentChanged = false;
                }
            }
            else if( pDeserializeParameters->bitstreamWrittenTo[1] )
            {
                // Unparent
                Object::parent( 0, pDeserializeParameters->sourceConnection->GetRakNetGUID() );
                mParentChanged = false;
            }
        }
        catch( Exception e )
        {
            OLOGD << "Could not change parent object: " << e.what();
        }

        // Deserialize display name
        if( pDeserializeParameters->bitstreamWrittenTo[2] )
        {
            RakNet::RakString str; pDeserializeParameters->serializationBitstream[2] >> str;
            mDisplayName = str.C_String();
            mDisplayNameSignal( mDisplayName );
        }

        if( pDeserializeParameters->bitstreamWrittenTo[3] &&
            !( mMode == CLIENT && Object::isThisControlled() ) )
        {
            // TODO: Only allow controller to change the transform.
            // Deserialize transform
            pDeserializeParameters->serializationBitstream[3] >> Node::mPosition;
            pDeserializeParameters->serializationBitstream[3] >> Node::mOrientation;
            pDeserializeParameters->serializationBitstream[3] >> Node::mScale;
            Node::needUpdate();
        }
    }
}

bool Object::delayedDestruction()
{
    if( !mDelayedDestruction.empty() )
    {
        // Trying to do delayed destruction while delayed destruction is already initiated.
        return true;
    }

    for( ComponentsByType::iterator i = mComponentsByType.begin(); i != mComponentsByType.end(); ++i )
    {
        if( i->second->delayedDestruction() )
        {
            mDelayedDestruction.insert( i->second );
        }
    }

    if( !mDelayedDestruction.empty() )
    {
        // A component requested delayed destruction.
        return true;
    }

    return false;
}

void Object::readyForDestruction( Component& rComponent )
{
    if( !mDelayedDestruction.empty() && hasComponent( rComponent.getName() ) )
    {
        mDelayedDestruction.erase( &rComponent );

        if( mDelayedDestruction.empty() )
        {
            // All components are ready for destruction now.
            mObjectManager.readyForDestruction( *this );
        }
    }
}

void Object::broadcastConstruction()
{
    OLOGD << "Broadcasting construction for object " << mName;
    mReplicaManager.Reference( this );
}

bool Object::queryBroadcastDestruction()
{
    return ( ( mMode == SERVER && mType == REMOTE ) || ( mMode == CLIENT && mType == REMOTE &&
        mSource == CLIENT ) );
}

void Object::broadcastDestruction()
{
    OLOGD << "Broadcasting destruction for object " << mName;

    mBroadcastingDestruction = true;
    Replica3::BroadcastDestruction();
    mReplicaManager.Dereference( this );
    mBroadcastingDestruction = false;
}

void Object::removeComponent( ComponentType type )
{
    std::pair<ComponentsByType::iterator, ComponentsByType::iterator> get = getComponents( type );

    for( ComponentsByType::iterator i = get.first; i != get.second; ++i )
    {
        mComponentsByName.erase( i->second->getName() );
        ComponentHandle handle( type, i->second->getName() );
        mComponentsByHandle.erase( handle );
    }

    mDestroyedComponents.insert( get.first, get.second );
    mComponentsByType.erase( get.first, get.second );
}

void Object::removeComponent( const String& rName )
{
    ComponentsByName::iterator i = mComponentsByName.find( rName );
    if( i != mComponentsByName.end() )
    {
        std::pair<ComponentsByType::iterator, ComponentsByType::iterator> get =
            getComponents( i->second->getType() );

        for( ComponentsByType::iterator j = get.first; j != get.second; ++j )
        {
            if( j->second == i->second )
            {
                mComponentsByType.erase( j );
                break;
            }
        }

        mDestroyedComponents.insert( std::make_pair( i->second->getType(), i->second ) );
        ComponentHandle handle( i->second->getType(), rName );
        mComponentsByHandle.erase( handle );
        mComponentsByName.erase( i );
    }
}

void Object::WriteAllocationID( RakNet::Connection_RM3* pConnection,
    RakNet::BitStream* pAllocationIdBitstream ) const
{
    // Write object replica type and object name as identifier.
    pAllocationIdBitstream->Write<ReplicaType>( REPLICATYPE_OBJECT );
    pAllocationIdBitstream->Write<RakNet::RakString>( RakNet::RakString( mName.c_str() ) );
    pAllocationIdBitstream->Write<RakNet::RakString>( RakNet::RakString( mDisplayName.c_str() ) );
}

void Object::DeallocReplica( RakNet::Connection_RM3* pSourceConnection )
{
    if( !mBroadcastingDestruction )
    {
        // Object is already destroyed so no permission checking is needed, act as server.
        try
        {
            mDestructionSignal( *this );
            mObjectManager.destroyObject( mName, mServerGUID );
        }
        catch ( Exception e )
        {
            // TODO: Is this bad? Just ignoring the error for now..
            OLOGW << "Failed to deallocate object from replica system: " << e.what();
            //DivAssert( 0, e.what() );
        }
    }
}

void Object::create()
{
    // Create components that should be automatically created.
    for( std::map<ComponentType, String>::iterator i = mAutoCreateComponents.begin();
        i!= mAutoCreateComponents.end(); ++i )
    {
        Object::createComponent( i->first, i->second, true );
    }
}

void Object::setParent( Node* pParent )
{
    Node* parent = Node::getParent();
    Node::setParent( pParent );

    if( parent != pParent )
    {
        mParentSignal( static_cast<Object*>( pParent ) );
        mParentChanged = true;
    }
}

void Object::objectChange( Object& rObject, bool created )
{
    if( rObject.getName() == mQueuedParentName )
    {
        if( created ) Object::parent( &rObject );
        mQueuedParentName.clear();
        mQueuedParentConnection.disconnect();
    }
}

//------------------------------------------------------------------------------
} // Namespace Object
} // Namespace Diversia