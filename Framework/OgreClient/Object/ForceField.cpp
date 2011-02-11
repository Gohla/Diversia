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

#include "OgreClient/Object/AreaTrigger.h"
#include "OgreClient/Object/ForceField.h"
#include "OgreClient/Object/RigidBody.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

ForceField::ForceField( const String& rName, Mode mode, NetworkingType networkingType, 
    RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject ):
    ClientComponent( rName, mode, networkingType, ForceField::getTypeStatic(), source, localOverride, 
        rObject ),
    mObjectsInArea( 0 ),
    mEnabled( true ),
    mForce( 0, 0, 0 )
{
    PropertySynchronization::storeUserObject();

    // Connect to component changes to see if the area trigger component gets created or destroyed.
    Component::getObject().connectComponentChange( sigc::mem_fun( this, &ForceField::componentChange ) );

    mUpdateConnection = GlobalsBase::mUpdateSignal->connect( sigc::mem_fun( this, &ForceField::update ) );
    if( Component::getObject().hasComponent( COMPONENTTYPE_AREATRIGGER ) )
    {
        mObjectsInArea = &Component::getObject().getComponent<AreaTrigger>().getObjectsInArea();
    }
    else
        mUpdateConnection.block( true );
}

ForceField::~ForceField()
{

}

void ForceField::setEnabled( bool enabled )
{
    mEnabled = enabled;
    if( !mObjectsInArea ) return;
    mUpdateConnection.block( !enabled );
}

void ForceField::setForce( const Vector3& rForce )
{
    mForce = toVector3<btVector3>( rForce );
}

Diversia::Util::Vector3 ForceField::getForce() const
{
    return toVector3<Vector3>( mForce );
}

void ForceField::update()
{
    for( ObjectsInArea::const_iterator i = mObjectsInArea->begin(); i != mObjectsInArea->end(); ++i )
    {
        i->second->getComponent<RigidBody>().getRigidBody()->applyCentralForce( mForce );
    }
}

void ForceField::componentChange( Component& rComponent, bool created )
{
    if( rComponent.getType() == COMPONENTTYPE_AREATRIGGER )
    {
        if( created )
        {
            mObjectsInArea = &Component::getObject().getComponent<AreaTrigger>().getObjectsInArea();
            if( mEnabled ) mUpdateConnection.block( false ); else mUpdateConnection.block( true );
        }
        else
        {
            mObjectsInArea = 0;
            mUpdateConnection.block( true );
            CLOGW << "Area trigger component was destroyed while a force field component was active.";
        }
    }
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia