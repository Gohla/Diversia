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

#include "DefaultClient/Platform/StableHeaders.h"

#include "DefaultClient/State/LoadingState.h"
#include "DefaultClient/State/PlayState.h"
#include "Client/Communication/GridManager.h"

namespace Diversia
{
namespace DefaultClient 
{
//------------------------------------------------------------------------------

LoadingState::LoadingState()
{
    ClientGlobals::mGrid->connectLoadingCompleted( sigc::mem_fun( this, &LoadingState::loadingComplete ) );
    ClientGlobals::mGrid->connectActiveServerDisconnect( sigc::mem_fun( this, 
        &LoadingState::activeServerDisconnected ) );
}

LoadingState::~LoadingState()
{

}

void LoadingState::start()
{
    mActive = true;
}

void LoadingState::suspend()
{
    mActive = false;
}

void LoadingState::wake()
{
    // This state cannot be waked up, immediately pop this state.
    mActive = true;
    ClientGlobals::mState->popState();
}

void LoadingState::loadingComplete()
{
    LOGI << "Loading complete!";
    ClientGlobals::mState->pushState( new PlayState() );
}

void LoadingState::activeServerDisconnected( ServerAbstract& rActiveServer )
{
    if( mActive )
    {
        LOGE << "Active server disconnected or failed to connect, logging out.";
        ClientGlobals::mGrid->clear();
        ClientGlobals::mState->popState();
    }
}

//------------------------------------------------------------------------------
} // Namespace DefaultClient
} // Namespace Diversia