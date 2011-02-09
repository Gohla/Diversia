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

#include "DefaultClient/Input/OISInputManager.h"
#include "OgreClient/Input/WindowsMouseController.h"
#include "OgreClient/Graphics/GraphicsManager.h"

namespace Diversia
{
namespace DefaultClient 
{
//------------------------------------------------------------------------------

OISInputManager::OISInputManager():
    OIS::MouseListener(),
    OIS::KeyListener(),
    InputManager( GlobalsBase::mGraphics->getWindow()->getWidth(), 
        GlobalsBase::mGraphics->getWindow()->getHeight() )
{
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;
    OIS::ParamList pl;
    GlobalsBase::mGraphics->getWindow()->getCustomAttribute( "WINDOW", &windowHnd );

    // Use OS mouse.
    windowHndStr << windowHnd;
    pl.insert( std::make_pair( std::string( "WINDOW" ), windowHndStr.str() ) );

#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32
    pl.insert( std::make_pair(
        std::string( "w32_mouse" ),
        std::string( "DISCL_FOREGROUND" ) ) );
    pl.insert( std::make_pair(
        std::string( "w32_mouse" ),
        std::string( "DISCL_NONEXCLUSIVE" ) ) );
    pl.insert( std::make_pair(
        std::string( "w32_keyboard" ),
        std::string( "DISCL_FOREGROUND" ) ) );
    pl.insert( std::make_pair(
        std::string( "w32_keyboard" ),
        std::string( "DISCL_NONEXCLUSIVE" ) ) );
#elif DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_LINUX
    pl.insert( std::make_pair(
        std::string( "x11_mouse_grab" ),
        std::string( "false" ) ) );
    pl.insert( std::make_pair(
        std::string( "x11_mouse_hide" ),
        std::string( "false" ) ) );
    pl.insert( std::make_pair(
        std::string( "x11_keyboard_grab" ),
        std::string( "false" ) ) );
    pl.insert( std::make_pair(
        std::string( "XAutoRepeatOn" ),
        std::string( "true" ) ) );
#else
    LOGW << "Cannot set mouse and keyboard features, unsupported OS".;
#endif

    mInputManager = OIS::InputManager::createInputSystem( pl );
    mKeyboard = static_cast<OIS::Keyboard*>( mInputManager->createInputObject( 
        OIS::OISKeyboard, true ) );
    mMouse = static_cast<OIS::Mouse*>( mInputManager->createInputObject( 
        OIS::OISMouse, true ) );

    mKeyboard->setEventCallback( this );
    mMouse->setEventCallback( this );

    GlobalsBase::mUpdateSignal->connect( sigc::mem_fun( this, &OISInputManager::update ) );
    GlobalsBase::mGraphics->connectWindowResized( sigc::mem_fun( this, 
        &OISInputManager::windowResized ) );
    OISInputManager::windowResized( InputManager::getWindowWidth(), 
        InputManager::getWindowHeight() );

    InputManager::setMouseController( MouseController::createMouseController( windowHnd ) );
}

OISInputManager::~OISInputManager()
{
    mInputManager->destroyInputObject( mKeyboard );
    mInputManager->destroyInputObject( mMouse );
    OIS::InputManager::destroyInputSystem( mInputManager );
}

void OISInputManager::update()
{
    mKeyboard->capture();
    mMouse->capture();
}

void OISInputManager::windowResized( unsigned int width, unsigned int height )
{
    InputManager::setWindowSize( width, height );

    const OIS::MouseState& mouseState = mMouse->getMouseState();
    mouseState.width = width;
    mouseState.height = height;
}

bool OISInputManager::mouseMoved( const OIS::MouseEvent& rEvent )
{
    return InputManager::mouseMoved( 
        Axis( rEvent.state.X.abs, rEvent.state.X.rel ), 
        Axis( rEvent.state.Y.abs, rEvent.state.Y.rel ), 
        Axis( rEvent.state.Z.abs, rEvent.state.Z.rel ) );
}

bool OISInputManager::mousePressed( const OIS::MouseEvent& rEvent, OIS::MouseButtonID id )
{
    return InputManager::mousePressed( (MouseButton)id );
}

bool OISInputManager::mouseReleased( const OIS::MouseEvent &rEvent, OIS::MouseButtonID id )
{
    return InputManager::mouseReleased( (MouseButton)id );
}

bool OISInputManager::keyPressed( const OIS::KeyEvent &rEvent )
{
    return InputManager::keyPressed( (KeyboardButton)rEvent.key, rEvent.text );
}

bool OISInputManager::keyReleased( const OIS::KeyEvent &rEvent )
{
    return InputManager::keyReleased( (KeyboardButton)rEvent.key, rEvent.text );
}

//------------------------------------------------------------------------------
} // Namespace DefaultClient
} // Namespace Diversia