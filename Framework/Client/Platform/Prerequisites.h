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

#ifndef DIVERSIA_CLIENT_PREREQUISITES_H
#define DIVERSIA_CLIENT_PREREQUISITES_H

// Platform-specific stuff
#include "Util/Build/Platform.h"

// Include dependent libraries
#include "Util/Platform/Prerequisites.h"
#include "Object/Platform/Prerequisites.h"
#include "Shared/Platform/Prerequisites.h"

namespace Diversia
{
namespace Client
{
// Exports
#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32
#   ifndef DIVERSIA_CLIENT_STATIC
#       ifndef DIVERSIA_CLIENT_API
#           if defined( DIVERSIA_CLIENT_EXPORTS )
#               define DIVERSIA_CLIENT_API __declspec(dllexport)
#           else
#               if defined(__MINGW32__)
#                   define DIVERSIA_CLIENT_API
#               else
#                   define DIVERSIA_CLIENT_API __declspec(dllimport)
#               endif
#           endif
#       endif
#   else
#       define DIVERSIA_CLIENT_API
#   endif
#endif

#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_LINUX || DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_APPLE
#   if defined( DIVERSIA_CLIENT_GCC_VISIBILITY )
#       define DIVERSIA_CLIENT_API  __attribute__ ((visibility("default")))
#   else
#       define DIVERSIA_CLIENT_API
#   endif
#endif

// Used namespaces
using namespace Diversia::Util;
using namespace Diversia::ObjectSystem;
using namespace Diversia;

// Forward declarations
// ClientServerPlugin
class ServerPlugin;
class ServerPluginManager;

// Communication
class GridManager;
class ServerAbstract;
class Server;
class OfflineServer;
class ServerConnection;

// Lua
class LuaPlugin;

// Object
class ClientObjectManager;
class ClientObject;
class ClientComponent;

// Permission
class PermissionManager;

// Undo
class ComponentCommand;
class ObjectCommand;
class FunctionCommand;
class PropertyChangeCommand;
class UndoCommand;
class UndoStack;

} // namespace Client
} // namespace Diversia

// Include after the configuration has been set.
#include "Util/Build/StdHeaders.h"

// Logging
#include "Util/Log/Log.h"
namespace Diversia
{
namespace Client
{
namespace Log
{
// Static logger
static boost::log::sources::severity_channel_logger_mt< Diversia::Util::LogLevel > lg
(
    boost::log::keywords::severity = Diversia::Util::LOG_INFO,
    boost::log::keywords::channel = "Client"
);

// Easy logging defines.
#define LCLOG(severity) BOOST_LOG_SEV(Diversia::Client::Log::lg, severity)
#define LCLOGC BOOST_LOG_SEV(Diversia::Client::Log::lg, Diversia::Util::LOG_CRITICAL)
#define LCLOGE BOOST_LOG_SEV(Diversia::Client::Log::lg, Diversia::Util::LOG_ERROR)
#define LCLOGW BOOST_LOG_SEV(Diversia::Client::Log::lg, Diversia::Util::LOG_WARNING)
#define LCLOGI BOOST_LOG_SEV(Diversia::Client::Log::lg, Diversia::Util::LOG_INFO)
#define LCLOGD BOOST_LOG_SEV(Diversia::Client::Log::lg, Diversia::Util::LOG_DEBUG)
#define LCLOGDE BOOST_LOG_SEV(Diversia::Client::Log::lg, Diversia::Util::LOG_ENTRYEXIT)
} // Namespace Log
} // Namespace Client
} // Namespace Diversia

// Camp bindings
#include "Client/Camp/CampBindings.h"

#endif // DIVERSIA_CLIENT_PREREQUISITES_H