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

#ifndef DIVERSIA_SHARED_PROPERTYSYNCHRONIZATION_H
#define DIVERSIA_SHARED_PROPERTYSYNCHRONIZATION_H

#include "Shared/Platform/Prerequisites.h"

#include "Shared/Camp/PropertyTransaction.h"

namespace Diversia
{
//------------------------------------------------------------------------------

typedef std::multimap<String, camp::Args> FunctionCalls;
typedef std::map<String, camp::Value> PropertyValueMap;

class DIVERSIA_SHARED_API PropertySynchronization
{
public:
    /**
    Constructor.

    @param  mode                The mode (Client/Server) to run in.
    @param  userObject          The user object to manage property synchronization for.
    @param  nextSerializeDelay  A serialization delay in milliseconds after a property is changed
                                or a value is inserted into a property. Defaults to 0.
    **/
    PropertySynchronization( Mode mode, RakNet::Time nextSerializeDelay = 0 );
    /**
    Destructor.
    **/
    virtual ~PropertySynchronization();

    /**
    Sets a property using camp.

    @param  rQuery  The name of the property.
    @param  rValue  The value to set.
    **/
    void set( const String& rQuery, const camp::Value& rValue );
    /**
    Inserts a value into an array using camp.

    @param  rName   The name of the array property.
    @param  rValue  The value to insert.
    **/
    void insert( const String& rQuery, const camp::Value& rValue );
    /**
    Gets a property using camp.

    @param  rQuery  The name of the property.
    **/
    camp::Value get( const String& rQuery );
    /**
    Calls a function using camp.

    @param  rQuery  The name of the function
    @param  rArgs   The function arguments.
    **/
    void call( const String& rQuery, const camp::Args& rArgs = camp::Args() );
    /**
    Makes properties serialize as soon as possible (when the next RakNet serialize tick occurs).
    **/
    inline void forceSerializeProperties() { mNextPropertySerialization = 0; }
    /**
    Makes function calls serialize as soon as possible (when the next RakNet serialize tick occurs).
    **/
    inline void forceSerializeFunctionCalls() { mNextFunctionSerialization = 0; }

    /**
    Turns on queuing, queuing all incoming property changes and insertions except for the
    properties that are ignored. Use processQueue() to stop queuing and process all queued property
    changes and insertions.

    @param  rIgnoredProperties  The properties to ignore (not queue).
    **/
    void queue( const std::set<String>& rIgnoredProperties = std::set<String>() );
    /**
    Process the property changed and insertion queue and stops queuing.
    **/
    void processQueue();
    /**
    Query if properties are being queued.
    **/
    inline bool isQueuing() const { return mQueue; }
    /**
    Turns on queuing the construction for this plugin, delaying the construction properties from
    being set. Use processQueuedConstruction() to process the construction properties.

    @param  processQueue    Set to true to process properties at construction anyway, the
                            construction bitstream will still be saved for use with
                            processQueuedConstruction().
    **/
    void queueConstruction( bool processQueue = false );
    /**
    Process the queued construction.
    **/
    void processQueuedConstruction();
    /**
    Query if construction properties are being queued.
    **/
    inline bool isQueingConstruction() const { return mQueueConstruction; }
    /**
    Stores the state of all properties. Previous state will be cleared.
    
    @param tag      Tag to include or exclude properties from the storage process.
    @param include  Set this to true to make the tag an including tag, false for excluding tag.
    **/
    inline void storeState( const camp::Value& tag, bool include = false ) { storeState( camp::Args( tag ), include ); }
    /**
    Stores the state of all properties. Previous state will be cleared.
    
    @param tags     Tags to include or exclude properties from the storage process.
    @param include  Set this to true to make the tag an including tag, false for excluding tag.
    **/
    void storeState( const camp::Args& tags, bool include = false );
    /**
    Loads the stored state. 
    **/
    void loadStoredState() const;
    /**
    Clears the stored state. 
    **/
    inline void clearStoredState() { mPropertyValueMap.clear(); }
    /**
    Queries if there is a stored state.
    **/
    inline bool hasStoredState() const { return mPropertyValueMap.size() != 0; }

    /**
    Serializes a function call.

    @param  rQuery  The name of the function to call.
    **/
    void serializeCall( const String& rQuery )
    {
        mOutputFunctionCalls.insert( std::make_pair( rQuery, camp::Args() ) );
        PropertySynchronization::forceSerializeFunctionCalls();
    }

    // Generate serializeCall for 1-20 parameters.
#define serializeCallBody(z, n, unused) \
    camp::Value value ## n = t ## n; \
    if( value ## n.type() == camp::userType ) \
        value ## n = value ## n.to<camp::UserObject>().copy(); \
    args += value ## n;

#define serializeCallSkeleton(z, n, unused) \
    template <BOOST_PP_ENUM_PARAMS(n, class T)> \
    void serializeCall( const String& rQuery, BOOST_PP_ENUM_BINARY_PARAMS(n, T, t) ) \
    { \
        camp::Args args; \
        BOOST_PP_REPEAT(n, serializeCallBody, ~); \
        mOutputFunctionCalls.insert( std::make_pair( rQuery, args ) ); \
        PropertySynchronization::forceSerializeFunctionCalls(); \
    }

BOOST_PP_REPEAT_FROM_TO(1, 20, serializeCallSkeleton, ~)

#undef serializeCallBody
#undef serializeCallSkeleton

protected:
    /**
    Stores the user object. This must be called by the base class before using any function in this
    class!
    **/
    void storeUserObject();
    /**
    Queries if a property may be changed.

    @param  rQuery  The name of the property.
    @param  rValue  The value to set.

    @note   Override this in a parent class to stop a property change. Throw an exception if
            the property may not be changed, the exception will be thrown back to the caller of
            set() and the property will not be changed.
    @note   The value may be adjusted to fit within permission bounds.
    **/
    inline virtual void querySetProperty( const String& rQuery, camp::Value& rValue ) {}
    /**
    Same as querySetProperty but occurs at deserialization.

    @see PropertySynchronization::querySetProperty
    **/
    inline virtual void querySetPropertyDeserialize( const String& rQuery, camp::Value& rValue,
        RakNet::RakNetGUID source ) {}
    /**
    Queries if a property may be inserted.

    @param  rQuery  The name of the array property.
    @param  rValue  The value to insert.

    @note   Override this in a parent class to stop a value insertion. Throw an exception if
            the value may not be inserted, the exception will be thrown back to the caller of
            insert() and the value will not be inserted.
    @note   The value may be adjusted to fit within permission bounds.
    **/
    inline virtual void queryInsertProperty( const String& rQuery, camp::Value& rValue ) {}
    /**
    Same as queryInsertProperty but occurs at deserialization.

    @see PropertySynchronization::queryInsertProperty
    **/
    inline virtual void queryInsertPropertyDeserialize( const String& rQuery, camp::Value& rValue,
        RakNet::RakNetGUID source ) {}
    /**
    Queries if a function may be called.

    @param  rQuery  The name of the function to call.
    @param  rArgs   The function arguments.

    @note   Override this in a parent class to stop a function call. Throw an exception if
            the function may not be called, the exception will be thrown back to the caller of
            call() and the function will not be called
    @note   The arguments may be adjusted to fit within permission bounds.
    **/
    inline virtual void queryCallFunction( const String& rQuery, camp::Args& rArgs ) {}
    /**
    Same as queryCallFunction but occurs at deserialization.

    @see PropertySynchronization::queryCallFunction
    **/
    inline virtual void queryCallFunctionDeserialize( const String& rQuery, camp::Args& rArgs,
        RakNet::RakNetGUID source ) {}

    void doSerializeConstruction( RakNet::BitStream* pConstructionBitstream,
        RakNet::Connection_RM3* pDestinationConnection );
    void doDeserializeConstruction( RakNet::BitStream* pConstructionBitstream,
        RakNet::Connection_RM3* pSourceConnection );
    RakNet::RM3SerializationResult doSerialize( RakNet::SerializeParameters* pSerializeParameters );
    void doDeserialize( RakNet::DeserializeParameters* pDeserializeParameters );

private:
    void blockChangeConnections( bool block );
    void propertyChanged( const camp::UserObject& rObject, const camp::Property& rProperty,
        const camp::Value& rValue );
    void valueInserted( const camp::UserObject& rObject, const camp::ArrayProperty& rProperty,
        const camp::Value& rValue );

    camp::UserObject        mUserObject;
    Mode                    mMode;

    RakNet::Time            mNextPropertySerialization;
    RakNet::Time            mNextPropertySerializationDelay;
    PropertyTransaction     mOutputPropertyTransaction;
    PropertyTransaction     mInputPropertyTransaction;

    RakNet::Time            mNextFunctionSerialization;
    RakNet::Time            mNextFunctionSerializationDelay;
    // TODO: Use vector so functions get called in the correct order.
    FunctionCalls           mOutputFunctionCalls;
    FunctionCalls           mInputFunctionCalls;

    bool                    mQueue;
    PropertyTransaction     mOutputQueueTransaction;
    PropertyTransaction     mInputQueueTransaction;
    FunctionCalls           mOutputQueueFunctionCalls;
    FunctionCalls           mInputQueueFunctionCalls;
    std::set<String>        mQueueIgnore;

    bool                    mQueueConstruction;
    bool                    mQueueConstructionProcess;
    RakNet::BitStream       mQueuedConstruction;

    PropertyValueMap        mPropertyValueMap;

    sigc::connection        mPropertyChangedConnection;
    sigc::connection        mValueInsertedConnection;

    static const unsigned int cBitStreamPropertySlot;
    static const unsigned int cBitStreamFunctionSlot;

    CAMP_RTTI()
};

//------------------------------------------------------------------------------
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::PropertySynchronization,
    &Diversia::Shared::Bindings::CampBindings::bindPropertySynchronization );

#endif // DIVERSIA_SHARED_PROPERTYSYNCHRONIZATION_H