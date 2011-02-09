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

#include "DefaultClient/ClientApplication.h"
#include "DefaultClient/GUI/GUIManager.h"
#include "DefaultClient/GUI/MenuGUI.h"
#include "OgreClient/Graphics/Fader.h"
#include "OgreClient/Graphics/GraphicsManager.h"
#include "Util/State/StateMachine.h"

namespace Diversia
{
namespace DefaultClient 
{
//------------------------------------------------------------------------------

MenuGUI::MenuGUI():
    mGui( ClientGlobals::mGUI->getGui() ),
    mVisible( false )
{
    // Create login widget.
    mWidgets = MyGUI::LayoutManager::getInstance().loadLayout( "Menu.layout" );
    MyGUI::ButtonPtr returnButton = mGui->findWidget<MyGUI::Button>( "ReturnToGame" );
    returnButton->eventMouseButtonClick += MyGUI::newDelegate( this, &MenuGUI::returnButtonPressed );

    MyGUI::ButtonPtr logoutButton = mGui->findWidget<MyGUI::Button>( "Logout" );
    logoutButton->eventMouseButtonClick += MyGUI::newDelegate( this, &MenuGUI::logoutButtonPressed );
    logoutButton->setEnabled( false );

    MyGUI::ButtonPtr exitButton = mGui->findWidget<MyGUI::Button>( "Exit" );
    exitButton->eventMouseButtonClick += MyGUI::newDelegate( this, &MenuGUI::exitButtonPressed );

    mWindow = mGui->findWidget<MyGUI::Widget>( "TopMenu" );
    mWindow->setVisible( false );

    ClientGlobals::mMenu = this;
}

MenuGUI::~MenuGUI()
{
    ClientGlobals::mMenu = 0;

    mGui->destroyWidgets( mWidgets );
}

void MenuGUI::setVisible( bool visibility )
{
    if( visibility == mVisible )
        return;

    mVisible = visibility;

    if( mVisible )
    {
        // Fade in the menu window.
        MyGUI::ControllerFadeAlpha* controller = MyGUI::ControllerManager::getInstance().createItem( 
            MyGUI::ControllerFadeAlpha::getClassTypeName() )->castType<MyGUI::ControllerFadeAlpha>();
        controller->setAlpha( 1 );
        controller->setCoef( 5.0 );
        controller->setEnabled( true );
        MyGUI::ControllerManager::getInstance().addItem( mWindow, controller );
    }
    else
    {
        // Fade in the menu window.
        MyGUI::ControllerFadeAlpha* controller = MyGUI::ControllerManager::getInstance().createItem( 
            MyGUI::ControllerFadeAlpha::getClassTypeName() )->castType<MyGUI::ControllerFadeAlpha>();
        controller->setAlpha( 0 );
        controller->setCoef( 5.0 );
        controller->setEnabled( true );
        controller->eventPostAction += MyGUI::newDelegate( this, &MenuGUI::fadeOutComplete );
        MyGUI::ControllerManager::getInstance().addItem( mWindow, controller );
    }
}

void MenuGUI::setVisibleInstant( bool visiblity )
{
    mWindow->setVisible( visiblity );
}

void MenuGUI::toggleVisible()
{
    MenuGUI::setVisible( !mVisible );
}

void MenuGUI::setButtonEnabled( const String& rButton, bool enabled )
{
    try
    {
        mGui->findWidget<MyGUI::Button>( rButton )->setEnabled( enabled );
    }
    catch ( ... )
    {
    }
}

void MenuGUI::returnButtonPressed( MyGUI::WidgetPtr _sender )
{
    MenuGUI::setVisible( false );
}

void MenuGUI::logoutButtonPressed( MyGUI::WidgetPtr _sender )
{
    MenuGUI::setButtonEnabled( "Logout", false );
    MenuGUI::setVisible( false );
    ClientGlobals::mGraphics->getFader()->startFadeOut( 0.5 );
    DelayedCall::create( sigc::mem_fun( ClientGlobals::mState, &StateMachine::popState ), 0.6 );
}

void MenuGUI::exitButtonPressed( MyGUI::WidgetPtr _sender )
{
    MenuGUI::setVisible( false );
    ClientGlobals::mApp->quitSoon();
}

void MenuGUI::fadeOutComplete( MyGUI::WidgetPtr _sender )
{
    MenuGUI::setVisibleInstant( false );
}

//------------------------------------------------------------------------------
} // Namespace DefaultClient
} // Namespace Diversia