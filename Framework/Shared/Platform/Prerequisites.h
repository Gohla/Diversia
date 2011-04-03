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

#ifndef DIVERSIA_SHARED_PREREQUISITES_H
#define DIVERSIA_SHARED_PREREQUISITES_H

// Platform-specific stuff
#include "Util/Build/Platform.h"

// Include dependent libraries
#include "Util/Platform/Prerequisites.h"
#include "Object/Platform/Prerequisites.h"

namespace Diversia
{
// Exports
#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32
#   ifndef DIVERSIA_SHARED_STATIC
#       ifndef DIVERSIA_SHARED_API
#           if defined( DIVERSIA_SHARED_EXPORTS )
#               define DIVERSIA_SHARED_API __declspec(dllexport)
#           else
#               if defined(__MINGW32__)
#                   define DIVERSIA_SHARED_API
#               else
#                   define DIVERSIA_SHARED_API __declspec(dllimport)
#               endif
#           endif
#       endif
#   else
#       define DIVERSIA_SHARED_API
#   endif
#endif

#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_LINUX || DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_APPLE
#   if defined( DIVERSIA_SHARED_GCC_VISIBILITY )
#       define DIVERSIA_SHARED_API  __attribute__ ((visibility("default")))
#   else
#       define DIVERSIA_SHARED_API
#   endif
#endif

// Used namespaces
using namespace Diversia::Util;
using namespace Diversia::ObjectSystem;

// Forward declarations
// Camp
class PropertyTransaction;

// ClientServerPlugin
class ServerNeighbors;
class ClientServerPlugin;
class ClientServerPluginFactory;
class ClientServerPluginFactoryManager;
class ClientServerPluginManager;

// Communication
class ReplicaConnection;
class ReplicaManager;
class GridPosition;
class ServerInfo;
class ServerPosition;
class UserInfo;

// Crash
class CrashReporter;
class WindowsCrashReporter;

// Lua
class LuaManager;

// Permission
class Permission;

// Physics
template <typename T> class AreaTriggerCallback;
class PhysicsHeightfield;

// Resource
class ResourceInfo;

// Defines
// Replica type
//#define REPLICATYPE_OBJECT            0x00
//#define REPLICATYPE_COMPONENT         0x01
#define REPLICATYPE_CLIENTSERVERPLUGIN  0x02
//#define REPLICATYPE_OBJECTTEMPLATE    0x03
//#define REPLICATYPE_COMPONENTTEMPLATE 0x04

// Client-server plugins
typedef unsigned char ClientServerPluginType;
enum ClientServerPluginTypeEnum
{
    CLIENTSERVERPLUGINTYPE_PERMISSIONMANAGER = 0x00,
    CLIENTSERVERPLUGINTYPE_RESOURCEMANAGER = 0x01,
    CLIENTSERVERPLUGINTYPE_LUA = 0x02,
    CLIENTSERVERPLUGINTYPE_SERVERNEIGHBORS = 0x03,
    CLIENTSERVERPLUGINTYPE_SKY = 0x04,
    CLIENTSERVERPLUGINTYPE_OBJECTMANAGER = 0x05,
    CLIENTSERVERPLUGINTYPE_TERRAIN = 0x06,
    CLIENTSERVERPLUGINTYPE_GAMEMODE = 0x07,
    CLIENTSERVERPLUGINTYPE_SCENEMANAGER = 0x08
};
#define CLIENTSERVERPLUGINNAME_PERMISSIONMANAGER "PermissionManager"
#define CLIENTSERVERPLUGINNAME_RESOURCEMANAGER "ResourceManager"
#define CLIENTSERVERPLUGINNAME_LUA "Lua"
#define CLIENTSERVERPLUGINNAME_SERVERNEIGHBORS "ServerNeighbors"
#define CLIENTSERVERPLUGINNAME_SKY "Sky"
#define CLIENTSERVERPLUGINNAME_OBJECTMANAGER "ObjectManager"
#define CLIENTSERVERPLUGINNAME_TERRAIN "Terrain"
#define CLIENTSERVERPLUGINNAME_GAMEMODE "GameMode"
#define CLIENTSERVERPLUGINNAME_SCENEMANAGER "SceneManager"

// Components
enum ComponentTypeEnum
{
    COMPONENTTYPE_SCENENODE = 0x00,
    COMPONENTTYPE_ENTITY = 0x01,
    COMPONENTTYPE_MESH = 0x02,
    COMPONENTTYPE_LIGHT = 0x03,
    COMPONENTTYPE_ANIMATION = 0x04,
    COMPONENTTYPE_PARTICLE = 0x05,
    COMPONENTTYPE_COLLISIONSHAPE = 0x06,
    COMPONENTTYPE_RIGIDBODY = 0x07,
    COMPONENTTYPE_AREATRIGGER = 0x08,
    COMPONENTTYPE_FORCEFIELD = 0x09,
    COMPONENTTYPE_AUDIO = 0x0A,
    COMPONENTTYPE_TEXT = 0x0B,
    COMPONENTTYPE_CAMERA = 0x0C,
    COMPONENTTYPE_LUAOBJECTSCRIPT = 0x0D
};

#define COMPONENTNAME_SCENENODE "SceneNode"
#define COMPONENTNAME_ENTITY "Entity"
#define COMPONENTNAME_MESH "Mesh"
#define COMPONENTNAME_LIGHT "Light"
#define COMPONENTNAME_ANIMATION "Animation"
#define COMPONENTNAME_PARTICLE "Particle"
#define COMPONENTNAME_COLLISIONSHAPE "CollisionShape"
#define COMPONENTNAME_RIGIDBODY "RigidBody"
#define COMPONENTNAME_AREATRIGGER "AreaTrigger"
#define COMPONENTNAME_FORCEFIELD "ForceField"
#define COMPONENTNAME_AUDIO "Audio"
#define COMPONENTNAME_TEXT "Text"
#define COMPONENTNAME_CAMERA "Camera"
#define COMPONENTNAME_LUAOBJECTSCRIPT "LuaObjectScript"

} // namespace Diversia

// Include after the configuration has been set.
#include "Util/Build/StdHeaders.h"

// Logging
#include "Util/Log/Log.h"
namespace Diversia
{
namespace Shared
{
namespace Log
{
// Static logger
static boost::log::sources::severity_channel_logger_mt< Diversia::Util::LogLevel > lg
(
    boost::log::keywords::severity = Diversia::Util::LOG_INFO,
    boost::log::keywords::channel = "Shared"
);

// Easy logging defines.
#define SLOG(severity) BOOST_LOG_SEV(Diversia::Shared::Log::lg, severity)
#define SLOGC BOOST_LOG_SEV(Diversia::Shared::Log::lg, Diversia::Util::LOG_CRITICAL)
#define SLOGE BOOST_LOG_SEV(Diversia::Shared::Log::lg, Diversia::Util::LOG_ERROR)
#define SLOGW BOOST_LOG_SEV(Diversia::Shared::Log::lg, Diversia::Util::LOG_WARNING)
#define SLOGI BOOST_LOG_SEV(Diversia::Shared::Log::lg, Diversia::Util::LOG_INFO)
#define SLOGD BOOST_LOG_SEV(Diversia::Shared::Log::lg, Diversia::Util::LOG_DEBUG)
#define SLOGDE BOOST_LOG_SEV(Diversia::Shared::Log::lg, Diversia::Util::LOG_ENTRYEXIT)
} // Namespace Log
} // Namespace Shared
} // Namespace Diversia

// Camp bindings
#include "Shared/Camp/CampBindings.h"

CAMP_AUTO_TYPE( Diversia::ClientServerPluginTypeEnum, 
    &Diversia::Shared::Bindings::CampBindings::bindClientServerPluginTypeEnum );

CAMP_AUTO_TYPE( Diversia::ComponentTypeEnum, 
    &Diversia::Shared::Bindings::CampBindings::bindComponentTypeEnum );

#endif // DIVERSIA_SHARED_PREREQUISITES_H