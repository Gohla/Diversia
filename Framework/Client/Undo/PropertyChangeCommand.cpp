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
    const camp::UserObject& rUserObject, const camp::Value& rNewValue, 
    const camp::Property* pParentProperty /*= 0*/, 
    const camp::UserObject& rParentObject /*= camp::UserObject::nothing*/ ):
    UndoCommand( PropertyChangeCommand::name( rPropertyName, pParentProperty ) ),
    mProperty( rUserObject.getClass().property( rPropertyName ) ),
    mUserObject( rUserObject ),
    mParentProperty( pParentProperty ),
    mParentUserObject( rParentObject ),
    mRedoValue( rNewValue )
{
    // Copy the value for user types.
    if( mRedoValue.type() == camp::userType ) mRedoValue = mRedoValue.to<camp::UserObject>().copy();

    PropertyChangeCommand::storeCurrentValue();
}

PropertyChangeCommand::PropertyChangeCommand( const camp::Property& rProperty, 
    const camp::UserObject& rUserObject, const camp::Value& rNewValue, 
    const camp::Property* pParentProperty /*= 0*/, 
    const camp::UserObject& rParentObject /*= camp::UserObject::nothing*/ ):
    UndoCommand( PropertyChangeCommand::name( rProperty.name(), pParentProperty ) ),
    mProperty( rProperty ),
    mUserObject( rUserObject ),
    mParentProperty( pParentProperty ),
    mParentUserObject( rParentObject ),
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

        // Re set the property if it has the DeserializeReSet tag
        if( mParentProperty && mParentUserObject != camp::UserObject::nothing && 
            mParentProperty->hasTag( "DeserializeReSet" ) )
        {
            camp::Value val = mParentProperty->get( mParentUserObject );
            mParentProperty->set( mParentUserObject, val );
        }
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

        // Re set the property if it has the DeserializeReSet tag
        if( mParentProperty && mParentUserObject != camp::UserObject::nothing && 
            mParentProperty->hasTag( "DeserializeReSet" ) )
        {
            camp::Value val = mParentProperty->get( mParentUserObject );
            mParentProperty->set( mParentUserObject, val );
        }
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

String PropertyChangeCommand::name( const String& rPropertyName, 
    const camp::Property* pParentProperty /*= 0*/ )
{
    return String( "Set " ) + ( pParentProperty ? pParentProperty->name() + "." : "" )  + 
        rPropertyName + " property";
}

String PropertyChangeCommand::name()
{
    return String( "Set " ) + ( mParentProperty ? mParentProperty->name() + "." : "" ) + 
        mProperty.name() + " property";
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