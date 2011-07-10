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

#ifndef DIVERSIA_OGRECLIENT_GAMEMODEPLUGIN_H
#define DIVERSIA_OGRECLIENT_GAMEMODEPLUGIN_H

#include "OgreClient/Platform/Prerequisites.h"

#include "Client/Plugin/ClientPlugin.h"
#include "OgreClient/Input/InputManager.h"
#include "Shared/Lua/LuaManager.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

/**
Values that represent signals that a lua game mode script can connect to. 
**/
enum LuaGameModeScriptEvent
{
    LUAGAMEMODESCRIPTEVENT_EARLYUPDATE,
    LUAGAMEMODESCRIPTEVENT_UPDATE,
    LUAGAMEMODESCRIPTEVENT_LATEUPDATE,
    LUAGAMEMODESCRIPTEVENT_EARLYFRAME,
    LUAGAMEMODESCRIPTEVENT_FRAME,
    LUAGAMEMODESCRIPTEVENT_LATEFRAME,
    LUAGAMEMODESCRIPTEVENT_OBJECTCHANGE,
    LUAGAMEMODESCRIPTEVENT_MOUSEPRESSED,
    LUAGAMEMODESCRIPTEVENT_MOUSERELEASED,
    LUAGAMEMODESCRIPTEVENT_MOUSEMOVED,
    LUAGAMEMODESCRIPTEVENT_KEYPRESSED,
    LUAGAMEMODESCRIPTEVENT_KEYRELEASED
};

/**
Lua game mode scripting for the entire world.
**/
class DIVERSIA_OGRECLIENT_API GameModePlugin : public ClientPlugin, public KeyboardListener, 
    public MouseListener, public sigc::trackable
{
public:
    GameModePlugin( Mode mode, PluginState state, ClientPluginManager& rPluginManager, 
        RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
        RakNet::NetworkIDManager& rNetworkIDManager );
    ~GameModePlugin();

    /**
    Gets the plugin type.
    **/
    inline PluginTypeEnum getType() const { return PLUGINTYPE_GAMEMODE; }
    static inline PluginTypeEnum getTypeStatic() { return PLUGINTYPE_GAMEMODE; }
    /**
    Gets the plugin type name.
    **/
    inline String getTypeName() const { return PLUGINNAME_GAMEMODE; }
    static inline String getTypeNameStatic() { return PLUGINNAME_GAMEMODE; }

    /**
    Gets the default game mode. 
    **/
    inline GameModeBase* getDefaultGameMode() const { return mGameMode; }
    /**
    Sets the slot that gets called when a default game mode needs to be constructed. Use this to
    construct the default game mode for this application.
    **/
    inline static void setDefaultSlot( const sigc::slot<GameModeBase*, GameModePlugin*>& 
        rSlot )
    {
        mDefaultSlot = rSlot;
    }
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.
    typedef std::map<LuaGameModeScriptEvent, sigc::connection> ConnectionMap;

    void resourcesLoaded();
    void load();
    void unload();
    void reload();
    void setServerState( ServerState serverState );
    void stateChanged( PluginState state, PluginState prevState );

    bool mousePressed( const MouseButton button );
    void mouseReleased( const MouseButton button );
    bool mouseMoved( const MouseState& rState );
    inline int getMousePriority() const { return mMousePriority; }
    void subscribeMouse();
    void unsubscribeMouse();
    bool keyPressed( const KeyboardButton button, unsigned int text );
    void keyReleased( const KeyboardButton button, unsigned int text );
    inline int getKeyboardPriority() const { return mKeyboardPriority; }
    void subscribeKeyboard();
    void unsubscribeKeyboard();

    String getEventName( LuaGameModeScriptEvent event );
    void disconnect( LuaGameModeScriptEvent event );
    bool isDisconnected( LuaGameModeScriptEvent event ) const;
    template <LuaGameModeScriptEvent T> void connectTemplate();
    template <LuaGameModeScriptEvent T> sigc::connection connectImpl();
    void connect( LuaGameModeScriptEvent event );
    bool isConnected( LuaGameModeScriptEvent event ) const;
    bool hasBeenConnected( LuaGameModeScriptEvent event ) const;
    void block( LuaGameModeScriptEvent event, bool blocked );
    bool isBlocked( LuaGameModeScriptEvent event ) const;
    void disconnectAll();
    void connectAll();
    void blockAll( bool blocked );

    GameModeBase*       mGameMode;

    bool                mClientLoadDefaultGamemode;
    std::vector<Path>   mClientScriptFiles;
    LuaSecurityLevel    mClientSecurityLevel;
    std::vector<Path>   mServerScriptFiles;
    LuaSecurityLevel    mServerSecurityLevel;

    int                 mMousePriority;
    int                 mKeyboardPriority;
    bool                mMouseSubscribed;
    bool                mKeyboardSubscribed;
    bool                mCreated;
    sigc::signal<void, const MouseButton>   mMousePressedSignal;    ///< Signals needed to fit in with connect/disconnect system...
    sigc::signal<void, const MouseButton>   mMouseReleasedSignal;
    sigc::signal<void, const MouseState&>   mMouseMovedSignal;
    sigc::signal<void, KeyboardButton, unsigned int> mKeyPressedSignal;
    sigc::signal<void, KeyboardButton, unsigned int> mKeyReleasedSignal;

    LuaManager&         mLuaManager;
    ResourceManager&    mResourceManager;
    ConnectionMap       mConnections;

    static sigc::slot<GameModeBase*, GameModePlugin*>   mDefaultSlot;
    static bool                                         mForceDefault;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::GameModePlugin, 
    &Diversia::OgreClient::Bindings::CampBindings::bindGameModePlugin );

CAMP_AUTO_TYPE( Diversia::OgreClient::LuaGameModeScriptEvent, 
    &Diversia::OgreClient::Bindings::CampBindings::bindLuaGameModeScriptEvent );

#endif // DIVERSIA_OGRECLIENT_GAMEMODEPLUGIN_H