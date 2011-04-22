/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.

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

#include "Shared/Platform/StableHeaders.h"

#include "Shared/Camp/CampBindings.h"
#include "Util/Camp/ValueMapper.h"

#include "Shared/Camp/PropertySynchronization.h"
#include "Shared/ClientServerPlugin/ClientServerPlugin.h"
#include "Shared/ClientServerPlugin/ClientServerPluginManager.h"
#include "Shared/Communication/ServerDirection.h"
#include "Shared/Communication/ServerInfo.h"
#include "Shared/Communication/ServerNeighbors.h"
#include "Shared/Communication/UserInfo.h"
#include "Shared/Crash/CrashReporter.h"
#include "Shared/Crash/WindowsCrashReporter.h"
#include "Shared/Graphics/Graphics.h"
#include "Shared/Lua/LuaManager.h"
#include "Shared/Permission/Permission.h"
#include "Shared/Physics/Physics.h"
#include "Shared/Resource/ResourceInfo.h"
#include "Shared/Terrain/Terrain.h"

namespace Diversia
{
namespace Shared
{
namespace Bindings
{
//------------------------------------------------------------------------------

void CampBindings::bindClientServerPluginManager()
{
    camp::Class::declare<ClientServerPluginManager>( "ClientServerPluginManager" )
        // Constructors
        // Properties (read-only)
        .property( "Mode", &ClientServerPluginManager::getMode )
            .tag( "NoBitStream" )
            .tag( "NoSerialization")
        .property( "Plugins", &ClientServerPluginManager::mPlugins )
            .tag( "NoBitStream" )
            .tag( "AddFunction", "CreatePlugin" )
        // Properties (read/write)
        // Functions
        .function( "CreatePlugin", &ClientServerPluginManager::createPlugin )
        .function( "GetPlugin", &ClientServerPluginManager::getPlugin )
        .function( "HasPlugin", &ClientServerPluginManager::hasPlugin )
        .function( "DestroyPlugin", &ClientServerPluginManager::destroyPlugin );
        // Static functions
        // Operators
}

void CampBindings::bindClientServerPlugin()
{
    camp::Class::declare<ClientServerPlugin>( "ClientServerPlugin" )
        // Constructors
        // Properties (read-only)
        .property( "Type", &ClientServerPlugin::getType )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
        .property( "TypeName", &ClientServerPlugin::getTypeName )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
        .property( "Mode", &ClientServerPlugin::getMode )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
        .property( "PluginManager", &ClientServerPlugin::getPluginManager )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" );
        // Properties (read/write)
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindServerInfo()
{
    camp::Class::declare<ServerInfo>( "ServerInfo" )
        // Constructors
        // Properties (read-only)
        .property( "AddressMerged", &ServerInfo::getAddressMerged )
            .tag( "NoBitStream" )
            .tag( "NoSerialization" )
        // Properties (read/write)
        .property( "Address", &ServerInfo::mAddress )
            .tag( "Configurable" )
        .property( "Port", &ServerInfo::mPort)
            .tag( "Configurable" )
        .property( "Name", &ServerInfo::mName )
            .tag( "Configurable" );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindServerNeighbors()
{
    camp::Class::declare<ServerNeighbors>( "ServerNeighbors" )
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        //.property( "Neighbors", &ServerNeighbors::mNeighbors )
        // Functions
        .function( "AddServer", &ServerNeighbors::addServer )
        .function( "GetServer", &ServerNeighbors::getServer )
        .function( "HasServer", &ServerNeighbors::hasServer )
        .function( "RemoveServer", &ServerNeighbors::removeServer )
        .function( "Clear", &ServerNeighbors::clear );
        // Static functions
        // Operators
}

void CampBindings::bindDirection()
{
    camp::Enum::declare<Direction>( "Direction" )
        .value( "North", NORTH )
        .value( "North east", NORTH_EAST )
        .value( "East", EAST )
        .value( "South east", SOUTH_EAST )
        .value( "South", SOUTH )
        .value( "South west", SOUTH_WEST )
        .value( "West", WEST )
        .value( "North west", NORTH_WEST );
}

void CampBindings::bindClientServerPluginTypeEnum()
{
    camp::Enum::declare<ClientServerPluginTypeEnum>( "ClientServerPluginType" )
        .value( CLIENTSERVERPLUGINNAME_PERMISSIONMANAGER, CLIENTSERVERPLUGINTYPE_PERMISSIONMANAGER )
        .value( CLIENTSERVERPLUGINNAME_RESOURCEMANAGER, CLIENTSERVERPLUGINTYPE_RESOURCEMANAGER )
        .value( CLIENTSERVERPLUGINNAME_LUA, CLIENTSERVERPLUGINTYPE_LUA )
        .value( CLIENTSERVERPLUGINNAME_SERVERNEIGHBORS, CLIENTSERVERPLUGINTYPE_SERVERNEIGHBORS )
        .value( CLIENTSERVERPLUGINNAME_SKY, CLIENTSERVERPLUGINTYPE_SKY )
        .value( CLIENTSERVERPLUGINNAME_OBJECTTEMPLATEMANAGER, CLIENTSERVERPLUGINTYPE_OBJECTTEMPLATEMANAGER )
        .value( CLIENTSERVERPLUGINNAME_OBJECTMANAGER, CLIENTSERVERPLUGINTYPE_OBJECTMANAGER )
        .value( CLIENTSERVERPLUGINNAME_TERRAIN, CLIENTSERVERPLUGINTYPE_TERRAIN )
        .value( CLIENTSERVERPLUGINNAME_GAMEMODE, CLIENTSERVERPLUGINTYPE_GAMEMODE )
        .value( CLIENTSERVERPLUGINNAME_SCENEMANAGER, CLIENTSERVERPLUGINTYPE_SCENEMANAGER );
}

void CampBindings::bindComponentTypeEnum()
{
    camp::Enum::declare<ComponentTypeEnum>( "ComponentType" )
        .value( COMPONENTNAME_SCENENODE, COMPONENTTYPE_SCENENODE )
        .value( COMPONENTNAME_ENTITY, COMPONENTTYPE_ENTITY )
        .value( COMPONENTNAME_MESH, COMPONENTTYPE_MESH )
        .value( COMPONENTNAME_LIGHT, COMPONENTTYPE_LIGHT )
        .value( COMPONENTNAME_ANIMATION, COMPONENTTYPE_ANIMATION )
        .value( COMPONENTNAME_PARTICLE, COMPONENTTYPE_PARTICLE )
        .value( COMPONENTNAME_COLLISIONSHAPE, COMPONENTTYPE_COLLISIONSHAPE )
        .value( COMPONENTNAME_RIGIDBODY, COMPONENTTYPE_RIGIDBODY )
        .value( COMPONENTNAME_AREATRIGGER, COMPONENTTYPE_AREATRIGGER )
        .value( COMPONENTNAME_FORCEFIELD, COMPONENTTYPE_FORCEFIELD )
        .value( COMPONENTNAME_AUDIO, COMPONENTTYPE_AUDIO )
        .value( COMPONENTNAME_TEXT, COMPONENTTYPE_TEXT )
        .value( COMPONENTNAME_CAMERA, COMPONENTTYPE_CAMERA )
        .value( COMPONENTNAME_LUAOBJECTSCRIPT, COMPONENTTYPE_LUAOBJECTSCRIPT );
}

void CampBindings::bindResourceInfo()
{
    camp::Class::declare<ResourceInfo>( "ResourceInfo" )
        .tag( "BindingType", BindingType_ResourceInfo )
        // Constructors
        .constructor0()
        //.constructor1<const Path&>()
        //.constructor2<const Path&, const String&>()
        // Properties (read-only)
        // Properties (read/write)
        .property( "File", &ResourceInfo::mFile )
        .property( "Type", &ResourceInfo::mType )
        .property( "Priority", &ResourceInfo::mPriority );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindTerrainTypeEnum()
{
    camp::Enum::declare<TerrainTypeEnum>( "TerrainType" )
        .value( "AutoBlendmap", TERRAINTYPE_AUTOBLENDMAP )
        .value( "OgreBinary", TERRAINTYPE_OGREBINARY );
}

void CampBindings::bindHeightmapTypeEnum()
{
    camp::Enum::declare<HeightmapTypeEnum>( "HeightmapType" )
        .value( "Image", HEIGHTMAPTYPE_IMAGE )
        .value( "Raw", HEIGHTMAPTYPE_RAW )
        .value( "OgreBinary", HEIGHTMAPTYPE_OGREBINARY );
}

void CampBindings::bindLayerInstance()
{
    camp::Class::declare<LayerInstance>( "LayerInstance" )
        // Constructors
        .constructor0()
        .constructor1<Real>()
        // Properties (read-only)
        // Properties (read/write)
        .property( "WorldSize", &LayerInstance::mWorldSize )
            .tag( "Configurable" )
        .property( "TextureNames", &LayerInstance::mTextureFiles )
            .tag( "Configurable" );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindUserInfo()
{
    camp::Class::declare<UserInfo>( "UserInfo" )
        // Constructors
        .constructor0()
        .constructor4<const String&, const String&, const String&, const ServerInfo&>()
        // Properties (read-only)
        // Properties (read/write)
        .property( "Nickname", &UserInfo::mNickname )
            .tag( "Configurable" )
        .property( "Username", &UserInfo::mUsername )
            .tag( "Configurable" )
        .property( "Password", &UserInfo::mPassword )
            .tag( "Configurable" )
        .property( "HomeServer", &UserInfo::mHomeServer )
            .tag( "Configurable" );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindPhysicsType()
{
    camp::Enum::declare<PhysicsType>( "PhysicsType" )
        .value( "Static", PHYSICSTYPE_STATIC )
        .value( "Kinematic", PHYSICSTYPE_KINEMATIC )
        .value( "Dynamic", PHYSICSTYPE_DYNAMIC );
}

void CampBindings::bindPhysicsShape()
{
    camp::Enum::declare<PhysicsShape>( "PhysicsShape" )
        .value( "Sphere", PHYSICSSHAPE_SPHERE )
        .value( "Box", PHYSICSSHAPE_BOX )
        .value( "Cylinder", PHYSICSSHAPE_CYLINDER )
        .value( "Capsule", PHYSICSSHAPE_CAPSULE )
        .value( "Cone", PHYSICSSHAPE_CONE )
        /*.value( "Multisphere", PHYSICSSHAPE_MULTISPHERE )
        .value( "BoxFromAABB", PHYSICSSHAPE_BOXFROMAABB )
        .value( "ConvexHull", PHYSICSSHAPE_CONVEXHULL )
        .value( "ConvexTrimesh", PHYSICSSHAPE_CONVEXTRIANGLEMESH )
        .value( "Trimesh", PHYSICSSHAPE_BVHTRIANGLEMESH )
        .value( "Compound", PHYSICSSHAPE_COMPOUND )*/
        .value( "Heightmap", PHYSICSSHAPE_HEIGHTMAPFILE )
        .value( "File", PHYSICSSHAPE_FILE );
}

void CampBindings::bindLuaManager()
{
    camp::Class::declare<LuaManager>( "LuaManager" );
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindLuaSecurityLevel()
{
    camp::Enum::declare<LuaSecurityLevel>( "LuaSecurityLevel" )
        .value( "High", LUASEC_HIGH )
        .value( "Medium", LUASEC_MEDIUM )
        .value( "Low", LUASEC_LOW );
}

void CampBindings::bindCrashReporter()
{
    camp::Class::declare<CrashReporter>( "CrashReporter" )
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "Enabled", &CrashReporter::isEnabled, &CrashReporter::setEnabled )
            .tag( "Configurable" )
        .property( "Silent", &CrashReporter::isSilent, &CrashReporter::setSilent )
            .tag( "Configurable" )
        .property( "LargeMiniDump", &CrashReporter::isCreateLargeMiniDump, &CrashReporter::setCreateLargeMiniDump )
            .tag( "Configurable" )
        .property( "AppName", &CrashReporter::getAppName,  &CrashReporter::setAppName )
            .tag( "Configurable" )
        .property( "AppVersion", &CrashReporter::getAppVersion, &CrashReporter::setAppVersion )
            .tag( "Configurable" );
        // Functions
        // Static functions
        // Operators
}

#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32
void CampBindings::bindWindowsCrashReporter()
{
    camp::Class::declare<WindowsCrashReporter>( "WindowsCrashReporter" )
        .base<CrashReporter>()
        // Constructors
        .constructor0();
        // Properties (read-only)
        // Properties (read/write)
        // Functions
        // Static functions
        // Operators
}
#endif

void CampBindings::bindPermission()
{
    camp::Class::declare<Permission>( "Permission" )
        // Constructors
        // Properties (read-only)
        .property( "CurrentItems", &Permission::mCurrentItems )
            .writable( false )
        .property( "CurrentItemsPerTime", &Permission::mCurrentItemsPerTime )
            .writable( false )
        // Properties (read/write)
        .property( "Name", &Permission::mName )
            .tag( "Configurable" )
        .property( "Allowed", &Permission::mAllowed )
            .tag( "Configurable" )
        .property( "MaxItemsAllowed", &Permission::mMaxItemsAllowed )
            .tag( "Configurable" )
        .property( "MaxItemsPerTime", &Permission::mMaxItemsPerTime )
            .tag( "Configurable" )
        .property( "Time", &Permission::mTimeSeconds )
            .tag( "Configurable" );
        // TODO: Bounds
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindPropertySynchronization()
{
    camp::Class::declare<PropertySynchronization>( "PropertySynchronization" );
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindGraphicsShape()
{
    camp::Enum::declare<GraphicsShape>( "GraphicsShape" )
        .value( "Box", GRAPHICSSHAPE_BOX )
        .value( "RoundedBox", GRAPHICSSHAPE_ROUNDEDBOX )
        .value( "Plane", GRAPHICSSHAPE_PLANE )
        .value( "Sphere", GRAPHICSSHAPE_SPHERE )
        .value( "IcoSphere", GRAPHICSSHAPE_ICOSPHERE )
        .value( "Cylinder", GRAPHICSSHAPE_CYLINDER )
        .value( "Tube", GRAPHICSSHAPE_TUBE )
        .value( "Capsule", GRAPHICSSHAPE_CAPSULE )
        .value( "Cone", GRAPHICSSHAPE_CONE )
        .value( "Torus", GRAPHICSSHAPE_TORUS )
        .value( "Mesh", GRAPHICSSHAPE_MESH );
}

void CampBindings::bindResourceLocationType()
{
    camp::Enum::declare<ResourceLocationType>( "ResourceLocationType" )
        .value( "FileSystem", RESOURCELOCATIONTYPE_FILESYSTEM )
        .value( "URL", RESOURCELOCATIONTYPE_URL );
}

void CampBindings::bindSkyType()
{
    camp::Enum::declare<SkyType>( "SkyType" )
        .value( "Box", SKYTYPE_BOX )
        .value( "Dome", SKYTYPE_DOME )
        .value( "Plane", SKYTYPE_PLANE );
}

void CampBindings::bindLightType()
{
    camp::Enum::declare<LightType>( "LightType" )
        .value( "Point", LIGHTTYPE_POINT )
        .value( "Spot", LIGHTTYPE_SPOT)
        .value( "Directional", LIGHTTYPE_DIRECTIONAL );
}

//------------------------------------------------------------------------------
} // Namespace Bindings
} // Namespace Shared
} // Namespace Diversia