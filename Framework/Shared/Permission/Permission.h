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

#ifndef DIVERSIA_SHARED_PERMISSIONBASE_H
#define DIVERSIA_SHARED_PERMISSIONBASE_H

#include "Shared/Platform/Prerequisites.h" 

#include "Shared/Communication/BitStream.h"
#include <boost/timer.hpp>

namespace Diversia
{
//------------------------------------------------------------------------------

class DIVERSIA_SHARED_API Permission
{
public:
    /**
    Values that represent the result of a permission check.
    **/
    enum ResultType
    {
        Permission_Denied = 0,                      ///< Denied.
        Permission_TooManyItems = 1,	            ///< Denied, there are too many items. 
        Permission_TooManyItemsPerTimeframe = 2,	///< Denied, too many actions at the same time.
        Permission_BoundsDenied = 3,	            ///< Denied, value was out of bounds. 
        Permission_BoundsAdjusted = 4,	            ///< Allowed, but value was adjusted to bounds. 
        Permission_Allowed = 5	                    ///< Allowed. 
    };

    /**
    Default constructor. This is used for receiving permissions from RakNet bitstreams only.
    **/
    Permission():
        mName( "" ), 
        mAllowed( false ), 
        mMaxItemsAllowed( MAXUINT ),
        mCurrentItems( 0 ), 
        mMaxItemsPerTime( MAXUINT ), 
        mCurrentItemsPerTime( 0 ),
        mTimeSeconds( 9999999999999999999 ), 
        mAdjustBounds( false ) {}
    /**
    Constructor with lower and upper bounds.
    
    @param  rName               The name of the permission.
    @param  allowed             True to allow the action, false to deny.
    @param  maxItemsAllowed     The maximum items allowed. 
    @param  maxItemsPerTime     The maximum items allowed per given time frame. 
    @param  timeSeconds         The time frame in seconds.
    @param  rLowerBounds        The lower bounds. 
    @param  rUpperBounds        The upper bounds. 
    @param  adjustBounds        True to adjust bounds, false to deny when value is out of bounds. 
    **/
    Permission( const String& rName, bool allowed, unsigned int maxItemsAllowed, 
        unsigned int maxItemsPerTime, double timeSeconds, const camp::Value& rLowerBounds, 
        const camp::Value& rUpperBounds, bool adjustBounds ):
        mName( rName ), 
        mAllowed( allowed ), 
        mMaxItemsAllowed( maxItemsAllowed ),
        mCurrentItems( 0 ), 
        mMaxItemsPerTime( maxItemsPerTime ), 
        mCurrentItemsPerTime( 0 ),
        mTimeSeconds( timeSeconds ), 
        mLowerBounds( rLowerBounds ), 
        mUpperBounds( rUpperBounds ), 
        mAdjustBounds( adjustBounds ) {}
    /**
    Constructor.
    
    @param  rName               The name of the permission.
    @param  allowed             True to allow the action, false to deny.
    @param  maxItemsAllowed     The maximum items allowed. 
    @param  maxItemsPerTime     The maximum items allowed per given time frame. 
    @param  timeSeconds         The time frame in seconds.
    **/
    Permission( const String& rName, bool allowed, unsigned int maxItemsAllowed = MAXUINT, 
        unsigned int maxItemsPerTime = MAXUINT, double timeSeconds = 9999999999999999999 ):
        mName( rName ),
        mAllowed( allowed ),
        mMaxItemsAllowed( maxItemsAllowed ),
        mCurrentItems( 0 ), 
        mMaxItemsPerTime( maxItemsPerTime ), 
        mCurrentItemsPerTime( 0 ),
        mTimeSeconds( timeSeconds ),
        mAdjustBounds( false ) {}
    /**
    Copy constructor. 
    **/
    Permission( const Permission& rPerm ):
        mName( rPerm.mName ), 
        mAllowed( rPerm.mAllowed ), 
        mMaxItemsAllowed( rPerm.mMaxItemsAllowed ),
        mCurrentItems( 0 ), 
        mMaxItemsPerTime( rPerm.mMaxItemsPerTime ), 
        mCurrentItemsPerTime( 0 ),
        mTimeSeconds( rPerm.mTimeSeconds ), 
        mLowerBounds( rPerm.mLowerBounds ),
        mUpperBounds( rPerm.mUpperBounds ),
        mAdjustBounds( rPerm.mAdjustBounds ) {}
    /**
    Destructor. 
    **/
    ~Permission() {}

    /**
    Check permission.
    
    @return Result of the permission check.
    **/
    Permission::ResultType checkPermission() 
    {
        // Allowed check.
        if( !mAllowed ) return Permission_Denied;

        // Maximum items allowed check.
        if( mCurrentItems >= mMaxItemsAllowed ) return Permission_TooManyItems;

        // Maximum items per time frame check.
        if( mTimer.elapsed() >= mTimeSeconds )
        {
            mCurrentItemsPerTime = 0;
            mTimer.restart();
        }
        if( mCurrentItemsPerTime >= mMaxItemsPerTime ) return Permission_TooManyItemsPerTimeframe;

        return Permission_Allowed;
    }
    /**
    Check permission. Only returns if the action is allowed or not.
    
    @return True if the action is allowed, false if not.
    **/
    inline bool checkPermissionAllowed() 
    { 
        return allowed( checkPermission() ); 
    }
    /**
    Check permission. Throws an exception if permission is denied.

    @param  rDesc   Exception description.
    @param  rSrc    Exception source.
    **/
    inline void checkPermissionThrows( const String& rDesc, const String& rSrc ) 
    { 
        exception( checkPermission(), rDesc, rSrc ); 
    }
    /**
    Check permission. Throws an exception if permission is denied.

    @param  rSrc    Exception source.
    **/
    inline void checkPermissionThrows( const String& rSrc ) 
    { 
        Permission::ResultType resultType = checkPermission();
        exception( resultType, Permission::toString( resultType ), rSrc ); 
    }
    /**
    Check permission and adjusts value if necessary.
    
    @param [in,out] rValue  The value to check permission for.
    
    @return Result of the permission check.
    **/
    Permission::ResultType checkPermission( camp::Value& rValue ) 
    {
        Permission::ResultType resultType = checkPermission();
        
        try
        {
            BindingType bindingType = getBindingType( rValue );

            if( resultType == Permission_Allowed )
            {
                // Check lower bounds.
                if( mLowerBounds != camp::Value::nothing )
                {
                    switch( bindingType )
                    {
                        case BindingType_Vector3:
                        {
                            Vector3 bounds = mLowerBounds.to<Vector3>();
                            Vector3 value = rValue.to<Vector3>();
                            bool changed = false;

                            // Check each variable separately.
                            if( value.x < bounds.x )
                            {
                                if( mAdjustBounds ) 
                                {
                                    value.x = bounds.x;
                                    changed = true;
                                }
                                else return Permission_BoundsDenied;
                            }
                            if( value.y < bounds.y )
                            {
                                if( mAdjustBounds ) 
                                {
                                    value.y = bounds.y;
                                    changed = true;
                                }
                                else return Permission_BoundsDenied;
                            }
                            if( value.z < bounds.z )
                            {
                                if( mAdjustBounds ) 
                                {
                                    value.z = bounds.z;
                                    changed = true;
                                }
                                else return Permission_BoundsDenied;
                            }

                            // Set changed value to given value reference.
                            if( changed )
                            {
                                rValue = camp::UserObject::copy( value );
                                return Permission_BoundsAdjusted;
                            }

                            break;
                        }
                        default:
                        {
                            if( rValue < mLowerBounds )
                            {
                                if( mAdjustBounds ) 
                                {
                                    rValue = mLowerBounds;
                                    return Permission_BoundsAdjusted;
                                }
                                else return Permission_BoundsDenied;
                            }
                            break;
                        }
                    }
                }

                // Check upper bounds.
                if( mUpperBounds != camp::Value::nothing )
                {
                    switch( bindingType )
                    {
                        case BindingType_Vector3:
                        {
                            Vector3 bounds = mUpperBounds.to<Vector3>();
                            Vector3 value = rValue.to<Vector3>();
                            bool changed = false;

                            // Check each variable separately.
                            if( value.x > bounds.x )
                            {
                                if( mAdjustBounds ) 
                                {
                                    value.x = bounds.x;
                                    changed = true;
                                }
                                else return Permission_BoundsDenied;
                            }
                            if( value.y > bounds.y )
                            {
                                if( mAdjustBounds ) 
                                {
                                    value.y = bounds.y;
                                    changed = true;
                                }
                                else return Permission_BoundsDenied;
                            }
                            if( value.z > bounds.z )
                            {
                                if( mAdjustBounds ) 
                                {
                                    value.z = bounds.z;
                                    changed = true;
                                }
                                else return Permission_BoundsDenied;
                            }

                            // Set changed value to given value reference.
                            if( changed )
                            {
                                rValue = camp::UserObject::copy( value );
                                return Permission_BoundsAdjusted;
                            }

                            break;
                        }
                        default:
                        {
                            if( rValue > mUpperBounds )
                            {
                                if( mAdjustBounds ) 
                                {
                                    rValue = mUpperBounds;
                                    return Permission_BoundsAdjusted;
                                }
                                else return Permission_BoundsDenied;
                            }
                            break;
                        }
                    }
                }

                return Permission_Allowed;
            }
            else return resultType;
        }
        catch ( camp::Error e )
        {
        	// Could not cast lower or upper bounds to given type, deny permission.
            return Permission_Denied;
        }
    }
    /**
    Check permission and adjusts value of necessary. Only returns if the action is allowed or not.
    
    @param [in,out] rValue  The value to check permission for.
    
    @return True if the action is allowed, false if not.
    **/
    inline bool checkPermissionAllowed( camp::Value& rValue ) 
    { 
        return allowed( checkPermission( rValue ) ); 
    }
    /**
    Check permission and adjusts value of necessary. Throws an exception if permission is denied.

    @param [in,out] rValue  The value to check permission for.
    @param          rDesc   Exception description.
    @param          rSrc    Exception source.
    **/
    inline void checkPermissionThrows( camp::Value& rValue, const String& rDesc, 
        const String& rSrc ) 
    { 
        exception( checkPermission( rValue ), rDesc, rSrc ); 
    }
    /**
    Check permission and adjusts value of necessary. Throws an exception if permission is denied.

    @param [in,out] rValue  The value to check permission for.
    @param          rSrc    Exception source.
    **/
    inline void checkPermissionThrows( camp::Value& rValue, const String& rSrc ) 
    { 
        Permission::ResultType resultType = checkPermission( rValue );
        exception( resultType, Permission::toString( resultType ), rSrc ); 
    }
    /**
    Checks if the result of a permission check allows the action or denies it.
    
    @param  resultType  The result of a permission check.
    
    @return True if the action is allowed, false if not.
    **/
    static bool allowed( Permission::ResultType resultType )
    {
        switch( resultType )
        {
            case Permission_Denied: return false;
            case Permission_TooManyItems: return false;
            case Permission_TooManyItemsPerTimeframe: return false;
            case Permission_BoundsDenied: return false;
            case Permission_BoundsAdjusted: return true;
            case Permission_Allowed: return true;
        }
        
        return false;
    }
    void exception( Permission::ResultType resultType, const String& rDesc,
        const String& rSrc )
    {
        if( !allowed( resultType ) )
        {
            std::stringstream ss; ss << mName << ": " << rDesc;

            switch( resultType )
            {
                case Permission_Denied:
                    DIVERSIA_EXCEPT( Exception::ERR_PERMISSION_DENIED, ss.str(), rSrc );
                case Permission_TooManyItems: 
                    DIVERSIA_EXCEPT( Exception::ERR_PERMISSION_TOOMANYITEMS, ss.str(), rSrc );
                case Permission_TooManyItemsPerTimeframe: 
                    DIVERSIA_EXCEPT( Exception::ERR_PERMISSION_TOOMANYITEMSPERTIMEFRAME, ss.str(), rSrc );
                case Permission_BoundsDenied: 
                    DIVERSIA_EXCEPT( Exception::ERR_PERMISSION_BOUNDSDENIED, ss.str(), rSrc );
            }
        }
    }
    String toString( Permission::ResultType resultType )
    {
        switch( resultType )
        {
            case Permission_Denied: 
                return "Permission denied.";
            case Permission_TooManyItems: 
                return "Permission denied, too many items.";
            case Permission_TooManyItemsPerTimeframe:
            {
                std::stringstream ss;
                ss << "Permission denied, too many actions at the same time. Wait " << 
                    mTimeSeconds - mTimer.elapsed() << " seconds to try again.";
                return ss.str();
            }
            case Permission_BoundsDenied: 
                return "Permission denied, value out of bounds.";
            case Permission_BoundsAdjusted: 
                return "Allowed, but value was adjusted to bounds.";
            case Permission_Allowed: 
                return "Allowed.";
        }

        return "Permission denied.";
    }

    /**
    Adds an item to the current number of items.
    
    @return The current number of items.
    **/
    inline unsigned int addItem() 
    {
        DivAssert( mCurrentItems < mMaxItemsAllowed, "More items than allowed items" );
        mCurrentItemsPerTime++;
        //SLOGD << "++ Permission " << mName << " items: " << mCurrentItems + 1;
        return mCurrentItems++;
    }
    /**
    Removes an item from the current number of items.
    
    @return The current number of items.
    **/
    inline unsigned int removeItem( bool removeFromItemsPerTime = false )
    {
        DivAssert( mCurrentItems > 0, "Less than 0 items" );
        if( removeFromItemsPerTime && mCurrentItemsPerTime > 0 )
            mCurrentItemsPerTime--;
        //SLOGD << "-- Permission " << mName << " items: " << mCurrentItems - 1;
        return mCurrentItems--;
    }
    /**
    Resets the number of items to 0.
    **/
    inline void resetItems() { mCurrentItems = 0; mCurrentItemsPerTime = 0; }

    inline String getName() const { return mName; }
    inline bool getAllowed() const { return mAllowed; }
    inline void setAllowed( bool allowed ) { mAllowed = allowed; }
    inline unsigned int getMaxItemsAllowed() const { return mMaxItemsAllowed; }
    inline void setMaxItemsAllowed( unsigned int max ) { mMaxItemsAllowed = max; }
    inline unsigned int getMaxItemsPerTime() const { return mMaxItemsPerTime; }
    inline void setMaxItemsPerTime( unsigned int max ) { mMaxItemsPerTime = max; }
    inline double getTimeSeconds() const { return mTimeSeconds; }
    inline void setTimeSeconds( double timeSeconds ) { mTimeSeconds = timeSeconds; }
    template <typename T> inline const T& getLowerBounds() const
    {
        return mLowerBounds.to<T>();
    }
    template <typename T> inline void setLowerBounds( const T& rLowerBounds )
    {
        mLowerBounds = camp::UserObject::copy( rLowerBounds );
    }
    inline void removeLowerBounds() { mLowerBounds = camp::Value::nothing; }
    template <typename T> inline const T& getUpperBounds() const
    {
        return mUpperBounds.to<T>();
    }
    template <typename T> inline void setUpperBounds( const T& rUpperBounds )
    {
        mUpperBounds = camp::UserObject::copy( rUpperBounds );
    }
    inline void removeUpperBounds() { mUpperBounds = camp::Value::nothing; }
    inline bool getAdjustBounds() const { return mAdjustBounds; }
    inline void setAdjustBounds( bool adjustBounds ) { mAdjustBounds = adjustBounds; }

