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

#include "OgreClient/Object/CollisionShape.h"
#include "OgreClient/Physics/PhysicsManager.h"
#include "OgreClient/Resource/GenericResource.h"
#include "OgreClient/Resource/ResourceManager.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

CollisionShape::CollisionShape( const String& rName, Mode mode, NetworkingType networkingType, 
    RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject ):
    ClientComponent( rName, mode, networkingType, CollisionShape::getTypeStatic(), source, localOverride, 
        rObject ),
    mResourceManager( rObject.getClientObjectManager().getPluginManager().getPlugin<ResourceManager>() ),
    mCollisionShape( 0 ),
    mShapeType( PHYSICSSHAPE_SPHERE ),
    mShapeParameters( Vector3::UNIT_SCALE ),
    mShapeParametersSet( false ),
    mCollisionPriority( 0 ),
    mCollisionMarginScaling( 1.0, 1.0, 1.0 ),
    mCreated( false )
{
    PropertySynchronization::storeUserObject();
}

CollisionShape::~CollisionShape()
{
    CollisionShape::destroyCollisionShape();
}

void CollisionShape::setCollisionFile( const Path& rFile )
{
    if( rFile.empty() || mCollisionFile == rFile ) return;
    mCollisionFile = rFile;
    mShapeParameters = Vector3::UNIT_SCALE;
    mShapeParametersSet = false;
    mShapeType = PHYSICSSHAPE_FILE;

    if( mCreated )
    {
        // TODO: Store and load properties when reloading the mesh so all the properties from the 
        // previous entity also get set onto the new entity.

        try
        {
            mResourceManager.loadResource( mCollisionFile, sigc::mem_fun( this, 
                &CollisionShape::resourceLoaded ) );
        }
        catch( Exception e )
        {
            CLOGE << "Could not load resource for collision shape component: " << e.what();
        }
    }
}

void CollisionShape::setShapeParameters( const Vector3& rParameters )
{
    mShapeParameters = rParameters;
    if( mShapeType == PHYSICSSHAPE_FILE ) return;
    mShapeParametersSet = true;
    mCollisionFile.clear();

    if( mCreated )
    {
        // TODO: Store and load properties when reloading the shape so all the properties from the 
        // previous shape also get set onto the new shape.

        try
        {
            CollisionShape::destroyCollisionShape();
            mCollisionShape = GlobalsBase::mPhysics->createCollisionShape( mShapeType, 
                toVector3<btVector3>( mShapeParameters ) );
            CollisionShape::createCollisionShape();
        }
        catch( Exception e )
        {
            CLOGE << "Could not create collision shape: " << e.what();
        }
    }
}

void CollisionShape::setShapeType( PhysicsShape type )
{
    mShapeType = type;

    // If shape type is set to file, try to load the mesh or do nothing.
    if( mShapeType == PHYSICSSHAPE_FILE || mShapeType == PHYSICSSHAPE_HEIGHTMAPFILE )
    {
        if( !mCollisionFile.empty() && mCreated )
        {
            // TODO: Store and load properties when reloading the mesh so all the properties from the 
            // previous entity also get set onto the new entity.

            try
            {
                mResourceManager.loadResource( mCollisionFile, sigc::mem_fun( this, 
                    &CollisionShape::resourceLoaded ) );
            }
            catch( Exception e )
            {
                CLOGE << "Could not load resource for collision shape component: " << e.what();
            }
        }

        return;
    }

    mShapeParametersSet = true;

    if( mCreated )
    {
        // TODO: Store and load properties when reloading the shape so all the properties from the 
        // previous shape also get set onto the new shape.

        try
        {
            CollisionShape::destroyCollisionShape();
            mCollisionShape = GlobalsBase::mPhysics->createCollisionShape( mShapeType, 
                toVector3<btVector3>( mShapeParameters ) );
            CollisionShape::createCollisionShape();
        }
        catch( Exception e )
        {
            CLOGE << "Could not create collision shape: " << e.what();
        }
    }
}

sigc::connection CollisionShape::connectLoaded( const sigc::slot<void, CollisionShape&>& rSlot )
{
    if( CollisionShape::isLoaded() ) rSlot( *this );
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
        if( !mCollisionFile.empty() )
        {
            mResourceManager.loadResource( mCollisionFile, sigc::mem_fun( this, 
                &CollisionShape::resourceLoaded ) );
        }
        else
        {
            mCollisionShape = GlobalsBase::mPhysics->createCollisionShape( mShapeType, 
                toVector3<btVector3>( mShapeParameters ) );
            CollisionShape::createCollisionShape();
        }
    }
    catch( Exception e )
    {
        CLOGE << "Could not load resource for collision shape component: " << e.what();
        Component::destroyComponentLocally();
    }
}

void CollisionShape::resourceLoaded( Ogre::ResourcePtr pResource )
{
    // Destroy previous collision shape.
    CollisionShape::destroyCollisionShape();

    // Check resource type
    if( pResource->getCreator()->getResourceType() != "Generic" )
    {
        CLOGE << "Could not load collision shape, physics shape file is not a physics mesh.";
        return;
    }

    GenericResourcePtr resource = pResource;
    if( resource->hasHolderData() )
    {
        if( resource->getHolderDataType() != "RigidBody" )
        {
            CLOGE << "Could not load collision shape, physics shape file is not a physics mesh.";
            return;
        }

        try
        {
            mCollisionShape = GlobalsBase::mPhysics->copyCollisionShape( 
                resource->getHolderData<btCollisionShape*>() );
        }
        catch( boost::bad_any_cast e )
        {
            CLOGE << "Could not load collision shape: " << e.what();
            return;
        }
    }
    else
    {
        char* data = resource->getCacheCopy();

        try
        {
            mCollisionShape = GlobalsBase::mPhysics->createCollisionShape( data, 
                resource->getCache()->size() );
            resource->setHolderData( mCollisionShape );
            resource->setHolderDataType( "RigidBody" );
            mCollisionShape = GlobalsBase::mPhysics->copyCollisionShape( mCollisionShape );
        }
        catch( Exception e )
        {
            CLOGE << "Could not load collision shape: " << e.what();
            delete[] data;
            return;
        }

        delete[] data;
    }

    CollisionShape::createCollisionShape();
}

void CollisionShape::destroyCollisionShape()
{
    if( mCollisionShape )
    {
        mTransformConnection.disconnect();
        delete mCollisionShape;
        mCollisionShape = 0;
    }
}

void CollisionShape::createCollisionShape()
{
    mCollisionMarginScaling = btVector3( 1.0, 1.0, 1.0 );
    mCollisionShape->setUserPointer( this );

    // Scale collision shape for non-heightmap shapes.
    if( mShapeType != PHYSICSSHAPE_HEIGHTMAPFILE )
        mCollisionShape->setLocalScaling( toVector3<btVector3>( 
        Component::getObject()._getDerivedScale() ) );
    mTransformConnection = Component::getObject().connectTransformChange( sigc::mem_fun( this, 
        &CollisionShape::transformChange ) );

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

    mLoadedSignal( *this );
}

void CollisionShape::transformChange( const Node& rNode )
{
    btVector3 scale = toVector3<btVector3>( rNode._getDerivedScale() );
    if( mShapeType != PHYSICSSHAPE_HEIGHTMAPFILE && mCollisionShape )
    {
        mCollisionShape->setLocalScaling( scale * mCollisionMarginScaling );
    }
}

void CollisionShape::collisionWith( CollisionShape& rCollisionShape )
{
    mCollisionSignal( rCollisionShape );
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia