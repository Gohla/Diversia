/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "DSLTypeExtractor/Platform/StableHeaders.h"

#include "DSLTypeExtractor/register.hpp"

#include "Client/ClientServerPlugin/ServerPlugin.h"
#include "Client/Communication/ServerNeighborsPlugin.h"
#include "Client/Lua/LuaPlugin.h"
#include "Client/Object/ClientComponent.h"
#include "Client/Object/ClientComponentTemplate.h"
#include "Client/Object/ClientObject.h"
#include "Client/Object/ClientObjectManager.h"
#include "Client/Object/ClientObjectTemplate.h"
#include "Client/Object/ClientObjectTemplateManager.h"
#include "Client/Permission/PermissionManager.h"
#include "Object/Component.h"
#include "Object/ComponentTemplate.h"
#include "Object/Object.h"
#include "Object/ObjectManager.h"
#include "Object/ObjectTemplate.h"
#include "Object/ObjectTemplateManager.h"
#include "OgreClient/GameMode/GameModePlugin.h"
#include "OgreClient/Graphics/SceneManagerPlugin.h"
#include "OgreClient/Graphics/SkyPlugin.h"
#include "OgreClient/Graphics/Terrain.h"
#include "OgreClient/Input/InputManager.h"
#include "OgreClient/Object/Animation.h"
#include "OgreClient/Object/AreaTrigger.h"
#include "OgreClient/Object/Audio.h"
#include "OgreClient/Object/Camera.h"
#include "OgreClient/Object/CollisionShape.h"
#include "OgreClient/Object/Entity.h"
#include "OgreClient/Object/ForceField.h"
#include "OgreClient/Object/Light.h"
#include "OgreClient/Object/LuaObjectScript.h"
#include "OgreClient/Object/Mesh.h"
#include "OgreClient/Object/Particle.h"
#include "OgreClient/Object/RigidBody.h"
#include "OgreClient/Object/SceneNode.h"
#include "OgreClient/Object/Text.h"
#include "OgreClient/Resource/ResourceManager.h"
#include "Shared/ClientServerPlugin/ClientServerPluginManager.h"
#include "Shared/ClientServerPlugin/Factories/ObjectManagerFactory.h"
#include "Shared/ClientServerPlugin/Factories/TemplatePluginFactory.h"
#include "Shared/Object/TemplateComponentFactory.h"
#include "Util/Camp/CampUtils.h"
#include "Util/Log/Logger.h"
#include "Util/Math/Node.h"

