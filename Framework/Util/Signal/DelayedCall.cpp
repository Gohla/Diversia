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

#include "Util/Platform/StableHeaders.h"

#include "Util/Signal/DelayedCall.h"
#include <boost/timer.hpp>

namespace Diversia
{
namespace Util 
{
//------------------------------------------------------------------------------

sigc::signal<void>* DelayedCall::mUpdateSignal = 0;

void DelayedCall::create( const sigc::slot<void>& rSlot, Real timeSeconds )
{
    DivAssert( mUpdateSignal, "Update signal has not been set." );
    new DelayedCall( rSlot, timeSeconds );
}

DelayedCall::DelayedCall( const sigc::slot<void>& rSlot, Real timeSecond ):
    mSlot( rSlot ),
    mCallTime( timeSecond ),
    mTimer( new boost::timer() )
{
    mUpdateSignal->connect( sigc::mem_fun( this, &DelayedCall::update ) );
}

DelayedCall::DelayedCall():
    mTimer( new boost::timer() )
{

}

DelayedCall::~DelayedCall()
{

}

void DelayedCall::update()
{
    if( mTimer->elapsed() >= mCallTime )
    {
        mSlot();
        delete this;
    }
}

//------------------------------------------------------------------------------

} // Namespace Util
} // Namespace Diversia