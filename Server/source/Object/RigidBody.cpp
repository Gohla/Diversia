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
#include "Object/Object.h"
#include "Object/RigidBody.h"
#include "Util/Math/Node.h"

namespace Diversia
{
namespace Server 
{
//------------------------------------------------------------------------------

RigidBody::RigidBody( const String& rName, Mode mode, NetworkingType networkingType, 
    RakNet::RakNetGUID source, bool localOverride, ServerObject& rObject ):
    ServerComponent( rName, mode, networkingType, RigidBody::getTypeStatic(), source, localOverride, 
        rObject ),
    mRigidBody( 0 ),
    mCollisionShape( 0 ),
    mPhysicsType( PHYSICSTYPE_DYNAMIC ),
    mMass( 1.0 ),
    mCollisionMarginScaling( 1.0, 1.0, 1.0 )
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
    try
    {
        Component::getObject().getComponent<CollisionShape>().connectLoaded( sigc::mem_fun( this, 
            &RigidBody::collisionShapeLoaded ) );
    }
    catch( Exception e )
    {
        LOGW << "Object has a rigid body component, but no collision shape component was found. \
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
        mShapeType = rCollisionShape.getShapeType();
        RigidBody::destroyRigidBody();

        // Scale collision shape for non-heightmap shapes.
        if( mShapeType != PHYSICSSHAPE_HEIGHTMAPFILE )
            mCollisionShape->setLocalScaling( toVector3<btVector3>( 
            Component::getObject()._getDerivedScale() ) );
        Component::getObject().connectDerivedChange( sigc::mem_fun( this, 
            &RigidBody::transformChange ) );

        // Determine collision margin scaling factor.
        if( mShapeType != PHYSICSSHAPE_SPHERE && mShapeType != PHYSICSSHAPE_BOX )
        {
            btVector3 min, max, diff; 
            mCollisionShape->getAabb( btTransform::getIdentity(), min, max );
            diff = max - min;
            diff.normalize();
            mCollisionMarginScaling.setX( 1 - ( mCollisionShape->getMargin() / diff.getX() ) );
            mCollisionMarginScaling.setY( 1 - ( mCollisionShape->getMargin() / diff.getY() ) );
            mCollisionMarginScaling.setZ( 1 - ( mCollisionShape->getMargin() / diff.getZ() ) );

            // Scale again with the collision margin scaling offset.
            if( mShapeType != PHYSICSSHAPE_HEIGHTMAPFILE )
                mCollisionShape->setLocalScaling( toVector3<btVector3>( 
                Component::getObject()._getDerivedScale() ) * mCollisionMarginScaling );
        }

        // Check mass
        if( mPhysicsType == PHYSICSTYPE_STATIC || mPhysicsType == PHYSICSTYPE_KINEMATIC )
            mMass = 0;

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

        LOGD << "Adding " << camp::enumByType<PhysicsType>().name( mPhysicsType ) << 
            " " << camp::enumByType<PhysicsShape>().name( mShapeType ) << " rigid body" <<
            " with mass " << mMass << ", scale " << Component::getObject().getScale() << 
            ", friction " << mRigidBody->getFriction() << ", restitution " << 
            mRigidBody->getRestitution();

        // Add body to the world.
        Globals::mPhysics->addBody( *mRigidBody );

        // Process queue after adding the body to the world so that functions like applyForce work.
        // Delay this call to the next tick, bullet doesn't do some things well immediately after
        // adding the body to the world.
        DelayedCall::create( sigc::mem_fun( this, &PropertySynchronization::processQueue ), 0 );
    }
    catch( Exception e )
    {
        LOGW << "Rigid body could not be created: " << e.what();
        Component::destroyComponent();
    }
}

void RigidBody::transformChange( const Node& rNode )
{
    btVector3 scale = toVector3<btVector3>( rNode._getDerivedScale() );
    if( mShapeType != PHYSICSSHAPE_HEIGHTMAPFILE && mCollisionShape )
    {
        mCollisionShape->setLocalScaling( scale * mCollisionMarginScaling );
    }
}

void RigidBody::destroyRigidBody()
{
    if( mRigidBody )
    {
        Globals::mPhysics->removeBody( *mRigidBody );
        delete mRigidBody;
        mRigidBody = 0;
    }
}

void RigidBody::componentChange( Component& rComponent, bool created )
{
    if( rComponent.getType() == COMPONENTTYPE_COLLISIONSHAPE )
    {
        if( created )
        {
            Component::getObject().getComponent<CollisionShape>().connectLoaded( sigc::mem_fun( this, 
                &RigidBody::collisionShapeLoaded ) );
        }
        else if( mCollisionShape )
        {
            LOGW << "Collision shape component was destroyed while a rigid body component was active.";
            RigidBody::destroyRigidBody();
            mCollisionShape = 0;
        }
    }
}

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia