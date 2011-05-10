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
#include "Util/Signal/UserObjectChange.h"

namespace Diversia
{
namespace ObjectSystem
{
//------------------------------------------------------------------------------

Component::Component( const String& rName, Mode mode, NetworkingType networkingType,
    ComponentType type, RakNet::RakNetGUID source, bool localOverride, Object& rObject ):
    mName( rName ),
    mMode( mode ),
    mNetworkingType( networkingType ),
    mType( type ),
    mSource( source == rObject.getServerGUID() ? SERVER : CLIENT ),
    mSourceGUID( source ),
    mLocalOverride( localOverride ),
    mBroadcastingDestruction( false ),
    mObject( rObject ),
    mFactory( ComponentFactoryManager::getComponentFactory( type ) ),
    mTemplate( 0 )
{
    // Override networking type to local.
    if( mLocalOverride || ( mMode == CLIENT && mFactory.clientOnly() ) ||
        ( mMode == SERVER && mFactory.serverOnly() ) )
    {
        mNetworkingType = LOCAL;
        mLocalOverride = true;
    }

    this->SetNetworkIDManager( &mObject.getNetworkIDManager() );

    if( ( ( mMode == SERVER && mNetworkingType == REMOTE ) || ( mMode == CLIENT &&
        mNetworkingType == REMOTE && mSource == CLIENT ) ) && !mLocalOverride )
    {
        broadcastConstruction();
    }
}

Component::~Component()
{
    mPropertyConnection.disconnect();
    mDestructionSignal( *this );
}

void Component::setLocalOverride( bool localOverride )
{
    if( mLocalOverride != localOverride )
    {
        if( mMode == CLIENT && mFactory.clientOnly() && !localOverride )
            DIVERSIA_EXCEPT( Exception::ERR_INVALID_STATE,
                "This component can only exist on the client, cannot set local override to false.",
                "Component::setLocalOverride" );
        if( mMode == SERVER && mFactory.serverOnly() && !localOverride )
            DIVERSIA_EXCEPT( Exception::ERR_INVALID_STATE,
                "This component can only exist on the server, cannot set local override to false.",
                "Component::setLocalOverride" );

        // Prevent changing local override on components the client doesn't own.
        if( !Component::isCreatedByOwnGUID() )
            DIVERSIA_EXCEPT( Exception::ERR_PERMISSION_DENIED,
                "Can only set owned components to be local overridden",
                "Component::setLocalOverride" );

        mLocalOverride = localOverride;

        if( mLocalOverride && mNetworkingType == REMOTE )
            Component::setNetworkingType( LOCAL );
    }
}

void Component::setNetworkingType( NetworkingType type )
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
            Component::broadcastConstruction();
        }
        else if( mNetworkingType == LOCAL )
        {
            Component::broadcastDestruction();
        }
    }
}

Component& Component::duplicate( Object& rObject )
{
    Component& component = rObject.createComponent( mType, mName, mLocalOverride );
    component.setOverriddenProperties( mOverriddenProperties );
    if( mTemplate ) 
    {
        component.setTemplate( mTemplate );
        mTemplate->addInstantiatedComponent( component );
    }

    camp::UserObject userObject = this;
    const camp::Class& metaclass = userObject.getClass();
    std::size_t count = metaclass.propertyCount( true );
    for( std::size_t i = 0; i < count; ++i )
    {
        try
        {
            const camp::Property& prop = metaclass.property( i, true );
            prop.set( component, prop.get( userObject ) );
        }
        catch( ... )
        {
            // Ignore errors (camp cannot write error can be ignored as it will delay setting
            // the property until the component is loaded).
        }
    }

    return component;
}

void Component::destroyComponentLocally()
{
    try
    {
        // Force networking type to local so that destruction won't be broadcasted since it's a
        // local destruction. This is only done for the creator of the object because the creator
        // needs to change the permission counters.
        if( Component::isCreatedByOwnGUID() && mNetworkingType != LOCAL )
        {
            mNetworkingType = LOCAL;
            mLocalOverride = false;
            setNetworkingTypeImpl( mNetworkingType );
        }

        // Act as server to evade permission checks.
        mObject.destroyComponent( getName(), Component::getServerGUID() );
    }
    catch ( Exception e )
    {
        DivAssert( 0, e.what() );
    }
}

void Component::setTemplate( ComponentTemplate* pTemplate )
{
    if( pTemplate )
    {
        mTemplate = pTemplate;
        mPropertyConnection = UserObjectChange::connectChange( this, sigc::mem_fun( this, 
            &Component::propertyChange ) );
    }
    else
    {
        mTemplate = 0;
        mPropertyConnection.disconnect();
        mOverriddenProperties.clear();
    }
}

void Component::setTemplate( const String& rName )
{
    if( rName.empty() )
        Component::setTemplate( 0 );
    else if( mObject.getTemplate() )
    {
        Component::setTemplate( &mObject.getTemplate()->getComponentTemplate( rName ) );
        mTemplate->addInstantiatedComponent( *this );
    }
    else
        OLOGE << "Component template " << rName << " does not exist, cannot set template for component " << mName;
}

String Component::getTemplateName() const
{
    if( mTemplate ) return mTemplate->getName();
    return String();
}

void Component::setProperty( const String& rPropertyName, const camp::Value& rValue )
{
    camp::UserObject componentObject = this;
    if( mOverriddenProperties.find( rPropertyName ) == mOverriddenProperties.end() )
    {
        componentObject.set( rPropertyName, rValue );
    }
}

void Component::broadcastConstruction()
{
    OLOGD << "Broadcasting construction for component " << mName << " in object " <<
        mObject.getName();

	mObject.getReplicaManager().Reference( this );
}

bool Component::queryBroadcastDestruction()
{
    return ( ( ( mMode == SERVER && mNetworkingType == REMOTE ) || ( mMode == CLIENT &&
        mNetworkingType == REMOTE && mSource == CLIENT ) ) && !mLocalOverride );
}

void Component::broadcastDestruction()
{
    OLOGD << "Broadcasting destruction for component " << mName << " in object " <<
        mObject.getName();

    mBroadcastingDestruction = true;
    Replica3::BroadcastDestruction();
    mObject.getReplicaManager().Dereference( this );
    mBroadcastingDestruction = false;
}

void Component::propertyChange( const camp::UserObject& rObject, const camp::Property& rProperty, 
    const camp::Value& rValue, const int reason )
{
    if( reason != 0 ) mOverriddenProperties.insert( rProperty.name() );
}

void Component::WriteAllocationID( RakNet::Connection_RM3* pConnection,
    RakNet::BitStream* pAllocationIdBitstream ) const
{
    // Write component replica type, component type and object network id.
    pAllocationIdBitstream->Write<ReplicaType>( REPLICATYPE_COMPONENT );
    pAllocationIdBitstream->Write<ComponentType>( getType() );
    pAllocationIdBitstream->Write<RakNet::RakString>( RakNet::RakString( mName.c_str() ) );
    pAllocationIdBitstream->Write<RakNet::NetworkID>( mObject.GetNetworkID() );
}

void Component::DeallocReplica( RakNet::Connection_RM3* pSourceConnection )
{
    // Component is already destroyed so no permission checking is needed, act as server.
    if( !mBroadcastingDestruction )
    {
        try
        {
            mObject.destroyComponent( getName(), Component::getServerGUID() );
        }
        catch ( Exception e )
        {
            // TODO: Is this bad? Just ignoring the error for now..
            OLOGW << "Failed to deallocate component from replica system: " << e.what();
            //DivAssert( 0, e.what() );
        }
    }
}

//------------------------------------------------------------------------------
} // Namespace Object
} // Namespace Diversia