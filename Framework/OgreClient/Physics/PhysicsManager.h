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

#ifndef DIVERSIA_OGRECLIENT_PHYSICSMANAGER_H
#define DIVERSIA_OGRECLIENT_PHYSICSMANAGER_H

#include "OgreClient/Platform/Prerequisites.h"

#include "Shared/Physics/Physics.h"
#include "BtOgre/BtOgreExtras.h"

#include <Extras/Serialize/BulletWorldImporter/btBulletWorldImporter.h>

namespace Diversia
{

// Specialize toQuaternion and toVector for btQuaternion and btVector3.
namespace Util
{
//------------------------------------------------------------------------------

template <> inline btQuaternion toQuaternion<btQuaternion, Quaternion>( const Quaternion& q )
{
    return btQuaternion( q.x, q.y, q.z, q.w );
}

template <> inline Quaternion toQuaternion<Quaternion, btQuaternion>( const btQuaternion& q )
{
    return Quaternion( q.w(), q.x(), q.y(), q.z() );
}

template <> inline Vector3 toVector3<Vector3, btVector3>( const btVector3& v )
{
    return Vector3( v.x(), v.y(), v.z() );
}

//------------------------------------------------------------------------------
} // Namespace Util

namespace OgreClient
{
//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API PhysicsManager : public sigc::trackable
{
public:
    /**
    Default constructor. 
    **/
    PhysicsManager();
    /**
    Destructor. 
    **/
    ~PhysicsManager();

    /**
    Initializes the physics.
    **/
    void init();

    inline btDiscreteDynamicsWorld* getDynamicsWorld() const { return mDynamicsWorld; }
    inline btDefaultCollisionConfiguration* getCollisionConfiguration() const { return mConfiguration; }
    inline btAxisSweep3* getBroadPhase() const { return mBroadphase; }
    inline btCollisionDispatcher* getCollisionDispatcher() const { return mDispatcher; }
    inline btSequentialImpulseConstraintSolver* getConstraintSolver() const { return mSolver; }

    /**
    Copies given collision shape. 
    
    @param [in,out] pShape  The collision shape.
    **/
    btCollisionShape* copyCollisionShape( btCollisionShape* pShape );
    /**
    Creates a collision shape from parameters.
    
    @param  type        The type of the collision shape. 
    @param  rParameters The parameters of the shape. 

    @throws Exception   When a shape of given type cannot be created.
    
    @return A new collision shape, the physics manager does not own this shape so the caller must
            destroy it their selfs!
    **/
    btCollisionShape* createCollisionShape( PhysicsShape type, const btVector3& rParameters );
    /**
    Creates a collision shape from memory. Make a copy of this collision shape using 
    copyCollisionShape if you plan to edit the shape!
    
    @param  pData   The collision shape data in memory.
    @param  size    The size of the data in memory.

    @throws Exception   When a shape of given type cannot be created.
    
    @return A new collision shape, the physics manager does not own this shape so the caller must
            destroy it their selfs!
    **/
    btCollisionShape* createCollisionShape( char* pData, size_t size );
    /**
    Adds a rigid body to the physics world. 
    **/
    void addBody( btRigidBody& rRigidBody );
    /**
    Removes the body from the physics world.
    **/
    void removeBody( btRigidBody& rRigidBody );
    /**
    Adds a collision object to the physics world. 
    **/
    void addCollisionObject( btCollisionObject& rObject );
    /**
    Removes the collision object from the physics world.
    **/
    void removeCollisionObject( btCollisionObject& rObject );
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    void update( Real timeElapsed );
    void updateDebug();

    btDiscreteDynamicsWorld*                mDynamicsWorld;
    btDefaultCollisionConfiguration*        mConfiguration;
    btAxisSweep3*                           mBroadphase;
    btCollisionDispatcher*                  mDispatcher;
    btSequentialImpulseConstraintSolver*    mSolver;
    BtOgre::DebugDrawer*                    mDebugDrawer;
    btBulletWorldImporter*                  mFileLoader;
    int                                     mCollisionShapeCounter;
    AreaTriggerCallback<AreaTrigger>*       mAreaTriggerCallback;

    // Options
    bool mDebugDraw;
};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::PhysicsManager, 
    &Diversia::OgreClient::Bindings::CampBindings::bindPhysicsManager );

#endif // DIVERSIA_OGRECLIENT_PHYSICSMANAGER_H