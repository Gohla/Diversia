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

#include "OgreClient/Object/Camera.h"
#include "OgreClient/Object/SceneNode.h"
#include "OgreClient/Graphics/QueryFlags.h"
#include "OgreClient/Graphics/CameraManager.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

Camera::Camera( const String& rName, Mode mode, NetworkingType networkingType, 
    RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject ):
    ClientComponent( rName, mode, networkingType, Camera::getTypeStatic(), source, localOverride, 
        rObject ),
    mCamera( 0 ),
    mActive( false )
{
    PropertySynchronization::storeUserObject();

    mCamera = GlobalsBase::mScene->createCamera( Component::getGUIDString() );
    mCamera->setNearClipDistance( 0.1 );
    mCamera->setAutoAspectRatio( true );
    ClientComponent::getObject().getComponent<SceneNode>().getNode()->attachObject( mCamera );

    mCamera->MovableObject::setUserAny( Ogre::Any( &ClientComponent::getClientObject() ) );
    mCamera->setQueryFlags( QueryFlags_Camera );
}

Camera::~Camera()
{
    if( mActive )
        Camera::setActive( false );

    if( mCamera ) GlobalsBase::mScene->destroyCamera( mCamera );
}

void Camera::setActive( bool active )
{
    if( mActive != active && mCamera )
    {
        if( active )
            GlobalsBase::mCamera->setActiveCamera( mCamera, ClientComponent::getServer() );
        else
            GlobalsBase::mCamera->unsetActiveCamera( mCamera, ClientComponent::getServer() );

        mActive = active;
    }
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia