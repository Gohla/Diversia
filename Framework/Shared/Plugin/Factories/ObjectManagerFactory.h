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

#ifndef DIVERSIA_SHARED_OBJECTMANAGERFACTORY_H
#define DIVERSIA_SHARED_OBJECTMANAGERFACTORY_H

#include "Shared/Platform/Prerequisites.h"

#include "Shared/Plugin/PluginFactory.h"

namespace Diversia
{
//------------------------------------------------------------------------------

template <typename T, class U> class ObjectManagerFactory : public PluginFactory
{
public:
    /**
    Constructor. 
    
    @param [in,out] rUpdateSignal   the update signal. 
    **/
    ObjectManagerFactory( sigc::signal<void>& rUpdateSignal, sigc::signal<void>& rLateUpdateSignal ): 
      mUpdateSignal( rUpdateSignal ), mLateUpdateSignal( rLateUpdateSignal ) {}

    /**
    Gets the plugin type.
    **/
    inline PluginTypeEnum getType() const { return T::getTypeStatic(); }
    inline static PluginTypeEnum getTypeStatic() { return T::getTypeStatic(); }
    /**
    Gets the plugin type name.
    **/
    inline String getTypeName() const { return T::getTypeNameStatic(); }
    inline static String getTypeNameStatic() { return T::getTypeNameStatic(); }

    /**
    Creates an instance of a client-server plugin.

    @see Plugin::Plugin()
    **/
    inline T& create( Mode mode, PluginState state, PluginManager& rPluginManager, 
        RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
        RakNet::NetworkIDManager& rNetworkIDManager )
    {
        return *new T( mode, state, mUpdateSignal, mLateUpdateSignal, 
            static_cast<U&>( rPluginManager ), rRakPeer, rReplicaManager, rNetworkIDManager );
    }
    /**
    Destroys an instance of a client-server plugin.
    **/
    inline void destroy( Plugin& rPlugin ) { delete &rPlugin; }

    /**
    Register this plugin factory with the factory manager. 
    
    @param [in,out] rUpdateSignal   The (frame/tick) update signal.
    **/
    inline static void registerFactory( sigc::signal<void>& rUpdateSignal, 
        sigc::signal<void>& rLateUpdateSignal )
    {
        PluginFactoryManager::registerPluginFactory( T::getTypeStatic(),
            new ObjectManagerFactory<T, U>( rUpdateSignal, rLateUpdateSignal ) );
    }
    
private:
    sigc::signal<void>& mUpdateSignal;
    sigc::signal<void>& mLateUpdateSignal;

};

//------------------------------------------------------------------------------
} // Namespace Diversia

#endif // DIVERSIA_SHARED_OBJECTMANAGERFACTORY_H