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

#ifndef DIVERSIA_OGRECLIENT_ENTITY_H
#define DIVERSIA_OGRECLIENT_ENTITY_H

#include "OgreClient/Platform/Prerequisites.h"

#include "Client/Object/ClientComponent.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API Entity : public ClientComponent, public sigc::trackable
{
public:
    Entity( const String& rName, Mode mode, NetworkingType networkingType, 
        RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject );
    ~Entity();

    /**
    Gets the entity. 
    **/
    inline Ogre::Entity* getEntity() const { return mEntity; }
    /**
    Query if the entity has been loaded. 
    **/
    inline bool isLoaded() const { return mEntity != 0; }
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

    /**
    Sets the material name for this entity.
    
    @param  rMaterial   Name of the material. 
    **/
    void setMaterial( const String& rMaterial );

    /**
    Connects a slot to the entity loaded signal. If the entity is already loaded, the slot will
    be called immediately.
    
    @param [in,out] rSlot   The slot (signature: void func(Entity&)) to connect. 
    
    @return Connection object to block or disconnect the connection, or an empty connection when
            the entity is already loaded.
    **/
    sigc::connection connect( const sigc::slot<void, Entity&>& rSlot );
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    inline bool delayedDestruction() { return false; }
    void create();
    void meshLoaded( Mesh& rMesh );
    void componentChange( Component& rComponent, bool created );

    SceneNode&          mNode;
    ResourceManager&    mResourceManager;

    String  mMaterial;
    bool    mCreated;

    Ogre::Entity*   mEntity;

    sigc::connection            mComponentChangeConnection;
    sigc::signal<void, Entity&> mLoadedSignal;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::Entity, 
    &Diversia::OgreClient::Bindings::CampBindings::bindEntity );

#endif // DIVERSIA_OGRECLIENT_ENTITY_H