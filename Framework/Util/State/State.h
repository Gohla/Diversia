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

#ifndef DIVERSIA_UTIL_STATE_H
#define DIVERSIA_UTIL_STATE_H

#include "Util/State/StateMachine.h"

namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------

/**
 * Abstract state class that defines a simple state that can be started or stopped.
**/
class DIVERSIA_UTIL_API State
{
protected:
    friend class StateMachine;

    /**
     * Destructor.
    **/
    virtual ~State() {}
    /**
     * Starts the state, called once when the state is added to the FSM.
    **/
    virtual void start() = 0;
    /**
    * Suspends the state. Called when a new state is added to the FSM. This should stop the state
    * but keep the objects for later use because newer states may depend on these.
    **/
    virtual void suspend() = 0;
    /**
    * Wakes the state from suspension. Called when a state is popped from the FSM. This should re
    * enable the state as it was again when start() was called.
    **/
    virtual void wake() = 0;
};

//------------------------------------------------------------------------------
}
}

#endif