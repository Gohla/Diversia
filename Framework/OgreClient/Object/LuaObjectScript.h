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

#ifndef DIVERSIA_OGRECLIENT_LUAOBJECTSCRIPT_H
#define DIVERSIA_OGRECLIENT_LUAOBJECTSCRIPT_H

#include "OgreClient/Platform/Prerequisites.h"

#include "Client/Object/ClientComponent.h"
#include "OgreClient/Input/InputManager.h"
#include "Shared/Lua/LuaManager.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

/**
Values that represent signals that a lua object script can connect to. 
**/
enum LuaObjectScriptEvent
{
    LUAOBJECTSCRIPTEVENT_EARLYUPDATE,
    LUAOBJECTSCRIPTEVENT_UPDATE,
    LUAOBJECTSCRIPTEVENT_LATEUPDATE,
    LUAOBJECTSCRIPTEVENT_EARLYFRAME,
    LUAOBJECTSCRIPTEVENT_FRAME,
    LUAOBJECTSCRIPTEVENT_LATEFRAME,
    LUAOBJECTSCRIPTEVENT_COMPONENTCHANGE,
    LUAOBJECTSCRIPTEVENT_TRANSFORMCHANGE,
    LUAOBJECTSCRIPTEVENT_AREACHANGE,
    LUAOBJECTSCRIPTEVENT_COLLISIONWITH,
    LUAOBJECTSCRIPTEVENT_MOUSEPRESSED,
    LUAOBJECTSCRIPTEVENT_MOUSERELEASED,
    LUAOBJECTSCRIPTEVENT_MOUSEMOVED,
    LUAOBJECTSCRIPTEVENT_KEYPRESSED,
    LUAOBJECTSCRIPTEVENT_KEYRELEASED
};

/**
Lua scripting component for objects in the world.
**/
class DIVERSIA_OGRECLIENT_API LuaObjectScript : public ClientComponent, public KeyboardListener, 
    public MouseListener, public sigc::trackable
{
public:
    LuaObjectScript( const String& rName, Mode mode, NetworkingType networkingType, 
        RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject );
    ~LuaObjectScript();

    /**
    Gets the component type.
    **/
    inline ComponentType getType() const { return COMPONENTTYPE_LUAOBJECTSCRIPT; }
    static inline ComponentType getTypeStatic() { return COMPONENTTYPE_LUAOBJECTSCRIPT; }
    /**
    Gets the component type name.
    **/
    inline String getTypeName() const { return COMPONENTNAME_LUAOBJECTSCRIPT; }
    static inline String getTypeNameStatic() { return COMPONENTNAME_LUAOBJECTSCRIPT; }

    /**
    Sets the client script file for this lua script component. When executed before this component 
    is created (next tick), this will just set the script file. If done after the component is 
    created, this will automatically reload the script file.
    
    @param  rClientScriptFile   Name of the client script file.
    **/
    void setClientScriptFile( const Path& rClientScriptFile );

    void replaceVarWithThisEnv( const String& rEnvName, const String& rVarName );
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.
    typedef std::map<LuaObjectScriptEvent, sigc::connection> ConnectionMap;
    void create();
    void destroy();
    void reload();
    inline bool delayedDestruction() { return false; }
    void resourceLoaded( Ogre::ResourcePtr pResource );
    void pluginStateChanged( PluginState state, PluginState prevState );

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

    String getEventName( LuaObjectScriptEvent event );
    void disconnect( LuaObjectScriptEvent event );
    bool isDisconnected( LuaObjectScriptEvent event ) const;
    template <LuaObjectScriptEvent T> void connectTemplate();
    template <LuaObjectScriptEvent T> sigc::connection connectImpl();
    void connect( LuaObjectScriptEvent event );
    bool isConnected( LuaObjectScriptEvent event ) const;
    bool hasBeenConnected( LuaObjectScriptEvent event ) const;
    void block( LuaObjectScriptEvent event, bool blocked );
    bool isBlocked( LuaObjectScriptEvent event ) const;
    void disconnectAll();
    void connectAll();
    void blockAll( bool blocked );

    Path                mClientScriptFile;
    String              mClientEnvironmentName;
    LuaSecurityLevel    mClientSecurityLevel;
    Path                mServerScriptFile;
    String              mServerEnvironmentName;
    LuaSecurityLevel    mServerSecurityLevel;
    bool                mLoaded;
    bool                mCreated;

    int                 mMousePriority;
    int                 mKeyboardPriority;
    bool                mMouseSubscribed;
    bool                mKeyboardSubscribed;
    sigc::signal<void, const MouseButton>   mMousePressedSignal;    ///< Signals needed to fit in with connect/disconnect system...
    sigc::signal<void, const MouseButton>   mMouseReleasedSignal;
    sigc::signal<void, const MouseState&>   mMouseMovedSignal;
    sigc::signal<void, KeyboardButton, unsigned int> mKeyPressedSignal;
    sigc::signal<void, KeyboardButton, unsigned int> mKeyReleasedSignal;

    ConnectionMap       mConnections;
    LuaManager&         mLuaManager;
    ResourceManager&    mResourceManager;

    static std::map<String, unsigned int> mDefaultEnvironmentCounter;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::LuaObjectScript, 
    &Diversia::OgreClient::Bindings::CampBindings::bindLuaObjectScript );

CAMP_AUTO_TYPE( Diversia::OgreClient::LuaObjectScriptEvent, 
    &Diversia::OgreClient::Bindings::CampBindings::bindLuaObjectScriptEvent );

#endif // DIVERSIA_OGRECLIENT_LUAOBJECTSCRIPT_H