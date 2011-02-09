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

#include "OgreClient/Graphics/CameraManager.h"
#include "OgreClient/Graphics/GraphicsManager.h"
#include "OgreClient/Graphics/Canvas.h"
#include "OgreClient/Graphics/TextCanvasBatch.h"
#include "Client/Communication/GridManager.h"
#include "Client/Communication/ServerAbstract.h"
#include "OgreClient/Audio/AudioManager.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

CameraManager::CameraManager():
    mGridManager( 0 ),
    mDefaultCamera( GlobalsBase::mGraphics->getSceneMgr()->createCamera( "DefaultCamera" ) ),
    mActiveCamera( mDefaultCamera ),
    mViewport( GlobalsBase::mGraphics->getWindow()->addViewport( mDefaultCamera ) ),
    mCanvas( new Canvas( GlobalsBase::mGraphics->getAtlas(), mViewport ) ),
    mAudioListener( GlobalsBase::mAudio->getListener() )
{
    mTextCanvasBatch = new TextCanvasBatch( *this, *GlobalsBase::mGraphics->getAtlas(), *mCanvas, 
        *mDefaultCamera, GlobalsBase::mGraphics->getAtlasFont() );

    mDefaultCamera->setNearClipDistance( 0.1 );
    mDefaultCamera->setAutoAspectRatio( true );

    GlobalsBase::mGraphics->getSceneMgr()->getRootSceneNode()->createChildSceneNode()->attachObject( 
        mCanvas );

    GlobalsBase::mLateUpdateSignal->connect( sigc::mem_fun( this, &CameraManager::update ) );
    GlobalsBase::mGraphics->connectWindowResized( sigc::mem_fun( this, 
        &CameraManager::windowResized ) );
    GlobalsBase::mCamera = this;
}

CameraManager::~CameraManager()
{
    GlobalsBase::mCamera = 0;

    delete mTextCanvasBatch;
    delete mCanvas;
}

void CameraManager::setGridManager( GridManager& rGridManager )
{
    mGridManager = &rGridManager;
    mGridManager->connectServerChange( sigc::mem_fun( this, &CameraManager::serverChange ) );

    // TODO: Check if camera must be changed if active server has changed.
}

void CameraManager::setActiveCamera( Ogre::Camera* pCamera, ServerAbstract& rServer )
{
    DivAssert( mGridManager, "Grid manager has not been set yet." );
    try
    {
        if( &rServer == &mGridManager->getActiveServer() )
        {
            CameraManager::setActiveCamera( pCamera );
        }
    }
    catch( Exception e )
    {
        // Ignore error.
    }

    mActiveCameras[ rServer.getGridPosition() ] = pCamera;
}

void CameraManager::unsetActiveCamera( Ogre::Camera* pCamera, ServerAbstract& rServer )
{
    DivAssert( mGridManager, "Grid manager has not been set yet." );
    try
    {
        if( &rServer == &mGridManager->getActiveServer() )
        {
            CameraManager::setActiveCamera( mDefaultCamera );
        }
    }
    catch( Exception e )
    {
    	// There is no active server, set camera to default camera as well.
        CameraManager::setActiveCamera( mDefaultCamera );
    }

    mActiveCameras[ rServer.getGridPosition() ] = mDefaultCamera;
}

void CameraManager::setActiveCamera( Ogre::Camera* pCamera )
{
    mViewport->setCamera( pCamera );
    mActiveCamera = pCamera;
    mCameraSignal( pCamera );
}

void CameraManager::serverChange( ServerAbstract& rServer, bool created )
{
    if( created )
        mActiveCameras.insert( std::make_pair( rServer.getGridPosition(), mDefaultCamera ) );
    else
        mActiveCameras.erase( rServer.getGridPosition() );
}

void CameraManager::windowResized( unsigned int width, unsigned int height )
{
    mCanvas->clearClip();
}

void CameraManager::update()
{
    if( mActiveCamera )
    {
        mAudioListener->setPosition( toVector3<cAudio::cVector3>( 
            mActiveCamera->getDerivedPosition() ) );
        mAudioListener->setDirection( toVector3<cAudio::cVector3>( 
            mActiveCamera->getDerivedDirection() ) );
    }
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia