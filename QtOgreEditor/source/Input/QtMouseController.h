/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_QTMOUSECONTROLLER_H
#define DIVERSIA_QTOGREEDITOR_QTMOUSECONTROLLER_H

#include "Platform/Prerequisites.h"
#include "OgreClient/Input/MouseController.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

class QtMouseController : public MouseController
{
public:
    /**
    Constructor. 
    
    @param [in,out] pWidget The widget where the mouse must be controlled.
    **/
    QtMouseController( QWidget* pWidget );

	/**
    Grabs the mouse cursor so that it cannot leave the window.
    **/
    void grab();
    /**
    Ungrabs the mouse cursor.
    **/
    void ungrab();
    /**
    Hides the mouse cursor and stores the current position.
    **/
    void hide();
    /**
    Shows the mouse cursor and restores the position to the position stored in hide().
    **/
    void show();
    /**
    Sets the visibility of the mouse cursor.
    
    @param  visible True to show, false to hide. 
    **/
    void setVisible( bool visible );
    /**
    Sets the position of the mouse cursor. Position (0,0) is at the top left corner, 
    position (window width,window height) is at the bottom right corner.
    
    @param  left    The left position
    @param  top     The top position
    **/
    void setPosition( unsigned int left, unsigned int top );
    
private:
    QWidget* mWidget;

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_QTMOUSECONTROLLER_H