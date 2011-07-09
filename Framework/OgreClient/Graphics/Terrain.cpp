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

#include "OgreClient/Platform/StableHeaders.h"

#include "Client/Plugin/ClientPluginManager.h"
#include "Client/Communication/GridManager.h"
#include "Client/Communication/Server.h"
#include "OgreClient/Graphics/GraphicsManager.h"
#include "OgreClient/Graphics/Terrain.h"
#include "OgreClient/Resource/ResourceManager.h"
#include "Shared/Communication/ServerPosition.h"
#include <Ogre/Terrain/OgreTerrainMaterialGeneratorA.h>

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

Ogre::TerrainGlobalOptions* Terrain::mTerrainGlobals = 0;

Terrain::Terrain( Mode mode, PluginState state, ClientPluginManager& rPluginManager, 
    RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
    RakNet::NetworkIDManager& rNetworkIDManager ):
    ClientPlugin( mode, state, rPluginManager, rRakPeer, rReplicaManager, rNetworkIDManager ),
    mTerrain( 0 ),
    mResourceManager( Plugin::getPluginManager().getPlugin<ResourceManager>() ),
    mTerrainType( TERRAINTYPE_AUTOBLENDMAP ),
    mHeightmapType( HEIGHTMAPTYPE_RAW ),
    mHeightmapFile( "terrain/heightmap.raw" ),
    mInputScale( 1 ),
    mHeightOffset( 0 ),
    mMinHeight0( 50 ),
    mFadeDistance0( 50 ),
    mMinHeight1( 75 ),
    mFadeDistance1( 50 )
{
    PropertySynchronization::storeUserObject();

    // Set global terrain options once
    if( !mTerrainGlobals )
    {
        mTerrainGlobals = new Ogre::TerrainGlobalOptions();

        mTerrainGlobals->setMaxPixelError( 4 );
        static_cast<Ogre::TerrainMaterialGeneratorA::SM2Profile*>( 
            mTerrainGlobals->getDefaultMaterialGenerator()->
            getActiveProfile() )->setLightmapEnabled( false );
        static_cast<Ogre::TerrainMaterialGeneratorA::SM2Profile*>(
            mTerrainGlobals->getDefaultMaterialGenerator()->
            getActiveProfile() )->setLayerNormalMappingEnabled( true );
        static_cast<Ogre::TerrainMaterialGeneratorA::SM2Profile*>(
            mTerrainGlobals->getDefaultMaterialGenerator()->
            getActiveProfile() )->setLayerParallaxMappingEnabled( false );
        static_cast<Ogre::TerrainMaterialGeneratorA::SM2Profile*>(
            mTerrainGlobals->getDefaultMaterialGenerator()->
            getActiveProfile() )->setLayerSpecularMappingEnabled( true );
    }
}

Terrain::~Terrain()
{
    Terrain::reset();
}

Real Terrain::getHeightAt( const Vector3& rPosition )
{
    try
    {
        return GlobalsBase::mGrid->getServer( GridPosition( ServerPosition( rPosition ) ) ).
            getPluginManager().getPlugin<Terrain>().getTerrain()->getHeightAtWorldPosition( 
            toVector3<Ogre::Vector3>( rPosition ) );
    }
    catch( ... )
    {
    }

    return 0.0;
}

void Terrain::create()
{
    // Load terrain resources.
    ResourceList resourceList;

    // Heightmap
    resourceList.push_back( ResourceInfo( mHeightmapFile ) );

    // Textures
    for( LayerInstances::iterator i = mLayerInstances.begin(); i != mLayerInstances.end(); ++i )
    {
        for( std::vector<Path>::iterator j = (*i).mTextureFiles.begin(); j != (*i).mTextureFiles.end(); ++j )
        {
            resourceList.push_back( ResourceInfo( *j ) );
        }
    }

    mResourceManager.loadResources( resourceList, sigc::mem_fun( this, &Terrain::resourcesLoaded ) );
}

void Terrain::reset()
{
    if( mTerrain )
    {
        /*Path terrainFile = Globals::mGraphics->getRootResourceLocation() / "Ogre" / "TerrainCache" /
            mResourceManager.getGroup();
        boost::filesystem::create_directories( terrainFile.string() );
        mTerrain->save( ( terrainFile / "terrain.bin" ).string() );*/

        mTerrain->unload();
        mTerrain->unprepare();
        OGRE_DELETE mTerrain;
    }
}

void Terrain::resourcesLoaded()
{
    mTerrain = OGRE_NEW Ogre::Terrain( GlobalsBase::mScene );

    Ogre::Terrain::ImportData imp;
    Ogre::Image img;

    switch( mHeightmapType )
    {
        case HEIGHTMAPTYPE_IMAGE:
        {
            Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().getByName( 
                mHeightmapFile.string(), mResourceManager.getGroup() );
            texture->convertToImage( img );
            imp.inputImage = &img;
            imp.terrainSize = img.getWidth();
            imp.inputScale = mInputScale;
            break;
        }
        case HEIGHTMAPTYPE_RAW:
        {
            Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource( 
                mHeightmapFile.string(), mResourceManager.getGroup() );
            img.loadRawData( stream, 1025, 1025, Ogre::PF_FLOAT32_R );
            imp.inputImage = &img;
            imp.terrainSize = img.getWidth();
            imp.inputScale = 1;
            break;
        }
        case HEIGHTMAPTYPE_OGREBINARY:
            break;
    }

    if( mTerrainType != TERRAINTYPE_OGREBINARY )
    {
        // Generic import data
        imp.worldSize = 1025;
        imp.minBatchSize = 33;
        imp.maxBatchSize = 65;
        imp.pos = toVector3<Ogre::Vector3>( 
            ServerPosition( ClientPlugin::getServer().getGridPosition() ).get3DPosition() );
        mTerrain->setPosition( imp.pos );

        // Set textures
        imp.layerList.resize( mLayerInstances.size() );
        for( unsigned int i = 0; i < mLayerInstances.size(); ++i )
        {
            imp.layerList[i].worldSize = mLayerInstances[i].mWorldSize;
            for( std::vector<Path>::iterator j = mLayerInstances[i].mTextureFiles.begin(); 
                j != mLayerInstances[i].mTextureFiles.end(); ++j )
            {
                imp.layerList[i].textureNames.push_back( (*j).string() );
            }
        }

        // Load the terrain
        mTerrain->prepare( imp );
        mTerrain->load();
    }

    // Set blending
    switch( mTerrainType )
    {
        case TERRAINTYPE_AUTOBLENDMAP:
        {
            Ogre::TerrainLayerBlendMap* blendMap0 = mTerrain->getLayerBlendMap(1);
            Ogre::TerrainLayerBlendMap* blendMap1 = mTerrain->getLayerBlendMap(2);
            float* pBlend1 = blendMap1->getBlendPointer();
            for (Ogre::uint16 y = 0; y < mTerrain->getLayerBlendMapSize(); ++y)
            {
                for (Ogre::uint16 x = 0; x < mTerrain->getLayerBlendMapSize(); ++x)
                {
                    Ogre::Real tx, ty;

                    blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
                    Ogre::Real height = mTerrain->getHeightAtTerrainPosition(tx, ty);
                    Ogre::Real val = (height - mMinHeight0) / mFadeDistance0;
                    val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);

                    val = (height - mMinHeight1) / mFadeDistance1;
                    val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
                    *pBlend1++ = val;
                }
            }

            blendMap0->dirty();
            blendMap1->dirty();
            blendMap0->update();
            blendMap1->update();
            break;
        }
        case TERRAINTYPE_OGREBINARY:
            // Everything is embedded in the binary file.
            mTerrain->setResourceGroup( mResourceManager.getGroup() );
            mTerrain->prepare( mHeightmapFile.string() );
            mTerrain->load( mHeightmapFile.string() );
            break;
    }

    mTerrain->freeTemporaryResources();

    ClientPlugin::mLoadingCompletedSignal( *this );
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia