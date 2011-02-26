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

#include "Client/Platform/StableHeaders.h"

#include "Client/Undo/ComponentCommand.h"
#include "Object/Component.h"
#include "Object/Object.h"
#include "Util/Serialization/XMLSerializationFile.h"

namespace Diversia
{
namespace Client 
{
//------------------------------------------------------------------------------

ComponentCommand::ComponentCommand( Component& rComponent ):
UndoCommand( "Destroyed " + rComponent.getName() + " component" ),
    mSerializationFile( new XMLSerializationFile( "", "NoSerialization", false, true ) ),
    mComponent( &rComponent ),
    mComponentType( rComponent.getType() ),
    mComponentName( rComponent.getName() ),
    mComponentLocalOverride( rComponent.getLocalOverride() ),
    mComponentSource( rComponent.getSource() ),
    mObject( rComponent.getObject() )
{
    try
    {
        mSerializationFile->serialize( rComponent, false );
    }
    catch( Exception e )
    {
        LCLOGE << "Could not serialize component in ComponentCommand: " << e.what();
    }

    mObject.connectDestruction( sigc::mem_fun( this, &ComponentCommand::objectDestroyed ) );
}

ComponentCommand::ComponentCommand( Object& rObject, ComponentType type, 
    const String& rName, bool localOverride /*= false*/, 
    RakNet::RakNetGUID source /*= RakNet::RakNetGUID( 0 )*/ ):
    UndoCommand( "Created " + rName + " component" ),
    mSerializationFile( 0 ),
    mComponent( 0 ),
    mComponentType( type ),
    mComponentName( rName ),
    mComponentLocalOverride( localOverride ),
    mComponentSource( source ),
    mObject( rObject )
{
    mObject.connectDestruction( sigc::mem_fun( this, &ComponentCommand::objectDestroyed ) );
}

ComponentCommand::~ComponentCommand()
{

}

bool ComponentCommand::mergeWith( const UndoCommand* pCommand )
{
    return false;
}

void ComponentCommand::redo()
{
    if( !mSerializationFile )
    {
        try
        {
            if( !mComponent ) mComponent = &mObject.createComponent( mComponentType, mComponentName, 
                mComponentLocalOverride, mComponentSource );
        }
        catch( Exception e )
        {
            LCLOGE << "Could not create component in ComponentCommand: " << e.what();
        }
    }
    else
    {
        try
        {
            if( mComponent ) mComponent->destroyComponent();
            mComponent = 0;
        }
        catch( Exception e )
        {
            LCLOGE << "Could not destroy component in ComponentCommand: " << e.what();
        }
    }
}

void ComponentCommand::undo()
{
    if( !mSerializationFile )
    {
        try
        {
            if( mComponent ) mComponent->destroyComponent();
            mComponent = 0;
        }
        catch( Exception e )
        {
            LCLOGE << "Could not destroy component in ComponentCommand: " << e.what();
        }
    }
    else 
    {
        if( !mComponent ) 
        {
            try
            {
                mComponent = &mObject.createComponent( mComponentType, mComponentName, 
                    mComponentLocalOverride, mComponentSource );
                mSerializationFile->deserialize( mComponent, false );
            }
            catch( Exception e )
            {
                LCLOGE << "Could not create and deserialize component in ComponentCommand: " << 
                    e.what();
            }
        }
    }
}

void ComponentCommand::objectDestroyed( Object& rObject )
{
    UndoCommand::remove();
}

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia