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

#include "OgreClient/Graphics/Fader.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

Fader::Fader( const String& rOverlayName, const String& rMaterialName, 
    sigc::signal<void, Real>& rFrameSignal ):
    mFadeOperation( FADE_NONE ),
    mAlpha( 0.0 )
{
	setMaterial( rMaterialName );
	setOverlay( rOverlayName );

    mFrameConnection = rFrameSignal.connect( sigc::mem_fun( this, &Fader::update ) );
    mFrameConnection.block( true );
}

void Fader::startFadeIn( Ogre::Real duration )
{
    CLOGD << "[Fader] Fading screen in";

	if ( duration < 0 )
		duration = -duration;
	/*if ( duration < 0.000001 )
		duration = 1.0;*/

	mAlpha = 1.0;
	mTotalDuration = duration;
	mCurrentDuration = duration;
	mFadeOperation = FADE_IN;
	mOverlay->show();

    mFrameConnection.block( false );
}

void Fader::startFadeOut( Ogre::Real duration )
{
    if ( duration <= 0.000001 )
    {
        mTextureUnit->setAlphaOperation(
            Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_TEXTURE, 1.0 );
        return;
    }

    CLOGD << "[Fader] Fading screen out";

	if ( duration < 0 )
		duration = -duration;
	/*if ( duration < 0.000001 )
		duration = 1.0;*/

	mAlpha = 0.0;
	mTotalDuration = duration;
	mCurrentDuration = 0.0;
	mFadeOperation = FADE_OUT;
	mOverlay->show();

    mFrameConnection.block( false );
}

void Fader::update( Ogre::Real elapsed )
{
	if( mFadeOperation != FADE_NONE && mTextureUnit )
	{
		// If fading in, decrease the _alpha until it reaches 0.0
		if( mFadeOperation == FADE_IN )
		{
			mCurrentDuration -= elapsed;
			mAlpha = mCurrentDuration / mTotalDuration;
			if( mAlpha <= 0.0 )
			{
				mOverlay->hide();
				mFadeOperation = FADE_NONE;
                mFrameConnection.block( true );
				mFadeSignal( FADE_IN );
			}
		}

		// If fading out, increase the _alpha until it reaches 1.0
		else if( mFadeOperation == FADE_OUT )
		{
			mCurrentDuration += elapsed;
			mAlpha = mCurrentDuration / mTotalDuration;
			if( mAlpha >= 1.0 )
			{
				mFadeOperation = FADE_NONE;
                mFrameConnection.block( true );
				mFadeSignal( FADE_OUT );
			}
		}

        // Set the _alpha value of the _overlay.
        // Change the _alpha operation.
        mTextureUnit->setAlphaOperation(
            Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_TEXTURE, mAlpha );
	}
}

void Fader::setMaterial( const String& rName )
{
	// Get the material by name.
	Ogre::ResourcePtr resptr = Ogre::MaterialManager::getSingleton().getByName( rName );
	Ogre::Material* mat = dynamic_cast<Ogre::Material*>( resptr.getPointer() );
    DivAssert( mat, "Material was not found." );

	// Get the technique.
	Ogre::Technique* tech = mat->getTechnique( 0 );
	// Get the pass.
	Ogre::Pass* pass = tech->getPass( 0 );
	// Get the texture_unit state.
	mTextureUnit = pass->getTextureUnitState( 0 );
    DivAssert( mTextureUnit, "Texture unit state was not found." );
}

void Fader::setOverlay( const String& rName )
{
	// Get the _overlay.
	mOverlay = Ogre::OverlayManager::getSingleton().getByName( rName );
    DivAssert( mOverlay, "Overlay was not found." );
	mOverlay->hide();
}

} // Namespace OgreClient
} // Namespace Diversia