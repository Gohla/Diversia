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
        mChangeSignal( *i, false );
        delete *i;
        mUndoCommands.erase( i++ );
    }

    mUndoCommands.push_front( pUndoCommand );
    mCurrentCommand = mUndoCommands.begin();
    mCurrentCommandReverse = mUndoCommands.rend();
    pUndoCommand->mIterator = mCurrentCommand;
    pUndoCommand->mReverseIterator = mCurrentCommandReverse;
    pUndoCommand->mUndoStack = this;
    pUndoCommand->redo();
    mChangeSignal( pUndoCommand, true );
    mCurrentChangedSignal( pUndoCommand );
}

void UndoStack::remove( UndoCommand* pUndoCommand )
{
    if( !pUndoCommand ) return;

    if( pUndoCommand->mIterator == mCurrentCommand )
    {
        if( mCurrentCommandReverse != mUndoCommands.rbegin() )
        {
            ++mCurrentCommand;
            --mCurrentCommandReverse;
        }
        else
        {
            --mCurrentCommand;
            ++mCurrentCommandReverse;
        }
        mCurrentChangedSignal( *mCurrentCommand );
    }

    mChangeSignal( pUndoCommand, false );
    delete pUndoCommand;
    mUndoCommands.erase( pUndoCommand->mIterator );
    
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
            mCurrentChangedSignal( 0 );
        else
            mCurrentChangedSignal( *mCurrentCommand );
    }
}

void UndoStack::undoTo( UndoCommand* pUndoCommand )
{
    while( mCurrentCommand != pUndoCommand->mIterator && mCurrentCommand != mUndoCommands.end() )
    {
        UndoStack::undo();
    }
}

void UndoStack::undoAll()
{
    while( mCurrentCommand != mUndoCommands.end() )
    {
        UndoStack::undo();
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
            mCurrentChangedSignal( 0 );
        else
            mCurrentChangedSignal( *mCurrentCommandReverse );
    }
}

void UndoStack::redoTo( UndoCommand* pUndoCommand )
{
    while( mCurrentCommandReverse != pUndoCommand->mReverseIterator && 
        mCurrentCommandReverse != mUndoCommands.rend() )
    {
        UndoStack::redo();
    }
}

void UndoStack::redoAll()
{
    while( mCurrentCommandReverse != mUndoCommands.rend() )
    {
        UndoStack::redo();
    }
}

void UndoStack::clear()
{
    mCurrentCommand = mUndoCommands.end();
    mCurrentCommandReverse = mUndoCommands.rend();
    mCurrentChangedSignal( 0 );

    UndoCommands::iterator i = mUndoCommands.begin();
    while( i != mUndoCommands.end() )
    {
        LCLOGD << "Removing undo command " << (*i)->getName();
        mChangeSignal( *i, false );
        delete *i;
        mUndoCommands.erase( i++ );
    }
}

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia