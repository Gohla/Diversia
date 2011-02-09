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

#ifndef DIVERSIA_OGRECLIENT_COLLISIONSHAPE_H
#define DIVERSIA_OGRECLIENT_COLLISIONSHAPE_H

#include "OgreClient/Platform/Prerequisites.h"

#include "Client/Object/ClientComponent.h"
#include "Shared/Physics/Physics.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API CollisionShape : public ClientComponent, public sigc::trackable
{
public:
    CollisionShape( const String& rName, Mode mode, NetworkingType networkingType, 
        RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject );
    ~CollisionShape();

    /**
    Gets the collision shape. 
    **/
    inline btCollisionShape* getCollisionShape() const { return mCollisionShape; }
    /**
    Gets the shape type. 
    **/
    inline PhysicsShape getShapeType() const { return mShapeType; }
    /**
    Query if the collision shape has been loaded. 
    **/
    inline bool isLoaded() const { return !mCollisionShape == 0; }
    /**
    Gets the component type.
    **/
    inline ComponentType getType() const { return COMPONENTTYPE_COLLISIONSHAPE; }
    static inline ComponentType getTypeStatic() { return COMPONENTTYPE_COLLISIONSHAPE; }
    /**
    Gets the component type name.
    **/
    inline String getTypeName() const { return COMPONENTNAME_COLLISIONSHAPE; }
    static inline String getTypeNameStatic() { return COMPONENTNAME_COLLISIONSHAPE; }

    /**
    Sets the collision shape file. When executed before this component is created (next tick), 
    this will just set the collision shape file. If done after the component is created, 
    this will automatically re-create the collision shape.
    
    @param  rFile   Collision shape file.
    **/
    void setCollisionFile( const Path& rFile );
    /**
    Sets the collision shape parameters. When executed before this component is created (next tick), 
    this will just set the collision shape parameters. If done after the component is created, 
    this will automatically re-create the collision shape.
    
    @param  rParameters Collision shape parameters.
    **/
    void setShapeParameters( const Vector3& rParameters );
    /**
    Sets the collision shape type. When executed before this component is created (next tick), 
    this will just set the collision shape type. If done after the component is created, 
    this will automatically re-create the collision shape.
    
    @param  type The collision shape type
    **/
    void setShapeType( PhysicsShape type );
    /**
    Query if this object is receiving collision callbacks. Only returns true if an object is
    subscribed to the collision signal.
    **/
    inline bool isReceivingCollisionCallbacks() const { return mCollisionSignal.size() != 0; }

    /**
    Connects a slot to the collision shape loaded signal. If the collision shape is already loaded, 
    the slot will be called immediately.
    
    @param [in,out] rSlot   The slot (signature: void func(CollisionShape&)) to connect. 
    
    @return Connection object to block or disconnect the connection, or an empty connection when
            the entity is already loaded.
    **/
    sigc::connection connectLoaded( const sigc::slot<void, CollisionShape&>& rSlot );
    /**
    Connects a slot to the collision with other shape signal.
    
    @param [in,out] rSlot   The slot (signature: void func(CollisionShape&)) to connect. 
    
    @return Connection object to block or disconnect the connection, or an empty connection when
            the entity is already loaded.
    **/
    sigc::connection connectCollision( const sigc::slot<void, CollisionShape&>& rSlot );
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.
    friend class PhysicsManager;

    void create();
    inline bool delayedDestruction() { return false; }
    void resourceLoaded( Ogre::ResourcePtr pResource );
    void destroyCollisionShape();
    void createCollisionShape();
    void transformChange( const Node& rNode );
    void collisionWith( CollisionShape& rCollisionShape );

    ResourceManager& mResourceManager;
    btCollisionShape* mCollisionShape;

    PhysicsShape    mShapeType;
    Path            mCollisionFile;
    Vector3         mShapeParameters;
    bool            mShapeParametersSet;
    int             mCollisionPriority;
    btVector3       mCollisionMarginScaling;
    bool            mCreated;

    sigc::connection                    mTransformConnection;
    sigc::signal<void, CollisionShape&> mLoadedSignal;
    sigc::signal<void, CollisionShape&> mCollisionSignal;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::CollisionShape, 
    &Diversia::OgreClient::Bindings::CampBindings::bindCollisionShape );

#endif // DIVERSIA_OGRECLIENT_COLLISIONSHAPE_H