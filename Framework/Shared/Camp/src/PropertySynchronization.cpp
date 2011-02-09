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

#include "Shared/Platform/StableHeaders.h"

#include "Shared/Camp/PropertySynchronization.h"
#include "Shared/Camp/CampStringInterpreter.h"
#include "Shared/Camp/CampBitStream.h"
#include "Shared/Communication/BitStream.h"
#include "Util/Signal/UserObjectChange.h"

#include <RakNet/GetTime.h>

namespace Diversia
{
//------------------------------------------------------------------------------

const unsigned int PropertySynchronization::cBitStreamPropertySlot = 7;
const unsigned int PropertySynchronization::cBitStreamFunctionSlot = 6;

PropertySynchronization::PropertySynchronization( Mode mode, 
    RakNet::Time nextSerializeDelay /*= 0*/ ):
    mMode( mode ),
    mNextPropertySerialization( MAXUINT ),
    mNextPropertySerializationDelay( nextSerializeDelay ),
    mNextFunctionSerialization( MAXUINT ),
    mNextFunctionSerializationDelay( nextSerializeDelay ),
    mQueue( false ),
    mQueueConstruction( false ),
    mQueueConstructionProcess( false )
{

}

PropertySynchronization::~PropertySynchronization()
{
    mPropertyChangedConnection.disconnect();
    mValueInsertedConnection.disconnect();
}

void PropertySynchronization::set( const String& rQuery, const camp::Value& rValue )
{
    // Queue property change if queuing is on.
    if( mQueue && !mQueueIgnore.count( rQuery ) )
    {
        // Copy the value for user types.
        camp::Value value;
        if( rValue.type() == camp::userType )
            value = rValue.to<camp::UserObject>().copy();
        else
            value = rValue;

        // Queue property change
        mOutputQueueTransaction.addChangedProperty( rQuery, value );
    }
    else
    {
        CampStringInterpreter::set( mUserObject, rQuery, rValue );
    }
}

void PropertySynchronization::insert( const String& rQuery, const camp::Value& rValue )
{
    // Queue property insertion if queuing is on.
    if( mQueue && !mQueueIgnore.count( rQuery ) )
    {
        // Copy the value for user types.
        camp::Value value;
        if( rValue.type() == camp::userType )
            value = rValue.to<camp::UserObject>().copy();
        else
            value = rValue;

        mOutputQueueTransaction.addInsertedProperty( rQuery, value );
    }
    else
    {
        CampStringInterpreter::insert( mUserObject, rQuery, rValue );
    }
}

camp::Value PropertySynchronization::get( const String& rQuery )
{
    return CampStringInterpreter::get( mUserObject, rQuery );
}

void PropertySynchronization::call( const String& rQuery, 
    const camp::Args& rArgs /*= camp::Args()*/ )
{
    // Copy over function arguments.
    camp::Args args;
    for( std::size_t i = 0; i < rArgs.count(); ++i )
    {
        // Copy the value for user types.
        camp::Value value;
        if( rArgs[i].type() == camp::userType )
            value = rArgs[i].to<camp::UserObject>().copy();
        else
            value = rArgs[i];

        args += value;
    }

    // TODO: Check permission

    // Queue function call if queuing is on.
    if( mQueue && !mQueueIgnore.count( rQuery ) )
    {
        mOutputQueueFunctionCalls.insert( std::make_pair( rQuery, args ) );
    }
    else
    {
        // Call function
        // TODO: Support nested calls.
        const camp::Function& func = mUserObject.getClass().function( rQuery );
        if( !func.hasTag( "NoCall" ) )
            mUserObject.call( rQuery, args );

        mOutputFunctionCalls.insert( std::make_pair( rQuery, args ) );

        // Serialize changes
        if( mNextFunctionSerialization == MAXUINT )
        {
            mNextFunctionSerialization = RakNet::GetTime() + mNextFunctionSerializationDelay;
        }
    }
}

void PropertySynchronization::queue( 
    const std::set<String>& rIgnoredProperties /*= std::set<String>()*/ )
{
    if( !mQueue )
    {
        mQueue = true;
        mOutputQueueTransaction.reset();
        mInputQueueTransaction.reset();
        mOutputQueueFunctionCalls.clear();
        mInputQueueFunctionCalls.clear();
        mQueueIgnore = rIgnoredProperties;
    }
}

void PropertySynchronization::processQueue()
{
    if( mQueue )
    {
        mQueue = false;

        // Outgoing
        // Insert
        for( ValueMultimap::iterator i = mOutputQueueTransaction.getInsertedProperties().begin(); 
            i != mOutputQueueTransaction.getInsertedProperties().end(); ++i )
        {
            PropertySynchronization::insert( i->first, i->second );
        }
        // Change
        for( ValueMap::iterator i = mOutputQueueTransaction.getChangedProperties().begin(); 
            i != mOutputQueueTransaction.getChangedProperties().end(); ++i )
        {
            PropertySynchronization::set( i->first, i->second );
        }
        // Call
        for( FunctionCalls::iterator i = mOutputQueueFunctionCalls.begin(); 
            i != mOutputQueueFunctionCalls.end(); ++i )
        {
            PropertySynchronization::call( i->first, i->second );
        }

        // Incoming
        // Insert
        for( ValueMultimap::iterator i = mInputQueueTransaction.getInsertedProperties().begin(); 
            i != mInputQueueTransaction.getInsertedProperties().end(); ++i )
        {
            if( mMode == SERVER )
                PropertySynchronization::insert( i->first, i->second );
            else if( mMode == CLIENT )
            {
                PropertySynchronization::blockChangeConnections( true );
                CampStringInterpreter::insert( mUserObject, i->first, i->second );
                PropertySynchronization::blockChangeConnections( false );
            }
        }
        // Change
        for( ValueMap::iterator i = mInputQueueTransaction.getChangedProperties().begin(); 
            i != mInputQueueTransaction.getChangedProperties().end(); ++i )
        {
            if( mMode == SERVER )
                PropertySynchronization::set( i->first, i->second );
            else if( mMode == CLIENT )
            {
                PropertySynchronization::blockChangeConnections( true );
                CampStringInterpreter::set( mUserObject, i->first, i->second );
                PropertySynchronization::blockChangeConnections( false );
            }
        }
        // Call
        for( FunctionCalls::iterator i = mInputQueueFunctionCalls.begin(); 
            i != mInputQueueFunctionCalls.end(); ++i )
        {
            // TODO: Support nested calls.
            if( mMode == SERVER )
                PropertySynchronization::call( i->first, i->second );
            else if( mMode == CLIENT )
            {
                const camp::Function& func = mUserObject.getClass().function( i->first );
                if( !func.hasTag( "NoCall" ) )
                    mUserObject.call( i->first, i->second );
            }
        }

        mOutputQueueTransaction.reset();
        mInputQueueTransaction.reset();
        mOutputQueueFunctionCalls.clear();
        mInputQueueFunctionCalls.clear();
        mQueueIgnore.clear();
    }
}

void PropertySynchronization::queueConstruction( bool processQueue /*= false*/ )
{
    if( !mQueueConstruction )
    {
        mQueueConstruction = true;
        mQueueConstructionProcess = processQueue;
        mQueuedConstruction.Reset();
    }
}

void PropertySynchronization::processQueuedConstruction()
{
    if( mQueueConstruction )
    {
        PropertySynchronization::blockChangeConnections( true );

        mQueueConstruction = false;
        if( mQueuedConstruction.GetNumberOfBytesUsed() )
            CampBitStream::deserialize( mUserObject, mQueuedConstruction, "NoBitStream" );
        mQueuedConstruction.Reset();

        PropertySynchronization::blockChangeConnections( false );
    }
}

void PropertySynchronization::storeState( const camp::Value& tag, bool include )
{
    // Clear existing state.
    PropertySynchronization::clearStoredState();

    const camp::Class& metaclass = mUserObject.getClass();
    for( unsigned int i = 0; i < metaclass.propertyCount(); ++i )
    {
        const camp::Property& prop = metaclass.property( i );

        // If the property has the exclude tag, ignore it.
        if ( ( tag != camp::Value::nothing ) && include ? !prop.hasTag( tag ) : prop.hasTag( tag ) ) 
            continue;

        // Ignore array and dictionary types for now.
        if( prop.type() == camp::arrayType || prop.type() == camp::dictionaryType )
            continue;

        // Ignore unreadable
        if( !prop.readable( mUserObject ) )
            continue;

        // Copy the value for user types.
        camp::Value value = prop.get( mUserObject );
        if( value.type() == camp::userType ) value = value.to<camp::UserObject>().copy();

        mPropertyValueMap.insert( std::make_pair( prop.name(), value ) );
    }
}

void PropertySynchronization::loadStoredState() const
{
    const camp::Class& metaclass = mUserObject.getClass();
    for( PropertyValueMap::const_iterator i = mPropertyValueMap.begin(); 
        i != mPropertyValueMap.end(); ++i )
    {
        try
        {
            const camp::Property& prop = metaclass.property( i->first );
            prop.set( mUserObject, i->second );
        }
        catch( Exception e )
        {
        	SLOGD << "Could not load stored data for property " << i->first << ": " << e.what();
        }
        catch( camp::Error e )
        {
            SLOGD << "Could not load stored data for property " << i->first << ": " << e.what();
        }
    }
}

void PropertySynchronization::storeUserObject()
{
    mUserObject = camp::UserObject( this );

    mPropertyChangedConnection = UserObjectChange::connectChange( mUserObject, sigc::mem_fun( this, 
        &PropertySynchronization::propertyChanged ) );
    mValueInsertedConnection = UserObjectChange::connectInsertion( mUserObject, sigc::mem_fun( this, 
        &PropertySynchronization::valueInserted ) );
}

void PropertySynchronization::doSerializeConstruction( RakNet::BitStream* pConstructionBitstream, 
    RakNet::Connection_RM3* pDestinationConnection )
{
    CampBitStream::serialize( mUserObject, *pConstructionBitstream, "NoBitStream" );
}

void PropertySynchronization::doDeserializeConstruction( RakNet::BitStream* pConstructionBitstream, 
    RakNet::Connection_RM3* pSourceConnection )
{
    PropertySynchronization::blockChangeConnections( true );

    if( mQueueConstruction )
    {
        RakNet::BitSize_t offset = pConstructionBitstream->GetReadOffset();
        mQueuedConstruction.Write( pConstructionBitstream );

        if( mQueueConstructionProcess )
        {
            pConstructionBitstream->SetReadOffset( offset );
            CampBitStream::deserialize( mUserObject, *pConstructionBitstream, "NoBitStream" );
        }
    }
    else
        CampBitStream::deserialize( mUserObject, *pConstructionBitstream, "NoBitStream" );

    PropertySynchronization::blockChangeConnections( false );
}

RakNet::RM3SerializationResult PropertySynchronization::doSerialize( 
    RakNet::SerializeParameters* pSerializeParameters )
{
    bool serialize = false;

    if( RakNet::GetTime() >= mNextPropertySerialization )
    {
        pSerializeParameters->outputBitstream[cBitStreamPropertySlot] << mOutputPropertyTransaction;
        mOutputPropertyTransaction.reset();
        mNextPropertySerialization = MAXUINT;
        serialize = true;
    }
    if( RakNet::GetTime() >= mNextFunctionSerialization )
    {
        pSerializeParameters->outputBitstream[cBitStreamFunctionSlot] << mOutputFunctionCalls;
        mOutputFunctionCalls.clear();
        mNextFunctionSerialization = MAXUINT;
        serialize = true;
    } 

    if( serialize )
        return RakNet::RM3SR_SERIALIZED_ALWAYS_IDENTICALLY;	///< RakNet doesn't have to check changes.
    else
        return RakNet::RM3SR_DO_NOT_SERIALIZE;
}

void PropertySynchronization::doDeserialize( 
    RakNet::DeserializeParameters* pDeserializeParameters )
{
    RakNet::RakNetGUID sourceGUID = pDeserializeParameters->sourceConnection->GetRakNetGUID();

    // Properties
    if( pDeserializeParameters->bitstreamWrittenTo[cBitStreamPropertySlot] )
    {
        pDeserializeParameters->serializationBitstream[cBitStreamPropertySlot] >> 
            mInputPropertyTransaction;

        // TODO: Remove properties.
        // Insert properties
        for( ValueMultimap::iterator i = mInputPropertyTransaction.getInsertedProperties().begin(); 
            i != mInputPropertyTransaction.getInsertedProperties().end(); ++i )
        {
            try
            {
                // Queue property change if queuing is on.
                if( mQueue && !mQueueIgnore.count( i->first ) )
                {
                    mInputQueueTransaction.addInsertedProperty( i->first, i->second  );
                }
                else
                {
                    if( mMode == SERVER )
                        PropertySynchronization::insert( i->first, i->second );
                    else if( mMode == CLIENT )
                    {
                        PropertySynchronization::blockChangeConnections( true );
                        CampStringInterpreter::insert( mUserObject, i->first, i->second );
                        PropertySynchronization::blockChangeConnections( false );
                    }
                }
            }
            catch( Exception e )
            {
                SLOGD << "Error deserializing property insertion: " << e.what();
            }
        }

        // Change properties
        for( ValueMap::iterator i = mInputPropertyTransaction.getChangedProperties().begin(); 
            i != mInputPropertyTransaction.getChangedProperties().end(); ++i )
        {
            try
            {
                // Queue property change if queuing is on.
                if( mQueue && !mQueueIgnore.count( i->first ) )
                {
                    mInputQueueTransaction.addChangedProperty( i->first, i->second  );
                }
                else
                {
                    if( mMode == SERVER )
                        PropertySynchronization::set( i->first, i->second );
                    else if( mMode == CLIENT )
                    {
                        PropertySynchronization::blockChangeConnections( true );
                        CampStringInterpreter::set( mUserObject, i->first, i->second );
                        PropertySynchronization::blockChangeConnections( false );
                    }
                }
            }
            catch( Exception e )
            {
            	SLOGD << "Error deserializing property change: " << e.what();
            }
        }

        mInputPropertyTransaction.reset();
    }

    // Function calls
    if( pDeserializeParameters->bitstreamWrittenTo[cBitStreamFunctionSlot] )
    {
        pDeserializeParameters->serializationBitstream[cBitStreamFunctionSlot] >> 
            mInputFunctionCalls;

        for( FunctionCalls::iterator i = mInputFunctionCalls.begin(); 
            i != mInputFunctionCalls.end(); ++i )
        {
            try
            {
                // Queue property change if queuing is on.
                if( mQueue && !mQueueIgnore.count( i->first ) )
                {
                    mInputQueueFunctionCalls.insert( std::make_pair( i->first, i->second ) );
                }
                else 
                {
                    // TODO: Support nested calls.
                    if( mMode == SERVER )
                        PropertySynchronization::call( i->first, i->second );
                    else if( mMode == CLIENT )
                    {
                        const camp::Function& func = mUserObject.getClass().function( i->first );
                        if( !func.hasTag( "NoCall" ) )
                            mUserObject.call( i->first, i->second );
                    }
                }
            }
            catch( Exception e )
            {
                SLOGD << "Error deserializing remote function call: " << e.what();
            }
        }

        mInputFunctionCalls.clear();
    }
}

void PropertySynchronization::blockChangeConnections( bool block )
{
    mPropertyChangedConnection.block( block );
    mValueInsertedConnection.block( block );
}

void PropertySynchronization::propertyChanged( const camp::UserObject& rObject, 
    const camp::Property& rProperty, const camp::Value& rValue )
{
    if( rProperty.hasTag( "NoBitStream" ) ) return;

    // Copy the value for user types.
    camp::Value value;
    if( rValue.type() == camp::userType )
        value = rValue.to<camp::UserObject>().copy();
    else
        value = rValue;

    // Check permission
    querySetProperty( rProperty.name(), value );

    // Queue property change if queuing is on.
    if( mQueue && !mQueueIgnore.count( rProperty.name() ) )
    {
        mOutputQueueTransaction.addChangedProperty( rProperty.name(), value );
    }
    else
    {
        // Set property
        mOutputPropertyTransaction.addChangedProperty( rProperty.name(), value );

        // Serialize changes
        if( mNextPropertySerialization == MAXUINT )
        {
            mNextPropertySerialization = RakNet::GetTime() + mNextPropertySerializationDelay;
        }
    }
}

void PropertySynchronization::valueInserted( const camp::UserObject& rObject,
    const camp::ArrayProperty& rProperty, const camp::Value& rValue )
{
    if( rProperty.hasTag( "NoBitStream" ) ) return;

    // Copy the value for user types.
    camp::Value value;
    if( rValue.type() == camp::userType )
        value = rValue.to<camp::UserObject>().copy();
    else
        value = rValue;

    // Check permission
    queryInsertProperty( rProperty.name(), value );

    // Queue property insertion if queuing is on.
    if( mQueue && !mQueueIgnore.count( rProperty.name() ) )
    {
        mOutputQueueTransaction.addInsertedProperty( rProperty.name(), value );
    }
    else
    {
        // Insert value into property.
        mOutputPropertyTransaction.addInsertedProperty( rProperty.name(), value );

        // Serialize changes
        if( mNextPropertySerialization == MAXUINT )
        {
            mNextPropertySerialization = RakNet::GetTime() + mNextPropertySerializationDelay;
        }
    }
}

//------------------------------------------------------------------------------
} // Namespace Diversia