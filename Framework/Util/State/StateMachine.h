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

#ifndef DIVERSIA_UTIL_STATEMACHINE_H
#define DIVERSIA_UTIL_STATEMACHINE_H

namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------

/**
 * A finite state machine using a stack to store states.
**/
class DIVERSIA_UTIL_API StateMachine
{
public:
    /**
     * Default constructor.
    **/
    StateMachine();
    /**
     * Destructor.
    **/
    virtual ~StateMachine();
    /**
     * Adds a state on top of the stack and starts it. The StateMachine will delete the state when it is
     * popped off the stack.
     *
     * @param [in,out]  pState  The state to add and execute.
    **/
    void pushState( State* pState );
    /**
     * Removes a state on top of the stack and stops it.
    **/
    void popState();
    /**
     * Gets the top state.
    **/
    State* getState();
    /**
     * Pops all states.
    **/
    void stop();
    /**
     * Pops states until size reaches given size.
     *
     * @param   size    Stack size to pop to.
    **/
    void popTo( unsigned int size );
    /** Pops states until state reaches given state
    @params pState  State to reach.
    @remarks
        This will do the same as the stop() function if a State is given that does
        not exist on the stack.
    */
    /**
     * Pops to state described by pState.
     *
     * @remarks
     *          This will do the same as the stop() function if a State is given that does not
     *          exist on the stack.
     *
     * @param [in,out]  pState  State to reach.
    **/
    void popToState( State* pState );

private:
    std::stack<State*> mStates;
};

//------------------------------------------------------------------------------
} // namespace Util
} // namespace Diversia

#endif // DIVERSIA_UTIL_STATEMACHINE_H