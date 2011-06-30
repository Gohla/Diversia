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

RequestManager::RequestManager()
{

}

RequestManager::~RequestManager()
{

}

void RequestManager::addRequest( Request* pRequest )
{
    mRequests.push_back( pRequest );
}

void RequestManager::addLaterJob( Job* pJob )
{
    mLaterJobs.push_back( pJob );
}

void RequestManager::update()
{
    // Run requests
    std::list<Request*>::iterator i;
    for ( i = mRequests.begin(); i != mRequests.end(); i++ )
    {
        Request::RequestState state = ( *i )->run();

        switch ( state )
        {
            case Request::REQUEST_DONE:
            {
                ( *i )->done();
                mFinishedRequests.push( i );
                break;
            }
            case Request::REQUEST_REWINDING_DONE:
            {
                mFinishedRequests.push( i );
                break;
            }
        }
    }

    // Delete finished requests
    while ( mFinishedRequests.size() > 0 )
    {
        Request* request = ( *mFinishedRequests.top() );
        mRequests.erase( mFinishedRequests.top() );
        mFinishedRequests.pop();
        delete request;
    }

    // TODO: Run 'later' jobs
}

void RequestManager::removeAll()
{
    for ( std::list<Request*>::iterator i = mRequests.begin(); i != mRequests.end(); )
    {
        delete ( *i );
        mRequests.erase( i++ );
    }
}

//------------------------------------------------------------------------------
}
}
