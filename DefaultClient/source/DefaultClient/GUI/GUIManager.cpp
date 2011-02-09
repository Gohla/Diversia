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

#include "DefaultClient/GUI/GUIManager.h"
#include "DefaultClient/GUI/MenuGUI.h"
#include "OgreClient/Graphics/GraphicsManager.h"
#include "OgreClient/Input/InputManager.h"

namespace Diversia
{
namespace DefaultClient 
{
//------------------------------------------------------------------------------

GUIManager::GUIManager():
    mWindowWidth( GlobalsBase::mGraphics->getWindow()->getWidth() ),
    mWindowHeight( GlobalsBase::mGraphics->getWindow()->getHeight() )
{
    // Load resources
    Ogre::ResourceGroupManager& rgm = Ogre::ResourceGroupManager::getSingleton();
    if( !rgm.resourceGroupExists( "MyGUI" ) || !rgm.isResourceGroupInitialised( "MyGUI" ) )
    {
        Path rootPath = GlobalsBase::mGraphics->getRootResourceLocation();
        Path myguiPath = "MyGUI";
        Path resourcePath = rootPath / myguiPath;

        rgm.addResourceLocation( resourcePath.directory_string(), "FileSystem", "MyGUI" );
        rgm.initialiseResourceGroup( "MyGUI" );

        GlobalsBase::mGraphics->addResourceLocation( OgreResourceParams( myguiPath.directory_string(),
            "MyGUI" ) );
    }

    // Load MyGUI
    mPlatform = new MyGUI::OgrePlatform();
    mPlatform->initialise( GlobalsBase::mGraphics->getWindow(), GlobalsBase::mGraphics->getSceneMgr(), "MyGUI" );
    mGui = new MyGUI::Gui();
    mGui->initialise();
    MyGUI::PointerManager::getInstance().setVisible( false );

    GlobalsBase::mInput->subscribeKeyboard( *this );
    GlobalsBase::mInput->subscribeMouse( *this );
    GlobalsBase::mGraphics->connectWindowResized( sigc::mem_fun( this, 
        &GUIManager::windowResized ) );

    ClientGlobals::mGUI = this;

    // Load menu GUI
    mMenuGUI = new MenuGUI();

    realign();
}

GUIManager::~GUIManager()
{
    delete mMenuGUI;

    GlobalsBase::mInput->unsubscribeKeyboard( *this );
    GlobalsBase::mInput->unsubscribeMouse( *this );

    mGui->shutdown();
    delete mGui;
    mPlatform->shutdown();
    delete mPlatform;

    ClientGlobals::mGUI = 0;
}

void GUIManager::realign()
{
    mGui->_resizeWindow( MyGUI::IntSize( mWindowWidth, mWindowHeight ) );
}

void GUIManager::windowResized( unsigned int width, unsigned int height )
{
    mWindowWidth = width;
    mWindowHeight = height;
    GUIManager::realign();
}

bool GUIManager::mouseMoved( const MouseState& rState )
{
    return MyGUI::InputManager::getInstance().injectMouseMove( rState.x.abs, 
        rState.y.abs, rState.z.abs );
}

bool GUIManager::mousePressed( const MouseButton button )
{
    return MyGUI::InputManager::getInstance().injectMousePress( 
        GlobalsBase::mInput->getMouseState().x.abs, 
        GlobalsBase::mInput->getMouseState().y.abs, MyGUI::MouseButton::Enum( button ) );
}

void GUIManager::mouseReleased( const MouseButton button )
{
    MyGUI::InputManager::getInstance().injectMouseRelease(
        GlobalsBase::mInput->getMouseState().x.abs, 
        GlobalsBase::mInput->getMouseState().y.abs, MyGUI::MouseButton::Enum( button ) );
}

bool GUIManager::keyPressed( const KeyboardButton button, unsigned int text )
{
    if( button == KC_ESCAPE )
        mMenuGUI->toggleVisible();

    return MyGUI::InputManager::getInstance().injectKeyPress( MyGUI::KeyCode::Enum( button ), text );
}

void GUIManager::keyReleased( const KeyboardButton button, unsigned int text )
{
    MyGUI::InputManager::getInstance().injectKeyRelease( MyGUI::KeyCode::Enum( button ) );
}

//------------------------------------------------------------------------------
} // Namespace DefaultClient
} // Namespace Diversia