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

#ifndef DIVERSIA_OBJECTSYSTEM_COMPONENTFACTORYMANAGER_H
#define DIVERSIA_OBJECTSYSTEM_COMPONENTFACTORYMANAGER_H

#include "Object/Platform/Prerequisites.h"

namespace Diversia
{
namespace ObjectSystem
{
//------------------------------------------------------------------------------

typedef std::map<ComponentType, ComponentFactory*> ComponentFactories;

class DIVERSIA_OBJECT_API ComponentFactoryManager
{
public:
    /**
    Register a component factory.

    @param  type                        The type of the factory.
    @param [in,out] pComponentFactory   The component factory to add.

    @throw  Exception   When a factory for that type already exists.
    **/
    static void registerComponentFactory( ComponentType type, ComponentFactory* pComponentFactory );
    /**
    Gets a component factory by type

    @param  type    The type of the factory.

    @throw  Exception   When a factory for that type doesn't exists.
    **/
    static ComponentFactory& getComponentFactory( ComponentType type );
    /**
    Gets the map of component factories.
    **/
    inline static const ComponentFactories& getComponentFactories() { return msComponentFactories; }
    /**
    Query if a component factory is registered.

    @param  type    The type of the factory

    @return true if factory exists, false if not.
    **/
    static bool hasComponentFactory( ComponentType type );
    /**
    Unregister component factory.

    @param  type    The type of the factory.

    @throw  Exception   When a factory for that type doesn't exists.

    @note   Doesn't destroy the factory!
    **/
    static void unregisterComponentFactory( ComponentType type );
    /**
    Destroys all component factories.
    **/
    static void destroyComponentFactories();

private:
    ComponentFactoryManager() {}
    ~ComponentFactoryManager() {}

    static ComponentFactories msComponentFactories;
};

//------------------------------------------------------------------------------
} // Namespace ObjectSystem
} // Namespace Diversia

#endif // DIVERSIA_OBJECTSYSTEM_COMPONENTFACTORYMANAGER_H