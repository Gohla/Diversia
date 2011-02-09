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

#include "OgreClient/Graphics/SceneManagerPlugin.h"
#include "OgreClient/Resource/ResourceManager.h"
#include "Shared/ClientServerPlugin/ClientServerPlugin.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

SceneManagerPlugin::SceneManagerPlugin( Mode mode, ServerPluginManager& rPluginManager, 
    RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
    RakNet::NetworkIDManager& rNetworkIDManager ):
    ServerPlugin( mode, rPluginManager, rRakPeer, rReplicaManager, rNetworkIDManager ),
    mResourceManager( ClientServerPlugin::getPluginManager().getPlugin<ResourceManager>() ),
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

}

void SceneManagerPlugin::create()
{
    SceneManagerPlugin::updateSky();
    ServerPlugin::mLoadingCompletedSignal( *this );
}

void SceneManagerPlugin::setServerState( ServerState serverState )
{
    
}

void SceneManagerPlugin::updateSky()
{
    try
    {
        switch( mSkyType )
        {
            case SKYTYPE_BOX:
                GlobalsBase::mScene->setSkyDome( false, mSkyMaterial );
                GlobalsBase::mScene->setSkyBox( mSkyEnabled, mSkyMaterial, mSkyDistance, 
                    mSkyDrawFirst, toQuaternion<Ogre::Quaternion>( mSkyOrientation ), 
                    mResourceManager.getGroup() );
                break;
            case SKYTYPE_DOME:
                GlobalsBase::mScene->setSkyBox( false, mSkyMaterial );
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

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia