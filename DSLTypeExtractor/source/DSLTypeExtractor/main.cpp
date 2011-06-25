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
#include "Shared/ClientServerPlugin/ClientServerPluginFactory.h"
#include "Shared/ClientServerPlugin/ClientServerPluginFactoryManager.h"
#include "Shared/ClientServerPlugin/ClientServerPluginManager.h"
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/assign.hpp>
#include <boost/lexical_cast.hpp>

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

inline static String S(const String& s) { return "\"" + s + "\""; };
inline String boolCons( bool b ) { return b ? "True()" : "False()"; }
inline String cons( const String& rName ) { return rName + "()"; }
inline String tupl( const String& t1, const String& t2 ) { return "(" + t1 + ", " + t2 + ")"; }
inline String tupl( const String& t1, const String& t2, const String& t3 ) { return "(" + t1 + ", " + t2 + ", " + t3 + ")"; }
inline bool isObject( const String& rObject ) { return rObject == "Object" || rObject == "ClientObject" || rObject == "Node"; }

int main( int argc, char* argv[] )
{
    // Initialize logging
    Logger* logger = new Logger( LOG_DEBUG, true, false );

    registerCamp();

    // Classes
    {
        scoped_ptr<SDFFile> sdfType( new SDFFile( "type.sdf", "data/type" ) );
        sdfType->contextfree();
        scoped_ptr<STRFile> strType( new STRFile( "type.str", "data/type", list_of("include/DiversiaScript") ) ); 
        strType->rules();

        scoped_ptr<STRFile> strTypeBases( new STRFile( "type-base.str", "data/type-base", list_of("include/DiversiaScript") ) );
        strTypeBases->rules();

        scoped_ptr<STRFile> strTypeCons( new STRFile( "type-cons.str", "data/type-cons", list_of("include/DiversiaScript") ) );
        strTypeCons->rules();

        scoped_ptr<SDFFile> sdfTypeProp( new SDFFile( "type-prop.sdf", "data/type-prop") );
        sdfTypeProp->contextfree();
        scoped_ptr<STRFile> strTypeProp( new STRFile( "type-prop.str", "data/type-prop", list_of("include/DiversiaScript") ) ); 
        strTypeProp->signatures();
        strTypeProp->signature( "Property", list_of("BuiltinProperty")("Type")("Writable"), "Property" );
        strTypeProp->removeIdent();
        strTypeProp->rules();

        scoped_ptr<SDFFile> sdfTypeFunc( new SDFFile( "type-func.sdf", "data/type-func") );
        sdfTypeFunc->contextfree();
        scoped_ptr<STRFile> strTypeFunc( new STRFile( "type-func.str", "data/type-func", list_of("include/DiversiaScript") ) ); 
        strTypeFunc->signatures();
        strTypeFunc->signature( "Function", list_of("FunctionName")("ReturnType")("List(ParamType)"), "Function" );
        strTypeFunc->removeIdent();
        strTypeFunc->rules();

        scoped_ptr<STRFile> strTypeOp( new STRFile( "type-op.str", "data/type-op", list_of("include/DiversiaScript")("signatures") ) ); 
        strTypeOp->signatures();
        strTypeOp->signature( "Operator", list_of("Op")("ReturnType")("ParamType"), "Operator" );
        strTypeOp->removeIdent();
        strTypeOp->rules();

        size_t classCount = camp::classCount();
        for( size_t i = 0; i < classCount; ++i )
        {
            const camp::Class& metaclass = camp::classByIndex( i );
            String className = metaclass.name();
            String classCons = cons( className );
            LOGI << className;
            sdfType->contextfree( S(className), "BuiltinType", className );
            strType->rule( "is-builtin-type", "?" + classCons );

            // Bases
            {
                String bases = "[";
                size_t baseCount = metaclass.baseCount();
                bool first = true;
                for( size_t j = 0; j < baseCount; ++j )
                {
                    String baseCons = cons( metaclass.base( j ).name() );

                    strTypeBases->rule( "is-builtin-type-base", "?" + tupl(classCons, baseCons ) );
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
                    strTypeCons->rule( "has-builtin-type-cons", "?" + tupl( classCons, types + "]" ) );
                }
                if( consCount ) strTypeCons->writeLn();
            }

            // Properties
            {
                String props = "[";
                size_t propCount = metaclass.propertyCount( true );
                bool first = true;
                for( size_t j = 0; j < propCount; ++j )
                {
                    const camp::Property& prop = metaclass.property( j, true );
                    String propName = algorithm::replace_all_copy( prop.name(), " ", "_" ); // TODO: Get rid of spaces in property names.
                    String propNameS = S(propName);
                    String propCons = cons( propName );
                    String propType = prop.typeInfo().apply_visitor( typeVisitor );
                    bool propWritable = prop.writable();

                    if( !first ) props += ", ";
                    props += "Property(" + propCons + ", " + propType + ", " + boolCons( propWritable ) + ")";
                    first = false; 

                    sdfTypeProp->contextfree( propNameS, "BuiltinProperty", propName );
                    if( isObject( className ) ) sdfTypeProp->contextfree( propNameS, "BuiltinObjectProperty", propName );
                    strTypeProp->rule( "has-builtin-type-prop", "?" + tupl( classCons, propCons ) );
                    if( propWritable ) strTypeProp->rule( "is-builtin-type-prop-writable", "?" + tupl( classCons, propCons ) );
                    strTypeProp->matchRule( "builtin-type-prop-type", tupl( classCons, propCons ), propType );
                }
                if( propCount ) sdfTypeProp->writeLn();
                strTypeProp->matchRule( "builtin-type-props", classCons, props + "]" );
                strTypeProp->writeLn();
            }

            // Functions
            {
                String funcs = "[";
                size_t funcCount = metaclass.functionCount( true );
                bool first = true;
                for( size_t j = 0; j < funcCount; ++j )
                {
                    const camp::Function& func = metaclass.function( j, true );
                    String funcName = algorithm::replace_all_copy( func.name(), " ", "_" ); // TODO: Get rid of spaces in function names.
                    String funcNameS = S(funcName);
                    String funcCons = cons( funcName );
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

                    sdfTypeFunc->contextfree( funcNameS, "BuiltinFunction", funcName );
                    strTypeFunc->rule( "has-builtin-type-func", "?" + tupl( classCons, funcCons ) );
                    strTypeFunc->matchRule( "builtin-type-func-return", tupl( classCons, funcCons ), funcReturn );
                    strTypeFunc->matchRule( "builtin-type-func-params", tupl( classCons, funcCons ), funcParams );
                }
                if( funcCount ) sdfTypeFunc->writeLn();
                strTypeFunc->matchRule( "builtin-type-funcs", classCons, funcs + "]" );
                strTypeFunc->writeLn();
            }

            // Operators
            {
                String ops = "[";
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

                    strTypeOp->rule( "has-builtin-type-op", "?" + tupl( classCons, opCons ) );
                    strTypeOp->rule( "has-builtin-type-op", "?" + tupl( classCons, opCons, opParam ) );
                    strTypeOp->matchRule( "builtin-type-op-return", tupl( classCons, opCons, opParam ), opReturn );
                }
                strTypeOp->matchRule( "builtin-type-ops", classCons, ops + "]" );
                strTypeOp->writeLn();
            }
        }
    }
    
    // Enums
    {
        scoped_ptr<SDFFile> sdfEnum( new SDFFile( "enum.sdf", "data/enum" ) );
        sdfEnum->contextfree();
        scoped_ptr<STRFile> strEnum( new STRFile( "enum.str", "data/enum", list_of("include/DiversiaScript") ) );
        strEnum->rules();

        size_t enumCount = camp::enumCount();
        for( size_t i = 0; i < enumCount; ++i )
        {
            const camp::Enum& metaenum = camp::enumByIndex( i );
            String enumName = metaenum.name();
            String enumType = enumName + "EnumType";
            String enumValue = enumName + "Value";
            String enumCons = cons( enumName );
            LOGI << enumName;
            sdfEnum->contextfree( S(enumName), enumType, enumName );
            sdfEnum->contextfree( enumType, "EnumType" );
            sdfEnum->contextfree( enumType + " \".\" " + enumValue, "Exp", "EnumValueRef", list_of("prefer") );
            strEnum->rule( "is-builtin-enum", "?" + enumCons );

            // Enum values
            String values = "[";
            size_t valueCount = metaenum.size();
            bool first = true;
            for( size_t j = 0; j < valueCount; ++j )
            {
                const camp::Enum::Pair& pair = metaenum.pair( j );
                String valueName = algorithm::replace_all_copy( pair.name, " ", "_" ); // TODO: Get rid of spaces in enum value names.
                String valueCons = cons( valueName );
                String value = lexical_cast<String>( pair.value );

                if( !first ) values += ", ";
                values += "(" + valueCons + ", " + value + ")";
                first = false;

                sdfEnum->contextfree( S(pair.name), enumValue, valueName );
                strEnum->rule( "has-builtin-enum-valueName", list_of(enumCons), "?" + valueCons );
                strEnum->matchRule( "builtin-enum-value", list_of(enumCons), valueCons, value );
                strEnum->matchRule( "builtin-enum-name", list_of(enumCons), value, valueCons );
            }
            if( valueCount ) sdfEnum->writeLn();
            strEnum->matchRule( "builtin-enum-values", enumCons, values + "]" );
            strEnum->writeLn();
        }
    }

    // Components
    {
        scoped_ptr<SDFFile> sdfComponent( new SDFFile( "component.sdf", "data/component" ) );
        sdfComponent->contextfree();
        scoped_ptr<STRFile> strComponent( new STRFile( "component.str", "data/component", list_of("include/DiversiaScript") ) );
        strComponent->rules();

        const ComponentFactories& components = ComponentFactoryManager::getComponentFactories();
        for( ComponentFactories::const_iterator i = components.begin(); i != components.end(); ++i )
        {
            if( Object::hasAutoCreateComponent( i->first ) ) continue;

            String componentName = i->second->getTypeName();
            String componentCons = cons( componentName );
            bool componentMultiple = i->second->multiple();
            LOGI << componentName;
            sdfComponent->contextfree( S(componentName), "ComponentType", componentName );
            strComponent->rule( "is-builtin-component", "?" + componentCons );
            if( componentMultiple ) strComponent->rule( "builtin-component-multiple", "?" + componentCons );
        }
    }

    // ClientServerPlugins
    {
        scoped_ptr<SDFFile> sdfPlugin( new SDFFile( "plugin.sdf", "data/plugin" ) );
        sdfPlugin->contextfree();
        scoped_ptr<STRFile> strPlugin( new STRFile( "plugin.str", "data/plugin", list_of("include/DiversiaScript") ) );
        strPlugin->rules();

        const ClientServerPluginFactories& plugins = ClientServerPluginFactoryManager::getPluginFactories();
        for( ClientServerPluginFactories::const_iterator i = plugins.begin(); i != plugins.end(); ++i )
        {
            if( ClientServerPluginManager::hasAutoCreatePlugin( i->first ) ) continue;

            String pluginName = i->second->getTypeName();
            String pluginCons = cons( pluginName );
            LOGI << pluginName;
            sdfPlugin->contextfree( S(pluginName), "PluginType", pluginName );
            strPlugin->rule( "is-builtin-plugin", "?" + pluginCons );
        }
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
