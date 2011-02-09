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

#include "Shared/ClientServerPlugin/ClientServerPluginFactoryManager.h"
#include "Shared/ClientServerPlugin/ClientServerPluginFactory.h"

namespace Diversia
{
//------------------------------------------------------------------------------

ClientServerPluginFactories ClientServerPluginFactoryManager::msClientServerPluginFactories = 
    ClientServerPluginFactories();

void ClientServerPluginFactoryManager::registerPluginFactory( ClientServerPluginTypeEnum type, 
    ClientServerPluginFactory* pClientServerPluginFactory )
{
    if( !hasPluginFactory( type ) )
    {
        msClientServerPluginFactories.insert( std::make_pair( type, pClientServerPluginFactory ) );
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_DUPLICATE_ITEM, 
            "Client-server plugin factory " + boost::lexical_cast<String>( type ) +
            " is already registered", 
            "ClientServerPluginFactoryManager::registerPluginFactory" );
    }
}

ClientServerPluginFactory& ClientServerPluginFactoryManager::getPluginFactory( 
    ClientServerPluginTypeEnum type )
{
    ClientServerPluginFactories::iterator i = msClientServerPluginFactories.find( type );
    if( i != msClientServerPluginFactories.end() )
    {
        return *i->second;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, 
            "Client-server plugin factory " + boost::lexical_cast<String>( type ) + 
            " is not registered.", 
            "ClientServerPluginFactoryManager::getPluginFactory" );
    }
}

bool ClientServerPluginFactoryManager::hasPluginFactory( ClientServerPluginTypeEnum type )
{
    return msClientServerPluginFactories.find( type ) != msClientServerPluginFactories.end();
}

void ClientServerPluginFactoryManager::unregisterPluginFactory( ClientServerPluginTypeEnum type )
{
    ClientServerPluginFactories::iterator i = msClientServerPluginFactories.find( type );
    if( i != msClientServerPluginFactories.end() )
    {
        msClientServerPluginFactories.erase( i );
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, 
            "Client-server plugin factory " + boost::lexical_cast<String>( type ) + 
            " is not registered.", 
            "ClientServerPluginFactoryManager::unregisterPluginFactory" );
    }
}

void ClientServerPluginFactoryManager::destroyPluginFactory()
{
    for( ClientServerPluginFactories::iterator i = msClientServerPluginFactories.begin(); 
        i != msClientServerPluginFactories.end(); ++i )
    {
        delete i->second;
    }
    msClientServerPluginFactories.clear();
}

//------------------------------------------------------------------------------
} // Namespace Diversia
