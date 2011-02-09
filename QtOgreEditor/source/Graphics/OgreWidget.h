/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_OGREWIDGET_H
#define DIVERSIA_QTOGREEDITOR_OGREWIDGET_H

#include "Platform/Prerequisites.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

class OgreWidget : public QWidget
{
    Q_OBJECT

public:
    OgreWidget( QWidget* pParentWidget = 0, Qt::WindowFlags flags = 0 );
    ~OgreWidget();

    void init( Ogre::Root* pRoot, Ogre::RenderWindow* pRenderWindow );
    String getWidgetHandle();
    inline QPaintEngine* paintEngine() const { return NULL; }

private:
    void paintEvent( QPaintEvent* pEvent );
    void resizeEvent( QResizeEvent* pEvent );
    void mouseMoveEvent( QMouseEvent* pEvent );
    void wheelEvent( QWheelEvent* pEvent );
    void mousePressEvent( QMouseEvent* pEvent );
    void mouseReleaseEvent( QMouseEvent* pEvent );
    void keyPressEvent( QKeyEvent* pEvent );
    void keyReleaseEvent( QKeyEvent* pEvent );

    boost::scoped_ptr<QtInputManager>   mInputManager;

    Ogre::Root*         mRoot;
    Ogre::RenderWindow* mRenderWindow;
    QTimer*             mTimer;
    int                 mKeyCounter;

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_OGREWIDGET_H