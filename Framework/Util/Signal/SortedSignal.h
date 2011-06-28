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

#ifndef DIVERSIA_UTIL_SORTEDSIGNAL_H
#define DIVERSIA_UTIL_SORTEDSIGNAL_H

namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------

/**
A sorted version of the sigc::signal class. Uses priority to sort from low priority # to high
priority #. Use the disconnect function to disconnect from the signal because it's not possible to
get a sigc::connection object.
**/
template <typename Signal> class DIVERSIA_UTIL_API SortedSignal
{
public:
    typedef signal::slot_type Slot;
    typedef std::map<int,Slot> SortedSlots;

    /**
    Connects a slot to the signal.

    @param  rSlot       The slot to connect.
    @param  priority    The priority of the slot.
    **/
    void connect( const Slot& rSlot, int priority )
    {
        if( mSortedSlots.find( priority ) == mSortedSlots.end() )
        {
            mSortedSlots.insert( std::make_pair( priority, rSlot ) );
            sort();
        }
        else
        {
            DIVERSIA_EXCEPT( Exception::ERR_DUPLICATE_ITEM, "Slot already exists for that priority.",
                "SortedSignal::connect" );
        }
    }
    /**
    Disconnects the slot at given priority

    @param  priority    The priority of the slot.
    **/
    void disconnect( int priority )
    {
        SortedSlots::iterator i = mSortedSlots.find( priority );
        if( i != mSortedSlots.end() )
        {
            mSortedSlots.erase( i );
        }
        else
        {
            DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Slot does not exist for that priority.",
            	"SortedSignal::disconnect" );
        }
    }
    /**
    Gets the sorted signal.
    **/
    inline Signal& getSignal() { return mSignal; }

private:
    /**
    Sorts the slots by priority.
    **/
    void sort()
    {
        mSignal.clear();
        for( SortedSlots::iterator i = mSortedSlots.begin(); i != mSortedSlots.end(); ++i )
        {
            mSignal.slots().push_back( i->second );
        }
    }

    Signal      mSignal;
    SortedSlots mSortedSlots;
};

//------------------------------------------------------------------------------
} // Namespace Util
} // Namespace Diversia

#endif // DIVERSIA_UTIL_SORTEDSIGNAL_H