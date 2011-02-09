/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "State/LoadingState.h"
#include "State/EditState.h"
#include "Client/Communication/GridManager.h"

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

LoadingState::LoadingState():
    State()
{
    EditorGlobals::mGrid->connectLoadingCompleted( sigc::mem_fun( this, 
        &LoadingState::loadingComplete ) );
    EditorGlobals::mGrid->connectActiveServerDisconnect( sigc::mem_fun( this, 
        &LoadingState::activeServerDisconnected ) );
}

LoadingState::~LoadingState()
{
    EditorGlobals::mGrid->clear();
}

void LoadingState::start()
{
    mActive = true;
}

void LoadingState::suspend()
{
    mActive = false;
}

void LoadingState::wake()
{
    // This state cannot be waked up, immediately pop this state.
    mActive = true;
    EditorGlobals::mState->popState();
}

void LoadingState::loadingComplete()
{
    LOGI << "Loading complete!";
    EditorGlobals::mState->pushState( new EditState() );
}

void LoadingState::activeServerDisconnected( ServerAbstract& rActiveServer )
{
    if( mActive )
    {
        LOGE << "Active server disconnected or failed to connect, logging out.";
        EditorGlobals::mState->popState();
    }
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia