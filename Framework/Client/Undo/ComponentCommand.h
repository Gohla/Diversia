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

#ifndef DIVERSIA_CLIENT_COMPONENTCOMMAND_H
#define DIVERSIA_CLIENT_COMPONENTCOMMAND_H

#include "Client/Platform/Prerequisites.h"

#include "Client/Undo/UndoCommand.h"

namespace Diversia
{
namespace Client
{
//------------------------------------------------------------------------------

class DIVERSIA_CLIENT_API ComponentCommand : public UndoCommand, public sigc::trackable
{
public:
    /**
    Constructor for component destruction.
    
    @param [in,out] rComponent  The component to destroy.
    **/
    ComponentCommand( Component& rComponent );
    /**
    Constructor for component creation.
    
    @param [in,out] rObject The object on which the component should be created. 
    @param  type            The component type. 
    @param  rName           The name of the component, must be unique within the object. 
    @param  localOverride   True to override component to local mode even if the object is a
                            remote object. 
    @param  source          Source of the construction request. Defaults to this system. 
    **/
    ComponentCommand( Object& rObject, ComponentType type, const String& rName, 
        bool localOverride = false, RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ) );
    virtual ~ComponentCommand();

    inline int id() const { return 2; }
    bool mergeWith( const UndoCommand* pCommand );
    void redo();
    void undo();
    
private:
    void objectDestroyed( Object& rObject );

    boost::scoped_ptr<SerializationStream>  mSerializationStream;
    Component*                              mComponent;
    ComponentType                           mComponentType;
    String                                  mComponentName;
    bool                                    mComponentLocalOverride;
    RakNet::RakNetGUID                      mComponentSource;
    Object&                                 mObject;

};

//------------------------------------------------------------------------------
} // Namespace Client
} // Namespace Diversia

#endif // DIVERSIA_CLIENT_COMPONENTCOMMAND_H