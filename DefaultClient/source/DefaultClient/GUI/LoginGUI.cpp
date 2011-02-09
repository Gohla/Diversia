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

#include "DefaultClient/GUI/LoginGUI.h"
#include "DefaultClient/GUI/GUIManager.h"

namespace Diversia
{
namespace DefaultClient 
{
//------------------------------------------------------------------------------

LoginGUI::Settings LoginGUI::msSettings = LoginGUI::Settings();

LoginGUI::LoginGUI():
    mGui( ClientGlobals::mGUI->getGui() ),
    mFadeOutController( 0 )
{
    // Create login widget.
    mWidgets = MyGUI::LayoutManager::getInstance().loadLayout( "LoginGUI.layout" );
    MyGUI::ButtonPtr loginButton = mGui->findWidget<MyGUI::Button>( "buttonLogin" );
    loginButton->eventMouseButtonClick += MyGUI::newDelegate( this, &LoginGUI::loginButtonPressed );

    // Load settings into login window.
    mGui->findWidget<MyGUI::Edit>( "editServerAddress" )->setOnlyText( 
        msSettings.mLastServer.mAddress );
    mGui->findWidget<MyGUI::Edit>( "editServerPort" )->setOnlyText( 
        boost::lexical_cast<String>( msSettings.mLastServer.mPort ) );
    mGui->findWidget<MyGUI::Edit>( "editUserName" )->setOnlyText( msSettings.mLastUserName );
    mGui->findWidget<MyGUI::Edit>( "editNickname" )->setOnlyText( msSettings.mLastNickname );
    mWindow = mGui->findWidget<MyGUI::Widget>( "windowServerLogin" );

    // Center the login widget.
    ClientGlobals::mGUI->realign();
}

LoginGUI::~LoginGUI()
{
    mGui->destroyWidgets( mWidgets );
}

void LoginGUI::fadeIn()
{
    // Fade out the login window.
    LoginGUI::setVisible( false );
    MyGUI::ControllerFadeAlpha* controller = MyGUI::ControllerManager::getInstance().createItem( 
        MyGUI::ControllerFadeAlpha::getClassTypeName() )->castType<MyGUI::ControllerFadeAlpha>();
    controller->setAlpha( 1 );
    controller->setCoef( 2 );
    controller->setEnabled( true );
    MyGUI::ControllerManager::getInstance().addItem( mWindow, controller );
}

void LoginGUI::fadeOut()
{
    if( !mFadeOutController )
    {
        // Fade out the login window.
        mFadeOutController = MyGUI::ControllerManager::getInstance().createItem( 
            MyGUI::ControllerFadeAlpha::getClassTypeName() )->castType<MyGUI::ControllerFadeAlpha>();
        mFadeOutController->setAlpha( 0 );
        mFadeOutController->setCoef( 2 );
        mFadeOutController->setEnabled( true );
        mFadeOutController->eventPostAction += MyGUI::newDelegate( this, 
            &LoginGUI::fadeOutComplete );
        MyGUI::ControllerManager::getInstance().addItem( mWindow, mFadeOutController );
    }
}

void LoginGUI::setVisible( bool visible )
{
    mWindow->setVisible( visible );
}

void LoginGUI::loginButtonPressed( MyGUI::WidgetPtr _sender )
{
    // Get values
    String address = mGui->findWidget<MyGUI::Edit>( "editServerAddress" )->getOnlyText();
    unsigned short port = boost::lexical_cast<unsigned short>( 
        mGui->findWidget<MyGUI::Edit>( "editServerPort" )->getOnlyText() );
    String userName = mGui->findWidget<MyGUI::Edit>( "editUserName" )->getOnlyText();
    String password = mGui->findWidget<MyGUI::Edit>( "editUserPassword" )->getOnlyText();
    String nickname = mGui->findWidget<MyGUI::Edit>( "editNickname" )->getOnlyText();
    msSettings.mLastServer.mAddress = address;
    msSettings.mLastServer.mPort = port;
    msSettings.mLastUserName = userName;
    msSettings.mLastNickname = nickname;

    // Disable all widgets.
    for( MyGUI::VectorWidgetPtr::iterator i = mWidgets.begin(); i != mWidgets.end(); ++i )
    {
        (*i)->setEnabled( false );
    }

    // Emit login signal.
    mLoginSignal( address, port, userName, password, nickname );
}

void LoginGUI::fadeOutComplete( MyGUI::WidgetPtr _sender )
{
    mFadeOutController = 0;
    mFadeOutSignal();
}

//------------------------------------------------------------------------------
} // Namespace DefaultClient
} // Namespace Diversia