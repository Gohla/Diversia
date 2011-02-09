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

namespace Diversia
{
namespace Server 
{
//------------------------------------------------------------------------------

CollisionShape::CollisionShape( const String& rName, Mode mode, NetworkingType networkingType, 
    RakNet::RakNetGUID source, bool localOverride, ServerObject& rObject ):
    ServerComponent( rName, mode, networkingType, CollisionShape::getTypeStatic(), source, localOverride, 
        rObject ),
    mCollisionShape( 0 ),
    mShapeType( PHYSICSSHAPE_SPHERE ),
    mShapeParameters( Vector3::UNIT_SCALE ),
    mShapeParametersSet( false ),
    mCreated( false )
{
    PropertySynchronization::storeUserObject();
}

CollisionShape::~CollisionShape()
{
    if( mCollisionShape )
    {
        delete mCollisionShape;
    }
}

void CollisionShape::setCollisionFile( const Path& rFile )
{
    if( rFile.empty() || mCollisionFile == rFile ) return;
    mCollisionFile = rFile;
    mShapeParameters = Vector3::UNIT_SCALE;
    mShapeParametersSet = false;

    if( mCreated )
    {
        // TODO: Store and load properties when reloading the mesh so all the properties from the 
        // previous entity also get set onto the new entity.

        try
        {
            mCollisionShape = Globals::mPhysics->createCollisionShape( mCollisionFile );
            mCollisionShape->setUserPointer( this );
            mLoadedSignal( *this );
        }
        catch( Exception e )
        {
            LOGE << "Could not load resource for collision shape component: " << e.what();
        }
    }
}

void CollisionShape::setShapeParameters( const Vector3& rParameters )
{
    if( rParameters == Vector3::ZERO || mShapeParameters == rParameters ) return;
    mShapeParameters = rParameters;
    mShapeParametersSet = true;
    mCollisionFile.clear();

    if( mCreated )
    {
        // TODO: Store and load properties when reloading the mesh so all the properties from the 
        // previous entity also get set onto the new entity.

        try
        {
            // Destroy previous collision shape.
            if( mCollisionShape )
            {
                delete mCollisionShape;
                mCollisionShape = 0;
            }

            mCollisionShape = Globals::mPhysics->createCollisionShape( mShapeType, 
                toVector3<btVector3>( mShapeParameters ) );
            mCollisionShape->setUserPointer( this );
            mLoadedSignal( *this );
        }
        catch( Exception e )
        {
            LOGE << "Could not create collision shape: " << e.what();
        }
    }
}

sigc::connection CollisionShape::connectLoaded( const sigc::slot<void, CollisionShape&>& rSlot )
{
    if( CollisionShape::isLoaded() )
    {
        rSlot( *this );
        return sigc::connection();
    }
    else
        return mLoadedSignal.connect( rSlot );
}

sigc::connection CollisionShape::connectCollision( const sigc::slot<void, CollisionShape&>& rSlot )
{
    return mCollisionSignal.connect( rSlot );
}

void CollisionShape::create()
{
    mCreated = true;
    if( mCollisionFile.empty() && !mShapeParametersSet ) return;

    try
    {
        // Create collision shape
        if( mCollisionFile.empty() )
            mCollisionShape = Globals::mPhysics->createCollisionShape( mShapeType, 
                toVector3<btVector3>( mShapeParameters ) );
        else
            mCollisionShape = Globals::mPhysics->createCollisionShape( mCollisionFile );

        mCollisionShape->setUserPointer( this );
        mLoadedSignal( *this );
    }
    catch( Exception e )
    {
        LOGE << "Could not load resource for collision shape component: " << e.what();
    }
}

void CollisionShape::collisionWith( CollisionShape& rCollisionShape )
{
    mCollisionSignal( rCollisionShape );
}

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia