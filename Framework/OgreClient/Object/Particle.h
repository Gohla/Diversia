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

#ifndef DIVERSIA_OGRECLIENT_PARTICLE_H
#define DIVERSIA_OGRECLIENT_PARTICLE_H

#include "OgreClient/Platform/Prerequisites.h"

#include "Client/Object/ClientComponent.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API Particle : public ClientComponent, public sigc::trackable
{
public:
    Particle( const String& rName, Mode mode, NetworkingType networkingType, 
        RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject );
    ~Particle();

    /**
    Gets the particle system. 
    **/
    inline Ogre::ParticleSystem* getParticleSystem() const { return mParticleSystem; }
    /**
    Query if the entity has been loaded. 
    **/
    inline bool isLoaded() const { return mParticleSystem != 0; }
    /**
    Gets the component type.
    **/
    inline ComponentType getType() const { return COMPONENTTYPE_PARTICLE; }
    static inline ComponentType getTypeStatic() { return COMPONENTTYPE_PARTICLE; }
    /**
    Gets the component type name.
    **/
    inline String getTypeName() const { return COMPONENTNAME_PARTICLE; }
    static inline String getTypeNameStatic() { return COMPONENTNAME_PARTICLE; }

    /**
    Sets the particle system name for this particle component. When executed before this component 
    is created (next tick), this will just set the particle system name. If done after the 
    component is created, this will automatically reload the particle system.
    
    @param  rParticleSystemName Name of the particle system.
    **/
    void setParticleSystemName( const String& rParticleSystemName );
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    void create();
    inline bool delayedDestruction() { return false; }
    void resourcesLoaded();

    Ogre::ParticleSystem*   mParticleSystem;
    bool                    mCreated;

    SceneNode&          mNode;
    ResourceManager&    mResourceManager;

    String mParticleSystemName;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::Particle, 
    &Diversia::OgreClient::Bindings::CampBindings::bindParticle );

#endif // DIVERSIA_OGRECLIENT_PARTICLE_H