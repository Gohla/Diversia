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

#ifndef DIVERSIA_OGRECLIENT_PREREQUISITES_H
#define DIVERSIA_OGRECLIENT_PREREQUISITES_H

// Platform-specific stuff
#include "Util/Build/Platform.h"

// OgreClient specific config
#include "OgreClient/Platform/Config.h"

// Include dependent libraries
#include "Util/Platform/Prerequisites.h"
#include "Object/Platform/Prerequisites.h"
#include "Shared/Platform/Prerequisites.h"
#include "Client/Platform/Prerequisites.h"

namespace Diversia
{
namespace OgreClient
{
// Exports
#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32
#   ifndef DIVERSIA_OGRECLIENT_STATIC
#       ifndef DIVERSIA_OGRECLIENT_API
#           if defined( DIVERSIA_OGRECLIENT_EXPORTS )
#               define DIVERSIA_OGRECLIENT_API __declspec(dllexport)
#           else
#               if defined(__MINGW32__)
#                   define DIVERSIA_OGRECLIENT_API
#               else
#                   define DIVERSIA_OGRECLIENT_API __declspec(dllimport)
#               endif
#           endif
#       endif
#   else
#       define DIVERSIA_OGRECLIENT_API
#   endif
#endif

#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_LINUX || DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_APPLE
#   if defined( DIVERSIA_OGRECLIENT_GCC_VISIBILITY )
#       define DIVERSIA_OGRECLIENT_API  __attribute__ ((visibility("default")))
#   else
#       define DIVERSIA_OGRECLIENT_API
#   endif
#endif

// Used namespaces
using namespace Diversia::Util;
using namespace Diversia::ObjectSystem;
using namespace Diversia;
using namespace Diversia::Client;

// Forward declarations
class ApplicationBase;

// Audio
class AudioManager;
class AudioResourceLoader;

// Game mode
class GameModeBase;
class GameModePlugin;

// Graphics
class Atlas;
class CameraManager;
class Canvas;
class Fader;
class Gizmo;
class GraphicsManager;
class RotationGizmo;
class ScaleGizmo;
class SceneManagerPlugin;
class SkyPlugin;
class Terrain;
class TextCanvas;
class TextCanvasBatch;
class TranslationGizmo;

// Input
class Axis;
class MouseState;
class KeyboardState;
class InputManager;
class MouseListener;
class KeyboardListener;
class MouseController;
class ObjectSelection;
class WindowsMouseController;

// Object
class Animation;
class AreaTrigger;
class Audio;
class Camera;
class CollisionShape;
class Entity;
class ForceField;
class Light;
class LuaObjectScript;
class Mesh;
class Particle;
class RigidBody;
class SceneNode;
class Text;

// Physics
class PhysicsManager;

// Resource
class GenericResource;
class GenericResourceManager;
class ResourceManager;
class URLArchive;

} // namespace OgreClient
} // namespace Diversia

// Include after the configuration has been set.
#include "Util/Build/StdHeaders.h"

// Logging
#include "Util/Log/Log.h"
namespace Diversia
{
namespace OgreClient
{
namespace Log
{
// Static logger
static boost::log::sources::severity_channel_logger_mt< Diversia::Util::LogLevel > lg
(
    boost::log::keywords::severity = Diversia::Util::LOG_INFO,
    boost::log::keywords::channel = "OgreClient"
);

// Easy logging defines.
#define CLOG(severity) BOOST_LOG_SEV(Diversia::OgreClient::Log::lg, severity)
#define CLOGC BOOST_LOG_SEV(Diversia::OgreClient::Log::lg, Diversia::Util::LOG_CRITICAL)
#define CLOGE BOOST_LOG_SEV(Diversia::OgreClient::Log::lg, Diversia::Util::LOG_ERROR)
#define CLOGW BOOST_LOG_SEV(Diversia::OgreClient::Log::lg, Diversia::Util::LOG_WARNING)
#define CLOGI BOOST_LOG_SEV(Diversia::OgreClient::Log::lg, Diversia::Util::LOG_INFO)
#define CLOGD BOOST_LOG_SEV(Diversia::OgreClient::Log::lg, Diversia::Util::LOG_DEBUG)
#define CLOGDE BOOST_LOG_SEV(Diversia::OgreClient::Log::lg, Diversia::Util::LOG_ENTRYEXIT)
} // Namespace Log
} // Namespace OgreClient
} // Namespace Diversia

// Globals
#include "OgreClient/GlobalsBase.h"

// Camp bindings
#include "OgreClient/Camp/CampBindings.h"

#endif // DIVERSIA_OGRECLIENT_PREREQUISITES_H