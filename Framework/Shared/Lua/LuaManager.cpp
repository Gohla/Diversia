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

#include "Shared/Platform/StableHeaders.h"

#include "Shared/Graphics/Graphics.h"
#include "Shared/Lua/LuaManager.h"
#include "Shared/Physics/Physics.h"
#include "Util/Math/Node.h"
#include <camp-lua/conversion.hpp>
#include <camp-lua/valuetoluavisitor.hpp>

namespace Diversia
{
//------------------------------------------------------------------------------

sigc::signal<void, LuaManager&> LuaManager::mCreatedSignal = sigc::signal<void, LuaManager&>();

LuaManager::LuaManager():
    mLuaState( mLuaContext.getLuaState() )
{
    lua_register( mLuaState, "print", LuaPrint );

    // Set allowed globals for security levels.
    mAllowedGlobals[LUASEC_HIGH] = initializer<LuaSecurityContainer>
    (
        "Vector2", "Vector3", "Vector4", "Quaternion", "Matrix3", "Matrix4", "Colour", "Radian",
        "Degree", "Angle",

        "pairs", "ipairs", "type", "tostring"
    );
    mAllowedGlobals[LUASEC_MEDIUM] = initializer<LuaSecurityContainer>
    (
        "Vector2", "Vector3", "Vector4", "Quaternion", "Matrix3", "Matrix4", "Colour", "Radian",
        "Degree", "Angle",

        "Sky", "ObjectManager",

        "pairs", "ipairs", "print", "type", "tostring"
    );
    mAllowedGlobals[LUASEC_LOW] = initializer<LuaSecurityContainer>
    (
        "Vector2", "Vector3", "Vector4", "Quaternion", "Matrix3", "Matrix4", "Colour", "Radian",
        "Degree", "Angle",

        "Sky", "ObjectManager", "Application",

        "pairs", "ipairs", "print", "type", "tostring"
    );

    LuaManager::registerDefaultClasses();
    LuaManager::registerDefaultEnums();
    mCreatedSignal( *this );
}

LuaManager::~LuaManager()
{
    lua_close( mLuaState );
}

void LuaManager::forceGarbageCollection()
{
    lua_gc( mLuaState, LUA_GCCOLLECT, 0 );
}

void LuaManager::registerClass( const String& rName, const camp::Class& rClass )
{
    mLuaContext[ rName ] = rClass;
}

void LuaManager::setSecurityLevel( const String& rName, LuaSecurityLevel securityLevel )
{
    // Remove name from all sets.
    mAllowedGlobals[LUASEC_HIGH].erase( rName );
    mAllowedGlobals[LUASEC_MEDIUM].erase( rName );
    mAllowedGlobals[LUASEC_LOW].erase( rName );

    // Insert name to all allowed sets.
    for( unsigned int i = securityLevel; i <= LUASEC_LOW; ++i )
        mAllowedGlobals[ (LuaSecurityLevel)i ].insert( rName );
}

void LuaManager::registerDefaultClasses()
{
    LuaManager::registerClass<Vector2>( "Vector2" );
    LuaManager::registerClass<Vector3>( "Vector3" );
    LuaManager::registerClass<Vector4>( "Vector4" );
    LuaManager::registerClass<Quaternion>( "Quaternion" );
    LuaManager::registerClass<Matrix3>( "Matrix3" );
    LuaManager::registerClass<Matrix4>( "Matrix4" );
    LuaManager::registerClass<Colour>( "Colour" );
    LuaManager::registerClass<Radian>( "Radian" );
    LuaManager::registerClass<Degree>( "Degree" );
    LuaManager::registerClass<Angle>( "Angle" );
}

void LuaManager::registerEnum( const String& rName, const camp::Enum& rEnum )
{
    mLuaContext[ rName ] = rEnum;

    // Allow the enum table to be used as globals.
    mAllowedGlobals[LUASEC_HIGH].insert( rName );
    mAllowedGlobals[LUASEC_MEDIUM].insert( rName );
    mAllowedGlobals[LUASEC_LOW].insert( rName );
}

void LuaManager::registerDefaultEnums()
{
    LuaManager::registerEnum<Mode>( "Mode" );
    LuaManager::registerEnum<NetworkingType>( "NetworkingType" );
    LuaManager::registerEnum<ClientServerPluginTypeEnum>( "PluginType" );
    LuaManager::registerEnum<ComponentTypeEnum>( "ComponentType" );
    LuaManager::registerEnum<PhysicsShape>( "PhysicsShape" );
    LuaManager::registerEnum<PhysicsType>( "PhysicsType" );
    LuaManager::registerEnum<GraphicsShape>( "GraphicsShape" );
    LuaManager::registerEnum<LightType>( "LightType" );
    LuaManager::registerEnum<Node::TransformSpace>( "TransformSpace" );
}

void LuaManager::execute( const String& rLua, const String& rEnvironment /*= ""*/, 
    const String& rParentEnvironment /*= ""*/, LuaSecurityLevel securityLevel /*= LUASEC_LOW*/ )
{
    if( !rLua.empty() )
    {
        try
        {
            if( !rEnvironment.empty() )
            {
                // Load string
                luaL_loadstring( mLuaState, rLua.c_str() );

                // Create environment
                LuaManager::createEnvironment( rEnvironment, rParentEnvironment, securityLevel );

                // Get and set environment.
                if( rParentEnvironment.empty() )
                {
                    lua_getglobal( mLuaState, rEnvironment.c_str() );
                    lua_setfenv( mLuaState, -2 );
                }
                else
                {
                    lua_getglobal( mLuaState, rParentEnvironment.c_str() );
                    lua_getfield( mLuaState, -1, rEnvironment.c_str() );
                    lua_setfenv( mLuaState, -3 );
                    lua_pop( mLuaState, 1 );
                }

                // Load string into environment.
                if( lua_pcall( mLuaState, 0, LUA_MULTRET, 0 ) ) throw LuaError( mLuaState );
            }
            else
            {
                if( luaL_dostring( mLuaState, rLua.c_str() ) ) throw LuaError( mLuaState );
            }
        }
        catch ( const LuaError& rError )
        {
            // If crashes occur here, use the commented out code and send luaError to the
            // log instead of rError.what()
            //std::wstring wError = boost::lexical_cast< std::wstring >( rError.what() );
            //std::string luaError = UtfConverter::ToUtf8( wError );

            BOOST_LOG_SEV( Diversia::luaLogger, Diversia::Util::LOG_ERROR ) << rError.what();
        }
    }
}

void LuaManager::executeFile( const Path& rFile, const String& rEnvironment /*= ""*/, 
    const String& rParentEnvironment /*= ""*/, LuaSecurityLevel securityLevel /*= LUASEC_LOW*/, 
    bool addRootPath /*= true*/ )
{
    try
    {
        if( !rEnvironment.empty() )
        {
            // Load file
            Path filePath;
            if( addRootPath )   filePath = mRootResourceLocation / rFile;
            else                filePath = rFile;

            if( luaL_loadfile( mLuaState, filePath.string().c_str() ) ) throw LuaError( mLuaState );

            // Create environment
            LuaManager::createEnvironment( rEnvironment, rParentEnvironment, securityLevel );

            // Get and set environment.
            if( rParentEnvironment.empty() )
            {
                lua_getglobal( mLuaState, rEnvironment.c_str() );
                lua_setfenv( mLuaState, -2 );
            }
            else
            {
                lua_getglobal( mLuaState, rParentEnvironment.c_str() );
                lua_getfield( mLuaState, -1, rEnvironment.c_str() );
                lua_setfenv( mLuaState, -3 );
                lua_pop( mLuaState, 1 );
            }

            // Load file into environment.
            if( lua_pcall( mLuaState, 0, LUA_MULTRET, 0 ) ) throw LuaError( mLuaState );
        }
        else
        {
            if( luaL_dofile( mLuaState, rFile.string().c_str() ) ) throw LuaError( mLuaState );
        }
    }
    catch ( const LuaError& rError )
    {
        // If crashes occur here, use the commented out code and send luaError to the
        // log instead of rError.what()
        //std::wstring wError = boost::lexical_cast< std::wstring >( rError.what() );
        //std::string luaError = UtfConverter::ToUtf8( wError );

        BOOST_LOG_SEV( Diversia::luaLogger, Diversia::Util::LOG_ERROR ) << rError.what();
    }
}

bool LuaManager::functionExists( const String& rFunctionName, const String& rEnvironment /*= ""*/, 
    const String& rParentEnvironment /*= "" */ )
{
    // Load function onto stack.
    LuaManager::pushStack( rFunctionName, rEnvironment, rParentEnvironment );

    // Check type
    bool exists = lua_type( mLuaState, -1 ) == LUA_TFUNCTION;

    // Pop function from the stack.
    LuaManager::popStack( rFunctionName, rEnvironment, rParentEnvironment );

    return exists;
}

void LuaManager::set( const camp::Value& rValue, const String& rValueName, 
    const String& rEnvironment /*= ""*/, const String& rParentEnvironment /*= "" */ )
{
    if( !rEnvironment.empty() && rParentEnvironment.empty() )
    {
        lua_getglobal( mLuaState, rEnvironment.c_str() );
        camp::lua::valueToLua( mLuaState, rValue );
        lua_setfield( mLuaState, -2, rValueName.c_str() );
        lua_pop( mLuaState, 1 );
    }
    else if( !rEnvironment.empty() )
    {
        lua_getglobal( mLuaState, rParentEnvironment.c_str() );
        lua_getfield( mLuaState, -1, rEnvironment.c_str() );
        camp::lua::valueToLua( mLuaState, rValue );
        lua_setfield( mLuaState, -2, rValueName.c_str() );
        lua_pop( mLuaState, 2 );
    }
    else
    {
        camp::lua::valueToLua( mLuaState, rValue );
        lua_setglobal( mLuaState, rValueName.c_str() );
    }
}

void LuaManager::createEnvironment( const String& rEnvironment, const String& rParentEnvironment,
    LuaSecurityLevel securityLevel )
{
    if( rEnvironment.empty() )
        return;

    if( rParentEnvironment.empty() )
    {
        // Prevent duplicate environment creation.
        lua_getglobal( mLuaState, rEnvironment.c_str() );
        if( lua_type( mLuaState, -1 ) != LUA_TNIL )
        {
            lua_pop( mLuaState, 1 );
            return;
        }
        lua_pop( mLuaState, 1 );
    }
    else
    {
        // Check if parent environment exists.
        lua_getglobal( mLuaState, rParentEnvironment.c_str() );
        if( lua_type( mLuaState, -1 ) == LUA_TNIL )
        {
            LuaManager::createEnvironment( rParentEnvironment, "", securityLevel );
        }
        lua_pop( mLuaState, 1 );

        // Prevent duplicate environment creation.
        lua_getglobal( mLuaState, rParentEnvironment.c_str() );
        lua_getfield( mLuaState, -1, rEnvironment.c_str() );
        if( lua_type( mLuaState, -1 ) != LUA_TNIL )
        {
            lua_pop( mLuaState, 2 );
            return;
        }
        lua_pop( mLuaState, 2 );
    }

    // Create a new environment.
    lua_createtable( mLuaState, 0, 1 );

    // Add objects and functions to new environment.
    for( LuaSecurityContainer::const_iterator i = mAllowedGlobals[securityLevel].begin(); 
        i != mAllowedGlobals[securityLevel].end(); ++i )
    {
        lua_getglobal( mLuaState, i->c_str() );
        lua_setfield( mLuaState, -2, i->c_str() );
    }

    if( rParentEnvironment.empty() )
    {
        // Set environment to global variable.
        lua_setglobal( mLuaState, rEnvironment.c_str() );
    }
    else
    {
        // Set environment to variable in parent environment.
        lua_getglobal( mLuaState, rParentEnvironment.c_str() );
        lua_pushvalue( mLuaState, -2 );
        lua_setfield( mLuaState, -2, rEnvironment.c_str() );
        lua_pop( mLuaState, 2 );
    }
}

void LuaManager::destroyeEnvironment( const String& rEnvironment, const String& rParentEnvironment )
{

}

void LuaManager::pushStack( const String& rName, const String& rEnvironment, 
    const String& rParentEnvironment )
{
    if( !rEnvironment.empty() )
    {
        //LuaManager::createEnvironment( rEnvironment, rParentEnvironment );

        if( !rParentEnvironment.empty() )
        {
            lua_getglobal( mLuaState, rParentEnvironment.c_str() );
            lua_getfield( mLuaState, -1, rEnvironment.c_str() );
            lua_getfield( mLuaState, -1, rName.c_str() );
        }
        else
        {
            lua_getglobal( mLuaState, rEnvironment.c_str() );
            lua_getfield( mLuaState, -1, rName.c_str() );
        }
    }
    else
    {
        lua_getglobal( mLuaState, rName.c_str() );
    }
}

void LuaManager::popStack( const String& rName, const String& rEnvironment, 
    const String& rParentEnvironment )
{
    if( !rEnvironment.empty() )
    {
        if( !rParentEnvironment.empty() )
        {
            lua_pop( mLuaState, 2 );
        }
        else
        {
            lua_pop( mLuaState, 1 );
        }
    }
}

void LuaManager::stackdump()
{
    int top = lua_gettop( mLuaState );

    BOOST_LOG_SEV( Diversia::luaLogger, Diversia::Util::LOG_ERROR ) << "Lua stack size: " << 
        top;

    for( int i = 1; i <= top; ++i )
    {
        int t = lua_type( mLuaState, i );
        switch( t ) 
        {
            case LUA_TSTRING:
                BOOST_LOG_SEV( Diversia::luaLogger, Diversia::Util::LOG_ERROR ) << 
                    i << " string: " << lua_tostring( mLuaState, i );
                break;
            case LUA_TBOOLEAN:
                BOOST_LOG_SEV( Diversia::luaLogger, Diversia::Util::LOG_ERROR ) << 
                    i << " bool: " << lua_toboolean( mLuaState, i );
                break;
            case LUA_TNUMBER:
                BOOST_LOG_SEV( Diversia::luaLogger, Diversia::Util::LOG_ERROR ) << 
                    i << " number: " << lua_tonumber( mLuaState, i );
                break;
            default:
                BOOST_LOG_SEV( Diversia::luaLogger, Diversia::Util::LOG_ERROR ) << 
                    i << " other: " << lua_typename( mLuaState, t );
                break;
        }
    }
}

//------------------------------------------------------------------------------
} // Namespace Diversia