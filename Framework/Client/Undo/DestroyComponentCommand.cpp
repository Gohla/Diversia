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

#include "Client/Undo/DestroyComponentCommand.h"
#include "Object/Component.h"
#include "Util/Serialization/XMLSerializationFile.h"

namespace Diversia
{
namespace Client 
{
//------------------------------------------------------------------------------

DestroyComponentCommand::DestroyComponentCommand( Component& rComponent ):
    UndoCommand( "Destroyed " + rComponent.getName() + " component" ),
    mSerializationFile( new XMLSerializationFile( "", "NoSerialization", false, true ) ),
    mComponent( &rComponent ),
    mComponentName( rComponent.getName() ),
    mComponentType( rComponent.getType() ),
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
        LCLOGC << "Could not serialize component in DestroyComponentCommand: " << e.what();
    }
}

DestroyComponentCommand::~DestroyComponentCommand()
{

}

bool DestroyComponentCommand::mergeWith( const UndoCommand* pCommand )
{
    return false;
}

void DestroyComponentCommand::redo()
{
    if( mComponent ) mComponent->destroyComponent();
    mComponent = 0;
}

void DestroyComponentCommand::undo()
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
            LCLOGC << "Could not deserialize component in DestroyComponentCommand: " << e.what();
        }
    }
}

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia