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

#ifndef DIVERSIA_OGRECLIENT_WINDOWSMOUSECONTROLLER_H
#define DIVERSIA_OGRECLIENT_WINDOWSMOUSECONTROLLER_H

#include "OgreClient/Platform/Prerequisites.h"

#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32

#include "OgreClient/Input/MouseController.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API WindowsMouseController : public MouseController
{
public:
    WindowsMouseController( HWND windowHandle );

    /**
    Grabs the mouse cursor so that it cannot leave the window.
    **/
    void grab();
    /**
    Ungrabs the mouse cursor.
    **/
    void ungrab();
    /**
    Hides the mouse cursor and stores the current position.
    **/
    void hide();
    /**
    Shows the mouse cursor and restores the position to the position stored in hide().
    **/
    void show();
    /**
    Sets the visibility of the mouse cursor.
    
    @param  visible True to show, false to hide. 
    **/
    void setVisible( bool visible );
    /**
    Sets the position of the mouse cursor. Position (0,0) is at the top left corner, 
    position (window width,window height) is at the bottom right corner.
    
    @param  left    The left position
    @param  top     The top position
    **/
    void setPosition( unsigned int left, unsigned int top );
    
private:
    HWND    mWindowHandle;
    POINT   mLastCursorPos;
        
};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

#endif // DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32

#endif // DIVERSIA_OGRECLIENT_WINDOWSMOUSECONTROLLER_H