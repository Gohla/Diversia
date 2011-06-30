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

#ifndef DIVERSIA_SHARED_REPLICAMANAGER_H
#define DIVERSIA_SHARED_REPLICAMANAGER_H

#include "Shared/Platform/Prerequisites.h"

#include "Shared/Communication/ReplicaConnection.h"
#include "Shared/Plugin/PluginManager.h"

namespace Diversia
{
//------------------------------------------------------------------------------

class ReplicaManager : public RakNet::ReplicaManager3
{
public:
    ReplicaManager(): RakNet::ReplicaManager3(), mObjectManager( 0 ),
        mPluginManager( 0 ) {}
    virtual ~ReplicaManager() {}

    inline void setObjectManager( ObjectManager& rObjectManager )
    { 
        mObjectManager = &rObjectManager;
  
        if( mPluginManager && mPluginManager->getMode() == CLIENT )
        {
            for( DataStructures::DefaultIndexType i = 0; i < ReplicaManager3::GetConnectionCount();
                ++i )
            {
                static_cast<ReplicaConnection*>( ReplicaManager3::GetConnectionAtIndex( i ) 
                    )->setObjectManager( rObjectManager );
            }
        }
    }
    inline void setPluginManager( PluginManager& rPluginManager ) 
    { 
        mPluginManager = &rPluginManager; 
    }

    RakNet::Connection_RM3* AllocConnection( RakNet::SystemAddress systemAddress, RakNet::RakNetGUID guid ) const 
    {
        DivAssert( mPluginManager, "PluginManager not set." );
        return new ReplicaConnection( systemAddress, guid, mObjectManager, 
            *mPluginManager );
    }
    void DeallocConnection( RakNet::Connection_RM3* pConnection ) const 
    {
        delete pConnection;
    }

private:
    ObjectManager*              mObjectManager;
    PluginManager*  mPluginManager;

};

//------------------------------------------------------------------------------
} // Namespace Diversia

#endif