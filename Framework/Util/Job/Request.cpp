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

#include "Util/Job/RequestManager.h"
#include "Util/Job/Request.h"
#include "Util/Job/Job.h"

namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------

Request::Request( RequestManager* pRequestManager, unsigned int priority, 
    const sigc::slot<void,bool>& rFinishedSlot ):
    mRequestManager( pRequestManager ),
    mPriority( priority ),
    mFinishedSlot( rFinishedSlot ),
    mRequestState( Request::REQUEST_IDLE )
{

}

Request::Request( RequestManager* pRequestManager, unsigned int priority ):
    mRequestManager( pRequestManager ),
    mPriority( priority ),
    mFinishedSlot( sigc::slot<void,bool>() ),
    mRequestState( Request::REQUEST_IDLE )
{

}

Request::~Request()
{
    // Remove any Jobs from queue and stack.
    while ( mJobs.size() > 0 )
    {
        Job* job = mJobs.front();
        mJobs.pop();
        delete job;
    }

    while ( mJobStack.size() > 0 )
    {
        Job* job = mJobStack.top();
        mJobStack.pop();
        delete job;
    }
}

Request::RequestState Request::run()
{
    if ( mRequestState == Request::REQUEST_REWINDING )
    {
        return rewind();
    }

    if ( mJobs.size() > 0 )
    {
        Job::JobState jobState = mJobs.front()->run();
        switch ( jobState )
        {
            case Job::JOB_WORKING:
            {
                mRequestState = Request::REQUEST_WORKING;
                break;
            }
            case Job::JOB_WAITING:
            {
                mRequestState = Request::REQUEST_WAITING;
                break;
            }
            case Job::JOB_IDLE:
            {
                mRequestState = Request::REQUEST_IDLE;
                break;
            }
            case Job::JOB_DONE:
            {
                // Is this not the last Job?
                if ( mJobs.size() > 1 )
                {
                    // Put the finished Job on the stack and remove it from the queue.
                    mJobStack.push( mJobs.front() );
                    mJobs.pop();

                    mRequestState = Request::REQUEST_IDLE;
                }
                else
                {
                    // This is the last Job and it is done, request is done!
                    // Put the finished Job on the stack and remove it from the queue.
                    mJobStack.push( mJobs.front() );
                    mJobs.pop();

                    mRequestState = Request::REQUEST_DONE;
                }

                break;
            }
            case Job::JOB_FAILED:
            {
                // Damn! A Job has failed..
                // Is this Job critical for the success of this Request?
                if ( mJobs.front()->isCritical() )
                {
                    // Put the failed Job on the stack and remove it from the queue.
                    mJobStack.push( mJobs.front() );
                    mJobs.pop();

                    // Remove remaining Jobs from the queue
                    while ( mJobs.size() > 0 )
                    {
                        Job* job = mJobs.front();
                        mJobs.pop();
                        delete job;
                    }

                    // Set state to rewinding so the next run() we can start rewinding.
                    mRequestState = Request::REQUEST_REWINDING;
                }
                else
                {
                    // Phew.. just add the Job to the 'later' list of the RequestManager
                    mRequestManager->addLaterJob( mJobs.front() );

                    // Is this the last Job?
                    if ( mJobs.size() == 1 )
                    {
                        // Don't add it to the stack because the RequestManager now owns it,
                        // this might cause problems if a Request needs to be rewinded.
                        mJobs.pop();

                        // Set state to done.
                        mRequestState = Request::REQUEST_DONE;
                    }

                }
                break;
            }
        }
    }

    return mRequestState;
}

void Request::addJob( Job* pJob )
{
    mJobs.push( pJob );
}

unsigned int Request::getPriority() const
{
    return mPriority;
}

bool Request::operator<( const Request& rRequest )
{
    return mPriority < rRequest.getPriority();
}

Request::RequestState Request::rewind()
{
    // Rewind remaining Jobs from the stack.
    if ( mJobStack.size() > 0 )
    {
        Job::JobState jobState = mJobStack.top()->rewind();
        switch ( jobState )
        {
            case Job::JOB_REWINDING:
            {
                mRequestState = Request::REQUEST_REWINDING;
                break;
            }
            case Job::JOB_REWINDING_DONE:
            {
                mJobStack.pop();
            }
        }
    }
    else
    {
        // Rewinding done
        mRequestState = Request::REQUEST_REWINDING_DONE;

        // Send the fail event if an event was given.
        mFinishedSlot( false );
    }

    return mRequestState;
}

void Request::done()
{
    // Send success event with stack of Jobs, these Jobs will be deleted after
    // the event is done so the Jobs cannot be stored!
    mFinishedSlot( true );

    // Remove remaining Jobs from the stack.
    while ( mJobStack.size() > 0 )
    {
        Job* job = mJobStack.top();
        mJobStack.pop();
        delete job;
    }
}

//------------------------------------------------------------------------------
}
}
