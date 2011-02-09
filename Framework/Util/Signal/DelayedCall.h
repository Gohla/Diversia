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

#ifndef DIVERSIA_UTIL_DELAYEDCALL_H
#define DIVERSIA_UTIL_DELAYEDCALL_H

#include "Util/Platform/Prerequisites.h"

namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------

/**
Calls a slot after given time has elapsed. Use DelayedCall::create to created a delayed call.
**/
class DIVERSIA_UTIL_API DelayedCall : public sigc::trackable
{
public:
    /**
    Sets the update signal.

    @param [in,out] rUpdateSignal   The update signal.
    **/
    inline static void setUpdateSignal( sigc::signal<void>& rUpdateSignal )
    {
        mUpdateSignal = &rUpdateSignal;
    }
    /**
    Creates a delayed call that calls a slot after given time has elapsed,

    @param [in,out] rSlot   The slot to call.
    @param  timeSeconds     After how much seconds the slot must be called.
    **/
    static void create( const sigc::slot<void>& rSlot, Real timeSeconds );

private:
    DelayedCall();
    DelayedCall( const sigc::slot<void>& rSlot, Real timeSecond );
    ~DelayedCall();

    void update();

    static sigc::signal<void>* mUpdateSignal;

    sigc::slot<void>    mSlot;
    Real                mCallTime;
    boost::timer        mTimer;
};

//------------------------------------------------------------------------------
} // Namespace Util
} // Namespace Diversia

#endif // DIVERSIA_UTIL_DELAYEDCALL_H