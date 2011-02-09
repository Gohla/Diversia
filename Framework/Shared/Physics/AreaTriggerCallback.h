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

#ifndef DIVERSIA_SHARED_AREATRIGGERCALLBACK_H
#define DIVERSIA_SHARED_AREATRIGGERCALLBACK_H

#include "Shared/Platform/Prerequisites.h"

#include "Object/Component.h"
#include "Object/Object.h"
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>

namespace Diversia
{
//------------------------------------------------------------------------------

template <typename T>
class DIVERSIA_SHARED_API AreaTriggerCallback : public btGhostPairCallback
{
public:
    AreaTriggerCallback( btDynamicsWorld* pDynamicsWorld ): mDynamicsWorld( pDynamicsWorld )
    {
        mDynamicsWorld->getBroadphase()->getOverlappingPairCache()-> 	 
            setInternalGhostPairCallback( this );
    }
    virtual ~AreaTriggerCallback()
    {
        mDynamicsWorld->getBroadphase()->getOverlappingPairCache()-> 	 
            setInternalGhostPairCallback( NULL );
    }

    btBroadphasePair* addOverlappingPair( btBroadphaseProxy* pProxy0, btBroadphaseProxy* pProxy1 )
    {
        btBroadphasePair* pair = btGhostPairCallback::addOverlappingPair( pProxy0, pProxy1 );

        T* areaTrigger;
        Object* object;

        Component* componentA = static_cast<Component*>( static_cast<btCollisionObject*>( 
            pProxy0->m_clientObject )->getUserPointer() );
        Component* componentB = static_cast<Component*>( static_cast<btCollisionObject*>( 
            pProxy1->m_clientObject )->getUserPointer() );

        if( componentA && componentB )
        {
            if( componentA->getType() != componentB->getType() )
            {
                switch( componentA->getType() )
                {
                    case COMPONENTTYPE_RIGIDBODY: 
                        object = &componentA->getObject();
                        break;
                    case COMPONENTTYPE_AREATRIGGER: 
                        areaTrigger = static_cast<T*>( componentA ); 
                        break;
                }
                switch( componentB->getType() )
                {
                    case COMPONENTTYPE_RIGIDBODY: 
                        object = &componentB->getObject();
                        break;
                    case COMPONENTTYPE_AREATRIGGER: 
                        areaTrigger = static_cast<T*>( componentB ); 
                        break;
                }

                areaTrigger->enterArea( *object );
            }
        }

        return pair;
    }
    void* removeOverlappingPair( btBroadphaseProxy* pProxy0, btBroadphaseProxy* pProxy1, 
        btDispatcher* pDispatcher )
    {
        T* areaTrigger;
        Object* object;

        Component* componentA = static_cast<Component*>( static_cast<btCollisionObject*>( 
            pProxy0->m_clientObject )->getUserPointer() );
        Component* componentB = static_cast<Component*>( static_cast<btCollisionObject*>( 
            pProxy1->m_clientObject )->getUserPointer() );

        if( componentA && componentB )
        {
            if( componentA->getType() != componentB->getType() )
            {
                switch( componentA->getType() )
                {
                    case COMPONENTTYPE_RIGIDBODY: 
                        object = &componentA->getObject();
                        break;
                    case COMPONENTTYPE_AREATRIGGER: 
                        areaTrigger = static_cast<T*>( componentA ); 
                        break;
                }
                switch( componentB->getType() )
                {
                    case COMPONENTTYPE_RIGIDBODY: 
                        object = &componentB->getObject();
                        break;
                    case COMPONENTTYPE_AREATRIGGER: 
                        areaTrigger = static_cast<T*>( componentB ); 
                        break;
                }

                areaTrigger->leaveArea( *object );
            }
        }

        return btGhostPairCallback::removeOverlappingPair( pProxy0, pProxy1, pDispatcher );
    }

private:
    btDynamicsWorld* mDynamicsWorld;

};

//------------------------------------------------------------------------------
} // Namespace Diversia

#endif // DIVERSIA_SHARED_AREATRIGGERCALLBACK_H