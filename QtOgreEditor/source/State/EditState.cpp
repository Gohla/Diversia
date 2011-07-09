/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "Client/Plugin/ClientPluginManager.h"
#include "Client/Communication/GridManager.h"
#include "Client/Communication/ServerAbstract.h"
#include "OgreClient/ApplicationBase.h"
#include "State/EditState.h"
#include "UI/ConnectDialog.h"
#include "UI/MainWindow.h"

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
    EditorGlobals::mGrid->getActiveServer().getPluginManager().connectPluginStateChange( 
        sigc::mem_fun( this, &EditState::pluginStateChange ) );
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
    EditorGlobals::mMainWindow->mUI.actionPlay->setEnabled( true );
    EditorGlobals::mMainWindow->mUI.actionGame_save->setEnabled( true );
    EditorGlobals::mMainWindow->mUI.actionGame_save_as->setEnabled( true );
    EditorGlobals::mMainWindow->mUI.actionGame_load->setEnabled( false );
    EditorGlobals::mMainWindow->mUI.actionGame_new->setEnabled( false );
    EditorGlobals::mMainWindow->mUI.actionLevel_save->setEnabled( true );
    EditorGlobals::mMainWindow->mUI.actionLevel_save_as->setEnabled( true );
    EditorGlobals::mMainWindow->mUI.actionLevel_load->setEnabled( true );
    EditorGlobals::mMainWindow->mUI.actionLevel_new->setEnabled( true );
    EditorGlobals::mMainWindow->mUI.actionUndo->setEnabled( true );
    EditorGlobals::mMainWindow->mUI.actionRedo->setEnabled( true );
}

void EditState::suspend()
{

}

void EditState::wake()
{
    // TODO: What about multiple 'servers'?
    GlobalsBase::mGrid->getActiveServer().getPluginManager().setState( STOP );
    EditorGlobals::mMainWindow->mUI.actionStop->setEnabled( false );
    EditorGlobals::mMainWindow->mUI.actionPause->setEnabled( false );
    EditorGlobals::mMainWindow->mUI.actionPlay->setEnabled( true );
    EditorGlobals::mMainWindow->mUI.actionGame_save->setEnabled( true );
    EditorGlobals::mMainWindow->mUI.actionGame_save_as->setEnabled( true );
    EditorGlobals::mMainWindow->mUI.actionLevel_save->setEnabled( true );
    EditorGlobals::mMainWindow->mUI.actionLevel_save_as->setEnabled( true );
    EditorGlobals::mMainWindow->mUI.actionLevel_load->setEnabled( true );
    EditorGlobals::mMainWindow->mUI.actionLevel_new->setEnabled( true );
}

void EditState::activeServerDisconnected( ServerAbstract& rActiveServer )
{
    LOGE << "Active server disconnected or failed to connect, logging out.";
    EditorGlobals::mState->popState();
}

void EditState::pluginStateChange( PluginState state, PluginState prevState )
{
    switch( state )
    {
        case STOP: case PAUSE: GlobalsBase::mApp->stopUpdates( true ); break;
        case PLAY: GlobalsBase::mApp->stopUpdates( false ); break;
    }
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia