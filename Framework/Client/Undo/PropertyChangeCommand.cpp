/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Client/Platform/StableHeaders.h"

#include "Client/Undo/PropertyChangeCommand.h"

namespace Diversia
{
namespace Client 
{
//------------------------------------------------------------------------------

PropertyChangeCommand::PropertyChangeCommand( const String& rPropertyName, 
    const camp::UserObject& rUserObject, const camp::Value& rNewValue ):
    UndoCommand( PropertyChangeCommand::name( rPropertyName ) ),
    mProperty( rUserObject.getClass().property( rPropertyName ) ),
    mUserObject( rUserObject ),
    mRedoValue( rNewValue )
{
    // Copy the value for user types.
    if( mRedoValue.type() == camp::userType ) mRedoValue = mRedoValue.to<camp::UserObject>().copy();

    PropertyChangeCommand::storeCurrentValue();
}

PropertyChangeCommand::PropertyChangeCommand( const camp::Property& rProperty, 
    const camp::UserObject& rUserObject, const camp::Value& rNewValue ):
    UndoCommand( PropertyChangeCommand::name( rProperty.name() ) ),
    mProperty( rProperty ),
    mUserObject( rUserObject ),
    mRedoValue( rNewValue )
{
    // Copy the value for user types.
    if( mRedoValue.type() == camp::userType ) mRedoValue = mRedoValue.to<camp::UserObject>().copy();

    PropertyChangeCommand::storeCurrentValue();
}

bool PropertyChangeCommand::mergeWith( const UndoCommand* pCommand )
{
    return false;
}

void PropertyChangeCommand::redo()
{
    try
    {
        mProperty.set( mUserObject, mRedoValue );
    }
    catch( Exception e )
    {
    	LCLOGW << "Could not redo '" << name() << "': " << e.what();
    }
    catch( camp::Error e )
    {
        LCLOGW << "Could not redo '" << name() << "': " << e.what();
    }
}

void PropertyChangeCommand::undo()
{
    try
    {
        mProperty.set( mUserObject, mUndoValue );
    }
    catch( Exception e )
    {
        LCLOGW << "Could not undo '" << name() << "': " << e.what();
    }
    catch( camp::Error e )
    {
        LCLOGW << "Could not undo '" << name() << "': " << e.what();
    }
}

String PropertyChangeCommand::name( const String& rPropertyName )
{
    return String( "Set " ) + rPropertyName + " property";
}

String PropertyChangeCommand::name()
{
    return String( "Set " ) + mProperty.name() + " property";
}

void PropertyChangeCommand::storeCurrentValue()
{
    try
    {
        mUndoValue = mProperty.get( mUserObject );
        // Copy the value for user types.
        if( mUndoValue.type() == camp::userType ) mUndoValue = mUndoValue.to<camp::UserObject>().copy();
    }
    catch( Exception e )
    {
        LCLOGW << "Could not store undo value for '" << name() << "': " << e.what();
    }
    catch( camp::Error e )
    {
        LCLOGW << "Could not store undo value for '" << name() << "': " << e.what();
    }
}

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia