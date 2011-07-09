/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_EDITORGLOBALS_H
#define DIVERSIA_QTOGREEDITOR_EDITORGLOBALS_H

#include "Platform/Prerequisites.h"

#include "OgreClient/GlobalsBase.h"
#include <QString>

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

class EditorGlobals : public GlobalsBase
{
public:
    static MainWindow*      mMainWindow;
    static ConnectDialog*   mConnectDialog;
    static OgreWidget*      mOgreWidget;
    static QString          mCurrentGame;
    static QString          mCurrentLevel;

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_EDITORGLOBALS_H