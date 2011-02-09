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

#ifndef DIVERSIA_DEFAULTCLIENT_OISINPUTMANAGER_H
#define DIVERSIA_DEFAULTCLIENT_OISINPUTMANAGER_H

#include "DefaultClient/Platform/Prerequisites.h"

#include "OgreClient/Input/InputManager.h"

namespace Diversia
{
namespace DefaultClient
{
//------------------------------------------------------------------------------

/**
InputManager implementation using OIS. 
**/
class OISInputManager : public OIS::MouseListener, public OIS::KeyListener, public InputManager
{
public:
    /**
    Default constructor. 
    **/
    OISInputManager();
    /**
    Destructor. 
    **/
    ~OISInputManager();

    inline OIS::InputManager* getInputManager() const { return mInputManager; }
    inline OIS::Mouse* getMouse() const { return mMouse; }
    inline OIS::Keyboard* getKeyboard() const { return mKeyboard; }

    bool mouseMoved( const OIS::MouseEvent& rEvent );
    bool mousePressed( const OIS::MouseEvent& rEvent, OIS::MouseButtonID id );
    bool mouseReleased( const OIS::MouseEvent& rEvent, OIS::MouseButtonID id );
    bool keyPressed( const OIS::KeyEvent& rEvent );
    bool keyReleased( const OIS::KeyEvent& rEvent );

private:
    void update();
    void windowResized( unsigned int width, unsigned int height );

    OIS::InputManager*                  mInputManager;
    OIS::Keyboard*                      mKeyboard;
    OIS::Mouse*                         mMouse;

};
//------------------------------------------------------------------------------
} // Namespace DefaultClient
} // Namespace Diversia

#endif // DIVERSIA_DEFAULTCLIENT_OISINPUTMANAGER_H