/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "Graphics/OgreWidget.h"
#include "Input/QtInputManager.h"
#include "Input/QtMouseController.h"
#include "OgreClient/Graphics/GraphicsManager.h"
#include <QTime>

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

OgreWidget::OgreWidget( QWidget* pParentWidget /*= 0*/, Qt::WindowFlags flags /*= 0*/ ):
    QWidget( pParentWidget, flags | Qt::MSWindowsOwnDC ),
    mTargetRenderCount( 60 ),
    mLastTime( 0 ),
    mRenderDelta( 0 ),
    mKeyCounter( 0 )
{
    QWidget::setAcceptDrops( true );
    QWidget::setContextMenuPolicy( Qt::PreventContextMenu );
    QWidget::setFocusPolicy( Qt::WheelFocus );
    QWidget::setMouseTracking( true );
    QWidget::setAttribute( Qt::WA_NoBackground );
    QWidget::setAttribute( Qt::WA_PaintOnScreen );

    // Input manager
    mInputManager.reset( new QtInputManager( QWidget::width(), QWidget::height() ) );
    mInputManager->setMouseController( new QtMouseController( this ) );

    EditorGlobals::mOgreWidget = this;
}

OgreWidget::~OgreWidget()
{
    EditorGlobals::mOgreWidget = 0;
}

void OgreWidget::init( Ogre::Root* pRoot, Ogre::RenderWindow* pRenderWindow )
{
    mRoot = pRoot;
    mRenderWindow = pRenderWindow;
}

Diversia::Util::String OgreWidget::getWidgetHandle()
{
#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32
    return Ogre::StringConverter::toString( (size_t)( (HWND)winId() ) );
#elif DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_LINUX
    QWidget* q_parent = dynamic_cast <QWidget *> ( parent() );
    QX11Info xInfo = x11Info();

    return Ogre::StringConverter::toString( (unsigned long)xInfo.display() ) +
        ":" + Ogre::StringConverter::toString( (unsigned int)xInfo.screen() ) +
        ":" + Ogre::StringConverter::toString( (unsigned long)q_parent->winId() );
#else
    DivAssert( 0, "Cannot get widget handle, unsupported OS" );
#endif

    return "";
}

void OgreWidget::update()
{
    QTime time = QTime::currentTime();
    unsigned int curtime = time.hour() * 60 + time.minute();
    curtime = (curtime * 60) + time.second();
    curtime = (curtime * 1000) + time.msec();

    unsigned int timediff = curtime - mLastTime;
    mLastTime = curtime;

    mRenderDelta += timediff;

    unsigned int targetDelta;

    if (mTargetRenderCount > 0)
        targetDelta = 1000 / mTargetRenderCount;
    else
        targetDelta = 1000000;

    if (mRenderDelta >= targetDelta)
    {
        mRenderDelta = mRenderDelta % targetDelta;
        QWidget::update();
    }
}

void OgreWidget::paintEvent( QPaintEvent* pEvent )
{
    if( !mRoot || !mRenderWindow ) return;

    mRoot->renderOneFrame();

#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_LINUX
    mRenderWindow->update();
#endif

    /*
    mRoot->_fireFrameStarted();
    mRenderWindow->update();
    mRoot->_fireFrameRenderingQueued();
    mRoot->_fireFrameEnded();
    pEvent->accept();
    */
}

void OgreWidget::resizeEvent( QResizeEvent* pEvent )
{
    if( !mRenderWindow ) return;

//#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_LINUX
//    mRenderWindow->resize( QWidget::width(), QWidget::height() );
//#endif

    mRenderWindow->windowMovedOrResized();
    GlobalsBase::mGraphics->windowResized( mRenderWindow );

    mInputManager->setWindowSize( QWidget::width(), QWidget::height() );
}

void OgreWidget::mouseMoveEvent( QMouseEvent* pEvent )
{
    mInputManager->mouseMoveEvent( pEvent );
}

void OgreWidget::wheelEvent( QWheelEvent* pEvent )
{
    mInputManager->wheelEvent( pEvent );
}

void OgreWidget::mousePressEvent( QMouseEvent* pEvent )
{
    mInputManager->mousePressEvent( pEvent );
}

void OgreWidget::mouseReleaseEvent( QMouseEvent* pEvent )
{
    mInputManager->mouseReleaseEvent( pEvent );
}

void OgreWidget::keyPressEvent( QKeyEvent* pEvent )
{
    mInputManager->keyPressEvent( pEvent );
}

void OgreWidget::keyReleaseEvent( QKeyEvent* pEvent )
{
    mInputManager->keyReleaseEvent( pEvent );
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia