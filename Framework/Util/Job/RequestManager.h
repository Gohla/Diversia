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

#ifndef DIVERSIA_UTIL_REQUESTMANAGER_H
#define DIVERSIA_UTIL_REQUESTMANAGER_H

#include "Util/Platform/Prerequisites.h"

namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------

/**
 * Manages a queue of non threaded requests.
**/
class DIVERSIA_UTIL_API RequestManager
{
public:
    /**
     * Default constructor.
    **/
    RequestManager();
    /**
     * Destructor.
    **/
    ~RequestManager();
    /**
     * Adds a request to the list
     *
     * @param [in,out]  pRequest    The request to add.
    **/
    void addRequest( Request* pRequest );
    /**
     * Adds a job to the queue of jobs that get executed again at a later time.
     *
     * @param [in,out]  pJob    The job to add.
    **/
    void addLaterJob( Job* pJob );
    /**
     * Runs as much requests as possible.
    **/
    void update();
    /**
     * Removes all requests.
    **/
    void removeAll();

private:
    std::list<Request*>                         mRequests;
    std::stack<std::list<Request*>::iterator>   mFinishedRequests;
    std::list<Job*>                             mLaterJobs;
};

//------------------------------------------------------------------------------
}
}

#endif