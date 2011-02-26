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

#include "Client/Undo/ObjectCommand.h"
#include "Object/Component.h"
#include "Object/Object.h"
#include "Object/ObjectManager.h"
#include "Util/Serialization/XMLSerializationFile.h"

namespace Diversia
{
namespace Client 
{
//------------------------------------------------------------------------------

ObjectCommand::ObjectCommand( Object& rObject ):
    UndoCommand( "Destroyed " + rObject.getDisplayName() + " object" ),
    mSerializationFile( new XMLSerializationFile( "", "NoSerialization", false, true ) ),
    mObject( &rObject ),
    mParentObject( 0 ),
    mObjectManager( rObject.getObjectManager() ),
    mObjectName( rObject.getName() ),
    mObjectNetworkingType( rObject.getNetworkingType() ),
    mObjectDisplayName( rObject.getDisplayName() ),
    mObjectSource( rObject.getSource() )
{
    try
    {
        mSerializationFile->serialize( rObject, false );
    }
    catch( Exception e )
    {
        LCLOGE << "Could not serialize object in ObjectCommand: " << e.what();
    }
}

ObjectCommand::ObjectCommand( ObjectManager& rObjectManager, const String& rName, 
    NetworkingType type /*= LOCAL*/, const String& rDisplayName /*= ""*/, 
    Object* pParentObject /*= 0*/, RakNet::RakNetGUID source /*= RakNet::RakNetGUID( 0 )*/ ):
    UndoCommand( "Destroyed " + ( rDisplayName.empty() ? rName : rDisplayName ) + " object" ),
    mSerializationFile( 0 ),
    mObject( 0 ),
    mParentObject( pParentObject ),
    mObjectManager( rObjectManager ),
    mObjectName( rName ),
    mObjectNetworkingType( type ),
    mObjectDisplayName( rDisplayName ),
    mObjectSource( source )
{
    if( mParentObject ) mParentObject->connectDestruction( sigc::mem_fun( this, 
        &ObjectCommand::parentObjectDestroyed ) );
}

ObjectCommand::~ObjectCommand()
{

}

bool ObjectCommand::mergeWith( const UndoCommand* pCommand )
{
    return false;
}

void ObjectCommand::redo()
{
    if( !mSerializationFile )
    {
        try
        {
            if( !mObject ) mObject = &mObjectManager.createObject( mObjectName, 
                mObjectNetworkingType, mObjectDisplayName, mObjectSource );
            mObjectConnection.disconnect();
            mObjectConnection = mObject->connectDestruction( sigc::mem_fun( this, 
                &ObjectCommand::objectDestroyed ) );
            if( mParentObject ) mObject->parent( mParentObject );
        }
        catch( Exception e )
        {
            LCLOGE << "Could not create object in ObjectCommand: " << e.what();
        }
    }
    else
    {
        try
        {
            mObjectConnection.disconnect();
            if( mObject ) mObject->destroyObject();
            mObject = 0;
        }
        catch( Exception e )
        {
            LCLOGE << "Could not destroy object in ObjectCommand: " << e.what();
        }
    }
}

void ObjectCommand::undo()
{
    if( !mSerializationFile )
    {
        try
        {
            mObjectConnection.disconnect();
            if( mObject ) mObject->destroyObject();
            mObject = 0;
        }
        catch( Exception e )
        {
            LCLOGE << "Could not destroy object in ObjectCommand: " << e.what();
        }
    }
    else 
    {
        if( !mObject ) 
        {
            try
            {
                if( !mObject ) mObject = &mObjectManager.createObject( mObjectName, 
                    mObjectNetworkingType, mObjectDisplayName, mObjectSource );
                mSerializationFile->deserialize( mObject, false );
                mObjectConnection.disconnect();
                mObjectConnection = mObject->connectDestruction( sigc::mem_fun( this, 
                    &ObjectCommand::objectDestroyed ) );
            }
            catch( Exception e )
            {
                LCLOGE << "Could not create and deserialize object in ObjectCommand: " << 
                    e.what();
            }
        }
    }
}

void ObjectCommand::objectDestroyed( Object& rObject )
{
    UndoCommand::remove();
}

void ObjectCommand::parentObjectDestroyed( Object& rObject )
{
    mParentObject = 0;
}

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia