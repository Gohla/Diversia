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
#include "Client/Object/ClientObjectManager.h"
#include "Object/Object.h"
#include "OgreClient/GameMode/GameModeBase.h"
#include "OgreClient/GameMode/GameModePlugin.h"
#include "OgreClient/Resource/GenericResource.h"
#include "OgreClient/Resource/GenericResourceManager.h"
#include "OgreClient/Resource/ResourceManager.h"
#include "Shared/Lua/LuaManager.h"
#include <camp-lua/valuetoluavisitor.hpp>
#include <sigc++/adaptors/lambda/lambda.h>

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

sigc::slot<GameModeBase*, GameModePlugin*> GameModePlugin::mDefaultSlot = 
    sigc::slot<GameModeBase*, GameModePlugin*>();
bool GameModePlugin::mForceDefault = false;

GameModePlugin::GameModePlugin( Mode mode, PluginState state, ClientPluginManager& rPluginManager, 
    RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
    RakNet::NetworkIDManager& rNetworkIDManager ):
    ClientPlugin( mode, state, rPluginManager, rRakPeer, rReplicaManager, rNetworkIDManager ),
    mGameMode( 0 ),
    mMousePriority( 0 ),
    mKeyboardPriority( 0 ),
    mMouseSubscribed( false ),
    mKeyboardSubscribed( false ),
    mCreated( false ),
    mResourceManager( Plugin::getPluginManager().getPlugin<ResourceManager>() ),
    mLuaManager( Plugin::getPluginManager().getPlugin<LuaPlugin>().get() )
{
    PropertySynchronization::storeUserObject();
}

GameModePlugin::~GameModePlugin()
{
    GameModePlugin::destroy();
}

void GameModePlugin::create()
{
    if( !mCreated )
    {
        if( mClientScriptFiles.empty() || mClientLoadDefaultGamemode || 
            Plugin::getState() == STOP )
        {
            // Create default gamemode
            if( !mDefaultSlot.empty() ) mGameMode = mDefaultSlot( this );

            if( mClientScriptFiles.empty() || Plugin::getState() == STOP )
                ClientPlugin::mLoadingCompletedSignal( *this );
        }

        if( !mClientScriptFiles.empty() && Plugin::getState() == PLAY )
        {
            // Load lua gamemode(s)
            ResourceList resourceList;
            for( std::vector<Path>::iterator i = mClientScriptFiles.begin(); 
                i != mClientScriptFiles.end(); ++i )
            {
                resourceList.push_back( ResourceInfo( *i, "Generic" ) );
            }

            try
            {
                mResourceManager.loadResources( resourceList, sigc::mem_fun( this, 
                    &GameModePlugin::resourcesLoaded ) );
            }
            catch( FileNotFoundException e )
            {
                CLOGE << e.what();
                ClientPlugin::mLoadingCompletedSignal( *this );
            }
        }

        mCreated = true;
    }
}

void GameModePlugin::destroy()
{
    if( mCreated )
    {
        if( !mClientScriptFiles.empty() )
        {
            // Call Destroy function if the lua script has one.
            if( mLuaManager.functionExists( "Destroy", "", "Global" ) )
            {
                mLuaManager.call( "Destroy", "", "Global" );
            }
        }

        GameModePlugin::disconnectAll();
        GameModePlugin::unsubscribeKeyboard();
        GameModePlugin::unsubscribeMouse();

        if( mGameMode ) 
        {
            delete mGameMode;
            mGameMode = 0;
        }

        // Unload script resources
        for( std::vector<Path>::iterator i = mClientScriptFiles.begin(); 
            i != mClientScriptFiles.end(); ++i )
        {
            GenericResourceManager::getSingletonPtr()->unload( (*i).string() );
        }

        mCreated = false;
    }
}

void GameModePlugin::reload()
{
    GameModePlugin::destroy();

    // Create in next tick.
    DelayedCall::create( sigc::mem_fun( this, &GameModePlugin::create ), 0 );
}

void GameModePlugin::setServerState( ServerState serverState )
{
    if( mGameMode ) mGameMode->setServerState( serverState );
}

void GameModePlugin::stateChanged( PluginState state, PluginState prevState )
{
    switch( state )
    {
        case STOP: GameModePlugin::reload(); break;
        case PLAY: if( prevState == STOP ) GameModePlugin::reload(); break;
    }
}

void GameModePlugin::resourcesLoaded()
{
    // Load scripts
    for( std::vector<Path>::iterator i = mClientScriptFiles.begin(); i != mClientScriptFiles.end(); 
        ++i )
    {
        GenericResourcePtr resource = GenericResourceManager::getSingleton().getByName( 
            (*i).string(), mResourceManager.getGroup() );
        String lua = resource->getCache()->getAsString();
        mLuaManager.set( this, "Script", "", "Global" );
        mLuaManager.execute( lua, "", "Global", mClientSecurityLevel );
    }

    // Connect signals to lua functions.
    GameModePlugin::connectAll();

    // Call Create function if the lua script has one.
    if( mLuaManager.functionExists( "Create", "", "Global" ) )
    {
        mLuaManager.call( "Create", "", "Global" );
    }

    ClientPlugin::mLoadingCompletedSignal( *this );
}

bool GameModePlugin::mousePressed( const MouseButton button )
{
    mMousePressedSignal( button );

    // TODO: Get return value from lua
    return true;
}

void GameModePlugin::mouseReleased( const MouseButton button )
{
    mMouseReleasedSignal( button );
}

bool GameModePlugin::mouseMoved( const MouseState& rState )
{
    mMouseMovedSignal( rState );

    // TODO: Get return value from lua
    return true;
}

void GameModePlugin::subscribeMouse()
{
    if( !mMouseSubscribed )
    {
        GlobalsBase::mInput->subscribeMouse( *this );
        mMouseSubscribed = true;
    }
}

void GameModePlugin::unsubscribeMouse()
{
    if( mMouseSubscribed )
    {
        GlobalsBase::mInput->unsubscribeMouse( *this );
        mMouseSubscribed = false;
    }
}

bool GameModePlugin::keyPressed( const KeyboardButton button, unsigned int text )
{
    mKeyPressedSignal( button, text );

    // TODO: Get return value from lua
    return true;
}

void GameModePlugin::keyReleased( const KeyboardButton button, unsigned int text )
{
    mKeyReleasedSignal( button, text);
}

void GameModePlugin::subscribeKeyboard()
{
    if( !mKeyboardSubscribed )
    {
        GlobalsBase::mInput->subscribeKeyboard( *this );
        mKeyboardSubscribed = true;
    }
}

void GameModePlugin::unsubscribeKeyboard()
{
    if( mKeyboardSubscribed )
    {
        GlobalsBase::mInput->unsubscribeKeyboard( *this );
        mKeyboardSubscribed = false;
    }
}

String GameModePlugin::getEventName( LuaGameModeScriptEvent event )
{
    try
    {
        return camp::enumByType<LuaGameModeScriptEvent>().name( event );
    }
    catch( camp::Error e )
    {
        CLOGW << "Could not get name for event " << event;
    }

    return String();
}

void GameModePlugin::disconnect( LuaGameModeScriptEvent event )
{
    ConnectionMap::iterator i = mConnections.find( event );
    if( i != mConnections.end() )
        i->second.disconnect();
}

bool GameModePlugin::isDisconnected( LuaGameModeScriptEvent event ) const
{
    return !GameModePlugin::isConnected( event );
}

template <LuaGameModeScriptEvent T> void GameModePlugin::connectTemplate()
{
    try
    {
        if( !mLuaManager.functionExists( GameModePlugin::getEventName( T ), "", "Global" ) || 
            GameModePlugin::isConnected( T ) ) return;

        mConnections[ T ] = connectImpl<T>();
    }
    catch( Exception e )
    {
    	CLOGW << "Could not connect to event " << T << ":" << e.what();
    }
}

template <> inline sigc::connection GameModePlugin::connectImpl<LUAGAMEMODESCRIPTEVENT_EARLYUPDATE>()
{
    return GlobalsBase::mEarlyUpdateSignal->connect( sigc::bind( sigc::mem_fun( mLuaManager, 
        &LuaManager::call ), "EarlyUpdate", "", "Global" ) );
}

template <> inline sigc::connection GameModePlugin::connectImpl<LUAGAMEMODESCRIPTEVENT_UPDATE>()
{
    return GlobalsBase::mUpdateSignal->connect( sigc::bind( sigc::mem_fun( mLuaManager, 
        &LuaManager::call ), "Update", "", "Global" ) );
}

template <> inline sigc::connection GameModePlugin::connectImpl<LUAGAMEMODESCRIPTEVENT_LATEUPDATE>()
{
    return GlobalsBase::mLateUpdateSignal->connect( sigc::bind( sigc::mem_fun( mLuaManager, 
        &LuaManager::call ), "LateUpdate", "", "Global" ) );
}

template <> inline sigc::connection GameModePlugin::connectImpl<LUAGAMEMODESCRIPTEVENT_EARLYFRAME>()
{
    return GlobalsBase::mEarlyFrameSignal->connect( sigc::group( sigc::mem_fun( mLuaManager, 
        &LuaManager::call<Real> ), "EarlyFrame", "", "Global", sigc::_1 ) );
}

template <> inline sigc::connection GameModePlugin::connectImpl<LUAGAMEMODESCRIPTEVENT_FRAME>()
{
    return GlobalsBase::mFrameSignal->connect( sigc::group( sigc::mem_fun( mLuaManager, 
        &LuaManager::call<Real> ), "Frame", "", "Global", sigc::_1 ) );
}

template <> inline sigc::connection GameModePlugin::connectImpl<LUAGAMEMODESCRIPTEVENT_LATEFRAME>()
{
    return GlobalsBase::mLateFrameSignal->connect( sigc::group( sigc::mem_fun( mLuaManager, 
        &LuaManager::call<Real> ), "LateFrame", "", "Global", sigc::_1 ) );
}

template <> inline sigc::connection GameModePlugin::connectImpl<LUAGAMEMODESCRIPTEVENT_OBJECTCHANGE>()
{
    ObjectManager& objectManager = ClientPlugin::getPluginManager().getPlugin<ClientObjectManager>();
    return objectManager.connect( sigc::group( sigc::mem_fun( mLuaManager, 
        &LuaManager::call<Object&, bool> ), "ObjectChange", "", "Global", sigc::_1, sigc::_2 ) );
}

template <> inline sigc::connection GameModePlugin::connectImpl<LUAGAMEMODESCRIPTEVENT_MOUSEPRESSED>()
{
    GameModePlugin::subscribeMouse();
    return mMousePressedSignal.connect( sigc::group( sigc::mem_fun( mLuaManager, 
        &LuaManager::call<const MouseButton> ), "MousePressed", "", "Global", sigc::_1 ) );
}

template <> inline sigc::connection GameModePlugin::connectImpl<LUAGAMEMODESCRIPTEVENT_MOUSERELEASED>()
{
    GameModePlugin::subscribeMouse();
    return mMouseReleasedSignal.connect( sigc::group( sigc::mem_fun( mLuaManager, 
        &LuaManager::call<const MouseButton> ), "MouseReleased", "", "Global", sigc::_1 ) );
}

template <> inline sigc::connection GameModePlugin::connectImpl<LUAGAMEMODESCRIPTEVENT_MOUSEMOVED>()
{
    GameModePlugin::subscribeMouse();
    return mMouseMovedSignal.connect( sigc::group( sigc::mem_fun( mLuaManager, 
        &LuaManager::call<const MouseState&> ), "MouseMoved", "", "Global", sigc::_1 ) );
}

template <> inline sigc::connection GameModePlugin::connectImpl<LUAGAMEMODESCRIPTEVENT_KEYPRESSED>()
{
    GameModePlugin::subscribeKeyboard();
    return mKeyPressedSignal.connect( sigc::group( sigc::mem_fun( mLuaManager, 
        &LuaManager::call<KeyboardButton, unsigned int> ), "KeyPressed", "", "Global", sigc::_1,
        sigc::_2 ) );
}

template <> inline sigc::connection GameModePlugin::connectImpl<LUAGAMEMODESCRIPTEVENT_KEYRELEASED>()
{
    GameModePlugin::subscribeKeyboard();
    return mKeyReleasedSignal.connect( sigc::group( sigc::mem_fun( mLuaManager, 
        &LuaManager::call<KeyboardButton, unsigned int> ), "KeyReleased", "", "Global", sigc::_1,
        sigc::_2 ) );
}

void GameModePlugin::connect( LuaGameModeScriptEvent event )
{
    switch( event )
    {
        case LUAGAMEMODESCRIPTEVENT_EARLYUPDATE: 
            GameModePlugin::connectTemplate<LUAGAMEMODESCRIPTEVENT_EARLYUPDATE>(); return;
        case LUAGAMEMODESCRIPTEVENT_UPDATE: 
            GameModePlugin::connectTemplate<LUAGAMEMODESCRIPTEVENT_UPDATE>(); return;
        case LUAGAMEMODESCRIPTEVENT_LATEUPDATE: 
            GameModePlugin::connectTemplate<LUAGAMEMODESCRIPTEVENT_LATEUPDATE>(); return;
        case LUAGAMEMODESCRIPTEVENT_EARLYFRAME: 
            GameModePlugin::connectTemplate<LUAGAMEMODESCRIPTEVENT_EARLYFRAME>(); return;
        case LUAGAMEMODESCRIPTEVENT_FRAME: 
            GameModePlugin::connectTemplate<LUAGAMEMODESCRIPTEVENT_FRAME>(); return;
        case LUAGAMEMODESCRIPTEVENT_LATEFRAME: 
            GameModePlugin::connectTemplate<LUAGAMEMODESCRIPTEVENT_LATEFRAME>(); return;
        case LUAGAMEMODESCRIPTEVENT_OBJECTCHANGE: 
            GameModePlugin::connectTemplate<LUAGAMEMODESCRIPTEVENT_OBJECTCHANGE>(); return;
        case LUAGAMEMODESCRIPTEVENT_MOUSEPRESSED: 
            GameModePlugin::connectTemplate<LUAGAMEMODESCRIPTEVENT_MOUSEPRESSED>(); return;
        case LUAGAMEMODESCRIPTEVENT_MOUSERELEASED: 
            GameModePlugin::connectTemplate<LUAGAMEMODESCRIPTEVENT_MOUSERELEASED>(); return;
        case LUAGAMEMODESCRIPTEVENT_MOUSEMOVED: 
            GameModePlugin::connectTemplate<LUAGAMEMODESCRIPTEVENT_MOUSEMOVED>(); return;
        case LUAGAMEMODESCRIPTEVENT_KEYPRESSED: 
            GameModePlugin::connectTemplate<LUAGAMEMODESCRIPTEVENT_KEYPRESSED>(); return;
        case LUAGAMEMODESCRIPTEVENT_KEYRELEASED: 
            GameModePlugin::connectTemplate<LUAGAMEMODESCRIPTEVENT_KEYRELEASED>(); return;
    }
}

bool GameModePlugin::isConnected( LuaGameModeScriptEvent event ) const
{
    ConnectionMap::const_iterator i = mConnections.find( event );
    return i != mConnections.end() && i->second.connected();
}

bool GameModePlugin::hasBeenConnected( LuaGameModeScriptEvent event ) const
{
    return mConnections.find( event ) != mConnections.end();
}

void GameModePlugin::block( LuaGameModeScriptEvent event, bool blocked )
{
    ConnectionMap::iterator i = mConnections.find( event );
    if( i != mConnections.end() )
        i->second.block( blocked );
}

bool GameModePlugin::isBlocked( LuaGameModeScriptEvent event ) const
{
    ConnectionMap::const_iterator i = mConnections.find( event );
    return i != mConnections.end() && i->second.blocked();
}

void GameModePlugin::disconnectAll()
{
    for( ConnectionMap::iterator i = mConnections.begin(); i != mConnections.end(); ++i )
        i->second.disconnect();
}

void GameModePlugin::connectAll()
{
    GameModePlugin::connectTemplate<LUAGAMEMODESCRIPTEVENT_EARLYUPDATE>();
    GameModePlugin::connectTemplate<LUAGAMEMODESCRIPTEVENT_UPDATE>();
    GameModePlugin::connectTemplate<LUAGAMEMODESCRIPTEVENT_LATEUPDATE>();
    GameModePlugin::connectTemplate<LUAGAMEMODESCRIPTEVENT_EARLYFRAME>();
    GameModePlugin::connectTemplate<LUAGAMEMODESCRIPTEVENT_FRAME>();
    GameModePlugin::connectTemplate<LUAGAMEMODESCRIPTEVENT_LATEFRAME>();
    GameModePlugin::connectTemplate<LUAGAMEMODESCRIPTEVENT_OBJECTCHANGE>();
    GameModePlugin::connectTemplate<LUAGAMEMODESCRIPTEVENT_MOUSEPRESSED>();
    GameModePlugin::connectTemplate<LUAGAMEMODESCRIPTEVENT_MOUSERELEASED>();
    GameModePlugin::connectTemplate<LUAGAMEMODESCRIPTEVENT_MOUSEMOVED>();
    GameModePlugin::connectTemplate<LUAGAMEMODESCRIPTEVENT_KEYPRESSED>();
    GameModePlugin::connectTemplate<LUAGAMEMODESCRIPTEVENT_KEYRELEASED>();
}

void GameModePlugin::blockAll( bool blocked )
{
    for( ConnectionMap::iterator i = mConnections.begin(); i != mConnections.end(); ++i )
        i->second.block( blocked );
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia