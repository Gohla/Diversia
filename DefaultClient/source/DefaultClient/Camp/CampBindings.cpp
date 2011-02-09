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

#include "DefaultClient/Platform/StableHeaders.h"

#include "DefaultClient/Camp/CampBindings.h"
#include "DefaultClient/ClientApplication.h"
#include "DefaultClient/GUI/LoginGUI.h"

namespace Diversia
{
namespace DefaultClient 
{
namespace Bindings
{
//------------------------------------------------------------------------------

void CampBindings::bindClientApplication()
{
    camp::Class::declare<ClientApplication>( "ClientApplication" )
    // Constructors
    // Properties (read-only)
    // Properties (read/write)
    .property( "LogLevel", &ClientApplication::mLogLevel )
    .tag( "Configurable" );
    // Functions
    // Static functions
    // Operators
}

void CampBindings::bindLoginGUISettings()
{
    camp::Class::declare<LoginGUI::Settings>( "LoginGUISettings" )
    // Constructors
    // Properties (read-only)
    // Properties (read/write)
    .property( "Servers", &LoginGUI::Settings::mServers )
    .tag( "Configurable" )
    .property( "LastServer", &LoginGUI::Settings::mLastServer )
    .tag( "Configurable" )
    .property( "LastUserName", &LoginGUI::Settings::mLastUserName )
    .tag( "Configurable" )
    .property( "LastNickname", &LoginGUI::Settings::mLastNickname )
    .tag( "Configurable" );
    // Functions
    // Static functions
    // Operators
}

//------------------------------------------------------------------------------
} // Namespace Bindings
} // Namespace DefaultClient
} // Namespace Diversia