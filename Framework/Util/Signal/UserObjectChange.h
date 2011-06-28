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

namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------

// Normal properties
typedef sigc::signal<void, const camp::UserObject&, const camp::Property&, const camp::Value&, 
    const int> PropertyChangeSignal;
// Array properties
typedef sigc::signal<void, const camp::UserObject&, const camp::ArrayProperty&, const camp::Value&>
    ArrayInsertedSignal;
typedef sigc::signal<void, const camp::UserObject&, const camp::ArrayProperty&, std::size_t, 
    const camp::Value&> ArrayChangedSignal;
typedef sigc::signal<void, const camp::UserObject&, const camp::ArrayProperty&, std::size_t>
    ArrayRemovedSignal;
// Dictionary properties
typedef sigc::signal<void, const camp::UserObject&, const camp::DictionaryProperty&, 
    const camp::Value&, const camp::Value&> DictChangedSignal;
typedef sigc::signal<void, const camp::UserObject&, const camp::DictionaryProperty&, 
    const camp::Value&> DictRemovedSignal;

typedef std::map<camp::UserObject, PropertyChangeSignal> ChangeSignalMap;
typedef std::map<camp::UserObject, ArrayInsertedSignal> ArrayInsertedSignalMap;
typedef std::map<camp::UserObject, ArrayChangedSignal> ArrayChangedSignalMap;
typedef std::map<camp::UserObject, ArrayRemovedSignal> ArrayRemovedSignalMap;
typedef std::map<camp::UserObject, DictChangedSignal> DictChangedSignalMap;
typedef std::map<camp::UserObject, DictRemovedSignal> DictRemovedSignalMap;

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
                            const camp::Value&)) to connect to.

    @return Connection object to block or disconnect the connection.
    **/
    static inline sigc::connection connectChange( const camp::UserObject& rObject,
        const PropertyChangeSignal::slot_type& rSlot )
    {
        UserObjectChange::connectToClass( rObject.getClass() );
        return mChangeMap[ rObject ].connect( rSlot );
    }
    /**
    Connects to the camp array inserted signal.

    @param          rObject The UserObject to get changes for.
    @param [in,out] rSlot   The slot (signature: void func(const camp::ArrayProperty&,
                            const camp::Value&)) to connect to.

    @return Connection object to block or disconnect the connection.
    **/
    static inline sigc::connection connectArrayInsertion( const camp::UserObject& rObject,
        const ArrayInsertedSignal::slot_type& rSlot )
    {
        UserObjectChange::connectToClass( rObject.getClass() );
        return mArrayInsertedMap[ rObject ].connect( rSlot );
    }
    /**
    Connects to the camp array changed signal.

    @param          rObject The UserObject to get changes for.
    @param [in,out] rSlot   The slot (signature: void func(const camp::ArrayProperty&,
                            std::size_t [index], const camp::Value&)) to connect to.

    @return Connection object to block or disconnect the connection.
    **/
    static inline sigc::connection connectArrayChange( const camp::UserObject& rObject,
        const ArrayChangedSignal::slot_type& rSlot )
    {
        UserObjectChange::connectToClass( rObject.getClass() );
        return mArrayChangedMap[ rObject ].connect( rSlot );
    }
    /**
    Connects to the camp array removed signal.

    @param          rObject The UserObject to get changes for.
    @param [in,out] rSlot   The slot (signature: void func(const camp::ArrayProperty&,
                            std::size_t [index])) to connect to.

    @return Connection object to block or disconnect the connection.
    **/
    static inline sigc::connection connectArrayRemoval( const camp::UserObject& rObject,
        const ArrayRemovedSignal::slot_type& rSlot )
    {
        UserObjectChange::connectToClass( rObject.getClass() );
        return mArrayRemovedMap[ rObject ].connect( rSlot );
    }
    /**
    Connects to the camp dictionary changed signal.

    @param          rObject The UserObject to get changes for.
    @param [in,out] rSlot   The slot (signature: void func(const camp::DictionaryProperty&,
                            const camp::Value& [key], const camp::Value& [value])) to connect to.

    @return Connection object to block or disconnect the connection.
    **/
    static inline sigc::connection connectDictChange( const camp::UserObject& rObject,
        const DictChangedSignal::slot_type& rSlot )
    {
        UserObjectChange::connectToClass( rObject.getClass() );
        return mDictChangedMap[ rObject ].connect( rSlot );
    }
    /**
    Connects to the camp dictionary removed signal.

    @param          rObject The UserObject to get changes for.
    @param [in,out] rSlot   The slot (signature: void func(const camp::DictionaryProperty&,
                            const camp::Value& [key])) to connect to.

    @return Connection object to block or disconnect the connection.
    **/
    static inline sigc::connection connectDictRemoval( const camp::UserObject& rObject,
        const DictRemovedSignal::slot_type& rSlot )
    {
        UserObjectChange::connectToClass( rObject.getClass() );
        return mDictRemovedMap[ rObject ].connect( rSlot );
    }
    /**
    Connects to the change signals for all properties inside given class.

    @param  rClass  The metaclass to get property changed notifications from.
    **/
    static inline void connectToClass( const camp::Class& rClass )
    {
        if( mClassesAdded.count( rClass.name() ) ) return;

        for( std::size_t i = 0; i < rClass.propertyCount(); ++i )
        {
            const camp::Property& prop = rClass.property( i );
            switch( prop.type() )
            {
                case camp::arrayType:
                {
                    const camp::ArrayProperty& array = static_cast<const camp::ArrayProperty&>( prop );
                    array.connectInserted( &UserObjectChange::arrayInserted );
                    array.connectInsertedNonwritable( &UserObjectChange::arrayInserted );
                    array.connectSetted( &UserObjectChange::arrayChanged );
                    array.connectSettedNonwritable( &UserObjectChange::arrayChanged );
                    array.connectRemoved( &UserObjectChange::arrayRemoved );
                    array.connectRemovedNonwritable( &UserObjectChange::arrayRemoved );
                    break;
                }
                case camp::dictionaryType:
                {
                    const camp::DictionaryProperty& dict = static_cast<const camp::DictionaryProperty&>( prop );
                    dict.connectSetted( &UserObjectChange::dictChanged );
                    dict.connectSettedNonwritable( &UserObjectChange::dictChanged );
                    dict.connectRemoved( &UserObjectChange::dictRemoved );
                    dict.connectRemovedNonwritable( &UserObjectChange::dictRemoved );
                    break;
                }
                default:
                    prop.connectSetted( &UserObjectChange::propertyChanged );
                    prop.connectSettedNonwritable( &UserObjectChange::propertyChanged );
                    break;
            }
        }

        mClassesAdded.insert( rClass.name() );
    }

private:
    static inline void propertyChanged( const camp::UserObject& rObject,
        const camp::Property& rProperty, const camp::Value& rValue, const int reason )
    {
        mChangeMap[ rObject ]( rObject, rProperty, rValue, reason );
    }

    static inline void arrayInserted( const camp::UserObject& rObject,
        const camp::ArrayProperty& rProperty, const camp::Value& rValue )
    {
        mArrayInsertedMap[ rObject ]( rObject, rProperty, rValue );
    }
    static inline void arrayChanged( const camp::UserObject& rObject,
        const camp::ArrayProperty& rProperty, std::size_t index, const camp::Value& rValue )
    {
        mArrayChangedMap[ rObject ]( rObject, rProperty, index, rValue );
    }
    static inline void arrayRemoved( const camp::UserObject& rObject,
        const camp::ArrayProperty& rProperty, std::size_t index )
    {
        mArrayRemovedMap[ rObject ]( rObject, rProperty, index );
    }

    static inline void dictChanged( const camp::UserObject& rObject,
        const camp::DictionaryProperty& rProperty, const camp::Value& rKey, const camp::Value& rValue )
    {
        mDictChangedMap[ rObject ]( rObject, rProperty, rKey, rValue );
    }
    static inline void dictRemoved( const camp::UserObject& rObject,
        const camp::DictionaryProperty& rProperty, const camp::Value& rKey )
    {
        mDictRemovedMap[ rObject ]( rObject, rProperty, rKey );
    }

    static ChangeSignalMap          mChangeMap;
    static ArrayInsertedSignalMap   mArrayInsertedMap;
    static ArrayChangedSignalMap    mArrayChangedMap;
    static ArrayRemovedSignalMap    mArrayRemovedMap;
    static DictChangedSignalMap     mDictChangedMap;
    static DictRemovedSignalMap     mDictRemovedMap;
    static std::set<String>         mClassesAdded;
};

//------------------------------------------------------------------------------
} // Namespace Util
} // Namespace Diversia

#endif // DIVERSIA_UTIL_USEROBJECTCHANGE_H