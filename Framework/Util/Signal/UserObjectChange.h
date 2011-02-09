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

#ifndef DIVERSIA_UTIL_USEROBJECTCHANGE_H
#define DIVERSIA_UTIL_USEROBJECTCHANGE_H

#include "Util/Platform/Prerequisites.h"

namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------

typedef sigc::signal<void, const camp::UserObject&, const camp::Property&, const camp::Value&>
    PropertySetSignal;
typedef sigc::signal<void, const camp::UserObject&, const camp::ArrayProperty&, const camp::Value&>
    ValueInsertedSignal;
typedef std::map<camp::UserObject, PropertySetSignal> SetSignalMap;
typedef std::map<camp::UserObject, ValueInsertedSignal> InsertSignalMap;

/**
Provides a signal to get notified of all property changes of a specific camp UserObject.
**/
class DIVERSIA_UTIL_API UserObjectChange
{
public:
    /**
    Connects to the camp property changed signal.

    @param          rObject The UserObject to get changes for.
    @param [in,out] rSlot   The slot (signature: void func(const camp::Property&,
                            const camp::Value&)) to connectChange.

    @return Connection object to block or disconnect the connection.
    **/
    static inline sigc::connection connectChange( const camp::UserObject& rObject,
        const PropertySetSignal::slot_type& rSlot )
    {
        UserObjectChange::connectToClass( rObject.getClass() );
        return mSetSignalMap[ rObject ].connect( rSlot );
    }
    /**
    Connects to the camp value inserted signal.

    @param          rObject The UserObject to get changes for.
    @param [in,out] rSlot   The slot (signature: void func(const camp::Property&,
                            const camp::Value&)) to connectChange.

    @return Connection object to block or disconnect the connection.
    **/
    static inline sigc::connection connectInsertion( const camp::UserObject& rObject,
        const ValueInsertedSignal::slot_type& rSlot )
    {
        UserObjectChange::connectToClass( rObject.getClass() );
        return mInsertSignalMap[ rObject ].connect( rSlot );
    }
    /**
    Connects to the property changed signal for all properties inside given class.

    @param  rClass  The metaclass to get property changed notifications from.
    **/
    static inline void connectToClass( const camp::Class& rClass )
    {
        if( mClassesAdded.count( rClass.name() ) ) return;

        for( std::size_t i = 0; i < rClass.propertyCount(); ++i )
        {
            const camp::Property& prop = rClass.property( i );
            if( prop.type() != camp::arrayType )
            {
                prop.connectSetted( &UserObjectChange::propertyChanged );
                prop.connectSettedNonwritable( &UserObjectChange::propertyChanged );
            }
            else
            {
                const camp::ArrayProperty& array = static_cast<const camp::ArrayProperty&>( prop );
                array.connectInserted( &UserObjectChange::valueInserted );
                array.connectInsertedNonwritable( &UserObjectChange::valueInserted );
            }
        }

        mClassesAdded.insert( rClass.name() );
    }

private:
    static inline void propertyChanged( const camp::UserObject& rObject,
        const camp::Property& rProperty, const camp::Value& rValue )
    {
        mSetSignalMap[ rObject ]( rObject, rProperty, rValue );
    }

    static inline void valueInserted( const camp::UserObject& rObject,
        const camp::ArrayProperty& rProperty, const camp::Value& rValue )
    {
        mInsertSignalMap[ rObject ]( rObject, rProperty, rValue );
    }

    static SetSignalMap     mSetSignalMap;
    static InsertSignalMap  mInsertSignalMap;
    static std::set<String> mClassesAdded;
};

//------------------------------------------------------------------------------
} // Namespace Util
} // Namespace Diversia

#endif // DIVERSIA_UTIL_USEROBJECTCHANGE_H