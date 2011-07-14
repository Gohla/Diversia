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

#ifndef DIVERSIA_CLIENT_OBJECTCOMMAND_H
#define DIVERSIA_CLIENT_OBJECTCOMMAND_H

#include "Client/Platform/Prerequisites.h"

#include "Client/Undo/UndoCommand.h"

namespace Diversia
{
namespace Client
{
//------------------------------------------------------------------------------

class DIVERSIA_CLIENT_API ObjectCommand : public UndoCommand, public sigc::trackable
{
public:
    /**
    Constructor for object destruction.
    
    @param [in,out] rObject The object to destroy.
    **/
    ObjectCommand( Object& rObject );
    /**
    Constructor for object creation
    
    @param [in,out] rObjectManager  The object manager where the object should be created at.
    @param  rName                   The name of the object. 
    @param  type                    The (local/remote) type of the object, defaults to local. 
    @param  rDisplayName            The display name of the object, defaults to the name of the
                                    object. 
    @param  source                  Source of the construction request. Leave blank to use this
                                    system's GUID. 
    **/
    ObjectCommand( ObjectManager& rObjectManager, const String& rName, NetworkingType type = LOCAL,
        const String& rDisplayName = "", Object* pParentObject = 0, 
        RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ) );
    virtual ~ObjectCommand();

    inline int id() const { return 3; }
    bool mergeWith( const UndoCommand* pCommand );
    void redo();
    void undo();
    
private:
    void objectDestroyed( Object& rObject );
    void parentObjectDestroyed( Object& rObject );

    boost::scoped_ptr<SerializationStream>  mSerializationStream;
    Object*                                 mObject;
    Object*                                 mParentObject;
    ObjectManager&                          mObjectManager;
    String                                  mObjectName;
    NetworkingType                          mObjectNetworkingType;
    String                                  mObjectDisplayName;
    RakNet::RakNetGUID                      mObjectSource;
    sigc::connection                        mObjectConnection;

};

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia

#endif // DIVERSIA_CLIENT_OBJECTCOMMAND_H