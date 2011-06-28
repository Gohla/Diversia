/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_DEFAULTCLIENT_FORWARD_H
#define DIVERSIA_DEFAULTCLIENT_FORWARD_H

#include "Util/Platform/Forward.h"
#include "Object/Platform/Forward.h"
#include "Shared/Platform/Forward.h"
#include "Client/Platform/Forward.h"
#include "OgreClient/Platform/Forward.h"

namespace Diversia
{
namespace DefaultClient
{
//------------------------------------------------------------------------------

class ClientApplication;

// Game mode
class DefaultGameMode;

// GUI
class GUIManager;
class LoginGUI;
class MenuGUI;

// Input
class OISInputManager;

// State
class LoadingState;
class MenuState;
class PlayState;

//------------------------------------------------------------------------------
} // Namespace DefaultClient
} // Namespace Diversia

#endif // DIVERSIA_DEFAULTCLIENT_FORWARD_H
