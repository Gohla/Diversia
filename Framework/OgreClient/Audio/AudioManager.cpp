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

#include "OgreClient/Audio/AudioManager.h"
#include "OgreClient/Audio/AudioResourceLoader.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

AudioManager::AudioManager():
    mAudioManager( 0 )
{
    GlobalsBase::mAudio = this;
}

AudioManager::~AudioManager()
{
    GlobalsBase::mAudio = 0;

    if( mAudioManager ) 
    {
        mAudioManager->releaseAllSources();
        mAudioManager->shutDown();
        cAudio::destroyAudioManager( mAudioManager );
    }
}

void AudioManager::init()
{
    mAudioManager = cAudio::createAudioManager();
}

void AudioManager::addResourceGroupLoader( const String& rResourceGroup )
{
    mAudioManager->registerDataSource( new AudioResourceLoader( rResourceGroup ), 
        rResourceGroup.c_str(), 1 );
}

void AudioManager::removeResourceGroupLoader( const String& rResourceGroup )
{
    mAudioManager->unRegisterDataSource( rResourceGroup.c_str() );
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia