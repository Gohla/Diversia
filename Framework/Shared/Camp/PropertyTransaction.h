/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

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

#ifndef DIVERSIA_SHARED_PROPERTYTRANSACTION_H
#define DIVERSIA_SHARED_PROPERTYTRANSACTION_H

#include "Shared/Platform/Prerequisites.h"

#include "Shared/Communication/BitStream.h"

namespace Diversia
{
//------------------------------------------------------------------------------

typedef std::map<String, camp::Value> ValueMap;
typedef std::multimap<String, camp::Value> ValueMultimap;
typedef std::set<camp::Value> ValueSet;

class DIVERSIA_SHARED_API PropertyTransaction
{
public:
    /**
    Adds a changed property.
    
    @param  rName   The name of the property.
    @param  rValue  The new value. 
    **/
    void addChangedProperty( const String& rName, const camp::Value& rValue );
    /**
    Adds a property that was inserted into an array/map.
    
    @param  rName   The name of the property. 
    @param  rValue  The inserted value. 
    **/
    void addInsertedProperty( const String& rName, const camp::Value& rValue );
    /**
    Adds a property that was removed from an array/map.
    
    @param  rName   The name of the property.
    **/
    void addRemovedProperty( const String& rName );
    /**
    Applies the changed, inserted and removed properties on a UserObject.
    
    @param [in,out] rObject The object to apply the changes to.
    **/
    void apply( camp::UserObject& rObject );
    /**
    Removes all property changes.
    **/
    void reset();

    /**
    Gets the changed properties. 
    **/
    inline ValueMap& getChangedProperties() { return mChangedProperties; }
    /**
    Gets the inserted properties. 
    **/
    inline ValueMultimap& getInsertedProperties() { return mInsertedProperties; }
    /**
    Gets the removed properties. 
    **/
    inline ValueSet& getRemovedProperties() { return mRemovedProperties; }

    friend inline RakNet::BitStream& operator<<( RakNet::BitStream& out, PropertyTransaction& in )
    {
        out << in.mChangedProperties;
        out << in.mInsertedProperties;
        out << in.mRemovedProperties;

        return out;
    }
    friend inline RakNet::BitStream& operator>>( RakNet::BitStream& in, PropertyTransaction& out )
    {
        in >> out.mChangedProperties;
        in >> out.mInsertedProperties;
        in >> out.mRemovedProperties;

        return in;
    }

private:
    ValueMap        mChangedProperties;     
    ValueMultimap   mInsertedProperties;	///< For inserting into arrays/maps. 
    ValueSet        mRemovedProperties;	    ///< For removing from arrays/maps.

};

//------------------------------------------------------------------------------
} // Namespace Diversia

#endif