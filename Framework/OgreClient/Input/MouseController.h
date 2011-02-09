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

#ifndef DIVERSIA_OGRECLIENT_MOUSECONTROLLER_H
#define DIVERSIA_OGRECLIENT_MOUSECONTROLLER_H

#include "OgreClient/Platform/Prerequisites.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

/**
Mouse controller interface.
**/
class DIVERSIA_OGRECLIENT_API MouseController
{
public:
    /**
    Default constructor. 
    **/
    MouseController() {}
    /**
    Destructor. 
    **/
    virtual ~MouseController() {}

    /**
    Creates a mouse controller implementation for the current OS. 
    
    @param  windowHandler   The window handler. 
    
    @return Mouse controller implementation, or 0 if no implementation could be made.
    **/
    static MouseController* createMouseController( size_t windowHandler );

    /**
    Grabs the mouse cursor so that it cannot leave the window.
    **/
    virtual void grab() = 0;
    /**
    Ungrabs the mouse cursor.
    **/
    virtual void ungrab() = 0;
    /**
    Hides the mouse cursor and stores the current position.
    **/
    virtual void hide() = 0;
    /**
    Shows the mouse cursor and restores the position to the position stored in hide().
    **/
    virtual void show() = 0;
    /**
    Sets the visibility of the mouse cursor.
    
    @param  visible True to show, false to hide. 
    **/
    virtual void setVisible( bool visible ) = 0;
    /**
    Sets the position of the mouse cursor. Position (0,0) is at the top left corner, 
    position (window width,window height) is at the bottom right corner.
    
    @param  left    The left position
    @param  top     The top position
    **/
    virtual void setPosition( unsigned int left, unsigned int top ) = 0;

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

#endif // DIVERSIA_OGRECLIENT_MOUSECONTROLLER_H