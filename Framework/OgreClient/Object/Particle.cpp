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

#include "OgreClient/Object/Particle.h"
#include "OgreClient/Object/SceneNode.h"
#include "OgreClient/Resource/ResourceManager.h"
#include "OgreClient/Graphics/QueryFlags.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

Particle::Particle( const String& rName, Mode mode, NetworkingType networkingType, 
    RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject ):
    ClientComponent( rName, mode, networkingType, Particle::getTypeStatic(), source, localOverride, 
        rObject ),
    mNode( rObject.getComponentCast<SceneNode>( "Node" ) ),
    mResourceManager( rObject.getClientObjectManager().getPluginManager().getPlugin<ResourceManager>() ),
    mParticleSystem( 0 ),
    mCreated( false )
{
    PropertySynchronization::storeUserObject();

    PropertySynchronization::queue( initializer< std::set<String> >( "ResourceList", "Name" ) );
    if( Component::isCreatedByServer() ) PropertySynchronization::queueConstruction( true );
}

Particle::~Particle()
{
    if( mParticleSystem ) GlobalsBase::mScene->destroyParticleSystem( mParticleSystem );
}

void Particle::setParticleSystemName( const String& rParticleSystemName )
{
    if( rParticleSystemName.empty() || mParticleSystemName == rParticleSystemName ) return;
    mParticleSystemName = rParticleSystemName;

    if( mCreated )
    {
        // TODO: Store and load properties when reloading the particle system so all the properties 
        // from the previous particle system also get set onto the new particle system.

        try
        {
            mResourceManager.loadResources( mResourceList, sigc::mem_fun( this, 
                &Particle::resourcesLoaded ) );
        }
        catch( FileNotFoundException e )
        {
            CLOGE << "Could not load resource(s) for particle component: " << e.what();
        }
    }
}

void Particle::create()
{
    mCreated = true;
    if( mParticleSystemName.empty() ) return;

    try
    {
        mResourceManager.loadResources( mResourceList, sigc::mem_fun( this, 
            &Particle::resourcesLoaded ) );
    }
    catch( FileNotFoundException e )
    {
        CLOGE << "Could not load resource(s) for particle component: " << e.what();
        Component::destroyComponent();
    }
}

void Particle::resourcesLoaded()
{
    try
    {
        if( mParticleSystem )
        {
            GlobalsBase::mScene->destroyParticleSystem( mParticleSystem );
        }

        // TODO: Store and load properties when reloading the particle system so all the properties 
        // from the previous particle system also get set onto the new particle system.

        mParticleSystem = GlobalsBase::mScene->createParticleSystem( Component::getGUIDString(), 
            mParticleSystemName );
        mParticleSystem->setEmitting( false );

        mParticleSystem->setUserAny( Ogre::Any( &ClientComponent::getClientObject() ) );
        mParticleSystem->setQueryFlags( QueryFlags_Particle );

        mNode.getNode()->attachObject( mParticleSystem );

        PropertySynchronization::processQueuedConstruction();
        PropertySynchronization::processQueue();
    }
    catch( Exception e )
    {
        CLOGE << "Could not create particle system: " << e.what();
    }
    catch( Ogre::Exception e )
    {
        CLOGE << "Could not create particle system: " << e.what();
    }
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia