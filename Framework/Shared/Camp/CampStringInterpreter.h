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
    static camp::Value get( const camp::UserObject& rUserObject, const String& rQuery )
    {
        boost::tuple<const camp::Property*, String, camp::UserObject> params = 
           getPropertyFromString( rUserObject, rQuery );

        if( params.get<1>().empty() )
        {
            // Not an array property.
            return params.get<0>()->get( params.get<2>() );
        }

        // An array property.
        const camp::ArrayProperty* arrayProperty = 
            static_cast<const camp::ArrayProperty*>( params.get<0>() );
        return arrayProperty->get( params.get<2>(), boost::lexical_cast<size_t>( 
            params.get<1>() ) );
    }
    /**
    Sets the value of the given property name and user object.

    @param  rUserObject The user object.
    @param  rQuery      The property to set the value to.
    @param  rValue      The value to set.

    @throws Exception               When there's a syntax error in rQuery.
    @throws camp::InvalidProperty   When property doesn't exist.
    **/
    static void set( const camp::UserObject& rUserObject, const String& rQuery, 
        const camp::Value& rValue )
    {
        boost::tuple<const camp::Property*, String, camp::UserObject> params = 
            getPropertyFromString( rUserObject, rQuery );

        if( params.get<0>() && !params.get<0>()->hasTag( "NoPropertySet" ) )
        {
            if( params.get<1>().empty() )
            {
                // Not an array property.
                params.get<0>()->set( params.get<2>(), rValue );
            }
            else
            {
                // An array property.
                const camp::ArrayProperty* arrayProperty = 
                    static_cast<const camp::ArrayProperty*>( params.get<0>() );
                return arrayProperty->set( params.get<2>(), boost::lexical_cast<size_t>( 
                    params.get<1>() ), rValue );
            }
        }
    }
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
        const camp::Value& rValue )
    {
        boost::tuple<const camp::Property*, String, camp::UserObject> params = 
            getPropertyFromString( rUserObject, rQuery, true );

        if( params.get<0>() && !params.get<0>()->hasTag( "NoPropertyInsert" ) )
        {
            if( params.get<0>()->type() == camp::arrayType )
            {
                // An array property.
                const camp::ArrayProperty* arrayProperty = 
                    static_cast<const camp::ArrayProperty*>( params.get<0>() );
                arrayProperty->insert( params.get<2>(), arrayProperty->size( params.get<2>() ), rValue );
            }
            else
            {
                // Not an array property.
                DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, 
                    "Given property is not an array", "CampStringInterpreter::insert" );
            }
        }
    }
    /**
    Removes the array identifiers from given query.
    
    @param  rQuery  The query. 
    
    @return The query with array identifiers removed.
    **/
    static String removeArrayIdentifiers( const String& rQuery )
    {
        return boost::regex_replace( rQuery, boost::regex( "\\[.*?\\]" ), "" );
    }

private:
    static boost::tuple<const camp::Property*, String, camp::UserObject> getPropertyFromString( 
        const camp::UserObject& rUserObject, const String& rQuery, bool wantArrayProperty = false )
    {
        camp::UserObject userObject = rUserObject;
        const camp::Class* metaClass = &userObject.getClass();
        std::size_t pos = 0;
        const std::size_t size = rQuery.size();
        camp::Value value = camp::Value::nothing;

        const camp::Property* property = 0;
        String arrayIdentifier;

        while( pos < size )
        {
            String dotString;
            std::size_t dotPos = rQuery.find_first_of( ".", pos );
            if( dotPos != String::npos )
            {
                // Get a string token, separated by a dot.
                dotString = rQuery.substr( pos, dotPos - pos );
            }
            else
            {
                // No dot was found, take the substring from position to the end of the string.
                dotString = rQuery.substr( pos );
                // End the loop.
                dotPos = size;
            }

            // Get array identifier if there is one.
            const std::size_t arrayBeginPos = dotString.find_first_of( "[" );
            if( arrayBeginPos != String::npos )
            {
                // Separate property name and array identifier.
                String propertyString = dotString.substr( 0, arrayBeginPos );
                const std::size_t arrayEndPos = dotString.find_first_of( "]" );

                arrayIdentifier = dotString.substr( arrayBeginPos + 1, 
                    dotString.size() - arrayEndPos );
                property = &metaClass->property( propertyString );

                if( property->type() == camp::arrayType )
                {
                    const camp::ArrayProperty* arrayProperty = 
                        static_cast<const camp::ArrayProperty*>( property );

                    // Convert array identifier to a number.
                    value = arrayProperty->get( userObject, boost::lexical_cast<size_t>( 
                        arrayIdentifier ) );
                }
                else
                {
                    // Array identifier found, but property is not an array.
                    DIVERSIA_EXCEPT( Exception::ERR_SYNTAX_ERROR, 
                        "Array identifier found in query string, but property is not an array", 
                        "CampStringInterpreter::getPropertyFromString" );
                }
            }
            else
            {
                property = &metaClass->property( dotString );

                if( property->type() != camp::arrayType )
                {
                    value = property->get( userObject );
                    arrayIdentifier = "";
                }
                else if( !wantArrayProperty )
                {
                    // Property is an array but no array identifier was found in the query.
                    DIVERSIA_EXCEPT( Exception::ERR_SYNTAX_ERROR, 
                        "Property is an array, but no array identifier was found in the query string", 
                        "CampStringInterpreter::getPropertyFromString" );
                }
            }

            if( value.type() == camp::userType && dotPos != size )
            {
                // Value is of user type, set the new metaclass and userobject and nest further.
                // Only nest further if the loop has not been completed yet.
                userObject = value.to<camp::UserObject>();
                metaClass = &userObject.getClass();
            }
            else
            {
                // No more nesting possible, end the loop.
                dotPos = size;
            }

            pos = dotPos + 1;
        }

        // Return the property and optional array identifier
        return boost::make_tuple( property, arrayIdentifier, userObject );
    }
};

//------------------------------------------------------------------------------
} // Namespace Diversia

#endif // DIVERSIA_SHARED_STRINGINTERPRETER_H