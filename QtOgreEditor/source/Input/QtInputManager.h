/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_QTINPUTMANAGER_H
#define DIVERSIA_QTOGREEDITOR_QTINPUTMANAGER_H

#include "Platform/Prerequisites.h"

#include "OgreClient/Input/InputManager.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

class QtInputManager : public InputManager
{
public:
    /**
    Constructor. 
    
    @param  windowWidth     Width of the render window. 
    @param  windowHeight    Height of the render window. 
    **/
    QtInputManager( unsigned int windowWidth, unsigned int windowHeight );
    /**
    Destructor. 
    **/
    ~QtInputManager();

    /**
    Converts a qt mouse button to a diversia mouse button. 
    **/
    MouseButton toMouseButton( Qt::MouseButton button );
    /**
    Converts a qt key to a diversia keyboard button. 
    **/
    KeyboardButton toKeyboardButton( Qt::Key key );
    
private:
    friend class OgreWidget;	///< Allow private access to event calls for OgreWidget. 

    void resizeEvent( QResizeEvent* pEvent );
    void mouseMoveEvent( QMouseEvent* pEvent );
    void wheelEvent( QWheelEvent* pEvent );
    void mousePressEvent( QMouseEvent* pEvent );
    void mouseReleaseEvent( QMouseEvent* pEvent );
    void keyPressEvent( QKeyEvent* pEvent );
    void keyReleaseEvent( QKeyEvent* pEvent );

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_QTINPUTMANAGER_H