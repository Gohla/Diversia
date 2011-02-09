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

#ifndef DIVERSIA_OGRECLIENT_SOUNDMANAGER_H
#define DIVERSIA_OGRECLIENT_SOUNDMANAGER_H

#include "OgreClient/Platform/Prerequisites.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API AudioManager
{
public:
    /**
    Default constructor. 
    **/
    AudioManager();
    /**
    Destructor. 
    **/
    ~AudioManager();

    /**
    Gets the cAudio audio manager. 
    **/
    inline cAudio::IAudioManager* getAudioManager() const { return mAudioManager; }
    /**
    Gets the cAudio listener. 
    **/
    inline cAudio::IListener* getListener() const { return mAudioManager->getListener(); }
    /**
    Query if the audio manager has been loaded.
    **/
    inline bool isLoaded() const { return mAudioManager != 0; }

    /**
    Adds a resource group loader for audio sources.
    
    @param  rResourceGroup  Name of the resource group.
    **/
    void addResourceGroupLoader( const String& rResourceGroup );
    /**
    Removes a resource group loader for audio sources.
    
    @param  rResourceGroup  Name of the resource group.
    **/
    void removeResourceGroupLoader( const String& rResourceGroup );
    /**
    Initializes the audio manager. 
    **/
    void init();
    
private:
    cAudio::IAudioManager* mAudioManager;

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::AudioManager, 
    &Diversia::OgreClient::Bindings::CampBindings::bindAudioManager );

#endif // DIVERSIA_OGRECLIENT_SOUNDMANAGER_H