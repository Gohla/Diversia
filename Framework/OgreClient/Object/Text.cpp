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

#include "OgreClient/Object/Text.h"
#include "OgreClient/Object/Entity.h"
#include "OgreClient/Graphics/TextCanvasBatch.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

Text::Text( const String& rName, Mode mode, NetworkingType networkingType, 
    RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject ):
    ClientComponent( rName, mode, networkingType, Text::getTypeStatic(), source, localOverride, 
        rObject ),
    mCreated( false ),
    mTextCanvas( 0 )
{
    PropertySynchronization::storeUserObject();

    PropertySynchronization::queue();
    if( Component::isCreatedByServer() ) PropertySynchronization::queueConstruction( true );

    // Connect to component changes to see if the entity component gets created or destroyed.
    Component::getObject().connectComponentChange( sigc::mem_fun( this, &Text::componentChange ) );
}

Text::~Text()
{
    try
    {
        GlobalsBase::mText->destroyTextCanvas( Component::getGUIDString() );
    }
    catch( Exception e )
    {
    	// Ignore error
    }
}

void Text::create()
{
    mCreated = true;

    try
    {
        Component::getObject().getComponent<Entity>().connect( sigc::mem_fun( this, 
            &Text::resourcesLoaded ) );
    }
    catch( Exception e )
    {
        CLOGW << "Object has a text component, but no entity component was found. " <<
            "Text component will become active once an entity is added to this object.";
    }
}

void Text::resourcesLoaded( Entity& rEntity )
{
    if( mTextCanvas )
    {
        mTextCanvas->setTarget( *rEntity.getEntity() );
    }
    else
    {
        mTextCanvas = &GlobalsBase::mText->createTextCanvas( Component::getGUIDString(), 
            *rEntity.getEntity() );
    }

    PropertySynchronization::processQueuedConstruction();
    PropertySynchronization::processQueue();
}

void Text::componentChange( Component& rComponent, bool created )
{
    if( rComponent.getType() == COMPONENTTYPE_ENTITY )
    {
        if( mCreated && created )
        {
            Component::getObject().getComponent<Entity>().connect( sigc::mem_fun( this, 
                &Text::resourcesLoaded ) );
        }
        else if( mTextCanvas && !created )
        {
            CLOGW << "Entity component was destroyed while a text component was active.";
            mTextCanvas->removeTarget();
        }
    }
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia