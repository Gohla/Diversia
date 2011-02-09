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

#include "Shared/Platform/StableHeaders.h"

#include "Shared/Camp/PropertyTransaction.h"
#include "Shared/Camp/CampStringInterpreter.h"

namespace Diversia
{
//------------------------------------------------------------------------------

void PropertyTransaction::addChangedProperty( const String& rName, const camp::Value& rValue )
{
    mChangedProperties[ rName ] = rValue;
}

void PropertyTransaction::addInsertedProperty( const String& rName, const camp::Value& rValue )
{
    mInsertedProperties.insert( std::make_pair( rName, rValue ) );
}

void PropertyTransaction::addRemovedProperty( const String& rName )
{
    mRemovedProperties.insert( rName );
}

void PropertyTransaction::apply( camp::UserObject& rObject )
{
    // TODO: Remove properties
    // Insert properties
    for( ValueMultimap::iterator i = mInsertedProperties.begin(); i != mInsertedProperties.end(); 
        ++i )
    {
        CampStringInterpreter::insert( rObject, i->first, i->second );
    }

    // Change properties
    for( ValueMap::iterator i = mChangedProperties.begin(); i != mChangedProperties.end(); ++i )
    {
        CampStringInterpreter::set( rObject, i->first, i->second );
    }
}

void PropertyTransaction::reset()
{
    mChangedProperties.clear();
    mInsertedProperties.clear();
    mRemovedProperties.clear();
}

//------------------------------------------------------------------------------
} // Namespace Diversia
