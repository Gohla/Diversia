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

#ifndef DIVERSIA_OGRECLIENT_AREATRIGGER_H
#define DIVERSIA_OGRECLIENT_AREATRIGGER_H

#include "OgreClient/Platform/Prerequisites.h"

#include "Client/Object/ClientComponent.h"
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API AreaTrigger : public ClientComponent, public sigc::trackable
{
public:
    AreaTrigger( const String& rName, Mode mode, NetworkingType networkingType, 
        RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject );
    ~AreaTrigger();

    /**
    Gets the ghost object
    **/
    inline btGhostObject* getGhostObject() const { return mGhostObject; }
    /**
    Query if the area trigger is loaded.
    **/
    inline bool isLoaded() const { return mGhostObject != 0; }
    /**
    Gets the component type.
    **/
    inline ComponentType getType() const { return COMPONENTTYPE_AREATRIGGER; }
    static inline ComponentType getTypeStatic() { return COMPONENTTYPE_AREATRIGGER; }
    /**
    Gets the component type name.
    **/
    inline String getTypeName() const { return COMPONENTNAME_AREATRIGGER; }
    static inline String getTypeNameStatic() { return COMPONENTNAME_AREATRIGGER; }

    /**
    Connects a slot to the area change (object entered or left area) event.
    
    @param [in,out] rSlot   The slot (signature: void func(Object&, bool)) to connect. 
    
    @return Connection object to block or disconnect the connection, or an empty connection when
            the entity is already loaded.
    **/
    sigc::connection connectAreaChange( const sigc::slot<void, Object&, bool>& rSlot );
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.
    friend class AreaTriggerCallback<AreaTrigger>;

    void create();
    inline bool delayedDestruction() { return false; }
    void destroyGhostObject();
    void collisionShapeLoaded( CollisionShape& rCollisionShape );
    void componentChange( Component& rComponent, bool created );
    void enterArea( Object& rObject );
    void leaveArea( Object& rObject );
    void transformChange( const Node& rNode );

    btGhostObject*      mGhostObject;
    btCollisionShape*   mCollisionShape;

    sigc::connection                    mTransformConnection;
    sigc::signal<void, Object&, bool>   mAreaTriggerSignal;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::AreaTrigger, 
    &Diversia::OgreClient::Bindings::CampBindings::bindAreaTrigger );

#endif // DIVERSIA_OGRECLIENT_AREATRIGGER_H