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

#ifndef DIVERSIA_CLIENT_CAMPBINDINGS_H
#define DIVERSIA_CLIENT_CAMPBINDINGS_H

#include "Client/Platform/Prerequisites.h"

namespace Diversia
{
namespace Client
{
namespace Bindings
{
//------------------------------------------------------------------------------

class DIVERSIA_CLIENT_API CampBindings
{
public:
    static void bindServerPluginManager();
    static void bindClientObjectManager();
    static void bindClientObject();
    static void bindClientComponent();
    static void bindServerPlugin();
    static void bindPermissionManager();
    static void bindGridManager();
    static void bindServerConnectionSettings();
    static void bindServerNeighborsPlugin();
    static void bindLuaPlugin();
    static void bindClientComponentTemplate();
    static void bindClientObjectTemplateManager();
    static void bindClientObjectTemplate();

};

//------------------------------------------------------------------------------
} // Namespace Bindings
} // Namespace Client
} // Namespace Diversia

#endif // DIVERSIA_CLIENT_CAMPBINDINGS_H