    /**
    Read permissions from a RakNet bitstream.
    **/
    friend RakNet::BitStream& operator>>( RakNet::BitStream& in, Permission& out )
    {
        in >> out.mName;
        in >> out.mAllowed;
        in >> out.mMaxItemsAllowed;
        in >> out.mMaxItemsPerTime;
        in >> out.mTimeSeconds;

        bool lowerBounds; in >> lowerBounds;
        if( lowerBounds ) 
            in >> out.mLowerBounds;

        bool upperBounds; in >> upperBounds;
        if( upperBounds ) 
            in >> out.mUpperBounds;

        in >> out.mAdjustBounds;

        return in;
    }
    /**
    Write permissions to a RakNet bitstream.
    **/
    friend RakNet::BitStream& operator<<( RakNet::BitStream& out, Permission& in )
    {
        out << in.mName;
        out << in.mAllowed;
        out << in.mMaxItemsAllowed;
        out << in.mMaxItemsPerTime;
        out << in.mTimeSeconds;

        if( in.mLowerBounds != camp::Value::nothing )
        {
            out.Write<bool>( true );
            out << in.mLowerBounds;
        }
        else out.Write<bool>( false );

        if( in.mUpperBounds != camp::Value::nothing ) 
        {
            out.Write<bool>( true );
            out << in.mUpperBounds;
        }
        else out.Write<bool>( false );

        out << in.mAdjustBounds;

        return out;
    }

private:
    friend class Shared::Bindings::CampBindings;    ///< Allow private access for camp bindings.

    String          mName;

    bool            mAllowed;
    unsigned int    mMaxItemsAllowed;
    unsigned int    mCurrentItems;
    unsigned int    mMaxItemsPerTime;
    unsigned int    mCurrentItemsPerTime;
    double          mTimeSeconds;
    boost::timer    mTimer;

    camp::Value     mLowerBounds;
    camp::Value     mUpperBounds;
    bool            mAdjustBounds;

};

//------------------------------------------------------------------------------
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Permission, 
    &Diversia::Shared::Bindings::CampBindings::bindPermission );

#endif // DIVERSIA_SHARED_PERMISSIONBASE_H