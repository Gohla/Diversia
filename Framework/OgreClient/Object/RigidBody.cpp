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

#include "OgreClient/Graphics/GraphicsManager.h"
#include "OgreClient/Object/CollisionShape.h"
#include "OgreClient/Object/RigidBody.h"
#include "Util/Math/Node.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

RigidBody::RigidBody( const String& rName, Mode mode, NetworkingType networkingType, 
    RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject ):
    ClientComponent( rName, mode, networkingType, RigidBody::getTypeStatic(), source, localOverride, 
        rObject ),
    mRigidBody( 0 ),
    mCollisionShape( 0 ),
    mPhysicsType( PHYSICSTYPE_DYNAMIC ),
    mMass( 1.0 ),
    mCreated( false )
{
    PropertySynchronization::storeUserObject();

    PropertySynchronization::queue( initializer< std::set<String> >( "PhysicsType", "Mass" ) );
    if( Component::isCreatedByServer() ) PropertySynchronization::queueConstruction( true );

    // Connect to component changes to see if the collision shape component gets created or destroyed.
    Component::getObject().connectComponentChange( sigc::mem_fun( this, &RigidBody::componentChange ) );
}

RigidBody::~RigidBody()
{
    RigidBody::destroyRigidBody();
}

void RigidBody::setMass( Real mass )
{
    if( !( mPhysicsType == PHYSICSTYPE_STATIC || mPhysicsType == PHYSICSTYPE_KINEMATIC ) &&
        mMass != mass )
    {
        mMass = mass;

        if( mRigidBody && mCollisionShape )
        {
            btVector3 inertia;
            mCollisionShape->calculateLocalInertia( mMass, inertia );
            mRigidBody->setMassProps( mMass, inertia );
        }
        // TODO: Probably have to recreate the body.
    }
}

void RigidBody::create()
{
    mCreated = true;

    try
    {
        Component::getObject().getComponent<CollisionShape>().connectLoaded( sigc::mem_fun( this, 
            &RigidBody::collisionShapeLoaded ) );
    }
    catch( Exception e )
    {
        CLOGW << "Object has a rigid body component, but no collision shape component was found. \
Rigid body component will become active once a collision shape component is added to this object.";
    }
}

void RigidBody::collisionShapeLoaded( CollisionShape& rCollisionShape )
{
    try
    {
        if( !rCollisionShape.isLoaded() ) 
        {
            mCollisionShape = 0;
            return;
        }
        mCollisionShape = rCollisionShape.getCollisionShape();
        RigidBody::destroyRigidBody();

        // Check mass
        if( mPhysicsType == PHYSICSTYPE_STATIC || mPhysicsType == PHYSICSTYPE_KINEMATIC ) mMass = 0;

        // Calculate inertia
        btVector3 inertia;
        mCollisionShape->calculateLocalInertia( mMass, inertia );

        // Create the rigid body.
        mRigidBody = new btRigidBody( mMass, this, mCollisionShape, inertia );
        mRigidBody->setUserPointer( this );

        // If physics type is kinematic set the body to be kinematic.
        if( mPhysicsType == PHYSICSTYPE_KINEMATIC )
        {
            mRigidBody->setCollisionFlags( mRigidBody->getCollisionFlags() |
                btCollisionObject::CF_KINEMATIC_OBJECT );
            mRigidBody->setActivationState( DISABLE_DEACTIVATION );
        }

        PropertySynchronization::processQueuedConstruction();

        // Set type to kinematic on the client for now when in online mode.
        if( !ClientComponent::getClientObject().getClientObjectManager().isOfflineMode() )
        {
            mRigidBody->setCollisionFlags( mRigidBody->getCollisionFlags() |
                btCollisionObject::CF_KINEMATIC_OBJECT );
            mRigidBody->setActivationState( DISABLE_DEACTIVATION );
        }

        // Add body to the world.
        GlobalsBase::mPhysics->addBody( *mRigidBody );

        // Process queue after adding the body to the world so that functions like applyForce work.
        // Delay this call to the next tick, bullet doesn't do some things well immediately after
        // adding the body to the world.
        DelayedCall::create( sigc::mem_fun( this, &PropertySynchronization::processQueue ), 0 );
    }
    catch( Exception e )
    {
        CLOGW << "Rigid body could not be created: " << e.what();
        Component::destroyComponentLocally();
    }
}

void RigidBody::destroyRigidBody()
{
    if( mRigidBody )
    {
        GlobalsBase::mPhysics->removeBody( *mRigidBody );
        delete mRigidBody;
        mRigidBody = 0;
    }
}

void RigidBody::componentChange( Component& rComponent, bool created )
{
    if( rComponent.getType() == COMPONENTTYPE_COLLISIONSHAPE )
    {
        if( mCreated && created )
        {
            Component::getObject().getComponent<CollisionShape>().connectLoaded( sigc::mem_fun( this, 
                &RigidBody::collisionShapeLoaded ) );
        }
        else if( mCollisionShape && !created )
        {
            CLOGW << "Collision shape component was destroyed while a rigid body component was active.";
            RigidBody::destroyRigidBody();
            mCollisionShape = 0;
        }
    }
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia