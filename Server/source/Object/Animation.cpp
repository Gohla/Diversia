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

#include "Object/Animation.h"

namespace Diversia
{
namespace Server 
{
//------------------------------------------------------------------------------

Animation::Animation( const String& rName, Mode mode, NetworkingType networkingType, 
    RakNet::RakNetGUID source, bool localOverride, ServerObject& rObject ):
    ServerComponent( rName, mode, networkingType, Animation::getTypeStatic(), source, localOverride, 
        rObject ),
    mSpeed( 1.0 ),
    mEnabled( false ),
    mLoop( true ),
    mPosition( 0.0 ),
    mLength( 1.0 ),
    mAutoLength( 1.0 ),
    mWeight( 1.0 )
{
    PropertySynchronization::storeUserObject();
}

Animation::~Animation()
{

}

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia