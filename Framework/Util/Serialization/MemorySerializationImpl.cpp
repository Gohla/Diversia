/*
--------------------------------------------------------------------------------
This file is part of Diversia engine
http://www.diversia.org/
Copyright (c) 2008-2011 Gabriël Konat
--------------------------------------------------------------------------------
*/

#include "Util/Platform/StableHeaders.h"

#include "Util/Serialization/MemorySerializationImpl.h"

namespace Diversia
{
namespace Util 
{
namespace Detail
{
//------------------------------------------------------------------------------

ValueToMemory::ValueToMemory( const camp::Args& rTags, bool include, bool throwExceptions ): 
    mTags( rTags ),
    mInclude( include ),
    mThrowExceptions( throwExceptions )
{

}

MemoryItem ValueToMemory::operator()( camp::NoType )
{
    DivAssert( 0, "Trying to convert a void type to memory." );
}

MemoryItem ValueToMemory::operator()( bool value )
{
    return value;
}

MemoryItem ValueToMemory::operator()( long value )
{
    return value;
}

MemoryItem ValueToMemory::operator()( double value )
{
    return value;
}

MemoryItem ValueToMemory::operator()( const std::string& value )
{
    return value;
}

MemoryItem ValueToMemory::operator()( const camp::EnumObject& value )
{
    return value.value();
}

MemoryItem ValueToMemory::operator()( const camp::UserObject& value )
{
    MemorySerializer serializer( value, mTags, mInclude, mThrowExceptions );
    value.getClass().visit( serializer );
    return serializer.mMemoryObject;
}

//------------------------------------------------------------------------------

MemorySerializer::MemorySerializer( const camp::UserObject& rObject, const camp::Args& rTags, 
    bool include, bool throwExceptions ):
    mObject( rObject ),
    mTags( rTags ),
    mInclude( include ),
    mThrowExceptions( throwExceptions ), 
    mToMemory( ValueToMemory( rTags, include, throwExceptions ) )
{

}

void MemorySerializer::visit( const camp::SimpleProperty& property )
{
    if( !querySerialize( property ) ) return;

    camp::Value value = property.get( mObject );
    mMemoryObject.mProperties.insert( std::make_pair( property.name(), value.visit( mToMemory ) ) );
}

void MemorySerializer::visit( const camp::ArrayProperty& property )
{
    if( !querySerialize( property ) ) return;

    MemoryArray memoryArray;

    std::size_t size = property.size( mObject );
    for( std::size_t i = 0; i < size; ++i )
    {
        camp::Value value = property.get( mObject, i );
        if( !querySerialize( value ) ) continue;
        memoryArray.mArray.push_back( value.visit( mToMemory ) );
    }

    mMemoryObject.mProperties.insert( std::make_pair( property.name(), memoryArray ) );
}

void MemorySerializer::visit( const camp::DictionaryProperty& property )
{
    if( !querySerialize( property ) ) return;

    MemoryDictionary memoryDictionary;

    camp::DictionaryIteratorPtr i = property.iterator( mObject );
    while( i->valid() )
    {
        camp::Value value = i->value();
        if( !querySerialize( value ) )
        {
            i->next();
            continue;
        }
        camp::Value key = i->key();
        

        // Copy the key for user types.
        if( key.type() == camp::userType )
            key = key.to<camp::UserObject>().copy();

        memoryDictionary.mDictionary.insert( std::make_pair( key, value.visit( mToMemory ) ) );

        i->next();
    }

    mMemoryObject.mProperties.insert( std::make_pair( property.name(), memoryDictionary ) );
}

void MemorySerializer::visit( const camp::EnumProperty& property )
{
    if( !querySerialize( property ) ) return;

    camp::Value value = property.get( mObject );
    mMemoryObject.mProperties.insert( std::make_pair( property.name(), value.visit( mToMemory ) ) );
}

void MemorySerializer::visit( const camp::UserProperty& property )
{
    if( !querySerialize( property ) ) return;

    camp::Value value = property.get( mObject );
    if( !querySerialize( value ) ) return;
    mMemoryObject.mProperties.insert( std::make_pair( property.name(), value.visit( mToMemory ) ) );
}

bool MemorySerializer::querySerialize( const camp::Property& property )
{
    for( std::size_t i = 0; i < mTags.count(); ++i )
    {
        const camp::Value& tag = mTags[i];
        if( mInclude && !property.hasTag( tag ) ) return false;
        else if( !mInclude && property.hasTag( tag ) ) return false;
    }
    
    return true;
}

bool MemorySerializer::querySerialize( const camp::Value& value )
{
    switch( value.type() )
    {
        case camp::userType:
            camp::UserObject object = value.to<camp::UserObject>();
            const camp::Class& metaclass = object.getClass();

            for( std::size_t i = 0; i < mTags.count(); ++i )
            {
                const camp::Value& tag = mTags[i];

                try
                {
                    return !metaclass.tag( tag, object ).to<bool>();
                }
                catch( const camp::Error& e )
                {
                    // Class doesn't have that tag or value is not a boolean.
                    return true;
                }
            }
    }

    return true;
}

//------------------------------------------------------------------------------

camp::Value MemoryToValue::operator()( bool value )
{
    return camp::Value( value );
}

camp::Value MemoryToValue::operator()( long value )
{
    return camp::Value( value );
}

camp::Value MemoryToValue::operator()( double value )
{
    return camp::Value( value );
}

camp::Value MemoryToValue::operator()( String value )
{
    return camp::Value( value );
}

camp::Value MemoryToValue::operator()( MemoryUserObject value )
{
    DivAssert( 0, "Trying to convert a user object to a camp::Value." );
}

camp::Value MemoryToValue::operator()( MemoryArray value )
{
    DivAssert( 0, "Trying to convert an array to a camp::Value." );
}

camp::Value MemoryToValue::operator()( MemoryDictionary value )
{
    DivAssert( 0, "Trying to convert a dictionary to a camp::Value." );
}

//------------------------------------------------------------------------------

MemoryDeserializer::MemoryDeserializer( const camp::UserObject& rObject, 
    const MemoryUserObject& rMemoryObject, const camp::Value& rTag, bool include, 
    bool throwExceptions ):
    mObject( rObject ),
    mTag( rTag ),
    mInclude( include ),
    mThrowExceptions( throwExceptions ),
    mMemoryObject( rMemoryObject )
{

}

void MemoryDeserializer::visit( const camp::SimpleProperty& property )
{
    if( !querySerialize( property ) ) return;
    MemoryProperties::const_iterator i = mMemoryObject.mProperties.find( property.name() );
    if( i != mMemoryObject.mProperties.end() )
    {
        MemoryItem item = i->second;
        property.set( mObject, item.apply_visitor( mToValue ) );
    }
}

void MemoryDeserializer::visit( const camp::ArrayProperty& property )
{
    if( !querySerialize( property ) ) return;
}

void MemoryDeserializer::visit( const camp::DictionaryProperty& property )
{
    if( !querySerialize( property ) ) return;
}

void MemoryDeserializer::visit( const camp::EnumProperty& property )
{
    if( !querySerialize( property ) ) return;
}

void MemoryDeserializer::visit( const camp::UserProperty& property )
{
    if( !querySerialize( property ) ) return;
}

bool MemoryDeserializer::querySerialize( const camp::Property& property )
{
    return mTag != camp::Value::nothing && ( mInclude ? !property.hasTag( mTag ) : property.hasTag( mTag ) );
}

//------------------------------------------------------------------------------
} // Namespace Detail
} // Namespace Util
} // Namespace Diversia
