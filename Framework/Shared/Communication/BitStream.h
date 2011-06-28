/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_SHARED_BITSTREAM_H
#define DIVERSIA_SHARED_BITSTREAM_H

#include "Shared/Platform/Prerequisites.h"

#include "Shared/Resource/ResourceInfo.h"
#include "Util/Camp/BindingType.h"

namespace RakNet
{
//------------------------------------------------------------------------------

// Template
//------------------------------------------------------------------------------
/*
/// Read a  from a RakNet bitstream. 
inline RakNet::BitStream& operator>>( RakNet::BitStream& in, & out )
{

    return in;
}
/// Write a  to a RakNet bitstream.
inline RakNet::BitStream& operator<<( RakNet::BitStream& out, & in )
{

    return out;
}
/// Write a const  to a RakNet bitstream.
inline RakNet::BitStream& operator<<( RakNet::BitStream& out, const & in )
{

    return out;
}
*/

// String
//------------------------------------------------------------------------------
/// Read a String from a RakNet bitstream.
inline RakNet::BitStream& operator>>( RakNet::BitStream& in, Diversia::Util::String& out )
{
    RakNet::RakString rakString;
    bool success = in.Read( rakString );
    DivAssert( success, "Reading String from bitstream failed" );
    out = rakString.C_String();
    return in;
}
/// Write a String to a RakNet bitstream.
inline RakNet::BitStream& operator<<( RakNet::BitStream& out, Diversia::Util::String& in )
{
    RakNet::RakString rakString = in.c_str();
    out.Write( rakString );
    return out;
}
/// Write a const String to a RakNet bitstream.
inline RakNet::BitStream& operator<<( RakNet::BitStream& out, const Diversia::Util::String& in )
{
    RakNet::RakString rakString = in.c_str();
    out.Write( rakString );
    return out;
}

// String
//------------------------------------------------------------------------------
/// Read a Path from a RakNet bitstream.
inline RakNet::BitStream& operator>>( RakNet::BitStream& in, Diversia::Util::Path& out )
{
    Diversia::Util::String pathString; in >> pathString;
    out = pathString;
    return in;
}
/// Write a String to a RakNet bitstream.
inline RakNet::BitStream& operator<<( RakNet::BitStream& out, Diversia::Util::Path& in )
{
    out << in.string();
    return out;
}
/// Write a const String to a RakNet bitstream.
inline RakNet::BitStream& operator<<( RakNet::BitStream& out, const Diversia::Util::Path& in )
{
    out << in.string();
    return out;
}

// std::map
//------------------------------------------------------------------------------
/// Read a std::map from a RakNet bitstream. 
template <class Key, class Value> inline RakNet::BitStream& operator>>( 
    RakNet::BitStream& in, std::map<Key, Value>& out )
{
    unsigned int size;
    bool success = in.Read<unsigned int>( size );
    DivAssert( success, "Reading std::map (size) from bitstream failed" );

    for( unsigned int i = 0; i < size; ++i )
    {
        Key key; 
        Value value;
        in >> key;
        in >> value;
        out.insert( std::make_pair( key, value ) );
    }
    
    return in;
}
/// Read a std::map with pointers from a RakNet bitstream.
template <class Key, class Value> inline RakNet::BitStream& operator>>( 
    RakNet::BitStream& in, std::map<Key, Value*>& out )
{
    unsigned int size;
    bool success = in.Read<unsigned int>( size );
    DivAssert( success, "Reading std::map (size) from bitstream failed" );

    for( unsigned int i = 0; i < size; ++i )
    {
        Key key; 
        Value value;
        in >> key;
        in >> value;
        Value* valueNew = new Value( value );
        out.insert( std::make_pair( key, valueNew ) );
    }
    
    return in;
}
/// Write a std::map to a RakNet bitstream.
template <typename Key, typename Value> inline RakNet::BitStream& operator<<( 
    RakNet::BitStream& out, std::map<Key, Value>& in )
{
    out.Write<unsigned int>( in.size() );

    typename std::map<Key,Value>::iterator i;

    for( i = in.begin(); i != in.end(); ++i )
    {
        out << i->first;
        out << i->second;
    }

    return out;
}
/// Write a const std::map to a RakNet bitstream.
template <typename Key, typename Value> inline RakNet::BitStream& operator<<( 
    RakNet::BitStream& out, const std::map<Key, Value>& in )
{
    out.Write<unsigned int>( in.size() );

    typename std::map<Key,Value>::const_iterator i;

    for( i = in.begin(); i != in.end(); ++i )
    {
        out << i->first;
        out << i->second;
    }

    return out;
}
/// Write a std::map with pointers to a RakNet bitstream.
template <typename Key, typename Value> inline RakNet::BitStream& operator<<( 
    RakNet::BitStream& out, std::map<Key, Value*>& in )
{
    out.Write<unsigned int>( in.size() );

    typename std::map<Key,Value*>::iterator i;

    for( i = in.begin(); i != in.end(); ++i )
    {
        out << i->first;
        out << *i->second;
    }

    return out;
}
/// Write a const std::map with pointers to a RakNet bitstream.
template <typename Key, typename Value> inline RakNet::BitStream& operator<<( 
    RakNet::BitStream& out, const std::map<Key, Value*>& in )
{
    out.Write<unsigned int>( in.size() );

    typename std::map<Key,Value*>::const_iterator i;

    for( i = in.begin(); i != in.end(); ++i )
    {
        out << i->first;
        out << *i->second;
    }

    return out;
}

// std::multimap
//------------------------------------------------------------------------------
/// Read a std::multimap from a RakNet bitstream. 
template <class Key, class Value> inline RakNet::BitStream& operator>>( 
    RakNet::BitStream& in, std::multimap<Key, Value>& out )
{
    unsigned int size;
    bool success = in.Read<unsigned int>( size );
    DivAssert( success, "Reading std::multimap (size) from bitstream failed" );

    for( unsigned int i = 0; i < size; ++i )
    {
        Key key; 
        Value value;
        in >> key;
        in >> value;
        out.insert( std::make_pair( key, value ) );
    }

    return in;
}
/// Read a std::multimap with pointers from a RakNet bitstream.
template <class Key, class Value> inline RakNet::BitStream& operator>>( 
    RakNet::BitStream& in, std::multimap<Key, Value*>& out )
{
    unsigned int size;
    bool success = in.Read<unsigned int>( size );
    DivAssert( success, "Reading std::multimap (size) from bitstream failed" );

    for( unsigned int i = 0; i < size; ++i )
    {
        Key key; 
        Value value;
        in >> key;
        in >> value;
        Value* valueNew = new Value( value );
        out.insert( std::make_pair( key, valueNew ) );
    }

    return in;
}
/// Write a std::multimap to a RakNet bitstream.
template <typename Key, typename Value> inline RakNet::BitStream& operator<<( 
    RakNet::BitStream& out, std::multimap<Key, Value>& in )
{
    out.Write<unsigned int>( in.size() );

    typename std::multimap<Key,Value>::iterator i;

    for( i = in.begin(); i != in.end(); ++i )
    {
        out << i->first;
        out << i->second;
    }

    return out;
}
/// Write a const std::multimap to a RakNet bitstream.
template <typename Key, typename Value> inline RakNet::BitStream& operator<<( 
    RakNet::BitStream& out, const std::multimap<Key, Value>& in )
{
    out.Write<unsigned int>( in.size() );

    typename std::multimap<Key,Value>::const_iterator i;

    for( i = in.begin(); i != in.end(); ++i )
    {
        out << i->first;
        out << i->second;
    }

    return out;
}
/// Write a std::multimap with pointers to a RakNet bitstream.
template <typename Key, typename Value> inline RakNet::BitStream& operator<<( 
    RakNet::BitStream& out, std::multimap<Key, Value*>& in )
{
    out.Write<unsigned int>( in.size() );

    typename std::multimap<Key,Value*>::iterator i;

    for( i = in.begin(); i != in.end(); ++i )
    {
        out << i->first;
        out << *i->second;
    }

    return out;
}
/// Write a const std::multimap with pointers to a RakNet bitstream.
template <typename Key, typename Value> inline RakNet::BitStream& operator<<( 
    RakNet::BitStream& out, const std::multimap<Key, Value*>& in )
{
    out.Write<unsigned int>( in.size() );

    typename std::multimap<Key,Value*>::const_iterator i;

    for( i = in.begin(); i != in.end(); ++i )
    {
        out << i->first;
        out << *i->second;
    }

    return out;
}

// std::set
//------------------------------------------------------------------------------
/// Read a std::set from a RakNet bitstream. 
template <typename Value> inline RakNet::BitStream& operator>>( RakNet::BitStream& in, 
    std::set<Value>& out )
{
    unsigned int size;
    bool success = in.Read<unsigned int>( size );
    DivAssert( success, "Reading std::set (size) from bitstream failed" );

    for( unsigned int i = 0; i < size; ++i )
    {
        Value value;
        in >> value;
        out.insert( value );
    }

    return in;
}
/// Write a std::set to a RakNet bitstream.
template <typename Value> inline RakNet::BitStream& operator<<( RakNet::BitStream& out, 
    std::set<Value>& in )
{
    out.Write<unsigned int>( in.size() );

    typename std::set<Value>::const_iterator i;

    for( i = in.begin(); i != in.end(); ++i )
    {
        out << *i;
    }

    return out;
}
/// Write a const std::set to a RakNet bitstream.
template <typename Value> inline RakNet::BitStream& operator<<( RakNet::BitStream& out, 
    const std::set<Value>& in )
{
    out.Write<unsigned int>( in.size() );

    typename std::set<Value>::const_iterator i;

    for( i = in.begin(); i != in.end(); ++i )
    {
        out << *i;
    }

    return out;
}

// Camp Value
//------------------------------------------------------------------------------
/// Read a Camp Value from a RakNet bitstream. 
#define BITSTREAM_TO_VALUE_PRIMITIVE( name ) \
case BindingType_ ## name: \
{ \
    name object; \
    success = in.Read<name>( object ); \
    DivAssert( success, "Reading Value from bitstream failed" ); \
    out = object; \
    break; \
}

#define BITSTREAM_TO_VALUE_TYPE( name ) \
case BindingType_ ## name: \
{ \
    name object; \
    in >> object; \
    out = camp::UserObject::copy( object ); \
    break; \
}

#define VALUE_TO_BITSTREAM_TYPE( name ) \
case BindingType_ ## name: \
{ \
    rBitStream << object.get<name>(); \
    break; \
}

inline RakNet::BitStream& operator>>( RakNet::BitStream& in, camp::Value& out )
{
    using namespace Diversia::Util;
    using namespace Diversia;

    // Read type
    BindingType bindingType;
    bool success = in.Read<BindingType>( bindingType );
    DivAssert( success, "Reading Value (type) from bitstream failed" );

    // Read object
    switch( bindingType )
    {
        // Primitive types.
        BITSTREAM_TO_VALUE_PRIMITIVE( bool )
        BITSTREAM_TO_VALUE_PRIMITIVE( long )
        BITSTREAM_TO_VALUE_PRIMITIVE( double )
        // String type.
        case BindingType_String:
        {
            String string; in >> string;
            out = string;
            break;
        }
        // User types.
        BITSTREAM_TO_VALUE_TYPE( Vector2 )
        BITSTREAM_TO_VALUE_TYPE( Vector3 )
        BITSTREAM_TO_VALUE_TYPE( Vector4 )
        BITSTREAM_TO_VALUE_TYPE( Colour )
        BITSTREAM_TO_VALUE_TYPE( Quaternion )
        BITSTREAM_TO_VALUE_TYPE( Matrix3 )
        BITSTREAM_TO_VALUE_TYPE( Matrix4 )
        BITSTREAM_TO_VALUE_TYPE( Radian )
        BITSTREAM_TO_VALUE_TYPE( Degree )
        BITSTREAM_TO_VALUE_TYPE( Angle )
        BITSTREAM_TO_VALUE_TYPE( ResourceInfo )
        // TODO: Doesn't work yet, needs a fix in camp, should be fixed in camp 0.8.0.
        /*case BindingType_unknown:
        {
            // Get class name.
            String className;
            bool success = in.Read<String>( className );
            DivAssert( success, "Reading Value (class name) from bitstream failed" );

            // Construct object from given class name.
            try
            {
                const camp::Class& metaClass = camp::classByName( className );
                camp::UserObject object = metaClass.construct();

                // Deserialize properties.
                for ( std::size_t i = 0; i < metaClass.propertyCount(); ++i )
                {
                    camp::Value value;
                    in >> value;
                    metaClass.property( i ).set( object, value );
                }

                out = objct;
            }
            catch( camp::Error e )
            {
            	DivAssert( success, "Reading Value from bitstream failed" );
            }
            
            break;
        }*/
        default:
            DivAssert( 0, "Reading Value from bitstream failed" );
    }

    return in;
}
inline void valueToBitStream( RakNet::BitStream& rBitStream, const camp::Value& rValue )
{
    using namespace Diversia::Util;
    using namespace Diversia;

    switch( rValue.type() )
    {
        case camp::boolType:
            rBitStream.Write<BindingType>( BindingType_bool );
            rBitStream.Write<bool>( rValue.to<bool>() );
            break;
        case camp::intType:
            rBitStream.Write<BindingType>( BindingType_long );
            rBitStream.Write<long>( rValue.to<long>() );
            break;
        case camp::realType:
            rBitStream.Write<BindingType>( BindingType_double );
            rBitStream.Write<double>( rValue.to<double>() );
            break;
        case camp::stringType:
            rBitStream.Write<BindingType>( BindingType_String );
            rBitStream << rValue.to<String>();
            break;
        case camp::enumType:
            rBitStream.Write<BindingType>( BindingType_long );
            rBitStream.Write<long>( rValue.to<long>() );
            break;
        case camp::userType:
        {
            camp::UserObject object = rValue.to<camp::UserObject>();
            const camp::Class& metaClass = object.getClass();
            if( metaClass.hasTag( "BindingType" ) )
            {
                // This is an object that can be directly serialized to a bitstream.
                BindingType bindingType = metaClass.tag( "BindingType" );

                rBitStream.Write<BindingType>( bindingType );

                try
                {
                    switch( bindingType )
                    {
                        VALUE_TO_BITSTREAM_TYPE( Vector2 )
                        VALUE_TO_BITSTREAM_TYPE( Vector3 )
                        VALUE_TO_BITSTREAM_TYPE( Vector4 )
                        VALUE_TO_BITSTREAM_TYPE( Colour )
                        VALUE_TO_BITSTREAM_TYPE( Quaternion )
                        VALUE_TO_BITSTREAM_TYPE( Matrix3 )
                        VALUE_TO_BITSTREAM_TYPE( Matrix4 )
                        VALUE_TO_BITSTREAM_TYPE( Radian )
                        VALUE_TO_BITSTREAM_TYPE( Degree )
                        VALUE_TO_BITSTREAM_TYPE( Angle )
                        VALUE_TO_BITSTREAM_TYPE( ResourceInfo )
                        default:
                            DivAssert( 0, "Writing Value to bitstream failed" );
                            break;
                    }
                }
                catch ( camp::Error e )
                {
                    DivAssert( 0, "Writing Value to bitstream failed" );
                }
            }
            else
            {
                DivAssert( 0, "Writing Value to bitstream failed" );
                // TODO: Doesn't work yet, needs a fix in camp, should be fixed in camp 0.8.0.
                /*// The current property is a composed type: serialize it recursively. This will only
                // work for classes that have a default constructor.

                // Serialize class name. TODO: Use a string compressor/table for class name.
                rBitStream.Write<BindingType>( BindingType_unknown );
                rBitStream << metaClass.name();

                // Serialize properties.
                for ( std::size_t i = 0; i < metaClass.propertyCount(); ++i )
                {
                    const camp::Property& property = metaClass.property( i );
                    camp::Value value = property.get( object );
                    rBitStream << value;
                }*/
            }
            break;
        }
        case camp::noType:
            // Intentionally no break.
        default:
            DivAssert( 0, "Value is not serializable" );
    }
}
/// Write a Camp Value to a RakNet bitstream.
inline RakNet::BitStream& operator<<( RakNet::BitStream& out, camp::Value& in )
{
    valueToBitStream( out, in );
    return out;
}
/// Write a const Camp Value to a RakNet bitstream.
inline RakNet::BitStream& operator<<( RakNet::BitStream& out, const camp::Value& in )
{
    valueToBitStream( out, in );
    return out;
}

/// Read Camp Args from a RakNet bitstream. 
inline RakNet::BitStream& operator>>( RakNet::BitStream& in, camp::Args& out )
{
    unsigned int size;
    bool success = in.Read<unsigned int>( size );
    DivAssert( success, "Reading camp::Args (size) from bitstream failed" );

    for( unsigned int i = 0; i < size; ++i )
    {
        camp::Value value; in >> value;
        out += value;
    }

    return in;
}
/// Write Camp Args  to a RakNet bitstream.
inline RakNet::BitStream& operator<<( RakNet::BitStream& out, camp::Args& in )
{
    out.Write<unsigned int>( in.count() );

    for( std::size_t i = 0; i < in.count(); ++i )
    {
        out << in[i];
    }

    return out;
}
/// Write const Camp Args to a RakNet bitstream.
inline RakNet::BitStream& operator<<( RakNet::BitStream& out, const camp::Args& in )
{
    out.Write<unsigned int>( in.count() );

    for( std::size_t i = 0; i < in.count(); ++i )
    {
        out << in[i];
    }

    return out;
}

//------------------------------------------------------------------------------
} // Namespace RakNet

#endif // DIVERSIA_SHARED_BITSTREAM_H
