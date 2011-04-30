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

#include "Object/Component.h"
#include "Object/ComponentFactory.h"
#include "Object/ComponentFactoryManager.h"
#include "Object/ComponentTemplate.h"

namespace Diversia
{
namespace ObjectSystem
{
//------------------------------------------------------------------------------

ComponentTemplate::ComponentTemplate( const String& rName, Mode mode, NetworkingType networkingType,
    ComponentType type, RakNet::RakNetGUID source, bool localOverride, 
    ObjectTemplate& rObjectTemplate ):
    mName( rName ),
    mMode( mode ),
    mNetworkingType( networkingType ),
    mType( type ),
    mSource( source == rObjectTemplate.getServerGUID() ? SERVER : CLIENT ),
    mSourceGUID( source ),
    mLocalOverride( localOverride ),
    mBroadcastingDestruction( false ),
    mObjectTemplate( rObjectTemplate ),
    mFactory( ComponentFactoryManager::getComponentFactory( type ) ),
    mComponentClass( camp::classByName( mFactory.getTypeName() ) )
{
    // Override networking type to local.
    if( mLocalOverride || ( mMode == CLIENT && mFactory.clientOnly() ) ||
        ( mMode == SERVER && mFactory.serverOnly() ) )
    {
        mNetworkingType = LOCAL;
        mLocalOverride = true;
    }

    this->SetNetworkIDManager( &mObjectTemplate.getNetworkIDManager() );

    if( ( ( mMode == SERVER && mNetworkingType == REMOTE ) || ( mMode == CLIENT &&
        mNetworkingType == REMOTE && mSource == CLIENT ) ) && !mLocalOverride )
    {
        broadcastConstruction();
    }
}

ComponentTemplate::~ComponentTemplate()
{
    for( InstantiatedComponents::iterator i = mInstantiatedComponents.begin(); 
        i != mInstantiatedComponents.end(); ++i )
    {
        (*i)->setTemplate( 0 );
    }

    mDestructionSignal( *this );
}

String ComponentTemplate::getTypeName() const
{
    return mFactory.getTypeName();
}

void ComponentTemplate::setLocalOverride( bool localOverride )
{
    if( mLocalOverride != localOverride )
    {
        if( mMode == CLIENT && mFactory.clientOnly() && !localOverride )
            DIVERSIA_EXCEPT( Exception::ERR_INVALID_STATE,
                "This component can only exist on the client, cannot set local override to false.",
                "ComponentTemplate::setLocalOverride" );
        if( mMode == SERVER && mFactory.serverOnly() && !localOverride )
            DIVERSIA_EXCEPT( Exception::ERR_INVALID_STATE,
                "This component can only exist on the server, cannot set local override to false.",
                "ComponentTemplate::setLocalOverride" );

        // Prevent changing local override on components the client doesn't own.
        if( !ComponentTemplate::isCreatedByOwnGUID() )
            DIVERSIA_EXCEPT( Exception::ERR_PERMISSION_DENIED,
                "Can only set owned components to be local overridden",
                "ComponentTemplate::setLocalOverride" );

        mLocalOverride = localOverride;

        if( mLocalOverride && mNetworkingType == REMOTE )
            ComponentTemplate::setNetworkingType( LOCAL );
    }
}

void ComponentTemplate::setNetworkingType( NetworkingType type )
{
    if( mNetworkingType != type )
    {
        // Do not allow local overridden components to become remote.
        if( mLocalOverride && type == REMOTE ) return;

        mNetworkingType = type;

        setNetworkingTypeImpl( mNetworkingType );

        if( ( ( mMode == SERVER && mNetworkingType == REMOTE ) ||
            ( mMode == CLIENT && mNetworkingType == REMOTE && mSource == CLIENT ) ) )
        {
            ComponentTemplate::broadcastConstruction();
        }
        else if( mNetworkingType == LOCAL )
        {
            ComponentTemplate::broadcastDestruction();
        }
    }
}

void ComponentTemplate::destroyComponentTemplateLocally()
{
    try
    {
        // Force networking type to local so that destruction won't be broadcasted since it's a
        // local destruction. This is only done for the creator of the object because the creator
        // needs to change the permission counters.
        if( ComponentTemplate::isCreatedByOwnGUID() && mNetworkingType != LOCAL )
        {
            mNetworkingType = LOCAL;
            mLocalOverride = false;
            setNetworkingTypeImpl( mNetworkingType );
        }

        // Act as server to evade permission checks.
        mObjectTemplate.destroyComponentTemplate( getName(), ComponentTemplate::getServerGUID() );
    }
    catch ( Exception e )
    {
        DivAssert( 0, e.what() );
    }
}

void ComponentTemplate::setTemplateProperty( const String& rPropertyName, 
    const camp::Value& rValue )
{
    if( !mComponentClass.hasProperty( rPropertyName, true ) )
        DIVERSIA_EXCEPT( Exception::ERR_INVALIDPARAMS, 
        "This property does not exist for this component.", 
        "ComponentTemplate::setTemplateProperty" );

    // Copy the value for user types.
    camp::Value value;
    if( rValue.type() == camp::userType )
        value = rValue.to<camp::UserObject>().copy();
    else
        value = rValue;

    mProperties[rPropertyName] = value;
    mPropertySignal( rPropertyName, value );

    // Propagate property change to all instantiated components.
    for( InstantiatedComponents::iterator i = mInstantiatedComponents.begin(); 
        i != mInstantiatedComponents.end(); ++i )
    {
        (*i)->setProperty( rPropertyName, value );
    }
}

void ComponentTemplate::setTemplateProperties( const Component& rComponent )
{
    if( rComponent.getType() != mType ) 
        DIVERSIA_EXCEPT( Exception::ERR_INVALIDPARAMS, 
        "Cannot set properties from a component that is not of the same type.", 
    	"ComponentTemplate::setTemplateProperties" );

    camp::UserObject userObject = rComponent;
    const camp::Class& metaclass = userObject.getClass();
    std::size_t count = metaclass.propertyCount( true );
    for( std::size_t i = 0; i < count; ++i )
    {
        const camp::Property& prop = metaclass.property( i, true );
        ComponentTemplate::setTemplateProperty( prop.name(), prop.get( userObject ) );
    }
}

Component& ComponentTemplate::createComponent( Object& rObject )
{
    // Create unique component name.
    String componentName = mFactory.getTypeName();
    if( mFactory.multiple() )
    {
        std::size_t count = rObject.componentCount( mType );
        if( count )
        {
            componentName += boost::lexical_cast<String>( count + 1 );
        }
    }

    // Create component
    Component& component = rObject.createComponent( mType, componentName );
    camp::UserObject componentObject = component;

    // Set template properties on component.
    for( Properties::const_iterator i = mProperties.begin(); i != mProperties.end(); ++i )
    {
        try
        {
            componentObject.set( i->first, i->second );
        }
        catch( ... )
        {
        	// Ignore errors
        }
    }

    // Create component links
    mInstantiatedComponents.insert( &component );
    component.setTemplate( this );
    component.connectDestruction( sigc::mem_fun( this, &ComponentTemplate::componentDestroyed ) );

    return component;
}

void ComponentTemplate::addInstantiatedComponent( Component& rComponent )
{
    mInstantiatedComponents.insert( &rComponent );
    rComponent.connectDestruction( sigc::mem_fun( this, &ComponentTemplate::componentDestroyed ) );

    // TODO: Send over all properties?
}

void ComponentTemplate::broadcastConstruction()
{
    OLOGD << "Broadcasting construction for component template " << mName << " in object template " 
        << mObjectTemplate.getName();

	mObjectTemplate.getReplicaManager().Reference( this );
}

bool ComponentTemplate::queryBroadcastDestruction()
{
    return ( ( ( mMode == SERVER && mNetworkingType == REMOTE ) || ( mMode == CLIENT &&
        mNetworkingType == REMOTE && mSource == CLIENT ) ) && !mLocalOverride );
}

void ComponentTemplate::broadcastDestruction()
{
    OLOGD << "Broadcasting destruction for component template " << mName << " in object template " 
        << mObjectTemplate.getName();

    mBroadcastingDestruction = true;
    Replica3::BroadcastDestruction();
    mObjectTemplate.getReplicaManager().Dereference( this );
    mBroadcastingDestruction = false;
}

void ComponentTemplate::WriteAllocationID( RakNet::Connection_RM3* pConnection,
    RakNet::BitStream* pAllocationIdBitstream ) const
{
    // Write component replica type, component type and object network id.
    pAllocationIdBitstream->Write<ReplicaType>( REPLICATYPE_COMPONENTTEMPLATE );
    pAllocationIdBitstream->Write<ComponentType>( getType() );
    pAllocationIdBitstream->Write<RakNet::RakString>( RakNet::RakString( mName.c_str() ) );
    pAllocationIdBitstream->Write<RakNet::NetworkID>( mObjectTemplate.GetNetworkID() );
}

void ComponentTemplate::DeallocReplica( RakNet::Connection_RM3* pSourceConnection )
{
    // ComponentTemplate is already destroyed so no permission checking is needed, act as server.
    if( !mBroadcastingDestruction )
    {
        try
        {
            mObjectTemplate.destroyComponentTemplate( getName(), ComponentTemplate::getServerGUID() );
        }
        catch ( Exception e )
        {
            // TODO: Is this bad? Just ignoring the error for now..
            OLOGW << "Failed to deallocate component template from replica system: " << e.what();
            //DivAssert( 0, e.what() );
        }
    }
}

void ComponentTemplate::componentDestroyed( Component& rComponent )
{
    mInstantiatedComponents.erase( &rComponent );
}

//------------------------------------------------------------------------------
} // Namespace ObjectSystem
} // Namespace Diversia