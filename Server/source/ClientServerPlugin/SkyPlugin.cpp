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

#include "ClientServerPlugin/SkyPlugin.h"
#include "Shared/Lua/LuaManager.h"

namespace Diversia
{
namespace Server 
{
//------------------------------------------------------------------------------

SkyPlugin::SkyPlugin( Mode mode, ClientPluginManager& rPluginManager, 
    RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
    RakNet::NetworkIDManager& rNetworkIDManager ):
    ClientPlugin( mode, rPluginManager, rRakPeer, rReplicaManager, rNetworkIDManager ),
    mTimescale( 1.0 ),
    mFogDensity( 0.0004 ),
    mFogColour( 0.7, 0.7, 0.7, 0.7 ),
    mObserverLongitude( 50.0 ),
    mObserverLatitude( 4.0 ),
    mCloudSpeed( 0.0002, 0.0002 ),
    mCloudBlendTime( 3600 * 24 ),
    mCloudCoverage( 0.3 ),
    mCloudHeight( 1500.0 ),
    mPrecipitationType( PRECTYPE_DRIZZLE ),
    mPrecipitationIntensity( 0.0 ),
    mPrecipitationSpeed( 0.25 ),
    mPrecipitationCameraSpeedScale( 0.1 ),
    mSunAmbientMultiplier( 0.5, 0.5, 0.5 ),
    mSunSpecularMultiplier( 0.2, 0.2, 0.2 ),
    mSunDiffuseMultiplier( 3, 3, 2.7 )
{
    PropertySynchronization::storeUserObject();

    Globals::mLua->object( "Sky" ) = this;
}

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia