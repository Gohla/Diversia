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

#include "Object/LuaObjectScript.h"
#include <camp-lua/valuetoluavisitor.hpp>

namespace Diversia
{
namespace Server
{
//------------------------------------------------------------------------------

std::map<String, unsigned int> LuaObjectScript::mDefaultEnvironmentCounter =
    std::map<String, unsigned int>();

LuaObjectScript::LuaObjectScript( const String& rName, Mode mode, NetworkingType networkingType,
    RakNet::RakNetGUID source, bool localOverride, ServerObject& rObject ):
    ServerComponent( rName, mode, networkingType, LuaObjectScript::getTypeStatic(), source,
        localOverride, rObject ),
    mLoaded( false ),
    mClientSecurityLevel( LUASEC_HIGH ),
    mServerSecurityLevel( LUASEC_HIGH )
{
    PropertySynchronization::storeUserObject();
}

LuaObjectScript::~LuaObjectScript()
{
    if( mLoaded )
    {
        // Call Destroy function if the lua script has one.
        if( Globals::mLua->functionExists( "Destroy", mServerEnvironmentName, "Global" ) )
        {
            Globals::mLua->call( "Destroy", mServerEnvironmentName, "Global" );
        }
    }
}

void LuaObjectScript::create()
{
    if( !mServerScriptFile.empty() )
    {
        if( mServerEnvironmentName.empty() )
        {
            mServerEnvironmentName = "LuaObjectScript";
        }

        // Get a unique environment name.
        if( !mDefaultEnvironmentCounter.count( mServerEnvironmentName ) )
        {
            mDefaultEnvironmentCounter[ mServerEnvironmentName ] = 0;
        }
        else
        {
            mServerEnvironmentName = mServerEnvironmentName + boost::lexical_cast<String>(
                mDefaultEnvironmentCounter[ mServerEnvironmentName ]++ );
        }

        // Create environment before executing file so this component's object can be set in the
        // environment before executing.
        Globals::mLua->createEnvironment( mServerEnvironmentName, "Global", mServerSecurityLevel );
        Globals::mLua->set( ServerComponent::getServerObject(), "Object", mServerEnvironmentName,
            "Global" );

        // Load script
        Globals::mLua->executeFile( mServerScriptFile, mServerEnvironmentName, "Global",
            mServerSecurityLevel );
        mLoaded = true;

        // Call Create function if the lua script has one.
        if( Globals::mLua->functionExists( "Create", mServerEnvironmentName, "Global" ) )
        {
            Globals::mLua->call( "Create", mServerEnvironmentName, "Global" );
        }

        // Connect signals to lua functions.
        // Update
        if( Globals::mLua->functionExists( "Update", mServerEnvironmentName, "Global" ) )
        {
            Globals::mUpdateSignal->connect( sigc::bind( sigc::mem_fun( Globals::mLua,
                &LuaManager::call ),
                "Update", mServerEnvironmentName, "Global" ) );
        }

        // Frame
        if( Globals::mLua->functionExists( "Frame", mServerEnvironmentName, "Global" ) )
        {
            Globals::mFrameSignal->connect( sigc::mem_fun( this, &LuaObjectScript::frame ) );
        }
    }
}

void LuaObjectScript::frame( Real timeSinceLastFrame )
{
    Globals::mLua->call( "Frame", mServerEnvironmentName, "Global", timeSinceLastFrame );
}

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia