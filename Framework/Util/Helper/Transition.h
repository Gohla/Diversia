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

#ifndef DIVERSIA_UTIL_TRANSITION_H
#define DIVERSIA_UTIL_TRANSITION_H

#include "Util/Platform/Prerequisites.h"

namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------

template<typename StoreType> 
struct TransitionZeroHelperImpl
{
    static StoreType mZero;
};

/**
Transion2 helper class to define what the default zero value is for a data type.

@tparam DataType   The type to define the default zero value for.
**/
template<typename DataType, 
    typename StoreType = boost::remove_const< boost::remove_reference<DataType>::type >::type> 
struct TransitionZeroHelper : 
    public TransitionZeroHelperImpl<StoreType>
{};

// Static initializations
template<typename StoreType> 
StoreType TransitionZeroHelperImpl<StoreType>::mZero = StoreType();

// Default zero values
Colour TransitionZeroHelper<Colour>::mZero = Colour::ZERO;
Vector2 TransitionZeroHelper<Vector2>::mZero = Vector2::ZERO;
Vector3 TransitionZeroHelper<Vector3>::mZero = Vector3::ZERO;
Vector4 TransitionZeroHelper<Vector4>::mZero = Vector4::ZERO;
Quaternion TransitionZeroHelper<Quaternion>::mZero = Quaternion::ZERO;
float TransitionZeroHelper<float>::mZero = 0.0;
double TransitionZeroHelper<double>::mZero = 0.0;
int TransitionZeroHelper<int>::mZero = 0;
unsigned int TransitionZeroHelper<unsigned int>::mZero = 0;
short TransitionZeroHelper<short>::mZero = 0;
unsigned short TransitionZeroHelper<unsigned short>::mZero = 0;
char TransitionZeroHelper<char>::mZero = 0;
unsigned char TransitionZeroHelper<unsigned char>::mZero = 0;

//------------------------------------------------------------------------------

typedef std::map< unsigned int, std::vector<sigc::connection> > TransitionBaseConnections;

/**
Transition base class, use this to set the update signal and stop all transitions for given class
and instance.

@tparam Class       The class to manage transitions for.
@tparam OwnerClass  The class that owns the instance to manage transitions for, the instance number
                    function will be called on objects of this class.
**/
template<class Class, class OwnerClass>
class TransitionBase
{
public:
    typedef unsigned int (OwnerClass::*InstanceNumberFunction)(void) const;

    /**
    Sets the frame update signal. 
    **/
    inline static void setUpdateSignal( sigc::signal<void, Real>& rUpdateSignal )
    {
        mUpdateSignal = &rUpdateSignal;
    }
    /**
    Sets the function where the instance number can be gotten from.
    **/
    inline static void setInstanceNumberFunction( InstanceNumberFunction pFunction ) 
    {
        mInstanceNumberFunction = pFunction;
    }
    /**
    Stops all transitions.
    **/
    static void stop() 
    {
        for( TransitionBaseConnections::iterator i = mBaseUpdateConnections.begin(); 
            i != mBaseUpdateConnections.end(); ++i )
        {
            for( std::vector<sigc::connection>::iterator j = i->second.begin();
                j != i->second.end(); ++j )
            {
                (*j).block( true );
            }
        }
    }
    /**
    Stops all transitions for given instance number.
    
    @param  instanceNumber  The instance number. 
    **/
    static void stop( unsigned int instanceNumber )
    {
        if( !mBaseUpdateConnections.count( instanceNumber ) )
            return;

        std::vector<sigc::connection>& connections = mBaseUpdateConnections[instanceNumber]; 

        for( std::vector<sigc::connection>::iterator i = connections.begin();
            i != connections.end(); ++i )
        {
            (*i).block( true );
        }
    }

protected:
    static sigc::signal<void, Real>*    mUpdateSignal;
    static TransitionBaseConnections    mBaseUpdateConnections;
    static InstanceNumberFunction       mInstanceNumberFunction;

};

// Static initializations
template<class Class, class OwnerClass> 
sigc::signal<void, Real>* TransitionBase<Class, OwnerClass>::mUpdateSignal = 0;

template<class Class, class OwnerClass> 
TransitionBaseConnections TransitionBase<Class, OwnerClass>::mBaseUpdateConnections = 
    TransitionBaseConnections();

template<class Class, class OwnerClass> 
typename TransitionBase<Class, OwnerClass>::InstanceNumberFunction TransitionBase<Class, 
    OwnerClass>::mInstanceNumberFunction = 0;

//------------------------------------------------------------------------------

/**
Transition class for doing smooth transitions of variables automatically.

@tparam DataType    The data type of the variable to transition, this is the parameter that the
                    set function (MemFun) accepts as a parameter.
@tparam Class       The class where the set function (MemFun) is in.
@tparam OwnerClass  The class that owns the instance to manage transitions for, the instance number
                    function will be called on objects of this class.
@tparam MemFun      The setter function that sets the variable.
@tparam StoreType   The type to store an instance of a DataType, defaults to the DataType without
                    const and reference.
**/
template<typename DataType, class Class, class OwnerClass, void (Class::*MemFun)(DataType), 
    typename StoreType = boost::remove_const<boost::remove_reference<DataType>::type>::type> 
