/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Client/Platform/StableHeaders.h"

#include "Client/Undo/FunctionCommand.h"

namespace Diversia
{
namespace Client 
{
//------------------------------------------------------------------------------

FunctionCommand::FunctionCommand():
    UndoCommand( "" )
{

}

FunctionCommand::~FunctionCommand()
{

}

bool FunctionCommand::mergeWith( const UndoCommand* pCommand )
{
    return false;
}

void FunctionCommand::redo()
{

}

void FunctionCommand::undo()
{

}

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia