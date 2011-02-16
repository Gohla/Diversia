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
    void undo();
    void redo();
    void clear();

protected:
    inline virtual void undoCommandAdded( UndoCommand* pUndoCommand ) {}
    inline virtual void undoCommandRemoved( UndoCommand* pUndoCommand ) {}
    inline virtual void currentCommandChanged( UndoCommand* pCurrentCommand ) {}
    
private:
    UndoCommands mUndoCommands;
    UndoCommands::iterator mCurrentCommand;
    UndoCommands::reverse_iterator mCurrentCommandReverse;

};

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia

#endif // DIVERSIA_CLIENT_UNDOSTACK_H