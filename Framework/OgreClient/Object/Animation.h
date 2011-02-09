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

#ifndef DIVERSIA_OGRECLIENT_ANIMATION_H
#define DIVERSIA_OGRECLIENT_ANIMATION_H

#include "OgreClient/Platform/Prerequisites.h"

#include "Client/Object/ClientComponent.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

typedef std::map<String, Ogre::AnimationState*> AnimationStates;

class DIVERSIA_OGRECLIENT_API Animation : public ClientComponent, public sigc::trackable
{
public:
    Animation( const String& rName, Mode mode, NetworkingType networkingType, 
        RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject );
    ~Animation();

    /**
    Gets the animation state
    **/
    inline Ogre::AnimationState* getAnimationState() const { return mAnimationState; }
    /**
    Query if the animation state has been loaded. 
    **/
    inline bool isLoaded() const { return mAnimationState != 0; }
    /**
    Gets the component type.
    **/
    inline ComponentType getType() const { return COMPONENTTYPE_ANIMATION; }
    static inline ComponentType getTypeStatic() { return COMPONENTTYPE_ANIMATION; }
    /**
    Gets the component type name.
    **/
    inline String getTypeName() const { return COMPONENTNAME_ANIMATION; }
    static inline String getTypeNameStatic() { return COMPONENTNAME_ANIMATION; }

    /**
    Sets the animation name. When executed before this component is created (next tick), 
    this will just set the animation name. If done after the component is created, this will 
    automatically load the animation.
    
    @param  rAnimationName  Name of the animation. 
    **/
    void setAnimationName( const String& rAnimationName );
    /**
    Sets the length of the animation.
    **/
    void setLength( Real length );
    /**
    Gets the length of the animation.
    **/
    Real getLength();
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    void create();
    inline bool delayedDestruction() { return false; }
    void update( Real timeElapsed );
    void resourcesLoaded( Entity& rEntity );
    void componentChange( Component& rComponent, bool created );

    String  mAnimationName;
    Real    mSpeed;
    bool    mAutoLength;

    Ogre::AnimationState*   mAnimationState;
    bool                    mCreated;
    sigc::connection        mUpdateSignal;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::Animation, 
    &Diversia::OgreClient::Bindings::CampBindings::bindAnimation );

#endif // DIVERSIA_OGRECLIENT_ANIMATION_H