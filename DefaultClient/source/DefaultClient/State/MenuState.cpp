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

#include "Client/Communication/GridManager.h"
#include "DefaultClient/GUI/LoginGUI.h"
#include "DefaultClient/State/LoadingState.h"
#include "DefaultClient/State/MenuState.h"
#include "OgreClient/Graphics/Fader.h"
#include "OgreClient/Graphics/GraphicsManager.h"
#include <sigc++/adaptors/retype_return.h>

namespace Diversia
{
namespace DefaultClient 
{
//------------------------------------------------------------------------------

MenuState::MenuState():
    mLoginGUI( 0 )
{

}

MenuState::~MenuState()
{
    destroyLoginUI();
}

void MenuState::start()
{
    createLoginUI();
    mLoginGUI->fadeIn();
}

void MenuState::suspend()
{
    mLoginGUI->fadeOut();
}

void MenuState::wake()
{
    ClientGlobals::mGraphics->getFader()->startFadeIn( 0.5 );
    createLoginUI();
    mLoginGUI->fadeIn();
}

void MenuState::createLoginUI()
{
    if( mLoginGUI )
    {
        destroyLoginUI();
    }

    mLoginGUI = new LoginGUI();
    mLoginGUI->connectLogin( sigc::mem_fun( this, &MenuState::login ) );
    mLoginGUI->connectFadeOut( sigc::mem_fun( this, &MenuState::destroyLoginUI ) );
}

void MenuState::destroyLoginUI()
{
    if( mLoginGUI )
    {
        delete mLoginGUI;
        mLoginGUI = 0;
    }
}

void MenuState::login( String serverAddress, unsigned short port, String userName, String password, 
    String nickname )
{
    // Connect to server and go into loading state.
    ClientGlobals::mGraphics->getFader()->startFadeOut( 0.5 );
    DelayedCall::create( sigc::hide_return( sigc::bind( sigc::mem_fun( ClientGlobals::mGrid, &GridManager::createServer ), 
        GridPosition( 0, 0 ), ServerInfo( serverAddress, port ), UserInfo( nickname, userName, password ) ) ), 0.6 );
    DelayedCall::create( sigc::bind( sigc::mem_fun( ClientGlobals::mState, &StateMachine::pushState ), 
        new LoadingState() ), 0.6 );
}

//------------------------------------------------------------------------------
} // Namespace DefaultClient
} // Namespace Diversia