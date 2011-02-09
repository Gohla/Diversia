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

#ifndef DIVERSIA_OGRECLIENT_INPUTMANAGER_H
#define DIVERSIA_OGRECLIENT_INPUTMANAGER_H

#include "OgreClient/Platform/Prerequisites.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

/**
Values that represent mouse buttons.
**/
enum MouseButton
{
    MB_Left = 0,
    MB_Right,
    MB_Middle,
    MB_Button3,
    MB_Button4,
    MB_Button5,
    MB_Button6,
    MB_Button7,

    MB_None = -1
};

/**
Values that represent keyboard buttons.
**/
enum KeyboardButton
{
    KC_UNASSIGNED  = 0x00,
    KC_ESCAPE      = 0x01,
    KC_1           = 0x02,
    KC_2           = 0x03,
    KC_3           = 0x04,
    KC_4           = 0x05,
    KC_5           = 0x06,
    KC_6           = 0x07,
    KC_7           = 0x08,
    KC_8           = 0x09,
    KC_9           = 0x0A,
    KC_0           = 0x0B,
    KC_MINUS       = 0x0C,    ///< - on main keyboard
    KC_EQUALS      = 0x0D,
    KC_BACK        = 0x0E,    ///< backspace
    KC_TAB         = 0x0F,
    KC_Q           = 0x10,
    KC_W           = 0x11,
    KC_E           = 0x12,
    KC_R           = 0x13,
    KC_T           = 0x14,
    KC_Y           = 0x15,
    KC_U           = 0x16,
    KC_I           = 0x17,
    KC_O           = 0x18,
    KC_P           = 0x19,
    KC_LBRACKET    = 0x1A,
    KC_RBRACKET    = 0x1B,
    KC_RETURN      = 0x1C,    ///< Enter on main keyboard
    KC_LCONTROL    = 0x1D,
    KC_A           = 0x1E,
    KC_S           = 0x1F,
    KC_D           = 0x20,
    KC_F           = 0x21,
    KC_G           = 0x22,
    KC_H           = 0x23,
    KC_J           = 0x24,
    KC_K           = 0x25,
    KC_L           = 0x26,
    KC_SEMICOLON   = 0x27,
    KC_APOSTROPHE  = 0x28,
    KC_GRAVE       = 0x29,    ///< accent
    KC_LSHIFT      = 0x2A,
    KC_BACKSLASH   = 0x2B,
    KC_Z           = 0x2C,
    KC_X           = 0x2D,
    KC_C           = 0x2E,
    KC_V           = 0x2F,
    KC_B           = 0x30,
    KC_N           = 0x31,
    KC_M           = 0x32,
    KC_COMMA       = 0x33,
    KC_PERIOD      = 0x34,    ///< . on main keyboard
    KC_SLASH       = 0x35,    ///< / on main keyboard
    KC_RSHIFT      = 0x36,
    KC_MULTIPLY    = 0x37,    ///< * on numeric keypad
    KC_LMENU       = 0x38,    ///< left Alt
    KC_SPACE       = 0x39,
    KC_CAPITAL     = 0x3A,
    KC_F1          = 0x3B,
    KC_F2          = 0x3C,
    KC_F3          = 0x3D,
    KC_F4          = 0x3E,
    KC_F5          = 0x3F,
    KC_F6          = 0x40,
    KC_F7          = 0x41,
    KC_F8          = 0x42,
    KC_F9          = 0x43,
    KC_F10         = 0x44,
    KC_NUMLOCK     = 0x45,
    KC_SCROLL      = 0x46,    ///< Scroll Lock
    KC_NUMPAD7     = 0x47,
    KC_NUMPAD8     = 0x48,
    KC_NUMPAD9     = 0x49,
    KC_SUBTRACT    = 0x4A,    ///< - on numeric keypad
    KC_NUMPAD4     = 0x4B,
    KC_NUMPAD5     = 0x4C,
    KC_NUMPAD6     = 0x4D,
    KC_ADD         = 0x4E,    ///< + on numeric keypad
    KC_NUMPAD1     = 0x4F,
    KC_NUMPAD2     = 0x50,
    KC_NUMPAD3     = 0x51,
    KC_NUMPAD0     = 0x52,
    KC_DECIMAL     = 0x53,    ///< . on numeric keypad
    KC_OEM_102     = 0x56,    ///< < > | on UK/Germany keyboards
    KC_F11         = 0x57,
    KC_F12         = 0x58,
    KC_F13         = 0x64,    ///<                     (NEC PC98)
    KC_F14         = 0x65,    ///<                     (NEC PC98)
    KC_F15         = 0x66,    ///<                     (NEC PC98)
    KC_KANA        = 0x70,    ///< (Japanese keyboard)
    KC_ABNT_C1     = 0x73,    ///< / ? on Portugese (Brazilian) keyboards
    KC_CONVERT     = 0x79,    ///< (Japanese keyboard)
    KC_NOCONVERT   = 0x7B,    ///< (Japanese keyboard)
    KC_YEN         = 0x7D,    ///< (Japanese keyboard)
    KC_ABNT_C2     = 0x7E,    ///< Numpad . on Portugese (Brazilian) keyboards
    KC_NUMPADEQUALS= 0x8D,    ///< = on numeric keypad (NEC PC98)
    KC_PREVTRACK   = 0x90,    ///< Previous Track (KC_CIRCUMFLEX on Japanese keyboard)
    KC_AT          = 0x91,    ///<                     (NEC PC98)
    KC_COLON       = 0x92,    ///<                     (NEC PC98)
    KC_UNDERLINE   = 0x93,    ///<                     (NEC PC98)
    KC_KANJI       = 0x94,    ///< (Japanese keyboard)
    KC_STOP        = 0x95,    ///<                     (NEC PC98)
    KC_AX          = 0x96,    ///<                     (Japan AX)
    KC_UNLABELED   = 0x97,    ///<                        (J3100)
    KC_NEXTTRACK   = 0x99,    ///< Next Track
    KC_NUMPADENTER = 0x9C,    ///< Enter on numeric keypad
    KC_RCONTROL    = 0x9D,
    KC_MUTE        = 0xA0,    ///< Mute
    KC_CALCULATOR  = 0xA1,    ///< Calculator
    KC_PLAYPAUSE   = 0xA2,    ///< Play / Pause
    KC_MEDIASTOP   = 0xA4,    ///< Media Stop
    KC_VOLUMEDOWN  = 0xAE,    ///< Volume -
    KC_VOLUMEUP    = 0xB0,    ///< Volume +
    KC_WEBHOME     = 0xB2,    ///< Web home
    KC_NUMPADCOMMA = 0xB3,    ///< , on numeric keypad (NEC PC98)
    KC_DIVIDE      = 0xB5,    ///< / on numeric keypad
    KC_SYSRQ       = 0xB7,
    KC_RMENU       = 0xB8,    ///< right Alt
    KC_PAUSE       = 0xC5,    ///< Pause
    KC_HOME        = 0xC7,    ///< Home on arrow keypad
    KC_UP          = 0xC8,    ///< UpArrow on arrow keypad
    KC_PGUP        = 0xC9,    ///< PgUp on arrow keypad
    KC_LEFT        = 0xCB,    ///< LeftArrow on arrow keypad
    KC_RIGHT       = 0xCD,    ///< RightArrow on arrow keypad
    KC_END         = 0xCF,    ///< End on arrow keypad
    KC_DOWN        = 0xD0,    ///< DownArrow on arrow keypad
    KC_PGDOWN      = 0xD1,    ///< PgDn on arrow keypad
    KC_INSERT      = 0xD2,    ///< Insert on arrow keypad
    KC_DELETE      = 0xD3,    ///< Delete on arrow keypad
    KC_LWIN        = 0xDB,    ///< Left Windows key
    KC_RWIN        = 0xDC,    ///< Right Windows key
    KC_APPS        = 0xDD,    ///< AppMenu key
    KC_POWER       = 0xDE,    ///< System Power
    KC_SLEEP       = 0xDF,    ///< System Sleep
    KC_WAKE        = 0xE3,    ///< System Wake
    KC_WEBSEARCH   = 0xE5,    ///< Web Search
    KC_WEBFAVORITES= 0xE6,    ///< Web Favorites
    KC_WEBREFRESH  = 0xE7,    ///< Web Refresh
    KC_WEBSTOP     = 0xE8,    ///< Web Stop
    KC_WEBFORWARD  = 0xE9,    ///< Web Forward
    KC_WEBBACK     = 0xEA,    ///< Web Back
    KC_MYCOMPUTER  = 0xEB,    ///< My Computer
    KC_MAIL        = 0xEC,    ///< Mail
    KC_MEDIASELECT = 0xED     ///< Media Select
};

//------------------------------------------------------------------------------

/**
Represents an axis of the mouse.
**/
class Axis
{
public:
    /**
    Constructor. 
    
    @param  abs The absolute component, defaults to 0.
    @param  rel The relative component, defaults to 0.
    **/
    Axis( int abs = 0, int rel = 0 ): abs( abs ), rel( rel ) {}

    /**
    Clears this object to its blank/initial state. 
    **/
    void clear() { abs = 0; rel = 0; }

    int abs, rel;	///< Absolute and relative value components. 

};

/**
Represents the state of the mouse.
**/
class MouseState
{
public:
    /**
    Default constructor. 
    **/
    MouseState() { clear(); }

    /**
    Queries if a mouse button is pressed down. 
    
    @param  button  The mouse button. 
    **/
    inline bool isButtonDown( MouseButton button ) const { return buttons[ button ]; }
    /**
    Clears this object to its blank/initial state. 
    **/
    inline void clear() { x.clear(); y.clear(); z.clear(); buttons = false; }

    Axis x;	                ///< X axis component
    Axis y;	                ///< Y axis component
    Axis z;	                ///< Z axis component
    std::bitset<8> buttons; ///< Represents all buttons - bit position indicates button down.

};

/**
Represents the state of the keyboard.
**/
class KeyboardState
{
public:
    /**
    Default constructor. 
    **/
    KeyboardState() { clear(); }

    /**
    Queries if a mouse button is pressed down. 
    
    @param  button  The mouse button. 
    **/
    inline bool isButtonDown( KeyboardButton button ) const { return buttons[ button ]; }
    /**
    Clears this object to its blank/initial state. 
    **/
    inline void clear() { buttons = false; }

    std::bitset<256> buttons;   ///< Represents all buttons - bit position indicates button down.

};

//------------------------------------------------------------------------------

// Forward declarations
struct MouseListenerCompare;
struct KeyboardListenerCompare;

/**
Mouse listener for receiving mouse events.
**/
class MouseListener
{
protected:
    /**
    Destructor. 
    **/
    virtual ~MouseListener() {}

    /**
    Callback for mouse movement.
    
    @param  rState  The mouse state.
    
    @return True if no other mouse listeners should receive this callback this frame.
    **/
    inline virtual bool mouseMoved( const MouseState& rState ) { return false; }
    /**
    Callback for mouse button presses.
    
    @param  button  The pressed mouse button.
    
    @return True if no other mouse listeners should receive this callback this frame.
    **/
    inline virtual bool mousePressed( const MouseButton button ) { return false; }
    /**
    Callback for mouse button releases.
    
    @param  button  The released mouse button.
    **/
    inline virtual void mouseReleased( const MouseButton button ) {}
    /**
    Gets the listener priority used for set ordering. Lower number is higher priority.
    
    @return The priority. 
    **/
    virtual int getMousePriority() const = 0;

    friend class InputManager;
    friend struct MouseListenerCompare;
    friend void camp::detail::destroy<MouseListener>( const UserObject& object );	///< Allow private access for camp bindings.

};

