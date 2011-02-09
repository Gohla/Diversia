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

#ifndef DIVERSIA_DEFAULTCLIENT_GUIMANAGER_H
#define DIVERSIA_DEFAULTCLIENT_GUIMANAGER_H

#include "DefaultClient/Platform/Prerequisites.h"

#include "OgreClient/Input/InputManager.h"

namespace Diversia
{
namespace DefaultClient
{
//------------------------------------------------------------------------------

/**
MyGUI GUI manager.
**/
class GUIManager : public MouseListener, public KeyboardListener
{
public:
    /**
    Default constructor. 
    **/
    GUIManager();
    /**
    Destructor. 
    **/
    virtual ~GUIManager();

    /**
    Gets the menu GUI.
    **/
    inline MenuGUI* getMenuGUI() const { return mMenuGUI; }
    /**
    Gets the MyGUI GUI.
    **/
    inline MyGUI::Gui* getGui() const { return mGui; }
    /**
    Gets the MyGUI ogre platform.
    **/
    inline MyGUI::OgrePlatform* getOgrePlatform() const { return mPlatform; }
    /**
    Realign all widgets.
    **/
    void realign();
    
private:
    void windowResized( unsigned int width, unsigned int height );
    bool mouseMoved( const MouseState& rState );
    bool mousePressed( const MouseButton button );
    void mouseReleased( const MouseButton button );
    bool keyPressed( const KeyboardButton button, unsigned int text );
    void keyReleased( const KeyboardButton button, unsigned int text );
    inline int getMousePriority() const { return 1; }
    inline int getKeyboardPriority() const { return 1; }

    unsigned int            mWindowWidth, mWindowHeight;
    MyGUI::Gui*             mGui;
    MyGUI::OgrePlatform*    mPlatform;
    MenuGUI*                mMenuGUI;

};

//------------------------------------------------------------------------------
} // Namespace DefaultClient
} // Namespace Diversia

#endif // DIVERSIA_DEFAULTCLIENT_GUIMANAGER_H