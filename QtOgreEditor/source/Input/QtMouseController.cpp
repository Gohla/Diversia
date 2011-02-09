/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "Input/QtMouseController.h"

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

QtMouseController::QtMouseController( QWidget* pWidget ):
    mWidget( pWidget )
{

}

void QtMouseController::grab()
{
    mWidget->setCursor( QCursor( Qt::ClosedHandCursor ) );
}

void QtMouseController::ungrab()
{
    mWidget->setCursor( QCursor( Qt::ArrowCursor ) );
}

void QtMouseController::hide()
{

}

void QtMouseController::show()
{

}

void QtMouseController::setVisible( bool visible )
{

}

void QtMouseController::setPosition( unsigned int left, unsigned int top )
{
    QCursor cursor = mWidget->cursor();
    cursor.setPos( QPoint( left, top ) );
    mWidget->setCursor( cursor );
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia