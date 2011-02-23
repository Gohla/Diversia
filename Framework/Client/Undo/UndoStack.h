/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_CLIENT_UNDOSTACK_H
#define DIVERSIA_CLIENT_UNDOSTACK_H

#include "Client/Platform/Prerequisites.h"

namespace Diversia
{
namespace Client
{
//------------------------------------------------------------------------------

typedef std::list<UndoCommand*> UndoCommands;

class UndoStack
{
public:
    UndoStack();
    ~UndoStack();

	void push( UndoCommand* pUndoCommand );
    void remove( UndoCommand* pUndoCommand );
    void undo();
    void undoTo( UndoCommand* pUndoCommand );
    void undoAll();
    void redo();
    void redoTo( UndoCommand* pUndoCommand );
    void redoAll();
    void clear();

    inline sigc::connection connectChange( const sigc::slot<void, UndoCommand*, bool>& rSlot ) 
    {
        return mChangeSignal.connect( rSlot );
    }
    inline sigc::connection connectCurrentChanged( const sigc::slot<void, UndoCommand*>& rSlot )
    {
        return mCurrentChangedSignal.connect( rSlot );
    }
    
private:
    UndoCommands mUndoCommands;
    UndoCommands::iterator mCurrentCommand;
    UndoCommands::reverse_iterator mCurrentCommandReverse;

    sigc::signal<void, UndoCommand*, bool> mChangeSignal;
    sigc::signal<void, UndoCommand*> mCurrentChangedSignal;

};

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia

#endif // DIVERSIA_CLIENT_UNDOSTACK_H