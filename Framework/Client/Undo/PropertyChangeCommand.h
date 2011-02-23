/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_CLIENT_PROPERTYCHANGECOMMAND_H
#define DIVERSIA_CLIENT_PROPERTYCHANGECOMMAND_H

#include "Client/Platform/Prerequisites.h"

#include "Client/Undo/UndoCommand.h"

namespace Diversia
{
namespace Client
{
//------------------------------------------------------------------------------

class PropertyChangeCommand : public UndoCommand
{
public:
    PropertyChangeCommand( const String& rPropertyName, const camp::UserObject& rUserObject, 
        const camp::Value& rNewValue, const camp::Property* pParentProperty = 0, 
        const camp::UserObject& rParentObject = camp::UserObject::nothing );
    PropertyChangeCommand( const camp::Property& rProperty, const camp::UserObject& rUserObject, 
        const camp::Value& rNewValue, const camp::Property* pParentProperty = 0, 
        const camp::UserObject& rParentObject = camp::UserObject::nothing );
    
    inline int id() const { return 0; }
    bool mergeWith( const UndoCommand* pCommand );
    void redo();
    void undo();
    
private:
    static String name( const String& rPropertyName, const camp::Property* pParentProperty = 0 );
    String name();
    void storeCurrentValue();

    const camp::Property&   mProperty;
    camp::UserObject        mUserObject;
    const camp::Property*   mParentProperty;
    camp::UserObject        mParentUserObject;
    camp::Value             mRedoValue;
    camp::Value             mUndoValue;

};

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia

#endif // DIVERSIA_CLIENT_PROPERTYCHANGECOMMAND_H