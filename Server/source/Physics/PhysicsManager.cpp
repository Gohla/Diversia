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

#include "Platform/StableHeaders.h"

#include "Object/CollisionShape.h"
#include "Physics/PhysicsManager.h"
#include "Resource/LocalResourceManager.h"
#include "Shared/Lua/LuaManager.h"
#include "Shared/Physics/PhysicsHeightfield.h"

namespace Diversia
{
namespace Server 
{
//------------------------------------------------------------------------------

PhysicsManager::PhysicsManager():
    mPhysicsHeightfield( 0 ),
    mCollisionShapeCounter( 0 ),
    mHeightmapWorldSize( DIVERSIA_SERVER_SIZE ),
    mHeightmapYScale( 200 )
{
    // Connect to resource found event for extension .bullet
    Globals::mResource->connect( ".bullet", sigc::mem_fun( this, 
        &PhysicsManager::loadCollisionShape ) );
    Globals::mResource->connect( ".raw", sigc::mem_fun( this, 
        &PhysicsManager::loadHeightfieldShape ) );

    Globals::mPhysics = this;
}

PhysicsManager::~PhysicsManager()
{
    LOGI << "Destroying physics";

    Globals::mPhysics = 0;
    Globals::mBroadphase = 0;
    Globals::mWorld = 0;

    if( mPhysicsHeightfield ) delete mPhysicsHeightfield;
    mFileLoader->deleteAllData();
    delete mFileLoader;
    delete mDynamicsWorld;
    delete mSolver;
    delete mDispatcher;
    delete mConfiguration;
    delete mBroadphase;
}

void PhysicsManager::init()
{
    LOGI << "Initializing physics";

    // Initialize bullet
    mBroadphase = new btAxisSweep3( btVector3( -DIVERSIA_SERVER_SIZE, -DIVERSIA_SERVER_SIZE,
        -DIVERSIA_SERVER_SIZE ), btVector3( DIVERSIA_SERVER_SIZE, DIVERSIA_SERVER_SIZE,
        DIVERSIA_SERVER_SIZE ) );

    mConfiguration = new btDefaultCollisionConfiguration();
    mDispatcher = new btCollisionDispatcher( mConfiguration );
    mSolver = new btSequentialImpulseConstraintSolver();
    mDynamicsWorld = new btDiscreteDynamicsWorld( mDispatcher, mBroadphase, mSolver,
        mConfiguration );
    mFileLoader = new btBulletWorldImporter( mDynamicsWorld );

    Globals::mWorld = mDynamicsWorld;
    Globals::mBroadphase = mBroadphase;

    Globals::mFrameSignal->connect( sigc::mem_fun( this, &PhysicsManager::update ) );

    Globals::mLua->object( "Physics" ) = this;
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

btCollisionShape* PhysicsManager::createCollisionShape( const Path& rFile )
{
    CollisionShapes::iterator i = mCollisionShapes.find( 
        Globals::mResource->getRootResourceLocation() / rFile );
    if( i != mCollisionShapes.end() )
    {
        switch( i->second->getShapeType() )
        {
            case SPHERE_SHAPE_PROXYTYPE:
                return new btSphereShape( *static_cast<btSphereShape*>( i->second ) );
            case BOX_SHAPE_PROXYTYPE:
                return new btBoxShape( *static_cast<btBoxShape*>( i->second ) );
            case CYLINDER_SHAPE_PROXYTYPE:
                return new btCylinderShape( *static_cast<btCylinderShape*>( i->second ) );
            case CAPSULE_SHAPE_PROXYTYPE:
                return new btCapsuleShape( *static_cast<btCapsuleShape*>( i->second ) );
            case CONE_SHAPE_PROXYTYPE:
                return new btConeShape( *static_cast<btConeShape*>( i->second ) );
            case CONVEX_HULL_SHAPE_PROXYTYPE:
                return new btConvexHullShape( *static_cast<btConvexHullShape*>( i->second ) );
            case TRIANGLE_MESH_SHAPE_PROXYTYPE:
                return new btBvhTriangleMeshShape( *static_cast<btBvhTriangleMeshShape*>( 
                    i->second ) );
            case TERRAIN_SHAPE_PROXYTYPE:
                return new btHeightfieldTerrainShape( *static_cast<btHeightfieldTerrainShape*>( 
                    i->second ) );
            default:
                DIVERSIA_EXCEPT( Exception::ERR_NOT_IMPLEMENTED, 
                    "Cannot copy collision shape for given type.", 
                    "PhysicsManager::copyCollisionShape" );
                break;
        }
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_FILE_NOT_FOUND, "Collision shape does not exist.", 
            "PhysicsManager::createCollisionShape" );
    }

    return 0;
}

void PhysicsManager::addBody( btRigidBody& rRigidBody )
{
    mDynamicsWorld->addRigidBody( &rRigidBody );
}

void PhysicsManager::removeBody( btRigidBody& rRigidBody )
{
    mDynamicsWorld->removeRigidBody( &rRigidBody );
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

void PhysicsManager::loadCollisionShape( const Path& rFile )
{
    if( mFileLoader->loadFile( rFile.string().c_str() ) && 
        mFileLoader->getNumCollisionShapes() > mCollisionShapeCounter )
    {
        mCollisionShapes.insert( std::make_pair( rFile, 
            mFileLoader->getCollisionShapeByIndex( mCollisionShapeCounter++ ) ) );
        return;
    }

    LOGE << "Failed to load collision shape from " << rFile;
}

void PhysicsManager::loadHeightfieldShape( const Path& rFile )
{
    if( !mPhysicsHeightfield )
    {
        // TODO: Config size;
        mPhysicsHeightfield = new PhysicsHeightfield( rFile, 1025 );
        mCollisionShapes.insert( std::make_pair( rFile, mPhysicsHeightfield->init() ) );
    }
    else
    {
        LOGW << "Trying to load more than one heightfields, only the first one will be loaded.";
    }
}

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia