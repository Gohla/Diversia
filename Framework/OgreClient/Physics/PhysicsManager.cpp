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

#include "OgreClient/Platform/StableHeaders.h"

#include "OgreClient/Object/AreaTrigger.h"
#include "OgreClient/Object/CollisionShape.h"
#include "OgreClient/Physics/PhysicsManager.h"
#include "Shared/Physics/AreaTriggerCallback.h"
#include "Shared/Physics/PhysicsHeightfield.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

PhysicsManager::PhysicsManager():
    mCollisionShapeCounter( 0 ),
    mDebugDraw( false )
{
    GlobalsBase::mPhysics = this;
}

PhysicsManager::~PhysicsManager()
{
    GlobalsBase::mPhysics = 0;
    GlobalsBase::mBroadphase = 0;
    GlobalsBase::mWorld = 0;

    delete mFileLoader;
    if( mDebugDraw ) delete mDebugDrawer;
    delete mDynamicsWorld;
    delete mSolver;
    delete mDispatcher;
    delete mConfiguration;
    delete mBroadphase;
}

void PhysicsManager::init()
{
    CLOGI << "Initializing physics";

    // Initialize bullet
    mBroadphase = new btAxisSweep3( btVector3( -DIVERSIA_SERVER_SIZE, -DIVERSIA_SERVER_SIZE,
        -DIVERSIA_SERVER_SIZE ), btVector3( DIVERSIA_SERVER_SIZE, DIVERSIA_SERVER_SIZE,
        DIVERSIA_SERVER_SIZE ) );

    mConfiguration = new btDefaultCollisionConfiguration();
    mDispatcher = new btCollisionDispatcher( mConfiguration );
    mSolver = new btSequentialImpulseConstraintSolver();
    mDynamicsWorld = new btDiscreteDynamicsWorld( mDispatcher, mBroadphase, mSolver,
        mConfiguration );
    mAreaTriggerCallback = new AreaTriggerCallback<AreaTrigger>( mDynamicsWorld );

    if( mDebugDraw )
    {
        mDebugDrawer = new BtOgre::DebugDrawer( 
            GlobalsBase::mScene->getRootSceneNode(), mDynamicsWorld );
        mDynamicsWorld->setDebugDrawer( mDebugDrawer );
    }

    mFileLoader = new btBulletWorldImporter( mDynamicsWorld );

    GlobalsBase::mWorld = mDynamicsWorld;
    GlobalsBase::mBroadphase = mBroadphase;

    GlobalsBase::mEarlyFrameSignal->connect( sigc::mem_fun( this, &PhysicsManager::update ) );
    GlobalsBase::mLateUpdateSignal->connect( sigc::mem_fun( this, &PhysicsManager::updateDebug ) );
}

btCollisionShape* PhysicsManager::copyCollisionShape( btCollisionShape* pShape )
{
    if( !pShape ) return 0;

    switch( pShape->getShapeType() )
    {
        case SPHERE_SHAPE_PROXYTYPE:
            return new btSphereShape( *static_cast<btSphereShape*>( pShape ) );
        case BOX_SHAPE_PROXYTYPE:
            return new btBoxShape( *static_cast<btBoxShape*>( pShape ) );
        case CYLINDER_SHAPE_PROXYTYPE:
            return new btCylinderShape( *static_cast<btCylinderShape*>( pShape ) );
        case CAPSULE_SHAPE_PROXYTYPE:
            return new btCapsuleShape( *static_cast<btCapsuleShape*>( pShape ) );
        case CONE_SHAPE_PROXYTYPE:
            return new btConeShape( *static_cast<btConeShape*>( pShape ) );
        case CONVEX_HULL_SHAPE_PROXYTYPE:
            return new btConvexHullShape( *static_cast<btConvexHullShape*>( pShape ) );
        case TRIANGLE_MESH_SHAPE_PROXYTYPE:
            return new btBvhTriangleMeshShape( *static_cast<btBvhTriangleMeshShape*>( 
                pShape ) );
        case TERRAIN_SHAPE_PROXYTYPE:
            return new btHeightfieldTerrainShape( *static_cast<btHeightfieldTerrainShape*>( 
                pShape ) );
        default:
            DIVERSIA_EXCEPT( Exception::ERR_NOT_IMPLEMENTED, 
                "Cannot copy collision shape for given type.", 
                "PhysicsManager::copyCollisionShape" );
            break;
    }

    return 0;
}

btCollisionShape* PhysicsManager::createCollisionShape( PhysicsShape type, 
    const btVector3& rParameters )
{
    switch( type )
    {
        case PHYSICSSHAPE_SPHERE:
            return new btSphereShape( rParameters.getX() );
        case PHYSICSSHAPE_BOX:
            return new btBoxShape( rParameters );
        case PHYSICSSHAPE_CYLINDER:
            return new btCylinderShape( rParameters );
        case PHYSICSSHAPE_CAPSULE:
            return new btCapsuleShape( rParameters.getX(), rParameters.getY() );
        case PHYSICSSHAPE_CONE:
            return new btConeShape( rParameters.getX(), rParameters.getY() );
        default:
            DIVERSIA_EXCEPT( Exception::ERR_NOT_IMPLEMENTED, 
                "Cannot create a collision shape for given type.", 
                "PhysicsManager::createCollisionShape" );
            break;
    }

    return 0;
}

btCollisionShape* PhysicsManager::createCollisionShape( char* pData, size_t size )
{
    if( mFileLoader->loadFileFromMemory( pData, size ) )
    {
        return mFileLoader->getCollisionShapeByIndex( mCollisionShapeCounter++ );
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_INTERNAL_ERROR, 
            "Could not create a collision shape from memory.", 
            "PhysicsManager::createCollisionShape" );
    }
}

void PhysicsManager::addBody( btRigidBody& rRigidBody )
{
    mDynamicsWorld->addRigidBody( &rRigidBody );
}

void PhysicsManager::removeBody( btRigidBody& rRigidBody )
{
    mDynamicsWorld->removeRigidBody( &rRigidBody );
}

void PhysicsManager::addCollisionObject( btCollisionObject& rObject )
{
    mDynamicsWorld->addCollisionObject( &rObject );
}

void PhysicsManager::removeCollisionObject( btCollisionObject& rObject )
{
    mDynamicsWorld->removeCollisionObject( &rObject );
}

void PhysicsManager::update( Real timeElapsed )
{
    mDynamicsWorld->stepSimulation( timeElapsed, 9 );

    // Dispatch collisions
    for( int i = 0; i < mDispatcher->getNumManifolds(); ++i )
    {
        // Get the colliding bodies and components.
        btPersistentManifold* contactManifold = mDispatcher->getManifoldByIndexInternal( i );

        CollisionShape* collisionShapeA = static_cast<CollisionShape*>( 
            static_cast<btCollisionObject*>( contactManifold->getBody0()
            )->getCollisionShape()->getUserPointer() );
        CollisionShape* collisionShapeB = static_cast<CollisionShape*>( 
            static_cast<btCollisionObject*>( contactManifold->getBody1()
            )->getCollisionShape()->getUserPointer() );

        if( collisionShapeA->isReceivingCollisionCallbacks() && 
            collisionShapeB->isReceivingCollisionCallbacks() )
        {
            int priorityA = collisionShapeA->mCollisionPriority;
            int priorityB = collisionShapeB->mCollisionPriority;
            if( priorityA >= priorityB )
                collisionShapeA->collisionWith( *collisionShapeB );
            if( priorityA <= priorityB )
                collisionShapeB->collisionWith( *collisionShapeA );
        }
        else if( collisionShapeA->isReceivingCollisionCallbacks() )
            collisionShapeA->collisionWith( *collisionShapeB );
        else if( collisionShapeB->isReceivingCollisionCallbacks() )
            collisionShapeB->collisionWith( *collisionShapeA );
    }
}

void PhysicsManager::updateDebug()
{
    if( mDebugDraw ) mDebugDrawer->step();
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia