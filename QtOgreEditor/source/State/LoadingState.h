/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_LOADINGSTATE_H
#define DIVERSIA_QTOGREEDITOR_LOADINGSTATE_H

#include "Platform/Prerequisites.h"

#include "Util/State/State.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

class LoadingState : public State, public sigc::trackable
{
public:
    LoadingState();
    ~LoadingState();
    
private:
    void start();
    void suspend();
    void wake();
    void loadingComplete();
    void activeServerDisconnected( ServerAbstract& rActiveServer );

    bool mActive;

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_LOADINGSTATE_H