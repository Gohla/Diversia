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

#include "OgreClient/Input/InputManager.h"
#include "OgreClient/Input/MouseController.h"
#include "Shared/Lua/LuaManager.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

InputManager::InputManager( unsigned int windowWidth /*= 0*/, unsigned int windowHeight /*= 0*/ ):
    mWindowWidth( windowWidth ), 
    mWindowHeight( windowHeight ),
    mMouseClaimer( 0 ),
    mMouseClaimed( false )
{
    GlobalsBase::mInput = this;
    LuaManager::connectCreated( sigc::mem_fun( this, &InputManager::luaManagerCreated ) );
}

InputManager::~InputManager()
{
    GlobalsBase::mInput = 0;
}

void InputManager::claimMouse( bool claim, MouseListener& rMouseListener )
{
    if( !mMouseClaimed && claim )
    {
        // Mouse has not been claimed yet and will be claimed now.
        mMouseClaimer = &rMouseListener;
        mMouseClaimed = true;
        if( mMouseController ) mMouseController->hide();
    }
    else if( mMouseClaimed && claim && 
        rMouseListener.getMousePriority() < mMouseClaimer->getMousePriority() )
    {
        // Mouse is already claimed but an object with a higher priority wants to claim the mouse.
        mMouseClaimer = &rMouseListener;
        mMouseClaimed = true;
        if( mMouseController ) mMouseController->hide();
    }
    else if( !claim && &rMouseListener == mMouseClaimer )
    {
        // Mouse is claimed and the owner of the claim wants to remove the claim.
        mMouseClaimer = 0;
        mMouseClaimed = false;
        if( mMouseController ) mMouseController->show();
    }
}

void InputManager::clearMouseClaim()
{
    mMouseClaimer = 0;
    mMouseClaimed = false;
    if( mMouseController ) mMouseController->setVisible( true );
}

void InputManager::subscribeMouse( MouseListener& rMouseListener )
{
    MouseListeners::iterator i = mMouseListeners.find( &rMouseListener );
    if( i == mMouseListeners.end() )
    {
        mMouseListeners.insert( &rMouseListener );
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_DUPLICATE_ITEM, 
            "There is already a mouse listener for that priority.", 
            "InputManager::subscribeMouse" );
    }
}

void InputManager::unsubscribeMouse( MouseListener& rMouseListener )
{
    mMouseListeners.erase( &rMouseListener );
}

void InputManager::subscribeKeyboard( KeyboardListener& rKeyboardListener )
{
    KeyboardListeners::iterator i = mKeyboardListeners.find( &rKeyboardListener );
    if( i == mKeyboardListeners.end() )
    {
        mKeyboardListeners.insert( &rKeyboardListener );
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_DUPLICATE_ITEM, 
            "There is already a keyboard listener for that priority.", 
            "InputManager::subscribeKeyboard" );
    }
}

void InputManager::unsubscribeKeyboard( KeyboardListener& rKeyboardListener )
{
    mKeyboardListeners.erase( &rKeyboardListener );
}

void InputManager::setWindowSize( unsigned int width, unsigned int height )
{
    mWindowWidth = width;
    mWindowHeight = height;
}

void InputManager::setMouseController( MouseController* pMouseController )
{
    mMouseController.reset( pMouseController );
}

bool InputManager::mouseMoved( int x, int y, int z )
{
    mMouseState.x.abs = x;
    mMouseState.y.abs = y;
    mMouseState.z.abs = z;

    InputManager::calculateMouseRelative();
    return InputManager::fireMouseMoved();
}

bool InputManager::mouseMoved( int x, int y )
{
    mMouseState.x.abs = x;
    mMouseState.y.abs = y;

    InputManager::calculateMouseRelative();
    return InputManager::fireMouseMoved();
}

bool InputManager::mouseMoved( int z )
{
    // Z is relative, so add it to the absolute value instead of setting it.
    mMouseState.z.abs += z;

    InputManager::calculateMouseRelative();
    return InputManager::fireMouseMoved();
}

bool InputManager::mouseMoved( const Axis& x, const Axis& y, const Axis& z )
{
    mMouseState.x = x;
    mMouseState.y = y;
    mMouseState.z = z;

    mPrevMouseState = mMouseState;

    return InputManager::fireMouseMoved();
}

void InputManager::calculateMouseRelative()
{
    mMouseState.x.rel = mMouseState.x.abs - mPrevMouseState.x.abs;
    mMouseState.y.rel = mMouseState.y.abs - mPrevMouseState.y.abs;
    mMouseState.z.rel = mMouseState.z.abs - mPrevMouseState.z.abs;

    mPrevMouseState = mMouseState;
}

bool InputManager::fireMouseMoved()
{
    if( mMouseClaimed )
    {
        mMouseClaimer->mouseMoved( mMouseState );
    }
    else
    {
        MouseListeners::iterator i;
        for( i = mMouseListeners.begin(); i != mMouseListeners.end(); ++i )
        {
            if( (*i)->mouseMoved( mMouseState ) )
            {
                return true;
            }
        }
    }

    return true;
}

bool InputManager::mousePressed( const MouseButton button )
{
    mMouseState.buttons[ button ] = true;

    // Only detect clicks inside the screen
    if( mMouseState.x.abs != 0 && mMouseState.x.abs != mWindowWidth && 
        mMouseState.y.abs != 0 && mMouseState.y.abs != mWindowHeight )

    {
        if( !mMouseClaimed && mMouseController )
        {
            mMouseController->grab();
        }

        MouseListeners::iterator i;
        for( i = mMouseListeners.begin(); i != mMouseListeners.end(); ++i )
        {
            if( ( *i )->mousePressed( button ) )
            {
                return true;
            }
        }
    }

    return false;
}

bool InputManager::mouseReleased( const MouseButton button )
{
    mMouseState.buttons[ button ] = false;

    // Only release the mouse if no buttons are pressed down.
    if( mMouseController && mMouseState.buttons == false ) mMouseController->ungrab();

    MouseListeners::iterator i;
    for( i = mMouseListeners.begin(); i != mMouseListeners.end(); ++i )
    {
        (*i)->mouseReleased( button );
    }

    return true;
}

bool InputManager::keyPressed( const KeyboardButton button, unsigned int text )
{
    mKeyboardState.buttons[ button ] = true;

    KeyboardListeners::iterator i;
    for( i = mKeyboardListeners.begin(); i != mKeyboardListeners.end(); ++i )
    {
        if( (*i)->keyPressed( button, text ) )
        {
            return true;
        }
    }

    return false;
}

bool InputManager::keyReleased( const KeyboardButton button, unsigned int text )
{
    mKeyboardState.buttons[ button ] = false;

    KeyboardListeners::iterator i;
    for( i = mKeyboardListeners.begin(); i != mKeyboardListeners.end(); ++i )
    {
        (*i)->keyReleased( button, text );
    }

    return true;
}

void InputManager::luaManagerCreated( LuaManager& rLuaManager )
{
    rLuaManager.registerEnum<MouseButton>( "MouseButton" );
    rLuaManager.registerEnum<KeyboardButton>( "KeyboardButton" );
    rLuaManager.registerClass<Axis>( "Axis" );
    rLuaManager.setSecurityLevel( "Axis", LUASEC_LOW );
    rLuaManager.registerClass<MouseState>( "MouseState" );
    rLuaManager.setSecurityLevel( "MouseState", LUASEC_LOW );
    rLuaManager.registerClass<KeyboardState>( "KeyboardState" );
    rLuaManager.setSecurityLevel( "KeyboardState", LUASEC_LOW );
    rLuaManager.object( "InputManager" ) = this;
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia