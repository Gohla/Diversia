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

#ifndef DIVERSIA_OGRECLIENT_TEXTCANVASBATCH_H
#define DIVERSIA_OGRECLIENT_TEXTCANVASBATCH_H

#include "OgreClient/Platform/Prerequisites.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

// Forward declaration
class TextCanvasBatch;

/**
Represents text floating in the 3D world.
**/
class DIVERSIA_OGRECLIENT_API TextCanvas
{
public:
    TextCanvas( Ogre::MovableObject& rTarget, const String& rCaption = "", 
        const Colour& rColour = Colour::White ): 
    mTarget( &rTarget ), mCaption( rCaption ), mColour( rColour ) {}

    inline void setTarget( Ogre::MovableObject& rTarget ) { mTarget = &rTarget; }
    inline void removeTarget() { mTarget = 0; }
    inline const Ogre::MovableObject& getTarget() const { return *mTarget; }
    inline Ogre::MovableObject& getTarget() { return *mTarget; }
    inline void setCaption( const String& rCaption ) { mCaption = rCaption; }
    inline void setColour( const Colour& rColour ) { mColour = rColour; }

    String  mCaption;
    Colour  mColour;

private:
    friend class TextCanvasBatch;

    Ogre::MovableObject*    mTarget;
    Ogre::Vector3           mPosition;

};

typedef std::map<String, TextCanvas*> TextCanvasMap;

/**
The TextCanvasBatch is used to render text above various MovableObjects dynamically, in a single 
batch. Text automatically follows their target, are sized based on camera distance, and have slight 
text-shadows.
**/
class DIVERSIA_OGRECLIENT_API TextCanvasBatch : public sigc::trackable
{
public:
    /**
    Constructor. 
    
    @param [in,out] rCameraManager  The camera manager.
    @param [in,out] rAtlas          The atlas. 
    @param [in,out] rCanvas         The canvas. 
    @param [in,out] rCamera         The active camera. 
    @param  rFont                   The atlas font. 
    **/
    TextCanvasBatch( CameraManager& rCameraManager, Atlas& rAtlas, Canvas& rCanvas, 
        Ogre::Camera& rCamera, const String& rFont );
    /**
    Destructor. 
    **/
    ~TextCanvasBatch();

    /**
    Creates a text canvas.
    
    @param  rName           The name. of the text
    @param [in,out] rTarget The target that the text should follow.
    @param  rCaption        The caption of the text.
    @param  rColour         The colour of the text.
    **/
    TextCanvas& createTextCanvas( const String& rName, Ogre::MovableObject& rTarget, 
        const String& rCaption = "", const Colour& rColour = Colour::White );
    /**
    Gets a text canvas.

    @param  rName   The name of the text canvas.
    **/
    TextCanvas& getTextCanvas( const String& rName ) const;
    /**
    Gets the text map. 
    **/
    inline const TextCanvasMap& getTextCanvasMap() const { return mTextMap; }
    /**
    Query if a text canvas with given name exists.

    @param  rName   The name of the text canvas.
    **/
    bool hasTextCanvas( const String& rName ) const;
    /**
    Destroys a text canvas.

    @param  rName   The name of the text canvas.
    **/
    void destroyTextCanvas( const String& rName );
    
private:
    void update();
    inline void cameraChanged( Ogre::Camera* pCamera ) { mCamera = pCamera; }

    Atlas&          mAtlas;
    Canvas&         mCanvas;
    Ogre::Camera*   mCamera;
    String          mFont;

    TextCanvasMap mTextMap;

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE( Diversia::OgreClient::TextCanvas, 
    &Diversia::OgreClient::Bindings::CampBindings::bindTextCanvas );

#endif // DIVERSIA_OGRECLIENT_TEXTCANVASBATCH_H