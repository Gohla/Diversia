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

#include "OgreClient/Platform/StableHeaders.h"

#include "Client/Lua/LuaPlugin.h"
#include "Client/Object/ClientObject.h"
#include "OgreClient/Object/AreaTrigger.h"
#include "OgreClient/Object/CollisionShape.h"
#include "OgreClient/Object/LuaObjectScript.h"
#include "OgreClient/Resource/GenericResource.h"
#include "OgreClient/Resource/GenericResourceManager.h"
#include "OgreClient/Resource/ResourceManager.h"
#include "Shared/ClientServerPlugin/ClientServerPlugin.h"
#include "Shared/ClientServerPlugin/ClientServerPluginManager.h"
#include <camp-lua/valuetoluavisitor.hpp>
#include <sigc++/adaptors/lambda/lambda.h>

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

std::map<String, unsigned int> LuaObjectScript::mDefaultEnvironmentCounter = 
    std::map<String, unsigned int>();

LuaObjectScript::LuaObjectScript( const String& rName, Mode mode, NetworkingType networkingType, 
    RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject ):
    ClientComponent( rName, mode, networkingType, LuaObjectScript::getTypeStatic(), source, 
        localOverride, rObject ),
    mClientSecurityLevel( LUASEC_HIGH ),
    mServerSecurityLevel( LUASEC_HIGH ),
    mLoaded( false ),
    mCreated( false ),
    mResourceManager( ClientComponent::getClientObject().getClientObjectManager().
        getPluginManager().getPlugin<ResourceManager>() ),
    mLuaManager( ClientComponent::getClientObject().getClientObjectManager().
        getPluginManager().getPlugin<LuaPlugin>().get() )
{
    PropertySynchronization::storeUserObject();
}

LuaObjectScript::~LuaObjectScript()
{
    LuaObjectScript::destroy();
}

void LuaObjectScript::setClientScriptFile( const Path& rClientScriptFile )
{
    if( rClientScriptFile.empty() || mClientScriptFile == rClientScriptFile ) return;
    mClientScriptFile = rClientScriptFile;

    if( mCreated )
    {
        // TODO: Store and load properties when reloading the audio file so all the properties from
        // previous audio component also get set onto the new audio component.

        // Destroy existing script
        LuaObjectScript::destroy();

        // Load script resource
        try
        {
            mResourceManager.loadResource( ResourceInfo( mClientScriptFile, "Generic" ), 
                sigc::mem_fun( this, &LuaObjectScript::resourceLoaded ) );
        }
        catch( FileNotFoundException e )
        {
            CLOGE << "Could not load resource for lua script component: " << e.what();
        }
    }
}

void LuaObjectScript::create()
{ 
    if( !mCreated )
    {
        if( mClientEnvironmentName.empty() )
        {
            mClientEnvironmentName = "LuaObjectScript";
        }

        // Get a unique environment name.
        if( !mDefaultEnvironmentCounter.count( mClientEnvironmentName ) )
        {
            mDefaultEnvironmentCounter[ mClientEnvironmentName ] = 0;
        }
        else
        {
            mClientEnvironmentName = mClientEnvironmentName + boost::lexical_cast<String>( 
                mDefaultEnvironmentCounter[ mClientEnvironmentName ]++ );
        }

        mCreated = true;
        if( mClientScriptFile.empty() ) return;

        // Load script resource
        try
        {
            mResourceManager.loadResource( ResourceInfo( mClientScriptFile, "Generic" ), 
                sigc::mem_fun( this, &LuaObjectScript::resourceLoaded ) );
        }
        catch( FileNotFoundException e )
        {
            CLOGE << "Could not load resource for lua script component: " << e.what();
        }
    }
    else if( !mLoaded )
    {
        // Load script resource
        try
        {
            mResourceManager.loadResource( ResourceInfo( mClientScriptFile, "Generic" ), 
                sigc::mem_fun( this, &LuaObjectScript::resourceLoaded ) );
        }
        catch( FileNotFoundException e )
        {
            CLOGE << "Could not load resource for lua script component: " << e.what();
        }
    }
}

void LuaObjectScript::destroy()
{
    if( mCreated && mLoaded )
    {
        LuaObjectScript::disconnectAll();
        mConnections.clear();

        // Call Destroy function if the lua script has one.
        if( mLuaManager.functionExists( "Destroy", mClientEnvironmentName, "Global" ) )
        {
            mLuaManager.call( "Destroy", mClientEnvironmentName, "Global" );
        }

        // Unload script resource
        GenericResourceManager::getSingletonPtr()->unload( mClientScriptFile.string() );

        mLoaded = false;
    }
}

void LuaObjectScript::reload()
{
    LuaObjectScript::destroy();

    // Create in next tick.
    DelayedCall::create( sigc::mem_fun( this, &LuaObjectScript::create ), 0 );
}

void LuaObjectScript::resourceLoaded( Ogre::ResourcePtr pResource )
{
    GenericResourcePtr resource = pResource;
    String lua = resource->getCache()->getAsString();

    // Create environment before executing file so this component's object can be set in the
    // environment before executing.
    mLuaManager.createEnvironment( mClientEnvironmentName, "Global", mClientSecurityLevel );
    mLuaManager.set( ClientComponent::getClientObject(), "Object", mClientEnvironmentName, 
        "Global" );
    mLuaManager.set( this, "Script", mClientEnvironmentName, "Global" );

    // Load script
    mLuaManager.execute( lua, mClientEnvironmentName, "Global", mClientSecurityLevel );
    mLoaded = true;

    // Connect signals to lua functions.
    LuaObjectScript::connectAll();

    // Call Create function if the lua script has one.
    if( mLuaManager.functionExists( "Create", mClientEnvironmentName, "Global" ) )
    {
        mLuaManager.call( "Create", mClientEnvironmentName, "Global" );
    }
}

String LuaObjectScript::getEventName( LuaObjectScriptEvent event )
{
    try
    {
        return camp::enumByType<LuaObjectScriptEvent>().name( event );
    }
    catch( camp::Error e )
    {
        CLOGW << "Could not get name for event " << event;
    }

    return String();
}

void LuaObjectScript::disconnect( LuaObjectScriptEvent event )
{
    ConnectionMap::iterator i = mConnections.find( event );
    if( i != mConnections.end() )
        i->second.disconnect();
}

bool LuaObjectScript::isDisconnected( LuaObjectScriptEvent event ) const
{
    return !LuaObjectScript::isConnected( event );
}

template <LuaObjectScriptEvent T> void LuaObjectScript::connectTemplate()
{
    try
    {
        if( !mLuaManager.functionExists( LuaObjectScript::getEventName( T ), mClientEnvironmentName, 
            "Global" ) || LuaObjectScript::isConnected( T ) ) return;

        mConnections[ T ] = connectImpl<T>();
    }
    catch( Exception e )
    {
        CLOGW << "Could not connect to event " << T << ":" << e.what();
    }
}

template <> inline sigc::connection LuaObjectScript::connectImpl<LUAOBJECTSCRIPTEVENT_UPDATE>()
{
    return GlobalsBase::mUpdateSignal->connect( sigc::bind( sigc::mem_fun( mLuaManager, 
        &LuaManager::call ), "Update", sigc::ref( mClientEnvironmentName ), "Global" ) );
}

template <> inline sigc::connection LuaObjectScript::connectImpl<LUAOBJECTSCRIPTEVENT_FRAME>()
{
    return GlobalsBase::mFrameSignal->connect( sigc::group( sigc::mem_fun( mLuaManager, 
        &LuaManager::call<Real> ), "Frame", sigc::ref( mClientEnvironmentName ), "Global", 
        sigc::_1 ) );
}

template <> inline sigc::connection LuaObjectScript::connectImpl<LUAOBJECTSCRIPTEVENT_COMPONENTCHANGE>()
{
    Object& object = Component::getObject();
    return object.connectComponentChange( sigc::group( sigc::mem_fun( mLuaManager, 
        &LuaManager::call<Component&, bool> ), "ComponentChange", 
        sigc::ref( mClientEnvironmentName ), "Global", sigc::_1, sigc::_2 ) );
}

template <> inline sigc::connection LuaObjectScript::connectImpl<LUAOBJECTSCRIPTEVENT_TRANSFORMCHANGE>()
{
    Object& object = Component::getObject();
    return object.connectTransformChange( sigc::group( sigc::mem_fun( mLuaManager, 
        &LuaManager::call<const Node&> ), "TransformChange", 
        sigc::ref( mClientEnvironmentName ), "Global", sigc::_1 ) );
}

template <> inline sigc::connection LuaObjectScript::connectImpl<LUAOBJECTSCRIPTEVENT_AREACHANGE>()
{
    AreaTrigger& areaTrigger = Component::getObject().getComponent<AreaTrigger>();
    return areaTrigger.connectAreaChange( sigc::group( sigc::mem_fun( mLuaManager, 
        &LuaManager::call<Object&, bool> ), "AreaChange", sigc::ref( mClientEnvironmentName ), 
        "Global", sigc::_1, sigc::_2 ) );
}

template <> inline sigc::connection LuaObjectScript::connectImpl<LUAOBJECTSCRIPTEVENT_COLLISIONWITH>()
{
    CollisionShape& collisionShape = Component::getObject().getComponent<CollisionShape>();
    return collisionShape.connectCollision( sigc::group( sigc::mem_fun( mLuaManager, 
        &LuaManager::call<CollisionShape&> ), "CollisionWith", 
        sigc::ref( mClientEnvironmentName ), "Global", sigc::_1 ) );
}

void LuaObjectScript::connect( LuaObjectScriptEvent event )
{
    switch( event )
    {
        case LUAOBJECTSCRIPTEVENT_UPDATE: 
            LuaObjectScript::connectTemplate<LUAOBJECTSCRIPTEVENT_UPDATE>(); return;
        case LUAOBJECTSCRIPTEVENT_FRAME: 
            LuaObjectScript::connectTemplate<LUAOBJECTSCRIPTEVENT_FRAME>(); return;
        case LUAOBJECTSCRIPTEVENT_COMPONENTCHANGE:
            LuaObjectScript::connectTemplate<LUAOBJECTSCRIPTEVENT_COMPONENTCHANGE>(); return;
        case LUAOBJECTSCRIPTEVENT_TRANSFORMCHANGE: 
            LuaObjectScript::connectTemplate<LUAOBJECTSCRIPTEVENT_TRANSFORMCHANGE>(); return;
        case LUAOBJECTSCRIPTEVENT_AREACHANGE: 
            LuaObjectScript::connectTemplate<LUAOBJECTSCRIPTEVENT_AREACHANGE>(); return;
        case LUAOBJECTSCRIPTEVENT_COLLISIONWITH: 
            LuaObjectScript::connectTemplate<LUAOBJECTSCRIPTEVENT_COLLISIONWITH>(); return;
    }
}

bool LuaObjectScript::isConnected( LuaObjectScriptEvent event ) const
{
    ConnectionMap::const_iterator i = mConnections.find( event );
    return i != mConnections.end() && i->second.connected();
}

bool LuaObjectScript::hasBeenConnected( LuaObjectScriptEvent event ) const
{
    return mConnections.find( event ) != mConnections.end();
}

void LuaObjectScript::block( LuaObjectScriptEvent event, bool blocked )
{
    ConnectionMap::iterator i = mConnections.find( event );
    if( i != mConnections.end() )
        i->second.block( blocked );
}

bool LuaObjectScript::isBlocked( LuaObjectScriptEvent event ) const
{
    ConnectionMap::const_iterator i = mConnections.find( event );
    return i != mConnections.end() && i->second.blocked();
}

void LuaObjectScript::disconnectAll()
{
    for( ConnectionMap::iterator i = mConnections.begin(); i != mConnections.end(); ++i )
        i->second.disconnect();
}

void LuaObjectScript::connectAll()
{
    LuaObjectScript::connectTemplate<LUAOBJECTSCRIPTEVENT_UPDATE>();
    LuaObjectScript::connectTemplate<LUAOBJECTSCRIPTEVENT_FRAME>();
    LuaObjectScript::connectTemplate<LUAOBJECTSCRIPTEVENT_COMPONENTCHANGE>();
    LuaObjectScript::connectTemplate<LUAOBJECTSCRIPTEVENT_TRANSFORMCHANGE>();
    LuaObjectScript::connectTemplate<LUAOBJECTSCRIPTEVENT_AREACHANGE>();
    LuaObjectScript::connectTemplate<LUAOBJECTSCRIPTEVENT_COLLISIONWITH>();
}

void LuaObjectScript::blockAll( bool blocked )
{
    for( ConnectionMap::iterator i = mConnections.begin(); i != mConnections.end(); ++i )
        i->second.block( blocked );
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia