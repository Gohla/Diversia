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

#ifndef DIVERSIA_DEFAULTCLIENT_PLAYSTATE_H
#define DIVERSIA_DEFAULTCLIENT_PLAYSTATE_H

#include "DefaultClient/Platform/Prerequisites.h"

#include "Util/State/State.h"

namespace Diversia
{
namespace DefaultClient
{
//------------------------------------------------------------------------------

class PlayState : public State, public sigc::trackable
{
public:
    PlayState();
    virtual ~PlayState();

    void start();
    void suspend();
    void wake();

private:
    void activeServerDisconnected( ServerAbstract& rActiveServer );

};

//------------------------------------------------------------------------------
} // Namespace DefaultClient
} // Namespace Diversia

#endif // DIVERSIA_DEFAULTCLIENT_PLAYSTATE_H