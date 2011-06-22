/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_DSLTYPEEXTRACTOR_CAMPTOOLS_H
#define DIVERSIA_DSLTYPEEXTRACTOR_CAMPTOOLS_H

#include "DSLTypeExtractor/Platform/Prerequisites.h"

#include <camp/typeinfo.hpp>
#include <camp/operator.hpp>

namespace Diversia
{
namespace DSLTypeExtractor
{
//------------------------------------------------------------------------------

using namespace std;
using namespace boost;
using namespace boost::assign;

struct TypeVisitor : public camp::TypeVisitor<String>
{
    String operator()( camp::Type type )
    {
        switch( type )
        {
            case camp::noType: return "VoidType()";
            case camp::boolType: return "BoolType()";
            case camp::intType: return "IntType()";
            case camp::realType: return "RealType()";
            case camp::stringType: return "StringType()";
            case camp::valueType: return "VoidType()";
        }

        return "VoidType()";
    }
    String operator()( const camp::Class* pClass )
    {
        return pClass->name() + "()";
    }
    String operator()( const camp::Enum* pEnum )
    {
        return pEnum->name() + "()";
    }
    String operator()( camp::ArrayType type )
    {
        // TODO: Implement array types in DSL.
        return "VoidType()";
    }
    String operator()( camp::DictionaryType type )
    {
        // TODO: Implement dictionary types in DSL.
        return "VoidType()";
    }
};

inline String operatorString( camp::OperatorType type )
{
    switch( type )
    {
        case camp::noop: return "NOOP()";
        case camp::add: return "ADD()";
        case camp::sub: return "SUB()";
        case camp::mul: return "MUL()";
        case camp::div: return "DIV()";
        case camp::uplus: return "UPLUS()";
        case camp::umin: return "UMIN()";
        case camp::mod: return "MOD()";
        case camp::preinc: return "PREINC()";
        case camp::postinc: return "POSTINC()";
        case camp::predec: return "PREDEC()";
        case camp::postdec: return "POSTDEC()";
        case camp::eq: return "EQ()";
        case camp::neq: return "NEQ()";
        case camp::lt: return "LT()";
        case camp::lte: return "LTE()";
        case camp::gt: return "GT()";
        case camp::gte: return "GTE()";
        case camp::not: return "NOT()";
        case camp::and: return "AND()";
        case camp::or: return "OR()";
        default: return "NOOP()";
    }
}

//------------------------------------------------------------------------------
} // Namespace DSLTypeExtractor
} // Namespace Diversia

#endif // DIVERSIA_DSLTYPEEXTRACTOR_CAMPTOOLS_H