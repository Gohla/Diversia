/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "DSLTypeExtractor/Platform/StableHeaders.h"

#include "DSLTypeExtractor/camptools.hpp"
#include "DSLTypeExtractor/filetools.hpp"
#include "DSLTypeExtractor/register.hpp"
#include "Object/ComponentFactory.h"
#include "Object/ComponentFactoryManager.h"
#include "Object/Object.h"
#include "Shared/ClientServerPlugin/ClientServerPluginFactoryManager.h"
#include "Shared/ClientServerPlugin/ClientServerPluginManager.h"
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/assign.hpp>

using namespace Diversia::Util;
using namespace Diversia::ObjectSystem;
using namespace Diversia;
using namespace Diversia::Client;
using namespace Diversia::OgreClient;
using namespace Diversia::DSLTypeExtractor;
using namespace std;
using namespace boost;
using namespace boost::assign;

TypeVisitor typeVisitor;

static String S(const String& s) { return "\"" + s + "\""; };

int main( int argc, char* argv[] )
{
    // Initialize logging
    Logger* logger = new Logger( LOG_DEBUG, true, false );

    registerCamp();

    // Create output files
    scoped_ptr<SDFFile> sdfType( new SDFFile( "builtin-type.sdf", "builtin-type" ) );
    sdfType->contextfree();
    sdfType->writeLn();
    scoped_ptr<STRFile> strType( new STRFile( "builtin-type.str", "data/builtin-type", list_of("include/DiversiaScript") ) ); 
    strType->rules();

    scoped_ptr<STRFile> strTypeBases( new STRFile( "builtin-type-base.str", "data/builtin-type-base", list_of("include/DiversiaScript") ) );
    strTypeBases->rules();

    scoped_ptr<STRFile> strTypeCons( new STRFile( "builtin-type-cons.str", "data/builtin-type-cons", list_of("include/DiversiaScript") ) );
    strTypeCons->rules();

    scoped_ptr<SDFFile> sdfTypeProp( new SDFFile( "builtin-type-prop.sdf", "builtin-type-prop") );
    sdfTypeProp->contextfree();
    scoped_ptr<STRFile> strTypeProp( new STRFile( "builtin-type-prop.str", "data/builtin-type-prop", list_of("include/DiversiaScript") ) ); 
    strTypeProp->signatures();
    strTypeProp->signature( "Property", list_of("BuiltinProperty")("Type")("Writable"), "Property" );
    strTypeProp->removeIdent();
    strTypeProp->rules();

    scoped_ptr<SDFFile> sdfTypeFunc( new SDFFile( "builtin-type-func.sdf", "builtin-type-func") );
    sdfTypeFunc->contextfree();
    scoped_ptr<STRFile> strTypeFunc( new STRFile( "builtin-type-func.str", "data/builtin-type-func", list_of("include/DiversiaScript") ) ); 
    strTypeFunc->signatures();
    strTypeFunc->signature( "Function", list_of("FunctionName")("ReturnType")("List(ParamType)"), "Function" );
    strTypeFunc->removeIdent();
    strTypeFunc->rules();

    scoped_ptr<STRFile> strTypeOp( new STRFile( "builtin-type-op.str", "data/builtin-type-op", list_of("include/DiversiaScript")("signatures") ) ); 
    strTypeOp->signatures();
    strTypeOp->signature( "Operator", list_of("Op")("ReturnType")("ParamType"), "Operator" );
    strTypeOp->removeIdent();
    strTypeOp->rules();

    size_t classCount = camp::classCount();
    for( size_t i = 0; i < classCount; ++i )
    {
        const camp::Class& metaclass = camp::classByIndex( i );
        String className = metaclass.name();
        String classCons = className + "()";
        LOGI << className;
        sdfType->contextfree( S(className), "BuiltinType", className );
        strType->rule( "is-builtin-type", "?" + classCons );

        // Bases
        {
            String bases = "![";
            size_t baseCount = metaclass.baseCount();
            bool first = true;
            for( size_t j = 0; j < baseCount; ++j )
            {
                String baseCons = metaclass.base( j ).name() + "()";

                strTypeBases->rule( "is-builtin-type-base", list_of(classCons), "?" + baseCons );
                if( !first ) bases += ", ";
                bases += baseCons;
                first = false;
            }
            strTypeBases->matchRule( "builtin-type-bases", classCons, bases + "]" );
            strTypeBases->writeLn();
        }

        // Constructors
        {
            size_t consCount = metaclass.constructorCount();
            for( size_t j = 0; j < consCount; ++j )
            {
                const camp::Constructor& cons = metaclass.constructor( j );

                String types = "[";
                size_t argCount = cons.argCount();
                bool first = true;
                for( size_t k = 0; k < argCount; ++k )
                {
                    if( !first ) types += ", ";
                    types += cons.argTypeInfo(k).apply_visitor( typeVisitor );
                    first = false;
                }
                strTypeCons->rule( "has-builtin-type-cons", list_of(classCons), "?" + types + "]" );
            }
            if( consCount ) strTypeCons->writeLn();
        }

        // Properties
        {
            String props = "![";
            size_t propCount = metaclass.propertyCount( true );
            bool first = true;
            for( size_t j = 0; j < propCount; ++j )
            {
                const camp::Property& prop = metaclass.property( j, true );
                String propName = algorithm::replace_all_copy( prop.name(), " ", "_" ); // TODO: Get rid of spaces in property names.
                String propNameS = S(propName);
                String propCons = propName + "()";
                String propType = prop.typeInfo().apply_visitor( typeVisitor );
                bool propWritable = prop.writable();

                if( !first ) props += ", ";
                props += "Property(" + propCons + ", " + propType + ", " + (propWritable ? "True()" : "False()") + ")";
                first = false; 

                sdfTypeProp->contextfree( propNameS, "BuiltinProperty", propName );
                strTypeProp->rule( "has-builtin-type-prop", list_of(classCons), "?" + propCons );
                if( propWritable ) strTypeProp->rule( "is-builtin-type-prop-writable", list_of(classCons), "?" + propCons );
                strTypeProp->matchRule( "builtin-type-prop-type", list_of(classCons), propCons, propType );
            }
            if( propCount ) sdfTypeProp->writeLn();
            strTypeProp->matchRule( "builtin-type-props", classCons, props + "]");
            strTypeProp->writeLn();
        }

        // Functions
        {
            String funcs = "![";
            size_t funcCount = metaclass.functionCount( true );
            bool first = true;
            for( size_t j = 0; j < funcCount; ++j )
            {
                const camp::Function& func = metaclass.function( j, true );
                String funcName = algorithm::replace_all_copy( func.name(), " ", "_" ); // TODO: Get rid of spaces in function names.
                String funcNameS = S(funcName);
                String funcCons = funcName + "()";
                String funcReturn = func.returnTypeInfo().apply_visitor( typeVisitor );
                String funcParams = "[";

                bool paramFirst = true;
                size_t argCount = func.argCount();
                for( size_t k = 0; k < argCount; ++k )
                {
                    if( !paramFirst ) funcParams += ", ";
                    funcParams += func.argTypeInfo( k ).apply_visitor( typeVisitor );
                    paramFirst = false;
                }
                funcParams += "]";

                if( !first ) funcs += ", ";
                funcs += "Function(" + funcCons + ", " + funcReturn + ", " + funcParams + ")";
                first = false; 

                sdfTypeFunc->contextfree( funcNameS, "FunctionName", funcName );
                strTypeFunc->rule( "has-builtin-type-func", list_of(classCons), "?" + funcCons );
                strTypeFunc->matchRule( "builtin-type-func-return", list_of(classCons), funcCons, funcReturn );
                strTypeFunc->matchRule( "builtin-type-func-params", list_of(classCons), funcCons, funcParams );
            }
            if( funcCount ) sdfTypeFunc->writeLn();
            strTypeFunc->matchRule( "builtin-type-funcs", classCons, funcs + "]" );
            strTypeFunc->writeLn();
        }

        // Operators
        {
            String ops = "![";
            size_t opCount = metaclass.operatorCount();
            bool first = true;
            for( size_t j = 0; j < opCount; ++j )
            {
                const camp::Function& func = metaclass.getOperator( j );
                String opCons = operatorString( func.operatorType() );
                String opReturn = func.returnTypeInfo().apply_visitor( typeVisitor );
                String opParam = func.argTypeInfoSafe<0>().apply_visitor( typeVisitor );

                if( !first ) ops += ", ";
                ops += "Operator(" + opCons + ", " + opReturn + ", " + opParam + ")";
                first = false;

                strTypeOp->rule( "has-builtin-type-op", list_of(classCons), "?" + opCons );
                strTypeOp->rule( "has-builtin-type-op", list_of(classCons), "?(" + opCons + ", " + opParam + ")" );
                strTypeOp->matchRule( "builtin-type-op-return", list_of(classCons), "(" + opCons + ", " + opParam + ")", opReturn );
            }
            strTypeOp->matchRule( "builtin-type-ops", classCons, ops + "]" );
            strTypeOp->writeLn();
        }
    }

    // Enums
    size_t enumCount = camp::enumCount();
    for( size_t i = 0; i < enumCount; ++i )
    {
        const camp::Enum& metaenum = camp::enumByIndex( i );
        LOGI << metaenum.name();

        // Enum values
        size_t enumValueCount = metaenum.size();
        for( size_t j = 0; j < enumValueCount; ++j )
        {
            const camp::Enum::Pair& pair = metaenum.pair(j);

            //"EnumValue(\"" << pair.name << "\", " << pair.value << ")";
        }
    }

    // Components
    const ComponentFactories& components = ComponentFactoryManager::getComponentFactories();
    for( ComponentFactories::const_iterator i = components.begin(); i != components.end(); ++i )
    {
        if( Object::hasAutoCreateComponent( i->first ) ) continue;

        //"\"" << i->second->getTypeName() << "\"";
    }

    // Components that can have multiple instances
    for( ComponentFactories::const_iterator i = components.begin(); i != components.end(); ++i )
    {
        if( Object::hasAutoCreateComponent( i->first ) ) continue;
        if( !i->second->multiple() ) continue;

        //"\"" << i->second->getTypeName() << "\"";
    }

    // ClientServerPlugins
    const ClientServerPluginFactories& plugins = ClientServerPluginFactoryManager::getPluginFactories();
    for( ClientServerPluginFactories::const_iterator i = plugins.begin(); i != plugins.end(); ++i )
    {
        if( ClientServerPluginManager::hasAutoCreatePlugin( i->first ) ) continue;
        
        //"\"" << i->second->getTypeName() << "\"";
    }
    
    delete logger;
    return 0;
}

#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    main( 0, 0 );
}
#endif
