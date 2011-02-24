/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Client/Platform/StableHeaders.h"

#include "Client/Undo/PropertyChangeCommand.h"
#include "Object/Component.h"
#include "Object/Object.h"

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
    PropertyChangeCommand::connectDestruction();
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
    PropertyChangeCommand::connectDestruction();
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

bool PropertyChangeCommand::hasBase( const camp::Class& rClass, const camp::Class& rBaseClass )
{
    for( std::size_t i = 0; i < rClass.baseCount(); ++i )
    {
        const camp::Class& base = rClass.base( i );

        if( base == rBaseClass ) return true;
        else if( base.baseCount() && PropertyChangeCommand::hasBase( base, rBaseClass ) ) return true; 
    }

    return false;
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

void PropertyChangeCommand::connectDestruction()
{
    const camp::Class* metaclass;
    void* ptr;
    if( mParentUserObject != camp::UserObject::nothing ) 
    {
        metaclass = &mParentUserObject.getClass();
        ptr = mParentUserObject.pointer();
    }
    else
    {
        metaclass = &mUserObject.getClass();
        ptr = mUserObject.pointer();
    }

    try
    {
        const camp::Class& componentClass = camp::classByType<Component>();
        const camp::Class& objectClass = camp::classByType<Object>();
        if( PropertyChangeCommand::hasBase( *metaclass, componentClass ) )
        {
            Component* component = static_cast<Component*>( metaclass->applyOffset( ptr, 
                componentClass ) );
            component->connectDestruction( sigc::mem_fun( this, 
                &PropertyChangeCommand::componentDestroyed ) );
        }
        else if( PropertyChangeCommand::hasBase( *metaclass, objectClass ) )
        {
            Object* object = static_cast<Object*>( metaclass->applyOffset( ptr, 
                objectClass ) );
            object->connectDestruction( sigc::mem_fun( this, 
                &PropertyChangeCommand::objectDestroyed ) );
        }
    }
    catch( camp::Error e )
    {
    	LCLOGC << "Could not connect to destruction signal in PropertyChangeCommand: " << e.what();
    }
}

void PropertyChangeCommand::componentDestroyed( Component& rComponent )
{
    UndoCommand::remove();
}

void PropertyChangeCommand::objectDestroyed( Object& rObject )
{
    UndoCommand::remove();
}

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia