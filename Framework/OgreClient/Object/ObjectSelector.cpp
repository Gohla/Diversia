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

#include "OgreClient/Platform/StableHeaders.h"

#include "Client/Object/ClientObject.h"
#include "Client/Object/ClientObjectManager.h"
#include "OgreClient/Input/ObjectSelection.h"
#include "OgreClient/Object/ObjectSelector.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

ObjectSelector::ObjectSelector( ClientObjectManager& rObjectManager ):
    mObjectManager( rObjectManager )
{
    mObjectManager.connect( sigc::mem_fun( this, &ObjectSelector::objectChange ) );
}

ObjectSelector::~ObjectSelector()
{

}

void ObjectSelector::objectChange( Object& rObject, bool created )
{
    ClientObject& object = static_cast<ClientObject&>( rObject );

    if( created )
    {
        GlobalsBase::mSelection->setSelectSlot( rObject, 
            sigc::bind( sigc::mem_fun( this, &ObjectSelector::objectSelected ), sigc::ref( 
            object ) ), true );
    }
    else 
    {
        GlobalsBase::mSelection->deselect( rObject );
        GlobalsBase::mSelection->removeSelectSlot( rObject );
    }
}

void ObjectSelector::objectSelected( bool select, ClientObject& rObject )
{
    rObject.setSelected( select );
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia