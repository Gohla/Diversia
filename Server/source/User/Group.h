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

#ifndef DIVERSIA_SERVER_GROUP_H
#define DIVERSIA_SERVER_GROUP_H

#include "Platform/Prerequisites.h"

namespace Diversia
{
namespace Server
{
//------------------------------------------------------------------------------

class Group
{
public:
    /**
    Constructor. 
    
    @param  rName   The group name. 
    **/
    Group( const String& rName );
    /**
    Destructor. 
    **/
    ~Group();

private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    String mName;

};

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Server::Group, 
    &Diversia::Server::Bindings::CampBindings::bindGroup );

#endif // DIVERSIA_SERVER_GROUP_H