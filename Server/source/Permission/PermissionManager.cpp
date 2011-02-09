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

#include "Permission/PermissionManager.h"
#include "Shared/Lua/LuaManager.h"
#include "Shared/Permission/Permission.h"
#include "User/Session.h"
#include "User/SessionManager.h"

namespace Diversia
{
namespace Server 
{
//------------------------------------------------------------------------------

PermissionManager::PermissionManager( Mode mode, ClientPluginManager& rPluginManager, 
    RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
    RakNet::NetworkIDManager& rNetworkIDManager ):
    ClientPlugin( mode, rPluginManager, rRakPeer, rReplicaManager, rNetworkIDManager ),
    mSessionManager( 0 ),
    mUserManager( 0 )
{
    PropertySynchronization::storeUserObject();

    Globals::mLua->object( "Permission" ) = this;
}

void PermissionManager::setSessionManager( SessionManager& rSessionManager )
{
    mSessionManager = &rSessionManager;
    mUserManager = &rSessionManager.getUserManager();

    // Set default permissions
    // Object
    PermissionManager::setPermission( "Guest", "ObjectManager_CreateRemoteObject", true, 300 );
    PermissionManager::setPermission( "Guest", "ObjectManager_CreateLocalObject", true );

    PermissionManager::setPermission( "Guest", "ObjectManager_DestroyOwnObject", true );
    PermissionManager::setPermission( "Guest", "ObjectManager_DestroyLocalObject", true );
    PermissionManager::setPermission( "Guest", "ObjectManager_DestroyOtherObject", false );

    PermissionManager::setPermission( "Guest", "Object_UnparentOnOwnObject", true );
    PermissionManager::setPermission( "Guest", "Object_UnparentOnOtherObject", false );
    PermissionManager::setPermission( "Guest", "Object_SetOwnParentOnOwnObject", true );
    PermissionManager::setPermission( "Guest", "Object_SetOtherParentOnOwnObject", false );
    PermissionManager::setPermission( "Guest", "Object_SetOwnParentOnOtherObject", false );
    PermissionManager::setPermission( "Guest", "Object_SetOtherParentOnOtherObject", false );

    // Component
    PermissionManager::setPermission( "Guest", "ObjectManager_CreateRemoteComponent", true, 900 );
    PermissionManager::setPermission( "Guest", "ObjectManager_CreateRemoteComponentOnOwnObject", true );
    PermissionManager::setPermission( "Guest", "ObjectManager_CreateRemoteComponentOnOtherObject", false );
    PermissionManager::setPermission( "Guest", "ObjectManager_CreateLocalComponent", true );

    PermissionManager::setPermission( "Guest", "ObjectManager_DestroyOwnComponentOnOwnObject", true );
    PermissionManager::setPermission( "Guest", "ObjectManager_DestroyOwnComponentOnOtherObject", false ); // Not possible?
    PermissionManager::setPermission( "Guest", "ObjectManager_DestroyOtherComponentOnOwnObject", false ); // Not possible?
    PermissionManager::setPermission( "Guest", "ObjectManager_DestroyOtherComponentOnOtherObject", false );
    PermissionManager::setPermission( "Guest", "ObjectManager_DestroyLocalComponent", true );

    // Components
    // Node
    PermissionManager::setPermission( "Guest", "SceneNode_Create", true );

    // Entity
    PermissionManager::setPermission( "Guest", "Entity_Create", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnEntityComponent_Material", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherEntityComponent_Material", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnEntityComponent_CastShadows", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherEntityComponent_CastShadows", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnEntityComponent_Visible", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherEntityComponent_Visible", false );

    // Camera
    PermissionManager::setPermission( "Guest", "Camera_Create", true );
    PermissionManager::setPermission( "Guest", "InsertValueInOwnCameraComponent_ResourceList", true );
    PermissionManager::setPermission( "Guest", "InsertValueInOtherCameraComponent_ResourceList", false );

    // Animation
    PermissionManager::setPermission( "Guest", "Animation_Create", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnAnimationComponent_AnimationName", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherAnimationComponent_AnimationName", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnAnimationComponent_Speed", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherAnimationComponent_Speed", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnAnimationComponent_Enabled", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherAnimationComponent_Enabled", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnAnimationComponent_Loop", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherAnimationComponent_Loop", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnAnimationComponent_Position", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherAnimationComponent_Position", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnAnimationComponent_Length", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherAnimationComponent_Length", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnAnimationComponent_AutoLength", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherAnimationComponent_AutoLength", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnAnimationComponent_Weight", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherAnimationComponent_Weight", false );
    PermissionManager::setPermission( "Guest", "InsertValueInOwnAnimationComponent_ResourceList", true );
    PermissionManager::setPermission( "Guest", "InsertValueInOtherAnimationComponent_ResourceList", false );

    // Text
    PermissionManager::setPermission( "Guest", "Text_Create", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnTextComponent_Caption", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherTextComponent_Caption", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnTextComponent_Colour", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherTextComponent_Colour", false );
    PermissionManager::setPermission( "Guest", "InsertValueInOwnTextComponent_ResourceList", true );
    PermissionManager::setPermission( "Guest", "InsertValueInOtherTextComponent_ResourceList", false );

    // RigidBody
    PermissionManager::setPermission( "Guest", "RigidBody_Create", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnRigidBodyComponent_PhysicsType", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherRigidBodyComponent_PhysicsType", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnRigidBodyComponent_Mass", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherRigidBodyComponent_Mass", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnRigidBodyComponent_Friction", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherRigidBodyComponent_Friction", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnRigidBodyComponent_Restitution", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherRigidBodyComponent_Restitution", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnRigidBodyComponent_LinearDampening", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherRigidBodyComponent_LinearDampening", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnRigidBodyComponent_AngularDampening", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherRigidBodyComponent_AngularDampening", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnRigidBodyComponent_LinearSleepingThreshold", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherRigidBodyComponent_LinearSleepingThreshold", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnRigidBodyComponent_AngularSleepingThreshold", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherRigidBodyComponent_AngularSleepingThreshold", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnRigidBodyComponent_AddToWorld", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherRigidBodyComponent_AddToWorld", false );
    PermissionManager::setPermission( "Guest", "InsertValueInOwnRigidBodyComponent_ResourceList", true );
    PermissionManager::setPermission( "Guest", "InsertValueInOtherRigidBodyComponent_ResourceList", false );

    // CollisionShape
    PermissionManager::setPermission( "Guest", "CollisionShape_Create", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnCollisionShapeComponent_ShapeType", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherCollisionShapeComponent_ShapeType", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnCollisionShapeComponent_CollisionFile", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherCollisionShapeComponent_CollisionFile", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnCollisionShapeComponent_ShapeParameters", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherCollisionShapeComponent_ShapeParameters", false );

    // Audio
    PermissionManager::setPermission( "Guest", "Audio_Create", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnAudioComponent_File", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherAudioComponent_File", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnAudioComponent_Volume", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherAudioComponent_Volume", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnAudioComponent_VolumeSmooth", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherAudioComponent_VolumeSmooth", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnAudioComponent_VolumeSmoothTimescale", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherAudioComponent_VolumeSmoothTimescale", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnAudioComponent_Loop", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherAudioComponent_Loop", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnAudioComponent_AutoPlay", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherAudioComponent_AutoPlay", false );
    PermissionManager::setPermission( "Guest", "InsertValueInOwnAudioComponent_ResourceList", true );
    PermissionManager::setPermission( "Guest", "InsertValueInOtherAudioComponent_ResourceList", false );

    // LuaObjectScript
    PermissionManager::setPermission( "Guest", "LuaObjectScript_Create", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnLuaObjectScriptComponent_ClientScriptFile", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherLuaObjectScriptComponent_ClientScriptFile", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnLuaObjectScriptComponent_ClientEnvironmentName", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherLuaObjectScriptComponent_ClientEnvironmentName", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnLuaObjectScriptComponent_ClientSecurityLevel", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherLuaObjectScriptComponent_ClientSecurityLevel", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnLuaObjectScriptComponent_ServerScriptFile", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherLuaObjectScriptComponent_ServerScriptFile", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnLuaObjectScriptComponent_ServerEnvironmentName", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherLuaObjectScriptComponent_ServerEnvironmentName", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnLuaObjectScriptComponent_ServerSecurityLevel", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherLuaObjectScriptComponent_ServerSecurityLevel", false );
    PermissionManager::setPermission( "Guest", "InsertValueInOwnLuaObjectScriptComponent_ResourceList", true );
    PermissionManager::setPermission( "Guest", "InsertValueInOtherLuaObjectScriptComponent_ResourceList", false );

    // Particle
    PermissionManager::setPermission( "Guest", "Particle_Create", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnParticleComponent_Name", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherParticleComponent_Name", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnParticleComponent_Quota", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherParticleComponent_Quota", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnParticleComponent_Enabled", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherParticleComponent_Enabled", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnParticleComponent_Speed", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherParticleComponent_Speed", false );
    PermissionManager::setPermission( "Guest", "InsertValueInOwnParticleComponent_ResourceList", true );
    PermissionManager::setPermission( "Guest", "InsertValueInOtherParticleComponent_ResourceList", false );

    // Mesh
    PermissionManager::setPermission( "Guest", "Mesh_Create", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnMeshComponent_MeshFile", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherMeshComponent_MeshFile", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnMeshComponent_ShapeType", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherMeshComponent_ShapeType", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOwnMeshComponent_ShapeParameters", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnOtherMeshComponent_ShapeParameters", false );
    PermissionManager::setPermission( "Guest", "InsertValueInOwnMeshComponent_ResourceList", true );
    PermissionManager::setPermission( "Guest", "InsertValueInOtherMeshComponent_ResourceList", false );

    // Plugins
    // Server neighbors
    PermissionManager::setPermission( "Guest", "SetPropertyOnServerNeighborsPluginPlugin_ServerNeighbors", false );

    // Sky
    PermissionManager::setPermission( "Guest", "SetPropertyOnSkyPlugin_Timescale", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnSkyPlugin_FogDensity", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnSkyPlugin_FogColour", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnSkyPlugin_ObserverLongitude", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnSkyPlugin_ObserverLatitude", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnSkyPlugin_CloudSpeed", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnSkyPlugin_CloudBlendTime", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnSkyPlugin_CloudCoverage", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnSkyPlugin_CloudHeight", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnSkyPlugin_PrecipitationType", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnSkyPlugin_PrecipitationIntensity", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnSkyPlugin_PrecipitationSpeed", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnSkyPlugin_PrecipitationCameraSpeedScale", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnSkyPlugin_SunAmbientMultiplier", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnSkyPlugin_SunSpecularMultiplier", true );
    PermissionManager::setPermission( "Guest", "SetPropertyOnSkyPlugin_SunDiffuseMultiplier", true );

    // Game mode
    PermissionManager::setPermission( "Guest", "SetPropertyOnGameModePlugin_ClientLoadDefaultGamemode", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnGameModePlugin_ClientScriptFiles", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnGameModePlugin_ClientSecurityLevel", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnGameModePlugin_ServerScriptFiles", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnGameModePlugin_ServerSecurityLevel", false );

    // Terrain
    PermissionManager::setPermission( "Guest", "SetPropertyOnTerrainPlugin_TerrainType", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnTerrainPlugin_HeightmapType", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnTerrainPlugin_ClientHeightmapFile", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnTerrainPlugin_ServerHeightmapFile", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnTerrainPlugin_InputScale", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnTerrainPlugin_HeightOffset", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnTerrainPlugin_LayerInstances", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnTerrainPlugin_MinHeight0", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnTerrainPlugin_FadeDistance0", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnTerrainPlugin_MinHeight1", false );
    PermissionManager::setPermission( "Guest", "SetPropertyOnTerrainPlugin_FadeDistance1", false );

    // Resource
    PermissionManager::setPermission( "Guest", "SetPropertyOnResourceManagerPlugin_ResourceServer", false );
}

void PermissionManager::setPermission( const String& rUserName, const String& rPermissionName, 
    bool allowed, unsigned int maxItemsAllowed /*= MAXUINT*/, 
    unsigned int maxItemsPerTime /*= MAXUINT*/, double timeSeconds /*= MAXDOUBLE */ )
{
    DivAssert( mUserManager, "UserManager not set." );

    // Update permission if it already exists, add if it doesn't exist.
    User& user = mUserManager->getUser( rUserName );
    if( user.hasPermission( rPermissionName ) )
    {
        Permission& permission = user.getPermission( rPermissionName );
        permission.setAllowed( allowed );
        permission.setMaxItemsAllowed( maxItemsAllowed );
        permission.setMaxItemsPerTime( maxItemsPerTime );
        permission.setTimeSeconds( timeSeconds );
    }
    else
    {
        user.addPermission( rPermissionName, allowed, maxItemsAllowed, maxItemsPerTime, 
            timeSeconds );
    }
}

Permission& PermissionManager::getPermission( RakNet::RakNetGUID guid, const String& rName )
{
    DivAssert( mSessionManager, "SessionManager not set." );

    try
    {
        User& user = mSessionManager->getSession( guid ).getUser();
        mUserLookup[ guid ] = &user; // Store guid-user relation for later usage.
        return user.getPermission( rName );
    }
    catch ( Exception e )
    {
        // Session was not found, try RakNet::RakNetGUID->User lookup table.
        std::map<RakNet::RakNetGUID, User*>::iterator i = mUserLookup.find( guid );
        if( i != mUserLookup.end() )
        {
            return i->second->getPermission( rName );
        }
        else
        {
            throw e;
        }
    }
}

Permission::ResultType PermissionManager::checkPermission( RakNet::RakNetGUID guid, const String& rName )
{
    return getPermission( guid, rName ).checkPermission();
}

bool PermissionManager::checkPermissionAllowed( RakNet::RakNetGUID guid, const String& rName )
{
    return getPermission( guid, rName ).checkPermissionAllowed();
}

void PermissionManager::checkPermissionThrows( RakNet::RakNetGUID guid, const String& rName, 
    const String& rDesc, const String& rSrc )
{
    getPermission( guid, rName ).checkPermissionThrows( rDesc, rSrc );
}

void PermissionManager::checkPermissionThrows( RakNet::RakNetGUID guid, const String& rName, 
    const String& rSrc )
{
    getPermission( guid, rName ).checkPermissionThrows( rSrc );
}

Permission::ResultType PermissionManager::checkPermission( RakNet::RakNetGUID guid, const String& rName, 
    camp::Value& rValue )
{
    return getPermission( guid, rName ).checkPermission( rValue );
}

bool PermissionManager::checkPermissionAllowed( RakNet::RakNetGUID guid, const String& rName, 
    camp::Value& rValue )
{
    return getPermission( guid, rName ).checkPermissionAllowed( rValue );
}

void PermissionManager::checkPermissionThrows( RakNet::RakNetGUID guid, const String& rName, 
    camp::Value& rValue, const String& rDesc, const String& rSrc )
{
    getPermission( guid, rName ).checkPermissionThrows( rValue, rDesc, rSrc );
}

void PermissionManager::checkPermissionThrows( RakNet::RakNetGUID guid, const String& rName, 
    camp::Value& rValue, const String& rSrc )
{
    getPermission( guid, rName ).checkPermissionThrows( rValue, rSrc );
}

void PermissionManager::create()
{

}

void PermissionManager::SerializeConstruction( RakNet::BitStream* pConstructionBitstream, 
    RakNet::Connection_RM3* pDestinationConnection )
{
    DivAssert( mSessionManager, "SessionManager not set." );

    // Write the user's permissions.
    *pConstructionBitstream << mSessionManager->getSession( 
        pDestinationConnection->GetRakNetGUID() ).getUser().getPermissions();

    LOGD << "Writing permissions to client";
}

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia
