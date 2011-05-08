/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_PAUSESTATE_H
#define DIVERSIA_QTOGREEDITOR_PAUSESTATE_H

#include "Platform/Prerequisites.h"

#include "Util/State/State.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

class PauseState : public State
{
public:
    PauseState();
    ~PauseState();
    
private:
    void start();
    void suspend();
    void wake();

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_PAUSESTATE_H