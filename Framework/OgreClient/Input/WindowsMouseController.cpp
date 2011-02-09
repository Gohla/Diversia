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

#include "OgreClient/Platform/StableHeaders.h"

#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32

#include "OgreClient/Input/WindowsMouseController.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

WindowsMouseController::WindowsMouseController( HWND windowHandle ): 
    mWindowHandle( windowHandle )
{
    mLastCursorPos.x = 0; mLastCursorPos.y = 0;
}

void WindowsMouseController::grab()
{
    RECT clipRegion;
    GetWindowRect( mWindowHandle, &clipRegion );
    clipRegion.left += 10;
    clipRegion.right -= 10;
    clipRegion.top += 33;
    clipRegion.bottom -= 10;
    ClipCursor( &clipRegion );
}

void WindowsMouseController::ungrab()
{
    ClipCursor( NULL );
}

void WindowsMouseController::hide()
{
    GetCursorPos( &mLastCursorPos );
    setVisible( false );
}

void WindowsMouseController::show()
{
    SetCursorPos( mLastCursorPos.x, mLastCursorPos.y );
    setVisible( true );
}

void WindowsMouseController::setVisible( bool visible )
{
    if( visible )
        while( ShowCursor( true ) < 0 );
    else
        while( ShowCursor( false ) >= 0 );
}

void WindowsMouseController::setPosition( unsigned int left, unsigned int top )
{
    SetCursorPos( left, top );
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

#endif // DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32