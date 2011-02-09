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

#include "OgreClient/Object/Animation.h"
#include "OgreClient/Object/Entity.h"
#include "OgreClient/Graphics/GraphicsManager.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

Animation::Animation( const String& rName, Mode mode, NetworkingType networkingType, 
    RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject ):
    ClientComponent( rName, mode, networkingType, Animation::getTypeStatic(), source, localOverride, 
        rObject ),
    mSpeed( 1.0 ),
    mAutoLength( true ),
    mAnimationState( 0 ),
    mCreated( false )
{
    PropertySynchronization::storeUserObject();

    PropertySynchronization::queue( initializer< std::set<String> >( "AnimationName" ) );
    if( Component::isCreatedByServer() ) PropertySynchronization::queueConstruction( true );

    // Connect to component changes to see if the entity component gets created or destroyed.
    Component::getObject().connectComponentChange( sigc::mem_fun( this, 
        &Animation::componentChange ) );

    mUpdateSignal = GlobalsBase::mFrameSignal->connect( sigc::mem_fun( this, 
        &Animation::update ) );
    mUpdateSignal.block( true );
}

Animation::~Animation()
{
    if( mAnimationState ) mAnimationState->setEnabled( false );
}

void Animation::setAnimationName( const String& rAnimationName )
{
    if( rAnimationName.empty() || mAnimationName == rAnimationName ) return;
    mAnimationName = rAnimationName;

    if( mCreated )
    {
        // TODO: Store and load properties when reloading the animation so all the properties from 
        // the previous animation state also get set onto the new animation state.

        try
        {
            ClientComponent::getObject().getComponent<Entity>().connect( sigc::mem_fun( this, 
                &Animation::resourcesLoaded ) );
        }
        catch( Exception e )
        {
            CLOGW << "Object has an animation component, but no entity component was found. " <<
                "Animation component will become active once an entity is added to this object.";
        }
    }
}

void Animation::setLength( Real length )
{
    if( !mAutoLength )
        mAnimationState->setLength( length );
}

Real Animation::getLength()
{
    return mAnimationState->getLength();
}

void Animation::create()
{
    mCreated = true;
    if( mAnimationName.empty() ) return;

    try
    {
        ClientComponent::getObject().getComponent<Entity>().connect( sigc::mem_fun( this, 
            &Animation::resourcesLoaded ) );
    }
    catch( Exception e )
    {
        CLOGW << "Object has an animation component, but no entity component was found. " <<
            "Animation component will become active once an entity is added to this object.";
    }
}

void Animation::update( Real timeElapsed )
{
    if( mAnimationState ) mAnimationState->addTime( timeElapsed * mSpeed );
}

void Animation::resourcesLoaded( Entity& rEntity )
{
    try
    {
        mAnimationState = rEntity.getEntity()->getAnimationState( mAnimationName );
        mUpdateSignal.block( false );

        // TODO: Store and load properties when reloading the animation so all the properties from 
        // the previous animation state also get set onto the new animation state.

        PropertySynchronization::processQueuedConstruction();
        PropertySynchronization::processQueue();
    }
    catch( Ogre::Exception e )
    {
    	
    }
}

void Animation::componentChange( Component& rComponent, bool created )
{
    if( rComponent.getType() == COMPONENTTYPE_ENTITY )
    {
        if( mCreated && created )
        {
            ClientComponent::getObject().getComponent<Entity>().connect( sigc::mem_fun( this, 
                &Animation::resourcesLoaded ) );
        }
        else if( mAnimationState && !created )
        {
            CLOGW << "Entity component was destroyed while an animation component was active.";
            mAnimationState = 0;
            mUpdateSignal.block( true );
        }
    }
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia