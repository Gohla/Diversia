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
#include "OgreClient/Graphics/QueryFlags.h"
#include "OgreClient/Object/Entity.h"
#include "OgreClient/Object/Mesh.h"
#include "OgreClient/Object/SceneNode.h"
#include "OgreClient/Resource/ResourceManager.h"
#include "Util/Helper/ContainerInitializer.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

Entity::Entity( const String& rName, Mode mode, NetworkingType networkingType, 
    RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject ):
    ClientComponent( rName, mode, networkingType, Entity::getTypeStatic(), source, localOverride, 
        rObject ),
    mNode( rObject.getComponentCast<SceneNode>( "Node" ) ),
    mResourceManager( rObject.getClientObjectManager().getPluginManager().getPlugin<ResourceManager>() ),
    mCreated( false ),
    mEntity( 0 )
{
    PropertySynchronization::storeUserObject();
    mUserObject = ClientComponent::getClientObject();

    PropertySynchronization::queue( initializer< std::set<String> >( "Mesh", "ResourceList" ) );
    if( Component::isCreatedByServer() ) PropertySynchronization::queueConstruction( true );
}

Entity::~Entity()
{
    if( mEntity ) GlobalsBase::mScene->destroyEntity( mEntity );
}

void Entity::setMaterial( const String& rMaterial )
{
    mMaterial = rMaterial;

    if( !mMaterial.empty() )
        mEntity->setMaterialName( mMaterial, mResourceManager.getGroup() );
}

sigc::connection Entity::connect( const sigc::slot<void, Entity&>& rSlot )
{
    if( Entity::isLoaded() ) rSlot( *this );
    return mLoadedSignal.connect( rSlot );
}

void Entity::create()
{
    mCreated = true;

    try
    {
        Component::getObject().getComponent<Mesh>().connect( sigc::mem_fun( this, 
            &Entity::meshLoaded ) );
    }
    catch( Exception e )
    {
        CLOGW << "Object has an entity component, but no mesh component was found. " <<
            "Entity component will become active once the mesh is added to this object.";

        // Connect to component changes to see if the mesh component gets created or destroyed.
        mComponentChangeConnection = Component::getObject().connectComponentChange( sigc::mem_fun( 
            this, &Entity::componentChange ) );
    }
}

void Entity::meshLoaded( Mesh& rMesh )
{
    try
    {
        // Store previous state and destroy entity if it was already loaded.
        if( mEntity ) 
        {
            PropertySynchronization::storeState( "NoBitStream" );
            GlobalsBase::mScene->destroyEntity( mEntity );
        }

        // TODO: Store and load properties when reloading the mesh so all the properties from the 
        // previous entity also get set onto the new entity.

        mEntity = GlobalsBase::mScene->createEntity( Component::getGUIDString(), 
            rMesh.getMeshName(), mResourceManager.getGroup() );
        mNode.getNode()->attachObject( mEntity );

        mEntity->getUserObjectBindings().setUserAny( "Object", Ogre::Any( &mUserObject ) );
        mEntity->setQueryFlags( QueryFlags_Entity );

        PropertySynchronization::processQueuedConstruction();
        PropertySynchronization::processQueue();
        PropertySynchronization::loadStoredState();

        mLoadedSignal( *this );
        mLoadedSignal.clear();
    }
    catch( Exception e )
    {
        CLOGE << "Could not create entity: " << e.what();
    }
    catch( Ogre::Exception e )
    {
        CLOGE << "Could not create entity: " << e.what();
    }
}

void Entity::componentChange( Component& rComponent, bool created )
{
    if( rComponent.getType() == COMPONENTTYPE_MESH )
    {
        if( mCreated && created )
        {
            Component::getObject().getComponent<Mesh>().connect( sigc::mem_fun( this, 
                &Entity::meshLoaded ) );
            mComponentChangeConnection.disconnect();
        }
    }
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia