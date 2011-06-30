/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.

Diversia is free software: you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation, either version 3 of the License, or (at your option) any later 
version.

Diversia is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with 
Diversia. If not, see <http://www.gnu.org/licenses/>.

You may contact the author of Diversia by e-mail at: equabyte@sonologic.nl
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_CLIENT_FORWARD_H
#define DIVERSIA_CLIENT_FORWARD_H

#include "Util/Platform/Forward.h"
#include "Object/Platform/Forward.h"
#include "Shared/Platform/Forward.h"

namespace Diversia
{
namespace Client
{
//------------------------------------------------------------------------------

// Plugin
class ClientPlugin;
class ClientPluginManager;

// Communication
class GridManager;
class ServerAbstract;
class Server;
class OfflineServer;
class ServerConnection;
class ServerNeighborsPlugin;

// Lua
class LuaPlugin;

// Object
class ClientComponent;
class ClientComponentTemplate;
class ClientObject;
class ClientObjectTemplate;
class ClientObjectManager;
class ClientObjectTemplateManager;

// Permission
class PermissionManager;

// Undo
class ComponentCommand;
class ObjectCommand;
class FunctionCommand;
class PropertyChangeCommand;
class UndoCommand;
class UndoStack;

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia

#endif // DIVERSIA_CLIENT_FORWARD_H
