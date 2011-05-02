/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_EDITOROBJECTMANAGER_H
#define DIVERSIA_QTOGREEDITOR_EDITOROBJECTMANAGER_H

#include "Platform/Prerequisites.h"

#include "Client/Object/ClientObjectManager.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

class EditorObjectManager : public ClientObjectManager
{
public:
    EditorObjectManager( Mode mode, PluginState state, sigc::signal<void>& rUpdateSignal, 
        sigc::signal<void>& rLateUpdateSignal, ServerPluginManager& rPluginManager, 
        RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
        RakNet::NetworkIDManager& rNetworkIDManager );
    virtual ~EditorObjectManager();

    /**
    Gets the plugin type.
    **/
    inline ClientServerPluginTypeEnum getType() const { return CLIENTSERVERPLUGINTYPE_OBJECTMANAGER; }
    static inline ClientServerPluginTypeEnum getTypeStatic() { return CLIENTSERVERPLUGINTYPE_OBJECTMANAGER; }
    /**
    Gets the plugin type name.
    **/
    inline String getTypeName() const { return CLIENTSERVERPLUGINNAME_OBJECTMANAGER; }
    static inline String getTypeNameStatic() { return CLIENTSERVERPLUGINNAME_OBJECTMANAGER; }
    
private:
    friend class EditorObject;	///< For delayed destruction.

    Object& createObjectImpl( const String& rName, NetworkingType type, const String& rDisplayName,
        RakNet::RakNetGUID source );

    CAMP_CLASS(EditorObjectManager)

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::QtOgreEditor::EditorObjectManager, 
    &Diversia::QtOgreEditor::Bindings::CampBindings::bindEditorObjectManager );

#endif // DIVERSIA_QTOGREEDITOR_EDITOROBJECTMANAGER_H