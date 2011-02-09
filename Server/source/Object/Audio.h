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

#ifndef DIVERSIA_SERVER_AUDIO_H
#define DIVERSIA_SERVER_AUDIO_H

#include "Platform/Prerequisites.h"

#include "Object/ServerComponent.h"

namespace Diversia
{
namespace Server
{
//------------------------------------------------------------------------------

class Audio : public ServerComponent
{
public:
    Audio( const String& rName, Mode mode, NetworkingType networkingType, 
        RakNet::RakNetGUID source, bool localOverride, ServerObject& rObject );
    ~Audio();

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
       
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    inline void create() {}
    inline bool delayedDestruction() { return false; }

    Path    mFile;
    Real    mVolume;
    Real    mVolumeSmoothTimescale;
    bool    mLoop;
    bool    mAutoPlay;
    Real    mPitch;
    Real    mMinDistance;
    Real    mMaxDistance;
    Real    mMinVolume;
    Real    mMaxVolume;
    Real    mStrength;
    Real    mDopplerStrength;
    Real    mRolloffFactor;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Server::Audio, 
    &Diversia::Server::Bindings::CampBindings::bindAudio );

#endif // DIVERSIA_SERVER_AUDIO_H