/**
Keyboard listener for keyboard events.
**/
class KeyboardListener
{
protected:
    /**
    Destructor. 
    **/
    virtual ~KeyboardListener() {}
    /**
    Callback for key presses.
    
    @param  button  The pressed keyboard button.
    @param  text    The text value of the pressed button.
    
    @return True if no other keyboard listeners should receive this callback this frame.
    **/
    inline virtual bool keyPressed( const KeyboardButton button, unsigned int text ) { return false; }
    /**
    Callback for key releases.
    
    @param  button  The released keyboard button.
    @param  text    The text value of the pressed button.
    **/
    inline virtual void keyReleased( const KeyboardButton button, unsigned int text ) {}
    /**
    Gets the listener priority used for set ordering. Lower number is higher priority.
    
    @return The priority. 
    **/
    virtual int getKeyboardPriority() const = 0;

    friend class InputManager;
    friend struct KeyboardListenerCompare;
    friend void camp::detail::destroy<KeyboardListener>( const UserObject& object );	///< Allow private access for camp bindings.

};

/**
MouseListener comparison for set ordering.
**/
struct MouseListenerCompare
{
    bool operator()( const MouseListener* left, const MouseListener* right ) const
    {
        return left->getMousePriority() < right->getMousePriority();
    }
};

/**
KeyboardListener comparison for set ordering.
**/
struct KeyboardListenerCompare
{
    bool operator()( const KeyboardListener* left, const KeyboardListener* right ) const
    {
        return left->getKeyboardPriority() < right->getKeyboardPriority();
    }
};

//------------------------------------------------------------------------------

typedef std::set<MouseListener*, MouseListenerCompare> MouseListeners;
typedef std::set<KeyboardListener*, KeyboardListenerCompare> KeyboardListeners;

/**
Keeps the state of the mouse and keyboard, provides signals for changes in mouse and keyboard state.
Inherit from this class and call the mouseMoved, mousePressed, etc.. functions to provide input to
the client.
**/
class DIVERSIA_OGRECLIENT_API InputManager
{
public:
    /**
    Constructor. 
    
    @param  windowWidth     Width of the render window. 
    @param  windowHeight    Height of the render window. 
    **/
    InputManager( unsigned int windowWidth = 0, unsigned int windowHeight = 0 );
    /**
    Destructor. 
    **/
    virtual ~InputManager();

    /**
    Gets the mouse state. 
    **/
    inline const MouseState& getMouseState() const { return mMouseState; }
    /**
    Gets the keyboard state. 
    **/
    inline const KeyboardState& getKeyboardState() const { return mKeyboardState; }
    /**
    Gets the window width. 
    **/
    unsigned int getWindowWidth() const { return mWindowWidth; }
    /**
    Gets the window height. 
    **/
    unsigned int getWindowHeight() const { return mWindowHeight; }

    /**
    Claims the mouse. 
    
    @param  claim                   True to claim the mouse, false to unclaim.
    @param [in,out] rMouseListener  The mouse listener that wants to claim the mouse.
    **/
    void claimMouse( bool claim, MouseListener& rMouseListener );
    /**
    Unclaims the mouse. 
    **/
    void clearMouseClaim();

    /**
    Subscribe for mouse events.
    
    @param [in,out] rMouseListener  The mouse listener. 
    **/
    void subscribeMouse( MouseListener& rMouseListener );
    /**
    Unsubscribe for mouse events.
    
    @param [in,out] rMouseListener  The mouse listener. 
    **/
    void unsubscribeMouse( MouseListener& rMouseListener );
    /**
    Subscribe for keyboard events.
    
    @param [in,out] rKeyboardListener   The keyboard listener. 
    **/
    void subscribeKeyboard( KeyboardListener& rKeyboardListener );
    /**
    Unsubscribe for keyboard events.
    
    @param [in,out] rKeyboardListener   The keyboard listener. 
    **/
    void unsubscribeKeyboard( KeyboardListener& rKeyboardListener );

protected:
    /**
    Sets the window size. Clicks outside this window will not be registered.
    
    @param  width   The new window width. 
    @param  height  The new window height. 
    **/
    void setWindowSize( unsigned int width, unsigned int height );
    /**
    Sets the mouse controller. The input manager will take ownership of the mouse controller.
    **/
    void setMouseController( MouseController* pMouseController );
    /**
    Notification for mouse movement over 3 coordinates.
    
    @param  x   The absolute x coordinate. 
    @param  y   The absolute y coordinate. 
    @param  z   The absolute z (scroll) coordinate. 
    
    @return True if input was captured, false if not.
    **/
    bool mouseMoved( int x, int y, int z );
    /**
    Notification for mouse movement over 2 coordinates.
    
    @param  x   The absolute x coordinate. 
    @param  y   The absolute y coordinate. 
    
    @return True if input was captured, false if not.
    **/
    bool mouseMoved( int x, int y );
    /**
    Notification for mouse scrolling.
    
    @param  z   The relative z (scroll) coordinate. 
    
    @return True if input was captured, false if not.
    **/
    bool mouseMoved( int z );
    /**
    Notification for mouse movement over 3 axises.
    
    @param  x   The x axis. 
    @param  y   The y axis. 
    @param  z   The z (scroll) axis. 
    
    @return True if input was captured, false if not.
    **/
    bool mouseMoved( const Axis& x, const Axis& y, const Axis& z );
    /**
    Notification for mouse button presses. 
    
    @param  button  The mouse button that was pressed 
    
    @return True if input was captured, false if not.
    **/
    bool mousePressed( const MouseButton button );
    /**
    Notification for mouse button releases. 
    
    @param  button  The mouse button that was released
    
    @return True if input was captured, false if not.
    **/
    bool mouseReleased( const MouseButton button );
    /**
    Notification for keyboard button presses. 
    
    @param  button  The keyboard button that was pressed.
    @param  text    The textual representation of the pressed button.
    
    @return True if input was captured, false if not.
    **/
    bool keyPressed( const KeyboardButton button, unsigned int text );
    /**
    Notification for keyboard button releases. 
    
    @param  button  The keyboard button that was released.
    @param  text    The textual representation of the released button.
    
    @return True if input was captured, false if not.
    **/
    bool keyReleased( const KeyboardButton button, unsigned int text );

private:
    void calculateMouseRelative();
    bool fireMouseMoved();
    void luaManagerCreated( LuaManager& rLuaManager );

    MouseState                          mMouseState;
    MouseState                          mPrevMouseState;
    unsigned int                        mWindowWidth, mWindowHeight;
    KeyboardState                       mKeyboardState;
    boost::scoped_ptr<MouseController>  mMouseController;

    MouseListeners              mMouseListeners;
    KeyboardListeners           mKeyboardListeners;
    MouseListener*              mMouseClaimer;
    bool                        mMouseClaimed;

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE( Diversia::OgreClient::MouseButton, 
    &Diversia::OgreClient::Bindings::CampBindings::bindMouseButton );
CAMP_AUTO_TYPE( Diversia::OgreClient::KeyboardButton, 
    &Diversia::OgreClient::Bindings::CampBindings::bindKeyboardButton );
CAMP_AUTO_TYPE( Diversia::OgreClient::Axis, 
    &Diversia::OgreClient::Bindings::CampBindings::bindAxis );
CAMP_AUTO_TYPE( Diversia::OgreClient::MouseState, 
    &Diversia::OgreClient::Bindings::CampBindings::bindMouseState );
CAMP_AUTO_TYPE( Diversia::OgreClient::KeyboardState, 
    &Diversia::OgreClient::Bindings::CampBindings::bindKeyboardState );
CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::MouseListener, 
    &Diversia::OgreClient::Bindings::CampBindings::bindMouseListener );
CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::KeyboardListener, 
    &Diversia::OgreClient::Bindings::CampBindings::bindKeyboardListener );
CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::InputManager, 
    &Diversia::OgreClient::Bindings::CampBindings::bindInputManager );

#endif // DIVERSIA_OGRECLIENT_INPUTMANAGER_H