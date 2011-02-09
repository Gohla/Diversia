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

#ifndef DIVERSIA_SERVER_PREREQUISITES_H
#define DIVERSIA_SERVER_PREREQUISITES_H

// Platform-specific stuff
#include "Util/Build/Platform.h"

// Server specific config
#include "Platform/Config.h"

// Include dependent libraries
#include "Util/Platform/Prerequisites.h"
#include "Object/Platform/Prerequisites.h"
#include "Shared/Platform/Prerequisites.h"

namespace Diversia
{
namespace Server
{
// Used namespaces
using namespace Diversia::Util;
using namespace Diversia::ObjectSystem;
using namespace Diversia;

// Forward declarations
class Application;

// ClientServerPlugin
class SkyPlugin;
class ClientPlugin;
class ClientPluginManager;
class ResourceManagerPlugin;

// Communication
class ClientConnection;
class ServerNeighborsPlugin;

// Game mode
class GameModePlugin;

// Resource
class LocalResourceManager;

// User
class Group;
class Session;
class SessionManager;
class User;
class UserManager;

// Permission
class PermissionManager;

// Physics
class PhysicsManager;

// Object
class Animation;
class Audio;
class CollisionShape;
class Entity;
class LuaObjectScript;
class Mesh;
class Particle;
class RigidBody;
class ServerComponent;
class ServerObject;
class ServerObjectManager;
class Text;

} // namespace Server
} // namespace Diversia

// Include after the configuration has been set.
#include "Util/Build/StdHeaders.h"

// Logging
#include "Util/Log/Log.h"
namespace Diversia
{
namespace Server
{
namespace Log
{
// Static logger
static boost::log::sources::severity_channel_logger_mt< Diversia::Util::LogLevel > lg
(
    boost::log::keywords::severity = Diversia::Util::LOG_INFO,
    boost::log::keywords::channel = "Server"
);

// Easy logging defines.
#define LOG(severity) BOOST_LOG_SEV(Diversia::Server::Log::lg, severity)
#define LOGC BOOST_LOG_SEV(Diversia::Server::Log::lg, Diversia::Util::LOG_CRITICAL)
#define LOGE BOOST_LOG_SEV(Diversia::Server::Log::lg, Diversia::Util::LOG_ERROR)
#define LOGW BOOST_LOG_SEV(Diversia::Server::Log::lg, Diversia::Util::LOG_WARNING)
#define LOGI BOOST_LOG_SEV(Diversia::Server::Log::lg, Diversia::Util::LOG_INFO)
#define LOGD BOOST_LOG_SEV(Diversia::Server::Log::lg, Diversia::Util::LOG_DEBUG)
#define LOGDE BOOST_LOG_SEV(Diversia::Server::Log::lg, Diversia::Util::LOG_ENTRYEXIT)
} // Namespace Log
} // Namespace Server
} // Namespace Diversia

// Globals
#include "Globals.h"

// Camp bindings
#include "Camp/CampBindings.h"

#endif // DIVERSIA_SERVER_PREREQUISITES_H