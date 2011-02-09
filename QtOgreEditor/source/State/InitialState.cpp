/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "State/InitialState.h"
#include "UI/MainWindow.h"

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

InitialState::InitialState():
    State()
{

}

InitialState::~InitialState()
{

}

void InitialState::start()
{
    InitialState::wake();
}

void InitialState::suspend()
{
    EditorGlobals::mMainWindow->mUI.actionConnect->setEnabled( false );
    EditorGlobals::mMainWindow->mUI.actionDisconnect->setEnabled( true );
}

void InitialState::wake()
{
    EditorGlobals::mMainWindow->mUI.objectsDockWidgetContents->setEnabled( false );
    EditorGlobals::mMainWindow->mUI.pluginsDockWidgetContents->setEnabled( false );
    EditorGlobals::mMainWindow->mUI.propertyBrowser->setEnabled( false );
    EditorGlobals::mMainWindow->mUI.graphicsView->setEnabled( false );
    EditorGlobals::mMainWindow->mUI.propertyBrowser->setEnabled( false );
    EditorGlobals::mMainWindow->mUI.actionConnect->setEnabled( true );
    EditorGlobals::mMainWindow->mUI.actionDisconnect->setEnabled( false );
    EditorGlobals::mOffline = false;
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia