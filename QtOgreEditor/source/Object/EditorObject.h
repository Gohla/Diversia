/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_EDITOROBJECT_H
#define DIVERSIA_QTOGREEDITOR_EDITOROBJECT_H

#include "Platform/Prerequisites.h"

#include "Client/Object/ClientObject.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

class EditorObject : public ClientObject
{
public:
    /**
    Values that represent a gizmo mode.
    **/
    enum GizmoMode 
    {
        NONE,
        MOVEMENT,
        ROTATION,
        SCALING
    };

    /**
    Selects or unselects this object.
    **/
    void setSelected( bool selected );

    /**
    Sets the gizmo mode. 
    **/
    inline static void setGizmoMode( GizmoMode mode ) { mGizmoModeSignal( mode ); }
    
private:
    friend class EditorObjectManager;	///< Only the EditorObjectManager class may construct objects. 

    EditorObject( const String& rName, Mode mode, NetworkingType type, const String& rDisplayName, 
        RakNet::RakNetGUID source, RakNet::RakNetGUID ownGUID, RakNet::RakNetGUID serverGUID, 
        sigc::signal<void>& rUpdateSignal, ClientObjectManager& rObjectManager, 
        PermissionManager& rPermissionManager, RakNet::ReplicaManager3& rReplicaManager,
        RakNet::NetworkIDManager& rNetworkIDManager, RakNet::RPC3& rRPC3 );
    virtual ~EditorObject();

    void gizmoModeChange( GizmoMode mode );
    void checkGizmo();

    Gizmo* mGizmo;
    sigc::connection mGizmoModeConnection;
    static sigc::signal<void, GizmoMode> mGizmoModeSignal;
    GizmoMode mGizmoMode;
    bool mSelected;


    CAMP_CLASS(EditorObject)

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::QtOgreEditor::EditorObject, 
    &Diversia::QtOgreEditor::Bindings::CampBindings::bindEditorObject );

#endif // DIVERSIA_QTOGREEDITOR_EDITOROBJECT_H