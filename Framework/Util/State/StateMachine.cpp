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

#include "Util/Platform/StableHeaders.h"

#include "Util/State/StateMachine.h"
#include "Util/State/State.h"

namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------

StateMachine::StateMachine()
{

}

StateMachine::~StateMachine()
{
    stop();
}

void StateMachine::pushState( State* pState )
{
    if ( mStates.size() > 0 )
    {
        mStates.top()->suspend();
    }
    mStates.push( pState );
    pState->start();
}

void StateMachine::popState()
{
    if ( mStates.size() > 0 )
    {
        delete mStates.top();
        mStates.pop();
        if ( mStates.size() > 0 )
        {
            mStates.top()->wake();
        }
    }
}

State* StateMachine::getState()
{
    if ( mStates.size() > 0 )
    {
        return mStates.top();
    }

    return 0;
}

void StateMachine::stop()
{
    popTo( 0 );
}

void StateMachine::popTo( unsigned int size )
{
    while ( mStates.size() > size )
    {
        popState();
    }
}

void StateMachine::popToState( State* pState )
{
    while ( mStates.size() > 0 )
    {
        if ( mStates.top() == pState )
        {
            break;
        }

        popState();
    }
}

//------------------------------------------------------------------------------
} // namespace Util
} // namespace Diversia
