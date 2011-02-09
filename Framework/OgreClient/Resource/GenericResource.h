/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.

Diversia is free software: you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation, either version 3 of the License, or (at your option) any later 
version.

Diversia is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with 
Diversia. If not, see <http://www.gnu.org/licenses/>.

You may contact the author of Diversia by e-mail at: equabyte@sonologic.nl
-----------------------------------------------------------------------------
**/

#ifndef DIVERSIA_OGRECLIENT_GENERICRESOURCE_H
#define DIVERSIA_OGRECLIENT_GENERICRESOURCE_H

#include "OgreClient/Platform/Prerequisites.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API GenericResource : public Ogre::Resource
{
public:
    /**
    Constructor. 
    **/
    GenericResource( Ogre::ResourceManager* pCreator, const std::string& rName, 
        Ogre::ResourceHandle handle, const Ogre::String& rGroup, bool isManual = false, 
        Ogre::ManualResourceLoader* pLoader = 0 );
    /**
    Destructor. 
    **/
    virtual ~GenericResource();
    /**
    Gets the resource data stream pointer.
    **/
    inline Ogre::DataStreamPtr getCache() const { return mCache; }
    /**
    Gets a copy of the cache as a char array. Caller must delete[] the return value!
    **/
    char* getCacheCopy() const;
    /**
    Gets the holder data as a boost::any.
    **/
    inline const boost::any& getHolderDataAny() { return mHolderData; }
    /**
    Gets the holder data using boost any cast.

    @throws boost::bad_any_cast If cast fails.
    **/
    template <class T> inline T getHolderData() { return boost::any_cast<T>( mHolderData ); }
    /**
    Sets a holder data. 
    **/
    template <class T> inline void setHolderData( T t ) { mHolderData = t; }
    /**
    Query if this object has holder data. 
    **/
    inline bool hasHolderData() { return !mHolderData.empty(); }
    /**
    Sets a holder data type. 
    **/
    inline void setHolderDataType( const String& rType ) { mHolderDataType = rType; }
    /**
    Gets the holder data type. 
    **/
    inline const String& getHolderDataType() const { return mHolderDataType; }

protected:
    /**
    Implementation of resource preparing.
    **/
    void prepareImpl();
    /**
    Implementation of resource unpreparing.
    **/
    void unprepareImpl();
    /**
    Implementation of resource loading. 
    **/
    void loadImpl();
    /**
    Implementation of resource unloading.
    **/
    void unloadImpl();
    /**
    Gets the size of the data.
    **/
    inline size_t calculateSize() const { return mSize; }

private:
    size_t              mSize;
    Ogre::DataStreamPtr mCache;
    boost::any          mHolderData;
    String              mHolderDataType;

};

class GenericResourcePtr : public Ogre::SharedPtr<GenericResource>
{
public:
    GenericResourcePtr() : SharedPtr<GenericResource>() {}
    explicit GenericResourcePtr( GenericResource* rep ) : SharedPtr<GenericResource>( rep ) {}
    GenericResourcePtr( const GenericResourcePtr& r ) : SharedPtr<GenericResource>( r ) {}
    GenericResourcePtr( const Ogre::ResourcePtr& r ) : SharedPtr<GenericResource>()
    {
        // lock & copy other mutex pointer
        OGRE_MUTEX_CONDITIONAL( r.OGRE_AUTO_MUTEX_NAME )
        {
            OGRE_LOCK_MUTEX( *r.OGRE_AUTO_MUTEX_NAME )
                OGRE_COPY_AUTO_SHARED_MUTEX( r.OGRE_AUTO_MUTEX_NAME )
                pRep = static_cast<GenericResource*>( r.getPointer() );
            pUseCount = r.useCountPointer();
            if ( pUseCount )
            {
                ++( *pUseCount );
            }
        }
    }

    /// Operator used to convert a ResourcePtr to a ObjectTemplatePtr
    GenericResourcePtr& operator=( const Ogre::ResourcePtr& r )
    {
        if ( pRep == static_cast<GenericResource*>( r.getPointer() ) )
            return *this;
        release();
        // lock & copy other mutex pointer
        OGRE_MUTEX_CONDITIONAL( r.OGRE_AUTO_MUTEX_NAME )
        {
            OGRE_LOCK_MUTEX( *r.OGRE_AUTO_MUTEX_NAME )
                OGRE_COPY_AUTO_SHARED_MUTEX( r.OGRE_AUTO_MUTEX_NAME )
                pRep = static_cast<GenericResource*>( r.getPointer() );
            pUseCount = r.useCountPointer();
            if ( pUseCount )
            {
                ++( *pUseCount );
            }
        }
        else
        {
            // RHS must be a null pointer
            assert( r.isNull() && "RHS must be null if it has no mutex!" );
            setNull();
        }
        return *this;
    }
};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

#endif // DIVERSIA_OGRECLIENT_GENERICRESOURCE_H