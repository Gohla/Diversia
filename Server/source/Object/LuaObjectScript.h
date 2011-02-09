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

#ifndef DIVERSIA_SERVER_LUAOBJECTSCRIPT_H
#define DIVERSIA_SERVER_LUAOBJECTSCRIPT_H

#include "Platform/Prerequisites.h"

#include "Object/ServerComponent.h"
#include "Shared/Lua/LuaManager.h"

namespace Diversia
{
namespace Server
{
//------------------------------------------------------------------------------

class LuaObjectScript : public ServerComponent, public sigc::trackable
{
public:
    LuaObjectScript( const String& rName, Mode mode, NetworkingType networkingType, 
        RakNet::RakNetGUID source, bool localOverride, ServerObject& rObject );
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
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    void create();
    inline bool delayedDestruction() { return false; }

    void frame( Real timeSinceLastFrame );

    Path                mClientScriptFile;
    String              mClientEnvironmentName;
    LuaSecurityLevel    mClientSecurityLevel;
    Path                mServerScriptFile;
    String              mServerEnvironmentName;
    LuaSecurityLevel    mServerSecurityLevel;
    bool                mLoaded;

    static std::map<String, unsigned int> mDefaultEnvironmentCounter;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Server::LuaObjectScript, 
    &Diversia::Server::Bindings::CampBindings::bindLuaObjectScript );

#endif // DIVERSIA_SERVER_LUAOBJECTSCRIPT_H