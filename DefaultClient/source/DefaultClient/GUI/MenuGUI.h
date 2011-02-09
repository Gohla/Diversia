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

#ifndef DIVERSIA_DEFAULTCLIENT_MENUGUI_H
#define DIVERSIA_DEFAULTCLIENT_MENUGUI_H

#include "DefaultClient/Platform/Prerequisites.h"

namespace Diversia
{
namespace DefaultClient
{
//------------------------------------------------------------------------------

class MenuGUI
{
public:
    /**
    Default constructor. 
    **/
    MenuGUI();
    /**
    Destructor. 
    **/
    ~MenuGUI();

    /**
    Sets the visibility of the menu, fades in and out.
    
    @param  visibility  True to show, false to hide.
    **/
	void setVisible( bool visibility );
    /**
    Sets the visibility of the menu instantly. 

    @param  visibility  True to show, false to hide.
    **/
    void setVisibleInstant( bool visiblity );
    /**
    Toggles visibility. 
    **/
    void toggleVisible();
    /**
    Sets a button to enabled or disabled.
    
    @param  rButton The name of the button.
    @param  enabled True to enable, false to disable. 
    **/
    void setButtonEnabled( const String& rButton, bool enabled );
    
private:
    void returnButtonPressed( MyGUI::WidgetPtr _sender );
    void logoutButtonPressed( MyGUI::WidgetPtr _sender );
    void exitButtonPressed( MyGUI::WidgetPtr _sender );
    void fadeOutComplete( MyGUI::WidgetPtr _sender );

    MyGUI::Gui*                 mGui;
    MyGUI::VectorWidgetPtr      mWidgets;
    MyGUI::Widget*              mWindow;

    bool                        mVisible;

};

//------------------------------------------------------------------------------
} // Namespace DefaultClient
} // Namespace Diversia

#endif // DIVERSIA_DEFAULTCLIENT_MENUGUI_H