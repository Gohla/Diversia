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

#ifndef DIVERSIA_SHARED_TEMPLATEPLUGINFACTORY_H
#define DIVERSIA_SHARED_TEMPLATEPLUGINFACTORY_H

#include "Shared/Platform/Prerequisites.h"

#include "Shared/ClientServerPlugin/ClientServerPluginFactoryManager.h"
#include "Shared/ClientServerPlugin/ClientServerPluginFactory.h"

namespace Diversia
{
//------------------------------------------------------------------------------

/**
Generic plugin factory for plugins that do not need special construction or destruction.
**/
template <class T, class U> class TemplatePluginFactory : public ClientServerPluginFactory
{
public:
    /**
    Gets the plugin type.
    **/
    inline ClientServerPluginTypeEnum getType() const { return T::getTypeStatic(); }
    inline static ClientServerPluginTypeEnum getTypeStatic() { return T::getTypeStatic(); }
    /**
    Gets the plugin type name.
    **/
    inline String getTypeName() const { return T::getTypeNameStatic(); }
    inline static String getTypeNameStatic() { return T::getTypeNameStatic(); }

    /**
    Creates an instance of a client-server plugin.

    @see ClientServerPlugin::ClientServerPlugin()
    **/
    inline T& create( Mode mode, PluginState state, ClientServerPluginManager& rPluginManager, 
        RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager,
        RakNet::NetworkIDManager& rNetworkIDManager )
    {
        return *new T( mode, state, static_cast<U&>( rPluginManager ), rRakPeer, rReplicaManager, 
            rNetworkIDManager ); 
    }
    /**
    Destroys an instance of a client-server plugin.
    **/
    inline void destroy( ClientServerPlugin& rPlugin ) { delete &rPlugin; }

    /**
    Register this plugin factory with the factory manager.
    **/
    inline static void registerFactory()
    {
        ClientServerPluginFactoryManager::registerPluginFactory( T::getTypeStatic(),
            new TemplatePluginFactory<T, U>() );
    }

};

//------------------------------------------------------------------------------
} // Namespace Diversia

#endif // DIVERSIA_SHARED_TEMPLATEPLUGINFACTORY_H