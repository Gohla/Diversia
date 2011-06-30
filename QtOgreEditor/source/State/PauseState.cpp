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
#include "State/PauseState.h"
#include "UI/MainWindow.h"

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

PauseState::PauseState()
{

}

PauseState::~PauseState()
{

}

void PauseState::start()
{
    // TODO: What about multiple 'servers'?
    GlobalsBase::mGrid->getActiveServer().getPluginManager().setState( PAUSE );
    EditorGlobals::mMainWindow->mUI.actionStop->setEnabled( false );
}

void PauseState::suspend()
{

}

void PauseState::wake()
{

}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia