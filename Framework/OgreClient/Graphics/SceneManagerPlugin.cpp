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

#include "OgreClient/Graphics/GraphicsManager.h"
#include "OgreClient/Graphics/SceneManagerPlugin.h"
#include "OgreClient/Resource/ResourceManager.h"
#include "Shared/ClientServerPlugin/ClientServerPlugin.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

SceneManagerPlugin::SceneManagerPlugin( Mode mode, PluginState state, 
    ServerPluginManager& rPluginManager, RakNet::RakPeerInterface& rRakPeer, 
    RakNet::ReplicaManager3& rReplicaManager, RakNet::NetworkIDManager& rNetworkIDManager ):
    ServerPlugin( mode, state, rPluginManager, rRakPeer, rReplicaManager, rNetworkIDManager ),
    mResourceManager( ClientServerPlugin::getPluginManager().getPlugin<ResourceManager>() ),
    mCreated( false ),
    mResourcesInitialized( false ),
    mLoaded( false ),
    mSkyType( SKYTYPE_BOX ),
    mSkyEnabled( false ),
    mSkyDistance( 5000 ),
    mSkyDrawFirst( true ),
    mSkyOrientation( Quaternion::IDENTITY ),
    mSkyDomeCurvature( 10 ),
    mSkyDomeTiling( 8 ),
    mSkyDomeXSegments( 16 ),
    mSkyDomeYSegments( 16 ),
    mSkyDomeKeepYSegments( -1 )
{
	PropertySynchronization::storeUserObject();
}

SceneManagerPlugin::~SceneManagerPlugin()
{
    GlobalsBase::mScene->setSkyDome( false, "" );
    GlobalsBase::mScene->setSkyBox( false, "" );

    Ogre::RTShader::ShaderGenerator::getSingletonPtr()->removeAllShaderBasedTechniques();
}

void SceneManagerPlugin::setSkyMaterial( const String& rMaterial )
{
    if( rMaterial.empty() || mSkyMaterial == rMaterial ) return;
    mSkyMaterial = rMaterial;

    if( mCreated && mResourcesInitialized )
    {
        SceneManagerPlugin::insertTexturesIntoResourceList();

        // TODO: Store and load properties when reloading the mesh so all the properties from the 
        // previous entity also get set onto the new entity.

        try
        {
            mLoaded = false;

            mResourceManager.loadResources( mResourceList, sigc::mem_fun( this, 
                &SceneManagerPlugin::resourcesLoaded ) );
        }
        catch( Exception e )
        {
            CLOGE << "Could not load resources for scene manager plugin: " << e.what();
        }
    }
}

void SceneManagerPlugin::create()
{
    mCreated = true;

    mResourceManager.connectInitialized( sigc::mem_fun( this, 
        &SceneManagerPlugin::resourcesInitialized ) );
}

void SceneManagerPlugin::setServerState( ServerState serverState )
{
    // TODO: Only set sky for active server.
}

void SceneManagerPlugin::resourcesInitialized( ResourceManager& rResourceManager )
{
    mResourcesInitialized = true;

    try
    {
        if( !mSkyMaterial.empty() )
        {
            SceneManagerPlugin::insertTexturesIntoResourceList();

            mResourceManager.loadResources( mResourceList, sigc::mem_fun( this, 
                &SceneManagerPlugin::resourcesLoaded ) );
        }
    }
    catch( Exception e )
    {
        CLOGE << "Could not load resources for scene manager plugin: " << e.what();
    }
}

void SceneManagerPlugin::resourcesLoaded()
{
    mLoaded = true;

    SceneManagerPlugin::updateSky();
    ServerPlugin::mLoadingCompletedSignal( *this );
}

void SceneManagerPlugin::updateSky()
{
    if( !mCreated || !mLoaded || mSkyMaterial.empty() ) return;

    try
    {
        switch( mSkyType )
        {
            case SKYTYPE_BOX:
                GlobalsBase::mScene->setSkyDome( false, "" );
                GlobalsBase::mScene->setSkyBox( mSkyEnabled, mSkyMaterial, mSkyDistance, 
                    mSkyDrawFirst, toQuaternion<Ogre::Quaternion>( mSkyOrientation ), 
                    mResourceManager.getGroup() );
                break;
            case SKYTYPE_DOME:
                GlobalsBase::mScene->setSkyBox( false, "" );
                GlobalsBase::mScene->setSkyDome( mSkyEnabled, mSkyMaterial, mSkyDomeCurvature, 
                    mSkyDomeTiling, mSkyDistance, mSkyDrawFirst, 
                    toQuaternion<Ogre::Quaternion>( mSkyOrientation ), mSkyDomeXSegments,
                    mSkyDomeYSegments, mSkyDomeKeepYSegments, mResourceManager.getGroup() );
                break;
        }
    }
    catch( Ogre::Exception e )
    {
        CLOGE << "Cannot load sky: " << e.what();
    }
}

void SceneManagerPlugin::insertTexturesIntoResourceList()
{
    // Insert texture names into resource list.
    ResourceSet textures = GraphicsManager::getTextureNamesFromMaterial( mSkyMaterial, 
        mResourceManager.getGroup() );
    mResourceList.swap( ResourceInfo::toResourceList( textures ) );
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia