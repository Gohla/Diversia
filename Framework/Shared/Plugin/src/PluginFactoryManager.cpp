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

#include "Shared/Plugin/PluginFactoryManager.h"
#include "Shared/Plugin/PluginFactory.h"

namespace Diversia
{
//------------------------------------------------------------------------------

PluginFactories PluginFactoryManager::msPluginFactories = 
    PluginFactories();

void PluginFactoryManager::registerPluginFactory( PluginTypeEnum type, 
    PluginFactory* pPluginFactory )
{
    if( !hasPluginFactory( type ) )
    {
        msPluginFactories.insert( std::make_pair( type, pPluginFactory ) );
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_DUPLICATE_ITEM, 
            "Client-server plugin factory " + boost::lexical_cast<String>( type ) +
            " is already registered", 
            "PluginFactoryManager::registerPluginFactory" );
    }
}

PluginFactory& PluginFactoryManager::getPluginFactory( 
    PluginTypeEnum type )
{
    PluginFactories::iterator i = msPluginFactories.find( type );
    if( i != msPluginFactories.end() )
    {
        return *i->second;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, 
            "Client-server plugin factory " + boost::lexical_cast<String>( type ) + 
            " is not registered.", 
            "PluginFactoryManager::getPluginFactory" );
    }
}

bool PluginFactoryManager::hasPluginFactory( PluginTypeEnum type )
{
    return msPluginFactories.find( type ) != msPluginFactories.end();
}

void PluginFactoryManager::unregisterPluginFactory( PluginTypeEnum type )
{
    PluginFactories::iterator i = msPluginFactories.find( type );
    if( i != msPluginFactories.end() )
    {
        msPluginFactories.erase( i );
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, 
            "Client-server plugin factory " + boost::lexical_cast<String>( type ) + 
            " is not registered.", 
            "PluginFactoryManager::unregisterPluginFactory" );
    }
}

void PluginFactoryManager::destroyPluginFactory()
{
    for( PluginFactories::iterator i = msPluginFactories.begin(); 
        i != msPluginFactories.end(); ++i )
    {
        delete i->second;
    }
    msPluginFactories.clear();
}

//------------------------------------------------------------------------------
} // Namespace Diversia
