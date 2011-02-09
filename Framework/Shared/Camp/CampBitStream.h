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

#ifndef DIVERSIA_SHARED_CAMPBITSTREAM_H
#define DIVERSIA_SHARED_CAMPBITSTREAM_H

#include "Shared/Platform/Prerequisites.h"

#include "Shared/Communication/BitStream.h"

namespace Diversia
{
//------------------------------------------------------------------------------

class DIVERSIA_SHARED_API CampBitStream
{
public:
    static void serialize( const camp::UserObject& rObject, RakNet::BitStream& rBitStream, 
        const camp::Value& rExcludeTag = camp::Value::nothing )
    {
        using namespace camp;

        // Iterate over the object's properties using its metaclass.
        const Class& metaclass = rObject.getClass();
        for( std::size_t i = 0; i < metaclass.propertyCount(); ++i )
        {
            const Property& property = metaclass.property( i );

            // If the property has the exclude tag, ignore it.
            if( ( rExcludeTag != Value::nothing ) && property.hasTag( rExcludeTag ) )
                continue;
            
            // Check if property is readable
            if( property.readable( rObject ) )
            {
                rBitStream.Write1();
            }
            else
            {
                rBitStream.Write0();
                continue;
            }

            if( property.type() == userType )
            {
                camp::Value val = property.get( rObject );
                camp::UserObject object = val.to<UserObject>();
                if( object.getClass().hasTag( "BindingType" ) )
                {
                    // Property can be directly serialized to the bitstream.
                    rBitStream << val;
                }
                else
                {
                    // The current property is a composed type: serialize it recursively.
                    CampBitStream::serialize( object, rBitStream, rExcludeTag );
                }
            }
            else if( property.type() == arrayType )
            {
                // The current property is an array.
                const ArrayProperty& arrayProperty = static_cast<const ArrayProperty&>( property );

                // Send array size.
                std::size_t count = arrayProperty.size( rObject );
                rBitStream.Write<std::size_t>( count );

                // Iterate over the array elements.
                for ( std::size_t j = 0; j < count; ++j )
                {
                    camp::Value val = arrayProperty.get( rObject, j );
                    if ( arrayProperty.elementType() == userType )
                    {
                        camp::UserObject object = val.to<UserObject>();
                        if( object.getClass().hasTag( "BindingType" ) )
                        {
                            // Property can be directly serialized to the bitstream.
                            rBitStream << val;
                        }
                        else
                        {
                            // The current property is a composed type: serialize it recursively.
                            CampBitStream::serialize( object, rBitStream, rExcludeTag );
                        }
                    }
                    else
                    {
                        // The array elements are simple properties: serialize value to bitstream.
                        rBitStream << val;
                    }
                }
            }
            else
            {
                // The current property is a simple property: serialize value to bitstream.
                rBitStream << property.get( rObject );
            }
        }
    }
    static void deserialize( const camp::UserObject& rObject, RakNet::BitStream& rBitStream, 
        const camp::Value& rExcludeTag = camp::Value::nothing )
    {
        using namespace camp;

        // Iterate over the object's properties using its metaclass.
        const Class& metaclass = rObject.getClass();
        for( std::size_t i = 0; i < metaclass.propertyCount(); ++i )
        {
            const Property& property = metaclass.property( i );

            // If the property has the exclude tag, ignore it.
            if( ( rExcludeTag != Value::nothing ) && property.hasTag( rExcludeTag ) )
                continue;

            // Check if the property was sent (readable at serialization)
            if( !rBitStream.ReadBit() )
                continue;

            // Check if the property is readable and writable, can't stop here because the 
            // bitstream pointer needs to be advanced.
            bool writable = property.writable( rObject );
            bool readable = property.readable( rObject );

            if( property.type() == userType )
            {
                // TODO: What happens if property is not readable and it's a composed type?
                if( !readable )
                {
                    Value value; rBitStream >> value;
                    continue;
                }

                camp::UserObject object = property.get( rObject ).to<UserObject>();
                if( object.getClass().hasTag( "BindingType" ) )
                {
                    // Property can be directly deserialized from the bitstream.
                    Value value; rBitStream >> value;
                    if( writable ) property.set( rObject, value );
                }
                else
                {
                    // The current property is a composed type: deserialize it recursively.
                    // TODO: What happens if property is not writable?
                    CampBitStream::deserialize( object, rBitStream, rExcludeTag );
                }
            }
            else if( property.type() == arrayType )
            {
                // The current property is an array.
                const ArrayProperty& arrayProperty = static_cast<const ArrayProperty&>( property );

                // Read array size.
                std::size_t size; rBitStream.Read( size );

                // Resize array if needed.
                if( size > arrayProperty.size( rObject ) )
                {
                    if ( arrayProperty.dynamic() )
                        arrayProperty.resize( rObject, size );
                    else
                        break;
                }

                for( std::size_t j = 0; j < size; ++j )
                {
                    if ( arrayProperty.elementType() == userType )
                    {
                        // TODO: What happens if property is not readable and it's a composed type?
                        if( !readable )
                        {
                            Value value; rBitStream >> value;
                            continue;
                        }

                        camp::UserObject object = arrayProperty.get( rObject, j ).to<UserObject>();
                        if( object.getClass().hasTag( "BindingType" ) )
                        {
                            // Property can be directly deserialized from the bitstream.
                            Value value; rBitStream >> value;
                            if( writable ) arrayProperty.set( rObject, j, value );
                        }
                        else
                        {
                            // The array elements are composed objects: deserialize them recursively.
                            // TODO: What happens if property is not writable?
                            CampBitStream::deserialize( object, rBitStream, rExcludeTag );
                        }
                    }
                    else
                    {
                        // The array elements are simple properties: deserialize value from bitstream.
                        Value value; rBitStream >> value;
                        if( writable ) arrayProperty.set( rObject, j, value );
                    }
                }
            }
            else
            {
                // The current property is a simple property: deserialize value from bitstream.
                Value value; rBitStream >> value;
                if( writable ) property.set( rObject, value );
            }
        }
    }
};

//------------------------------------------------------------------------------
} // Namespace Diversia

#endif // DIVERSIA_SHARED_CAMPBITSTREAM_H