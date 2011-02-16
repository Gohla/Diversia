/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2011 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_CLIENT_UNDOCOMMAND_H
#define DIVERSIA_CLIENT_UNDOCOMMAND_H

#include "Client/Platform/Prerequisites.h"

namespace Diversia
{
namespace Client
{
//------------------------------------------------------------------------------

class UndoCommand
{
public:
    UndoCommand( const String& rName ): mName( rName ) {}
    virtual ~UndoCommand() {}

    virtual int id() const = 0;
    virtual bool mergeWith( const UndoCommand* pCommand ) = 0;
    virtual void redo() = 0;
    virtual void undo() = 0;
    virtual inline const String& getName() const { return mName; }
    
private:
    String mName;

};

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia

#endif // DIVERSIA_CLIENT_UNDOCOMMAND_H