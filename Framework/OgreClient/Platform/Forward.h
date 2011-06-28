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

#ifndef DIVERSIA_OGRECLIENT_FORWARD_H
#define DIVERSIA_OGRECLIENT_FORWARD_H

#include "Util/Platform/Forward.h"
#include "Object/Platform/Forward.h"
#include "Shared/Platform/Forward.h"
#include "Client/Platform/Forward.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

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
class OgreResourceParams;
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
class ObjectSelector;
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
class URLArchiveParams;
class URLArchiveParamsHolder;
class URLArchive;

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

class btDiscreteDynamicsWorld;
class btAxisSweep3;
namespace Ogre
{
    class SceneManager;
}

#endif // DIVERSIA_OGRECLIENT_FORWARD_H
