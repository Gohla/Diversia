/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "EditorGlobals.h"

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

MainWindow*     EditorGlobals::mMainWindow = 0;
ConnectDialog*  EditorGlobals::mConnectDialog = 0;
OgreWidget*     EditorGlobals::mOgreWidget = 0;
QString         EditorGlobals::mCurrentGame = "";
QString         EditorGlobals::mCurrentLevel = "";

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia