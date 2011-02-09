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

#ifndef DIVERSIA_OGRECLIENT_GAMEMODEINTERFACE_H
#define DIVERSIA_OGRECLIENT_GAMEMODEINTERFACE_H

#include "OgreClient/Platform/Prerequisites.h"
#include "OgreClient/GameMode/GameModePlugin.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API GameModeBase
{
public:
    GameModeBase( GameModePlugin& rGameModePlugin ): mGameModePlugin( rGameModePlugin ) {}
    virtual ~GameModeBase() {}

protected:
    friend class GameModePlugin;

    virtual void setServerState( ServerState serverState ) = 0;
    
    GameModePlugin& mGameModePlugin;

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

#endif // DIVERSIA_OGRECLIENT_GAMEMODEINTERFACE_H