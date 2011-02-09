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

#ifndef DIVERSIA_SHARED_LUAMANAGER_H
#define DIVERSIA_SHARED_LUAMANAGER_H

#include "Shared/Platform/Prerequisites.h"

#include <camp-lua/lua.hpp>
#include <camp-lua/context.hpp>

namespace Diversia
{
//------------------------------------------------------------------------------

// Static logger for lua
static boost::log::sources::severity_channel_logger_mt< Diversia::Util::LogLevel > luaLogger
(
    boost::log::keywords::severity = Diversia::Util::LOG_INFO,
    boost::log::keywords::channel = "Lua"
);

class DIVERSIA_SHARED_API LuaError : public std::exception
{
public:
    // Construct using top-most element on Lua stack as error.
    LuaError( lua_State * L ): m_L( L ), m_lua_resource( L, LuaError_lua_resource_delete ) {}
    LuaError( const LuaError & other ): m_L( other.m_L ), m_lua_resource( other.m_lua_resource ) {}
    virtual const char * what() const throw() 
    {
        const char* s = lua_tostring( m_L, -1 );
        if ( s == NULL ) s = "unrecognized Lua error";

        return s;
    }

private:
    lua_State*                      m_L;
    boost::shared_ptr<lua_State>    m_lua_resource;

    LuaError & operator=(const LuaError & other);
    static void LuaError_lua_resource_delete( lua_State * L )
    {
        lua_pop( L, 1 );
    }

};

enum LuaSecurityLevel
{
    LUASEC_HIGH,
    LUASEC_MEDIUM,
    LUASEC_LOW
};

typedef std::set<String> LuaSecurityContainer;
typedef std::map<LuaSecurityLevel, LuaSecurityContainer> LuaSecurityGlobals;

class DIVERSIA_SHARED_API LuaManager : public sigc::trackable
{
public:
    /**
    Default constructor. 
    **/
    LuaManager();
    /**
    Destructor. 
    **/
    ~LuaManager();

    /**
    Gets the lua state. 
    **/
    inline lua_State* getLuaState() const { return mLuaState; }
    /**
    Sets the root resource location. 
    **/
    inline void setRootResourceLocation( const Path& rLocation ) { mRootResourceLocation = rLocation; }

