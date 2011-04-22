/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "State/EditState.h"
#include "UI/MainWindow.h"
#include "UI/ConnectDialog.h"
#include "Client/Communication/GridManager.h"

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

EditState::EditState():
    State()
{
    EditorGlobals::mGrid->connectActiveServerDisconnect( sigc::mem_fun( this, 
        &EditState::activeServerDisconnected ) );
}

EditState::~EditState()
{
    EditorGlobals::mGrid->clear();
}

void EditState::start()
{
    LOGI << "Entering edit state.";

    EditorGlobals::mMainWindow->mUI.objectsDockWidgetContents->setEnabled( true );
    EditorGlobals::mMainWindow->mUI.objectTemplatesDockWidgetContents->setEnabled( true );
    EditorGlobals::mMainWindow->mUI.pluginsDockWidgetContents->setEnabled( true );
    EditorGlobals::mMainWindow->mUI.propertyBrowserDockWidgetContents->setEnabled( true );
    EditorGlobals::mMainWindow->mUI.graphicsView->setEnabled( true );
    EditorGlobals::mMainWindow->mUI.listViewUndo->setEnabled( true );
}

void EditState::suspend()
{

}

void EditState::wake()
{

}

void EditState::activeServerDisconnected( ServerAbstract& rActiveServer )
{
    LOGE << "Active server disconnected or failed to connect, logging out.";
    EditorGlobals::mState->popState();
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia