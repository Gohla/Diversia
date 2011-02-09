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

#ifndef DIVERSIA_OGRECLIENT_FADER_H
#define DIVERSIA_OGRECLIENT_FADER_H

#include "OgreClient/Platform/Prerequisites.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

enum FadeOperation
{
    FADE_NONE,
    FADE_IN,
    FADE_OUT,
    FADE_OUTIN,
};

class DIVERSIA_OGRECLIENT_API Fader : public sigc::trackable
{
public:
    /**
    Constructor. 
    
    @param  rOverlayName            Name of the overlay. 
    @param  rMaterialName           Name of the material. 
    @param [in,out] rFrameSignal    The frame signal. 
    **/
    Fader( const String& rOverlayName, const String& rMaterialName, 
        sigc::signal<void, Real>& rFrameSignal );

    /**
    Starts a fade in. 
    
    @param  duration    The duration of the fade in, in seconds. 
    **/
    void startFadeIn( Ogre::Real duration = 1.0f );
    /**
    Starts a fade out. 
    
    @param  duration    The duration of the fade out, in seconds.
    **/
    void startFadeOut( Ogre::Real duration = 1.0f );
    /**
    Sets the fade material. 
    
    @param  rName   The name of the material. 
    **/
    void setMaterial( const String& rName );
    /**
    Sets the fade overlay. 
    
    @param  rName   The name of the overlay.
    **/
    void setOverlay( const String& rName );

    /**
    Connects a slot to the fade-in/out completed signal. 
    
    @param [in,out] rSlot   The slot (signature: void func(FadeOperation [FADE_IN or FADE_OUT])) 
                            to connect. 
    
    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connect( const sigc::slot<void, FadeOperation>& rSlot ) 
    {
        return mFadeSignal.connect( rSlot );
    }

private:
    void update( Real elapsed );

    FadeOperation           mFadeOperation;
    Ogre::Real              mAlpha;
    Ogre::Real              mCurrentDuration;
    Ogre::Real              mTotalDuration;
    Ogre::TextureUnitState* mTextureUnit;
    Ogre::Overlay*          mOverlay;
    sigc::connection        mFrameConnection;

    sigc::signal<void, FadeOperation> mFadeSignal;

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

#endif // DIVERSIA_OGRECLIENT_FADE_H