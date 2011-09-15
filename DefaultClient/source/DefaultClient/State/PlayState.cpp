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

#include "Client/Plugin/ClientPluginManager.h"
#include "Client/Communication/GridManager.h"
#include "Client/Communication/ServerAbstract.h"
#include "DefaultClient/GUI/MenuGUI.h"
#include "DefaultClient/State/PlayState.h"
#include "OgreClient/Graphics/Fader.h"
#include "OgreClient/Graphics/GraphicsManager.h"

namespace Diversia
{
namespace DefaultClient 
{
//------------------------------------------------------------------------------

PlayState::PlayState()
{
    ClientGlobals::mGrid->connectActiveServerDisconnect( sigc::mem_fun( this, 
        &PlayState::activeServerDisconnected ) );
}

PlayState::~PlayState()
{
    ClientGlobals::mGrid->clear();
    ClientGlobals::mMenu->setButtonEnabled( "Logout", false );
}

void PlayState::start()
{
    LOGI << "Entering play state.";
    GlobalsBase::mGrid->getActiveServer().getPluginManager().setState( PLAY );

    ClientGlobals::mGraphics->getFader()->startFadeIn();

    ClientGlobals::mMenu->setButtonEnabled( "Logout", true );
}

void PlayState::suspend()
{

}

void PlayState::wake()
{

}

void PlayState::activeServerDisconnected( ServerAbstract& rActiveServer )
{
    LOGE << "Active server disconnected or failed to connect, logging out.";
    ClientGlobals::mMenu->setButtonEnabled( "Logout", false );
    ClientGlobals::mGraphics->getFader()->startFadeOut( 0.5 );
    DelayedCall::create( sigc::mem_fun( ClientGlobals::mState, &StateMachine::popState ), 0.6 );
}

//------------------------------------------------------------------------------
} // Namespace DefaultClient
} // Namespace Diversia