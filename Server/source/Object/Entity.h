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

#ifndef DIVERSIA_SERVER_ENTITY_H
#define DIVERSIA_SERVER_ENTITY_H

#include "Platform/Prerequisites.h"

#include "Object/ServerComponent.h"

namespace Diversia
{
namespace Server
{
//------------------------------------------------------------------------------

class Entity : public ServerComponent
{
public:
    Entity( const String& rName, Mode mode, NetworkingType networkingType, 
        RakNet::RakNetGUID source, bool localOverride, ServerObject& rObject );
    ~Entity();

	/**
    Gets the component type.
    **/
    inline ComponentType getType() const { return COMPONENTTYPE_ENTITY; }
    static inline ComponentType getTypeStatic() { return COMPONENTTYPE_ENTITY; }
    /**
    Gets the component type name.
    **/
    inline String getTypeName() const { return COMPONENTNAME_ENTITY; }
    static inline String getTypeNameStatic() { return COMPONENTNAME_ENTITY; }
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    inline bool delayedDestruction() { return false; }
    inline void create() {}

    String  mMaterial;
    bool    mCastShadows;
    bool    mVisible;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Server::Entity, 
    &Diversia::Server::Bindings::CampBindings::bindEntity );

#endif // DIVERSIA_SERVER_ENTITY_H