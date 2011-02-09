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

#include "OgreClient/Input/MouseController.h"

#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32
#   include "OgreClient/Input/WindowsMouseController.h"
#elif DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_LINUX

#elif DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_APPLE

#else

#endif

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

MouseController* MouseController::createMouseController( size_t windowHandler )
{
#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32
    return new WindowsMouseController( (HWND)windowHandler );
#elif DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_LINUX
    SLOGW << "Cannot create linux mouse controller, OS not supported.";
#elif DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_APPLE
    SLOGW << "Cannot create apple mouse controller, OS not supported.";
#else
    SLOGW << "Cannot create mouse controller, OS not supported.";
#endif

    return 0;
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia