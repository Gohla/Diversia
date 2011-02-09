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

#ifndef DIVERSIA_OGRECLIENT_CAMERA_H
#define DIVERSIA_OGRECLIENT_CAMERA_H

#include "OgreClient/Platform/Prerequisites.h"

#include "Client/Object/ClientComponent.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API Camera : public ClientComponent
{
public:
    Camera( const String& rName, Mode mode, NetworkingType networkingType, 
        RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject );
    ~Camera();

    /**
    Gets the entity. 
    **/
    inline Ogre::Camera* getCamera() const { return mCamera; }
    /**
    Gets the component type.
    **/
    inline ComponentType getType() const { return COMPONENTTYPE_CAMERA; }
    static inline ComponentType getTypeStatic() { return COMPONENTTYPE_CAMERA; }
    /**
    Gets the component type name.
    **/
    inline String getTypeName() const { return COMPONENTNAME_CAMERA; }
    static inline String getTypeNameStatic() { return COMPONENTNAME_CAMERA; }

    /**
    Sets the camera to active or inactive.
    
    @param  active  True to set as active, false to set as inactive.
    **/
    void setActive( bool active );
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    inline bool delayedDestruction() { return false; }
    inline void create() {}

    Ogre::Camera*   mCamera;
    bool            mActive;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::Camera, 
    &Diversia::OgreClient::Bindings::CampBindings::bindCamera );

#endif // DIVERSIA_OGRECLIENT_CAMERA_H