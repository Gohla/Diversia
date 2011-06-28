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

#ifndef DIVERSIA_SHARED_STRINGINTERPRETER_H
#define DIVERSIA_SHARED_STRINGINTERPRETER_H

#include "Shared/Platform/Prerequisites.h"

namespace Diversia
{
//------------------------------------------------------------------------------

/**
Simple string interpreter to get camp properties from an user object.
**/
class DIVERSIA_SHARED_API CampStringInterpreter
{
public:
    /**
    Gets the value of the given property name and user object.
    
    @param  rUserObject The user object.
    @param  rQuery      The property to get data from.

    @throws Exception               When there's a syntax error in rQuery.
    @throws camp::InvalidProperty   When property doesn't exist.
    **/
    static camp::Value get( const camp::UserObject& rUserObject, const String& rQuery );
    /**
    Sets the value of the given property name and user object.

    @param  rUserObject The user object.
    @param  rQuery      The property to set the value to.
    @param  rValue      The value to set.

    @throws Exception               When there's a syntax error in rQuery.
    @throws camp::InvalidProperty   When property doesn't exist.
    **/
    static void set( const camp::UserObject& rUserObject, const String& rQuery, 
        const camp::Value& rValue );
    /**
    Inserts the value into the given array name and user object.

    @param  rUserObject The user object.
    @param  rQuery      The array to insert the value into.
    @param  rValue      The value to insert.

    @throws Exception               When there's a syntax error in rQuery.
    @throws Exception               When the property isn't an array.
    @throws camp::InvalidProperty   When property doesn't exist.
    **/
    static void insert( const camp::UserObject& rUserObject, const String& rQuery, 
        const camp::Value& rValue );
    /**
    Removes the array identifiers from given query.
    
    @param  rQuery  The query. 
    
    @return The query with array identifiers removed.
    **/
    static String removeArrayIdentifiers( const String& rQuery );

private:
    static boost::tuple<const camp::Property*, String, camp::UserObject> getPropertyFromString( 
        const camp::UserObject& rUserObject, const String& rQuery, bool wantArrayProperty = false );
};

//------------------------------------------------------------------------------
} // Namespace Diversia

#endif // DIVERSIA_SHARED_STRINGINTERPRETER_H