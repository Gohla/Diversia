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

#include "Client/Platform/StableHeaders.h"

#include "Client/Camp/CampBindings.h"
#include "Client/Plugin/ClientPlugin.h"
#include "Client/Plugin/ClientPluginManager.h"
#include "Client/Communication/GridManager.h"
#include "Client/Communication/ServerConnection.h"
#include "Client/Communication/ServerNeighborsPlugin.h"
#include "Client/Lua/LuaPlugin.h"
#include "Client/Object/ClientComponent.h"
#include "Client/Object/ClientComponentTemplate.h"
#include "Client/Object/ClientObject.h"
#include "Client/Object/ClientObjectManager.h"
#include "Client/Object/ClientObjectTemplate.h"
#include "Client/Object/ClientObjectTemplateManager.h"
#include "Client/Permission/PermissionManager.h"
#include "Util/Camp/ValueMapper.h"

namespace Diversia
{
namespace Client 
{
namespace Bindings
{
//------------------------------------------------------------------------------

void CampBindings::bindClientPluginManager()
{
    camp::Class::declare<ClientPluginManager>( "ClientPluginManager" )
        .base<PluginManager>();
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindClientObjectManager()
{
    camp::Class::declare<ClientObjectManager>( "ClientObjectManager" )
        .tag( "NoLevelSerialization" )
        .base<ObjectManager>()
        .base<ClientPlugin>();
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindClientObject()
{
    camp::Class::declare<ClientObject>( "ClientObject" )
        .tag( "NoSerialization", &Object::isRuntimeObject )
        .tag( "Selectable" )
        .base<Object>();
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindClientComponent()
{
    camp::Class::declare<ClientComponent>( "ClientComponent" )
        .base<Component>()
        .base<PropertySynchronization>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "ResourceList", &ClientComponent::mResourceList );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindClientPlugin()
{
    camp::Class::declare<ClientPlugin>( "ClientPlugin" )
        .base<Plugin>()
        .base<PropertySynchronization>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        // Functions
        .function( "ForceSerialize", &ClientPlugin::forceSerializeProperties );
        // Static functions
        // Operators
}

void CampBindings::bindPermissionManager()
{
    camp::Class::declare<PermissionManager>( "PermissionManager" )
        .tag( "NoLevelSerialization" )
        .tag( "QtIcon", ":/Icons/Icons/apps/klipper.png" )
        .base<ClientPlugin>();
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindGridManager()
{
    camp::Class::declare<GridManager>( "GridManager" )
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "HalfConnectRange", &GridManager::mHalfConnectRange )
            .tag( "Configurable" )
        .property( "ConnectRange", &GridManager::mConnectRange )
            .tag( "Configurable" )
        .property( "SwitchStayTime", &GridManager::mSwitchStayTimeS )
            .tag( "Configurable" );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindServerConnectionSettings()
{
    camp::Class::declare<ServerConnection::Settings>( "ServerConnectionSettings" )
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "ThreadSleepTimer", &ServerConnection::Settings::mThreadSleepTimer )
            .tag( "Configurable" )
        .property( "ConnectionAttemptCount", &ServerConnection::Settings::mConnectionAttemptCount )
            .tag( "Configurable" )
        .property( "TimeBetweenConnectionAttempts", &ServerConnection::Settings::mTimeBetweenConnectionAttemptsMS )
            .tag( "Configurable" )
        .property( "Timeout", &ServerConnection::Settings::mTimeoutMS )
            .tag( "Configurable" )
        .property( "ShutdownBlockDuraction", &ServerConnection::Settings::mShutdownBlockDuractionMS )
            .tag( "Configurable" );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindServerNeighborsPlugin()
{
    camp::Class::declare<ServerNeighborsPlugin>( "ServerNeighborsPlugin" )
        .base<ClientPlugin>()
        .tag( "QtIcon", ":/Icons/Icons/places/network-workgroup.png" )
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "ServerNeighbors", &ServerNeighborsPlugin::mServerNeighbors );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindLuaPlugin()
{
    camp::Class::declare<LuaPlugin>( "Lua" )
        .tag( "QtIcon", ":/Icons/Icons/mimetypes/stock_script.png" )
        .tag( "NoLevelSerialization" )
        .base<ClientPlugin>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        // Functions
        .function( "Run garbage collection", &LuaManager::forceGarbageCollection, &LuaPlugin::get )
            .tag( "NoBitStream" )
            .tag( "QtPopupMenu" )
            .tag( "QtIcon", ":/Icons/Icons/mimetypes/application-x-trash.png" );
        // Static functions
        // Operators
}

void CampBindings::bindClientComponentTemplate()
{
    camp::Class::declare<ClientComponentTemplate>( "ClientComponentTemplate" )
        .base<ComponentTemplate>()
        .base<PropertySynchronization>();
	    // Constructors
	    // Properties (read-only)
	    // Properties (read/write)
	    // Functions
	    // Static functions
	    // Operators
}

void CampBindings::bindClientObjectTemplateManager()
{
    camp::Class::declare<ClientObjectTemplateManager>( "ClientObjectTemplateManager" )
        .tag( "NoLevelSerialization" )
        .base<ObjectTemplateManager>()
        .base<ClientPlugin>();
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindClientObjectTemplate()
{
    camp::Class::declare<ClientObjectTemplate>( "ClientObjectTemplate" )
        .base<ObjectTemplate>()
        .base<PropertySynchronization>();
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        // Functions
        // Static functions
        // Operators
}

//------------------------------------------------------------------------------
} // Namespace Bindings
} // Namespace Client
} // Namespace Diversia