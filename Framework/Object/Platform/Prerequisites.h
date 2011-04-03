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

#ifndef DIVERSIA_OBJECT_PREREQUISITES_H
#define DIVERSIA_OBJECT_PREREQUISITES_H

// Platform-specific stuff
#include "Util/Build/Platform.h"

// Include dependent libraries
#include "Util/Platform/Prerequisites.h"

namespace Diversia
{
namespace ObjectSystem
{
// Exports
#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32
#   ifndef DIVERSIA_OBJECT_STATIC
#       ifndef DIVERSIA_OBJECT_API
#           if defined( DIVERSIA_OBJECT_EXPORTS )
#               define DIVERSIA_OBJECT_API __declspec(dllexport)
#           else
#               if defined(__MINGW32__)
#                   define DIVERSIA_OBJECT_API
#               else
#                   define DIVERSIA_OBJECT_API __declspec(dllimport)
#               endif
#           endif
#       endif
#   else
#       define DIVERSIA_OBJECT_API
#   endif
#endif

#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_LINUX || DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_APPLE
#   if defined( DIVERSIA_OBJECT_GCC_VISIBILITY )
#       define DIVERSIA_OBJECT_API  __attribute__ ((visibility("default")))
#   else
#       define DIVERSIA_OBJECT_API
#   endif
#endif

// Forward declarations
class Component;
class ComponentFactory;
class ComponentTemplate;
class Object;
class ObjectManager;
class ObjectTemplate;
class ObjectTemplateManager;

// Used namespaces
using namespace Diversia::Util;

// Typedefs
typedef unsigned char ReplicaType;
typedef unsigned char ComponentType;

// Enums
/**
Represents client or server mode.
**/
enum Mode
{
    CLIENT = 0,
    SERVER = 1
};

/**
Represents a source.
**/
typedef Mode Source;

/**
Represents local or remote mode.
**/
enum NetworkingType
{
    LOCAL = 0,
    REMOTE = 1
};

// Defines
// Replica type
#define REPLICATYPE_OBJECT                  0x00
#define REPLICATYPE_COMPONENT               0x01
//#define REPLICATYPE_CLIENTSERVERPLUGIN    0x02
#define REPLICATYPE_OBJECTTEMPLATE          0x03
#define REPLICATYPE_COMPONENTTEMPLATE       0x04

} // namespace ObjectSystem
} // namespace Diversia

// Include after the configuration has been set.
#include "Util/Build/StdHeaders.h"

// Logging
#include "Util/Log/Log.h"
namespace Diversia
{
namespace ObjectSystem
{
namespace Log
{
// Static logger
static boost::log::sources::severity_channel_logger_mt< Diversia::Util::LogLevel > lg
(
    boost::log::keywords::severity = Diversia::Util::LOG_INFO,
    boost::log::keywords::channel = "Object"
);

// Easy logging defines.
#define OLOG(severity) BOOST_LOG_SEV(Diversia::ObjectSystem::Log::lg, severity)
#define OLOGC BOOST_LOG_SEV(Diversia::ObjectSystem::Log::lg, Diversia::Util::LOG_CRITICAL)
#define OLOGE BOOST_LOG_SEV(Diversia::ObjectSystem::Log::lg, Diversia::Util::LOG_ERROR)
#define OLOGW BOOST_LOG_SEV(Diversia::ObjectSystem::Log::lg, Diversia::Util::LOG_WARNING)
#define OLOGI BOOST_LOG_SEV(Diversia::ObjectSystem::Log::lg, Diversia::Util::LOG_INFO)
#define OLOGD BOOST_LOG_SEV(Diversia::ObjectSystem::Log::lg, Diversia::Util::LOG_DEBUG)
#define OLOGDE BOOST_LOG_SEV(Diversia::ObjectSystem::Log::lg, Diversia::Util::LOG_ENTRYEXIT)
} // Namespace Log
} // Namespace Object
} // Namespace Diversia

// RPC3 forward declaration.
namespace RakNet {
    class RPC3;
}

// Camp bindings
#include "Object/Camp/CampBindings.h"

CAMP_AUTO_TYPE( Diversia::ObjectSystem::Mode, &Diversia::ObjectSystem::Bindings::CampBindings::bindModeEnum );
CAMP_AUTO_TYPE( Diversia::ObjectSystem::NetworkingType, &Diversia::ObjectSystem::Bindings::CampBindings::bindNetworkingTypeEnum );

#endif // DIVERSIA_OBJECT_PREREQUISITES_H