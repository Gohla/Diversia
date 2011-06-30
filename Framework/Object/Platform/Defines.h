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

#ifndef DIVERSIA_OBJECT_DEFINES_H
#define DIVERSIA_OBJECT_DEFINES_H

namespace Diversia
{
namespace ObjectSystem
{
//------------------------------------------------------------------------------

// Typedefs
typedef unsigned char ReplicaType;
typedef unsigned char ComponentType;

// Enums
/**
Represents client or server mode.
**/
enum Mode
{
    CLIENT = 0,
    SERVER = 1
};

/**
Represents a source.
**/
typedef Mode Source;

/**
Represents local or remote mode.
**/
enum NetworkingType
{
    LOCAL = 0,
    REMOTE = 1
};

// Defines
// Replica type
#define REPLICATYPE_OBJECT                  0x00
#define REPLICATYPE_COMPONENT               0x01
//#define REPLICATYPE_PLUGIN    0x02
#define REPLICATYPE_OBJECTTEMPLATE          0x03
#define REPLICATYPE_COMPONENTTEMPLATE       0x04

//------------------------------------------------------------------------------
} // Namespace ObjectSystem
} // Namespace Diversia

CAMP_AUTO_TYPE( Diversia::ObjectSystem::Mode, 
    &Diversia::ObjectSystem::Bindings::CampBindings::bindModeEnum );
CAMP_AUTO_TYPE( Diversia::ObjectSystem::NetworkingType, 
    &Diversia::ObjectSystem::Bindings::CampBindings::bindNetworkingTypeEnum );

#endif // DIVERSIA_OBJECT_DEFINES_H
