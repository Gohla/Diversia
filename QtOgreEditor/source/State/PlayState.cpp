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
#include "OgreClient/Input/ObjectSelection.h"
#include "State/PlayState.h"
#include "UI/MainWindow.h"

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

PlayState::PlayState():
    State()
{

}

PlayState::~PlayState()
{

}

void PlayState::start()
{
    // TODO: What about multiple 'servers'?
    GlobalsBase::mGrid->getActiveServer().getPluginManager().setState( PLAY );
    EditorGlobals::mMainWindow->mUI.actionPlay->setEnabled( false );
    EditorGlobals::mMainWindow->mUI.actionPause->setEnabled( true );
    EditorGlobals::mMainWindow->mUI.actionStop->setEnabled( true );
    EditorGlobals::mMainWindow->mUI.actionGame_save->setEnabled( false );
    EditorGlobals::mMainWindow->mUI.actionGame_save_as->setEnabled( false );
    EditorGlobals::mMainWindow->mUI.actionLevel_save->setEnabled( false );
    EditorGlobals::mMainWindow->mUI.actionLevel_save_as->setEnabled( false );
    EditorGlobals::mMainWindow->mUI.actionLevel_load->setEnabled( false );
    EditorGlobals::mMainWindow->mUI.actionLevel_new->setEnabled( false );

    // Deselect all objects
    GlobalsBase::mSelection->deselectAll();
}

void PlayState::suspend()
{

}

void PlayState::wake()
{
    // TODO: What about multiple 'servers'?
    GlobalsBase::mGrid->getActiveServer().getPluginManager().setState( PLAY );
    EditorGlobals::mMainWindow->mUI.actionStop->setEnabled( true );
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia