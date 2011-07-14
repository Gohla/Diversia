/*
--------------------------------------------------------------------------------
This file is part of Diversia engine
http://www.diversia.org/
Copyright (c) 2008-2011 Gabriël Konat
--------------------------------------------------------------------------------
*/

#ifndef DIVERSIA_UTIL_MEMORYSERIALIZATIONIMPL_H
#define DIVERSIA_UTIL_MEMORYSERIALIZATIONIMPL_H

#include <camp/classvisitor.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

namespace Diversia
{
namespace Util
{
namespace Detail
{
//------------------------------------------------------------------------------

// Forward declarations
struct MemoryUserObject;
struct MemoryArray;
struct MemoryDictionary;

// Recursive variant
typedef boost::variant< bool, long, double, String, boost::recursive_wrapper<MemoryUserObject>, 
    boost::recursive_wrapper<MemoryArray>, boost::recursive_wrapper<MemoryDictionary> > MemoryItem;

typedef std::map<String, MemoryItem> MemoryProperties;
struct MemoryUserObject
{
    MemoryUserObject();

    boost::shared_ptr<MemoryProperties> mProperties;
};

typedef std::vector<MemoryItem> MemoryVector;
struct MemoryArray
{
    MemoryArray();

    boost::shared_ptr<MemoryVector> mArray;
};

typedef std::map<camp::Value, MemoryItem> MemoryMap;
struct MemoryDictionary
{
    MemoryDictionary();

    boost::shared_ptr<MemoryMap> mDictionary;
};

//------------------------------------------------------------------------------

struct ValueToMemory : public camp::ValueVisitor<MemoryItem>
{
    ValueToMemory( const camp::Args& rTags, bool include, bool throwExceptions );

    MemoryItem operator()( camp::NoType );
    MemoryItem operator()( bool value );
    MemoryItem operator()( long value );
    MemoryItem operator()( double value );
    MemoryItem operator()( const std::string& value );
    MemoryItem operator()( const camp::EnumObject& value );
    MemoryItem operator()( const camp::UserObject& value );

    camp::Args mTags;
    bool mInclude;
    bool mThrowExceptions;
};

struct MemorySerializer : public camp::ClassVisitor
{
    MemorySerializer( const camp::UserObject& rObject, const camp::Args& rTags, 
        bool include, bool throwExceptions );

    void visit( const camp::SimpleProperty& property );
    void visit( const camp::ArrayProperty& property );
    void visit( const camp::DictionaryProperty& property );
    void visit( const camp::EnumProperty& property );
    void visit( const camp::UserProperty& property );

    bool querySerialize( const camp::Property& property );
    bool querySerialize( const camp::Value& value );

    camp::UserObject mObject;
    camp::Args mTags;
    bool mInclude;
    bool mThrowExceptions;
    MemoryUserObject mMemoryObject;
    ValueToMemory mToMemory;
};

//------------------------------------------------------------------------------

struct MemoryToValue : public boost::static_visitor<camp::Value>
{
    camp::Value operator()( bool value );
    camp::Value operator()( long value );
    camp::Value operator()( double value );
    camp::Value operator()( String value );
    camp::Value operator()( MemoryUserObject value );
    camp::Value operator()( MemoryArray value );
    camp::Value operator()( MemoryDictionary value );
};

struct MemoryDeserializer : public camp::ClassVisitor
{
    MemoryDeserializer( const camp::UserObject& rObject, const MemoryUserObject& rMemoryObject, 
        const camp::Value& rTag, bool include, bool throwExceptions );
    MemoryDeserializer( const camp::UserObject& rObject, const MemoryUserObject& rMemoryObject, 
        const MemoryDeserializer& rDeserializer );

    void visit( const camp::SimpleProperty& property );
    void visit( const camp::ArrayProperty& property );
    void visit( const camp::DictionaryProperty& property );
    void visit( const camp::EnumProperty& property );
    void visit( const camp::UserProperty& property );

    bool querySerialize( const camp::Property& property );

    camp::UserObject mObject;
    camp::Value mTag;
    bool mInclude;
    bool mThrowExceptions;
    const MemoryUserObject& mMemoryObject;
    
    static MemoryToValue mToValue;
};

//------------------------------------------------------------------------------
} // Namespace Detail
} // Namespace Util
} // Namespace Diversia

#endif // DIVERSIA_UTIL_MEMORYSERIALIZATIONIMPL_H
