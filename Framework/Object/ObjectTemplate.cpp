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

#include "Object/Platform/StableHeaders.h"

#include "Object/Component.h"
#include "Object/ComponentFactory.h"
#include "Object/ComponentFactoryManager.h"
#include "Object/ComponentTemplate.h"
#include "Object/ObjectTemplate.h"

namespace Diversia
{
namespace ObjectSystem
{
//------------------------------------------------------------------------------

ObjectTemplate::ObjectTemplate( const String& rName, Mode mode, NetworkingType type, 
    const String& rDisplayName, RakNet::RakNetGUID source, RakNet::RakNetGUID ownGUID, 
    RakNet::RakNetGUID serverGUID, ObjectTemplateManager& rObjectTemplateManager,
    RakNet::ReplicaManager3& rReplicaManager, RakNet::NetworkIDManager& rNetworkIDManager ):
    Node( rName ),
    mName( rName ),
    mDisplayName( rDisplayName ),
    mMode( mode ),
    mOwnGUID( ownGUID ),
    mServerGUID( serverGUID ),
    mSource( source == serverGUID ? SERVER : CLIENT ),
    mSourceGUID( source ),
    mType( type ),
    mBroadcastingDestruction( false ),
    mRuntime( false ),
    mObjectTemplateManager( rObjectTemplateManager ),
    mParentChanged( false ),
    mReplicaManager( rReplicaManager ),
    mNetworkIDManager( rNetworkIDManager )
{
    OLOGD << "Object template" << mName << " created";

    this->SetNetworkIDManager( &mNetworkIDManager );

    // Broadcast object if necessary.
    if( ( mMode == SERVER && mType == REMOTE && mSource == SERVER ) ||
        ( mMode == CLIENT && mType == REMOTE && mSource == CLIENT ) )
    {
        ObjectTemplate::broadcastConstruction();
    }
}

ObjectTemplate::~ObjectTemplate()
{
    // Destroy all component templates.
    for( ComponentTemplatesByType::reverse_iterator i = mComponentTemplatesByType.rbegin();
        i != mComponentTemplatesByType.rend(); ++i )
    {
        if( i->second->queryBroadcastDestruction() ) i->second->broadcastDestruction();
        delete i->second;
    }

    mDestructionSignal( *this );

    OLOGD << "Object template" << mName << " destroyed";
}

void ObjectTemplate::setNetworkingType( NetworkingType type, bool redirectToChilds /*= false */ )
{
    if( mType != type )
    {
        // Not the root parent and not redirecting to childs, let root handle it.
        if( !redirectToChilds && Node::hasParent() )
        {
            ObjectTemplate::getRootParentObjectTemplate()->setNetworkingType( type );
            return;
        }

        // Check if it's possible to set the networking type for the whole tree of objects. Only
        // do this for the root object, because canSetNetworkingType will go trough the whole tree.
        if( !Node::hasParent() && !redirectToChilds )
        {
            ObjectTemplate::querySetNetworkingType( type, redirectToChilds );
        }

        mType = type;

        // Notify derived class of networking type change.
        setNetworkingTypeImpl( type );

        if( ( mMode == SERVER && mType == REMOTE ) || ( mMode == CLIENT && mType == REMOTE &&
            mSource == CLIENT ) )
        {
            ObjectTemplate::broadcastConstruction();
        }
        else if( mType == LOCAL )
        {
            ObjectTemplate::broadcastDestruction();
        }

        // Set networking type for all components.
        for( ComponentTemplatesByType::iterator i = mComponentTemplatesByType.begin(); 
            i != mComponentTemplatesByType.end(); ++i )
        {
            i->second->setNetworkingType( mType );
        }

        // If this object is a root parent or it's redirecting to all childs, set the networking
        // type for all childs.
        if( !Node::hasParent() || redirectToChilds )
        {
            ObjectTemplateChilds childs = ObjectTemplate::getChildObjectTemplates();
            for( ObjectTemplateChilds::iterator i = childs.begin(); i != childs.end(); ++i )
            {
                // Set redirectToChilds to true so that the networking type will be set across the
                // whole tree of objects.
                i->second->setNetworkingType( type, true );
            }
        }
    }
}

void ObjectTemplate::querySetNetworkingType( NetworkingType type, 
    bool redirectToChilds /*= false */ )
{
    // Not the root parent and not redirecting to childs, let root handle it.
    if( !redirectToChilds && Node::hasParent() )
    {
        ObjectTemplate::getRootParentObjectTemplate()->querySetNetworkingType( type );
        return;
    }

    // Ask derived class if the networking type can be changed.
    querySetNetworkingTypeImpl( type );

    try
    {
        std::vector<ComponentTemplate*> cleanup;
        try
        {
            // Check if all components' networking type can be changed.
            for( ComponentTemplatesByType::iterator i = mComponentTemplatesByType.begin(); 
                i != mComponentTemplatesByType.end(); ++i )
            {
                i->second->querySetNetworkingTypeImpl( type );
                cleanup.push_back( i->second );
            }

            // Cleanup the changes made by querySetNetworkingTypeImpl.
            for( ComponentTemplatesByType::reverse_iterator i = mComponentTemplatesByType.rbegin(); 
                i != mComponentTemplatesByType.rend(); ++i )
            {
                i->second->cleanupQuerySetNetworkingType( type );
            }
        }
        catch ( Exception e )
        {
            // Cleanup the changes made by querySetNetworkingTypeImpl.
            for( std::vector<ComponentTemplate*>::iterator i = cleanup.begin(); i != cleanup.end(); 
                ++i )
                (*i)->cleanupQuerySetNetworkingType( type );

            throw e;
        }

        // If this object is a root parent or it's redirecting to all childs, ask if the networking
        // type can be changed for all childs.
        if( !Node::hasParent() || redirectToChilds )
        {
            ObjectTemplateChilds childs = ObjectTemplate::getChildObjectTemplates();
            for( ObjectTemplateChilds::iterator j = childs.begin(); j != childs.end(); ++j )
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

void ObjectTemplate::setDisplayName( const String& rDisplayName )
{
    mDisplayName = rDisplayName;
    mDisplayNameSignal( mDisplayName );
}

ObjectTemplate& ObjectTemplate::createChildObjectTemplate( const String& rName, 
    const String& rDisplayName )
{
    ObjectTemplate& objectTemplate = mObjectTemplateManager.createObjectTemplate( rName, mType );
    objectTemplate.parent( this );
    objectTemplate.setDisplayName( rDisplayName );
    return objectTemplate;
}

ComponentTemplate& ObjectTemplate::createComponentTemplate( ComponentType type, const String& rName,
    bool localOverride /*= false*/, RakNet::RakNetGUID source /*= RakNet::RakNetGUID( 0 )*/ )
{
    if( !ObjectTemplate::hasComponentTemplate( rName ) && 
        ( !ObjectTemplate::hasComponentTemplate( type ) ||
        ComponentFactoryManager::getComponentFactory( type ).multiple() ) )
    {
        // Default to own GUID as source.
        if( source == RakNet::RakNetGUID( 0 ) ) source = mOwnGUID;

        queryCreateComponentTemplate( type, localOverride, source );
        ComponentTemplate* componentTemplate = createComponentTemplateImpl( type, rName, 
            localOverride, source );

        mComponentTemplatesByType.insert( std::make_pair( type, componentTemplate ) );
        mComponentTemplatesByName.insert( std::make_pair( rName, componentTemplate ) );
        mComponentTemplatesByHandle.insert( std::make_pair( ComponentHandle( type, rName ), 
            componentTemplate ) );
        mComponentTemplateSignal( *componentTemplate, true );

        return *componentTemplate;
    } 
    else 
    {
        DIVERSIA_EXCEPT( Exception::ERR_DUPLICATE_ITEM,
            "Component template of this type already exists in this object template.", 
            "ObjectTemplate::createComponentTemplate" );
    }
}

ComponentTemplate& ObjectTemplate::createComponentTemplate( const Component& rComponent )
{
    ComponentTemplate& componentTemplate = ObjectTemplate::createComponentTemplate( 
        rComponent.getType(), rComponent.getName(), rComponent.getLocalOverride(), mOwnGUID );
    componentTemplate.setTemplateProperties( rComponent );
    return componentTemplate;
}

void ObjectTemplate::createComponentTemplates( const Object& rObject )
{
    // Set transform from object.
    Node::setPosition( rObject.getPosition() );
    Node::setOrientation( rObject.getOrientation() );
    Node::setScale( rObject.getScale() );

    // Recursively go trough all child objects.
    ObjectHashMap childs = rObject.getChildObjects();
    for( ObjectHashMap::iterator i = childs.begin(); i != childs.end(); ++i )
    {
        mObjectTemplateManager.createObjectTemplate( *i->second, 
            mObjectTemplateManager.generateName(), mType ).parent( this );
    }

    // Create all components in object as component templates.
    const ComponentsByType& components = rObject.getComponentsByType();
    for( ComponentsByType::const_iterator i = components.begin(); i != components.end(); ++i )
    {
        if( !Object::hasAutoCreateComponent( i->first ) ) 
            ObjectTemplate::createComponentTemplate( *i->second );
    }
}

ComponentTemplate& ObjectTemplate::getComponentTemplate( ComponentType type ) const
{
    ComponentTemplatesByType::const_iterator i = mComponentTemplatesByType.find( type );
    if( i != mComponentTemplatesByType.end() )
    {
        return *i->second;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, 
            "Component template not found in this object template.",
            "ObjectTemplate::getComponentTemplate" );
    }
}

ComponentTemplate& ObjectTemplate::getComponentTemplate( const String& rName ) const 
{
    ComponentTemplatesByName::const_iterator i = mComponentTemplatesByName.find( rName );
    if( i != mComponentTemplatesByName.end() )
    {
        return *i->second;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, 
            "Component template not found in this object template.",
            "ObjectTemplate::getComponentTemplate" );
    }
}

bool ObjectTemplate::hasComponentTemplate( ComponentType type ) const
{
    return mComponentTemplatesByType.find( type ) != mComponentTemplatesByType.end();
}

bool ObjectTemplate::hasComponentTemplate( const String& rName ) const
{
    return mComponentTemplatesByName.find( rName ) != mComponentTemplatesByName.end();
}

void ObjectTemplate::destroyComponentTemplate( ComponentType type, 
    RakNet::RakNetGUID source /*= RakNet::RakNetGUID( 0 )*/ )
{
    // Don't allow destroying by type if there's more than one component template with that type.
    if( mComponentTemplatesByType.count( type ) > 1 )
    {
        DIVERSIA_EXCEPT( Exception::ERR_INTERNAL_ERROR,
            "There are multiple component templates of this type in this object template.",
            "ObjectTemplate::destroyComponentTemplate" );
    }

    ComponentTemplatesByType::iterator i = mComponentTemplatesByType.find( type );
    if( i != mComponentTemplatesByType.end() )
    {
        ComponentTemplate* componentTemplate = i->second;

        // Default to own GUID as source.
        if( source == RakNet::RakNetGUID( 0 ) ) source = mOwnGUID;

        // Get permission to destroy the component template.
        queryDestroyComponentTemplate( *componentTemplate, source );

        // Destroy component template
        ObjectTemplate::removeComponentTemplate( type );
        mComponentTemplateSignal( *componentTemplate, false );
        delete componentTemplate;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, 
            "Component template not found in this object template.",
            "ObjectTemplate::destroyComponentTemplate" );
    }
}

void ObjectTemplate::destroyComponentTemplate( const String& rName, 
    RakNet::RakNetGUID source /*= RakNet::RakNetGUID( 0 )*/ )
{
    ComponentTemplatesByName::iterator i = mComponentTemplatesByName.find( rName );
    if( i != mComponentTemplatesByName.end() )
    {
        ComponentTemplate* componentTemplate = i->second;

        // Default to own GUID as source.
        if( source == RakNet::RakNetGUID( 0 ) ) source = mOwnGUID;

        // Get permission to destroy the component template.
        queryDestroyComponentTemplate( *componentTemplate, source );

        // Destroy component template
        ObjectTemplate::removeComponentTemplate( rName );
        mComponentTemplateSignal( *componentTemplate, false );
        delete componentTemplate;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, 
            "Component template not found in this object template.",
            "ObjectTemplate::destroyComponentTemplate" );
    }
}

Object& ObjectTemplate::createObject( ObjectManager& rObjectManager, const String& rName, 
    NetworkingType type )
{
    // Create object for this object template.
    Object& object = rObjectManager.createObject( rName, type, mDisplayName );

    // Set transform to object.
    object.setPosition( Node::getPosition() );
    object.setOrientation( Node::getOrientation() );
    object.setScale( Node::getScale() );
    object.setRuntimeObject( mRuntime );

    // Recursively go trough all child object templates.
    ObjectTemplateChilds childs = ObjectTemplate::getChildObjectTemplates();
    for( ObjectTemplateChilds::iterator i = childs.begin(); i != childs.end(); ++i )
    {
        i->second->createObject( rObjectManager, rName + "Parent", type ).parent( &object );
    }

    // Create all components in created object.
    for( ComponentTemplatesByType::iterator i = mComponentTemplatesByType.begin(); 
        i != mComponentTemplatesByType.end(); ++i )
    {
        if( !Object::hasAutoCreateComponent( i->first ) ) i->second->createComponent( object );
    }

    object.setTemplate( this );

    return object;
}

void ObjectTemplate::parent( ObjectTemplate* pObjectTemplate, 
    RakNet::RakNetGUID source /*= RakNet::RakNetGUID( 0 ) */ )
{
    if( ObjectTemplate::getParentObjectTemplate() == pObjectTemplate ) return;

    // Default to own GUID as source.
    if( source == RakNet::RakNetGUID( 0 ) ) source = mOwnGUID;

    // Query if parent may be changed.
    querySetParent( pObjectTemplate, source );

    if( !pObjectTemplate && Node::hasParent() )
    {
        Node::getParent()->removeChild( this );

        OLOGD << "Removing parent for object template " << mName;
    }
    else if( pObjectTemplate )
    {
        // Remove current parent
        if( Node::hasParent() )
            Node::getParent()->removeChild( this );

        // Change object networking type to parent networking type.
        if( mType != pObjectTemplate->getNetworkingType() )
            ObjectTemplate::setNetworkingType( pObjectTemplate->getNetworkingType() );

        // Set new parent
        pObjectTemplate->addChild( this );

        OLOGD << "Setting parent for object template " << mName << " to " << 
            pObjectTemplate->getName();
    }
}

void ObjectTemplate::parent( const String& rName )
{
    if( rName.empty() )
    {
        ObjectTemplate::parent( 0, mOwnGUID );
    }
    else if( mObjectTemplateManager.hasObjectTemplate( rName ) )
    {
        ObjectTemplate::parent( &mObjectTemplateManager.getObjectTemplate( rName ), mOwnGUID );
    }
    else
    {
        // Parent object does not exist yet, wait for it to be created.
        mQueuedParentName = rName;
        mQueuedParentConnection = mObjectTemplateManager.connect( sigc::mem_fun( this,
            &ObjectTemplate::objectTemplateChange ) );
    }
}

String ObjectTemplate::getParentName() const
{
    if( Node::hasParent() )
        return getParentObjectTemplate()->getName();

    return String();
}

void ObjectTemplate::broadcastConstruction()
{
    OLOGD << "Broadcasting construction for object template " << mName;
    mReplicaManager.Reference( this );
}

bool ObjectTemplate::queryBroadcastDestruction()
{
    return ( ( mMode == SERVER && mType == REMOTE ) || ( mMode == CLIENT && mType == REMOTE &&
        mSource == CLIENT ) );
}

void ObjectTemplate::broadcastDestruction()
{
    OLOGD << "Broadcasting destruction for object template" << mName;

    mBroadcastingDestruction = true;
    Replica3::BroadcastDestruction();
    mReplicaManager.Dereference( this );
    mBroadcastingDestruction = false;
}

void ObjectTemplate::removeComponentTemplate( ComponentType type )
{
    std::pair<ComponentTemplatesByType::iterator, ComponentTemplatesByType::iterator> get = 
        mComponentTemplatesByType.equal_range( type );

    for( ComponentTemplatesByType::iterator i = get.first; i != get.second; ++i )
    {
        mComponentTemplatesByName.erase( i->second->getName() );
        ComponentHandle handle( type, i->second->getName() );
        mComponentTemplatesByHandle.erase( handle );
    }

    mComponentTemplatesByType.erase( get.first, get.second );
}

void ObjectTemplate::removeComponentTemplate( const String& rName )
{
    ComponentTemplatesByName::iterator i = mComponentTemplatesByName.find( rName );
    if( i != mComponentTemplatesByName.end() )
    {
        std::pair<ComponentTemplatesByType::iterator, ComponentTemplatesByType::iterator> get =
            mComponentTemplatesByType.equal_range( i->second->getType() );

        for( ComponentTemplatesByType::iterator j = get.first; j != get.second; ++j )
        {
            if( j->second == i->second )
            {
                mComponentTemplatesByType.erase( j );
                break;
            }
        }

        ComponentHandle handle( i->second->getType(), rName );
        mComponentTemplatesByHandle.erase( handle );
        mComponentTemplatesByName.erase( i );
    }
}

void ObjectTemplate::setParent( Node* pParent )
{
    Node* parent = Node::getParent();
    Node::setParent( pParent );

    if( parent != pParent )
    {
        mParentSignal( static_cast<ObjectTemplate*>( pParent ) );
        mParentChanged = true;
    }
}

void ObjectTemplate::objectTemplateChange( ObjectTemplate& rObjectTemplate, bool created )
{
    if( rObjectTemplate.getName() == mQueuedParentName )
    {
        if( created ) ObjectTemplate::parent( &rObjectTemplate );
        mQueuedParentName.clear();
        mQueuedParentConnection.disconnect();
    }
}

void ObjectTemplate::WriteAllocationID( RakNet::Connection_RM3* pConnection, 
    RakNet::BitStream* pAllocationIdBitstream ) const
{
    // Write object replica type and object name as identifier.
    pAllocationIdBitstream->Write<ReplicaType>( REPLICATYPE_OBJECTTEMPLATE );
    pAllocationIdBitstream->Write<RakNet::RakString>( RakNet::RakString( mName.c_str() ) );
    pAllocationIdBitstream->Write<RakNet::RakString>( RakNet::RakString( mDisplayName.c_str() ) );
}

void ObjectTemplate::DeallocReplica( RakNet::Connection_RM3* pSourceConnection )
{
    if( !mBroadcastingDestruction )
    {
        // Object is already destroyed so no permission checking is needed, act as server.
        try
        {
            //mDestructionSignal( *this );
            mObjectTemplateManager.destroyObjectTemplate( mName, mServerGUID );
        }
        catch ( Exception e )
        {
            // TODO: Is this bad? Just ignoring the error for now..
            OLOGW << "Failed to deallocate object template from replica system: " << e.what();
            //DivAssert( 0, e.what() );
        }
    }
}

//------------------------------------------------------------------------------
} // Namespace ObjectSystem
} // Namespace Diversia