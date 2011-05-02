/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "Camp/CampBindings.h"
#include "EditorApplication.h"
#include "Object/EditorObject.h"
#include "Object/EditorObjectManager.h"
#include "UI/ConnectDialog.h"

namespace Diversia
{
namespace QtOgreEditor 
{
namespace Bindings 
{
//------------------------------------------------------------------------------

void CampBindings::bindEditorApplication()
{
    camp::Class::declare<EditorApplication>( "EditorApplication" );
    	// Constructors
    	// Properties (read-only)
    	// Properties (read/write)
    	// Functions
    	// Static functions
    	// Operators
}

void CampBindings::bindConnectDialog()
{
    camp::Class::declare<ConnectDialog>( "ConnectDialog" )
    	// Constructors
    	// Properties (read-only)
    	// Properties (read/write)
        .property( "LastServer", &ConnectDialog::mLastServer )
            .tag( "Configurable" )
        .property( "LastUserName", &ConnectDialog::mLastUsername )
            .tag( "Configurable" )
        .property( "LastNickname", &ConnectDialog::mLastNickname )
            .tag( "Configurable" );
    	// Functions
    	// Static functions
    	// Operators
}

void CampBindings::bindEditorObject()
{
    camp::Class::declare<EditorObject>( "EditorObject" )
        .tag( "NoSerialization", &Object::isRuntimeObject )
        .base<ClientObject>();
	    // Constructors
	    // Properties (read-only)
	    // Properties (read/write)
	    // Functions
	    // Static functions
	    // Operators
}

void CampBindings::bindEditorObjectManager()
{
    camp::Class::declare<EditorObjectManager>( "EditorObjectManager" )
        .base<ClientObjectManager>();
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        // Functions
        // Static functions
        // Operators
}

//------------------------------------------------------------------------------
} // Namespace Bindings
} // Namespace QtOgreEditor
} // Namespace Diversia