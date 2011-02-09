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

#include "Util/Job/Job.h"
#include "Util/Job/Request.h"

namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------

Job::Job( Request& rRequest, const sigc::slot<void,bool>& rFinishedSlot ):
    mRequest( rRequest ),
    mFinishedSlot( rFinishedSlot ),
    mJobState( Job::JOB_IDLE ),
    mRewindJob( 0 ),
    mTimeout( 3000 ),
    mRetries( 2 ),
    mRetriesCounter( 0 ),
    mLaterTimer( 0 ),
    mLaterRetries( 10 ),
    mLaterRetriesCounter( 0 ),
    mIsCritical( true )
{

}

Job::Job( Request& rRequest ):
    mRequest( rRequest ),
    mFinishedSlot( sigc::slot<void,bool>() ),
    mJobState( Job::JOB_IDLE ),
    mRewindJob( 0 ),
    mTimeout( 3000 ),
    mRetries( 2 ),
    mRetriesCounter( 0 ),
    mLaterTimer( 0 ),
    mLaterRetries( 10 ),
    mLaterRetriesCounter( 0 ),
    mIsCritical( true )
{

}

Job::~Job()
{
    if( mRewindJob ) delete mRewindJob;
}

Job::JobState Job::run()
{
    return Job::JOB_FAILED;
}

Job::JobState Job::runLater()
{
    return Job::JOB_FAILED;
}

Job::JobState Job::getState() const
{
    return mJobState;
}

Job::JobState Job::rewind()
{
    if ( mRewindJob )
    {
        return mRewindJob->run();
    }
    else
    {
        return Job::JOB_REWINDING_DONE;
    }

    return Job::JOB_REWINDING;
}

Job* Job::getRewindJob() const
{
    return mRewindJob;
}

void Job::setRewindJob( Job* pJob )
{
    if ( mRewindJob )
    {
        delete mRewindJob;
    }

    mRewindJob = pJob;
}

unsigned int Job::getTimeout() const
{
    return mTimeout;
}

void Job::setTimeout( unsigned int timeout )
{
    mTimeout = timeout;
}

unsigned int Job::getTimeoutCounter()
{
    return 0;
}

int Job::getRetries() const
{
    return mRetries;
}

void Job::setRetries( int retries )
{
    mRetries = retries;
}

int Job::getRetriesCounter() const
{
    return mRetriesCounter;
}

int Job::getLaterRetries() const
{
    return mLaterRetries;
}

void Job::setLaterRetries( int laterRetries )
{
    mLaterRetries = laterRetries;
}

int Job::getLaterRetriesCounter() const
{
    return mLaterRetriesCounter;
}

unsigned int Job::getLaterTimer() const
{
    return mLaterTimer;
}

void Job::setLaterTimer( unsigned int laterTimer )
{
    mLaterTimer = laterTimer;
}

bool Job::isCritical() const
{
    return mIsCritical;
}

void Job::setIsCritical( bool critical )
{
    mIsCritical = critical;
}

void Job::done()
{
    mFinishedSlot( true );
}

void Job::failed()
{
    mFinishedSlot( false );
}

//------------------------------------------------------------------------------
}
}
