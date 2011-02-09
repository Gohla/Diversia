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

#include "Platform/StableHeaders.h"

#include "Application.h"
#include "Camp/CampBindings.h"
#include "ClientServerPlugin/ClientPlugin.h"
#include "ClientServerPlugin/ClientPluginManager.h"
#include "ClientServerPlugin/ResourceManagerPlugin.h"
#include "ClientServerPlugin/SkyPlugin.h"
#include "ClientServerPlugin/Terrain.h"
#include "Communication/ClientConnection.h"
#include "Communication/ServerNeighborsPlugin.h"
#include "GameMode/GameModePlugin.h"
#include "Object/Animation.h"
#include "Object/Audio.h"
#include "Object/CollisionShape.h"
#include "Object/Entity.h"
#include "Object/LuaObjectScript.h"
#include "Object/Mesh.h"
#include "Object/Particle.h"
#include "Object/RigidBody.h"
#include "Object/ServerComponent.h"
#include "Object/ServerObject.h"
#include "Object/ServerObjectManager.h"
#include "Object/Text.h"
#include "Permission/PermissionManager.h"
#include "Physics/PhysicsManager.h"
#include "Resource/LocalResourceManager.h"
#include "User/Group.h"
#include "User/User.h"
#include "User/UserManager.h"
#include "Util/Camp/ValueMapper.h"

