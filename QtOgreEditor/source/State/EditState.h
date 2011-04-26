/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_EDITSTATE_H
#define DIVERSIA_QTOGREEDITOR_EDITSTATE_H

#include "Platform/Prerequisites.h"

#include "Shared/ClientServerPlugin/ClientServerPluginManager.h"
#include "Util/State/State.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

class EditState : public State
{
public:
    EditState();
    ~EditState();
    
private:
    void start();
    void suspend();
    void wake();
    void activeServerDisconnected( ServerAbstract& rActiveServer );
    void pluginStateChange( PluginState state );

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_EDITSTATE_H