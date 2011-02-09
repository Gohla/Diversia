/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

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

#ifndef DIVERSIA_SHARED_REPLICACONNECTION_H
#define DIVERSIA_SHARED_REPLICACONNECTION_H

#include "Shared/Platform/Prerequisites.h"

namespace Diversia
{
//------------------------------------------------------------------------------

class DIVERSIA_SHARED_API ReplicaConnection : public RakNet::Connection_RM3
{
public:
    ReplicaConnection( RakNet::SystemAddress systemAddress, RakNet::RakNetGUID guid, 
        ObjectManager* pObjectManager, ClientServerPluginManager& rPluginManager );

    RakNet::Replica3* AllocReplica( RakNet::BitStream* pAllocationIdBitstream, 
        RakNet::ReplicaManager3* pReplicaManager3 );

    inline void setObjectManager( ObjectManager& rObjectManager ) { mObjectManager = &rObjectManager; }

private:
    ObjectManager*              mObjectManager;
    ClientServerPluginManager&  mPluginManager;
};

//------------------------------------------------------------------------------
} // Namespace Diversia

#endif