class Transition : public TransitionBase<Class, OwnerClass>
{
public:
    /**
    Starts a transition. 
    
    @param  begin               The initial value.
    @param  end                 The target value.
    @param  timeScale           Scales the duration of the change, a scale of 1.0 results in a
                                duration of 1 second, a scale of 0.5 in 2 seconds, etc.
    @param [in,out] pInstance   The instance to call the setter function (MemFun) on.
    **/
    static void setTransition( StoreType begin, StoreType end, Real timeScale, Class* pInstance,
        OwnerClass* pOwnerInstance )
    {
        // Connect to update signal.
        unsigned int instanceNumber = 0;
        if( pOwnerInstance && mInstanceNumberFunction )
        {
            instanceNumber = (pOwnerInstance->*mInstanceNumberFunction)();
        }

        connect( instanceNumber );        

        // Set up parameters
        mBegin[instanceNumber] = begin;
        mEnd[instanceNumber] = end;
        mCurrent[instanceNumber] = begin;
        mTimeScale[instanceNumber] = timeScale;
        mInstance[instanceNumber] = pInstance;

        // Stop blocking updates so the transition starts.
        mUpdateConnections[instanceNumber].block( false );

        //ULOGD << "Transition start: " << instanceNumber << ", " << begin << " <-> " << end << ", " << timeScale;
    }
    /**
    Stops the transition for given instance number immediately.

    @param  instanceNumber  The number of the instance, defaults to 0.
    **/
    inline static void stopInstance( unsigned int instanceNumber ) 
    {
        mUpdateConnections[instanceNumber].block( true );
    }
    /**
    Stop the transition for all instances immediately.
    **/
    inline static void stopAllInstances()
    {
        for( std::map<unsigned int, sigc::connection>::iterator i = mUpdateConnections.begin();
            i != mUpdateConnections.end(); ++i )
        {
            i->second.block( true );
        }
    }

private:
    Transition() {}
    ~Transition() {}

    static void connect( unsigned int instanceNumber )
    {
        DivAssert( mUpdateSignal, "Update signal was not set." );

        if( !mUpdateConnections.count( instanceNumber ) )
        {
            // Connect a new connection to the update signal and bind instance number.
            sigc::connection connection = mUpdateSignal->connect( 
                sigc::bind( sigc::ptr_fun( &Transition<DataType, Class, OwnerClass, MemFun, 
                StoreType>::update ), instanceNumber ) );

            mUpdateConnections.insert( std::make_pair( instanceNumber, connection ) );
            mBaseUpdateConnections[instanceNumber].push_back( connection );

            // Block updates until the transition starts.
            connection.block( true );
        }
    }
    static void update( Real timeSinceLastFrame, unsigned int instanceNumber )
    {
        StoreType diff = mEnd[instanceNumber] - mBegin[instanceNumber];

        mCurrent[instanceNumber] += diff * timeSinceLastFrame * mTimeScale[instanceNumber];
        if( ( diff >= TransitionZeroHelper<DataType>::mZero && mCurrent[instanceNumber] >= mEnd[instanceNumber] ) || 
            ( diff <= TransitionZeroHelper<DataType>::mZero && mCurrent[instanceNumber] <= mEnd[instanceNumber] ) )
            mCurrent[instanceNumber] = mEnd[instanceNumber];

        (mInstance[instanceNumber]->*MemFun)( mCurrent[instanceNumber] );

        /*ULOGD << "Transition update: " << instanceNumber << ", " << mBegin[instanceNumber] << 
            " < " << mCurrent[instanceNumber] << " > " << mEnd[instanceNumber] << ", " << 
            mTimeScale[instanceNumber];*/

        if( mCurrent[instanceNumber] == mEnd[instanceNumber] )
             mUpdateConnections[instanceNumber].block( true );
    }

    static std::map<unsigned int, StoreType>        mBegin;
    static std::map<unsigned int, StoreType>        mEnd;
    static std::map<unsigned int, StoreType>        mCurrent;
    static std::map<unsigned int, Real>             mTimeScale;
    static std::map<unsigned int, Class*>           mInstance;
    static std::map<unsigned int, sigc::connection> mUpdateConnections;

};

// Static initializations
template<typename DataType, class Class, class OwnerClass, void (Class::*MemFun)(const DataType), 
    typename StoreType> 
std::map<unsigned int, StoreType> Transition<DataType, Class, OwnerClass, MemFun, 
    StoreType>::mBegin = std::map<unsigned int, StoreType>();

template<typename DataType, class Class, class OwnerClass, void (Class::*MemFun)(const DataType),
    typename StoreType> 
std::map<unsigned int, StoreType> Transition<DataType, Class, OwnerClass, MemFun, 
    StoreType>::mEnd = std::map<unsigned int, StoreType>();

template<typename DataType, class Class, class OwnerClass, void (Class::*MemFun)(const DataType), 
    typename StoreType>
std::map<unsigned int, StoreType> Transition<DataType, Class, OwnerClass, MemFun, 
    StoreType>::mCurrent = std::map<unsigned int, StoreType>();

template<typename DataType, class Class, class OwnerClass, void (Class::*MemFun)(const DataType), 
    typename StoreType> 
std::map<unsigned int, Real> Transition<DataType, Class, OwnerClass, MemFun, 
    StoreType>::mTimeScale = std::map<unsigned int, Real>();

template<typename DataType, class Class, class OwnerClass, void (Class::*MemFun)(const DataType), 
    typename StoreType> 
std::map<unsigned int, Class*> Transition<DataType, Class, OwnerClass, MemFun, 
    StoreType>::mInstance = std::map<unsigned int, Class*>();

template<typename DataType, class Class, class OwnerClass, void (Class::*MemFun)(const DataType), 
    typename StoreType> 
std::map<unsigned int, sigc::connection> Transition<DataType, Class, OwnerClass, MemFun, 
    StoreType>::mUpdateConnections = std::map<unsigned int, sigc::connection>();

//------------------------------------------------------------------------------
} // Namespace Util
} // Namespace Diversia

#endif // DIVERSIA_UTIL_TRANSITION_H