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

#ifndef DIVERSIA_OGRECLIENT_AUDIO_H
#define DIVERSIA_OGRECLIENT_AUDIO_H

#include "OgreClient/Platform/Prerequisites.h"

#include "Client/Object/ClientComponent.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API Audio : public ClientComponent, public sigc::trackable
{
public:
    Audio( const String& rName, Mode mode, NetworkingType networkingType, 
        RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject );
    ~Audio();

    /**
    Gets the audio source. 
    **/
    inline cAudio::IAudioSource* getAudioSource() const { return mAudioSource; }
    /**
    Query if the audio source has been loaded. 
    **/
    inline bool isLoaded() const { return mAudioSource != 0; }
    /**
    Gets the component type.
    **/
    inline ComponentType getType() const { return COMPONENTTYPE_AUDIO; }
    static inline ComponentType getTypeStatic() { return COMPONENTTYPE_AUDIO; }
    /**
    Gets the component type name.
    **/
    inline String getTypeName() const { return COMPONENTNAME_AUDIO; }
    static inline String getTypeNameStatic() { return COMPONENTNAME_AUDIO; }

    /**
    Sets the audio file for this audio component. When executed before this component is 
    created (next tick), this will just set the audio file. If done after the component is created, 
    this will automatically reload the audio file.
    
    @param  rAudioFile  Name of the audio file
    **/
    void setAudioFile( const Path& rAudioFile );
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    void create();
    inline bool delayedDestruction() { return false; }
    void resourceLoaded( Ogre::ResourcePtr pResource );
    void transformChange( const Node& rNode );
    inline unsigned int getInstanceNumber() const { return mInstanceNumber; }
    inline Audio* getThis() { return this; }    ///< Hack for camp bindings.

    ResourceManager& mResourceManager;

    Path    mFile;
    Real    mVolumeSmoothTimescale;
    bool    mAutoPlay;

    cAudio::IAudioSource*   mAudioSource;
    unsigned int            mInstanceNumber;
    bool                    mCreated;
    sigc::connection        mTransformSignal;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::Audio, 
    &Diversia::OgreClient::Bindings::CampBindings::bindAudio );

#endif // DIVERSIA_OGRECLIENT_AUDIO_H