namespace Diversia
{
namespace DSLTypeExtractor 
{
//------------------------------------------------------------------------------

void registerCamp()
{
    // Use all classes
    camp::classByType<Vector2>();
    camp::classByType<Vector3>();
    camp::classByType<Vector4>();
    camp::classByType<Colour>();
    camp::classByType<Quaternion>();
    camp::classByType<Matrix3>();
    camp::classByType<Matrix4>();
    camp::classByType<Radian>();
    camp::classByType<Degree>();
    camp::classByType<Angle>();
    camp::classByType<Node>();
    //camp::classByType<ClientServerPluginManager>();
    //camp::classByType<ClientServerPlugin>();
    //camp::classByType<ServerInfo>();
    //camp::classByType<ServerNeighbors>();
    //camp::classByType<ResourceInfo>();
    //camp::classByType<LayerInstance>();
    //camp::classByType<UserInfo>();
    //camp::classByType<LuaManager>();
    //camp::classByType<CrashReporter>();
    //camp::classByType<WindowsCrashReporter>();
    //camp::classByType<Permission>();
    //camp::classByType<PropertySynchronization>();
    //camp::classByType<OgreResourceParams>();
    //camp::classByType<GraphicsManager>();
    camp::classByType<ResourceManager>();
    //camp::classByType<URLArchiveParams>();
    //camp::classByType<URLArchiveParamsHolder>();
    camp::classByType<SkyPlugin>();
    camp::classByType<SceneNode>();
    camp::classByType<Entity>();
    camp::classByType<Camera>();
    camp::classByType<GameModePlugin>();
    camp::classByType<Terrain>();
    camp::classByType<Animation>();
    //camp::classByType<TextCanvas>();
    camp::classByType<Text>();
    //camp::classByType<PhysicsManager>();
    camp::classByType<RigidBody>();
    camp::classByType<Audio>();
    //camp::classByType<AudioManager>();
    //camp::classByType<LuaObjectScript>();
    camp::classByType<Particle>();
    camp::classByType<Axis>();
    camp::classByType<MouseState>();
    camp::classByType<KeyboardState>();
    //camp::classByType<MouseListener>();
    //camp::classByType<KeyboardListener>();
    camp::classByType<InputManager>();
    camp::classByType<Mesh>();
    camp::classByType<CollisionShape>();
    camp::classByType<SceneManagerPlugin>();
    camp::classByType<AreaTrigger>();
    camp::classByType<ForceField>();
    camp::classByType<Light>();
    //camp::classByType<Gizmo>();
    //camp::classByType<RotationGizmo>();
    //camp::classByType<ScaleGizmo>();
    //camp::classByType<TranslationGizmo>();
    //camp::classByType<RakNet::RakNetGUID>();
    camp::classByType<ObjectManager>();
    camp::classByType<Object>();
    camp::classByType<Component>();
    //camp::classByType<ComponentHandle>();
    camp::classByType<ObjectTemplate>();
    camp::classByType<ObjectTemplateManager>();
    camp::classByType<ComponentTemplate>();
    //camp::classByType<ServerPluginManager>();
    camp::classByType<ClientObjectManager>();
    camp::classByType<ClientObject>();
    camp::classByType<ClientComponent>();
    camp::classByType<ServerPlugin>();
    camp::classByType<PermissionManager>();
    //camp::classByType<GridManager>();
    //camp::classByType<ServerConnection::Settings>();
    camp::classByType<ServerNeighborsPlugin>();
    camp::classByType<LuaPlugin>();
    camp::classByType<ClientComponentTemplate>();
    camp::classByType<ClientObjectTemplateManager>();
    camp::classByType<ClientObjectTemplate>();

    // Use all enums
    //camp::enumByType<BindingType>();
    //camp::enumByType<LogLevel>();
    camp::enumByType<Node::TransformSpace>();
    //camp::enumByType<Direction>();
    camp::enumByType<ClientServerPluginTypeEnum>();
    camp::enumByType<ComponentTypeEnum>();
    camp::enumByType<TerrainTypeEnum>();
    camp::enumByType<HeightmapTypeEnum>();
    camp::enumByType<PhysicsType>();
    camp::enumByType<PhysicsShape>();
    //camp::enumByType<LuaSecurityLevel>();
    camp::enumByType<GraphicsShape>();
    //camp::enumByType<ResourceLocationType>();
    camp::enumByType<SkyType>();
    camp::enumByType<LightType>();
    //camp::enumByType<Ogre::TextureFilterOptions>();
    camp::enumByType<Caelum::PrecipitationType>();
    camp::enumByType<LuaGameModeScriptEvent>();
    camp::enumByType<LuaObjectScriptEvent>();
    camp::enumByType<MouseButton>();
    camp::enumByType<KeyboardButton>();
    //camp::enumByType<Mode>();
    //camp::enumByType<NetworkingType>();

    // Add component factories, get camp class to ensure that the class is registered.
    TemplateComponentFactory<SceneNode, ClientObject, false, false, true>::registerFactory();
    Object::addAutoCreateComponent<SceneNode>( "Node" );
    camp::classByType<SceneNode>();
    TemplateComponentFactory<Mesh, ClientObject, false>::registerFactory();
    camp::classByType<Mesh>();
    TemplateComponentFactory<Entity, ClientObject, false>::registerFactory();
    camp::classByType<Entity>();
    TemplateComponentFactory<Light, ClientObject, false>::registerFactory();
    camp::classByType<Light>();
    TemplateComponentFactory<Camera, ClientObject, false, true, true>::registerFactory();
    camp::classByType<Camera>();
    TemplateComponentFactory<Animation, ClientObject, true>::registerFactory();
    camp::classByType<Animation>();
    TemplateComponentFactory<Text, ClientObject, true>::registerFactory();
    camp::classByType<Text>();
    TemplateComponentFactory<CollisionShape, ClientObject, false>::registerFactory();
    camp::classByType<CollisionShape>();
    TemplateComponentFactory<RigidBody, ClientObject, false>::registerFactory();
    camp::classByType<RigidBody>();
    TemplateComponentFactory<AreaTrigger, ClientObject, false>::registerFactory();
    camp::classByType<AreaTrigger>();
    TemplateComponentFactory<ForceField, ClientObject, false>::registerFactory();
    camp::classByType<ForceField>();
    TemplateComponentFactory<Audio, ClientObject, true>::registerFactory();
    camp::classByType<Audio>();
    //TemplateComponentFactory<LuaObjectScript, ClientObject, true>::registerFactory();
    //camp::classByType<LuaObjectScript>();
    TemplateComponentFactory<Particle, ClientObject, true>::registerFactory();
    camp::classByType<Particle>();

    // Add plugin factories, get camp class to ensure that the class is registered.
    TemplatePluginFactory<PermissionManager, ServerPluginManager>::registerFactory();
    camp::classByType<PermissionManager>();
    ClientServerPluginManager::addAutoCreatePlugin<PermissionManager>();
    TemplatePluginFactory<ResourceManager, ServerPluginManager>::registerFactory();
    camp::classByType<ResourceManager>();
    ClientServerPluginManager::addAutoCreatePlugin<ResourceManager>();
    TemplatePluginFactory<ClientObjectTemplateManager, ServerPluginManager>::registerFactory();
    camp::classByType<ClientObjectTemplateManager>();
    ClientServerPluginManager::addAutoCreatePlugin<ClientObjectTemplateManager>();
    sigc::signal<void> dummySignal;
    ObjectManagerFactory<ClientObjectManager, ServerPluginManager>::registerFactory( dummySignal, dummySignal );
    camp::classByType<ClientObjectManager>();
    ClientServerPluginManager::addAutoCreatePlugin<ClientObjectManager>();
    TemplatePluginFactory<ServerNeighborsPlugin, ServerPluginManager>::registerFactory();
    camp::classByType<ServerNeighborsPlugin>();
    TemplatePluginFactory<SkyPlugin, ServerPluginManager>::registerFactory();
    camp::classByType<SkyPlugin>();
    TemplatePluginFactory<Terrain, ServerPluginManager>::registerFactory();
    camp::classByType<Terrain>();
    TemplatePluginFactory<SceneManagerPlugin, ServerPluginManager>::registerFactory();
    camp::classByType<SceneManagerPlugin>();
    TemplatePluginFactory<LuaPlugin, ServerPluginManager>::registerFactory();
    camp::classByType<LuaPlugin>();
    ClientServerPluginManager::addAutoCreatePlugin<LuaPlugin>();
}

//------------------------------------------------------------------------------
} // Namespace DSLTypeExtractor
} // Namespace Diversia