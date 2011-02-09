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

#ifndef DIVERSIA_OGRECLIENT_CAMERAMANAGER_H
#define DIVERSIA_OGRECLIENT_CAMERAMANAGER_H

#include "OgreClient/Platform/Prerequisites.h"

#include "Shared/Communication/GridPosition.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

typedef std::map<GridPosition, Ogre::Camera*> ActiveCameras;

class DIVERSIA_OGRECLIENT_API CameraManager : public sigc::trackable
{
public:
    /**
    Default constructor. 
    **/
    CameraManager();
    /**
    Destructor. 
    **/
    ~CameraManager();

    /**
    Gets the default camera. 
    **/
    inline Ogre::Camera* getDefaultCamera() const { return mDefaultCamera; }
    /**
    Gets the active camera. 
    **/
    inline Ogre::Camera* getActiveCamera() const { return mActiveCamera; }
    /**
    Gets the viewport. 
    **/
    inline Ogre::Viewport* getViewport() const { return mViewport; }
    /**
    Gets the canvas. 
    **/
    inline Canvas* getCanvas() const { return mCanvas; }
    /**
    Gets the text canvas batch. 
    **/
    inline TextCanvasBatch* getTextCanvasBatch() const { return mTextCanvasBatch; }

    /**
    Sets the server grid manager. 
    **/
    void setGridManager( GridManager& rGridManager );
    /**
    Sets the active camera.
    **/
    void setActiveCamera( Ogre::Camera* pCamera, ServerAbstract& rServer );
    /**
    Unsets an active camera. 
    **/
    void unsetActiveCamera( Ogre::Camera* pCamera, ServerAbstract& rServer );

    /**
    Connects a slot to the camera changed signal.
    
    @param [in,out] rSlot   The slot (signature: void func(Ogre::Camera*)) to connect. 
    
    @return Connection object to block or disconnect the connection.
    **/
    sigc::connection connect( const sigc::slot<void, Ogre::Camera*>& rSlot ) 
    {
        return mCameraSignal.connect( rSlot );
    }
    
private:
    void setActiveCamera( Ogre::Camera* pCamera );
    void serverChange( ServerAbstract& rServer, bool created );
    void windowResized( unsigned int width, unsigned int height );
    void update();

    GridManager*        mGridManager;
    Ogre::Camera*       mDefaultCamera;
    Ogre::Camera*       mActiveCamera;
    Ogre::Viewport*     mViewport;
    Canvas*             mCanvas;
    cAudio::IListener*  mAudioListener;

    ActiveCameras       mActiveCameras;

    sigc::signal<void, Ogre::Camera*> mCameraSignal;

    TextCanvasBatch*    mTextCanvasBatch;	///< Needs mCameraSignal. 
};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

#endif // DIVERSIA_OGRECLIENT_CAMERAMANAGER_H