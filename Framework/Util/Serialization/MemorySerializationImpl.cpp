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

MemoryUserObject::MemoryUserObject(): mProperties( boost::make_shared<MemoryProperties>() )
{

}


MemoryArray::MemoryArray(): mArray( boost::make_shared<MemoryVector>() )
{

}


MemoryDictionary::MemoryDictionary(): mDictionary( boost::make_shared<MemoryMap>() )
{

}

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
    return false;
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
    try
    {
	    if( !querySerialize( property ) ) return;
	
	    camp::Value value = property.get( mObject );
	    mMemoryObject.mProperties->insert( std::make_pair( property.name(), value.visit( mToMemory ) ) );
    }
    catch( const camp::Error& e )
    {
        if( mThrowExceptions ) throw e;
    }
}

void MemorySerializer::visit( const camp::ArrayProperty& property )
{
    try
    {
	    if( !querySerialize( property ) ) return;
	
	    MemoryArray memoryArray;
	
	    std::size_t size = property.size( mObject );
	    memoryArray.mArray->reserve( size );
	    for( std::size_t i = 0; i < size; ++i )
	    {
	        camp::Value value = property.get( mObject, i );
	        if( !querySerialize( value ) ) continue;
	        memoryArray.mArray->push_back( value.visit( mToMemory ) );
	    }
	
	    mMemoryObject.mProperties->insert( std::make_pair( property.name(), memoryArray ) );
    }
    catch( const camp::Error& e )
    {
        if( mThrowExceptions ) throw e;
    }
}

void MemorySerializer::visit( const camp::DictionaryProperty& property )
{
    try
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
	
	        memoryDictionary.mDictionary->insert( std::make_pair( key, value.visit( mToMemory ) ) );
	
	        i->next();
	    }
	
	    mMemoryObject.mProperties->insert( std::make_pair( property.name(), memoryDictionary ) );
    }
    catch( const camp::Error& e )
    {
        if( mThrowExceptions ) throw e;
    }
}

void MemorySerializer::visit( const camp::EnumProperty& property )
{
    try
    {
	    if( !querySerialize( property ) ) return;
	
	    camp::Value value = property.get( mObject );
	    mMemoryObject.mProperties->insert( std::make_pair( property.name(), value.visit( mToMemory ) ) );
    }
    catch( const camp::Error& e )
    {
        if( mThrowExceptions ) throw e;
    }
}

void MemorySerializer::visit( const camp::UserProperty& property )
{
    try
    {
	    if( !querySerialize( property ) ) return;
	
	    camp::Value value = property.get( mObject );
	    if( !querySerialize( value ) ) return;
	    mMemoryObject.mProperties->insert( std::make_pair( property.name(), value.visit( mToMemory ) ) );
    }
    catch( const camp::Error& e )
    {
        if( mThrowExceptions ) throw e;
    }
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
    return camp::Value::nothing;
}

camp::Value MemoryToValue::operator()( MemoryArray value )
{
    DivAssert( 0, "Trying to convert an array to a camp::Value." );
    return camp::Value::nothing;
}

camp::Value MemoryToValue::operator()( MemoryDictionary value )
{
    DivAssert( 0, "Trying to convert a dictionary to a camp::Value." );
    return camp::Value::nothing;
}

//------------------------------------------------------------------------------

MemoryToValue MemoryDeserializer::mToValue = MemoryToValue();

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

MemoryDeserializer::MemoryDeserializer( const camp::UserObject& rObject, 
    const MemoryUserObject& rMemoryObject, const MemoryDeserializer& rDeserializer ):
    mObject( rObject ),
    mTag( rDeserializer.mTag ),
    mInclude( rDeserializer.mInclude ),
    mThrowExceptions( rDeserializer.mThrowExceptions ),
    mMemoryObject( rMemoryObject )
{

}

void MemoryDeserializer::visit( const camp::SimpleProperty& property )
{
    try
    {
        if( !querySerialize( property ) ) return;

        MemoryProperties::const_iterator it = mMemoryObject.mProperties->find( property.name() );
        if( it == mMemoryObject.mProperties->end() ) return;

        property.set( mObject, it->second.apply_visitor( mToValue ) );
    }
    catch( const camp::Error& e )
    {
        if( mThrowExceptions ) throw e;
    }
}

void MemoryDeserializer::visit( const camp::ArrayProperty& property )
{
    try
    {
	    if( !querySerialize( property ) ) return;
	
	        MemoryProperties::const_iterator it = mMemoryObject.mProperties->find( property.name() );
	        if( it == mMemoryObject.mProperties->end() ) return;
	
	        const MemoryArray& memoryArray = boost::get<const MemoryArray&>( it->second );
	
		    // Make sure that there are enough elements in the array.
	        std::size_t size = memoryArray.mArray->size();
		    std::size_t campSize = property.size( mObject );
	        if( size > campSize )
	        {
	            if( property.dynamic() )
		            property.resize( mObject, size );
		        else
		            return;
	        }
	
	        std::size_t index = 0;
	        for( MemoryVector::const_iterator i = memoryArray.mArray->begin(); i != memoryArray.mArray->end(); 
	            ++i )
	        {
	            if( property.elementType() == camp::userType )
		        {
	                const MemoryUserObject& memoryObject = boost::get<const MemoryUserObject&>( *i );
	
	                // The array elements are composed objects: deserialize them recursively
	                camp::UserObject userObject = property.get( mObject, index ).to<camp::UserObject>();
	                MemoryDeserializer deserializer( userObject, memoryObject, *this );
	                userObject.getClass().visit( deserializer );
	
	                if( property.hasTag( "DeserializeCopySet" ) )
	                {
	                    // Set the recursively deserialized user object
	                    property.set( mObject, index, userObject );
	                }
	                else if( property.hasTag( "DeserializeReSet" ) )
	                {
	                    // Re set the property if it has the DeserializeReSet tag
	                    camp::Value val = property.get( mObject, index );
	                    property.set( mObject, index, val );
	                }
	            }
	            else
	            {
	                // The array elements are simple properties: read their value from the text of their XML node
	                property.set( mObject, index, (*i).apply_visitor( mToValue ) );
	            }
	
	            ++index;
	        }
    }
    catch( const camp::Error& e )
    {
        if( mThrowExceptions ) throw e;
    }
}

void MemoryDeserializer::visit( const camp::DictionaryProperty& property )
{
    try
    {
	    if( !querySerialize( property ) ) return;
	
	    MemoryProperties::const_iterator it = mMemoryObject.mProperties->find( property.name() );
	    if( it == mMemoryObject.mProperties->end() ) return;
	
	    const MemoryDictionary& memoryDictionary = boost::get<const MemoryDictionary&>( it->second );
	
	    std::size_t index = 0;
	    for( MemoryMap::const_iterator i = memoryDictionary.mDictionary->begin(); 
	        i != memoryDictionary.mDictionary->end(); ++i )
	    {
	        // Deserialize element key
	        camp::Value keyValue = i->first;
	
	        // Deserialize element value
	        if( property.elementType() == camp::userType )
		    {
	            const MemoryUserObject& memoryObject = boost::get<const MemoryUserObject&>( i->second );
	
		        // Add key to dictionary before deserializing the value
		        if( property.hasTag( "AddFunction" ) && !property.exists( mObject, keyValue ) )
		        {
		            mObject.getClass().function( property.tag( "AddFunction" ).to<String>() ).call( 
	                    mObject, keyValue );
		        }
	
	            // The array elements are composed objects: deserialize them recursively
	            camp::UserObject userObject = property.get( mObject, keyValue ).to<camp::UserObject>();
	            MemoryDeserializer deserializer( userObject, memoryObject, *this );
	            userObject.getClass().visit( deserializer );
	
	            if( property.hasTag( "DeserializeCopySet" ) )
	            {
	                // Set the recursively deserialized user object
	                property.set( mObject, keyValue, userObject );
	            }
	            else if( property.hasTag( "DeserializeReSet" ) )
	            {
	                // Re set the property if it has the DeserializeReSet tag
	                camp::Value val = property.get( mObject, keyValue );
	                property.set( mObject, keyValue, val );
	            }
		    }
	        else if( property.elementType() == camp::valueType )
	        {
	            /*
	            Value val = deserializeErasureValue<Proxy>(value, tag, include, throwExceptions);
	
	            // Add key to dictionary before deserializing the value
	            if(property.hasTag("SetFunction"))
	            {
	                metaclass.function(property.tag("SetFunction").to<std::string>()).call(object, Args(keyValue, val));
	            }
	            else if(property.hasTag("AddFunction") && dictionaryProperty.exists(object, keyValue))
	            {
	                metaclass.function(property.tag("AddFunction").to<std::string>()).call(object, keyValue);
	            }
	            else
	            {
	                property.set(object, keyValue, val);
	            }*/
	        }
		    else
		    {
		        // The array elements are simple properties: read their value from the text of their XML node
		        property.set( mObject, keyValue,  i->second.apply_visitor( mToValue ) );
		    }
	    }
    }
    catch( const camp::Error& e )
    {
        if( mThrowExceptions ) throw e;
    }
}

void MemoryDeserializer::visit( const camp::EnumProperty& property )
{
    try
    {
	    if( !querySerialize( property ) ) return;
	
	    MemoryProperties::const_iterator it = mMemoryObject.mProperties->find( property.name() );
	    if( it == mMemoryObject.mProperties->end() ) return;
	
	    property.set( mObject, it->second.apply_visitor( mToValue ) );
    }
    catch( const camp::Error& e )
    {
        if( mThrowExceptions ) throw e;
    }
}

void MemoryDeserializer::visit( const camp::UserProperty& property )
{
    try
    {
	    if( !querySerialize( property ) ) return;
	
	    MemoryProperties::const_iterator it = mMemoryObject.mProperties->find( property.name() );
	    if( it == mMemoryObject.mProperties->end() ) return;
	
	    const MemoryUserObject& memoryUserObject = boost::get<const MemoryUserObject&>( it->second );
	
	    // The current property is a composed type: deserialize it recursively
	    camp::UserObject userObject = property.get( mObject ).to<camp::UserObject>();
	    MemoryDeserializer deserializer( userObject, memoryUserObject, *this );
	    userObject.getClass().visit( deserializer );
	
	    if( property.hasTag( "DeserializeCopySet" ) )
	    {
	        // Set the recursively deserialized user object
	        property.set( mObject, userObject );
	    }
	    else if( property.hasTag( "DeserializeReSet" ) )
	    {
	        camp::Value val = property.get( mObject );
	        property.set( mObject, val );
	    }
    }
    catch( const camp::Error& e )
    {
        if( mThrowExceptions ) throw e;
    }
}

bool MemoryDeserializer::querySerialize( const camp::Property& property )
{
    return mTag == camp::Value::nothing || ( mInclude ? property.hasTag( mTag ) : !property.hasTag( mTag ) );
}

//------------------------------------------------------------------------------
} // Namespace Detail
} // Namespace Util
} // Namespace Diversia
