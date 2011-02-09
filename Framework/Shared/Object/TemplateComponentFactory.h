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

#ifndef DIVERSIA_SHARED_TEMPLATECOMPONENTFACTORY_H
#define DIVERSIA_SHARED_TEMPLATECOMPONENTFACTORY_H

#include "Shared/Platform/Prerequisites.h"

#include "Object/ComponentFactory.h"
#include "Object/ComponentFactoryManager.h"

namespace Diversia
{
//------------------------------------------------------------------------------

template <typename T, typename U, bool Multiple = false, bool CanDestroy = true, 
    bool ClientOnly = false, bool ServerOnly = false> 
class TemplateComponentFactory : public ComponentFactory
{
public:
    /**
    Gets the type.
    **/
    inline ComponentType getType() const { return T::getTypeStatic(); }
    inline static ComponentType getTypeStatic() { return T::getTypeStatic(); }
    /**
    Gets the type name.
    **/
    inline String getTypeName() const { return T::getTypeNameStatic(); }
    inline static String getTypeNameStatic() { return T::getTypeNameStatic(); }
    /**
    Query if multiple instances of this component can be added to one object.
    **/
    inline bool multiple() const { return Multiple; }
    /**
    Queries if this component can be destroyed. Defaults to true.
    **/
    virtual bool canDestroy() { return CanDestroy; }
    /**
    Queries if this component can only be created on the client. Defaults to false.
    **/
    virtual bool clientOnly() { return ClientOnly; }
    /**
    Queries if this component can only be created on the server. Defaults to false.
    **/
    virtual bool serverOnly() { return ServerOnly; }

    /**
    Creates an instance of a component.

    @see Component::Component()
    **/
    T& create( const String& rName, Mode mode, NetworkingType networkingType,
        RakNet::RakNetGUID source, bool localOverride, Object& rObject ) 
    { 
        return *new T( rName, mode, networkingType, source, localOverride, 
            static_cast<U&>( rObject ) ); 
    }

    /**
    Destroys an instance of a component.
    **/
    void destroy( Component& rComponent ) { delete &static_cast<T&>( rComponent ); }

    /**
    Register this component factory with the factory manager.
    **/
    inline static void registerFactory()
    {
        ComponentFactoryManager::registerComponentFactory( T::getTypeStatic(),
            new TemplateComponentFactory<T, U, Multiple, CanDestroy, ClientOnly, ServerOnly>() );
    }
};

//------------------------------------------------------------------------------
} // Namespace Diversia

#endif // DIVERSIA_SHARED_TEMPLATECOMPONENTFACTORY_H