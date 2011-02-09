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

#ifndef DIVERSIA_OBJECT_COMPONENTFACTORY_H
#define DIVERSIA_OBJECT_COMPONENTFACTORY_H

#include "Object/Platform/Prerequisites.h"

#include "Object/ObjectManager.h"

namespace Diversia
{
namespace ObjectSystem
{
//------------------------------------------------------------------------------

class DIVERSIA_OBJECT_API ComponentFactory
{
public:
    /**
    Default constructor.
    **/
    ComponentFactory() {}
    /**
    Destructor.
    **/
    virtual ~ComponentFactory() {}

    /**
    Gets the type.
    **/
    virtual ComponentType getType() const = 0;
    /**
    Gets the type name.
    **/
    virtual String getTypeName() const = 0;
    /**
    Query if multiple instances of this component can be added to one object.
    **/
    virtual bool multiple() const = 0;
    /**
    Queries if this component can be destroyed. Defaults to true.
    **/
    virtual bool canDestroy() { return true; }
    /**
    Queries if this component can only be created on the client. Defaults to false.
    **/
    virtual bool clientOnly() { return false; }
    /**
    Queries if this component can only be created on the server. Defaults to false.
    **/
    virtual bool serverOnly() { return false; }

    /**
    Creates an instance of a component.

    @see Component::Component()
    **/
    virtual Component& create( const String& rName, Mode mode, NetworkingType networkingType,
        RakNet::RakNetGUID source, bool localOverride, Object& rObject ) = 0;
    /**
    Destroys an instance of a component.
    **/
    virtual void destroy( Component& rComponent ) = 0;
};

//------------------------------------------------------------------------------
} // Namespace Object
} // Namespace Diversia

#endif