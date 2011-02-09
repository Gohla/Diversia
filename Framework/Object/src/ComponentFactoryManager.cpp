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

#include "Object/Platform/StableHeaders.h"

#include "Object/ComponentFactoryManager.h"
#include "Object/ComponentFactory.h"

namespace Diversia
{
namespace ObjectSystem 
{
//------------------------------------------------------------------------------

ComponentFactories ComponentFactoryManager::msComponentFactories = ComponentFactories();

void ComponentFactoryManager::registerComponentFactory( ComponentType type, 
    ComponentFactory* pComponentFactory )
{
    if( !hasComponentFactory( type ) )
    {
        msComponentFactories.insert( std::make_pair( type, pComponentFactory ) );
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_DUPLICATE_ITEM, "Component factory " + 
            boost::lexical_cast<String>( type ) + " is already registered", 
            "ComponentFactoryManager::registerComponentFactory" );
    }
}

ComponentFactory& ComponentFactoryManager::getComponentFactory( ComponentType type )
{
    ComponentFactories::iterator i = msComponentFactories.find( type );
    if( i != msComponentFactories.end() )
    {
        return *i->second;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Component factory " + 
            boost::lexical_cast<String>( type ) + " is not registered.", 
            "ComponentFactoryManager::getComponentFactory" );
    }
}

bool ComponentFactoryManager::hasComponentFactory( ComponentType type )
{
    return msComponentFactories.find( type ) != msComponentFactories.end();
}

void ComponentFactoryManager::unregisterComponentFactory( ComponentType type )
{
    ComponentFactories::iterator i = msComponentFactories.find( type );
    if( i != msComponentFactories.end() )
    {
        msComponentFactories.erase( i );
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Component factory " + 
            boost::lexical_cast<String>( type ) + " is not registered.", 
            "ComponentFactoryManager::unregisterComponentFactory" );
    }
}

void ComponentFactoryManager::destroyComponentFactories()
{
    for( ComponentFactories::iterator i = msComponentFactories.begin(); 
        i != msComponentFactories.end(); ++i )
    {
        delete i->second;
    }
    msComponentFactories.clear();
}

//------------------------------------------------------------------------------
} // Namespace ObjectSystem
} // Namespace Diversia