namespace Diversia
{
namespace Server 
{
namespace Bindings
{
//------------------------------------------------------------------------------

void CampBindings::bindServerObjectManager()
{
    camp::Class::declare<ServerObjectManager>( "ServerObjectManager" )
        .base<ObjectManager>()
        .base<ClientPlugin>();
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindServerObject()
{
    camp::Class::declare<ServerObject>( "ServerObject" )
        .base<Object>();
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindServerComponent()
{
    camp::Class::declare<ServerComponent>( "ServerComponent" )
        .base<Component>()
        .base<PropertySynchronization>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "ResourceList", &ServerComponent::mResourceList );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindClientPluginManager()
{
    camp::Class::declare<ClientPluginManager>( "ClientPluginManager" )
        .base<ClientServerPluginManager>();
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindClientPlugin()
{
    camp::Class::declare<ClientPlugin>( "ClientPlugin" )
        .base<ClientServerPlugin>()
        .base<PropertySynchronization>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        // Functions
        .function( "ForceSerialize", &ClientPlugin::forceSerializeProperties );
        // Static functions
        // Operators
}

void CampBindings::bindPermissionManager()
{
    camp::Class::declare<PermissionManager>( "PermissionManager" )
        .base<ClientPlugin>();
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindServerNeighborsPlugin()
{
    camp::Class::declare<ServerNeighborsPlugin>( "ServerNeighborsPlugin" )
        .base<ClientPlugin>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "ServerNeighbors", &ServerNeighborsPlugin::mServerNeighbors )
            .tag( "Configurable" );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindResourceManagerPlugin()
{
    camp::Class::declare<ResourceManagerPlugin>( "ResourceManager" )
        .base<ClientPlugin>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "Type", &ResourceManagerPlugin::mType )
            .tag( "Configurable" )
        .property( "ResourceLocation", &ResourceManagerPlugin::mResourceLocation )
            .tag( "Configurable" );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindPrecipitationType()
{
    camp::Enum::declare<PrecipitationType>( "PrecipitationType" )
        .value( "Drizzle", PRECTYPE_DRIZZLE )
        .value( "Rain", PRECTYPE_RAIN )
        .value( "Snow", PRECTYPE_SNOW )
        .value( "Snowgrains", PRECTYPE_SNOWGRAINS )
        .value( "Icecrystals", PRECTYPE_ICECRYSTALS )
        .value( "Icepellets", PRECTYPE_ICEPELLETS )
        .value( "Hail", PRECTYPE_HAIL )
        .value( "SmallHail", PRECTYPE_SMALLHAIL );
}

void CampBindings::bindSkyPlugin()
{
    camp::Class::declare<SkyPlugin>( "Sky" )
        .base<ClientPlugin>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "Timescale", &SkyPlugin::mTimescale )
            .tag( "Configurable" )
        .property( "FogDensity", &SkyPlugin::mFogDensity )
            .tag( "Configurable" )
        .property( "FogColour", &SkyPlugin::mFogColour )
            .tag( "Configurable" )
        .property( "ObserverLongitude", &SkyPlugin::mObserverLongitude )
            .tag( "Configurable" )
        .property( "ObserverLatitude", &SkyPlugin::mObserverLatitude )
            .tag( "Configurable" )
        .property( "CloudSpeed", &SkyPlugin::mCloudSpeed )
            .tag( "Configurable" )
        .property( "CloudBlendTime", &SkyPlugin::mCloudBlendTime )
            .tag( "Configurable" )
        .property( "CloudCoverage", &SkyPlugin::mCloudCoverage )
            .tag( "Configurable" )
        .property( "CloudHeight", &SkyPlugin::mCloudHeight )
            .tag( "Configurable" )
        .property( "PrecipitationType", &SkyPlugin::mPrecipitationType )
            .tag( "Configurable" )
        .property( "PrecipitationIntensity", &SkyPlugin::mPrecipitationIntensity )
            .tag( "Configurable" )
        .property( "PrecipitationSpeed", &SkyPlugin::mPrecipitationSpeed )
            .tag( "Configurable" )
        .property( "PrecipitationCameraSpeedScale", &SkyPlugin::mPrecipitationCameraSpeedScale )
            .tag( "Configurable" )
        .property( "SunAmbientMultiplier", &SkyPlugin::mSunAmbientMultiplier )
            .tag( "Configurable" )
        .property( "SunSpecularMultiplier", &SkyPlugin::mSunSpecularMultiplier )
            .tag( "Configurable" )
        .property( "SunDiffuseMultiplier", &SkyPlugin::mSunDiffuseMultiplier )
            .tag( "Configurable" );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindClientConnectionSettings()
{
    camp::Class::declare<ClientConnection::Settings>( "ClientConnectionSettings" )
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "Plugins", &ClientConnection::Settings::mPlugins )
            .tag( "Configurable" )
        .property( "ServerInfo", &ClientConnection::Settings::mServerInfo )
            .tag( "Configurable" )
        .property( "MaxConnections", &ClientConnection::Settings::mMaxConnections )
            .tag( "Configurable" )
        .property( "ThreadSleepTimer", &ClientConnection::Settings::mThreadSleepTimer )
            .tag( "Configurable" )
        .property( "ConnectionAttemptCount", &ClientConnection::Settings::mConnectionAttemptCount )
            .tag( "Configurable" )
        .property( "TimeBetweenConnectionAttempts", &ClientConnection::Settings::mTimeBetweenConnectionAttemptsMS )
            .tag( "Configurable" )
        .property( "Timeout", &ClientConnection::Settings::mTimeoutMS )
            .tag( "Configurable" )
        .property( "ShutdownBlockDuraction", &ClientConnection::Settings::mShutdownBlockDuractionMS )
            .tag( "Configurable" );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindApplication()
{
    camp::Class::declare<Application>( "Application" )
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "LogLevel", &Application::mLogLevel )
            .tag( "Configurable" )
        .property( "SleepMS", &Application::mSleepMS )
            .tag( "Configurable" )
        // Functions
        .function( "Quit", &Application::quit );
        // Static functions
        // Operators
}

void CampBindings::bindEntity()
{
    camp::Class::declare<Entity>( "Entity" )
        .base<ServerComponent>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "Material", &Entity::mMaterial )
        .property( "CastShadows", &Entity::mCastShadows )
        .property( "Visible", &Entity::mVisible );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindGameModePlugin()
{
    camp::Class::declare<GameModePlugin>( "GameMode" )
        .base<ClientPlugin>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "ClientLoadDefaultGamemode", &GameModePlugin::mClientLoadDefaultGamemode )
            .tag( "Configurable" )
        .property( "ClientScriptFiles", &GameModePlugin::mClientScriptFiles )
            .tag( "Configurable" )
        .property( "ClientSecurityLevel", &GameModePlugin::mClientSecurityLevel )
            .tag( "Configurable" )
        .property( "ServerScriptFiles", &GameModePlugin::mServerScriptFiles )
            .tag( "Configurable" )
        .property( "ServerSecurityLevel", &GameModePlugin::mServerSecurityLevel )
            .tag( "Configurable" );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindTerrain()
{
    camp::Class::declare<Terrain>( "Terrain" )
        .base<ClientPlugin>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "TerrainType", &Terrain::mTerrainType )
            .tag( "Configurable" )
        .property( "HeightmapType", &Terrain::mHeightmapType )
            .tag( "Configurable" )
        .property( "ClientHeightmapFile", &Terrain::mClientHeightmapFile )
            .tag( "Configurable" )
        .property( "ServerHeightmapFile", &Terrain::mServerHeightmapFile )
            .tag( "Configurable" )
            .tag( "NoBitStream" )
        .property( "InputScale", &Terrain::mInputScale )
            .tag( "Configurable" )
        .property( "HeightOffset", &Terrain::mHeightOffset )
        .property( "LayerInstances", &Terrain::mLayerInstances )
            .tag( "Configurable" )
        .property( "MinHeight0", &Terrain::mMinHeight0 )
            .tag( "Configurable" )
        .property( "FadeDistance0", &Terrain::mFadeDistance0 )  
            .tag( "Configurable" )
        .property( "MinHeight1", &Terrain::mMinHeight1 )
            .tag( "Configurable" )
        .property( "FadeDistance1", &Terrain::mFadeDistance1 )
            .tag( "Configurable" );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindAnimation()
{
    camp::Class::declare<Animation>( "Animation" )
        .base<ServerComponent>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "AnimationName", &Animation::mAnimationName )
        .property( "Speed", &Animation::mSpeed )
        .property( "Enabled", &Animation::mEnabled )
        .property( "Loop", &Animation::mLoop )
        .property( "Position", &Animation::mPosition )
        .property( "Length", &Animation::mLength )
        .property( "AutoLength", &Animation::mAutoLength )
        .property( "Weight", &Animation::mWeight );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindText()
{
    camp::Class::declare<Text>( "Text" )
        .base<ServerComponent>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "Caption", &Text::mCaption )
        .property( "Colour", &Text::mColour );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindPhysicsManager()
{
    camp::Class::declare<PhysicsManager>( "PhysicsManager" )
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "HeightmapWorldSize", &PhysicsManager::mHeightmapWorldSize )
            .tag( "Configurable" )
        .property( "HeightmapYScale", &PhysicsManager::mHeightmapYScale )
            .tag( "Configurable" );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindRigidBody()
{
    camp::Class::declare<RigidBody>( "RigidBody" )
        .base<ServerComponent>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "PhysicsType", &RigidBody::mPhysicsType )
        .property( "Mass", &RigidBody::mMass )
        .property( "Friction", &btRigidBody::getFriction, &btRigidBody::setFriction, &RigidBody::getRigidBody )
            .readable( &RigidBody::isLoaded )
            .writable( &RigidBody::isLoaded )
        .property( "Restitution", &btRigidBody::getRestitution, &btRigidBody::setRestitution, &RigidBody::getRigidBody )
            .readable( &RigidBody::isLoaded )
            .writable( &RigidBody::isLoaded )
        .property( "LinearDampening", &RigidBody::getLinearDampening, &RigidBody::setLinearDampening )
            .readable( &RigidBody::isLoaded )
            .writable( &RigidBody::isLoaded )
        .property( "AngularDampening", &RigidBody::getAngularDampening, &RigidBody::setAngularDampening )
            .readable( &RigidBody::isLoaded )
            .writable( &RigidBody::isLoaded )
        .property( "LinearSleepingThreshold", &RigidBody::getLinearSleepingThreshold, &RigidBody::setLinearSleepingThreshold )
            .readable( &RigidBody::isLoaded )
            .writable( &RigidBody::isLoaded )
        .property( "AngularSleepingThreshold", &RigidBody::getAngularSleepingThreshold, &RigidBody::setAngularSleepingThreshold )
            .readable( &RigidBody::isLoaded )
            .writable( &RigidBody::isLoaded )
        // Functions
        .function( "Activate", &btRigidBody::activate, &RigidBody::getRigidBody )
        .function( "ApplyForce", boost::function<void(RigidBody&, const Vector3&, const Vector3&)>( boost::bind( &btRigidBody::applyForce, boost::bind( &RigidBody::getRigidBody, _1 ), boost::bind( &toVector3<btVector3, Vector3>, _2 ), boost::bind( &toVector3<btVector3, Vector3>, _3 ) ) ) )
            .callable( &RigidBody::isLoaded )
        .function( "ApplyImpulse", boost::function<void(RigidBody&, const Vector3&, const Vector3&)>( boost::bind( &btRigidBody::applyImpulse, boost::bind( &RigidBody::getRigidBody, _1 ), boost::bind( &toVector3<btVector3, Vector3>, _2 ), boost::bind( &toVector3<btVector3, Vector3>, _3 ) ) ) )
            .callable( &RigidBody::isLoaded );
        // Static functions
        // Operators
}

void CampBindings::bindLocalResourceManager()
{
    camp::Class::declare<LocalResourceManager>( "LocalResourceManager" )
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "RootResourceLocation", &LocalResourceManager::mRootResourceLocation )
            .tag( "Configurable" );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindClientConnection()
{
    camp::Class::declare<ClientConnection>( "ClientConnection" );
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindAudio()
{
    camp::Class::declare<Audio>( "Audio" )
        .base<ServerComponent>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "File", &Audio::mFile )
        .property( "Volume", &Audio::mVolume )
        .property( "VolumeSmooth", &Audio::mVolume )
        .property( "VolumeSmoothTimescale", &Audio::mVolumeSmoothTimescale )
        .property( "Loop", &Audio::mLoop )
        .property( "AutoPlay", &Audio::mAutoPlay )
        .property( "Pitch", &Audio::mPitch )
        .property( "MinDistance", &Audio::mMinDistance )
        .property( "MaxDistance", &Audio::mMaxDistance )
        .property( "MinVolume", &Audio::mMinVolume )
        .property( "MaxVolume", &Audio::mMaxVolume )
        .property( "Strength", &Audio::mStrength )
        .property( "DopplerStrength", &Audio::mDopplerStrength )
        .property( "RolloffFactor", &Audio::mRolloffFactor )
        // Functions
        .function( "Play", boost::function<void(Audio&)>( boost::bind( &Audio::serializeCall, _1, String( "Play" ) ) ) )
            .tag( "NoCall" )
        .function( "Stop", boost::function<void(Audio&)>( boost::bind( &Audio::serializeCall, _1, String( "Stop" ) ) ) )
            .tag( "NoCall" )
        .function( "Pause", boost::function<void(Audio&)>( boost::bind( &Audio::serializeCall, _1, String( "Pause" ) ) ) )
            .tag( "NoCall" );
        // Static functions
        // Operators
}

void CampBindings::bindLuaObjectScript()
{
    camp::Class::declare<LuaObjectScript>( "LuaObjectScript" )
        .base<ServerComponent>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "ClientScriptFile", &LuaObjectScript::mClientScriptFile )
        .property( "ClientEnvironmentName", &LuaObjectScript::mClientEnvironmentName )
        .property( "ClientSecurityLevel", &LuaObjectScript::mClientSecurityLevel )
        .property( "ServerScriptFile", &LuaObjectScript::mServerScriptFile )
        .property( "ServerEnvironmentName", &LuaObjectScript::mServerEnvironmentName )
        .property( "ServerSecurityLevel", &LuaObjectScript::mServerSecurityLevel );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindParticle()
{
    camp::Class::declare<Particle>( "Particle" )
        .base<ServerComponent>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "Name", &Particle::mParticleSystemName )
        .property( "Quota", &Particle::mQuota )
        .property( "Enabled", &Particle::mEnabled )
        .property( "Speed", &Particle::mSpeed )
        // Functions
        .function( "Clear", boost::function<void(Particle&)>( boost::bind( &Particle::serializeCall, _1, String( "Clear" ) ) ) )
            .tag( "NoCall" );
        // Static functions
        // Operators
}

void CampBindings::bindUserManager()
{
    camp::Class::declare<UserManager>( "UserManager" )
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "Groups", &UserManager::mGroups )
            .tag( "Configurable" )
            .tag( "AddFunction", "AddGroup" )
        .property( "Users", &UserManager::mUsers )
            .tag( "Configurable" )
            .tag( "AddFunction", "AddUser" )
        // Functions
        .function( "AddGroup", &UserManager::addGroup )
        .function( "HasGroup", &UserManager::hasGroup )
        .function( "GetGroup", &UserManager::getGroup )
        .function( "AddUser", (User&(UserManager::*)(const String&))&UserManager::addUser )
        .function( "HasUser", &UserManager::hasUser )
        .function( "GetUser", &UserManager::getUser );
        // Static functions
        // Operators
}

void CampBindings::bindUser()
{
    camp::Class::declare<User>( "User" )
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "Name", &User::mName )
            .tag( "Configurable" )
        .property( "Password", &User::mPassword )
            .tag( "Configurable" )
        .property( "Guest", &User::mGuest )
            .tag( "Configurable" )
        .property( "Permissions", &User::mPermissions )
            .tag( "Configurable" )
            .tag( "AddFunction", "AddPermission" )
        // Functions
        .function( "AddPermission", (Permission&(User::*)(const String&))&User::addPermission )
        .function( "HasPermission", &User::hasPermission )
        .function( "GetPermission", &User::getPermission );
        // Static functions
        // Operators
}

void CampBindings::bindGroup()
{
    camp::Class::declare<Group>( "Group" )
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "Name", &Group::mName )
            .tag( "Configurable" );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindMesh()
{
    camp::Class::declare<Mesh>( "Mesh" )
        .base<ServerComponent>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "MeshFile", &Mesh::mMeshFile )
        .property( "ShapeType", &Mesh::mShapeType )
        .property( "ShapeParameters", &Mesh::mShapeParameters );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindCollisionShape()
{
    camp::Class::declare<CollisionShape>( "CollisionShape" )
        .base<ServerComponent>()
        // Constructors
        // Properties (read-only)
        .property( "IsReceivingCollisionCallbacks", &CollisionShape::isReceivingCollisionCallbacks )
            .tag( "NoBitStream" )
        // Properties (read/write)
        .property( "CollisionFile", &CollisionShape::mCollisionFile, &CollisionShape::setCollisionFile )
        .property( "ShapeType", &CollisionShape::mShapeType )
        .property( "ShapeParameters", &CollisionShape::mShapeParameters, &CollisionShape::setShapeParameters )
        .property( "CollisionPriority", &CollisionShape::mCollisionPriority );
        // Functions
        // Static functions
        // Operators
}

//------------------------------------------------------------------------------
} // Namespace Bindings
} // Namespace Server
} // Namespace Diversia