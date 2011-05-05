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

#ifndef DIVERSIA_OGRECLIENT_LIGHT_H
#define DIVERSIA_OGRECLIENT_LIGHT_H

#include "OgreClient/Platform/Prerequisites.h"

#include "Client/Object/ClientComponent.h"
#include "Shared/Graphics/Graphics.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API Light : public ClientComponent
{
public:
    Light( const String& rName, Mode mode, NetworkingType networkingType, 
        RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject );
    ~Light();

    /**
    Gets the light. 
    **/
    inline Ogre::Light* getLight() const { return mLight; }
    /**
    Query if the light has been loaded. 
    **/
    inline bool isLoaded() const { return mLight != 0; }
    /**
    Gets the component type.
    **/
    inline ComponentType getType() const { return COMPONENTTYPE_LIGHT; }
    static inline ComponentType getTypeStatic() { return COMPONENTTYPE_LIGHT; }
    /**
    Gets the component type name.
    **/
    inline String getTypeName() const { return COMPONENTNAME_LIGHT; }
    static inline String getTypeNameStatic() { return COMPONENTNAME_LIGHT; }

    /**
    Sets the light type.
    **/
    inline void setType( LightType type ) { mType = type; if( mLight ) mLight->setType( Light::getOgreLightType( type ) ); }
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    inline void create() { }
    inline bool delayedDestruction() { return false; }
    Ogre::Light::LightTypes getOgreLightType( LightType type );

    SceneNode& mNode;
    Ogre::Light* mLight;
    
    LightType mType;

	CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::Light, 
    &Diversia::OgreClient::Bindings::CampBindings::bindLight );

#endif // DIVERSIA_OGRECLIENT_LIGHT_H