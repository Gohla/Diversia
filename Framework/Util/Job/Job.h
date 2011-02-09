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

#ifndef DIVERSIA_UTIL_JOB_H
#define DIVERSIA_UTIL_JOB_H

#include "Util/Platform/Prerequisites.h"

namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------

/**
 * Job base class, derive from this class to make jobs that can be executed.
**/
class DIVERSIA_UTIL_API Job
{
public:
    /**
     * Job state enumeration.
    **/
    enum JobState
    {
        JOB_IDLE,
        JOB_WORKING,
        JOB_WAITING,
        JOB_FAILED,
        JOB_LATER,
        JOB_REWINDING,
        JOB_REWINDING_DONE,
        JOB_DONE
    };

    /**
     * Constructor with slot.
     *
     * @param [in,out]  rRequest    The Request object that owns the Job.
     * @param   rFinishedSlot       Slot (signature: void func(bool [false when job fails])) that is
     *                              executed when job has finished.
    **/
    Job( Request& rRequest, const sigc::slot<void,bool>& rFinishedSlot );
    /**
     * Constructor.
     *
     * @param [in,out]  rRequest    The Request object that owns the Job.
    **/
    Job( Request& rRequest );
    /**
     * Destructor.
    **/
    virtual ~Job();
    /**
     * Runs the job and returns the state.
     *
     * @remarks
     *          This method must be overridden to do something with a job, by default it does
     *          nothing and returns Job::JOB_FAILED.
    **/
    virtual JobState run();

    /**
     * Runs the job at a later time and returns the state. If this job fails in the request but
     * the rest of the request does not depend on this job, it will be added to the
     * RequestManager's 'later jobs'. The RequestManager will call runLater after the period of
     * time that is stored in mLaterTimer is passed. It will keep calling this until it returns
     * JOB::JOB_DONE or if it has retried mLaterRetries times.
     *
     * @remarks
     *          This method must be overridden to do something with a Job, by default it does
     *          nothing and returns Job::JOB_FAILED.
    **/
    virtual JobState runLater();
    /**
     * Gets the state of this job.
    **/
    virtual JobState getState() const;
    /**
     * Rewind the state of this job, This is called if a Job later in the Request fails but this
     * Job was done. It should rewind all the actions that it has done in run().
     *
     * @remarks
     *          Will run mRewindJob if it is set. It will run this job in the same fashion as a
     *          request. If the rewind job fails this will cause undefined behavior.
     *
     * @return  The state of the rewind job, by default returns Job::JOB_FAILED if no rewind Job
     *          is set.
    **/
    virtual JobState rewind();
    /**
     * Gets the rewind job.
    **/
    virtual Job* getRewindJob() const;
    /**
     * Sets the rewind job. Will delete the old rewind Job if there was one.
     *
     * @param [in,out]  pJob    The new rewind job for this job.
    **/
    virtual void setRewindJob( Job* pJob );
    /**
     * Query if this object is critical. Critical means that the request fails if this job fails.
    **/
    virtual bool isCritical() const;
    /**
     * Sets if this object is critical. Critical means that the request fails if this job fails.
     *
     * @param   critical    True to set to critical.
    **/
    virtual void setIsCritical( bool critical );
    /**
     * Gets the timeout. Timeout is the amount of times a job will stay in JOB_WAIT before doing
     * a retry or failing.
    **/
    virtual unsigned int getTimeout() const;
    /**
     * Sets the timeout. Timeout is the amount of times a job will stay in JOB_WAIT before doing
     * a retry or failing.
    **/
    virtual void setTimeout( unsigned int timeout );
    /**
     * Gets the timeout counter. Timeout is the amount of times a job will stay in JOB_WAIT before
     * doing a retry or failing.
    **/
    virtual unsigned int getTimeoutCounter();
    /**
     * Gets the amount of retries this job will do.
    **/
    virtual int getRetries() const;
    /**
     * Sets the amount of retries this job will do.
    **/
    virtual void setRetries( int retries );
    /**
     * Gets the amount of retries this job has done.
    **/
    virtual int getRetriesCounter() const;
    /**
     * Gets the amount of retries this job will do in 'later' mode.
    **/
    virtual int getLaterRetries() const;
    /**
     * Sets the amount of retries this job will do in 'later' mode.
    **/
    virtual void setLaterRetries( int laterRetries );
    /**
     * Gets the amount of retries this job has done in 'later' mode.
    **/
    virtual int getLaterRetriesCounter() const;
    /**
     * Gets the amount of time to wait to run this job in 'later' mode.
    **/
    virtual unsigned int getLaterTimer() const;
    /**
     * Sets the amount of time to wait to run this job in 'later' mode.
    **/
    virtual void setLaterTimer( unsigned int laterTimer );
    /**
     * Called when job is done.
    **/
    virtual void done();
    /**
     * Called when job has failed.
    **/
    virtual void failed();

    /**
     * Sets finished slot ( void func(bool [false when job fails]) )
     *
     * @param   rFinishedSlot   The slot to set.
    **/
    inline void setFinishedSlot( const sigc::slot<void,bool>& rFinishedSlot ) { mFinishedSlot = rFinishedSlot; }

protected:
    JobState                mJobState;
    Request&                mRequest;
    Job*                    mRewindJob;
    sigc::slot<void,bool>   mFinishedSlot;

    unsigned int            mTimeout;
    int                     mRetries;
    int                     mRetriesCounter;
    unsigned int            mLaterTimer;
    int                     mLaterRetries;
    int                     mLaterRetriesCounter;

    bool                    mIsCritical;
};

//------------------------------------------------------------------------------
}
}

#endif