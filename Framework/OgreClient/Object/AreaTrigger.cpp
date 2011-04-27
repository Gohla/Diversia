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

#include "Object/Component.h"
#include "OgreClient/Object/AreaTrigger.h"
#include "OgreClient/Object/CollisionShape.h"
#include "OgreClient/Physics/PhysicsManager.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

AreaTrigger::AreaTrigger( const String& rName, Mode mode, NetworkingType networkingType, 
    RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject ):
    ClientComponent( rName, mode, networkingType, AreaTrigger::getTypeStatic(), source, 
        localOverride, rObject ),
    mGhostObject( 0 ),
    mCollisionShape( 0 )
{
    PropertySynchronization::storeUserObject();

    // Connect to component changes to see if the collision shape component gets created or destroyed.
    Component::getObject().connectComponentChange( sigc::mem_fun( this, &AreaTrigger::componentChange ) );
}

AreaTrigger::~AreaTrigger()
{
    AreaTrigger::destroyGhostObject();
}

void AreaTrigger::create()
{
    try
    {
        Component::getObject().getComponent<CollisionShape>().connectLoaded( sigc::mem_fun( this, 
            &AreaTrigger::collisionShapeLoaded ) );
    }
    catch( Exception e )
    {
        CLOGW << "Object has a area trigger component, but no collision shape component was found. \
Area trigger component will become active once a collision shape component is added to this object.";
    }
}

void AreaTrigger::destroyGhostObject()
{
    if( mGhostObject )
    {
        mObjectsInArea.clear();
        mTransformConnection.disconnect();
        GlobalsBase::mPhysics->removeCollisionObject( *mGhostObject );
        delete mGhostObject;
        mGhostObject = 0;
    }
}

void AreaTrigger::collisionShapeLoaded( CollisionShape& rCollisionShape )
{
    if( !rCollisionShape.isLoaded() ) 
    {
        mCollisionShape = 0;
        return;
    }
    mCollisionShape = rCollisionShape.getCollisionShape();
    AreaTrigger::destroyGhostObject();

    mGhostObject = new btPairCachingGhostObject();
    mGhostObject->setUserPointer( this );
    mGhostObject->setCollisionShape( mCollisionShape );
    mGhostObject->setCollisionFlags( btCollisionObject::CF_KINEMATIC_OBJECT | 
        btCollisionObject::CF_NO_CONTACT_RESPONSE );
    mGhostObject->setActivationState( DISABLE_SIMULATION );
    mGhostObject->setWorldTransform( btTransform( 
        toQuaternion<btQuaternion>( Component::getObject()._getDerivedOrientation() ), 
        toVector3<btVector3>( Component::getObject()._getDerivedPosition() ) ) );
    GlobalsBase::mPhysics->addCollisionObject( *mGhostObject );

    mTransformConnection = Component::getObject().connectTransformChange( sigc::mem_fun( this, 
        &AreaTrigger::transformChange ) );
}

void AreaTrigger::componentChange( Component& rComponent, bool created )
{
    if( rComponent.getType() == COMPONENTTYPE_COLLISIONSHAPE )
    {
        if( created )
        {
            Component::getObject().getComponent<CollisionShape>().connectLoaded( sigc::mem_fun( this, 
                &AreaTrigger::collisionShapeLoaded ) );
        }
        else if( mCollisionShape )
        {
            CLOGW << "Collision shape component was destroyed while an area trigger component was active.";
            AreaTrigger::destroyGhostObject();
            mCollisionShape = 0;
        }
    }
}

void AreaTrigger::enterArea( Object& rObject )
{
    mObjectsInArea.insert( std::make_pair( rObject.getName(), &rObject ) );
    if( ClientComponent::getPluginState() != STOP ) mAreaTriggerSignal( rObject, true );
}

void AreaTrigger::leaveArea( Object& rObject )
{
    mObjectsInArea.erase( rObject.getName() );
    if( ClientComponent::getPluginState() != STOP ) mAreaTriggerSignal( rObject, false );
}

sigc::connection AreaTrigger::connectAreaChange( const sigc::slot<void, Object&, bool>& rSlot )
{
    return mAreaTriggerSignal.connect( rSlot );
}

void AreaTrigger::transformChange( const Node& rNode )
{
    if( !Component::getObject().hasComponent( COMPONENTTYPE_RIGIDBODY ) )
        mGhostObject->setWorldTransform( btTransform( 
            toQuaternion<btQuaternion>( rNode._getDerivedOrientation() ), 
            toVector3<btVector3>( rNode._getDerivedPosition() ) ) );
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia