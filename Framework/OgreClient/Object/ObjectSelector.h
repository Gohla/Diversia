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

#ifndef DIVERSIA_OGRECLIENT_OBJECTSELECTOR_H
#define DIVERSIA_OGRECLIENT_OBJECTSELECTOR_H

#include "OgreClient/Platform/Prerequisites.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

/**
Provides selection using ObjectSelection for the object system. 
**/
class DIVERSIA_OGRECLIENT_API ObjectSelector
{
public:
    /**
    Constructor. 
    
    @param [in,out] rObjectManager  The object manager to provide selection for.
    **/
    ObjectSelector( ClientObjectManager& rObjectManager );
    /**
    Destructor. 
    **/
    ~ObjectSelector();
    
private:
    void objectChange( Object& rObject, bool created );
    void objectSelected( bool select, ClientObject& rObject );

    ClientObjectManager& mObjectManager;

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

#endif // DIVERSIA_OGRECLIENT_OBJECTSELECTOR_H