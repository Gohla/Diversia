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

#include "OgreClient/Object/Light.h"
#include "OgreClient/Object/SceneNode.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

Light::Light( const String& rName, Mode mode, NetworkingType networkingType, 
    RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject ):
    ClientComponent( rName, mode, networkingType, Light::getTypeStatic(), source, localOverride, 
        rObject ),
    mNode( rObject.getComponentCast<SceneNode>( "Node" ) ),
    mLight( GlobalsBase::mScene->createLight() ),
    mType( LIGHTTYPE_POINT )
{
	PropertySynchronization::storeUserObject();

    mNode.getNode()->attachObject( mLight );
    mLight->setType( Light::getOgreLightType( mType ) );
}

Light::~Light()
{
    GlobalsBase::mScene->destroyLight( mLight );
}

Ogre::Light::LightTypes Light::getOgreLightType( LightType type )
{
    switch( type )
    {
        case LIGHTTYPE_POINT: return Ogre::Light::LT_POINT;
        case LIGHTTYPE_SPOT: return Ogre::Light::LT_SPOTLIGHT;
        case LIGHTTYPE_DIRECTIONAL: return Ogre::Light::LT_DIRECTIONAL;
    }

    return Ogre::Light::LT_POINT;
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia