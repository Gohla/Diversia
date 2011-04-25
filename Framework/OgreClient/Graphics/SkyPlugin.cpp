/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.

Diversia is free software: you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation, either version 3 of the License, or (at your option) any later 
version.

Diversia is distributed in the hope that it will be useful, but WITHOUT ANY h
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with 
Diversia. If not, see <http://www.gnu.org/licenses/>.

You may contact the author of Diversia by e-mail at: equabyte@sonologic.nl
-----------------------------------------------------------------------------
*/

#include "OgreClient/Platform/StableHeaders.h"

#include "OgreClient/Graphics/SkyPlugin.h"
#include "OgreClient/Graphics/GraphicsManager.h"
#include "OgreClient/Graphics/CameraManager.h"
#include "Util/Helper/Transition.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

Caelum::CaelumSystem* SkyPlugin::mCaelumSystem = 0;
Ogre::Camera* SkyPlugin::mCamera = 0;
sigc::connection SkyPlugin::mUpdateConnection = sigc::connection();
sigc::connection SkyPlugin::mCameraConnection = sigc::connection();
unsigned int SkyPlugin::mInstanceCounter = 0;

SkyPlugin::SkyPlugin( Mode mode, PluginState state, ServerPluginManager& rPluginManager, 
    RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
    RakNet::NetworkIDManager& rNetworkIDManager ):
    ServerPlugin( mode, state, rPluginManager, rRakPeer, rReplicaManager, rNetworkIDManager )
{
    PropertySynchronization::storeUserObject();

    ++mInstanceCounter;

    // Queue all properties until server state is set to active.
    PropertySynchronization::queue();
    PropertySynchronization::queueConstruction();
}

SkyPlugin::~SkyPlugin()
{
    if( !--mInstanceCounter )
        SkyPlugin::destroy();
}

Caelum::PrecipitationType SkyPlugin::getPrecipitationType() const
{
    if( mCaelumSystem ) return mCaelumSystem->getPrecipitationController()->getPresetType();
    return Caelum::PRECTYPE_CUSTOM;
}

void SkyPlugin::setPrecipitationType( Caelum::PrecipitationType type )
{
    if( mCaelumSystem && mCaelumSystem->getPrecipitationController()->isPresetType( type ) )
        mCaelumSystem->getPrecipitationController()->setPresetType( type );
}

void SkyPlugin::create()
{
    if( mInstanceCounter == 1 )
        SkyPlugin::initialize();

    ServerPlugin::mLoadingCompletedSignal( *this );
}

void SkyPlugin::setServerState( ServerState serverState )
{
    if( serverState == CONNECTEDACTIVE )
    {
        PropertySynchronization::processQueue();
        PropertySynchronization::processQueuedConstruction();
    }
    else
    {
        // Queue all properties until server state is set to active.
        PropertySynchronization::queue();
    }
}

void SkyPlugin::update( Real elapsed )
{
    if( mCaelumSystem ) 
    {
        if( mCamera ) mCaelumSystem->notifyCameraChanged( mCamera );
        mCaelumSystem->updateSubcomponents( elapsed );
    }
}

void SkyPlugin::cameraChanged( Ogre::Camera* pCamera )
{
    mCamera = pCamera;
}

void SkyPlugin::initialize()
{
    try
    {
        // Load resources
        Ogre::ResourceGroupManager& rgm = Ogre::ResourceGroupManager::getSingleton();
        if( !rgm.resourceGroupExists( "Caelum" ) || !rgm.isResourceGroupInitialised( "Caelum" ) )
        {
            Path rootPath = GlobalsBase::mGraphics->getRootResourceLocation();
            Path caelumPath = "Caelum";
            Path resourcePath = rootPath / caelumPath;

            rgm.addResourceLocation( resourcePath.directory_string(), "FileSystem", "Caelum" );
            rgm.initialiseResourceGroup( "Caelum" );

            GlobalsBase::mGraphics->addResourceLocation( 
                OgreResourceParams( caelumPath.directory_string(), "Caelum" ) );
        }

        // Set caelum parameters.
        Caelum::CaelumSystem::CaelumComponent componentMask;
        componentMask = static_cast<Caelum::CaelumSystem::CaelumComponent>(
            Caelum::CaelumSystem::CAELUM_COMPONENT_SUN |
            Caelum::CaelumSystem::CAELUM_COMPONENT_MOON |
            Caelum::CaelumSystem::CAELUM_COMPONENT_SKY_DOME |
            Caelum::CaelumSystem::CAELUM_COMPONENT_POINT_STARFIELD |
            Caelum::CaelumSystem::CAELUM_COMPONENT_CLOUDS |
            Caelum::CaelumSystem::CAELUM_COMPONENT_PRECIPITATION |
            0
        );

        // Get camera
        mCamera = GlobalsBase::mCamera->getActiveCamera();

        // Create caelum system.
        mCaelumSystem = new Caelum::CaelumSystem( GlobalsBase::mGraphics->getRoot(), 
            GlobalsBase::mGraphics->getSceneMgr(), componentMask );
        mCaelumSystem->attachViewport( GlobalsBase::mCamera->getViewport() );

        // Set default parameters.
        mCaelumSystem->setManageSceneFog( false );
        mCaelumSystem->setSceneFogDensityMultiplier( 0.0004 );
        mCaelumSystem->setManageAmbientLight( true );
        mCaelumSystem->setEnsureSingleLightSource( true );
        mCaelumSystem->setEnsureSingleShadowSource( true );

        // Reduce specular lighting.
        mCaelumSystem->getSun()->setSpecularMultiplier( Ogre::ColourValue( .2, .2, .2 ) );

        // Configure clouds.
        Caelum::FlatCloudLayer* clouds = mCaelumSystem->getCloudSystem()->getLayer( 0 );
        if ( clouds )
        {
            clouds->setCloudSpeed( Ogre::Vector2( 0.0002, 0.0002 ) );
            clouds->setCloudBlendTime( 3600 * 24 );
            clouds->setCloudCover( 0.3 );
            clouds->setHeight( 1500 );
        }

        // Set the time scale.
        mCaelumSystem->getUniversalClock()->setTimeScale( 1.0 );

        // Set weather
        mCaelumSystem->getPrecipitationController()->setPresetType( Caelum::PRECTYPE_DRIZZLE );
        mCaelumSystem->getPrecipitationController()->setIntensity( 0.0 );
        mCaelumSystem->getPrecipitationController()->setSpeed( 0.25 );
        mCaelumSystem->getPrecipitationController()->setCameraSpeedScale( 0.1 );

        // Connect to signals.
        mUpdateConnection = GlobalsBase::mFrameSignal->connect( sigc::ptr_fun( &SkyPlugin::update ) );
        mCameraConnection = GlobalsBase::mCamera->connect( sigc::ptr_fun( &SkyPlugin::cameraChanged ) );

        // Initialize transitions
        TransitionBase<Caelum::CaelumSystem, SkyPlugin>::setUpdateSignal( *GlobalsBase::mFrameSignal );
        TransitionBase<Caelum::PrecipitationController, SkyPlugin>::setUpdateSignal( 
            *GlobalsBase::mFrameSignal );
        // TODO: Support multiple cloud layers.
        TransitionBase<Caelum::FlatCloudLayer, SkyPlugin>::setUpdateSignal( *GlobalsBase::mFrameSignal );
        TransitionBase<Caelum::BaseSkyLight, SkyPlugin>::setUpdateSignal( *GlobalsBase::mFrameSignal );
    }
    catch ( Ogre::Exception& e )
    {
        CLOGE << e.what();
    }
}

void SkyPlugin::destroy()
{
    // Stop transitions
    TransitionBase<Caelum::CaelumSystem, SkyPlugin>::stop();
    TransitionBase<Caelum::PrecipitationController, SkyPlugin>::stop();
    TransitionBase<Caelum::FlatCloudLayer, SkyPlugin>::stop();
    TransitionBase<Caelum::BaseSkyLight, SkyPlugin>::stop();

    // Stop caelum
    mCameraConnection.disconnect();
    mUpdateConnection.disconnect();
    mCaelumSystem->detachAllViewports();
    delete mCaelumSystem;
    mCaelumSystem = 0;
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia