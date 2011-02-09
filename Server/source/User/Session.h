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

#ifndef DIVERSIA_SERVER_SESSION_H
#define DIVERSIA_SERVER_SESSION_H

#include "Platform/Prerequisites.h"

namespace Diversia
{
namespace Server
{
//------------------------------------------------------------------------------

class Session
{
public:
    /**
    Constructor. 
    
    @param  guid            Unique session identifier. 
    @param [in,out] rUser   The user that's associated with the session.
    **/
    Session( RakNet::RakNetGUID guid, User& rUser );
    /**
    Destructor. 
    **/
    ~Session();

    /**
    Gets the user that's associated with this session.
    **/
    User& getUser() const { return mUser; }

private:
    RakNet::RakNetGUID  mGUID;
    User&               mUser;

};

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia

#endif // DIVERSIA_SERVER_SESSION_H
