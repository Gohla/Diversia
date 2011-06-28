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

#ifndef DIVERSIA_UTIL_REQUEST_H
#define DIVERSIA_UTIL_REQUEST_H

namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------

/**
 * A request holds one or more jobs that get executed sequentially.
**/
class DIVERSIA_UTIL_API Request
{
public:
    /**
     * Request state enumeration.
    **/
    enum RequestState
    {
        REQUEST_IDLE,
        REQUEST_WORKING,
        REQUEST_WAITING,
        REQUEST_REWINDING,
        REQUEST_DONE,
        REQUEST_REWINDING_DONE,
    };

    /**
     * Constructor with slot.
     *
     * @param [in,out]  pRequestManager The RequestManager that owns this Request.
     * @param   priority                The priority this request should have.
     * @param   rFinishedSlot           Slot ( void func(bool [false when request fails]) ) that
     *                                  is executed when request has finished.
    **/
    Request( RequestManager* pRequestManager, unsigned int priority,
        const sigc::slot<void,bool>& rFinishedSlot );
    /**
     * Constructor.
     *
     * @param [in,out]  pRequestManager The RequestManager that owns this Request.
     * @param   priority                The priority this request should have.
    **/
    Request( RequestManager* pRequestManager, unsigned int priority );
    /**
     * Destructor.
    **/
    ~Request();
    /**
     * Runs the job at the top of the queue and returns the result.
    **/
    RequestState run();
    /**
     * Add a job to the queue.
     *
     * @param [in,out]  pJob    The job to add.
    **/
    void addJob( Job* pJob );
    /**
     * Gets the priority for this Request.
    **/
    unsigned int getPriority() const;
    /**
     *  Compares priority with another request.
    **/
    bool operator<( const Request& rRequest );
    /**
     * Rewinds the changes made by all completed jobs, calls the failed event and returns the
     * result.
    **/
    RequestState rewind();
    /**
     * Calls the done event.
    **/
    void done();

    /**
     * Sets finished slot ( void func(bool [false when request fails]) )
     *
     * @param   rFinishedSlot   The slot to set.
    **/
    inline void setFinishedSlot( const sigc::slot<void,bool>& rFinishedSlot ) { mFinishedSlot = rFinishedSlot; }

private:
    RequestState            mRequestState;
    std::queue<Job*>        mJobs;
    std::stack<Job*>        mJobStack;
    unsigned int            mPriority;
    RequestManager*         mRequestManager;
    sigc::slot<void,bool>   mFinishedSlot;
};

//------------------------------------------------------------------------------
}
}

#endif