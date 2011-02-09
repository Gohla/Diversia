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

#include "ClientServerPlugin/Terrain.h"
#include "Object/CollisionShape.h"
#include "Object/RigidBody.h"
#include "Object/ServerObjectManager.h"
#include "Physics/PhysicsManager.h"
#include "Shared/Lua/LuaManager.h"
#include "Shared/Physics/PhysicsHeightfield.h"

namespace Diversia
{
namespace Server 
{
//------------------------------------------------------------------------------

Terrain::Terrain( Mode mode, ClientPluginManager& rPluginManager, 
    RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
    RakNet::NetworkIDManager& rNetworkIDManager ):
    ClientPlugin( mode, rPluginManager, rRakPeer, rReplicaManager, rNetworkIDManager ),
    mTerrainType( TERRAINTYPE_AUTOBLENDMAP ),
    mHeightmapType( HEIGHTMAPTYPE_RAW ),
    mClientHeightmapFile( "terrain/heightmap.raw" ),
    mServerHeightmapFile( "physics/heightmap.raw" ),
    mInputScale( 1 ),
    mHeightOffset( 0 ),
    mTerrain( 0 ),
    mMinHeight0( 50 ),
    mFadeDistance0( 50 ),
    mMinHeight1( 75 ),
    mFadeDistance1( 50 )
{
    PropertySynchronization::storeUserObject();

    mLayerInstances.resize( 3 );
    mLayerInstances[0].mWorldSize = 25;
    mLayerInstances[0].mTextureFiles.push_back( "terrain/grass_green-01_diffusespecular.dds" );
    mLayerInstances[0].mTextureFiles.push_back( "terrain/grass_green-01_normalheight.dds" );
    mLayerInstances[1].mWorldSize = 200;
    mLayerInstances[1].mTextureFiles.push_back( "terrain/growth_weirdfungus-03_diffusespecular.dds" );
    mLayerInstances[1].mTextureFiles.push_back( "terrain/growth_weirdfungus-03_normalheight.dds" );
    mLayerInstances[2].mWorldSize = 40;
    mLayerInstances[2].mTextureFiles.push_back( "terrain/dirt_grayrocky_diffusespecular.dds" );
    mLayerInstances[2].mTextureFiles.push_back( "terrain/dirt_grayrocky_normalheight.dds" );

    Globals::mLua->object( "Terrain" ) = this;
}

Terrain::~Terrain()
{
    if( mTerrain ) mTerrain->destroyObject();
}

void Terrain::create()
{
    // Create heightmap
    mHeightOffset = Globals::mPhysics->getPhysicsHeightfield()->mOffset;

    mTerrain = &ClientPlugin::getPluginManager().getPlugin<ServerObjectManager>().createObject(
        "Heightmap", LOCAL );
    mTerrain->setPosition( Vector3( 0, mHeightOffset / 2 + 4.75, 0 ) );

    CollisionShape& collisionShape = mTerrain->createComponent<CollisionShape>( "Collision", true );
    collisionShape.set( "ShapeType", PHYSICSSHAPE_HEIGHTMAPFILE );
    collisionShape.set( "CollisionFile", mServerHeightmapFile );

    RigidBody& rigidBody = mTerrain->createComponent<RigidBody>( "Physics", true );
    rigidBody.set( "PhysicsType", PHYSICSTYPE_STATIC );
}

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia