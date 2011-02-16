/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Client/Platform/StableHeaders.h"

#include "Client/Undo/UndoStack.h"
#include "Client/Undo/UndoCommand.h"

namespace Diversia
{
namespace Client 
{
//------------------------------------------------------------------------------

UndoStack::UndoStack():
    mCurrentCommand( mUndoCommands.end() ),
    mCurrentCommandReverse( mUndoCommands.rend() )
{

}

UndoStack::~UndoStack()
{
    UndoStack::clear();
}

void UndoStack::push( UndoCommand* pUndoCommand )
{
    if( !pUndoCommand ) return;

    int id = pUndoCommand->id();

    LCLOGD << "Pushing undo command with id " << id << " and name " << pUndoCommand->getName();

    // Invalidate all commands from front to current command.
    UndoCommands::iterator i = mUndoCommands.begin();
    while( i != mCurrentCommand )
    {
        LCLOGD << "Removing undo command " << (*i)->getName();
        undoCommandRemoved( *i );
        delete *i;
        mUndoCommands.erase( i++ );
    }

    mUndoCommands.push_front( pUndoCommand );
    mCurrentCommand = mUndoCommands.begin();
    mCurrentCommandReverse = mUndoCommands.rend();
    pUndoCommand->redo();
    undoCommandAdded( pUndoCommand );
    currentCommandChanged( pUndoCommand );
}

void UndoStack::undo()
{
    if( mCurrentCommand != mUndoCommands.end() )
    {
        LCLOGD << "Undoing command with id " << (*mCurrentCommand)->id() << " and name " << 
            (*mCurrentCommand)->getName();
        (*mCurrentCommand)->undo();
        
        ++mCurrentCommand;
        --mCurrentCommandReverse;

        if( mCurrentCommand == mUndoCommands.end() )
            currentCommandChanged( 0 );
        else
            currentCommandChanged( *mCurrentCommand );
    }
}

void UndoStack::redo()
{
    if( mCurrentCommandReverse != mUndoCommands.rend() )
    {

        LCLOGD << "Redoing command with id " << (*mCurrentCommandReverse)->id() << " and name " << 
            (*mCurrentCommandReverse)->getName();
        (*mCurrentCommandReverse)->redo();

        --mCurrentCommand;
        ++mCurrentCommandReverse;

        if( mCurrentCommandReverse == mUndoCommands.rend() )
            currentCommandChanged( 0 );
        else
            currentCommandChanged( *mCurrentCommandReverse );
    }
}

void UndoStack::clear()
{
    UndoCommands::iterator i = mUndoCommands.begin();
    while( i != mUndoCommands.end() )
    {
        LCLOGD << "Removing undo command " << (*i)->getName();
        undoCommandRemoved( *i );
        delete *i;
        mUndoCommands.erase( i++ );
    }

    mCurrentCommand = mUndoCommands.end();
    mCurrentCommandReverse = mUndoCommands.rend();
    currentCommandChanged( 0 );
}

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia