/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "Input/QtInputManager.h"

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

QtInputManager::QtInputManager( unsigned int windowWidth, unsigned int windowHeight ):
    InputManager( windowWidth, windowHeight )
{

}

QtInputManager::~QtInputManager()
{

}

MouseButton QtInputManager::toMouseButton( Qt::MouseButton button )
{
    switch( button )
    {
        case Qt::NoButton: return MB_None;
        case Qt::LeftButton: return MB_Left;
        case Qt::RightButton: return MB_Right;
        case Qt::MidButton: return MB_Middle;
        case Qt::XButton1: return MB_Button3;
        case Qt::XButton2: return MB_Button4;
        case Qt::MouseButtonMask: return MB_None;
    }

    return MB_None;
}

KeyboardButton QtInputManager::toKeyboardButton( Qt::Key key )
{
    switch( key )
    {
        case Qt::Key_Escape: return KC_ESCAPE;
        case Qt::Key_Tab: return KC_TAB;
        case Qt::Key_Return: return KC_RETURN;
        case Qt::Key_Backspace: return KC_BACK;
        case Qt::Key_Enter: return KC_RETURN;
        case Qt::Key_Insert: return KC_INSERT;
        case Qt::Key_Delete: return KC_DELETE;
        case Qt::Key_Pause: return KC_PAUSE;
        //case Qt::Key_Print: return ;
        case Qt::Key_SysReq: return KC_SYSRQ;
        case Qt::Key_Home: return KC_HOME;
        case Qt::Key_End: return KC_END;
        case Qt::Key_Left: return KC_LEFT;
        case Qt::Key_Up: return KC_UP;
        case Qt::Key_Right: return KC_RIGHT;
        case Qt::Key_Down: return KC_DOWN;
        case Qt::Key_PageUp: return KC_PGUP;
        case Qt::Key_PageDown: return KC_PGDOWN;
        case Qt::Key_Shift: return KC_LSHIFT;     // TODO: What about other shift?
        case Qt::Key_Control: return KC_LCONTROL; // TODO: What about other control?
        //case Qt::Key_Meta: return ;
        case Qt::Key_Alt: return KC_LMENU;
        case Qt::Key_CapsLock: return KC_CAPITAL;
        case Qt::Key_NumLock: return KC_NUMLOCK;
        case Qt::Key_ScrollLock: return KC_SCROLL;
        case Qt::Key_F1: return KC_F1;
        case Qt::Key_F2: return KC_F2;
        case Qt::Key_F3: return KC_F3;
        case Qt::Key_F4: return KC_F4;
        case Qt::Key_F5: return KC_F5;
        case Qt::Key_F6: return KC_F6;
        case Qt::Key_F7: return KC_F7;
        case Qt::Key_F8: return KC_F8;
        case Qt::Key_F9: return KC_F9;
        case Qt::Key_F10: return KC_F10;
        case Qt::Key_Space: return KC_SPACE;
        //case Qt::Key_Any: return ;
        //case Qt::Key_Exclam: return ;
        //case Qt::Key_QuoteDbl: return ;
        //case Qt::Key_NumberSign: return ;
        //case Qt::Key_Dollar: return ;
        //case Qt::Key_Percent: return ;
        //case Qt::Key_Ampersand: return ;
        case Qt::Key_Apostrophe: return KC_APOSTROPHE;
        //case Qt::Key_ParenLeft: return ;
        //case Qt::Key_ParenRight: return ;
        case Qt::Key_Asterisk: return KC_MULTIPLY;
        case Qt::Key_Plus: return KC_EQUALS;
        case Qt::Key_Comma: return KC_COMMA;
        case Qt::Key_Minus: return KC_MINUS;
        case Qt::Key_Period: return KC_PERIOD;
        case Qt::Key_Slash: return KC_SLASH;
        case Qt::Key_0: return KC_0;
        case Qt::Key_1: return KC_1;
        case Qt::Key_2: return KC_2;
        case Qt::Key_3: return KC_3;
        case Qt::Key_4: return KC_4;
        case Qt::Key_5: return KC_5;
        case Qt::Key_6: return KC_6;
        case Qt::Key_7: return KC_7;
        case Qt::Key_8: return KC_8;
        case Qt::Key_9: return KC_9;
        //case Qt::Key_Colon: return ;
        case Qt::Key_Semicolon: return KC_SEMICOLON;
        //case Qt::Key_Less: return ;
        case Qt::Key_Equal: return KC_EQUALS;
        //case Qt::Key_Greater: return ;
        //case Qt::Key_Question: return ;
        //case Qt::Key_At: return ;
        case Qt::Key_A: return KC_A;
        case Qt::Key_B: return KC_B;
        case Qt::Key_C: return KC_C;
        case Qt::Key_D: return KC_D;
        case Qt::Key_E: return KC_E;
        case Qt::Key_F: return KC_F;
        case Qt::Key_G: return KC_G;
        case Qt::Key_H: return KC_H;
        case Qt::Key_I: return KC_I;
        case Qt::Key_J: return KC_J;
        case Qt::Key_K: return KC_K;
        case Qt::Key_L: return KC_L;
        case Qt::Key_M: return KC_M;
        case Qt::Key_N: return KC_N;
        case Qt::Key_O: return KC_O;
        case Qt::Key_P: return KC_P;
        case Qt::Key_Q: return KC_Q;
        case Qt::Key_R: return KC_R;
        case Qt::Key_S: return KC_S;
        case Qt::Key_T: return KC_T;
        case Qt::Key_U: return KC_U;
        case Qt::Key_V: return KC_V;
        case Qt::Key_W: return KC_W;
        case Qt::Key_X: return KC_X;
        case Qt::Key_Y: return KC_Y;
        case Qt::Key_Z: return KC_Z;
    }

    return KC_UNASSIGNED;
}

void QtInputManager::resizeEvent( QResizeEvent* pEvent )
{
    InputManager::setWindowSize( pEvent->size().width(), pEvent->size().height() );
}

void QtInputManager::mouseMoveEvent( QMouseEvent* pEvent )
{
    if( InputManager::mouseMoved( pEvent->pos().x(), pEvent->pos().y() ) )
        pEvent->accept();
}

void QtInputManager::wheelEvent( QWheelEvent* pEvent )
{
    if( InputManager::mouseMoved( pEvent->delta() ) )
        pEvent->accept();
}

void QtInputManager::mousePressEvent( QMouseEvent* pEvent )
{
    if( InputManager::mousePressed( toMouseButton( pEvent->button() ) ) )
        pEvent->accept();
}

void QtInputManager::mouseReleaseEvent( QMouseEvent* pEvent )
{
    if( InputManager::mouseReleased( toMouseButton( pEvent->button() ) ) )
        pEvent->accept();
}

void QtInputManager::keyPressEvent( QKeyEvent* pEvent )
{
    if( pEvent->isAutoRepeat() ) return;

    // TODO: Convert key
    if( InputManager::keyPressed( toKeyboardButton( (Qt::Key)pEvent->key() ), 'a' ) )
        pEvent->accept();
}

void QtInputManager::keyReleaseEvent( QKeyEvent* pEvent )
{
    if( pEvent->isAutoRepeat() ) return;

    // TODO: Convert text
    if( InputManager::keyReleased( toKeyboardButton( (Qt::Key)pEvent->key() ), 'a' ) )
        pEvent->accept();
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia