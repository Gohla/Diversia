/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_INITIALSTATE_H
#define DIVERSIA_QTOGREEDITOR_INITIALSTATE_H

#include "Platform/Prerequisites.h"

#include "Util/State/State.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

class InitialState : public State
{
public:
    InitialState();
    ~InitialState();

private:
    void start();
    void suspend();
    void wake();
    void connect( String rServerAddress, unsigned short serverPort, String rUsername, 
        String rPassword, String rNickname );
    void offline();

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_INITIALSTATE_H