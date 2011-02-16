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

class FunctionCommand : public UndoCommand
{
public:
    FunctionCommand();
    virtual ~FunctionCommand();

	
    
private:

};

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia

#endif // DIVERSIA_CLIENT_FUNCTIONCOMMAND_H