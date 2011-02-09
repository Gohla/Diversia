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

#include "Platform/StableHeaders.h"

#include "GameMode/GameModePlugin.h"
#include "Shared/Lua/LuaManager.h"

namespace Diversia
{
namespace Server 
{
//------------------------------------------------------------------------------

GameModePlugin::GameModePlugin( Mode mode, ClientPluginManager& rPluginManager, 
    RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
    RakNet::NetworkIDManager& rNetworkIDManager ):
    ClientPlugin( mode, rPluginManager, rRakPeer, rReplicaManager, rNetworkIDManager ),
    mClientLoadDefaultGamemode( true ),
    mClientSecurityLevel( LUASEC_HIGH ),
    mServerSecurityLevel( LUASEC_LOW )
{
    PropertySynchronization::storeUserObject();

    Globals::mLua->object( "GameMode" ) = this;
}

GameModePlugin::~GameModePlugin()
{
    if( mLoaded )
    {
        // Call Destroy function if the lua script has one.
        if( Globals::mLua->functionExists( "Destroy", "", "Global" ) )
        {
            Globals::mLua->call( "Destroy", "", "Global" );
        }
    }
}

void GameModePlugin::create()
{
    // Load scripts
    for( Paths::const_iterator i = mServerScriptFiles.begin(); i != mServerScriptFiles.end(); ++i )
    {
        Globals::mLua->executeFile( *i, "", "Global", mServerSecurityLevel );
        mLoaded = true;
    }

    // Call Create function if the lua script has one.
    if( Globals::mLua->functionExists( "Create", "", "Global" ) )
    {
        Globals::mLua->call( "Create", "", "Global" );
    }

    // Connect signals to lua functions.
    // Update
    if( Globals::mLua->functionExists( "Update", "", "Global" ) )
    {
        Globals::mUpdateSignal->connect( sigc::bind( sigc::mem_fun( Globals::mLua, 
            &LuaManager::call ), "Update", "", "Global" ) );
    }
}

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia