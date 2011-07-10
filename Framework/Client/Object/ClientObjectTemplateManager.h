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

#ifndef DIVERSIA_CLIENT_CLIENTOBJECTTEMPLATEMANAGER_H
#define DIVERSIA_CLIENT_CLIENTOBJECTTEMPLATEMANAGER_H

#include "Client/Platform/Prerequisites.h"

#include "Client/Plugin/ClientPlugin.h"
#include "Object/ObjectTemplateManager.h"
#include "Shared/Plugin/Factories/TemplatePluginFactory.h"

namespace Diversia
{
namespace Client
{
//------------------------------------------------------------------------------

class DIVERSIA_CLIENT_API ClientObjectTemplateManager : public ObjectTemplateManager, 
    public ClientPlugin
{
public:
    /**
    Gets the plugin type.
    **/
    inline PluginTypeEnum getType() const { return PLUGINTYPE_OBJECTTEMPLATEMANAGER; }
    static inline PluginTypeEnum getTypeStatic() { return PLUGINTYPE_OBJECTTEMPLATEMANAGER; }
    /**
    Gets the plugin type name.
    **/
    inline String getTypeName() const { return PLUGINNAME_OBJECTTEMPLATEMANAGER; }
    static inline String getTypeNameStatic() { return PLUGINNAME_OBJECTTEMPLATEMANAGER; }
	
private:
    friend class TemplatePluginFactory<ClientObjectTemplateManager, ClientPluginManager>;

    ClientObjectTemplateManager( Mode mode, PluginState state, ClientPluginManager& rPluginManager, 
        RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
        RakNet::NetworkIDManager& rNetworkIDManager );
    virtual ~ClientObjectTemplateManager();

    void load();
    void unload();
    inline void setServerState( ServerState serverState ) {}
    void offlineModeChanged( bool offlineMode );

    void queryDestroyObjectTemplate( ObjectTemplate& rObjectTemplate, RakNet::RakNetGUID source );
    ObjectTemplate& createObjectTemplateImpl( const String& rName, NetworkingType type, 
        const String& rDisplayName, RakNet::RakNetGUID source );

    PermissionManager&  mPermissionManager;

    CAMP_CLASS(ClientObjectTemplateManager)

};

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Client::ClientObjectTemplateManager, 
    &Diversia::Client::Bindings::CampBindings::bindClientObjectTemplateManager );

#endif // DIVERSIA_CLIENT_CLIENTOBJECTTEMPLATEMANAGER_H