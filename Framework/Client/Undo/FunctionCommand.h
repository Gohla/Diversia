/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_CLIENT_FUNCTIONCOMMAND_H
#define DIVERSIA_CLIENT_FUNCTIONCOMMAND_H

#include "Client/Platform/Prerequisites.h"

#include "Client/Undo/UndoCommand.h"

namespace Diversia
{
namespace Client
{
//------------------------------------------------------------------------------

class DIVERSIA_CLIENT_API FunctionCommand : public UndoCommand
{
public:
    FunctionCommand();
    virtual ~FunctionCommand();

    inline int id() const { return 1; }
    bool mergeWith( const UndoCommand* pCommand );
    void redo();
    void undo();
    
private:

};

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia

#endif // DIVERSIA_CLIENT_FUNCTIONCOMMAND_H