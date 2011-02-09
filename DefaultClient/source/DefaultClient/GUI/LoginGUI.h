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

#ifndef DIVERSIA_DEFAULTCLIENT_LOGINGUI_H
#define DIVERSIA_DEFAULTCLIENT_LOGINGUI_H

#include "DefaultClient/Platform/Prerequisites.h"

#include "Shared/Communication/ServerInfo.h"

namespace Diversia
{
namespace DefaultClient
{
//------------------------------------------------------------------------------

typedef std::vector<ServerInfo> Servers;

class LoginGUI : public sigc::trackable
{
public:
    /**
    Constructor. 
    
    @param [in,out] rGUIManager GUI manager
    **/
    LoginGUI();
    /**
    Destructor. 
    **/
    ~LoginGUI();

    /**
    Fades the login window in.
    **/
    void fadeIn();
    /**
    Fades the login window out.
    **/
    void fadeOut();
    /**
    Sets the visibility of the login window.
    
    @param  visible True to show, false to hide. 
    **/
    void setVisible( bool visible );

    /**
    Connects a slot to the login signal.
    
    @param [in,out] rSlot   The slot (signature: void(String [server address], 
                            unsigned short [server port], String [user name], String [password], 
                            String [nickname])) to connect.
    
    @return Connection object to block or disconnect the connection.
    **/
    sigc::connection connectLogin( 
        const sigc::slot<void, String, unsigned short, String, String, String>& rSlot ) 
    {
        return mLoginSignal.connect( rSlot );
    }

    /**
    Connects a slot to the fade out completed signal.
    
    @param [in,out] rSlot   The slot (signature: void()) to connect.
    
    @return Connection object to block or disconnect the connection.
    **/
    sigc::connection connectFadeOut( const sigc::slot<void>& rSlot ) 
    {
        return mFadeOutSignal.connect( rSlot );
    }
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    void loginButtonPressed( MyGUI::WidgetPtr _sender );
    void fadeOutComplete( MyGUI::WidgetPtr _sender );

    MyGUI::Gui*                 mGui;
    MyGUI::VectorWidgetPtr      mWidgets;
    MyGUI::Widget*              mWindow;
    MyGUI::ControllerFadeAlpha* mFadeOutController;

    sigc::signal<void, String, unsigned short, String, String, String> mLoginSignal;
    sigc::signal<void> mFadeOutSignal;

    /**
    Settings for login GUI.
    **/
    static struct Settings
    {
        Servers     mServers;
        ServerInfo  mLastServer;
        String      mLastUserName;
        String      mLastNickname;
    } msSettings;

public:
    /**
    Gets the settings. 
    **/
    inline static LoginGUI::Settings& getSettings() { return msSettings; }

};

//------------------------------------------------------------------------------
} // Namespace DefaultClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::DefaultClient::LoginGUI::Settings, 
    &Diversia::DefaultClient::Bindings::CampBindings::bindLoginGUISettings );

#endif // DIVERSIA_DEFAULTCLIENT_LOGINGUI_H