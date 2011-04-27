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

#ifndef DIVERSIA_OGRECLIENT_RIGIDBODY_H
#define DIVERSIA_OGRECLIENT_RIGIDBODY_H

#include "OgreClient/Platform/Prerequisites.h"

#include "Client/Object/ClientComponent.h"
#include "Shared/Physics/Physics.h"
#include "OgreClient/Physics/PhysicsManager.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API RigidBody : public ClientComponent, public btMotionState, 
    public sigc::trackable
{
public:
    RigidBody( const String& rName, Mode mode, NetworkingType networkingType, 
        RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject );
    ~RigidBody();

    /**
    Gets the rigid body. 
    **/
    inline btRigidBody* getRigidBody() const { return mRigidBody; }
    /**
    Query if the rigid body is loaded.
    **/
    inline bool isLoaded() const { return mRigidBody != 0; }
    /**
    Gets the component type.
    **/
    inline ComponentType getType() const { return COMPONENTTYPE_RIGIDBODY; }
    static inline ComponentType getTypeStatic() { return COMPONENTTYPE_RIGIDBODY; }
    /**
    Gets the component type name.
    **/
    inline String getTypeName() const { return COMPONENTNAME_RIGIDBODY; }
    static inline String getTypeNameStatic() { return COMPONENTNAME_RIGIDBODY; }

    /**
    Gets the world transform. 
    **/
    inline void getWorldTransform( btTransform& rWorldTrans ) const
    {
        rWorldTrans.setOrigin( toVector3<btVector3>( Component::getObject()._getDerivedPosition() ) );
        rWorldTrans.setRotation( toQuaternion<btQuaternion>( 
            Component::getObject()._getDerivedOrientation() ) );
    }
    /**
    Sets the world transform. 
    **/
    inline void setWorldTransform( const btTransform& rWorldTrans )
    {
        Component::getObject().setPosition( toVector3<Vector3>( rWorldTrans.getOrigin() ) );
        Component::getObject().setOrientation( toQuaternion<Quaternion>( 
            rWorldTrans.getRotation() ) );
    }
    /**
    Gets the mass.
    **/
    inline Real getMass() const { return mMass; }
    /**
    Sets the mass.
    **/
    void setMass( Real mass );
    /**
    Gets the linear dampening. 
    **/
    inline Real getLinearDampening() const { return mRigidBody->getLinearDamping(); }
    /**
    Sets the linear dampening. 
    **/
    inline void setLinearDampening( Real dampening ) const { mRigidBody->setDamping( dampening, 
        RigidBody::getAngularDampening() ); }
    /**
    Gets the angular dampening. 
    **/
    inline Real getAngularDampening() const { return mRigidBody->getAngularDamping(); }
    /**
    Sets the angular dampening. 
    **/
    inline void setAngularDampening( Real dampening ) const { mRigidBody->setDamping( 
        RigidBody::getLinearDampening(), dampening ); }
    /**
    Gets the linear sleeping threshold. 
    **/
    inline Real getLinearSleepingThreshold() const { 
        return mRigidBody->getLinearSleepingThreshold(); }
    /**
    Sets the linear sleeping treshhold. 
    **/
    inline void setLinearSleepingThreshold( Real treshhold ) { mRigidBody->setSleepingThresholds( 
        treshhold, RigidBody::getAngularSleepingThreshold() ); }
    /**
    Gets the angular sleeping threshold. 
    **/
    inline Real getAngularSleepingThreshold() const { 
        return mRigidBody->getAngularSleepingThreshold(); }
    /**
    Sets the angular sleeping threshold. 
    **/
    inline void setAngularSleepingThreshold( Real treshhold ) { mRigidBody->setSleepingThresholds( 
        RigidBody::getLinearSleepingThreshold(), treshhold ); }
    /**
    Sets the physics type. 
    **/
    inline void setPhysicsType( PhysicsType type ) { mPhysicsType = type; if( mCreated ) RigidBody::createRigidBody(); }

private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    void create();
    void collisionShapeLoaded( CollisionShape& rCollisionShape );
    void createRigidBody();
    void destroyRigidBody();
    inline bool delayedDestruction() { return false; }
    void componentChange( Component& rComponent, bool created );
    void pluginStateChanged( PluginState state, PluginState prevState );
    void transformChange( const Node& rNode );

    btRigidBody*        mRigidBody;
    btCollisionShape*   mCollisionShape;
    sigc::connection    mTransformConnection;

    PhysicsType     mPhysicsType;
    Real            mMass;
    bool            mCreated;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::RigidBody, 
    &Diversia::OgreClient::Bindings::CampBindings::bindRigidBody );

#endif // DIVERSIA_OGRECLIENT_RIGIDBODY_H