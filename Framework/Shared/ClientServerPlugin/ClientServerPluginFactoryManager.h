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

#ifndef DIVERSIA_SHARED_CLIENTSERVERPLUGINFACTORYMANAGER_H
#define DIVERSIA_SHARED_CLIENTSERVERPLUGINFACTORYMANAGER_H

#include "Shared/Platform/Prerequisites.h"

namespace Diversia
{
//------------------------------------------------------------------------------

typedef std::map<ClientServerPluginTypeEnum, ClientServerPluginFactory*> ClientServerPluginFactories;

class DIVERSIA_SHARED_API ClientServerPluginFactoryManager
{
public:
    /**
    Register client server plugin factory. 
    
    @param  type                                The type of the factory. 
    @param [in,out] pClientServerPluginFactory  The client-server plugin factory to add.
                                                
    @throw  Exception   When a factory for that type already exists. 
    **/
    static void registerPluginFactory( ClientServerPluginTypeEnum type, 
        ClientServerPluginFactory* pClientServerPluginFactory );
    /**
    Gets a client server plugin factory. 
    
    @param  type    The type of the factory.
                    
    @throw  Exception   When a factory for that type doesn't exists. 
    **/
    static ClientServerPluginFactory& getPluginFactory( ClientServerPluginTypeEnum type );
    /**
    Gets the map of client-server plugin factories. 
    **/
    inline static const ClientServerPluginFactories& getPluginFactories() { return msClientServerPluginFactories; }
    /**
    Query if a client-server plugin factory is registered.
    
    @param  type    The type of the factory
    
    @return true if factory exists, false if not.
    **/
    static bool hasPluginFactory( ClientServerPluginTypeEnum type );
    /**
    Unregister client-server plugin factory. 
    
    @param  type    The type of the factory.

    @throw  Exception   When a factory for that type doesn't exists. 

    @note   Doesn't destroy the factory!
    **/
    static void unregisterPluginFactory( ClientServerPluginTypeEnum type );
    /**
    Destroys all client-server plugin factories.
    **/
    static void destroyPluginFactory();

private:
    ClientServerPluginFactoryManager() {}
    ~ClientServerPluginFactoryManager() {}

    static ClientServerPluginFactories msClientServerPluginFactories;

};

//------------------------------------------------------------------------------
} // Namespace Diversia

#endif // DIVERSIA_SHARED_CLIENTSERVERPLUGINFACTORYMANAGER_H