    /**
    Force garbage collection in lua.
    **/
    void forceGarbageCollection();
    /**
    Register a class with camp-lua to allow construction of that class in lua.
    
    @param  rName   The name the class should get in lua.
    @param  rClass  The class to register.
    **/
    void registerClass( const String& rName, const camp::Class& rClass );
    /**
    Register a class with camp-lua to allow construction of that class in lua.
    
    @param  rName   The name the class should get in lua.
    @tparam T       The type of the class.
    **/
    template <class T> void registerClass( const String& rName ) 
    { 
        LuaManager::registerClass( rName, camp::classByType<T>() );
    }
    /**
    Sets the security level for given class.
    
    @param  rName           The name of the class.
    @param  securityLevel   The security level to set the class to.
    **/
    void setSecurityLevel( const String& rName, LuaSecurityLevel securityLevel );
    /**
    Registers default classes (Vector3, Colour, etc..) with camp-lua.
    **/
    void registerDefaultClasses();
    /**
    Register an enum with camp-lua. 
    
    @param  rName   The name of the table that should contain the enum values.
    @param  rEnum   The enum to register.
    **/
    void registerEnum( const String& rName, const camp::Enum& rEnum );
    /**
    Register an enum with camp-lua. 
    
    @param  rName   The name of the table that should contain the enum values.
    @tparam T       The type of the enum.
    **/
    template <class T> void registerEnum( const String& rName ) 
    {
        LuaManager::registerEnum( rName, camp::enumByType<T>() ); 
    }
    /**
    Registers default enums (Mode, NetworkingType, etc..) with camp-lua.
    **/
    void registerDefaultEnums();
    /**
    Executes a string of lua. 
    
    @param  rLua                The string of lua code to execute.
    @param  rEnvironment        The lua environment to execute the file in, defaults to empty.
    @param  rParentEnvironment  The parent environment for rEnvironment, defaults to empty.
    @param  securityLevel       The level of security to execute the code at, defaults to low.
    **/
    void execute( const String& rLua, const String& rEnvironment = "", 
        const String& rParentEnvironment = "", LuaSecurityLevel securityLevel = LUASEC_LOW );
    /**
    Executes a lua file.
    
    @param  rFile               The lua file to execute.
    @param  rEnvironment        The lua environment to execute the file in, defaults to empty.
    @param  rParentEnvironment  The parent environment for rEnvironment, defaults to empty.
    @param  securityLevel       The level of security to execute the code at, defaults to low.
    @param  addRootPath         True to add the root path to the file in rFile, defaults to true.
    **/
    void executeFile( const Path& rFile, const String& rEnvironment = "", 
        const String& rParentEnvironment = "", LuaSecurityLevel securityLevel = LUASEC_LOW, 
        bool addRootPath = true );
    /**
    Query if a function in lua exists.
    
    @param  rFunctionName       Name of the function. 
    @param  rEnvironment        The lua environment to execute the file in, defaults to empty.
    @param  rParentEnvironment  The parent environment for rEnvironment, defaults to empty.
    
    @return true if function, false if not. 
    **/
    bool functionExists( const String& rFunctionName, const String& rEnvironment = "", 
        const String& rParentEnvironment = "" );
    /**
    Sets a user object to a value in lua.
    
    @param  rObject             The value to set. 
    @param  rObjectName         The name the value should get inside the environment.
    @param  rEnvironment        The lua environment to set the value to, defaults to empty.
    @param  rParentEnvironment  The parent environment for rEnvironment, defaults to empty.
    **/
    void set( const camp::Value& rValue, const String& rValueName, 
        const String& rEnvironment = "", const String& rParentEnvironment = "" );
    /**
    Creates a lua environment. 
    
    @param  rEnvironment        The new environment name. 
    @param  rParentEnvironment  The parent environment for the new environment.
    @param  securityLevel       The level of security of the new environment.
    **/
    void createEnvironment( const String& rEnvironment, const String& rParentEnvironment,
        LuaSecurityLevel securityLevel );
    /**
    Destroys a lua environment. 
    
    @param  rEnvironment        The new environment name. 
    @param  rParentEnvironment  The parent environment for the environment.
    **/
    void destroyeEnvironment( const String& rEnvironment, const String& rParentEnvironment );
    /**
    Calls a lua function.
    
    @param  rFunctionName       Name of the function. 
    @param  rEnvironment        The lua environment to execute the file in, defaults to empty.
    @param  rParentEnvironment  The parent environment for rEnvironment, defaults to empty.
    **/
    void call( const String& rFunctionName, const String& rEnvironment, 
        const String& rParentEnvironment )
    {
        // Check if function exists.
        if( !LuaManager::functionExists( rFunctionName, rEnvironment, rParentEnvironment ) )
            return;

        // Push function onto stack.
        LuaManager::pushStack( rFunctionName, rEnvironment, rParentEnvironment );

        // Call function
        try
        {
            if( lua_pcall( mLuaState, 0, 0, 0 ) ) throw LuaError( mLuaState );
        }
        catch ( const LuaError& rError )
        {
            BOOST_LOG_SEV( Diversia::luaLogger, Diversia::Util::LOG_ERROR ) << rError.what();
        }

        // Pop function from the stack.
        LuaManager::popStack( rFunctionName, rEnvironment, rParentEnvironment );
    }

    // Generate call for 1-20 parameters.
#define callBody(z, n, unused) \
    camp::Value( t ## n ).visit( visitor );

#define callSkeleton(z, n, unused) \
    template <BOOST_PP_ENUM_PARAMS(n, class T)> \
    void call( const String& rFunctionName, const String& rEnvironment, \
        const String& rParentEnvironment, BOOST_PP_ENUM_BINARY_PARAMS(n, T, t) ) \
    { \
        if( !LuaManager::functionExists( rFunctionName, rEnvironment, rParentEnvironment ) ) \
            return; \
        LuaManager::pushStack( rFunctionName, rEnvironment, rParentEnvironment ); \
        camp::lua::ValueToLuaVisitor visitor( mLuaState ); \
        BOOST_PP_REPEAT(n, callBody, ~); \
        try \
        { \
            if( lua_pcall( mLuaState, n, 0, 0 ) ) throw LuaError( mLuaState ); \
        } \
        catch ( const LuaError& rError ) \
        { \
            BOOST_LOG_SEV( Diversia::luaLogger, Diversia::Util::LOG_ERROR ) << rError.what(); \
        } \
        LuaManager::popStack( rFunctionName, rEnvironment, rParentEnvironment ); \
    }

    BOOST_PP_REPEAT_FROM_TO(1, 20, callSkeleton, ~)

#undef callBody
#undef callSkeleton

    /**
     * \brief Access to the global variable \a rName.
     *
     * Return a proxy object allowing to read/write the global variable named \a rName. If the
     * variable does not exists, it will be created by the affectation:
     * \code
     * camp::lua::Context c;
     * c["a"] = 1; // Create the global variable "a" with the value 1
     * c["b"] = c["a"]; // Create the global variable "b" with the value of the global variable "a"
     * \endcode
     *
     * Note that the proxy object returned should not be copied or referenced.
     *
     * \param name The global variable name to be accessed.
     * \return A proxy object providing read/write access on the global variable.
    **/
    inline camp::lua::Object operator[]( const String& rName ) { return mLuaContext[ rName ]; }
    inline camp::lua::Object object( const String& rName ) { return mLuaContext[ rName ]; }

    /**
    Connects a slot to the lua manager created signal.

    @param [in,out] rSlot   The slot (signature: void func(LuaManager&)) to connect.

    @return Connection object to block or disconnect the connection.
    **/
    static inline sigc::connection connectCreated( const sigc::slot<void, LuaManager&>& rSlot )
    {
        return mCreatedSignal.connect( rSlot );
    }

private:
    void pushStack( const String& rName, const String& rEnvironment, 
        const String& rParentEnvironment );
    void popStack( const String& rName, const String& rEnvironment, 
        const String& rParentEnvironment );
    void stackdump();

    camp::lua::Context  mLuaContext;
    lua_State*          mLuaState;
    Path                mRootResourceLocation;

    LuaSecurityGlobals mAllowedGlobals;

    static sigc::signal<void, LuaManager&> mCreatedSignal;

};

//------------------------------------------------------------------------------
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::LuaManager, 
    &Diversia::Shared::Bindings::CampBindings::bindLuaManager );

CAMP_AUTO_TYPE( Diversia::LuaSecurityLevel, 
    &Diversia::Shared::Bindings::CampBindings::bindLuaSecurityLevel );

static int LuaPrint( lua_State* L )
{
    int nArgs = lua_gettop(L);
    int i;
    lua_getglobal(L, "tostring");
    Diversia::Util::String ret;

    //make sure you start at 1 *NOT* 0
    for(i = 1; i <= nArgs; i++)
    {
        const char *s;
        lua_pushvalue(L, -1);
        lua_pushvalue(L, i);
        lua_call(L, 1, 1);
        s = lua_tostring(L, -1);
        if(s == NULL)
            return luaL_error(L, LUA_QL("tostring") " must return a string to ", LUA_QL("print"));

        if(i > 1) ret.append("\t");

        ret.append(s);
        lua_pop(L, 1);
    };

    BOOST_LOG_SEV( Diversia::luaLogger, Diversia::Util::LOG_INFO ) << ret;

    return 0;
}

#endif // DIVERSIA_SHARED_LUAMANAGER_H