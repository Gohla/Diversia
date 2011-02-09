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

#include "Platform/Prerequisites.h"

#include <Extras/Serialize/BulletWorldImporter/btBulletWorldImporter.h>

#include "Shared/Physics/Physics.h"

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

namespace Server
{
//------------------------------------------------------------------------------

typedef std::map<Path, btCollisionShape*> CollisionShapes;

class PhysicsManager : public sigc::trackable
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
    inline PhysicsHeightfield* getPhysicsHeightfield() const { return mPhysicsHeightfield; }

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
    Creates a collision shape from a file.
    
    @param  type    The type of the collision shape.  
    @param  rFile   The file containing the collision shape. 

    @throws Exception   When file does not exist.
    @throws Exception   When a shape of given type cannot be created.
    
    @return A new collision shape, the physics manager does not own this shape so the caller must
            destroy it their selfs!
    **/
    btCollisionShape* createCollisionShape( const Path& rFile );
    /**
    Adds a rigid body to the physics world. 
    
    @param [in,out] rRigidBody  The rigid body to add.
    **/
    void addBody( btRigidBody& rRigidBody );
    /**
    Removes the body from the physics world.
    
    @param [in,out] rRigidBody  The rigid body to remove.
    **/
    void removeBody( btRigidBody& rRigidBody );

private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    void update( Real timeElapsed );
    void loadCollisionShape( const Path& rFile );
    void loadHeightfieldShape( const Path& rFile );

    btDiscreteDynamicsWorld*                mDynamicsWorld;
    btDefaultCollisionConfiguration*        mConfiguration;
    btAxisSweep3*                           mBroadphase;
    btCollisionDispatcher*                  mDispatcher;
    btSequentialImpulseConstraintSolver*    mSolver;
    btBulletWorldImporter*                  mFileLoader;
    PhysicsHeightfield*                     mPhysicsHeightfield;

    int             mCollisionShapeCounter;
    CollisionShapes mCollisionShapes;

    // Options
    unsigned int mHeightmapWorldSize;
    unsigned int mHeightmapYScale;

};

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Server::PhysicsManager, 
    &Diversia::Server::Bindings::CampBindings::bindPhysicsManager );

#endif // DIVERSIA_SERVER_PHYSICSMANAGER_H